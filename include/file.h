#ifndef FILE_H
#define FILE_H

#include <stdbool.h>
#include <string.h>

#define PATH_STRING_LENGTH 256 // should be enough to contain any reasonable file path

#define IS_FOLDER(path) (path[strlen(path) - 1] == '/')

int get_from_file(char **, const char *);

bool is_directory(const char *);

bool is_only_periods(const char *);

int get_paths_in_dir(char ***, const char *);

void get_parent_dir(char *, const char *);

#endif