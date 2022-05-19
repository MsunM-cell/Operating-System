#include "file_operation.h"

/**
 * @brief construct a new FileOperation object with a FileManager pointer
 * 
 * @param fm a pointer to FileManager instance
 */
FileOperation::FileOperation(FileManager* fm)
{
    file_manager = fm;
}

/**
 * @brief create a new file
 * 
 * @param current_dir the working directory (absolute)
 * @param file_name the name of the file
 * @return bool
 */
bool FileOperation::create_file(string current_dir, string file_name)
{
    current_dir = pathConverter(current_dir);
    if (current_dir == "error") {
        puts("touch: Not such directory");
        return false;
    }
    if (current_dir.back() != (char)path::preferred_separator)  
        current_dir.push_back((char)path::preferred_separator);
    string cur_file_path = current_dir + file_name;
    // cout << cur_file_path << endl;

    if (exists(cur_file_path)) {
        printf("Existed! Please try again.\n");
        return false;
    }

    // default setting
    json new_file;
    new_file["name"] = file_name;
    new_file["type"] = "crw-";
    new_file["size"] = 8;
    new_file["content"] = "";

    string relative_path = cur_file_path.substr(file_manager->home_path.size());
    cout << relative_path << endl;
    if (file_manager->fill_file_into_blocks(new_file, relative_path, 0)) {
        ofstream outfile(cur_file_path, ios::out);
        outfile << std::setw(4) << new_file;
        outfile.close();
        // check if the file has been created and if add to the file_system_tree
        if (exists(cur_file_path) && file_manager->add_json_node_to_tree(cur_file_path, new_file)) {
            printf("Success: make file %s\n", file_name.c_str());
            return true;
        }
        printf("open file failed or add json failed !\n");
    }
    else
        printf("disk storage error: no enough space\n");

    return false;
}

/**
 * @brief delete a new file (only for file !! Not used to delete a directory!!)
 * 
 * @param current_dir the working directory (absolute)
 * @param file_name the name of the file
 * @return bool
 */
bool FileOperation::delete_file(string current_dir, string file_name)
{
    current_dir = pathConverter(current_dir);
    if (current_dir == "error") {
        puts("rm: Not such directory");
        return false;
    }
    if (current_dir.back() != path::preferred_separator)
        current_dir.push_back(path::preferred_separator);

    string cur_file_path = current_dir + file_name;
    if (!exists(cur_file_path)) {
        printf("rm: '%s' No such file\n", file_name.c_str());
        return false;
    }

    if (is_directory(cur_file_path)) {
        printf("rm: cannot remove '%s': is a directory\n", file_name.c_str());
        return false;
    }
    string relative_path = cur_file_path.substr(file_manager->home_path.size());

    if (file_manager->delete_json_node_from_tree(cur_file_path)) {
        if (file_manager->delete_file_from_blocks(relative_path) && remove(cur_file_path)) {
            printf("Success: %s deleted\n", file_name.c_str());
            return true;
        }
    }

    return false;
}

/**
 * @brief create a new directory called 'dir_name' in the current_dir called 'current_dir'
 * 
 * @param current_dir the working directory (absolute)
 * @param dir_name the name of the directory
 * @return bool
 */
bool FileOperation::create_dir(string current_dir, string dir_name)
{
    current_dir = pathConverter(current_dir);
    if (current_dir == "error") {
        puts("mkdir: Not such directory");
        return false;
    }
    if (current_dir.back() != path::preferred_separator)
        current_dir.push_back(path::preferred_separator);

    string cur_dir_path = current_dir + dir_name;
    if (exists(cur_dir_path)) {
        printf("Cannot create directory '%s': File exists\n", dir_name.c_str());
        return false;
    }

    json new_dir;
    new_dir["name"] = dir_name;
    new_dir["type"] = "drw-";
    new_dir["size"] = 8;
    new_dir["content"] = "";

    if (create_directory(cur_dir_path)) {
        if (file_manager->add_json_node_to_tree(cur_dir_path, new_dir)) {
            printf("Success: make directory %s\n", dir_name.c_str());
            return true;
        }
    }     
    
    printf("Create directory '%s' failed.\n", dir_name.c_str());
    return false;
}

/**
 * @brief delete a new directory
 * 
 * @param current_dir the working directory (absolute)
 * @param dir_name the name of the directory
 * @return bool
 */
bool FileOperation::delete_dir(string current_dir, string dir_name)
{
    current_dir = pathConverter(current_dir);
    if (current_dir == "error") {
        puts("rm: Not such directory");
        return false;
    }
    if (current_dir.back() != path::preferred_separator)
        current_dir.push_back(path::preferred_separator);

    string cur_dir_path = current_dir + dir_name;
    if (!exists(cur_dir_path)) {
        printf("rm: '%s' No such directory.\n", dir_name.c_str());
        return false;
    }
    
    for (auto &item: directory_iterator(path(cur_dir_path))) {
        string file_path = STRING(item.path());
        string file_name = STRING(item.path().filename());
        
        if (is_directory(file_path)) {
            string tmp_dir = cur_dir_path.substr(file_manager->home_path.size());
            if (!delete_dir(tmp_dir , file_name))   return false;
        }
        else {
            string tmp_dir = cur_dir_path.substr(file_manager->home_path.size());
            if (!delete_file(tmp_dir, file_name))  return false;
        }
    }
    if (file_manager->delete_json_node_from_tree(cur_dir_path)) {
        remove(cur_dir_path);
        puts("DEBUG: delete dir");
        return true;
    }
    return false;
}

/**
 * @brief traverse a directory recursively (make direct use of the public function in the FileManager)
 * 
 * @param dir the directory which needs to be traversed
 * @param layer 
 */
void FileOperation::tree(string dir, int layer) {
    file_manager->print_file_system_tree(dir, layer);
}

/**
 * @brief return the size of the file, -1 means 'a existed path', -2 means 'is a directroy', 
 *         -3 means 'file open error', else means the size of the file. 
 * @param path the absolute of the file
 * @return  int
 */
int FileOperation::file_size(string path) {
    string file_name = path.substr(path.find_last_of((char)path::preferred_separator) + 1);

    if (!exists(path)) {
        printf("'%s': Not such file.\n", file_name.c_str());
        return -1;
    }

    if (is_directory(path)) {
        printf("'%s': is a directory.\n", file_name.c_str());
        return -2;
    }

    json temp;
    ifstream input(path);
    if (!input.is_open()) {
        printf("'%s' open failed.\n", file_name.c_str());
        input.close();
        return -3;
    }
    input >> temp;
    input.close();
    
    return temp["size"];
}

/**
 * @brief modify the type of the file
 * 
 * @param file_path the absolute path of the file
 * @param mode use a decimal number for a three-bit binary, 1 means the permission is not available. 
 *             From the left to right is to read, write and execute, for example, mode '7' : '111'.
 * @return bool
 */
bool FileOperation::modify_file_type(string file_path, unsigned int mode) {
    file_path = pathConverter(file_path);
    if (!exists(file_path)) {
        printf("Cannot find the file whose type will be modified.\n");
        return false;
    }
    string file_name = file_path.substr(file_path.find_last_of((char)path::preferred_separator) + 1);

    if (is_directory(file_path)) {
        printf("'%s' is a directory.\n", file_name.c_str());
        return false;
    }

    if (!is_regular_file(file_path)) {
        printf("'%s' is not a regular file.\n", file_name.c_str());
        return false;
    } 

    json temp;
    ifstream input(file_path);
    if (!input.is_open()) {
        printf("'%s' file open failed.\n", file_name.c_str());
        input.close();
        return false;
    }
    input >> temp;
    input.close();

    string type = temp["type"];
    // cout << "debug: " << type << endl;
    type = type.substr(0, 1);
    // cout << "debug: " << type << endl;

    for (int i = 2; i >= 0; i--) {
        if ((mode >> i) & 1) {
            if (i == 2) type += "r";
            else if (i == 1) type += "w";
            else type += "x";
        }
        else {
            if (i == 2) type += "-";
            else if (i == 1) type += "-";
            else type += "-";
        }
    }
    temp["type"] = type;

    ofstream output(file_path, ios::out);
    if (!output.is_open()) {
        printf("'%s' file open failed.\n", file_name.c_str());
        output.close();
        return false;
    }
    output << setw(4) << temp;
    output.close();
    if (file_manager->delete_json_node_from_tree(file_path)) {
        puts("DEBUG: delete_json_node done!");
        if (file_manager->add_json_node_to_tree(file_path, temp)) {
            puts("DEBUG: add_json_node done!");
            printf("chmod '%s' success.\n", file_name.c_str());
            return true;
        }
    }
    
    printf("chmod: opearte the system tree failed.\n");
    return false;
}

/**
 * @brief move a file from the src_path to the dst_path
 * 
 * @param src_path the source path of a file (absolute)
 * @param dst_path the destination path of the file (absolute)
 * @return bool
 */
bool FileOperation::move_file(string src_path, string dst_path) {
    src_path = pathConverter(src_path);
    dst_path = pathConverter(dst_path);
    if (src_path == "error" || dst_path == "error") {
        printf("mv: Cannot find the source file.\n");
        return false;
    }
    string relative_src_path = src_path.substr(file_manager->home_path.size());

    if (!exists(src_path)) {
        printf("mv: Cannot find the source file.\n");
        return false;
    }

    json temp;
    ifstream input(src_path);
    if (!input.is_open()) {
        printf("mv: Cannot open the source file.\n");
        input.close();
        return false;
    }
    input >> temp;
    input.close();

    string file_name = src_path.substr(src_path.find_last_of((char)path::preferred_separator) + 1);

    if (is_directory(src_path)) {
        printf("mv: '%s' is a directory.\n", file_name.c_str());
        return false;
    }

    if (!is_regular_file(src_path)) {
        printf("mv: '%s' is not a regular file.\n", file_name.c_str());
        return false;
    }

    // if dst_path is a existed directroy, move directly.
    if (exists(dst_path) && is_directory(dst_path)) {
        if (dst_path.back() != path::preferred_separator)  dst_path += path::preferred_separator;
        if (file_manager->delete_json_node_from_tree(src_path) && file_manager->delete_file_from_blocks(relative_src_path)) {
            puts("DEBUG: delete_json_node done!");
            puts("DEBUG: delete_file_blocks done!");
            rename(src_path, dst_path + file_name);
            if (file_manager->add_json_node_to_tree(dst_path + file_name, temp) 
                    && file_manager->fill_file_into_blocks(temp, dst_path.substr(file_manager->home_path.size()), 0)) {
                puts("DEBUG: add_json_node done!");
                printf("mv: Success.\n");
                return true;
            }
            else {
                printf("mv: add json or fill blocks error.\n");
                return false;
            }
        }
        else {
            printf("mv: delete json error.\n");
            return false;
        }
    }

    // if the directory is not existed
    if (dst_path.back() == path::preferred_separator) {
        if (!exists(dst_path)) {
            printf("mv: '%s': Not a directory.\n", dst_path.c_str());
            return false;
        }
    } 

    // if src_path is the same as the dst_path, return.
    if (src_path == dst_path) {
        printf("mv: '%s' and '%s' are the same file.\n", file_name.c_str(), file_name.c_str());
        return false;
    } 

    string new_name = dst_path.substr(dst_path.find_last_of((char)path::preferred_separator) + 1);
    // cout << "debug: " << new_name << endl;

    // the dst_path turns to the dst_path's dir
    dst_path.erase(dst_path.find_last_of((char)path::preferred_separator) + 1);
    // cout << "debug: " << dst_path << endl;
    if (exists(dst_path)) {
        temp["name"] = new_name;
        ofstream output(src_path);
        if (!output.is_open()) {
            printf("mv: '%s' file open failed.\n", file_name.c_str());
            output.close();
            return false;
        }
        output << setw(4) << temp;
        output.close();

        if (file_manager->delete_json_node_from_tree(src_path) && file_manager->delete_file_from_blocks(relative_src_path)) {
            puts("DEBUG: delete_json_node done!");
            puts("DEBUG: delete_file_blocks done!");
            rename(src_path, dst_path + new_name);
            if (file_manager->add_json_node_to_tree(dst_path + new_name, temp) 
                    && file_manager->fill_file_into_blocks(temp, dst_path.substr(file_manager->home_path.size()) + new_name, 0)) {
                puts("DEBUG: add_json_node done!");
                puts("DEBUG: fill_file_blocks done!");
                printf("mv: Success.\n");
                return true;
            }
            else {
                printf("mv: add json or fill blocks error.\n");
                return false;
            }
        }
        else {
            printf("mv: delete json error.\n");
            return false;
        } 
    }
    else {
        printf("mv: '%s': Not a directory.\n", dst_path.c_str());
        return false;
    }
    return false;
}

 /**
 * @brief copy a file from the src_path to the dst_path
 * 
 * @param src_path the source path of a file
 * @param dst_path the destination path of the file
 * @return bool
 */
bool FileOperation::copy_file(string src_path, string dst_path) {
    src_path = pathConverter(src_path);
    dst_path = pathConverter(dst_path);
    if (src_path == "error" || dst_path == "error") {
        printf("cp: Cannot find the source file.\n");
        return false;
    }

    if (!exists(src_path)) {
        printf("cp: Cannot find the source file.\n");
        return false;
    }
    string file_name = src_path.substr(src_path.find_last_of((char)path::preferred_separator) + 1);

    if (is_directory(src_path)) {
        printf("cp: '%s' is a directory.\n", file_name.c_str());
        return false;
    }

    if (!is_regular_file(src_path)) {
        printf("cp: '%s' is not a regular file.\n", file_name.c_str());
        return false;
    }

    if (src_path == dst_path) {
        printf("cp: '%s' and '%s' are the same file.\n", file_name.c_str(), file_name.c_str());
        return false;
    } 
    // if dst_path is a existed directroy, move directly.
    if (exists(dst_path) && is_directory(dst_path)) {
        if (dst_path.back() != path::preferred_separator)  dst_path += path::preferred_separator;
        if (src_path == dst_path + file_name) {
            printf("cp: '%s' and '%s' are the same file.\n", file_name.c_str(), (dst_path + file_name).c_str());
            return false;
        }
        copy(src_path, dst_path + file_name);
        string relative_path = (dst_path + file_name).substr(file_manager->home_path.size());

        ifstream input(src_path);
        if (!input.is_open()) {
            printf("cp: open file failed.\n");
            input.close();
        }
        json temp;
        input >> temp;
        input.close();

        if (file_manager->add_json_node_to_tree(dst_path + file_name, temp) 
                && file_manager->fill_file_into_blocks(temp, relative_path, 0)) {
            puts("DEBUG: add_json_node done!");
            puts("DEBUG: fill_file_blocks done!");
            printf("cp: Success.\n");
            return true;
        }
        else {
            printf("cp: add json or fill blocks error.\n");
            return false;
        }
    }

    // if the directory is not existed
    if (dst_path.back() == path::preferred_separator) {
        if (!exists(dst_path)) {
            printf("cp: '%s': Not a directory.\n", dst_path.c_str());
            return false;
        }
    }  

    string new_name = dst_path.substr(dst_path.find_last_of((char)path::preferred_separator) + 1);
    // cout << "debug: " << new_name << endl;

    dst_path.erase(dst_path.find_last_of((char)path::preferred_separator) + 1);
    // cout << "debug: " << dst_path << endl;
    if (exists(dst_path)) {
        json temp;
        ifstream input(src_path);
        if (!input.is_open()) {
            printf("cp: '%s' file open failed.\n", file_name.c_str());
            input.close();
            return false;
        }
        input >> temp;
        input.close();

        temp["name"] = new_name;
        ofstream output(dst_path + new_name);
        if (!output.is_open()) {
            printf("cp: '%s' file open failed.\n", file_name.c_str());
            output.close();
            return false;
        }
        output << setw(4) << temp;
        output.close();

        if (file_manager->add_json_node_to_tree(dst_path + new_name, temp)
                && file_manager->fill_file_into_blocks(temp, (dst_path + new_name).substr(file_manager->home_path.size()), 0)) {
            puts("DEBUG: add_json_node done!");
            puts("DEBUG: fill_file_blocks done!");
            printf("cp: Success.\n");
            return true;
        }
    }
    else {
        printf("cp: '%s': Not a directory.\n", dst_path.c_str());
        return false;
    }
    return false;
}

/**
 * @brief chang the directory command (like 'cd' in the Linux)
 * 
 * @param dir_path the target dir_path (absolute or relative)
 * @return void
 */
void FileOperation::cd_command(string dir_path) {
    string tmp = dir_path;
    if (tmp.front() == (char)path::preferred_separator) 
        tmp.erase(0, 1);
    tmp = file_manager->get_absolute_working_path() + tmp;
    if (!exists(tmp)) {
        printf("cd: '%s' Not such directory.\n", dir_path.c_str());
        return;
    }
    if (string::npos == STRING(canonical((path)tmp)).find(file_manager->home_path))
        return;

    // if the dir_path is absolute
    if (dir_path.front() == (char)path::preferred_separator) {
        string target_path = file_manager->home_path + dir_path;
        if (exists(target_path)) {
            if (is_directory(target_path)) {
                file_manager->working_path = dir_path;
                if (file_manager->working_path.back() != (char)path::preferred_separator)
                    file_manager->working_path += (char)path::preferred_separator;
                return;
            }
            else {
                printf("cd: '%s' is not a directory.\n", dir_path.c_str());
                return;
            }
        }
        else {
            printf("cd: '%s' Not such directory.\n", dir_path.c_str());
            return;
        }
    }

    // if the dir_path is relative
    string target_path = file_manager->get_absolute_working_path() + dir_path;
    if (exists(target_path)) {
        if (is_directory(target_path)) {
            path p = target_path;
            // cout << "DEBUG: " << STRING(p) << endl;
            file_manager->working_path = STRING(canonical(p)).substr(file_manager->home_path.size());
            // cout << "DEBUG: " << file_manager->working_path << endl;
            if (file_manager->working_path.back() != (char)path::preferred_separator)
                file_manager->working_path += (char)path::preferred_separator;
        }
        else {
            printf("cd: '%s' is not a directory.\n", dir_path.c_str());
        }
    }
    else {
        printf("cd: '%s' Not such directory.\n", dir_path.c_str());
    } 
}

/**
     * @brief move a directory from the src_path to the dst_path
     * 
     * @param src_path the absolute path of the directory
     * @param dst_path the absolute path of the directory
     * @return bool 
     */
bool FileOperation::move_dir(string src_path, string dst_path) {
    src_path = pathConverter(src_path);
    dst_path = pathConverter(dst_path);
    if (src_path == "error" || dst_path == "error") {
        printf("mv: Cannot find the source directory.\n");
        return false;
    }

    if (!copy_dir(src_path, dst_path))
        return false;
    if (src_path.back() == path::preferred_separator)   src_path.pop_back();
    string dir_name = src_path.substr(src_path.find_last_of(path::preferred_separator) + 1);
    src_path.erase(src_path.find_last_of(path::preferred_separator));
    if (!delete_dir(src_path, dir_name))
        return false;
    return true;
}

/**
 * @brief copy a directory somewhere recursively
 * 
 * @param src_path the absolute source path of directory
 * @param dst_path the absolute destination path of directory
 * @return bool
 */
bool FileOperation::recursive_copy_dir(string src_path, string dst_path) {
    if (dst_path.back() != path::preferred_separator)
        dst_path.push_back(path::preferred_separator);

    if (!exists(src_path)) {
        printf("cp: '%s' No such directory\n", src_path.c_str());
        return false;
    }
    for (auto &item : directory_iterator(path(src_path))) {
        string file_path = STRING(item.path());
        string file_name = STRING(item.path().filename());
        if (is_directory(file_path)) {
            if (!create_dir(dst_path, file_name))   return false;
            if (!recursive_copy_dir(file_path, dst_path + file_name))     return false;
            puts("DEBUG: create dir.");
        }
        else {
            copy_file(file_path, dst_path);
            puts("DEBUG: copy file.");
        }
    }
    return true;
}

/**
 * @brief copy a directory somewhere safely
 * 
 * @param src_path the absolute source path of directory
 * @param dst_path the absolute destination path of directory
 * @return bool
 */
bool FileOperation::copy_dir(string src_path, string dst_path) {
    src_path = pathConverter(src_path);
    dst_path = pathConverter(dst_path);
    if (src_path == "error" || dst_path == "error") {
        printf("cp: Cannot find the source file.\n");
        return false;
    }

    if (!exists(src_path) || !is_directory(src_path)) {
        printf("cp: '%s' No such directory.\n", src_path.c_str());
        return false;
    }
    
    if (src_path.back() == path::preferred_separator)   src_path.pop_back();
    if (dst_path.back() == path::preferred_separator)   dst_path.pop_back();
    if (!exists(dst_path)) {
        if (!exists(dst_path.substr(0, dst_path.find_last_of(path::preferred_separator)))) {
            printf("cp: '%s' No such directory.\n", dst_path.c_str());
            return false;
        }
    }

    string dst_dir_name = src_path.substr(src_path.find_last_of(path::preferred_separator) + 1);

    if (!exists(dst_path)) {
        dst_dir_name = dst_path.substr(dst_path.find_last_of(path::preferred_separator) + 1);
        dst_path.erase(dst_path.find_last_of(path::preferred_separator));
    } 
    string tmp = file_manager->home_path + (char)path::preferred_separator + ".tmp";

    if (!create_dir(file_manager->home_path + (char)path::preferred_separator, ".tmp"))
        return false;
    if (!recursive_copy_dir(src_path, tmp))
        return false;
    if (dst_path.back() != path::preferred_separator)
        dst_path.push_back(path::preferred_separator);
    if (!create_dir(dst_path, dst_dir_name))
        return false;
    if (!recursive_copy_dir(tmp, dst_path + dst_dir_name))
        return false;
    if (!delete_dir(file_manager->home_path + (char)path::preferred_separator, ".tmp"))
        return false;

    return true;
}

/**
 * @brief list the files like the command 'ls' in Linux
 * 
 * @param dir_path the path of directory (absolute or relative)
 * @return void
 */
void FileOperation::ls_command(string dir_path) {
    // if dir_path is null string
    if (dir_path == "") 
        dir_path = file_manager->working_path;
    
    string target_dir, tmp;

    if (dir_path.front() == (char)path::preferred_separator) 
        target_dir = file_manager->home_path + dir_path;    // dir_path is absolute
    else 
        target_dir = file_manager->home_path + file_manager->working_path + dir_path;   // dir_path is relative
    
    if (!exists(target_dir)) {
        printf("ls: '%s' No such file or directory\n", dir_path.c_str());
        return;
    }

    tmp = target_dir = STRING(canonical((path)target_dir));
    if (string::npos == STRING(canonical((path)target_dir)).find(file_manager->home_path))
        tmp = target_dir = file_manager->home_path;
    // cout << "DEBUG:" << target_dir << endl;

    target_dir = target_dir.erase(0, file_manager->home_path.size() + 1);
    // cout << "DEBUG:" << target_dir << endl;

    json file_tree = file_manager->get_file_system_tree();
    // Need to search the direct directory
    while (target_dir.find((char)path::preferred_separator) != string::npos) {
        string temp_name = target_dir.substr(0, target_dir.find_first_of((char)path::preferred_separator));
        if (file_tree.contains(temp_name) == false) {
            printf("ls: '%s' No such file or directory\n", dir_path.c_str());
            return;
        }
        file_tree = file_tree[temp_name];
        target_dir.erase(0, target_dir.find_first_of((char)path::preferred_separator) + 1);
    }
    
    if (target_dir != "" && file_tree.contains(target_dir) == false) {
        printf("ls: '%s' No such file or directory\n", dir_path.c_str());
        return;
    }
    
    if (is_directory(tmp)) {
        if (target_dir != "")
            file_tree = file_tree[target_dir];
        int max_len = 0;
        for (auto &element : file_tree.items()) {
            if (max_len < element.key().size())
                max_len = element.key().size();
        }
        int count = 0;
        for (auto &element : file_tree.items()) {
            if ((++count) % 8 == 0) 
                cout << element.key() << "\n";
            else
                cout << element.key() << setw(max_len + 2);
        }
        cout << setw(0);
        if (count % 8) puts("");
    }
    else {
        cout << target_dir << "\n";
    }
}

/**
 * @brief Convert from the input path of the shell to the local hard disk path
 * 
 * @param shell_input_path the input path of the shell
 * @return string
 */
string FileOperation::pathConverter(string shell_input_path)
{
    // shell input a absolute path from the home
    if (shell_input_path.front() == (char)path::preferred_separator) {
        cout << "DEBUG " << file_manager->home_path + shell_input_path << endl;;
        return file_manager->home_path + shell_input_path;
    }
    else {
        string path = file_manager->home_path + file_manager->working_path + shell_input_path;
        if (!exists(path)) {
            if (path.back() == path::preferred_separator)
                path.pop_back();
            string file_name = path.substr(path.find_last_of(path::preferred_separator) + 1);
            path = path.erase(path.find_last_of(path::preferred_separator));
            if (!exists(path)) {
                return "error";
            }
            path = STRING(canonical(path));
            path.push_back((char)path::preferred_separator);
            path += file_name;
            cout << "DEBUG " << path << endl;
            return path;
        }
        path = STRING(canonical(path));
        cout << "DEBUG " << path << endl;
        return path;
    }
}