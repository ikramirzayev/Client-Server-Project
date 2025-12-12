#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <math.h>

#define MAX_KAPASITE 100 
#define MAX_BUFFER 256

void parse_args(int argc, char *argv[], char **host, int *port) {
    if (argc != 5 || strcmp(argv[1], "-h") != 0 || strcmp(argv[3], "-p") != 0) {
        fprintf(stderr, "Usage: %s -h <host_ip> -p <port_number>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    *host = argv[2];
    *port = atoi(argv[4]);
}

int main(int argc, char *argv[]) {
    int sockfd, port, channel_count = 0;
    char *host;
    struct sockaddr_in server_addr;
    char send_buffer[MAX_BUFFER];
    char recv_buffer[MAX_BUFFER];
    double capacities[MAX_KAPASITE]; 
    double max_capacity = -1.0;
    int best_channel = -1;
    
    parse_args(argc, argv, &host, &port);
    
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        return EXIT_FAILURE;
    }
    
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, host, &server_addr.sin_addr) <= 0) {
        close(sockfd);
        return EXIT_FAILURE;
    }
    
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        close(sockfd);
        return EXIT_FAILURE;
    }
    
    double bant, sinyal;
    char finish_str[10];
    
    while (1) {
        if (scanf("%lf %lf", &bant, &sinyal) == 2) {
            channel_count++;
            snprintf(send_buffer, MAX_BUFFER, "%.0lf %.0lf", bant, sinyal); 
            
            if (send(sockfd, send_buffer, strlen(send_buffer), 0) < 0) {
                 break;
            }
            
            ssize_t bytes_received = recv(sockfd, recv_buffer, MAX_BUFFER - 1, 0);
            
            if (bytes_received > 0) {
                recv_buffer[bytes_received] = '\0';
                double C = atof(recv_buffer);
                
                printf("%.2lf\n", C); 
                
                if (C > max_capacity) {
                    max_capacity = C;
                    best_channel = channel_count;
                }
            } else if (bytes_received == 0) {
                 break;
            } else {
                 break;
            }
        } 
        else if (scanf(" %9s", finish_str) == 1) { 
            if (strcmp(finish_str, "finish") == 0) {
                send(sockfd, finish_str, strlen(finish_str), 0);
                break;
            }
            while (getchar() != '\n' && !feof(stdin)); 
        }
        else {
            break;
        }
    }
    if (best_channel != -1) {
        printf("Selected Channel: %d\n", best_channel);
    }
    
    close(sockfd);
    return EXIT_SUCCESS;
}