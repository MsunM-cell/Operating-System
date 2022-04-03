
// include guard
#ifndef FILE_MANAGER_H_
#define FILE_MANAGER_H_

#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>
#include "json.hpp" // JSON for Modern C++

using namespace std;
using namespace std::filesystem; // filesystem and their components
using nlohmann::json;

// cross platform
// fit to Windows, Linux and Unix
# ifdef _WIN64
#define STRING(s) WStringToString(s) 
# elif _WIN32
#define STRING(s) WStringToString(s) 
# else
#define STRING(s) (string)s 
# endif

// compare each other on the basis of 2nd element of pairs
// in ascending order
bool cmp(pair<int, int> a, pair<int, int> b);

// If you need to convert a wstring into a string, just call it.
string WStringToString(const wstring &s);

// If you need to convert a string into a wstring, just call it.
wstring StringToWString(const string &s);

// Block class
class Block
{
public:
    /**
     * @brief Construct a new Block object.
     *
     * @param total_space total space of the Block (byte)
     * @param track track index of the Block
     * @param sector sector index of the Block
     */
    Block(int total_space, int track, int sector);
    /**
     * @brief Set new value to free_space.
     *
     * @param free_space
     */
    void set_free_space(int free_space);
    /**
     * @brief Set new value to fp.
     *
     * @param fp
     */
    void set_fp(string fp);

private:
    int total_space; // total space of the Block (byte)
    int free_space;  // free space of the Block (byte)
    int track;       // track index of the Block
    int sector;      // sector index of the Block
    string fp;       // file path relative to home directory
};

// FileManager class
class FileManager
{
public:
    char file_separator = (char)path::preferred_separator;               // directory separator
    string home_path = STRING(current_path()) + file_separator + "home"; // absolute path of home directory

    /**
     * @brief Construct a new File Manager object
     *
     * @param block_size block size (byte)
     * @param track_num the number of tracks
     * @param sector_num the number of sectors
     */
    FileManager(int block_size, int track_num, int sector_num);
    /**
     * @brief Initialize all disk blocks.
     *
     */
    void init_blocks();
    /**
     * @brief Set busy blocks to 0 in bitmap.
     *
     */
    void set_busy_block();
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
    json init_file_system_tree(string current_path);
    /**
     * @brief Allocate disk blocks to a file.
     *
     * @param file_info information about the file in JSON
     * @param file_path file path relative to home directory
     * @param method how to find the idle disk blocks
     * @return true if allocate successfully
     */
    bool fill_file_into_blocks(json file_info, string file_path, int method);
    /**
     * @brief Find the idle blocks in a certain method.
     *
     * @param num the number of disk blocks we need
     * @param method 0(first fit), 1(best fit), 2(worst fit)
     * @return first block index of allocated blocks
     */
    int find_idle_blocks(int num, int method);
    /**
     * @brief Convert bitmap to string.
     *
     * @param source_bitmap
     * @return string
     */
    string bitmap2str(vector<int> source_bitmap);
    /**
     * @brief Use First Fit to find the idle blocks.
     *
     * @param target_str
     * @return int
     */
    int first_fit(string target_str);
    /**
     * @brief Use Best Fit to find the idle blocks.
     *
     * @param target_str
     * @return int
     */
    int best_fit(string target_str);
    /**
     * @brief Worst First Fit to find the idle blocks.
     *
     * @param target_str
     * @return int
     */
    int worst_fit(string target_str);
    int get_number_of_files(string directory);
    /**
     * @brief print file system by tree recursively
     * 
     * @param directory 
     * @param layer 
     */
    void print_file_system_tree(string directory, int layer = 0);

    /**
     * @brief return the relative working path
     * @return string
     */
    string get_relative_working_path();

    /**
     * @brief add the json node to the file_system_tree
     * 
     * @param path the path of the file
     * @return bool
     */
    bool add_json_node_to_tree(string path, json node);

private:
    vector<Block> blocks;  // all disk blocks
    vector<int> bitmap;    // bitmap for disk blocks
                           // 1 stands for a idle block and 0 a busy block
    json file_system_tree; // file system tree
    json file_blocks;      // store file's blocks info
                           // directory entry: first block index, blocks length, file size

    int block_size; // block size (byte)
    int track_num;  // the number of tracks
    int sector_num; // the number of sectors for each track
    int block_num;  // the number of blocks

    string working_path;     // working directory
    vector<int> busy_blocks; // busy blocks list
};

#endif