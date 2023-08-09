#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "http_response.h"

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
                // getting the path of the resource to get
                // need to malloc otherwise the string itself will go out of scope
                char *resource_path = (char *)malloc(64);      // 64 is probably enough for file paths
                strcpy(resource_path, curr_line_buff + 4 + 1); // + 1 to not include a /
                // adding termination character at first space
                *strchr(resource_path, ' ') = '\0';
                // updating struct to the resource path
                response.resource_path = resource_path;
            }
            else if (strcmp(buff, "POST") == 0)
            {
                // Do nothing here for now as we don't really need to post yet
                response.method = HTTP_POST;
                printf("WARNING: POST branch in get_response_data is unimplemented!\n");
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