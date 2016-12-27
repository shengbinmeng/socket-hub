#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUF_SIZE 1024

int main(int argc, char **argv)
{
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <remote_host> <remote_port>\n", argv[0]);
        exit(1);
    }

    char *host = argv[1];
    int port = atoi(argv[2]);

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        perror("Error on opening socket");
        return -1;
    }
    struct sockaddr_in address = {0};
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = inet_addr(host);

    char buf[BUF_SIZE];

    // Get ready.
    while (1) {
        sprintf(buf, "ready");
        size_t data_len = strlen(buf) + 1;
        ssize_t size = sendto(sockfd, buf, data_len, 0,
                              (struct sockaddr*)&address, sizeof(address));
        if (size == -1) {
            perror("Error on sending");
        } else {
            printf("%zd bytes sent\n", size);
            break;
        }
    }

    // Receive data.
    while (1) {
        bzero(buf, BUF_SIZE);
        ssize_t size = recvfrom(sockfd, buf, BUF_SIZE, 0, NULL, NULL);
        if (size == -1) {
            perror("Error on receiving");
        } else {
            // Use the data.
            printf("%zd bytes received\n", size);
            printf("CONTENT: %s\n", buf);
        }

        sleep(1);
    }

    return 0;
}
