#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <dirent.h>
#include <string.h>

#include "file.h"

void get_parent_dir(char *parent_buff, const char *curr_dir)
{
    strncpy(parent_buff, curr_dir, PATH_STRING_LENGTH);
    for (int i = strnlen(parent_buff, PATH_STRING_LENGTH) - 2; i >= 0; i--) // -2 to skip a triling /
    {
        if (parent_buff[i] == '/')
        {
            parent_buff[i + 1] = '\0';
            break;
        }
    }
}

// returns size of buffer assigned to byte_buff (-1 if file failed to open)
// FIXME: there is something here causing a SIGFAULT, I believe it may not be calculating buffers right
// Suggested fix? Rewrite function, its quite cumbersome anyway
// Make sure to make it non-reliant on null-termination characters
// the png has null terminators within it - to see binary in powershell `format-hex PATH | more`
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

// populates path_arr with paths in curr_dir,
// resizes path_arr to perfectly fit all paths,
// returns length of path_arr
int get_paths_in_dir(char ***path_arr, const char *curr_dir_path)
{
    // expects **path_arr == NULL
    if (path_arr == NULL || *path_arr != NULL)
        return -1;

    // iterate over curr_dir and count exactly how many elements there are
    size_t num_paths = 0; // logical length of path_arr
    DIR *curr_dir = opendir(curr_dir_path);
    char curr_path[PATH_STRING_LENGTH]; // path to directory found in each iteration
    struct dirent *found_dir;           // data found about directory
    while (curr_dir)
    { // while curr_dir is open
        // getting next dir struct in stream
        found_dir = readdir(curr_dir);
        // ending search if end of stream reached
        if (found_dir == NULL)
            break;
        // skipping path to go back
        if (is_only_periods(found_dir->d_name))
            continue;
        // increase count of paths to allocate space for
        num_paths++;
    }

    // case where no directories are found to avoid malloc(0)
    if (num_paths < 1)
        return 0;

    // allocating required memory for array
    // no need to free memory because we have already checked that there was none allocated
    *path_arr = (char **)malloc(num_paths * sizeof(char *));
    // checking for failed allocation
    if (*path_arr == NULL)
        return -1;
    // allocating required memory for strings
    for (size_t i = 0; i < num_paths; i++)
    {
        // Lost many MANY hours of my life because I forgot to assign a starting value of i
        (*path_arr)[i] = (char *)malloc(PATH_STRING_LENGTH * sizeof(char));
        // checking for failed allocation
        if ((*path_arr)[i] == NULL)
            return -1;
    }

    // populate path_arr with paths excluding "./" and "../"
    rewinddir(curr_dir); // resetting curr_dir
    int i = 0;           // iterator for path_arr
    while (curr_dir)
    { // while curr_dir is open
        // getting next dir struct in stream
        found_dir = readdir(curr_dir);
        // ending search if end of stream reached
        if (found_dir == NULL)
        {
            // cleaning up :P
            closedir(curr_dir);
            break;
        }
        // skipping path to go back
        if (is_only_periods(found_dir->d_name))
            continue;
        // getting full relative path to folder or file not just its name
        strcpy(curr_path, curr_dir_path);
        if (curr_dir_path[strlen(curr_dir_path) - 1] != '/') // adding slash if not present
            strcat(curr_path, "/");
        strcat(curr_path, found_dir->d_name);
        // concatinating '/' to end if element if a directory
        if (is_directory(curr_path))
            strcat(curr_path, "/");
        // append path to path_arr
        strcpy((*path_arr)[i++], curr_path);
    }

    return num_paths;
}