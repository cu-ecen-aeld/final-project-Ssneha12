#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>
 
#define PIR_DEVICE "/dev/pir"
 
#define BOARD_B_IP "192.168.10.22"
#define PORT 5001
 
int main(void)
{
    int pir_fd;
    int sockfd;
    char buffer[64];
 
    struct sockaddr_in board_b;
 
    /* Open PIR device */
    pir_fd = open(PIR_DEVICE, O_RDONLY);
 
    if (pir_fd < 0)
    {
        perror("open");
        return -1;
    }
 
    /* Create UDP socket */
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
 
    if (sockfd < 0)
    {
        perror("socket");
        close(pir_fd);
        return -1;
    }
 
    memset(&board_b, 0, sizeof(board_b));
 
    board_b.sin_family = AF_INET;
    board_b.sin_port = htons(PORT);
    inet_pton(AF_INET, BOARD_B_IP, &board_b.sin_addr);
 
    printf("Waiting for PIR interrupt...\n");
 
    while (1)
    {
        int n;
 
        memset(buffer, 0, sizeof(buffer));
 
        n = read(pir_fd, buffer, sizeof(buffer));
 
        if (n > 0)
        {
            printf("%s", buffer);
 
            sendto(sockfd,
                   "PIR",
                   3,
                   0,
                   (struct sockaddr *)&board_b,
                   sizeof(board_b));
 
            printf("Notification sent to Board B\n");
        }
 
        usleep(100000);
    }
 
    close(sockfd);
    close(pir_fd);
 
    return 0;
}



