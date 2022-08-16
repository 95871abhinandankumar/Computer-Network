#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#define PORT 5000
   
int main(int argc, char const *argv[])
{
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char *msg = "Hello, I am client";
    char buffer[1024] = {0};
    printf("Creating socket.......\n");
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }
    printf("Socket created\n");
   
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) 
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
   
    printf("Connecting to the server.......\n");
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }
    printf("Connected\n");

    printf("sending message.......\n");

    send(sock , msg , strlen(msg) , 0 );
    printf("message sent\n");
    valread = read( sock , buffer, 1024);
    printf("Message recieved from server : %s\n",buffer );
    return 0;
}