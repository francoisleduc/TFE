/* 
 * udpclient.c - A simple UDP client
 * usage: udpclient <host> <port>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h> 


#include "common.h"
#include "log.h"
#include "client.h"

extern List* eventsQACK;
extern List* eventsQNOACK;

extern pthread_mutex_t lock;
extern bool terminated;
extern int seqNB;


static void* lambda_communication_thread(void* input)
{
    socklen_t serverlen = 0;
    int n = 0;
    bool t = true;
    int nbevents = 0;

    int msec = 0, trigger1 = 500, trigger2 = 1000; /* 500ms */ /* and */ /* 1000ms */
    clock_t before1 = clock(); // associated to trigger1
    clock_t before2 = clock(); // associated to trigger2

    struct spacket* pts = NULL;
    struct event** selectedEvts = malloc(MAX_EVENT_P*sizeof(struct event));
    struct event** selectedEvtsNoAck = malloc(MAX_EVENT_P*sizeof(struct event));

    for(int x = 0; x < MAX_EVENT_P; x++)
    {
        selectedEvts[x] = NULL;
        selectedEvtsNoAck[x] = NULL;
    }

    bzero(((struct args*)input)->buf, BUFSIZE);
    nbevents = send_new(pts, (struct args*) input, lock, selectedEvts, serverlen, true);
    log_info("Setting flags \n");

    // Non blocking receive method flags 
    int flags = fcntl(((struct args*)input)->sockfd, F_GETFL);
    flags |= O_NONBLOCK;
    fcntl(((struct args*)input)->sockfd, F_SETFL, flags);

    int counter = nbevents;
    while(t)
    {
        if(counter > 50)
            break; // testing purposes 
        clock_t difference1 = clock() - before1;
        msec = difference1 * 1000 / CLOCKS_PER_SEC;
        if(msec > trigger1 && nbevents > 0)
        {
            // Resend because timer expired and still did not receive an ACK for the last packet 
            log_info("Timer alert: Retransmission of last packet \n");
            serverlen = sizeof(((struct args*)input)->serveraddr);
            n = sendto(((struct args*)input)->sockfd, (const char*) ((struct args*)input)->buf, BUFSIZE, 0, 
                (struct sockaddr *)&(((struct args*)input)->serveraddr), serverlen);
            if (n < 0)
            {
                log_error("Could not send datagram", __func__, __LINE__);
            }
            else
            {
                before1 = clock(); // reset timer 
            }
        }

        if(msec > trigger1 && nbevents == 0)
        {
            // send new 
            //log_info("Timer alert: Sending now because queue was empty last time \n");
            nbevents = send_new(pts, (struct args*) input, lock, selectedEvts, serverlen, true);
            before1 = clock();
            counter = counter + nbevents;
            printf("COUNTER EVENTS %d \n", counter);

        }


        /* Here we check if the events that don't need an ACK can be sent depending on their own timer 
         * (which is most likely to be different from the previous one)
        */
        clock_t difference2 = clock() - before2;
        msec = difference2 * 1000 / CLOCKS_PER_SEC;
        if(msec > trigger2)
        {
            //log_info("Timer alert: Sending from non essential queue \n");
            bzero(((struct args*)input)->bufSecondary, BUFSIZE);
            int noa = send_new(pts, (struct args*) input, lock, selectedEvtsNoAck, serverlen, false);
            if(noa > 0)
            {
                pthread_mutex_lock(&lock);
                free_buffer_waiting_events(selectedEvtsNoAck, eventsQNOACK);
                pthread_mutex_unlock(&lock);
            }
            
            before2 = clock();
        }
        
        /* print the server's reply */
        n = recvfrom(((struct args*)input)->sockfd, ((struct args*)input)->buf, BUFSIZE, O_NONBLOCK, 
            (struct sockaddr *)&(((struct args*)input)->serveraddr), &serverlen);

        if (n > 0 && n <= 1024)
        {
            if(((struct args*)input)->buf[0] == 'A' && ((struct args*)input)->buf[1] == 'C' && ((struct args*)input)->buf[2] == 'K')
            {
                pthread_mutex_lock(&lock);
                free_buffer_waiting_events(selectedEvts, eventsQACK); // remove from Q and selectedEvts array
                pthread_mutex_unlock(&lock);

                ((struct args*)input)->buf[n] = '\0';
                printf("Echo from server: %s \n", ((struct args*)input)->buf);
                log_info("Acknowledgment received: sending new packet \n");

                bzero(((struct args*)input)->buf, BUFSIZE);
                nbevents = send_new(pts,(struct args*) input, lock, selectedEvts, serverlen, true);
                before1 = clock();
                before2 = clock();

                counter = counter + nbevents;
                printf("COUNTER EVENTS %d \n", counter);

            }
        }
    }

    printf("COUNTER EVENTS %d \n", counter);
    free(selectedEvts);
    free(selectedEvtsNoAck);

    free((struct args*)input);
    terminated = true;
    pthread_detach(pthread_self()); // To avoid memory leaks 
    pthread_exit(NULL);
}


int main(int argc, char **argv) {
    int sockfd, portno;
    struct sockaddr_in serveraddr;
    struct hostent *server;
    char *hostname;
    char buf[BUFSIZE];
    char bufSecondary[BUFSIZE];

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
    bzero(bufSecondary, BUFSIZE);
    /* send the message to the server */
    
    /* Create a fake list of events to be sent periodically to the lambda server */
    unsigned char myip[SRCIP_S] = {0x7F, 0x0, 0x0, 0x1}; // this should be from a config file for each device 
    int identifierNumber = 210; // same 210 was taken as an example but should correspond to some kind of mapping id -> (device name, interface)
    unsigned char version[VERSION_S] = {0x1}; // same v1.0

    eventsQACK = new_list();
    eventsQNOACK = new_list();
    

    /**** Do not to include in XDP ****/
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

    /*** End of do not include in XDP ***/


    struct args *in = (struct args *)malloc(sizeof(struct args));
    
    in->sockfd = sockfd;
    memcpy(in->buf, buf, BUFSIZE);
    memcpy(in->bufSecondary, bufSecondary, BUFSIZE);
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


    /* Do not include in XDP */

    usleep(300 * 1000);
    while(1)
    {
        /* This loop plays the role of the polling system on the switch which is responsible every x miliseconds to check if conditions
        are met to trigger lambda events and thus send them to the lambda server 
        NB: Normally this is in this same loop that we add events too but for now they are pre-loaded for testing purposes
        */
        usleep(60 * 1000);
        pthread_mutex_lock(&lock);
        printf("### Main process adding lambda events in the queue \n");
        unsigned char s[18];
        memcpy(s, "Added Event - ACK\0", 18);
        struct event* newEvent = make_event(1, s, 1, 18);
        insert_in_list(eventsQACK, newEvent);
        
        pthread_mutex_unlock(&lock);
        
        if(terminated)
            break;
    }
    /* End of do not include in XDP */

    free_list_event(eventsQACK);
    free_list_event(eventsQNOACK);
    // Should refill with zero for next send because packet payload might be different 
    return 0;
}