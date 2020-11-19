#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFSIZE 1024
#define FILESIZE 2000000

void error_handling(char *message);

int main(int argc, char **argv)
{

    char message[BUFSIZE];

    int sock;
    int str_len;
    struct sockaddr_in serv_addr;

    if (argc != 4)
    {
        printf("Usage : %s <IP> <port> <filename>\n", argv[0]);
        exit(1);
    }

    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1)
        error_handling("socket() error");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("connect() error");

    FILE *fp = fopen(argv[3], "r");

    write(sock, argv[3], strlen(argv[3]));

    while (fgets(message, sizeof(message), fp) != NULL)
    {
        printf("%s", message);
        write(sock, message, strlen(message));
    }

    fclose(fp);

    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

/*
./TCPclient 10.1.0.1 57632 file.txt
*/