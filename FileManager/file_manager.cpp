#include "file_manager.h"
#include "file_operation.hpp"

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
 * @brief return track and sector by pair
 *
 * @return pair<int, int>
 */
pair<int, int> Block::get_location()
{
    return {this->track, this->sector};
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
    // cout << setw(4) << this->file_system_tree << endl;

    this->disk = Disk(this->block_size, this->track_num, this->sector_num);
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
    if (occupy == 0)
        num = num - 1;
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
 * @brief free blocks occupied by a file
 *
 * @param file_path path relative to home
 * @return bool
 */
bool FileManager::delete_file_from_blocks(string file_path)
{
    int start = (int)this->file_blocks[file_path][0];
    int length = (int)this->file_blocks[file_path][1];
    for (int i = start; i < start + length; i++)
    {
        this->blocks[i].set_free_space(this->block_size);
        this->blocks[i].set_fp("");
        this->bitmap[i] = 1;
    }
    file_blocks.erase(file_path);
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
    json *temp = &(this->file_system_tree); // the temporary pointer to the file_system_tree

    string relative_path = path.substr(this->home_path.size() + 1); // get the path relative to the home
    // cout << relative_path << endl;

    int index = -1; // -1 means not exists file_separator, >= 0 means exists.
    while ((index = relative_path.find(path::preferred_separator)) != -1)
    {
        string temp_dir = relative_path.substr(0, index); // get the next directory name
        relative_path = relative_path.substr(index + 1);  // get the next path of the next directory

        // if not contains the directory name, maybe there is a fault.
        if ((*temp).contains(temp_dir) == false)
            return false;
        temp = &(*temp)[temp_dir]; // 'temp' points to the next directory
    }

    if (exists(path) && is_directory(path))
    {
        (*temp)[(string)node["name"]] = nlohmann::detail::value_t::null; // This is a new directory.
        // cout << setw(4) << this->file_system_tree << endl;               // debug
        return true;
    }
    else if (exists(path))
    {
        (*temp)[(string)node["name"]] = node["type"]; // This is a new file.
        // cout << setw(4) << this->file_system_tree << endl;               // debug
        return true;
    }
    return false;
}

/**
 * @brief delete the json node from the file_system_tree
 *
 * @param path the path of the file (absolute)
 * @return bool
 */
bool FileManager::delete_json_node_from_tree(string path)
{
    json *temp = &(this->file_system_tree); // the temporary pointer to the file_system_tree

    string relative_path = path.substr(this->home_path.size() + 1); // get the path relative to the home
    // cout << relative_path << endl;

    int index = -1; // -1 means not exists file_separator, >= 0 means exists.
    while ((index = relative_path.find(path::preferred_separator)) != -1)
    {
        string temp_dir = relative_path.substr(0, index); // get the next directory name
        relative_path = relative_path.substr(index + 1);  // get the next path of the next directory

        // if not contains the directory name, maybe there is a fault.
        if ((*temp).contains(temp_dir) == false)
            return false;
        temp = &(*temp)[temp_dir]; // 'temp' points to the next directory
    }

    string file_name = relative_path;

    if (exists(path) && is_directory(path))
    {
        (*temp).erase(file_name);
        // cout << setw(4) << this->file_system_tree << endl;               // debug
        return true;
    }
    else if (exists(path))
    {
        (*temp).erase(file_name);
        // cout << setw(4) << this->file_system_tree << endl;               // debug
        return true;
    }
    return false;
}

/**
 * @brief set disk's head pointer new position
 *
 * @param head_pointer
 */
void FileManager::set_disk_head_pointer(int head_pointer)
{
    this->disk.set_head_pointer(head_pointer);
}

/**
 * @brief get dict by path
 *
 * @param file_path file path relative to home
 * @return json
 */
json FileManager::path2dict(string file_path)
{
    json temp = this->file_system_tree;
    file_path.erase(0, 1); // remove first file separator
    int index = -1;
    while ((index = file_path.find(this->file_separator)) != -1)
    {
        string temp_dir = file_path.substr(0, index);
        file_path = file_path.substr(index + 1);
        temp = temp[temp_dir];
    }
    return temp;
}

/**
 * @brief just a demo for disk seek
 *
 * @param seek_algo
 */
void FileManager::get_file_demo(string seek_algo)
{
    vector<pair<int, int>> seek_queue;
    seek_queue = {{98, 3}, {183, 5}, {37, 2}, {122, 11}, {119, 5}, {14, 0}, {124, 8}, {65, 5}, {67, 1}, {198, 5}, {105, 5}, {53, 3}};
    if (seek_algo == "FCFS")
        this->disk.FCFS(seek_queue);
    else if (seek_algo == "SSTF")
        this->disk.SSTF(seek_queue);
    else if (seek_algo == "SCAN")
        this->disk.SCAN(seek_queue);
    else if (seek_algo == "C-SCAN")
        this->disk.C_SCAN(seek_queue);
}

/**
 * @brief get file info
 *
 * @param file_path file path relative to home
 * @param mode read or write
 * @param seek_algo seek algorithm
 * @return json
 */
json FileManager::get_file(string file_path, string mode, string seek_algo)
{
    // remove last file separator
    if (file_path.back() == this->file_separator)
        file_path.pop_back();
    // split path
    int pos = file_path.find_last_of(this->file_separator);
    string upper_directory = file_path.substr(0, pos + 1); // upper directory
    string file_name = file_path.substr(pos + 1);          // file name

    // get upper directory dict
    json upper_dict = this->path2dict(upper_directory);
    cout << setw(4) << upper_dict << endl;

    if (upper_dict.contains(file_name))
    {
        // check if the file is a directory
        string path = home_path + file_path;
        if (is_directory(path))
            printf("file: cannot access '%s': not a regular file but a directory\n", file_name.c_str());
        else
        {
            // get seek queue
            int start = this->file_blocks[file_path][0];
            int length = this->file_blocks[file_path][1];
            vector<pair<int, int>> seek_queue;
            for (int i = start; i < start + length; i++)
                seek_queue.push_back(this->blocks[i].get_location());

            // seek by algorithm
            if (seek_algo == "FCFS")
                this->disk.FCFS(seek_queue);
            else if (seek_algo == "SSTF")
                this->disk.SSTF(seek_queue);

            // get file info
            ifstream i(path);
            json file_info;
            i >> file_info;
            i.close();

            return file_info;
        }
    }
    else // file not exist
        printf("file: cannot access '%s': No such file or directory\n", file_name.c_str());

    return {};
}

/**
 * @brief Construct a new Disk object
 *
 * @param block_size the size of a block
 * @param track_num the number of tracks
 * @param sector_num // the number of sectors for each track
 */
Disk::Disk(int block_size, int track_num, int sector_num)
{
    this->sector_size = block_size; // default 512 bytes
    this->track_num = track_num;    // default 200
    this->track_size = sector_num;  // default 12
    this->head_pointer = 12;        // default 12

    this->seek_speed = 0.1; // default 0.1 ms
    this->rotate_speed = 4; // default 4 ms
    // for Windows, it is Sleep function
    // for Linux and Unix, it is usleep function
    // accurate to 1 ms, so need to multiply by 10
    this->slow_ratio = 10;
    this->seek_speed = this->seek_speed * this->slow_ratio;
    this->rotate_speed = this->rotate_speed * this->slow_ratio;
}

/**
 * @brief default constructor
 *
 */
Disk::Disk()
{
}

/**
 * @brief set head pointer new position
 *
 * @param head_pointer
 */
void Disk::set_head_pointer(int head_pointer)
{
    this->head_pointer = head_pointer;
}

/**
 * @brief seek one by one in sequence
 *
 * @param seek_queue
 */
void Disk::seek_by_queue(vector<pair<int, int>> seek_queue)
{
    double seek_time = 0;  // time cost
    int seek_byte = 0;     // read-write bytes
    int seek_distance = 0; // head movement distance
    for (auto q : seek_queue)
    {
        // seek: calculate the distance the head has to travel
        int distance = abs(q.first - this->head_pointer);
        seek_distance += distance;
        // seek: simulate delay of moving head
        SLEEP(distance * this->seek_speed);
        // record time cost
        seek_time += (distance * this->seek_speed) / this->slow_ratio;
        // update head
        this->head_pointer = q.first;
        // rotate: simulate sector seeking and read-write delay
        SLEEP(this->rotate_speed);
        seek_time += this->rotate_speed / this->slow_ratio;
        // record read-write bytes
        seek_byte += this->sector_size;
    }
    printf("disk access success: time used: %.5lf ms\n", seek_time);
}

/**
 * @brief FCFS algorithm
 *
 * @param seek_queue
 */
void Disk::FCFS(vector<pair<int, int>> seek_queue)
{
    this->seek_by_queue(seek_queue);
}

/**
 * @brief SSTF
 *
 * @param seek_queue
 */
void Disk::SSTF(vector<pair<int, int>> seek_queue)
{
    vector<pair<int, int>> temp_seek_queue;
    temp_seek_queue.push_back({this->head_pointer, 0});
    // sort by seek time cost
    while (!seek_queue.empty())
    {
        int index = 0;
        int min_distance = this->track_num;
        for (int i = 0; i < seek_queue.size(); i++)
        {
            int temp_head_pointer = temp_seek_queue[temp_seek_queue.size() - 1].first;
            int distance = abs(seek_queue[i].first - temp_head_pointer);
            if (distance < min_distance)
            {
                min_distance = distance;
                index = i;
            }
        }
        temp_seek_queue.push_back(seek_queue[index]);
        seek_queue.erase(seek_queue.begin() + index);
    }
    temp_seek_queue.erase(temp_seek_queue.begin());
    seek_queue = temp_seek_queue;
    this->seek_by_queue(seek_queue);
}

/**
 * @brief SCAN
 *
 * @param seek_queue
 */
void Disk::SCAN(vector<pair<int, int>> seek_queue)
{
    vector<pair<int, int>> temp_seek_queue;
    sort(seek_queue.begin(), seek_queue.end());
    int first_location = -1;
    for (int i = 0; i < seek_queue.size(); i++)
        if (seek_queue[i].first >= this->head_pointer)
        {
            first_location = i;
            break;
        }
    temp_seek_queue.insert(temp_seek_queue.end(), seek_queue.begin() + first_location, seek_queue.end());

    if (temp_seek_queue != seek_queue)
    {
        temp_seek_queue.push_back({this->track_num - 1, -1});
        reverse(seek_queue.begin(), seek_queue.begin() + first_location);
        temp_seek_queue.insert(temp_seek_queue.end(), seek_queue.begin(), seek_queue.begin() + first_location);
        seek_queue = temp_seek_queue;
    }

    this->seek_by_queue(seek_queue);
}

/**
 * @brief C-SCAN
 *
 * @param seek_queue
 */
void Disk::C_SCAN(vector<pair<int, int>> seek_queue)
{
    vector<pair<int, int>> temp_seek_queue;
    sort(seek_queue.begin(), seek_queue.end());
    int first_location = -1;
    for (int i = 0; i < seek_queue.size(); i++)
        if (seek_queue[i].first >= this->head_pointer)
        {
            first_location = i;
            break;
        }
    temp_seek_queue.insert(temp_seek_queue.end(), seek_queue.begin() + first_location, seek_queue.end());

    if (temp_seek_queue != seek_queue)
    {
        temp_seek_queue.push_back({this->track_num - 1, -1});
        temp_seek_queue.push_back({0, -1});
        temp_seek_queue.insert(temp_seek_queue.end(), seek_queue.begin(), seek_queue.begin() + first_location);
        seek_queue = temp_seek_queue;
    }

    for (auto i : seek_queue)
        cout << i.first << " ";
    cout << endl;

    this->seek_by_queue(seek_queue);
}

int main()
{
    FileManager fm(512, 200, 12);
    // fm.print_file_system_tree(fm.home_path);
    fm.set_disk_head_pointer(110);
    fm.get_file_demo("C-SCAN");
    // fm.set_disk_head_pointer(12);
    // fm.get_file_demo("SSTF");
    // Disk d(512, 200, 12);
    // vector<pair<int, int>> seek_queue;
    // seek_queue.push_back({100, 1});
    // seek_queue.push_back({40, 1});
    // seek_queue.push_back({60, 1});
    // seek_queue.push_back({10, 1});
    // d.SSTF(seek_queue);
    return 0;
}