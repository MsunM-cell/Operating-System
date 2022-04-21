
#ifndef FILE_OPERATION_HPP
#define FILE_OPERATION_HPP

#include "file_manager.h"

// FileOperaion class
class FileOperation
{
public:
    /**
     * @brief construct a new FileOperation object with a FileManager pointer
     * 
     * @param fm a pointer to FileManager instance
     */
    FileOperation(FileManager* fm);

    /**
     * @brief create a new file
     * 
     * @param current_dir the working directory (absolute)
     * @param file_name the name of the file
     * @return bool
     */
    bool create_file(string current_dir, string file_name);
    
    /**
     * @brief delete a new file
     * 
     * @param current_dir the working directory (absolute)
     * @param file_name the name of the file
     * @return bool
     */
    bool delete_file(string current_dir, string file_name);

    /**
     * @brief create a new directory
     * 
     * @param current_dir the working directory (absolute)
     * @param dir_name the name of the directory
     * @return bool
     */
    bool create_dir(string currenet_dir, string dir_name);

    /**
     * @brief delete a new directory
     * 
     * @param current_dir the working directory (absolute)
     * @param dir_name the name of the directory
     * @return bool
     */
    bool delete_dir(string current_dir, string dir_name);

    /**
     * @brief traverse a directory recursively (make direct use of the public function in the FileManager)
     * 
     * @param dir the directory which needs to be traversed
     * @param layer 
     */
    void tree(string dir, int layer = 0);

    /**
     * @brief return the size of the file, -1 means 'a existed path', -2 means 'is a directroy', 
     *        -3 means 'file open error', else means the size of the file.
     * 
     * @param path the absolute of the file
     * @return int 
     */
    int file_size(string path);

    /**
     * @brief modify the type of the file
     * 
     * @param file_path the absolute path of the file
     * @param mode use a decimal number for a three-bit binary, 1 means the permission is not available. 
     *             From the left to right is to read, write and execute.
     * @return bool
     */
    bool modify_file_type(string file_path, unsigned int mode);

    /**
     * @brief rename the file
     * 
     * @param file_path the absolute file path
     * @param new_name the new name of the file
     * @return bool
     */
    bool rename_file(string file_path, string new_name);

    /**
     * @brief move a file from the src_path to the dst_path
     * 
     * @param src_path the source path of a file
     * @param dst_path the destination path of the file
     * @return bool
     */
    bool move_file(string src_path, string dst_path);

    /**
     * @brief copy a file from the src_path to the dst_path
     * 
     * @param src_path the source path of a file
     * @param dst_path the destination path of the file
     * @return bool
     */
    bool copy_file(string src_path, string dst_path);



private:
    FileManager* file_manager;  // The pointer to a FileManager instance

};

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
    string cur_file_path = current_dir + file_name;
    // cout << cur_file_path << endl;

    ifstream input;
    input.open(cur_file_path, ios::in);
    if (input.is_open()) {
        input.close();
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
    if (file_manager->fill_file_into_blocks(new_file, relative_path, 0)) {
        ofstream outfile(cur_file_path, ios::out);
        outfile << std::setw(4) << new_file;
        outfile.close();
        input.open(cur_file_path, ios::in);
        // check if the file has been created and if add to the file_system_tree
        if (input.is_open() && file_manager->add_json_node_to_tree(cur_file_path, new_file)) {
            input.close();
            printf("Success: make file %s\n", file_name.c_str());
            return true;
        }
        input.close();
        printf("open file failed or add json failed !\n");
    }
    else
        printf("disk storage error: no enough space\n");

    return false;
}

/**
 * @brief delete a new file
 * 
 * @param current_dir the working directory (absolute)
 * @param file_name the name of the file
 * @return bool
 */
bool FileOperation::delete_file(string current_dir, string file_name)
{
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
 * @brief create a new directory
 * 
 * @param current_dir the working directory (absolute)
 * @param dir_name the name of the directory
 * @return bool
 */
bool FileOperation::create_dir(string current_dir, string dir_name)
{
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
    string cur_dir_path = current_dir + dir_name;
    if (!exists(cur_dir_path)) {
        printf("Cannot delete directory '%s': File not exists\n", dir_name.c_str());
        return false;
    }

    if (!filesystem::is_empty(cur_dir_path)) {
        printf("rmdir: failed to remove '%s': Directory not empty\n", dir_name.c_str());
        return false;
    }

    if (file_manager->delete_json_node_from_tree(cur_dir_path) && remove_all(cur_dir_path)) {
        printf("Remove directory '%s' success.\n", dir_name.c_str());
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
 *             From the left to right is to read, write and execute.
 * @return bool
 */
bool FileOperation::modify_file_type(string file_path, unsigned int mode) {
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

    printf("chmod '%s' success.\n", file_name.c_str());

    return true;
}

/**
 * @brief rename the file
 * 
 * @param file_path the absolute file path
 * @param new_name the new name of the file
 * @return bool
 */
bool FileOperation::rename_file(string file_path, string new_name) {
    if (!exists(file_path)) {
        printf("Cannot find the file which will be renamed.\n");
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
    temp["name"] = new_name;
    
    ofstream output(file_path);
    if (!output.is_open()) {
        printf("'%s' file open failed.\n", file_name.c_str());
        output.close();
        return false;
    }

    output << setw(4) << temp;
    output.close();

    string file_dir = file_path.substr(0, file_path.find_last_of((char)path::preferred_separator) + 1);
    // cout << "debug: " << file_dir << endl;
    rename(file_path, file_dir + new_name);
    
    return true;
}

/**
 * @brief move a file from the src_path to the dst_path
 * 
 * @param src_path the source path of a file
 * @param dst_path the destination path of the file
 * @return bool
 */
bool FileOperation::move_file(string src_path, string dst_path) {
    if (!exists(src_path)) {
        printf("mv: Cannot find the source file.\n");
        return false;
    }
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
        rename(src_path, dst_path + file_name);
        printf("mv: Success.\n");
        return true;
    }

    if (src_path == dst_path) {
        printf("mv: '%s' and '%s' are the same file.\n", file_name.c_str(), file_name.c_str());
        return false;
    } 

    // if the directory is not existed
    if (dst_path.back() == path::preferred_separator) {
        if (!exists(dst_path)) {
            printf("mv: '%s': Not a directory.\n", dst_path.c_str());
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
            printf("mv: '%s' file open failed.\n", file_name.c_str());
            input.close();
            return false;
        }
        input >> temp;
        input.close();

        temp["name"] = new_name;
        ofstream output(src_path);
        if (!output.is_open()) {
            printf("mv: '%s' file open failed.\n", file_name.c_str());
            output.close();
            return false;
        }
        output << setw(4) << temp;
        output.close();

        rename(src_path, dst_path + new_name);
        printf("mv: Success.\n");
        return true;
    }
    else {
        printf("mv: '%s': Not a directory.\n", dst_path.c_str());
        return false;
    }

}

 /**
 * @brief copy a file from the src_path to the dst_path
 * 
 * @param src_path the source path of a file
 * @param dst_path the destination path of the file
 * @return bool
 */
bool FileOperation::copy_file(string src_path, string dst_path) {
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

    // if dst_path is a existed directroy, move directly.
    if (exists(dst_path) && is_directory(dst_path)) {
        if (dst_path.back() != path::preferred_separator)  dst_path += path::preferred_separator;
        if (src_path == dst_path + file_name) {
            printf("cp: '%s' and '%s' are the same file.\n", file_name.c_str(), (dst_path + file_name).c_str());
            return false;
        }
        copy(src_path, dst_path + file_name);
        printf("cp: Success.\n");
        return true;
    }

    if (src_path == dst_path) {
        printf("cp: '%s' and '%s' are the same file.\n", file_name.c_str(), file_name.c_str());
        return false;
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

        printf("cp: Success.\n");
        return true;
    }
    else {
        printf("cp: '%s': Not a directory.\n", dst_path.c_str());
        return false;
    }
}

#endif