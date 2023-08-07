#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#include "http_response.h"
#include "file.h"

#pragma comment(lib, "ws2_32.lib")

#define PORT 80
#define LOCAL_HOST "127.0.0.1"
#define RESOURCES_DIR "resources/"

/*
 * NOTE:
 *  For now server.c is the main file as I work out running the server
 *  This will change in future as development shifts to focus on the file explorer aspects
 */
int main(void) {
    // getting test html from file
    char* html_buff;
    int html_buff_size = get_from_file(&html_buff, "resources/index.html");
    // printf(html_buff);
    // printf("\n\n"); // no new line at the end of file so we add extras

    // initialising 
    WSADATA wsa_data;
    WSAStartup(MAKEWORD(2,2), &wsa_data); // MAKEWORD(2,2) calls for v2.2 of winsock

    // establishing socket and catching invalid sockets
    SOCKET listener_skt = socket(AF_INET , SOCK_STREAM , 0); // listener socket
    if (listener_skt == INVALID_SOCKET) {
        printf("Could not create socket with error : %d\n", WSAGetLastError());
        // shouldn't continue if socket creation fails
        closesocket(listener_skt);
        WSACleanup();
        return 1;
    }
    // printf("Socket created.\n");

    // creating socket address
    const char* local_IP = LOCAL_HOST;
    struct sockaddr_in skt_addr;
    // Set up the sockaddr structure
    skt_addr.sin_family = AF_INET;
    skt_addr.sin_addr.s_addr = inet_addr(local_IP);
    skt_addr.sin_port = htons(PORT);
    // binding socket
    bind(listener_skt, (SOCKADDR*)&skt_addr, sizeof(skt_addr));

    // listen for oncomming connections
    if (listen(listener_skt, SOMAXCONN) == SOCKET_ERROR) { // listen to connections from a max sized queue
        printf("Failed listen function with error : %d\n", WSAGetLastError());
        // shouldn't continue if socket creation fails
        closesocket(listener_skt);
        WSACleanup();
        return 1;
    }
    printf("Socket created at http://%s:%d\n", local_IP, PORT);
    printf("Listening to socket...\n");

    // accepting connections for a little bit
    SOCKET accepted_skt; // handler socket
    for (;;) { // loop indefinitely
        accepted_skt = accept(listener_skt, NULL, NULL);

        // validating accepted socket
        if (accepted_skt == INVALID_SOCKET) {
            printf("Failed to accept socket with error : %d\n", WSAGetLastError());
            closesocket(accepted_skt);
            continue;
        }
        // printf("Accepted socket.\n");

        // recieveing requests from accepted socket
        char request_buff[1024]; // keep it in a big buffer for now
        int request_buff_size = recv(accepted_skt, request_buff, sizeof(request_buff), 0);
        // convert request_buff into usable data
        HTTP_Response request = get_response_data(request_buff, sizeof(request_buff));
        
        printf("\nHTTP Method: %d\n", request.method);
        printf("Accept: %d\n", request.accept);
        printf("Resource Path: %s\n", request.resource_path);

        // special case as "" is actually "index.html"
        if (strcmp(request.resource_path, "") == 0) { 
            // resource_path should be 64 bytes so this is safe
            strcpy(request.resource_path, "index");
        }
        // special case for html files so we don't have to end pages in ".html"
        if (request.accept == HTTP_HTML) {
            char html_extension[6] = ".html";
            strcat(request.resource_path, html_extension);
        }
        // checking http method
        if (request.method == HTTP_GET) {
            // sending requested files
            char* buff;
            char prefix_dir[] = RESOURCES_DIR;
            int buff_size = get_from_file(&buff, strcat(prefix_dir, request.resource_path));
            // checking valid buffer
            if (buff_size > 0) { // no errors encountered
                send(accepted_skt, buff, buff_size, 0);
            }
            else {
                printf("WARNING: Failed to get \"%s\".\n", request.resource_path);
            }
        }

        // closing socket
        closesocket(accepted_skt);
    }

    // socket cleanup
    closesocket(listener_skt);
    WSACleanup();

    // cleanup
    return 0;
}