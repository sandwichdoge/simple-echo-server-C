#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> //read()/write()
#include <sys/types.h> //socket stuff
#include <sys/socket.h> //socket stuff
#include <netinet/in.h> //INADDR_ANY
#include <pthread.h> //multi-connection
#include <string.h> //memset()

/*Creating socket server in C:
 *socket() -> setsocketopt() -> bind() -> listen() -> accept()
 *then read/write into socket returned by accept()
 */

struct server_socket {
    int fd;
    socklen_t len;
    struct sockaddr_in *handle;
};

void *conn_handler(void *);
struct server_socket create_server_socket(int port);


int main()
{
    int port = 12345;
    int new_fd = 0;
    struct server_socket sock = create_server_socket(port);
    pthread_t pthread;

    printf("Listening on port %d\n", port);
    while (1) {
        new_fd = accept(sock.fd, (struct sockaddr*)sock.handle, &sock.len);
        if (new_fd > 0) {
            pthread_create(&pthread, NULL, conn_handler, &new_fd); //create a thread for each new connection
        }
    }
    printf("Server stopped\n");
    
    return 0;
}


struct server_socket create_server_socket(int port)
{
    int err = 0;
    struct server_socket ret;
    struct sockaddr_in server;

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = INADDR_ANY;

    socklen_t len = sizeof(server);

    int fd = socket(AF_INET, SOCK_STREAM, 0); //Inet TCP
    if (!fd) printf("error creating socket\n");
    int opt = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)); //optional, prevents address already in use error
    err = bind(fd, (struct sockaddr*)&server, sizeof(server));
    if (err < 0) printf("error binding\n");
    err = listen(fd, 16); //max connections
    if (err < 0) printf("error listening\n");

    ret.fd = fd;
    ret.len = len;
    ret.handle = &server;
    return ret;
}


void *conn_handler(void *fd)
{
    char buf[2048] = "";
    char msg[64] = "Welcome to echo server 1!\nEnter 'exit' to disconnect.";
    int client_fd = *(int*)fd;
    printf("Client connected, fd: %d\n", client_fd);
    write(client_fd, msg, sizeof(msg)); //send welcome message

    while (1) {
        read(client_fd, buf, sizeof(buf));  //get message from client
        if (buf[0] != 0) {
            if (strcmp(buf, "exit") == 0) {
                strcpy(buf, "Disconnected.\n");
                write(client_fd, buf, sizeof(buf)); //send disconnect msg to client
                break; //break if "exit" message is received
            }
            printf("Echoing: %s\n", buf);
            fflush(stdout);
            write(client_fd, buf, sizeof(buf)); //echo it back to client
            memset(buf, 0, sizeof(buf));
        }
    }

    printf("Shutting down client: %d\n", client_fd);
    fflush(stdout);
    shutdown(client_fd, 2); //shutdown connection
    close(client_fd);
}