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

// returns length of path_arr

/*
1. assign

*/

int get_paths_in_dir(char ***path_arr, const char *path)
{
    size_t num_paths = 0; // logical length of path_arr

    // populate path_arr with paths excluding "./" and "../"
    strcpy((*path_arr)[0], "0");
    num_paths = 1;

    return num_paths;
}