#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFSIZE 1024

void error_handling(char *message);
int binary_to_decimal(char *binary);

int main(int argc, char **argv)
{
    int serv_sock;
    int clnt_sock;

    int str_len, num = 0;

    char message[BUFSIZE + 1];

    struct sockaddr_in serv_addr;
    struct sockaddr_in clnt_addr;
    int clnt_addr_size;

    if (argc != 2)
    {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    serv_sock = socket(PF_INET, SOCK_STREAM, 0);

   if (serv_sock == -1)
        error_handling("socket() error");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    if (bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("bind() error");

    if (listen(serv_sock, 5) == -1)
        error_handling("listen() error");

    while (1)
    {
        clnt_addr_size = sizeof(clnt_addr);

        clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_size);
        if (clnt_sock == -1)
            error_handling("accept() error");        

        str_len = read(clnt_sock, message, sizeof(message));


        printf("처음\n%s\n", message);
        int message_length = strlen(message);
        char filename_length_binary[12];
        char filename[BUFSIZE];
        
        strncpy(filename_length_binary, message, 10);
        filename_length_binary[11] = '\0';
        
        int filename_length = binary_to_decimal(filename_length_binary);
        
        printf("%s %d\n", filename_length_binary, filename_length);
        
        strncpy(filename, message + 10, filename_length);
        filename[filename_length] = '\0';

        printf("%s\n", filename);

        FILE *fp = fopen(filename, "w");

        strcpy(message, message + 10 + filename_length);
        message[message_length - 10 - filename_length + 1] = '\0';
        printf("\n%s\n", message);
        fprintf(fp, message);
        fflush(fp);

        while ((str_len = read(clnt_sock, message, sizeof(message))) != 0)
        {
            printf("%d\n", str_len);
            message[str_len] = '\0';
            fprintf(fp, message);
            fflush(fp);
        }

        fclose(fp);
        close(clnt_sock);
    }

    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

int binary_to_decimal(char *binary) {
    int decimal = 0;
    int i, posValue = 512;
    for(i = 0; i < 10; i++) {
        if(binary[i] == '1')decimal += posValue;
        posValue /= 2;
    }
    return decimal;
}

/*
 * ./TCPserver 57632
 */