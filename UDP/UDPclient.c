#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFSIZE 1024
void error_handling(char *message);

int main(int argc, char **argv)
{
    int sock;
    long long file_size = 0, sent_size = 0;
    char message[BUFSIZE];
    int str_len, addr_size, i;

    struct sockaddr_in serv_addr;
    struct sockaddr_in from_addr;

    if (argc != 4)
    {
        printf("Usage : %s <IP> <port> <filename>\n", argv[0]);
        exit(1);
    }

    sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (sock == -1)
        error_handling("UDP socket() error");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    printf("%s\n", argv[3]);

    sendto(sock, argv[3], strlen(argv[3]), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    str_len = recvfrom(sock, message, BUFSIZE, 0, (struct sockaddr *)&from_addr, &addr_size);
    message[str_len] = '\0';

    printf("%s\n", argv[3]);

    if (strcmp(argv[3], message))
        error_handling("filename transfer is failed.\n");

    FILE *fp = fopen(argv[3], "r");

    fseek(fp, 0, SEEK_END);
    file_size = ftell(fp);

    fseek(fp, 0, SEEK_SET);

    while (fgets(message, sizeof(message), fp) != NULL)
    {
        fseek(fp, 0, SEEK_CUR);
        sent_size = ftell(fp);
        printf("%lld / %lld\n", sent_size, file_size);

        sendto(sock, message, strlen(message), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

        str_len = recvfrom(sock, message, BUFSIZE, 0, (struct sockaddr *)&from_addr, &addr_size);
        message[str_len] = '\0';
    }

    fclose(fp);
    close(sock);

    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

/*
./UDPclient 10.1.0.1 57633 file.txt
*/