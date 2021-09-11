#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

// Maximum number of bytes that can transfer and receive.
#define MESSAGE_BUFFER 4096
#define PORT 8888 // port number to connect

// Global Data
char buffer[MESSAGE_BUFFER];
char message[MESSAGE_BUFFER];
int socket_file_descriptor, message_size;
socklen_t length;
const char *end_string = "end";
int quit_status;
struct sockaddr_in serveraddress, client;
extern int errno;

void processRequest()
{
    int status = 0;

    // Server Properties
    bzero(&serveraddress, sizeof(serveraddress));
   
    // Create a UDP Socket via PORT 8888
    // AF_INET --> IPv4
    // SOCK_DGRAM --> Connectionless (UDP)
    socket_file_descriptor = socket(AF_INET, SOCK_DGRAM, 0);

    if(socket_file_descriptor == -1)
    {
        printf("Socket creation failed! -- %s\n", strerror(errno));
        exit(1);
    }

    // htonl ---------> Converts unsigned integer from
    //                  host byte order to network byte 
    //                  order
    // INADDR_ANY ----> Used when we don't want to bind
    //                  a socket to any specific IP
    serveraddress.sin_addr.s_addr = htonl(INADDR_ANY);
   
    // Connecting via port 8888
    // htons ----> converts unsigned integer hostshort
    //             from hostbyte order to network byte
    //             order.
    serveraddress.sin_port = htons(PORT);
   
    // Protocol Family
    serveraddress.sin_family = AF_INET;
   
    // bind server address to socket descriptor
    bind(socket_file_descriptor, (struct sockaddr*)&serveraddress, sizeof(serveraddress));
   
    while(1)
    {
        // Calculating the Client Datagram length
        length = sizeof(client);
        message_size = recvfrom(socket_file_descriptor, 
                                buffer, 
                                sizeof(buffer), 
                                0, 
                                (struct sockaddr*)&client,
                                &length);
        if(message_size == -1)
        {
            printf("Something went wrong with recvfrom: %s\n", strerror(errno));
            goto cleanup;
        }
   
        buffer[message_size] = '\0';
        quit_status = strcmp(buffer, end_string);
        
        if(quit_status == 0)
        {
            printf("Server is Quitting\n");
            close(socket_file_descriptor);
            exit(0);
        }
       
        printf("Message Received from Client: %s\n",buffer);
       
        // sending the response to the client
        printf("Enter reply message to the client: ");
        status = scanf("%[^\n]%*c", message);

        if (status == -1)
        {
            printf("Failed to get message input from client: %s\n", strerror(errno));
            goto cleanup;
        }

        status = sendto(socket_file_descriptor, 
                        message, 
                        MESSAGE_BUFFER, 
                        0, 
                        (struct sockaddr*)&client, 
                        sizeof(client));

        if(status == -1)
        {
            printf("Failed to send data to the server: %s\n", strerror(errno));
            goto cleanup;
        }
        printf("Message Sent Successfully to the client: %s\n", message);
        printf("Waiting for the Reply from Client..!\n");
    }

cleanup:    
    
    // Closing the Socket File Descriptor.
    close(socket_file_descriptor);
}

int main() 
{
    printf("SERVER IS LISTENING THROUGH THE PORT: %d WITHIN A LOCAL SYSTEM\n", PORT);
   
    // Calling the process request function to process the client request and give the response.
    processRequest();
   
    return 0;
}
