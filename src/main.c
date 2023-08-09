#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "server.h"

// https://stackoverflow.com/questions/4553012/checking-if-a-file-is-a-directory-or-just-a-file
int is_directory(const char *path)
{
    struct stat stat_buff;
    if (stat(path, &stat_buff) != 0)
        return 0;
    return S_ISDIR(stat_buff.st_mode);
}

int main(void)
{

    // short program to print every file and folder name within dir
    DIR *user_dir = opendir("."); // scanner for dirs
    char *dir_arr[1024];          // big number of folders
    int dir_arr_length = 0;       // logical length for dir_arr
    char curr_dir_path[256] = ""; // path of directory currently scanning
    char path[256] = "";          // path to currently found file/folder
    struct dirent *found_dir;     // data regarding found file/folder
    int max_iterations = 1024;    // big number to avoid infinite loops or running too long while testing

    while (user_dir && (max_iterations-- > 0))
    { // while user_dir is open
        // getting next dir struct
        found_dir = readdir(user_dir);

        if (found_dir != NULL)
        {
            // checking if found_dir is a folder or file
            strcpy(path, strcat(curr_dir_path, found_dir->d_name));
            printf("%s", path);
            if (is_directory(path))
            {
                // adding to dir_path
                dir_arr[dir_arr_length] = path;
                dir_arr_length++;
                printf("/");
            }
            printf("\n");
        }
        else
        {
            printf("End of stream reached for:\n\t%s\n", curr_dir_path);

            // if (user_dir != NULL)
            //     closedir(user_dir);

            if (dir_arr_length > 0)
            {

                user_dir = opendir(dir_arr[0]); // get the first element of queue and start search again from there

                // updating curr_dir_path
                strcpy(curr_dir_path, dir_arr[0]);

                // printing dir_arr
                printf("~dir_arr~\n");
                for (int i = 0; i < dir_arr_length; i++)
                    printf("\t%s\n", dir_arr[i]);

                // removing first element from dir_arr
                char *dir_arr_buff[1024];
                for (int i = 1; i < dir_arr_length; i++)
                    dir_arr_buff[i - 1] = dir_arr[i];
                dir_arr_length--;
                for (int i = 0; i < dir_arr_length; i++)
                    dir_arr[i] = dir_arr_buff[i];

                // printing dir_arr
                printf("~dir_arr~\n");
                for (int i = 0; i < dir_arr_length; i++)
                    printf("\t%s\n", dir_arr[i]);
            }
        }
    }

    return 0;
}