/* 
 * udpclient.c - A simple UDP client
 * usage: udpclient <host> <port>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

#include <ctype.h>
#include "common.h"
#include "log.h"
#include "client.h"

int main(int argc, char **argv) {
    int sockfd, portno, n;
    socklen_t serverlen;
    struct sockaddr_in serveraddr;
    struct hostent *server;
    char *hostname;
    char buf[BUFSIZE];

    /* check command line arguments */
    if (argc != 3) {
       log_fatal("usage: ./client <hostname> <port>", __FILE__, __func__, __LINE__);
       exit(0);
    }
    
    hostname = argv[1];
    portno = atoi(argv[2]);

    /* socket: create the socket */
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) 
       log_error("Could not open socket", __func__, __LINE__);

    /* gethostbyname: get the server's DNS entry */
    server = gethostbyname(hostname);
    if (server == NULL) {
       log_fatal("No such host", __FILE__, __func__, __LINE__);
       exit(0);
    }

    /* build the server's Internet address */
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
	  (char *)&serveraddr.sin_addr.s_addr, server->h_length);
    serveraddr.sin_port = htons(portno);

    /* get a message from the user */
    bzero(buf, BUFSIZE);

    /* send the message to the server */
    

    // Building packet 
    struct spacket *p;
    p = (struct spacket*) malloc(sizeof(struct spacket));
    if(!p)
        return -1;
    
    p = create_dummy_packet_struct(p);
    if(!p)
        log_warn("Could not create proper packet");
    printf("----------------------\n");
    create_packet_buf(buf, p);
    printf("\n\n\n");
    //print_byte_array(buf, BUFSIZE); 

    log_info("Sending: ");
    printf("\n\n");

    serverlen = sizeof(serveraddr);
    n = sendto(sockfd, (const char*) buf, BUFSIZE, 0, (struct sockaddr *)&serveraddr, serverlen);
    if (n < 0) 
      log_error("Could not send datagram", __func__, __LINE__);
    
    /* print the server's reply */
    n = recvfrom(sockfd, buf, BUFSIZE, 0, (struct sockaddr *)&serveraddr, &serverlen);
	//printf("Rcv %d bytes back from the server \n", n);
    if (n < 0)
      log_error("Could not read received bytes", __func__, __LINE__);


    buf[n] = '\0';

    printf("Echo from server: %s \n", buf);

    // Should refill with zero for next send because packet payload might be different 
    return 0;
}