#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>


#define BUFSIZE 1024

void error_handling(char *message);
void add_binary_to_filename(char *message);

int main(int argc, char **argv)
{

    char message[BUFSIZE];

    int sock;
    int str_len;
    long long file_size = 0, sent_size = 0;
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

    char filename[BUFSIZE];
    strcpy(filename, argv[3]);

    FILE *fp = fopen(filename, "r");

    add_binary_to_filename(filename);
    // printf("%s\n", filename);
    write(sock, filename, strlen(filename));
    
    fseek(fp, 0, SEEK_END);
    file_size = ftell(fp);

    fseek(fp, 0, SEEK_SET);

    while (fgets(message, sizeof(message), fp) != NULL)
    {
        sent_size = ftell(fp);
        printf("%lld / %lld\n", sent_size, file_size);
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

void add_binary_to_filename(char *message) 
{
    int count = strlen(message);
    char binary[BUFSIZE] = "0000000000";
    int divider = 512;
    int i;
    for(i = 0; i < 10; i++) 
    {
        if(count / divider) 
        {
            binary[i] = '1';
            count %= divider; 
        }
        
        divider /= 2;
    }
    strcpy(message, strcat(binary, message));
}

/*
./TCPclient 10.1.0.1 57632 file.txt
*/