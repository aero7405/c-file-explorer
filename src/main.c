#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>

#include "server.h"
#include "file.h"

int main(void)
{
    char **paths = NULL;
    paths = (char **)malloc(10 * sizeof(char *));
    paths[0] = (char *)malloc(16 * sizeof(char));

    strcpy(paths[0], "test");
    printf("10->%s\n", paths[0]);

    int len = get_paths_in_dir(&paths, 10, ".");

    printf("%d->%s\n", len, paths[0]);

    return 0; // intetionally making everything below this dead code

    // short program to print every file and folder name within dir
    DIR *user_dir = opendir(".");                  // scanner for dirs
    char dir_arr[1024][256];                       // max queue of 1024 directories to search and max path length of 256
    int dir_arr_length = 0;                        // logical length for dir_arr
    const int DIR_ARRAY_MAX_LENGTH = 1023;         // last valid index in dir_array
    char curr_dir_path[256] = ".";                 // path of directory currently scanning
    char path[256] = "";                           // path to currently found file/folder
    char path_buff[256] = "";                      // buffer for concatinating strings
    struct dirent *found_dir;                      // data regarding found file/folder
    int max_iterations = DIR_ARRAY_MAX_LENGTH + 1; // big number to avoid infinite loops or running too long while testing

    while (user_dir && (max_iterations-- > 0))
    { // while user_dir is open
        // getting next dir struct
        found_dir = readdir(user_dir);
        // checking if there was a dir remaining in the stream
        if (found_dir != NULL)
        {
            // skipping path to go back
            if (is_only_periods(found_dir->d_name))
                continue;
            // getting full relative path to folder or file not just its name
            strcpy(path_buff, curr_dir_path);
            strcat(path_buff, "/");
            strcat(path_buff, found_dir->d_name);
            strcpy(path, path_buff);
            printf("%s", path);
            // checking if found_dir is a folder or file
            if (is_directory(path))
            {
                // checking there is space in array
                if (dir_arr_length < DIR_ARRAY_MAX_LENGTH)
                {
                    // adding to dir_path
                    strcpy(dir_arr[dir_arr_length], path);
                    dir_arr_length++;
                    printf("/");
                }
            }
            printf("\n");
        }
        else
        {
            // printf("End of stream reached for:\n\t%s\n", curr_dir_path);
            // ensuring that the dir is always closed
            closedir(user_dir);

            if (dir_arr_length > 0)
            {
                user_dir = opendir(dir_arr[0]); // get the first element of queue and start search again from there

                // updating curr_dir_path
                strcpy(curr_dir_path, dir_arr[0]);
                // printf("Now searching: %s\n", curr_dir_path);

                // printing dir_arr
                // printf("~search queue~\n");
                // for (int i = 0; i < dir_arr_length; i++)
                //     printf("\t%s\n", dir_arr[i]);

                // removing first element from dir_arr
                char dir_arr_buff[1024][256];
                for (int i = 1; i < dir_arr_length; i++)
                    strcpy(dir_arr_buff[i - 1], dir_arr[i]);
                dir_arr_length--;
                for (int i = 0; i < dir_arr_length; i++)
                    strcpy(dir_arr[i], dir_arr_buff[i]);
            }
            else
            {
                // finished search
                break;
            }
        }
    }

    return 0;
}