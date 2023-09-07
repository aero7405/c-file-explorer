#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "generate_page.h"
#include "file.h"
#include "http_response.h"
#include "server.h"

int generate_page(char **html, HTTP_Response *request)
{
    *html = (char *)malloc(HTML_MAX_SIZE);
    strcpy(*html, "");

    // inserting header for page
    strncat(*html,
            "<!DOCTYPE html>"
            "<head>"
            "  <title>Test Page</title>"
            "  <link rel = 'stylesheet' href = 'css/styles.css'>"
            "</head>"
            "<body>"
            "  <form>"
            "    <label id = 'dir'>Move to directory:</label>"
            "    <input type = 'text' id = 'dir' name = 'dir'>"
            "    <input type = 'submit' value = 'Submit'>"
            "  </form>",
            HTML_MAX_SIZE - strnlen(*html, HTML_MAX_SIZE));

    // getting directory to search and decoding any percent encoding
    char *curr_dir = get_param_from_query_string(request->query_string, "dir");
    if (curr_dir == NULL)
    {
        curr_dir = (char *)malloc((strlen(DEFAULT_DIR) + 1) * sizeof(char));
        if (curr_dir == NULL)
            return -1; // memory allocation failed
        strcpy(curr_dir, DEFAULT_DIR);
    }
    // blank should also navigate to default
    if (strcmp(curr_dir, "") == 0)
        strcpy(curr_dir, DEFAULT_DIR);
    url_decode(curr_dir);

    // getting results at path
    char **path_dirs = NULL;
    int path_dirs_len = get_paths_in_dir(&path_dirs, curr_dir);

    // inserting back link
    char path[PATH_STRING_LENGTH];

    // inserting found paths into page
    for (int i = 0; i < path_dirs_len; i++)
    {
        if (path_dirs[i][strnlen(path_dirs[i], PATH_STRING_LENGTH) - 1] == '/') // only make path a link if it is a directory
            snprintf(path, PATH_STRING_LENGTH, "<div class = 'path'><a href = 'http://%s/?dir=%s'>%s</a></div>", LOCAL_HOST, path_dirs[i], path_dirs[i]);
        else
            snprintf(path, PATH_STRING_LENGTH, "<div class = 'path'>%s<br></div>", path_dirs[i]);
        strncat(*html, path, HTML_MAX_SIZE - strnlen(*html, HTML_MAX_SIZE));
    }

    // cleaning up html
    strncat(*html, "</body></html>", HTML_MAX_SIZE - strnlen(*html, HTML_MAX_SIZE));

    // cleaning up
    free(curr_dir);

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