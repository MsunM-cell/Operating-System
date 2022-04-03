#include "file_manager.h"
#ifndef FILE_OPERATION_HPP
#define FILE_OPERATION_HPP

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
     * @param current_dir the working directory
     * @param file_name the name of the file
     * @return bool
     */
    bool create_file(string current_dir, string file_name);
    
    /**
     * @brief delete a new file
     * 
     * @param current_dir the working directory
     * @param file_name the name of the file
     * @return bool
     */
    bool delete_file(string current_dir, string file_name);

    /**
     * @brief create a new directory
     * 
     * @param current_dir the working directory
     * @param dir_name the name of the directory
     * @return bool
     */
    bool create_dir(string current_dir, string dir_name);

    /**
     * @brief delete a new directory
     * 
     * @param current_dir the working directory
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
 * @param current_dir the working directory
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
        printf("Existed! Please try again.\n");
        return false;
    }

    // default setting
    json new_file;
    new_file["name"] = file_name;
    new_file["type"] = "crw-";
    new_file["size"] = 128;
    new_file["content"] = "";

    if (file_manager->fill_file_into_blocks(new_file, cur_file_path, 0)) {
        ofstream outfile(cur_file_path, ios::out);
        outfile << std::setw(4) << new_file;
        outfile.close();
        input.open(cur_file_path, ios::in);
        if (input.is_open()) {
            return true;
        }
    }

    return false;
}

/**
 * @brief delete a new file
 * 
 * @param current_dir the working directory
 * @param file_name the name of the file
 * @return bool
 */
bool FileOperation::delete_file(string current_dir, string file_name)
{
    string cur_file_path = current_dir + file_name;
    if (!exists(cur_file_path)) {
        printf("Cannot create file '%s': File exists\n", file_name.c_str());
        return false;
    }

    if (remove(cur_file_path)) 
        return true;

    return false;
}

/**
 * @brief create a new directory
 * 
 * @param current_dir the working directory
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
    if (create_directory(cur_dir_path)) 
        return true;
    
    printf("Create directory '%s' failed.\n", dir_name.c_str());
    return false;

}
/**
 * @brief delete a new directory
 * 
 * @param current_dir the working directory
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
    
    if (remove(cur_dir_path))
        return true;
    
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



#endif