#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <dirent.h>
#include <string.h>

#include "file.h"

// returns size of buffer assigned to byte_buff
int get_from_file(char **byte_buff, const char *filename)
{
    FILE *file_ptr;
    file_ptr = fopen(filename, "rb");
    // catching failed to open file
    if (file_ptr == NULL)
        return -1;
    // getting length of file
    fseek(file_ptr, 0, SEEK_END);
    size_t len = ftell(file_ptr);
    rewind(file_ptr);
    // creating buffer for bytes
    free(*byte_buff);
    *byte_buff = (char *)malloc(len * sizeof(char));
    // catching failed allocation
    if (*byte_buff == NULL)
        return 0;
    // read bytes into buffer
    fread(*byte_buff, len * sizeof(char), 1, file_ptr);
    // cleaning up
    fclose(file_ptr);
    // returns len so we can use it as a parameter elsewhere
    return len * sizeof(char);
}

// returns true if path points to directory
bool is_directory(const char *path)
{
    struct stat stat_buff;
    // catching unsuccessful updates to stat_buff
    if (stat(path, &stat_buff) != 0)
        return false;
    // macro that returns non-zero if file is to directory
    return S_ISDIR(stat_buff.st_mode);
}

// returns true if path only has dots in it
bool is_only_periods(const char *path)
{
    char c = path[0];
    // iterating over array checking that every char is a period
    int i = 0;
    while (c != '\0')
    {
        if (c != '.')
            return false;
        c = path[++i];
    }
    return true;
}

// populates path_arr with paths in curr_dir, resizes path_arr to perfectly fit all paths, returns length of path_arr
int get_paths_in_dir(char ***path_arr, const char *curr_dir)
{
    size_t num_paths = 0; // logical length of path_arr

    // create buffer
    // TEMP: starting at 1 to test resizing
    int max_path_arr_buff_size = 1; // starting with size of 64 hoping to not need to resize ever
    char **path_arr_buff = (char **)malloc(max_path_arr_buff_size * sizeof(char *));

    // populate buffer
    // TEMP: adding random data into array to test resizing
    for (int i = 0; i < 10; i++)
    {
        // increase buffer size when needed
        if (num_paths + 1 >= max_path_arr_buff_size)
        {
            max_path_arr_buff_size += 2;              // TEMP: only adding space for 2 more for testing
            char **tmp_path_arr_buff = path_arr_buff; // creating 2nd pointer to values
            path_arr_buff = (char **)malloc(max_path_arr_buff_size * sizeof(char *));
            // repopulating old buffer
            for (int j = 0; j < num_paths; j++)
            {
                path_arr_buff[j] = tmp_path_arr_buff[j];
            }
            free(tmp_path_arr_buff);
        }

        // appending data to path_arr_buff
        path_arr_buff[0] = (char *)malloc(64 * sizeof(char)); // TODO: standardise path string lengths in program
        strcpy(path_arr_buff[0], "test");

        // keep track of logical length of buffer
        num_paths++;
    }

    // assign enough memory to path_arr to perfectly fit buffer

    // populate path_arr

    // populate path_arr with paths excluding "./" and "../"
    strcpy((*path_arr)[0], "0");
    num_paths = 1;

    return num_paths;
}