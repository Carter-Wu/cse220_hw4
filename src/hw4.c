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
#define O = 1
#define I = 2
#define S = 3
#define L = 4
#define Z = 5
#define J = 6
#define T = 7
#define HIT = 8
#define MISS = 9


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
int add_shape_to_board(int shape, int rotation, int row, int col, int *board, int len, int wid) {
    if (board[(row*wid)+col] == 0) {
        switch(shape) {
            case 1:
                printf("square");
                if (row >= 0 & row + 1  < len & col > 0 & col+1 < wid) {
                    if((board[((row)*wid)+col] + board[((row+1)*wid)+col] + board[(row*wid)+col+1] + board[((row+1)*wid)+col+1]) == 0) {
                        board[(row*wid)+col] = 1;
                        board[((row+1)*wid)+col] = 1;
                        board[(row*wid)+col+1] = 1;
                        board[((row+1)*wid)+col+1] = 1;
                        return 0; //no errors
                    } else return 303; // overlap  
                } else return 302; // shape doesn't fit in game board
                break;
            case 2:
                printf("Long piece");
                if (rotation%2 == 1) { //rotation 1 & 3
                    if(col + 3 > wid) return 301; //rotation out of range
                        if((board[(row*wid)+col] + board[((row+1)*wid)+col] + board[((row+2)*wid)+col] + board[((row+3)*wid)+col]) == 0) {
                            board[(row*wid)+col] = 2;
                            board[((row+1)*wid)+col] = 2;
                            board[((row+2)*wid)+col] = 2;
                            board[((row+3)*wid)+col] = 2;
                            return 0; //no errors
                        } else return 303; //overlap
                } else { //rotation 2 & 4
                    if(row + 3 > len) return 301; //rotation out of range
                        if((board[(row*wid)+col] + board[(row*wid)+col+1] + board[(row*wid)+col+2] + board[(row*wid)+col+3]) == 0) {
                            board[(row*wid)+col] = 2;
                            board[(row*wid)+col+1] = 2;
                            board[(row*wid)+col+2] = 2;
                            board[(row*wid)+col+3] = 2;
                            return 0; //no errors
                        } else return 303; //overlap
                }
                break;
            case 3: 
                printf("S piece");
                if (rotation%2 == 1) { //rotation 1 & 3
                    if(col + 2 < wid & row - 1 > 0) {
                        if((board[(row*wid)+col] + board[(row*wid)+col+1] + board[((row-1)*wid)+col+1] + board[((row-1)*wid)+col+2]) == 0) {
                            board[(row*wid)+col] = 3;
                            board[(row*wid)+col+1] = 3;
                            board[((row-1)*wid)+col+1] = 3;
                            board[((row-1)*wid)+col+2] = 3;
                            return 0;
                        } else return 303; // overlap
                    } else return 301; // rotation out of bounds
                } else { //rotation 2 & 4
                    if(col + 1 < wid & row + 2 < len) {
                        if((board[(row*wid)+col] + board[((row+1)*wid)+col] + board[((row+1)*wid)+col+1] + board[((row+2)*wid)+col+1]) == 0) {
                            board[(row*wid)+col] = 3;
                            board[((row+1)*wid)+col] = 3;
                            board[((row+1)*wid)+col+1] = 3;
                            board[((row+2)*wid)+col+1] = 3;
                            return 0;
                        } else return 303; // overlap
                    } else return 301; // rotation out of bounds
                }
                break;
            case 4:
                printf("L piece");
                switch(rotation) {
                    case 1:
                        if(row + 2 < len & col + 1 < wid) {
                            if((board[(row*wid)+col] + board[((row+1)*wid)+col] + board[((row+2)*wid)+col] + board[((row+2)*wid)+col+1])== 0) {
                                board[(row*wid)+col] = 4;
                                board[((row+1)*wid)+col] = 4;
                                board[((row+2)*wid)+col] = 4;
                                board[((row+2)*wid)+col+1] = 4;
                            } else return 303; //overlap
                        } else return 301; //rotation out of bound
                    case 2:
                        if(row + 1 < len & col + 2 < wid) {
                            if((board[(row*wid)+col] + board[((row+1)*wid)+col] + board[(row*wid)+col+1] + board[(row*wid)+col+2])== 0) {
                                board[(row*wid)+col] = 4;
                                board[((row+1)*wid)+col] = 4;
                                board[(row*wid)+col+1] = 4;
                                board[(row*wid)+col+2] = 4;
                            } else return 303; //overlap
                        } else return 301; //rotation out of bound
                }
                break;
            case 5:
                printf("Z piece");
                break;
            case 6:
                printf("J piece");
                break;
            case 7:
                printf("T piece");
                break;
            default:
                return -1;
        }
    } else return 300; // ship out of range
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
    // Accept incoming connection
    if ((conn_fd = accept(listen_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
    {
        perror("[Server] accept() failed.");
        exit(EXIT_FAILURE);
    }

    printf("Player 1 connected, waiting for player 2 ...\n");

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
    printf("Both players connect, beginning game...\n");
    // Receive and process commands
    char *p1_shots, *p2_shots; //shots in a round
    int error = 1;
    int length;
    int width;
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
                sscanf(word, "%d", &length);
                word = strtok(NULL, " ");
                if (length <10 || width < 10 || length > 32 || width > 32) {
                    memset(buffer, 0, BUFFER_SIZE);
                    strcpy(buffer, "E 200");
                    send(conn_fd, buffer, strlen(buffer), 0);
                    break;
                }
                sscanf(word, "%d", &width);
                word = strtok(NULL, " ");
                if (word == NULL) {
                    memset(buffer, 0, BUFFER_SIZE);
                    strcpy(buffer, "E 200");
                    send(conn_fd, buffer, strlen(buffer), 0);
                    break;
                }
                int *board = (int *)malloc(length*width*sizeof(int));
                memset(board, 0, length*width*sizeof(int));
                printf("board size: %d and %d", length, width);
                memset(buffer, 0, BUFFER_SIZE);
                strcpy(buffer, "A");
                send(conn_fd, buffer, strlen(buffer), 0);
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
    //seeing if player joins
    error = 1;
    while(error) {
        memset(buffer, 0, BUFFER_SIZE);
        nbytes = read(conn_fd2, buffer, BUFFER_SIZE);
        if (nbytes <= 0)
        {
            perror("[Server] read() failed.");
            exit(EXIT_FAILURE);
        }
        
        word = strtok(buffer, " ");
        if (strcmp(word, "B")) {
            memset(buffer, 0, BUFFER_SIZE);
            send(conn_fd2, "A", 2, 0);
            error = 0;
        } else if(strcmp(word, "F")) {
            memset(buffer, 0, BUFFER_SIZE);
                send(conn_fd2, "H 0", 4, 0);
                send(conn_fd, "H 1", 4, 0);
                break_conn(conn_fd,listen_fd,conn_fd2,listen_fd2);
                return EXIT_SUCCESS;
        } else {
            send(conn_fd2, "E 100", 6, 0);
        }
    }
    error = 1;
    int shape, rotation, row, col;
    while(error) {
        // now ask for initialization
        memset(buffer, 0, BUFFER_SIZE);
        nbytes = read(conn_fd, buffer, BUFFER_SIZE);
        if (nbytes <= 0)
        {
            perror("[Server] read() failed.");
            exit(EXIT_FAILURE);
        }
        printf("[Server] Received from client1: %s\n", buffer);
        word = strtok(buffer, " ");
        switch(*word) {
            case 'I':
                for(int j = 0;j < 5; j++) {
                    for (int i = 0;i < 4;i++) {
                        word = strtok(NULL, " ");
                        switch(i) {
                            case 0:
                                shape = *word;
                            case 1:
                                rotation = *word;
                            case 2:
                                row = *word;
                            case 3:
                                col = *word;
                        }
                        //type, rotation, row, col
                    }
                }
            case 'F':
                memset(buffer, 0, BUFFER_SIZE);
                //strcpy(buffer, "H 1");
                send(conn_fd, "H 0", 4, 0);
                //strcpy(buffer, "H 0");
                send(conn_fd2, "H 1", 4, 0);
                break_conn(conn_fd,listen_fd,conn_fd2,listen_fd2);
                return EXIT_SUCCESS;
            default:
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
    }
    printf("[Server] Shutting down.\n");

    close(conn_fd);
    close(listen_fd);
    close(conn_fd2);
    close(listen_fd2);
    return EXIT_SUCCESS;
}