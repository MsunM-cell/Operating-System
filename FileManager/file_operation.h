
#ifndef FILE_OPERATION_H
#define FILE_OPERATION_H

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

    /**
     * @brief chang the directory command (like 'cd' in the Linux)
     * 
     * @param dir_path the target dir_path (absolute or relative)
     * @return void
     */
    void cd_command(string dir_path);

    /**
     * @brief move a directory from the src_path to the dst_path
     * 
     * @param src_path the absolute path of the directory
     * @param dst_path the absolute path of the directory
     * @return bool 
     */
    bool move_dir(string src_path, string dst_path);
    
    /**
     * @brief copy a directory somewhere safely
     * 
     * @param src_path the absolute source path of directory
     * @param dst_path the absolute destination path of directory
     * @return bool
     */
    bool copy_dir(string src_path, string dst_path);

    /**
     * @brief list the files like the command 'ls' in Linux
     * 
     * @param dir_path the path of directory (absolute or relative)
     * @return void
     */
    void ls_command(string dir_path);

    /**
     * @brief Convert from the input path of the shell to the local hard disk path
     * 
     * @param shell_input_path the input path of the shell
     * @return string
     */
    string pathConverter(string shell_input_path);
    
private:
    FileManager* file_manager;  // The pointer to a FileManager instance
    
    /**
     * @brief copy a directory somewhere recursively (maybe recursive repeat)
     * 
     * @param src_path the absolute source path of directory
     * @param dst_path the absolute destination path of directory
     * @return bool
     */
    bool recursive_copy_dir(string src_path, string dst_path);

    

};


#endif