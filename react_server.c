#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "st_reactor.h"

#define PORT "9034"   // Port we're listening on


void clients_handler(void* this, int fd) {
    char buf[256];    // Buffer for client data
    int nbytes = recv(fd, buf, sizeof buf, 0);

    int sender_fd = fd;

    if (nbytes <= 0) {
        // Got error or connection closed by client
        if (nbytes == 0) {
            // Connection closed
            printf("pollserver: socket %d hung up\n", sender_fd);
        } else {
            perror("recv");
        }

        close(fd); // Bye!

        deleteFd(this, fd) ;

    } else {
        // We got some good data from a client
        printf("%s", buf) ;
        memset(buf,0,256);
    }
} // END handle data from client

// Get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

// Return a listening socket
int get_listener_socket(void)
{
    int listener;     // Listening socket descriptor
    int yes=1;        // For setsockopt() SO_REUSEADDR, below
    int rv;

    struct addrinfo hints, *ai, *p;

    // Get us a socket and bind it
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0) {
        fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
        exit(1);
    }

    for(p = ai; p != NULL; p = p->ai_next) {
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listener < 0) {
            continue;
        }

        // Lose the pesky "address already in use" error message
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
            close(listener);
            continue;
        }

        break;
    }

    freeaddrinfo(ai); // All done with this

    // If we got here, it means we didn't get bound
    if (p == NULL) {
        return -1;
    }

    // Listen
    if (listen(listener, 10) == -1) {
        return -1;
    }

    return listener;
}

int accept_socket(int listener){
    int newfd ;       // Newly accept()ed socket descriptor
    struct sockaddr_storage remoteaddr; // Client address
    socklen_t addrlen;
    addrlen = sizeof remoteaddr;
    newfd = accept(listener,
                   (struct sockaddr *)&remoteaddr,
                   &addrlen);

    if (newfd == -1) {
        perror("accept");
    } else {
        return newfd ;
    }
    return -1 ;
}

// Main
int main(void)
{
    struct Reactor *reactor = createReactor() ;
    int listener;     // Listening socket descriptor
    char remoteIP[INET6_ADDRSTRLEN];
    listener = get_listener_socket();
    if (listener == -1) {
        fprintf(stderr, "error getting listening socket\n");
        exit(1);
    }
    startReactor(reactor);
    int newfd ;
    // Main loop
    for(;;) {
        newfd = accept_socket(listener);
        addFd(reactor, newfd, &clients_handler) ;
    }
}