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
    char *query_string;
} HTTP_Response;

typedef struct Query_String_Parameter
{
    char *key;
    char *value;
} Query_String_Parameter;

HTTP_Response get_response_data(char *, size_t);

char *get_param_from_query_string(const char *, const char *);

void url_decode(char *);

int get_query_string_parameters(Query_String_Parameter **, const char *);

#endif