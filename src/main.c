#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>

#include "server.h"
#include "file.h"

int main(void)
{
    // initialising to take minimum amount of memory until needed because it is dynamically reallocated anyway
    char **paths = NULL;
    int paths_len = 0;

    char curr_dir[PATH_STRING_LENGTH] = "C:/";
    char folder_path_queue[1024][PATH_STRING_LENGTH];
    int queue_len = 1;

    // TEMP: limiting iterations so testing doesn't take *forever*
    int max_iterations = 200;
    while (max_iterations-- > 0) // repeats until break
    {
        paths = NULL; // proper cleanup is at bottom
        // getting all dirs in current dir
        paths_len = get_paths_in_dir(&paths, curr_dir);

        // raising error if paths hasn't been initialised correctly
        if (paths_len == -1)
        {
            printf("WARNING: UNABLE TO GET DIRECTORIES AT REQUESTED PATH!\n");
            return 1;
        }

        // adding any folder dirs to path
        for (int i = 0; i < paths_len; i++)
        {
            // adding dir to queue if found to be a folder and space in queue
            if (queue_len + 1 < 1024 && IS_FOLDER(paths[i]))
            {
                strcpy(folder_path_queue[queue_len], paths[i]);
                queue_len++;
            }
            printf("%s\n", paths[i]);
        }

        // breaking if no elements in queue
        if (queue_len < 1)
            break;
        // getting next path in queue to search
        strcpy(curr_dir, folder_path_queue[0]);
        // shift all elements in queue left 1
        for (int i = 0; i < queue_len - 1; i++) // -1 so that we don't try to access out of bounds when we [i + 1]
            strcpy(folder_path_queue[i], folder_path_queue[i + 1]);
        // 1st element is removed from queue so deincrement queue_len
        queue_len--;

        // cleaning up paths
        for (int i = 0; i < paths_len; i++)
            free(paths[i]);
        free(paths);
    }

    getchar(); // pausing :)

    return 0;
}