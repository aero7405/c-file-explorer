#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "generate_page.h"
#include "file.h"
#include "http_response.h"

int generate_page(char **html, HTTP_Response *request)
{
    *html = (char *)malloc(HTML_MAX_SIZE);
    strcpy(*html, "");

    strcat(*html, "<!DOCTYPE html> \
                    <head> \
                        <title>Test Page</title> \
                        <link rel = 'stylesheet' href = 'css/styles.css'> \
                    </head> \
                    <body> \
                    <form> \
                        <input type = 'text' id = 'dir' name = 'dir'><br> \
                        <input type = 'submit' value = 'Submit'> \
                    </form>");

    char *curr_dir = get_param_from_query_string(request->query_string, "dir");
    // TODO: make sure to check that curr_dir != NULL

    // TODO: fill with page contents
    char **path_dirs = NULL;
    int path_dirs_len = get_paths_in_dir(&path_dirs, curr_dir);

    printf("%d paths found for dir \"%s\"\n", path_dirs_len, curr_dir);
    for (int i = 0; i < path_dirs_len; i++)
    {
        url_decode(path_dirs[i]);
        printf("%s\n", path_dirs[i]);
    }

    // TODO: add checking to ensure page never exceeds HTML_MAX_SIZE unless you want segfaults

    strcat(*html, "</body></html>");

    return strlen(*html);
}

int get_resource(char **response_buff, HTTP_Response *request)
{
    // if nothing is actually requested just return nothing
    if (request->method != HTTP_GET)
        return 0;

    // if html is requested generate page and return
    char *buff;
    int buff_size;
    if (request->accept == HTTP_HTML)
    {
        buff_size = generate_page(&buff, request);
    }
    else
    {
        // getting all non-HTML resources
        char prefix_dir[] = RESOURCES_DIR; // strcat need to concatinate variable not a macro :/
        buff_size = get_from_file(&buff, strcat(prefix_dir, request->resource_path));
    }
    // returning requested data
    *response_buff = buff;
    return buff_size;
}

int get_test_page_resource(char **response_buff, HTTP_Response *request)
{
    // if nothing is actually requested just return nothing
    if (request->method != HTTP_GET)
        return 0;

    // special case as "" is actually "index.html"
    if (strcmp(request->resource_path, "") == 0)
    {
        // resource_path should be 64 bytes so this is safe
        strcpy(request->resource_path, "index");
    }
    // special case for html files so we don't have to end pages in ".html"
    if (request->accept == HTTP_HTML)
    {
        char html_extension[6] = ".html";
        strcat(request->resource_path, html_extension);
    }
    // sending requested files
    char *buff;
    char prefix_dir[] = RESOURCES_DIR "test_page/";
    int buff_size = get_from_file(&buff, strcat(prefix_dir, request->resource_path));

    *response_buff = buff;
    return buff_size;
}