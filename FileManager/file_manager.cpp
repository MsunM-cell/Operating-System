#include "file_manager.h"
// #include "file_operation.hpp"

// compare each other on the basis of 2nd element of pairs
// in ascending order
bool cmp(pair<int, int> a, pair<int, int> b)
{
    return a.second < b.second;
}

/**
 * @brief convert a wstring into a string
 *
 * @param s a const wstring
 */
string WStringToString(const wstring &s)
{
    string temp(s.length(), ' ');
    copy(s.begin(), s.end(), temp.begin());
    return temp;
}

/**
 * @brief convert a string into a wstring
 *
 * @param s a const string
 */
wstring StringToWString(const string &s)
{
    wstring temp(s.length(), L' ');
    copy(s.begin(), s.end(), temp.begin());
    return temp;
}

/**
 * @brief Construct a new Block object
 *
 * @param total_space total space of the Block (byte)
 * @param track track index of the Block
 * @param sector sector index of the Block
 */
Block::Block(int total_space, int track, int sector)
{
    this->total_space = total_space;
    this->free_space = total_space;
    this->track = track;
    this->sector = sector;
    this->fp = "";
}

/**
 * @brief Set new value to free_space.
 *
 * @param free_space
 */
void Block::set_free_space(int free_space)
{
    this->free_space = free_space;
}

/**
 * @brief Set new value to fp.
 *
 * @param fp
 */
void Block::set_fp(string fp)
{
    this->fp = fp;
}

/**
 * @brief Construct a new File Manager object
 *
 * @param block_size block size (byte)
 * @param track_num the number of tracks
 * @param sector_num the number of sectors
 */
FileManager::FileManager(int block_size, int track_num, int sector_num)
{
    this->block_size = block_size;
    this->track_num = track_num;
    this->sector_num = sector_num;
    this->block_num = track_num * sector_num;

    // initialize current working directory
    this->working_path.push_back(file_separator);
    // set busy blocks
    this->busy_blocks.push_back(3);
    this->busy_blocks.push_back(6);
    this->busy_blocks.push_back(9);
    this->busy_blocks.push_back(17);

    this->init_blocks();
    this->set_busy_block();
    this->file_system_tree = this->init_file_system_tree(this->home_path);
    cout << setw(4) << this->file_system_tree << endl;
}

/**
 * @brief Initialize all disk blocks.
 *
 */
void FileManager::init_blocks()
{
    int track, sector;
    for (int i = 0; i < this->block_num; i++)
    {
        track = i / this->sector_num;
        sector = i % this->sector_num;
        Block b(this->block_size, track, sector);
        blocks.push_back(b);
    }

    // initialize the bitmap, which 1 stands for an idle block
    // and 0 a busy block
    vector<int> temp_bitmap(this->block_num, 1);
    this->bitmap = temp_bitmap;
}

/**
 * @brief Set busy blocks to 0 in bitmap.
 *
 */
void FileManager::set_busy_block()
{
    for (int i = 0; i < this->busy_blocks.size(); i++)
        this->bitmap[this->busy_blocks[i]] = 0;
}

/**
 * @brief Initialize file system tree.
 *
 * build tree recursively
 * tree stored by dictionay (JSON)
 * key: file name
 * value: a string like "type/read/write/execute" for files, a new dictionary for directories
 *
 * @param current_path absolute path of current directory
 * @return file system tree
 */
json FileManager::init_file_system_tree(string current_path)
{
    json tree;
    // convert std::string to filesystem::path
    path cur_path(current_path);
    // get file entry container to traverse directory
    directory_iterator file_list(cur_path);
    // traverse directory
    for (auto &file : file_list)
    {
        string file_path = STRING(file.path());            // absolute path
        string file_name = STRING(file.path().filename()); // file name
        // if path is a directory, set key's value a new dictionary
        // otherwise, set key's value a string like "crwx"
        if (file.status().type() == file_type::directory)
            tree[file_name] = this->init_file_system_tree(file_path);
        else
        {
            // here a file's information is stored in JSON
            // example:
            // {
            //     "name" : "file",
            //     "type" : "crwx",
            //     "size" : 128,
            //     "content" : "hello bupt"
            // }
            ifstream i(file_path);
            json file_info;
            i >> file_info;
            tree[file_name] = file_info["type"];
            string relative_path = file_path.substr(this->home_path.size());
            // check if there is enough space to fill file into disk blocks
            if (this->fill_file_into_blocks(file_info, relative_path, 0) == false)
                printf("disk storage error: no enough initial space.\n");
            // close input file stream
            i.close();
        }
    }
    return tree;
}

/**
 * @brief Allocate disk blocks to a file.
 *
 * @param file_info information about the file in JSON
 * @param file_path file path relative to home directory
 * @param method how to find the idle disk blocks
 * @return true if allocate successfully
 */
bool FileManager::fill_file_into_blocks(json file_info, string file_path, int method)
{
    int num = (int)file_info["size"] / block_size;
    int occupy = (int)file_info["size"] % block_size;
    // get first block index of allocated blocks
    int first_idle_block = this->find_idle_blocks(num + 1, method);
    if (first_idle_block == -1)
        return false;
    int free = this->block_size - occupy;
    // store file's blocks info
    // directory entry: first block index, blocks length, file size
    this->file_blocks[file_path] = {first_idle_block, num + 1, (int)file_info["size"]};
    // set bitmap and blocks info
    int count = first_idle_block;
    for (int i = 0; i < num + 1; i++)
    {
        if (i == num)
            blocks[count].set_free_space(free);
        else
            blocks[count].set_free_space(0);
        bitmap[count] = 0;
        blocks[count].set_fp(file_path);
        count += 1;
    }
    return true;
}

/**
 * @brief Find the idle blocks in a certain method.
 *
 * @param num the number of disk blocks we need
 * @param method 0(first fit), 1(best fit), 2(worst fit)
 * @return first block index of allocated blocks
 */
int FileManager::find_idle_blocks(int num, int method)
{
    // initialize target bitmap
    // for example, if we need 8 disk blocks, target will be 11111111.
    vector<int> target(num, 1);
    string target_str = this->bitmap2str(target);
    // find the idle blocks
    if (method == 0) // first fit
        return this->first_fit(target_str);
    else if (method == 1) // best fit
        return this->best_fit(target_str);
    else if (method == 2) // worst fit
        return this->worst_fit(target_str);
    return -1;
}

/**
 * @brief Convert bitmap to string.
 *
 * @param source_bitmap
 * @return string
 */
string FileManager::bitmap2str(vector<int> source_bitmap)
{
    string target_str("");
    for (int i = 0; i < source_bitmap.size(); i++)
        target_str += to_string(source_bitmap[i]);
    return target_str;
}

/**
 * @brief Use First Fit to find the idle blocks.
 *
 * @param target_str
 * @return int
 */
int FileManager::first_fit(string target_str)
{
    string bitmap_str = this->bitmap2str(this->bitmap);
    // search the target string for the first occurrence of the bitmap string
    int first_idle_block = bitmap_str.find(target_str);
    return first_idle_block;
}

/**
 * @brief Use Best Fit to find the idle blocks.
 *
 * @param target_str
 * @return int
 */
int FileManager::best_fit(string target_str)
{
    int count = 0;
    vector<pair<int, int>> free_blocks; // 1st fot index, 2nd for partition size
    // find all idle partitions
    for (int i = 0; i < this->bitmap.size(); i++)
    {
        int bit = this->bitmap[i];
        if (bit == 0)
        {
            count = 0;
            continue;
        }
        else
        {
            if (count == 0)
                free_blocks.push_back({i, 0});
            count += 1;
            free_blocks[free_blocks.size() - 1].second = count;
        }
    }
    // sort patitions on the basis of patition size
    // in ascending order
    sort(free_blocks.begin(), free_blocks.end(), cmp);
    // core of Best Fit
    for (int i = 0; i < free_blocks.size(); i++)
    {
        count = free_blocks[i].second;
        if (count >= target_str.size())
            return free_blocks[i].first;
    }
    return -1;
}

/**
 * @brief Use Worst Fit to find the idle blocks.
 *
 * @param target_str
 * @return int
 */
int FileManager::worst_fit(string target_str)
{
    int count = 0;
    vector<pair<int, int>> free_blocks; // 1st fot index, 2nd for partition size
    // find all idle partitions
    for (int i = 0; i < bitmap.size(); i++)
    {
        int bit = bitmap[i];
        if (bit == 0)
        {
            count = 0;
            continue;
        }
        else
        {
            if (count == 0)
                free_blocks.push_back({i, 0});
            count += 1;
            free_blocks[free_blocks.size() - 1].second = count;
        }
    }
    // sort patitions on the basis of patition size
    // in ascending order
    sort(free_blocks.begin(), free_blocks.end(), cmp);
    return free_blocks[free_blocks.size() - 1].first;
}

int FileManager::get_number_of_files(string directory)
{
    path cur_directory(directory);
    directory_iterator file_list(cur_directory);
    int count = 0;
    for (auto file : file_list)
        count++;
    return count;
}

/**
 * @brief print file system by tree recursively
 *
 * @param directory
 * @param layer
 */
void FileManager::print_file_system_tree(string directory, int layer)
{
    path cur_directory(directory);
    // the number of files in current directory
    int count = (int)distance(directory_iterator(cur_directory), directory_iterator{});
    int index = 0; // the index of files in current directory

    directory_iterator file_list(cur_directory);
    // start to print tree
    if (layer == 0)
        printf(".\n");
    for (auto file : file_list)
    {
        string file_path = STRING(file.path());
        string file_name = STRING(file.path().filename());
        for (int i = 0; i < layer; i++)
            printf("|   ");
        index++;
        if (index == count)
            printf("`-- ");
        else
            printf("|-- ");
        printf("%s\n", file_name.c_str());
        if (file.status().type() == file_type::directory)
            this->print_file_system_tree(file_path, layer + 1);
    }
}

/**
 * @brief return the absolute working path
 * @return string
 */
string FileManager::get_absolute_working_path()
{
    return home_path + working_path;
}

/**
 * @brief add the json node to the file_system_tree
 * 
 * @param path the path of the file
 * @return bool
 */
bool FileManager::add_json_node_to_tree(string path, json node)
{
    json* temp = &(this->file_system_tree); // the temportary pointer to the file_system_tree
    
    string relative_path = path.substr(this->home_path.size() + 1); // get the path relative to the home
    // cout << relative_path << endl;

    int index = -1; // -1 means not exists file_separator, >= 0 means exists.
    while ((index = relative_path.find('/')) != -1) {
        string temp_dir = relative_path.substr(0, index);   // get the next directory name
        relative_path = relative_path.substr(index + 1);    // get the next path of the next directory

        // if not contains the directory name, maybe there is a fault.
        if ((*temp).contains(temp_dir) == false)  return false; 
        temp = &(*temp)[temp_dir];  // 'temp' points to the next directory
    }

    if (exists(path) && is_directory(path)) {
        (*temp)[(string)node["name"]] = nlohmann::detail::value_t::null; // This is a new directory.
        // cout << setw(4) << this->file_system_tree << endl;   // debug
        return true;
    }
    else if (exists(path)) {
        (*temp)[(string)node["name"]] = node["type"];   // This is a new file.
        // cout << setw(4) << this->file_system_tree << endl;   // debug
        return true;
    }
    return false;
}


int main()
{
    FileManager fm(512, 200, 12);
    fm.print_file_system_tree(fm.home_path);
    return 0;
}