#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "http_response.h"
#include "file.h"

// returns the number of parameters put into the array parameter_arr
int get_query_string_parameters(Query_String_Parameter **parameter_arr, char *query_string)
{
    // if query string is empty just return 0 and do nothing
    if (strcmp(query_string, "") == 0)
        return 0;

    // counting number of tokens
    int parameter_cnt = 0;
    char *parameter_string = strtok(query_string, "&");
    while (parameter_string != NULL)
    {
        parameter_cnt++;
        parameter_string = strtok(NULL, "&"); // getting next token
    }

    // allocating memory for parameter_arr
    *parameter_arr = (Query_String_Parameter *)malloc(parameter_cnt * sizeof(Query_String_Parameter));
    // catching failed memory allocation
    if (*parameter_arr == NULL)
        return -1;

    // populating parameter_arr
    parameter_string = strtok(query_string, "&");
    int i = 0;
    char *parameter_buff;
    for (; i < 2; i++)
    {
        // creating string for key
        parameter_buff = (char *)malloc(PATH_STRING_LENGTH * sizeof(char));
        strcpy(parameter_buff, "test");
        (*parameter_arr)[i].key = parameter_buff;
        // TODO: split parameter_string into key and value componenets
        // getting next token
        parameter_string = strtok(NULL, "&");
        i++;
    }

    return parameter_cnt;
}

HTTP_Response get_response_data(char *response_buff, size_t response_buff_size)
{
    HTTP_Response response;
    // initialising with default values just in case not all values are assigned
    response.method = HTTP_NO_METHOD;
    response.accept = HTTP_NO_ACCEPT_TYPE;
    response.resource_path = NULL;

    // iterate over buffer and find relevant data
    int i = 0;
    char curr_line_buff[1024] = ""; // big buffer
    while (response_buff[i] != '\0' && i < response_buff_size)
    {
        // dividing into lines
        if (response_buff[i] != '\n')
        {
            if (strlen(curr_line_buff) >= 1024)
            { // case we are adding outside of buffer
                // throwing a fit because no line of text should be more than a kilobyte
                printf("Response line read exceeded 1024 bytes!");
                break;
            }
            curr_line_buff[strlen(curr_line_buff)] = response_buff[i];
            // curr_line_buff[strlen(curr_line_buff)] = '\0'; // adding new terminating character because we just overrode the existing one
        }
        else
        { // new line has been completed
            // processing collected line
            char buff[8] = ""; // shouldn't need more than 8 given we are comparing only a single term
            // splitting string with spaces and colons as delimiter
            int j = 0;
            while (curr_line_buff[j] != ' ' && j < 7)
            {
                buff[j] = curr_line_buff[j];
                j++;
            }
            buff[j] = '\0'; // ending buff at delimiter

            // checking if first term is a relevent action
            if (strcmp(buff, "GET") == 0)
            {
                response.method = HTTP_GET;
                // printf("->GET\n");
                // allocating memory for paths
                char *resource_path = (char *)malloc(PATH_STRING_LENGTH * sizeof(char));
                strcpy(resource_path, "");
                char *query_string = (char *)malloc(PATH_STRING_LENGTH * sizeof(char));
                strcpy(query_string, "");
                // splits path at ' '
                char *token = strtok(curr_line_buff, " ");
                token = strtok(NULL, " "); // getting 2nd term
                if (token != NULL)
                {
                    strcpy(resource_path, token + 1);
                    // checking if resource path can be split at '?'
                    token = strtok(resource_path, "?");
                    token = strtok(NULL, "?"); // getting 2nd term
                    if (token != NULL)
                        strcpy(query_string, token);
                }
                // updating struct to the resource path
                response.resource_path = resource_path;
                response.query_string = query_string;
            }
            else if (strcmp(buff, "POST") == 0)
            {
                // Do nothing here for now as we don't really need to post yet
                response.method = HTTP_POST;
                printf("WARNING: POST branch in function get_response_data is unimplemented!\n");
            }
            else if (strcmp(buff, "Accept:") == 0)
            {
                // in theory this occurs for every response that we ever have to deal with
                // printf("->Accept\n");
                // printf(curr_line_buff);
                // printf("\n");
                // fields are comma seperated and order is not important *sigh*
                // seperating at commas
                char *token = strtok(curr_line_buff + 8, ","); // +8 should push past "Accept: "
                // anything except html is a resouce as far as I care
                response.accept = HTTP_RESOURCE;
                while (token != NULL)
                {
                    // checking if previous token was html (as this is out only special case as of now)
                    if (strcmp(token, "text/html") == 0)
                    {
                        response.accept = HTTP_HTML;
                        break;
                    }
                    // getting next token
                    token = strtok(NULL, ",");
                }
            }

            // clearing curr_line_buff
            for (j = 0; j < 1024; j++)
            { // for some reason we need to reset every element
                curr_line_buff[j] = '\0';
            }
        }
        i++;
    }

    return response;
}