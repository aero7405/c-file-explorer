#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "http_response.h"
#include "file.h"

// the index of the value in the keys matchs that in the values
char PERCENT_ENCODING_LOOKUP_KEYS[22][4] = {"%20", "%21", "%22", "%23", "%24", "%25", "%26", "%27", "%28", "%29", "%2A", "%2B", "%2C", "%2F", "%3A", "%3B", "%3D", "%3F", "%40", "%5B", "%5D"};
char PERCENT_ENCODING_LOOKUP_VALUES[22] = {' ', '!', '"', '#', '$', '%', '&', '\'', '(', ')', '*', '+', ',', '/', ':', ';', '=', '?', '@', '[', ']'};
void url_decode(char *string)
{
    char modified_string[PATH_STRING_LENGTH] = ""; // Shouldn't encounter a longer string than this
    int modified_string_len = 0;
    int i = 0;
    while (i < strlen(string))
    {
        if (string[i] == '%' && i + 2 < strlen(string)) // ensuring that there is enough space in original string to get a code
        {
            // https://en.wikipedia.org/wiki/Percent-encoding has a table showing all reserved characters and their encoding
            // getting the 4 digits that could be making up the code
            char code[4] = {'%', string[i + 1], string[i + 2], '\0'};
            int idx = 0;
            // searching lookup for code
            for (; idx < 22; idx++)
            {
                if (strcmp(PERCENT_ENCODING_LOOKUP_KEYS[idx], code) == 0)
                {
                    modified_string[modified_string_len] = PERCENT_ENCODING_LOOKUP_VALUES[idx];
                    break;
                }
            }
            // proceed as if no code was found if code not in lookup
            if (idx >= 22)
            {
                modified_string[modified_string_len] = '%';
                i++; // moving i past the code
            }
            else
                i += 3; // moving i past the code
        }
        else
        {
            modified_string[modified_string_len] = string[i];
            i++; // moving i to the next character
        }
        modified_string_len++; // only 1 character is appended so we increment by 1
    }
    modified_string[modified_string_len] = '\0';
    strcpy(string, modified_string);
}

char *get_param_from_query_string(const char *query_string, const char *param)
{
    // splitting query string into parameter and value pairs
    Query_String_Parameter *param_arr;
    int param_cnt = get_query_string_parameters(&param_arr, query_string);
    // searching param_arr for the desired parameter
    char *param_val = NULL;
    for (int i = 0; i < param_cnt; i++)
    {
        if (strcmp(param_arr[i].key, param) == 0)
        {
            param_val = param_arr[i].value;
            free(param_arr[i].key);
            break;
        }
        else
        {
            // cleanup if data is no longer needed
            free(param_arr[i].value);
            free(param_arr[i].key);
        }
    }
    if (param_cnt > 0) // only free if there is memory to free
        free(param_arr);
    return param_val;
}

// returns the number of parameters put into the array param_arr
int get_query_string_parameters(Query_String_Parameter **param_arr, const char *query_string)
{
    // creating copy of query_string because strtok may modify strings apparently
    char query_string_cpy[strlen(query_string)];
    strcpy(query_string_cpy, query_string);

    // if query string is empty just return 0 and do nothing
    if (strcmp(query_string_cpy, "") == 0)
        return 0;

    // counting number of tokens
    int param_cnt = 0;
    char *param_string = strtok(query_string_cpy, "&");
    while (param_string != NULL)
    {
        param_cnt++;
        param_string = strtok(NULL, "&"); // getting next token
    }

    // allocating memory for param_arr
    *param_arr = (Query_String_Parameter *)malloc(param_cnt * sizeof(Query_String_Parameter));
    // catching failed memory allocation
    if (*param_arr == NULL)
        return -1;

    // repairing copy of string
    strcpy(query_string_cpy, query_string);

    // populating param_arr
    param_string = strtok(query_string_cpy, "&");
    char param_string_cpy[PATH_STRING_LENGTH];
    char *param_buff = NULL;
    for (int i = 0; i < param_cnt; i++)
    {
        // strtok modifies the string so we must copy it
        strcpy(param_string_cpy, param_string);

        // getting index of first occurance of '='
        char *c_ptr = strchr(param_string_cpy, '=');
        int split_idx = (int)(c_ptr - param_string_cpy);

        // creating string for key
        param_buff = (char *)malloc(PATH_STRING_LENGTH * sizeof(char));
        // catching failed memory allocation
        if (param_buff == NULL)
            return -1;
        // getting key
        strncpy(param_buff, param_string_cpy, split_idx);
        param_buff[split_idx] = '\0'; // adding termination character because strncpy won't (lazy prick)
        // catching invalid query parameter format
        if (param_buff == NULL)
            return i; // returning number of parameters that were successfully populated
        (*param_arr)[i].key = param_buff;

        // creating string for value
        param_buff = (char *)malloc(PATH_STRING_LENGTH * sizeof(char));
        // catching failed memory allocation
        if (param_buff == NULL)
            return -1;
        // getting key
        strcpy(param_buff, param_string_cpy + split_idx + 1); // + 1 to more the pointer past the =
        // catching invalid query parameter format
        if (param_buff == NULL)
            return i; // returning number of parameters that were successfully populated
        (*param_arr)[i].value = param_buff;

        // getting next pair
        param_string = strtok(NULL, "&");
    }

    return param_cnt;
}

// WARNING: there are no catches for failed memory allocation written in this function
HTTP_Response get_response_data(char *response_buff, size_t response_buff_size)
{
    HTTP_Response response;
    // initialising with default values just in case not all values are assigned
    response.method = HTTP_NO_METHOD;
    response.accept = HTTP_NO_ACCEPT_TYPE;
    response.resource_path = NULL;
    response.query_string = NULL;

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
                    char c = resource_path[0];
                    token = strtok(resource_path, "?");
                    if (c != '?')
                        token = strtok(NULL, "?"); // getting 2nd term if path doesn't start with ?
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