#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define BUF_SIZE 1024

static int create_socket(int port)
{
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        perror("Error on opening socket");
        return -1;
    }
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) != 0) {
        perror("Error on binding");
        return -1;
    }
    return sockfd;
}

int main(int argc, char **argv)
{
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <left_port> <right_port>\n", argv[0]);
        exit(1);
    }

    int left_port = atoi(argv[1]);
    int right_port = atoi(argv[2]);

    int left_socket = create_socket(left_port);
    if (left_socket == -1) {
        fprintf(stderr, "Create socket failed with left port:%d\n", left_port);
        exit(1);
    }
    int right_socket = create_socket(right_port);
    if (right_socket == -1) {
        fprintf(stderr, "Create socket failed with right port:%d\n", right_port);
        exit(1);
    }

    fprintf(stdout, "Waiting at left port:%d, right port:%d\n", left_port, right_port);

    char left_buf[BUF_SIZE], right_buf[BUF_SIZE];
    struct sockaddr_in left_addr = {0}, right_addr = {0};
    socklen_t left_addr_len = sizeof(left_addr), right_addr_len = sizeof(right_addr);
    int right_ready = 0, left_ready = 0;
    char left_host[NI_MAXHOST], left_serv[NI_MAXSERV];
    char right_host[NI_MAXHOST], right_serv[NI_MAXSERV];

    while (1) {
        if (right_ready == 0) {
            bzero(right_buf, BUF_SIZE);
            ssize_t size = recvfrom(right_socket, right_buf, BUF_SIZE, 0,
                               (struct sockaddr *) &right_addr, &right_addr_len);
            if (size == -1) {
                perror("Error when receive from right");
            } else {
                int error = getnameinfo((struct sockaddr *) &right_addr,
                                        right_addr_len, right_host, sizeof(right_host),
                                        right_serv, sizeof(right_serv),
                                        NI_NUMERICHOST | NI_NUMERICSERV);
                if (error != 0) {
                    perror(gai_strerror(error));
                } else {
                    printf("%zd bytes received from %s:%s\n", size, right_host, right_serv);
#ifdef DEBUG
                    char content[64];
                    snprintf(content, sizeof(content), "0x%02x 0x%02x 0x%02x 0x%02x ... %s",
                             right_buf[0], right_buf[1], right_buf[2], right_buf[3], right_buf);
                    printf("CONTENT: %s\n", content);
#endif
                }

                if (strcmp(right_buf, "ready") == 0) {
                    printf("Right ready\n");
                    right_ready = 1;
                }
            }
        }

        if (right_ready == 1) {
            bzero(left_buf, BUF_SIZE);
            ssize_t size = recvfrom(left_socket, left_buf, BUF_SIZE, 0,
                                    (struct sockaddr *) &left_addr, &left_addr_len);
            if (size == -1) {
                perror("Error when receive from left");
            } else {
                int error = getnameinfo((struct sockaddr *) &left_addr, left_addr_len,
                                        left_host, sizeof(left_host),
                                        left_serv, sizeof(left_serv),
                                        NI_NUMERICHOST | NI_NUMERICSERV);
                if (error != 0) {
                    perror(gai_strerror(error));
                } else {
                    printf("%zd bytes received from %s:%s\n", size, left_host, left_serv);
#ifdef DEBUG
                    char content[64];
                    snprintf(content, sizeof(content), "0x%02x 0x%02x 0x%02x 0x%02x ... %s",
                             left_buf[0], left_buf[1], left_buf[2], left_buf[3], left_buf);
                    printf("CONTENT: %s\n", content);
#endif
                }
            }

            size = sendto(right_socket, left_buf, size, 0,
                       (struct sockaddr *) &right_addr, right_addr_len);
            printf("%zd bytes are resent to %s:%s\n", size, right_host, right_serv);
        }
    }

    return 0;
}
