#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>

#define MESSAGE_LENGTH 1024 // Maximum number of data that can transfer
#define PORT 8888 // port number to connect
#define MAX_CONNECTIONS 5

struct sockaddr_in serveraddress, client;
socklen_t length;
int socket_file_descriptor, connection, bind_status, connection_status;

char message[MESSAGE_LENGTH];
extern int errno;

int main()
{
    // Creating the Socket
    // Notes:
    // AF_INET --> IPv4
    // SOCK_STREAM --> Connection-Oriented socket (TCP)
    // 0 --> Internet Protocol
    socket_file_descriptor = socket(AF_INET, SOCK_STREAM, 0);
    
    // If the socket is unable to be created, show the error and exit
    if(socket_file_descriptor == -1)
    {
        printf("Socket creation failed! -- %s\n", strerror(errno));
        exit(1);
    }
   
    // Erases the memory of the server address object that is being created
    bzero(&serveraddress, sizeof(serveraddress));
   
    // Server Properties - Notes:
    // htonl -------> Converts unsigned integer from host 
    //                byte order to network byte order
    // INADDR_ANY --> Used when we don't want to bind 
    //                a socket to any specific IP
    serveraddress.sin_addr.s_addr = htonl(INADDR_ANY); 
    
    // Setting the port number
    serveraddress.sin_port = htons(PORT);
   
    // Protocol family
    serveraddress.sin_family = AF_INET;
    
    // Binding the newly created socket with the given Ip Address
    bind_status = bind(socket_file_descriptor, (struct sockaddr*)&serveraddress, sizeof(serveraddress));
   
    if(bind_status == -1){
        printf("Socket binding failed! -- %s\n", strerror(errno));
        exit(1);
    }
   
    // Server is listening to establish a connection
    connection_status = listen(socket_file_descriptor, MAX_CONNECTIONS);
   
    // If the server is unable to listen for a connection, it should exit
    if(connection_status == -1){
        printf("Socket is unable to listen for new connections! -- %s\n", strerror(errno));
        exit(1);
    }else{
        printf("Server is listening for new connection: \n");
    }
   
    // Server is accepting data sent from the client
    length =  sizeof(client);
    connection = accept(socket_file_descriptor, (struct sockaddr*)&client, &length);
   
    // Server exits if there are any errors
    if(connection == -1){
        printf("Server is unable to accept the data from client! -- %s\n", strerror(errno));
        exit(1);
    }

    // Communication is established and an infinite loop is 
    // used to receive data from the client and to send repsonses 
    // to the client.
    //
    // When the client sends 'end' both the client and server are terminated
    while(1){
        bzero(message, MESSAGE_LENGTH);
        read(connection, message, sizeof(message));
       
        if (strncmp("end", message, 3) == 0) 
        {
            printf("Client Exited.\n");
            printf("Server is Exiting..!\n");
            break;
        }
       
        printf("Data received from client: %s\n", message);
        bzero(message, MESSAGE_LENGTH);
        printf("Enter the message you want to send to the client: ");
        scanf("%[^\n]%*c", message);
        
        // Sending the data to the server
        ssize_t bytes = write(connection, message, sizeof(message));
       
        // If the number of bytes is >= 0 then the data is sent successfully
        if(bytes >= 0){
            printf("Data successfully sent to the client!\n");
        }
    }
   
    // Closing the Socket Connection
    close(socket_file_descriptor);
   
    return 0;
}
