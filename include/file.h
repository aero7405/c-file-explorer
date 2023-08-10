#ifndef FILE_H
#define FILE_H

#include <stdbool.h>

int get_from_file(char **, const char *);

bool is_directory(const char *);

bool is_only_periods(const char *);

int get_paths_in_dir(char ***, const char *);

#endif