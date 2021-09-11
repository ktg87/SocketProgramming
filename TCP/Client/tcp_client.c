#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

// Maximum number of data that can transfer
#define MESSAGE_LENGTH 1024
#define PORT 8888 // port number to connect

// Global Data
int socket_file_descriptor, connection, status;
struct sockaddr_in serveraddress, client;
char message[MESSAGE_LENGTH];
extern int errno;

int main()
{
    // Socket Creation
    // AF_INET --> IPv4
    // SOCK_STREAM --> Connection-Oriented
    // 0 -> Internet Protocol
    socket_file_descriptor = socket(AF_INET, SOCK_STREAM, 0);
   
    if(socket_file_descriptor == -1)
    {
        printf("Creation of Socket failed! -- %s\n", strerror(errno));
        exit(1);
    }
    
    //Just so we can see how this compares to the TCP server
    printf("Socket File Descriptor: %d\n", socket_file_descriptor);

    // Erases the memory
    bzero(&serveraddress, sizeof(serveraddress));
   
    // Setting the Server Properties
    serveraddress.sin_addr.s_addr = inet_addr("127.0.0.1");
   
    // Setting the port number
    // Converts the unsigned short integer
    // hostshort from host byte order to
    // network byte order
    serveraddress.sin_port = htons(PORT);
   
    // Protocol family
    serveraddress.sin_family = AF_INET;
   
    // Establishing the Connection with server
    connection = connect(socket_file_descriptor, (struct sockaddr*)&serveraddress, sizeof(serveraddress));
   
    if(connection == -1)
    {
        printf("Connection with the server failed! -- %s\n", strerror(errno));
        exit(1);
    }
   
    // Interacting with the server
    while(1)
    {
        bzero(message, sizeof(message));
        printf("Enter the message you want to send to the server: ");
        scanf("%[^\n]%*c", message);
        
        if ((strncmp(message, "end", 3)) == 0) 
        {
            write(socket_file_descriptor, message, sizeof(message));
            printf("Client Exit.\n");
            break;
        }
       
        // Sending the data to the server by storing the number of bytes transferred in bytes variable
        ssize_t bytes = write(socket_file_descriptor, message, sizeof(message));
       
        // If the number of bytes is >= 0 then the data is sent successfully
        if(bytes >= 0)
        {
            printf("Data transmitted to TCP server: %zd\n", bytes);
        }
       
        bzero(message, sizeof(message));
       
        // Reading the response from the server.
        status = read(socket_file_descriptor, message, sizeof(message));

        if(status == -1)
        {
            printf("Failed to read data from TCP Server: %s", strerror(errno));
            exit(1);
        }
       
        printf("Data received from server: %s\n", message);
    }
    // Closing the Socket Connection
    close(socket_file_descriptor);
   
    return 0;
}
