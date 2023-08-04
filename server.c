#include <stdio.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib") // Winsock Library boilerplate

#define PORT 80
#define MAX_READ_LINE_LENGTH 1024

// takes a filename and lines to read, returns a string with read data
const char* get_text_from_file(char* content, const char* filename, int max_lines_to_read) {
    // reading the html file for testing
    FILE* file_ptr;
    file_ptr = fopen(filename, "r");
    // hold the values for a single line
    char line[MAX_READ_LINE_LENGTH]; 
    // iterating over file to read all lines
    while (fgets(line, MAX_READ_LINE_LENGTH, file_ptr)) {
        strcat(content, line);
    }
    return content;
} 

int main(void) {

    // getting test html from file
    char content[MAX_READ_LINE_LENGTH * 128];
    get_text_from_file(content, "html/Home.html", 128);
    // printf(content);
    // printf("\n\n"); // no new line at the end of file

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
    printf("Socket created.\n");

    // creating socket address
    const char* local_IP = "127.0.0.1";
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
        printf("Accepted socket.\n");

        // sending data over accepted socket
        send(accepted_skt, content, sizeof(content), 0);

        // closing socket
        closesocket(accepted_skt);
    }

    // socket cleanup
    closesocket(listener_skt);
    WSACleanup();

    // cleanup
    return 0;
}