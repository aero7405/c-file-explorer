#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

#include <stdio.h>

#include "file.h"

typedef enum HTTP_Method
{
    HTTP_NO_METHOD,
    HTTP_GET,
    HTTP_POST,
} HTTP_Method;

typedef enum HTTP_Accept_Types
{
    HTTP_NO_ACCEPT_TYPE,
    HTTP_HTML,
    HTTP_RESOURCE,
} HTTP_Accept_Types;

typedef struct HTTP_Response
{
    HTTP_Method method;
    HTTP_Accept_Types accept;
    char *resource_path;
    char *query_strings;
} HTTP_Response;

HTTP_Response get_response_data(char *, size_t);

#endif