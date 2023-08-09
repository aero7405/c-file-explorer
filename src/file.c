#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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