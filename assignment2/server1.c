#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#define PORT 5000

char * evalute_expression(char *expression)
{
    int n = strlen(expression);
    char left[20];
    char right[20];
    char operator='\0';
    char *ans = (char *)malloc(sizeof(char) * 100);
    int answer;

    int l = 0;
    int r = 0;
    for(int i = 0 ; i < n ; i++)
    {
        if(operator == '\0' && expression[i] >= 48 && expression[i] <= 57)
        {
            left[l++] = expression[i];
        }else if(operator != '\0' && expression[i] >= 48 && expression[i] <= 57){
            right[r++] = expression[i];
        }else{
            operator = expression[i];
        }
    }
    left[l]='\0';
    right[r]='\0';
    //printf("%s %s", left, right);

    switch(operator)
    {
        case '+':
            answer = atoi(left) + atoi(right);
            sprintf(ans, "%d", answer);
            return ans;
        case '-':
            answer = atoi(left) - atoi(right);
            sprintf(ans, "%d", answer);
            return ans;
        case '*':
            answer = atoi(left) * atoi(right);
            sprintf(ans, "%d", answer);
            return ans;
        case '/':

            if(atoi(right) == 0){
                strcpy(ans,"division by zero not possible");
                return ans;
            }
            answer = atoi(left) / atoi(right);
            sprintf(ans, "%d", answer);
            return ans;
        default :
            strcpy(ans ,"Invalid operator");
            return ans;
    }
}

int create_listen(struct sockaddr_in address){
	int server_fd;
    int opt = 1;

	printf("Creating socket.......\n");
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    printf("Socket created\n");

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

	return server_fd;
}


int main(int argc, char const *argv[])
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char *ans;

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );

    server_fd = create_listen(address);

    while(1){

        printf("Wating for connection.......\n");
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        printf("Connection established\n");
        close(server_fd);

        while(read( new_socket , buffer, 1024) != 0){
            

            ans = evalute_expression(buffer);
            printf("sending answer of expression.....\n");
            send(new_socket , ans, strlen(ans) , 0 );
            printf("Answer sent\n");
            free(ans);
        }

        server_fd = create_listen(address);

    }
    return 0;
}