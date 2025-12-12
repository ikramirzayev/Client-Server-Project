#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <math.h>

#define MAX_BUFFER 256

int parse_port(int argc, char *argv[]) {
    if (argc < 3 || strcmp(argv[1], "-p") != 0) {
        fprintf(stderr, "Usage: %s -p <port_number>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    return atoi(argv[2]);
}

double shannon_hesap(double B, double SN) {
    return B * log2(1.0 + SN);
}

int main(int argc, char *argv[]) {
    int listen_fd, conn_fd;
    int port = parse_port(argc, argv);
    socklen_t client_len;
    struct sockaddr_in server_addr, client_addr;
    char buffer[MAX_BUFFER];
    ssize_t bytes_read;

    if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        return EXIT_FAILURE;
    }
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
    if (bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        close(listen_fd);
        return EXIT_FAILURE;
    }
    if (listen(listen_fd, 5) < 0) {
        close(listen_fd);
        return EXIT_FAILURE;
    }
    client_len = sizeof(client_addr);
    if ((conn_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &client_len)) < 0) {
        close(listen_fd);
        return EXIT_FAILURE;
    }
    while ((bytes_read = recv(conn_fd, buffer, MAX_BUFFER - 1, 0)) > 0) {
        buffer[bytes_read] = '\0';
        
        if (strcmp(buffer, "finish") == 0) {
            break; 
        }
        
        double B, SN, C;
        if (sscanf(buffer, "%lf %lf", &B, &SN) == 2) {
            C = shannon_hesap(B, SN);
            
            char result_buffer[MAX_BUFFER];
            snprintf(result_buffer, MAX_BUFFER, "%.2lf", C);
            
            send(conn_fd, result_buffer, strlen(result_buffer), 0);
        }
    }
   close(conn_fd);
    close(listen_fd);

    return EXIT_SUCCESS;
}