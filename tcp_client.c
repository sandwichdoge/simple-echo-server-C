#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> //read()/write()
#include <sys/types.h> //socket stuff
#include <sys/socket.h> //socket stuff
#include <netdb.h> //gethostbyname()
#include <string.h> //memset()


/*Steps to create a client socket in C:
 *gethostbyname() -> socket() -> connect() -> read/write()
 */

int create_client_sock(char *hostname, int port);

int main()
{
    char incoming[2048];
    char outgoing[1024];
    
    int client_fd = create_client_sock("localhost", 12345); //create a client connection socket to target server
    if (client_fd <0) return -1; //failed to create client socket
    
    read(client_fd, incoming, sizeof(incoming)); //get welcome message from server
    printf("%s\n", incoming);

    while (1) {
        scanf("%1024s", &outgoing);
        write(client_fd, outgoing, sizeof(outgoing)); //send msg from stdin to server
        read(client_fd, incoming, sizeof(incoming)); //get response from server
        printf("%s\n", incoming);
        memset(incoming, 0, sizeof(incoming));
    }

    return 0;
}

int create_client_sock(char *hostname, int port)
{
    struct hostent *he;
    he = gethostbyname(hostname); //query DNS for IP address
    if (he == NULL) {
        printf("Could not resolve hostname.\n");
        return -1;
    }
    
    struct sockaddr_in server;
    server.sin_family = AF_INET; //INET connection
    server.sin_addr.s_addr = *(in_addr_t*)he->h_addr_list[0]; //sin_addr is black magic, it's different a hack on each system
    server.sin_port = htons(port); //convert to compliant network short int

    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    int err = connect(client_fd, (struct sockaddr*)&server, sizeof(server));
    if (err < 0) return -2;

    return client_fd;
}
