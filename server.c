#include <stdio.h>
#include <string.h>
#include <winsock2.h>

#include "http_response.h"
#include "file.h"

#pragma comment(lib, "ws2_32.lib")

#define PORT 80
#define LOCAL_HOST "127.0.0.1"

int main(void) {
    // getting test html from file
    char content[1024] = ""; // this should be a big enough buffer, defaulting to empty because we are appending
    get_text_from_file(content, 1024, "html/Home.html");
    // printf(content);
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

        // sending data over accepted socket
        send(accepted_skt, content, sizeof(content), 0);

        // recieveing requests from accepted socket
        char request_buff[1024]; // keep it in a big buffer for now
        int request_buff_size = recv(accepted_skt, request_buff, sizeof(request_buff), 0);
        // convert request_buff into usable data
        HTTP_Response request = get_response_data(request_buff, sizeof(request_buff));
        // TEMP: printing request
        printf("\nHTTP Method: %d\n", request.method);
        printf("Accept: %d\n", request.accept);
        printf("Resource Path: %s\n", request.resource_path);
        // TODO: send resources requested

        // closing socket
        closesocket(accepted_skt);
    }

    // socket cleanup
    closesocket(listener_skt);
    WSACleanup();

    // cleanup
    return 0;
}