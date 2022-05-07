
// include guard
#ifndef FILE_MANAGER_H_
#define FILE_MANAGER_H_

#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>
#include <algorithm>
#include <cmath>
#include "json.hpp" // JSON for Modern C++
#include <chrono>
#include <thread>

// cross platform
// fit to Windows, Linux and Unix
#ifdef _WIN64
#include <windows.h>
#define STRING(s) WStringToString(s)
#define SLEEP(t) Sleep(t)
#elif _WIN32
#include <windows.h>
#define STRING(s) WStringToString(s)
#define SLEEP(t) Sleep(t)
#else
#include <unistd.h>
#define STRING(s) (string) s
#define SLEEP(t) usleep(1000 * t)
#endif

using namespace std;
using namespace std::filesystem; // filesystem and their components
using nlohmann::json;

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
    /**
     * @brief return track and sector by pair
     *
     * @return pair<int, int>
     */
    pair<int, int> get_location();
    /**
     * @brief return free_space
     *
     * @return int
     */
    int get_free_space();
    /**
     * @brief return fp
     *
     * @return string
     */
    string get_fp();

private:
    int total_space; // total space of the Block (byte)
    int free_space;  // free space of the Block (byte)
    int track;       // track index of the Block
    int sector;      // sector index of the Block
    string fp;       // file path relative to home directory
};

// Disk class
class Disk
{
public:
    /**
     * @brief Construct a new Disk object
     *
     * @param block_size the size of a block
     * @param track_num the number of tracks
     * @param sector_num // the number of sectors for each track
     */
    Disk(int block_size, int track_num, int sector_num);
    /**
     * @brief default constructor
     *
     */
    Disk();
    /**
     * @brief set head pointer new position
     *
     * @param head_pointer
     */
    void set_head_pointer(int head_pointer);
    /**
     * @brief seek one by one in sequence
     *
     * @param seek_queue
     */
    void seek_by_queue(vector<pair<int, int>> seek_queue);
    /**
     * @brief FCFS algorithm
     *
     * @param seek_queue
     */
    void FCFS(vector<pair<int, int>> seek_queue);
    /**
     * @brief SSTF
     *
     * @param seek_queue
     */
    void SSTF(vector<pair<int, int>> seek_queue);
    /**
     * @brief SCAN
     *
     * @param seek_queue
     */
    void SCAN(vector<pair<int, int>> seek_queue);
    /**
     * @brief C-SCAN
     *
     * @param seek_queue
     */
    void C_SCAN(vector<pair<int, int>> seek_queue);
    /**
     * @brief LOOK
     *
     * @param seek_queue
     */
    void LOOK(vector<pair<int, int>> seek_queue);
    /**
     * @brief C-LOOK
     *
     * @param seek_queue
     */
    void C_LOOK(vector<pair<int, int>> seek_queue);

private:
    int sector_size;     // the size of a sector
    int track_num;       // the number of tracks
    int track_size;      // the number of sectors for each track
    int head_pointer;    // the track index of the head
    double seek_speed;   // time taken to cross a track
    double rotate_speed; // average sector seek and read time
    int slow_ratio;      // slow ratio for simulation disk operation
};

// FileManager class
class FileManager
{
public:
    char file_separator = (char)path::preferred_separator;                             // directory separator
    string home_path = STRING(current_path().parent_path()) + file_separator + "home"; // absolute path of home directory
    string working_path;                                                               // working directory

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
     * @brief free blocks occupied by a file
     *
     * @param file_path path relative to home
     * @return bool
     */
    bool delete_file_from_blocks(string file_path);
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
     * @brief return the absolute working path
     * @return string
     */
    string get_absolute_working_path();
    /**
     * @brief add the json node to the file_system_tree
     *
     * @param path the path of the file(absolute)
     * @return bool
     */
    bool add_json_node_to_tree(string path, json node);
    /**
     * @brief delete the json node from the file_system_tree
     *
     * @param path the path of the file (absolute)
     * @return bool
     */
    bool delete_json_node_from_tree(string path);
    /**
     * @brief set disk's head pointer new position
     *
     * @param head_pointer
     */
    void set_disk_head_pointer(int head_pointer);
    /**
     * @brief get dict by path
     *
     * @param file_path file path relative to home
     * @return json
     */
    json path2dict(string file_path);
    /**
     * @brief just a demo for disk seek
     *
     * @param seek_algo
     */
    void get_file_demo(string seek_algo = "FCFS");
    /**
     * @brief get file info
     *
     * @param file_path file path relative to home
     * @param mode read or write
     * @param seek_algo seek algorithm
     * @return json
     */
    json get_file(string file_path, string mode, string seek_algo);
    /**
     * @brief simulate the paging read process
     *
     * @param file_path file path relative to home
     * @param address starting address relative to file
     * @param length the length of the data to be read
     * @return bool
     */
    bool read_data(string file_path, int address, int length);
    /**
     * @brief simulate the write process
     *
     * @param file_path file path relative to home
     * @param length the length of the data to be write
     * @return bool
     */
    bool write_data(string file_path, int length);
    /**
     * @brief tidy disk external fragmentation
     *
     */
    void tidy_disk();
    /**
     * @brief print status of all blocks
     *
     */
    void display_storage_status();

    /**
     * @brief return the private file_system_tree
     * @return json
     */
    json get_file_system_tree();

private:
    Disk disk;             // disk
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

    vector<int> busy_blocks; // busy blocks list
};

#endif