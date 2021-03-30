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


#include <errno.h>


#include <time.h>
//#include <ctype.h>
#include <pthread.h>  // pthread 
#include <fcntl.h> // IO rcv()

#include "common.h"
#include "log.h"
#include "client.h"

extern List* eventsQACK;
extern List* eventsQNOACK;


pthread_mutex_t lock;

bool terminated = false;

struct args {
    unsigned char myip[SRCIP_S];
    char buf[BUFSIZE];
    int identifierNumber;
    unsigned char version[VERSION_S];
    int sockfd;
    struct sockaddr_in serveraddr;
};


static int send_new(struct spacket* pts, struct args* input, pthread_mutex_t lock, 
    struct event** selectedEvts, socklen_t serverlen)
{

    int n = 0;
    int nbevents = 0;
    
    // Send the next packet 
    selectedEvts = organize_packet(eventsQACK, eventsQNOACK, true, &nbevents, selectedEvts); // selected events, true means we focus on ACK Queue only
    // make buf from packet
    if(selectedEvts) // If there is no event to send on the line then just ignore making a packet 
    {
        pts = create_packet_struct(selectedEvts, input->identifierNumber, input->myip , input->version, nbevents);
        // Send this packet 
        create_packet_buf(input->buf, pts);
        free_packet_struct(pts); // free last packet structure 
        serverlen = sizeof(input->serveraddr);
        n = sendto(input->sockfd, (const char*) input->buf, BUFSIZE, 0, (struct sockaddr *)&(input->serveraddr), serverlen);
        if (n < 0)
        {
            log_error("Could not send datagram", __func__, __LINE__);
        }
        // Should release the lock here 
    }
    else
        log_info("Send new packets from Q but no events is available \n");

    pthread_mutex_unlock(&lock);
    return nbevents;
}


static void* lambda_communication_thread(void* input)
{
    socklen_t serverlen = 0;
    int n = 0;
    bool t = true;
    int nbevents = 0;

    int msec = 0, trigger = 500; /* 500ms */
    clock_t before = clock(); // start timer for the first packet 
    struct spacket* pts = NULL;
    struct event** selectedEvts = malloc(MAX_EVENT_P*sizeof(struct event));

    bzero(((struct args*)input)->buf, BUFSIZE);
    nbevents = send_new(pts, (struct args*) input, lock, selectedEvts, serverlen);
    before = clock();
    // Should release the lock here 


    log_info("Setting flags \n");

    // Non blocking receive method flags 
    int flags = fcntl(((struct args*)input)->sockfd, F_GETFL);
    flags |= O_NONBLOCK;
    fcntl(((struct args*)input)->sockfd, F_SETFL, flags);


    int i = 0;
    while(t)
    {
        if(i > 5) // If 20 packets are sent we stop for testing purposes only 
            break;
        clock_t difference = clock() - before;
        msec = difference * 1000 / CLOCKS_PER_SEC;
        if(msec > trigger && nbevents > 0)
        {
            // Resend because timer expired and still did not receive an ACK for the last packet 
            log_info("Timer alert: Retransmission of last packet \n");
            serverlen = sizeof(((struct args*)input)->serveraddr);
            n = sendto(((struct args*)input)->sockfd, (const char*) ((struct args*)input)->buf, BUFSIZE, 0, (struct sockaddr *)&(((struct args*)input)->serveraddr), serverlen);
            if (n < 0)
            {
                log_error("Could not send datagram", __func__, __LINE__);
            }
            else
            {
                before = clock(); // reset timer 
            }
        }

        if(msec > trigger && nbevents == 0)
        {
            // send new 
            i++;
            log_info("Timer alert: Sending now because queue was empty last time \n");
            nbevents = send_new(pts, (struct args*) input, lock, selectedEvts, serverlen);
            before = clock();
        }

        /* print the server's reply */
        n = recvfrom(((struct args*)input)->sockfd, ((struct args*)input)->buf, BUFSIZE, O_NONBLOCK, (struct sockaddr *)&(((struct args*)input)->serveraddr), &serverlen);
        if (n > 0 && n < 1024)
        {
            if(((struct args*)input)->buf[0] == 'A' && ((struct args*)input)->buf[1] == 'C' && ((struct args*)input)->buf[2] == 'K')
            {
                pthread_mutex_lock(&lock);
                free_buffer_waiting_events(selectedEvts, eventsQACK); // remove from Q and selectedEvts array
                pts = NULL;

                pthread_mutex_unlock(&lock);
                // Did read something on the socket 
                ((struct args*)input)->buf[n] = '\0';
                printf("Echo from server: %s \n", ((struct args*)input)->buf);
                log_info("Acknowledgment received: sending new packet \n");
                bzero(((struct args*)input)->buf, BUFSIZE);
                nbevents = send_new(pts,(struct args*) input, lock, selectedEvts, serverlen);
                before = clock();
            }
        }
    }

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
        unsigned char s[28];
        memcpy(s, "My event needs an ack - ACK\0", 28);
        struct event* newEvent = make_event(1, s, 1, 28);
        insert_in_list(eventsQACK, newEvent);
    }
    
    /*
    for(int i = 0; i < nb/2; i++)
    {
        unsigned char s[42];
        memcpy(s, "My event does not need to be acknowledged\0", 42);
        struct event* newEvent = make_event(2, s, 0, 42);
        insert_in_list(eventsQNOACK, newEvent);
    }
    */

    struct args *in = (struct args *)malloc(sizeof(struct args));
    
    in->sockfd = sockfd;
    memcpy(in->buf, buf, BUFSIZE);
    memcpy(in->myip, myip, SRCIP_S);
    in->identifierNumber = identifierNumber;
    memcpy(in->version, version, VERSION_S);
    in->serveraddr = serveraddr;
    
    pthread_t tid; 
    pthread_create(&tid, NULL, lambda_communication_thread, (void *)in); // This thread manages message exchanges between the switch and the lambda server

    if(pthread_mutex_init(&lock, NULL) != 0)
    {
        log_fatal("Mutex init failed", __FILE__, __func__, __LINE__);
        return 1;
    }

    while(1)
    {
        /* This loop plays the role of the polling system on the switch which is responsible every x miliseconds to check if conditions
        are met to trigger lambda events and thus send them to the lambda server 
        NB: Normally this is in this same loop that we add events too but for now they are pre-loaded for testing purposes
        */
        usleep(800 * 1000); // sleep for 50 ms
        printf("### Main process adding lambda events in the queue \n");

        pthread_mutex_lock(&lock);
        
        unsigned char s[18];
        memcpy(s, "Added Event - ACK\0", 18);
        struct event* newEvent = make_event(1, s, 1, 18);
        insert_in_list(eventsQACK, newEvent);
        
        pthread_mutex_unlock(&lock);
        
        if(terminated)
            break;
    }

    free_list_event(eventsQACK);
    free_list_event(eventsQNOACK);
    // Should refill with zero for next send because packet payload might be different 
    return 0;
}