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

void break_conn(int con1, int lis1, int con2, int lis2) {
    close(con1);
    close(lis1);
    close(con2);
    close(lis1);
}
int main()
{
    // Server Setup --------------------------------------------------------------------------->
    int listen_fd, conn_fd, listen_fd2, conn_fd2;
    struct sockaddr_in address, address2;
    int opt,opt2 = 1;
    int addrlen = sizeof(address);
    int addrlen2 = sizeof(address2);
    char buffer[BUFFER_SIZE] = {0};

    printf("waiting for players...\n");
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
    address.sin_port = htons(PORT1); //do the same htons as on the  player automated
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
    printf("Player 1 connected, waiting for 2\n");
    // Accept incoming connection
    if ((conn_fd = accept(listen_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
    {
        perror("[Server] accept() failed.");
        exit(EXIT_FAILURE);
    }

    printf("Player 1 connected, waiting for player 2 ...");

    if ((listen_fd2 = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    if (setsockopt(listen_fd2, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    {
        perror("setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))");
        exit(EXIT_FAILURE);
    }
    if (setsockopt(listen_fd2, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt))");
        exit(EXIT_FAILURE);
    }
    address2.sin_family = AF_INET;
    address2.sin_addr.s_addr = INADDR_ANY;
    address2.sin_port = htons(PORT2);
    if (bind(listen_fd2, (struct sockaddr *)&address2, sizeof(address2)) < 0)
    {
        perror("[Server] bind() failed.");
        exit(EXIT_FAILURE);
    }
    if (listen(listen_fd2, 3) < 0)
    {
        perror("[Server] listen() failed.");
        exit(EXIT_FAILURE);
    }
    if ((conn_fd2 = accept(listen_fd2, (struct sockaddr *)&address2, (socklen_t *)&addrlen2)) < 0)
    {
        perror("[Server] accept() failed.");
        exit(EXIT_FAILURE);
    }
    printf("Both players connect, beginning game...");
    // Receive and process commands
    int error = 1;
    char *word;
    int nbytes;
    while (error)
    {
        memset(buffer, 0, BUFFER_SIZE);
        nbytes = read(conn_fd, buffer, BUFFER_SIZE);
        if (nbytes <= 0)
        {
            perror("[Server] read() failed.");
            exit(EXIT_FAILURE);
        }
        printf("[Server] Received from client: %s\n", buffer);
        word = strtok(buffer, " ");
        switch(*word) {
            case 'B':
                word = strtok(NULL, " ");
                int length;
                sscanf(word, "%d", &length);
                word = strtok(NULL, " ");
                int width;
                if (length <10 || width < 10) {
                    memset(buffer, 0, BUFFER_SIZE);
                    strcpy(buffer, "E 100");
                    send(conn_fd, buffer, strlen(buffer), 0);
                    continue;
                }
                sscanf(word, "%d", &width);
                int *board = (int *)malloc(length*width*sizeof(int));
                memset(board, 0, length*width*sizeof(int));
                printf("board size: %d and %d", length, width);
                error = 0;
                break;
            case 'F':
                memset(buffer, 0, BUFFER_SIZE);
                //strcpy(buffer, "H 1");
                send(conn_fd, "H 0", 4, 0);
                //strcpy(buffer, "H 0");
                send(conn_fd2, "H 1", 4, 0);
                break_conn(conn_fd,listen_fd,conn_fd2,listen_fd2);
                return EXIT_SUCCESS;
            default:
            //should send an error packet E 100
                memset(buffer, 0, BUFFER_SIZE);
                strcpy(buffer, "E 100");
                send(conn_fd, buffer, strlen(buffer), 0);
                break;
        }
    }
    while(1) {
        // now ask for initialization
        
        memset(buffer, 0, BUFFER_SIZE);
        strcpy(buffer, "A");
        send(conn_fd, buffer, strlen(buffer), 0);
        memset(buffer, 0, BUFFER_SIZE);
        nbytes = read(conn_fd, buffer, BUFFER_SIZE);
        if (nbytes <= 0)
        {
            perror("[Server] read() failed.");
            exit(EXIT_FAILURE);
        }
        printf("[Server] Received from client1: %s\n", buffer);
        word = strtok(buffer, " ");
        if (*word == 'I') {

        } else {
            memset(buffer, 0, BUFFER_SIZE);
            strcpy(buffer, "E 101");
            send(conn_fd, buffer, strlen(buffer), 0);
        }
        
        // if (strcmp(buffer, "quit") == 0)
        // {
        //     printf("[Server] Client quitting...\n");
        //     printf("[Server] Quitting...\n");
        //     send(conn_fd, buffer, strlen(buffer), 0);
        //     break;
        // }
    printf("[Server] Shutting down.\n");

    close(conn_fd);
    close(listen_fd);
    close(conn_fd2);
    close(listen_fd2);
    return EXIT_SUCCESS;

}