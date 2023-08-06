#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

#include <stdio.h>

typedef enum HTTP_Method {
    NONE,
    GET, 
    POST,
} HTTP_Method;

typedef enum HTTP_Accept_Types {
    TEXT,
    STYLESHEET,
    IMAGE,
    HTML,
    JAVASCRIPT,
} HTTP_Accept_Types;

typedef struct HTTP_Response {
    HTTP_Method method;
    HTTP_Accept_Types accept;
    char* resource_path;
} HTTP_Response;

HTTP_Response get_response_data(char*, size_t);

#endif