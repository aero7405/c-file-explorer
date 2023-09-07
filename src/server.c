#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#include "server.h"
#include "http_response.h"
#include "file.h"
#include "generate_page.h"

// This *was* needed at one point but linking with Make seems to have removed the need for it
// #pragma comment(lib, "ws2_32.lib")

// return of 0 is successful, else failed to run
int run_server()
{
    // initialising
    WSADATA wsa_data;
    WSAStartup(MAKEWORD(2, 2), &wsa_data); // MAKEWORD(2,2) calls for v2.2 of winsock

    // establishing socket and catching invalid sockets
    SOCKET listener_skt = socket(AF_INET, SOCK_STREAM, 0); // listener socket
    if (listener_skt == INVALID_SOCKET)
    {
        printf("Could not create socket with error : %d\n", WSAGetLastError());
        // shouldn't continue if socket creation fails
        closesocket(listener_skt);
        WSACleanup();
        return 1;
    }
    // printf("Socket created.\n");

    // creating socket address
    const char *local_IP = LOCAL_HOST;
    struct sockaddr_in skt_addr;
    // Set up the sockaddr structure
    skt_addr.sin_family = AF_INET;
    skt_addr.sin_addr.s_addr = inet_addr(local_IP);
    skt_addr.sin_port = htons(PORT);
    // binding socket
    bind(listener_skt, (SOCKADDR *)&skt_addr, sizeof(skt_addr));

    // listen for oncomming connections
    if (listen(listener_skt, SOMAXCONN) == SOCKET_ERROR)
    { // listen to connections from a max sized queue
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
    for (;;)
    { // loop indefinitely
        accepted_skt = accept(listener_skt, NULL, NULL);

        // validating accepted socket
        if (accepted_skt == INVALID_SOCKET)
        {
            printf("Failed to accept socket with error : %d\n", WSAGetLastError());
            closesocket(accepted_skt);
            continue;
        }

        // recieveing requests from accepted socket
        char request_buff[1024]; // keep it in a big buffer for now
        recv(accepted_skt, request_buff, sizeof(request_buff), 0);
        // convert request_buff into usable data
        HTTP_Response request = get_response_data(request_buff, sizeof(request_buff));

        // sending requested files
        char *buff = NULL;
        int buff_size = get_resource(&buff, &request);
        // checking valid buffer
        if (buff_size > 0)
        { // no errors encountered
            send(accepted_skt, buff, buff_size, 0);
            // only try to free if memory was allocated
            free(buff);
        }
        else
        {
            printf("WARNING: Failed to get \"%s\".\n", request.resource_path);
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