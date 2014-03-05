/*******************************/
/*                             */
/*    assuming using UNIX      */
/*                             */
/*******************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

/* takes port as command line argument */

int main(int argc, char** argv){
if(argc<2){printf("No port specified"); exit(1);}

int status;
char ipstr[INET6_ADDRSTRLEN];
struct addrinfo hints, *serv, *p;

// SET UP addrinfo STRUCT
memset(&hints, 0, sizeof hints); // init the struct
hints.ai_family = AF_UNSPEC; 
hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
hints.ai_flags = AI_PASSIVE; // fill in my IP for me

if ((status = getaddrinfo(argv[1], NULL, &hints, &serv)) != 0) {
fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
exit(1);
}

for(p = serv;p != NULL; p = p->ai_next) {
void *addr;
char *ipver;
// get the pointer to the address itself,
// different fields in IPv4 and IPv6:
if (p->ai_family == AF_INET) { // IPv4
struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
addr = &(ipv4->sin_addr);
ipver = "IPv4";
} else { // IPv6
struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
addr = &(ipv6->sin6_addr);
ipver = "IPv6";
}
// convert the IP to a string and print it:
inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
printf(" %s: %s\n", ipver, ipstr);
}

freeaddrinfo(serv);
}
