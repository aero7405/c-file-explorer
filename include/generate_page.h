#ifndef GENERATE_HTML_H
#define GENERATE_HTML_H

#include "http_response.h"

#define RESOURCES_DIR "resources/"

#define HTML_MAX_SIZE 2048

int get_resource(char **, HTTP_Response *);

int get_test_page_resource(char **, HTTP_Response *);

#endif