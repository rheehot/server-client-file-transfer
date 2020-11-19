#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFSIZE 1024
#define FILENAMESIZE 64

void error_handling(char *message);

int main(int argc, char **argv)
{
    int serv_sock;
    char message[BUFSIZE];
    int str_len, num = 0;

    struct sockaddr_in serv_addr;
    struct sockaddr_in clnt_addr;
    int clnt_addr_size;

    if (argc != 2)
    {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    serv_sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

    struct timeval optVal = {5, 0};
    int optLen = sizeof(optVal);

    setsockopt(serv_sock, SOL_SOCKET, SO_RCVTIMEO, &optVal, optLen);

    clnt_addr_size = sizeof(clnt_addr);
    if (serv_sock == -1)
        error_handling("UDP socket() error");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    if (bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("bind() error");

    char filename[FILENAMESIZE] = {0,};
    FILE *fp;

    while (1)
    {
        clnt_addr_size = sizeof(clnt_addr);

        str_len = recvfrom(serv_sock, message, sizeof(message), 0,
                           (struct sockaddr *)&clnt_addr, &clnt_addr_size);
       
        if(str_len == -1) {
            if(filename[0] != '\0') {
                filename[0] = '\0';
                fclose(fp);
            }
            continue;
        }

        message[str_len] = '\0';

        if(filename[0] == '\0') {
            strcpy(filename, message);
            fp = fopen(filename, "w");
        }

        else {
            fprintf(fp, message);
            fflush(fp);
        }

        sendto(serv_sock, message, str_len, 0, (struct sockaddr *)&clnt_addr, sizeof(clnt_addr));
    }

    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}