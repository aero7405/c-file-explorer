#include <stdio.h>

void get_text_from_file(char* content, unsigned int max_bytes_to_read, const char* filename) {
    // reading the html file for testing
    FILE* file_ptr;
    file_ptr = fopen(filename, "r");
    // hold the values for a single line
    fread(content, sizeof(char), max_bytes_to_read, file_ptr);
    // cleaning up
    fclose(file_ptr);
}