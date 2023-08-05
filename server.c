#include <stdio.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 80
#define LOCAL_HOST "127.0.0.1"
#define MAX_READ_LINE_LENGTH 128

// takes a filename and lines to read, returns a string with read data
// reallocates memory for content so the output fits perfectly within
void get_text_from_file(char* content, unsigned int max_bytes_to_read, const char* filename) {
    // reading the html file for testing
    FILE* file_ptr;
    file_ptr = fopen(filename, "r");
    // hold the values for a single line
    char line[MAX_READ_LINE_LENGTH]; 
    // iterating over file to read all lines
    while (fgets(line, MAX_READ_LINE_LENGTH, file_ptr)) {
        // checking if line will fit into content
        if (strlen(content) + strlen(line) + 1 < max_bytes_to_read) { // +1 for termination character
            strcat(content, line);
        }
        else {
            // fill array but no more
            char buff[max_bytes_to_read];
            strcpy(buff, "");
            int bytes_to_read = max_bytes_to_read - strlen(content) - 1;
            strncpy(buff, line, bytes_to_read);
            strcat(content, buff);

            break; // no more space in char array
        }
    };
    // cleaning up
    fclose(file_ptr);
}

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
    printf("Socket created.\n");

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
    printf("Socket established at %s:%d\n", local_IP, PORT);
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
        char request_buff[MAX_READ_LINE_LENGTH * 128]; // keep it in a big buffer for now
        int request_buff_size = recv(accepted_skt, request_buff, sizeof(request_buff), 0);
        // reading buffer
        printf("\n~~~~~~~~~~ REQUEST BUFFER ~~~~~~~~~~\n");
        printf("%s", request_buff); // TODO: do something with this data maybe?

        // closing socket
        closesocket(accepted_skt);
    }

    // socket cleanup
    closesocket(listener_skt);
    WSACleanup();

    // cleanup
    free(content);

    return 0;
}