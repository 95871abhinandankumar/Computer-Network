#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <errno.h>

#define MAXLINE 256
#define LISTENQ 3
#define TRUE   1  
#define FALSE  0  

#define COLOR_RED 	  "\x1B[31m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_RESET   "\x1b[0m"

char * evalute_expression(char *expression)
{
    int n = strlen(expression);
    char left[20];
    char right[20];
    int operator='\0';
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

int create_listen(struct sockaddr_in serv_addr){
	int listenfd;

	if ((listenfd = socket (AF_INET, SOCK_STREAM, 0)) <0) {
		fprintf(stderr, "ERROR opening socket\n");
		exit(1);
	}

	if (bind (listenfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) <0) {
		fprintf(stderr, "ERROR binding\n");
		exit(1);
	}

	if (listen (listenfd, LISTENQ) <0) {
		fprintf(stderr, "ERROR listening\n");
		exit(1);
	}

	return listenfd;
}

int main (int argc, char *argv[])
{
	int listenfd, connfd, n, PORT_NUM, opt = TRUE;
	int client_socket[10], max_clients = 10, activity;   
    int i, sd, max_sd;
	char *answer;
	socklen_t cli_len;
	char buf[MAXLINE];
	struct sockaddr_in cli_addr, serv_addr;
	fd_set readfds;

	if (argc < 2) {
		fprintf(stderr,"usage %s <port>\n",argv[0]);
		exit(0);
	}
	PORT_NUM = atoi(argv[1]);

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port =  htons(PORT_NUM);
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	/*initialize all client_socket[] to 0 so not checked */ 
    for (i = 0; i < max_clients; i++)   {   
    	client_socket[i] = 0;   
    }

	listenfd = create_listen(serv_addr);

	printf("%s\n","Server running...waiting for connections.");
	cli_len = sizeof(cli_addr);

	while(1) {
        FD_ZERO(&readfds);   
        FD_SET(listenfd, &readfds);   
        max_sd = listenfd;

        for(i = 0; i < max_clients; ++i)   {   
            sd = client_socket[i];     
            if(sd > 0) FD_SET( sd , &readfds);                    
            if(sd > max_sd) max_sd = sd;   
        }

		/*wait for an activity on one of the sockets, timeout is NULL, so wait indefinitely */
        activity = select(max_sd + 1 , &readfds , NULL , NULL , NULL);
        if ((activity < 0) && (errno!=EINTR))   
        {   
            printf("select error");   
        }  

		/*If something happened on the master socket ,then its an incoming connection*/
		if (FD_ISSET(listenfd, &readfds))   
        {     
            if ((connfd = accept (listenfd, (struct sockaddr *) &cli_addr, &cli_len)) < 0) { 
				perror("server acccept failed"); 
				exit(0); 
			}  
            else printf("%s%s%d%s\n",COLOR_GREEN,"Connected with client socket number ",connfd,COLOR_RESET);
                 
            for (i = 0; i < max_clients; i++)   {   
                if( client_socket[i] == 0 )   {   
                    client_socket[i] = connfd;   
                    printf("Adding to list of sockets as %d\n" , i);          
                    break;   
                }   
            }   
        }
		
        for (i = 0; i < max_clients; i++) {   
            sd = client_socket[i];   
            if (FD_ISSET( sd , &readfds)){   
                if ((n = recv(sd, buf, MAXLINE, 0)) == 0){   
                    getpeername(sd, (struct sockaddr*)&cli_addr, &cli_len);   
					printf("%sDisconnected client %d : socket no. %d, ip %s, port %d%s\n",COLOR_RED,i,sd, inet_ntoa(cli_addr.sin_addr) , ntohs(cli_addr.sin_port),COLOR_RESET);   
                    close(sd);   
                    client_socket[i] = 0;   
                }
                else {   
                    printf("Client socket %d sent message: %s",sd,buf);
					answer = evalute_expression(buf);
					bzero(buf,MAXLINE);
					
					sprintf(buf,"%s",answer);
					if ((n = send(sd,buf,MAXLINE,0)) < 0){
						fprintf(stderr, "ERROR reading from socket\n");
						exit(1);
					}
					printf("Sending reply: %s\n",buf);
                }   
            }   
        }
	}
	exit(0);
}
