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
#include <pthread.h> 


#include "common.h"
#include "log.h"
#include "client.h"

extern List* eventsQACK;
extern List* eventsQNOACK;


bool terminated = false;

struct args {
    unsigned char myip[SRCIP_S];
    char buf[BUFSIZE];
    int identifierNumber;
    unsigned char version[VERSION_S];
    int sockfd;
    struct sockaddr_in serveraddr;
};


static void* lambda_communication_thread(void* input)
{
    socklen_t serverlen = 0;
    int n = 0;
    bool t = true;
    int counter = 0;
    struct event** selectedEvts;


    while(t)
    {
        printf("SIZE OF ACK QUEUE : %ld \n", get_list_size(eventsQACK));

        usleep(500 * 1000); // sleep for 500 ms
        log_info("Iteration: ");
        printf("%d \n", counter++);
        /*
        Code the algorithm here 

        */
        int nbevents = 0;
        // Two queues of events: in the first one all the events that need an ACK , the second one all the events that don't need ACK 
        selectedEvts = organize_packet(eventsQACK, eventsQNOACK, true, &nbevents); // selected events, true means we focus on ACK Queue only
        // make buf from packet 
        struct spacket* pts = create_packet_struct(selectedEvts, ((struct args*)input)->identifierNumber, ((struct args*)input)->myip , ((struct args*)input)->version, nbevents);
        // Send this packet 
        create_packet_buf(((struct args*)input)->buf, pts);
        serverlen = sizeof(((struct args*)input)->serveraddr);
        n = sendto(((struct args*)input)->sockfd, (const char*) ((struct args*)input)->buf, BUFSIZE, 0, (struct sockaddr *)&(((struct args*)input)->serveraddr), serverlen);
        if (n < 0)
        {
            log_error("Could not send datagram", __func__, __LINE__);
        }

        /* *** */
        
        
        /* print the server's reply */
        n = recvfrom(((struct args*)input)->sockfd, ((struct args*)input)->buf, BUFSIZE, O_NONBLOCK, (struct sockaddr *)&(((struct args*)input)->serveraddr), &serverlen);
        //printf("Rcv %d bytes back from the server \n", n);
        if (n < 0 || n > 1024)
        {
            printf("n: %d \n", n);
            log_error("Could not read received bytes", __func__, __LINE__);
            // didn't receive anything, start again the loop
            continue;
        }

        ((struct args*)input)->buf[n] = '\0';
        printf("Echo from server: %s \n", ((struct args*)input)->buf);

        free_buffer_waiting_events(selectedEvts, eventsQACK, nbevents);

        free_packet_struct(pts);

        printf("SIZE OF ACK QUEUE : %ld \n", get_list_size(eventsQACK));
        
        // Refill buffer with zeros ? I guess so if something was written on it $
        t = false;
    }

    free_list_event(eventsQACK);
    free_list_event(eventsQNOACK);
    free(selectedEvts);
    free((struct args*)input);
    terminated = true;
    pthread_detach(pthread_self());
    pthread_exit(NULL);
}



int main(int argc, char **argv) {
    int sockfd, portno;
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
    
    /* Create a fake list of events to be sent periodically to the lambda server */
    unsigned char myip[SRCIP_S] = {0x7F , 0x0, 0x0, 0x1}; // this should be from a config file for each device 
    int identifierNumber = 210; // same 210 was taken as an example but should correspond to some kind of mapping id -> (device name, interface)
    unsigned char version[VERSION_S] = {0x1}; // same v1.0

    eventsQACK = new_list();
    eventsQNOACK = new_list();
    int nb = 10;
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

    struct args *in = (struct args *)malloc(sizeof(struct args));
    
    in->sockfd = sockfd;
    memcpy(in->buf, buf, BUFSIZE);
    memcpy(in->myip, myip, SRCIP_S);
    in->identifierNumber = identifierNumber;
    memcpy(in->version, version, VERSION_S);
    in->serveraddr = serveraddr;
    
    pthread_t tid; 
    pthread_create(&tid, NULL, lambda_communication_thread, (void *)in); // This thread manages message exchanges between the switch and the lambda server

    while(1)
    {
        /* This loop plays the role of the polling system on the switch which is responsible every x miliseconds to check if conditions
        are met to trigger lambda events and thus send them to the lambda server 
        NB: Normally this is in this same loop that we add events too but for now they are pre-loaded for testing purposes
        */
        usleep(50 * 1000); // sleep for 50 ms
        printf("### Main process has slept a period equivalent to the rate of at which the polling process (kernel tables polling) runs ###\n");
        if(terminated)
            break;
    }
    // Should refill with zero for next send because packet payload might be different 
    return 0;
}