#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>

#include "server.h"
#include "file.h"
#include "http_response.h"

int main(void)
{
    run_server();

    return 0;
}