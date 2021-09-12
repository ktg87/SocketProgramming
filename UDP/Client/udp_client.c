#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/socket.h> // To work with socket programming
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>

// Maximum number of bytes that can transfer and receive.
#define MAX_MSG_SIZE 4096
#define PORT 8888 // port number to connect

// Global Data
char buffer[MAX_MSG_SIZE];
char message[MAX_MSG_SIZE];
int socket_file_descriptor, n;
int size = 0;
int quit_status;
const char *end_string = "end";
struct sockaddr_in serveraddress;

void sendRequest()
{
    extern int errno;
    int status = 0;

    // Setting the properties to connect with Server
    bzero(&serveraddress, sizeof(serveraddress));
   
    // Working with Localhost Address
    serveraddress.sin_addr.s_addr = inet_addr("127.0.0.1");
   
    // Connecting via port 8888
    // htons ----> converts unsigned integer hostshort
    //             from hostbyte order to network byte
    //             order.
    serveraddress.sin_port = htons(PORT);
   
    // Protocol Family
    serveraddress.sin_family = AF_INET;
   
    // creating the datagram socket
    // AF_INET --> IPv4
    // SOCK_DGRAM --> Connectionless (UDP)
    // 0 --> Internet Protocol
    socket_file_descriptor = socket(AF_INET, SOCK_DGRAM, 0);
   
    // Establishing a connection with the server.
    if(connect(socket_file_descriptor, (struct sockaddr *)&serveraddress, sizeof(serveraddress)) < 0) 
    {
        printf("\n Something went wrong Connection Failed: %s \n", strerror(errno));
        exit(1);
    }
   
    while(1)
    {
        printf("Enter a message you want to send to the server: ");
        scanf("%[^\n]%*c", message);
        quit_status = strcmp(message, end_string);
        if(quit_status == 0)
        {
            status = sendto(socket_file_descriptor, message, MAX_MSG_SIZE, 0, (struct sockaddr*)NULL, sizeof(serveraddress));
            if(status == -1)
            {
                printf("Something went wrong when sending the message to the server: %s\n", strerror(errno));
                goto cleanup;
            }
            printf("Client work is done.!\n");
            close(socket_file_descriptor);
            exit(0);
        }else
        {
            status = sendto(socket_file_descriptor, message, MAX_MSG_SIZE, 0, (struct sockaddr*)NULL, sizeof(serveraddress));
            if (status == -1)
            {
                printf("Something went wrong when sending the message to the server: %s\n", strerror(errno));
                goto cleanup;
            }
            printf("Message sent successfully to the server: %s\n", message);
            printf("Waiting for the Response from Server..!\n");
        }
       
        printf("Message Received From Server: ");
        status = recvfrom(socket_file_descriptor, buffer, sizeof(buffer), 0, (struct sockaddr*)NULL, NULL);
        if (status == -1)
        {
            printf("Something went wrong when trying to receive data from the server: %s\n", strerror(errno));
            goto cleanup;
        }

        printf("%s\n", buffer);
    }

cleanup:    
    // closing the Socket File Descriptor
    close(socket_file_descriptor);
}

int main() 
{
    printf("CLIENT IS ESTABLISHING A CONNECTION WITH SERVER THROUGH PORT: %d\n", PORT);
    
    // Calling the Send Request to send a request to the server.
    sendRequest();
   
    return 0;
}
