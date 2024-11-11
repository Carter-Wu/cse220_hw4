#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <asm-generic/socket.h>

#define PORT1 2201
#define PORT2 2202
#define BUFFER_SIZE 1024

long file_size = 0;

char *load_file(const char *filename)
{
    char *buffer = 0;
    long length;
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        // ERROR("Could not open file %s", filename);
        return NULL;
    }
    (void)fseek(file, 0, SEEK_END);
    length = ftell(file);
    (void)fseek(file, 0, SEEK_SET);
    buffer = (char *)malloc(length + 1);

    if (buffer != NULL)
    {
        (void)fread(buffer, 1, length, file);
        (void)fclose(file);
    }

    file_size = length;

    return buffer;
}

int main()
{
    // Server Setup --------------------------------------------------------------------------->
    int listen_fd, conn_fd;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    // Create socket
    if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Set socket options
    if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    {
        perror("setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))");
        exit(EXIT_FAILURE);
    }
    if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt))");
        exit(EXIT_FAILURE);
    }

    // Bind socket to port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT1);
    if (bind(listen_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("[Server] bind() failed.");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(listen_fd, 3) < 0)
    {
        perror("[Server] listen() failed.");
        exit(EXIT_FAILURE);
    }

    printf("[Server] Running on port %d\n", PORT1);

    // Accept incoming connection
    if ((conn_fd = accept(listen_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
    {
        perror("[Server] accept() failed.");
        exit(EXIT_FAILURE);
    }
}

    // End Server Setup --------------------------------------------------------------------------->
