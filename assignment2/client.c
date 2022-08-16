#include <stdio.h>
#include<stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

   
int main(int argc, char const *argv[])
{
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char expression[50];
    char buffer[1024] = {0};
    int PORT;
    char *ip_address;

    if(argc == 3)
    {
        printf("%s %s %s\n", argv[0], argv[1], argv[2]);
        ip_address= (char *)argv[1];
        PORT = atoi(argv[2]);

    }else{
        printf("Use the format ./client <Ip-Address> <port>\n");
        exit(0);
    }

    printf("Creating socket.......\n");
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }
    printf("Socket created\n");
   
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if(inet_pton(AF_INET, ip_address, &serv_addr.sin_addr)<=0) 
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
   
    printf("Connecting to the server.......\n");
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nServer is busy try after some time\n");
        return -1;
    }
    printf("Connected\n");

    while(1){
        printf("Enter the arithmetic expression : ");
        scanf("%s", expression);

        send(sock , expression , strlen(expression) , 0);
        valread = read( sock , buffer, 1024);
        printf("Output : %s\n", buffer );
        memset(buffer, 0, 1024);
    }
}