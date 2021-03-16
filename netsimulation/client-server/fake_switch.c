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
#include <fcntl.h>
#include <netdb.h> 

#include <time.h>
#include <ctype.h>
#include "common.h"
#include "log.h"
#include "client.h"

extern List* eventsQACK;
extern List* eventsQNOACK;

int main(int argc, char **argv) {
    int sockfd, portno, n;
    socklen_t serverlen;
    struct sockaddr_in serveraddr;
    struct hostent *server;
    char *hostname;
    char buf[BUFSIZE];

    srand(time(NULL));
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
    


    /*
    // Building packet 
    log_info("Before dummy packets \n");
    
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
    
    // copy pasta
    serverlen = sizeof(serveraddr);
    n = sendto(sockfd, (const char*) buf, BUFSIZE, 0, (struct sockaddr *)&serveraddr, serverlen);
    if (n < 0) 
      log_error("Could not send datagram", __func__, __LINE__);
    
    n = recvfrom(sockfd, buf, BUFSIZE, 0, (struct sockaddr *)&serveraddr, &serverlen);
    //printf("Rcv %d bytes back from the server \n", n);
    if (n < 0)
      log_error("Could not read received bytes", __func__, __LINE__);


    buf[n] = '\0';

    printf("Echo from server: %s \n", buf);
    return 0;
    */

    //copy pasta ends here 




    /* Create a fake list of events to be sent periodically to the lambda server */

    eventsQACK = new_list();
    eventsQNOACK = new_list();
    int nb = 20;
    for(int i = 0; i < nb; i++)
    {
        struct event* newEvent = make_event(rand()%300, "My event description so farrr - ACK", 1);
        insert_in_list(eventsQACK, newEvent);
    }
    for(int i = 0; i < nb/2; i++)
    {
        struct event* newEvent = make_event(rand()%500, "My event does not need to be acknowledged", 0);
        insert_in_list(eventsQNOACK, newEvent);
    }



    /* This loop plays the role of the polling system on the switch which is responsible every x miliseconds to check if conditions
    are met to trigger lambda events and thus send them to the lambda server 
    NB: Normally this is in this same loop that we add events too but for now they are pre-loaded for testing purposes
    */

    char* ip = "127.0.0.1"; // this should be from a config file for each device 
    int identifierNumber = 210; // same 210 was taken as an example but should correspond to some kind of mapping id -> (device name, interface)
    unsigned char version[VERSION_S] = {0x1}; // same v1.0

    int seq_low = 0;
    int seq_top = 0;
    int window_s = 2; // length of window  

    while(1)
    {
        usleep(500 * 1000); // sleep for 500 ms

        /*
        Code the algorithm here 

        */
        if (seq_top - seq_low >= window_s)
        {
            /* Means that we sent all packets in window size - we must wait for acks basically */
            // do nothing ?
            printf("Packets are on route, waiting for acks \n");
        }

        else
        {
            // Two queues of events: in the first one all the events that need an ACK , the second one all the events that don't need ACK 
            struct spacket* pts = organize_packet(eventsQACK, eventsQNOACK, true); // pts= packet to send , true means we focus on ACK Queue only
            // make buf from packet 
            // Send this packet 
            serverlen = sizeof(serveraddr);
            n = sendto(sockfd, (const char*) buf, BUFSIZE, 0, (struct sockaddr *)&serveraddr, serverlen);
            if (n < 0)
            {
                log_error("Could not send datagram", __func__, __LINE__);
                seq_top = seq_top - 1; // to cancel effect of incrementation next 
            }

            seq_top = seq_top + 1; // upgrade upper bound 

        }
        /* *** */
        
        
        /* print the server's reply */
        n = recvfrom(sockfd, buf, BUFSIZE, O_NONBLOCK, (struct sockaddr *)&serveraddr, &serverlen);
        //printf("Rcv %d bytes back from the server \n", n);
        if (n < 0 || n > 1024)
        {
            printf("n: %d \n", n);
            log_error("Could not read received bytes", __func__, __LINE__);
            // didn't receive anything, start again the loop
            continue;
        }

        buf[n] = '\0';
        printf("Echo from server: %s \n", buf);
        // TODO:
        /*** Implement ack/received packet logic ***/ 

        /* If NACK */

        /* IF ACK */

        // Refill buffer with zeros ? I guess so if something was written on it 
    }
        
    // Should refill with zero for next send because packet payload might be different 
    return 0;
}