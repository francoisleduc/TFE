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

extern pthread_mutex_t lock_ack;
extern pthread_mutex_t lock_non_ack;

extern bool terminated;
extern int seqNB;
int noack_p = 0;
int counter_ack = 0;

static void request_forge_flow_packet(int src, int dest, int ports, int64_t size, int protocol, unsigned char* s, int count) 
{
    int_to_bytes(s, src);
    int_to_bytes(s+4, dest);
    int_to_bytes(s+8, ports);
    int_to_bytes(s+12, protocol);
    int_to_bytes(s+16, count);
    int64_to_bytes(s+20, size);
}

static void* lambda_communication_thread(void* input)
{
    socklen_t serverlen = 0;
    int n = 0;
    int nbevents = 0;

    int msec = 0, retrans = 30, trigger1 = 2000, trigger2 = 2000; 
    usleep(300000); // ensure sending thread operates two tenth of a second later than the polling thread to ensure all events were added to the queue and 
    // optimize the sending of maximum events
    
    clock_t before1 = clock(); // associated to trigger1
    clock_t before2 = clock(); // associated to trigger2
    clock_t retran_c = clock(); // retransmission timer 

    struct spacket* pts = NULL;
    struct event** selectedEvts = malloc(MAX_EVENT_P*sizeof(struct event));
    struct event** selectedEvtsNoAck = malloc(MAX_EVENT_P*sizeof(struct event));

    for(int x = 0; x < MAX_EVENT_P; x++)
    {
        selectedEvts[x] = NULL;
        selectedEvtsNoAck[x] = NULL;
    }

    bzero(((struct args*)input)->buf, BUFSIZE);
    nbevents = send_new(pts, (struct args*) input, lock_ack, lock_non_ack, selectedEvts, serverlen, true);
    log_info("Setting flags \n");

    // Non blocking receive method flags 
    int flags = fcntl(((struct args*)input)->sockfd, F_GETFL);
    flags |= O_NONBLOCK;
    fcntl(((struct args*)input)->sockfd, F_SETFL, flags);

    while(true)
    {
        //if (msec % 1 == 0)
        clock_t difference = clock() - retran_c;
        msec = difference * 1000 / CLOCKS_PER_SEC;
        if(msec > retrans && nbevents > 0)
        {
            // Resend because timer expired and still did not receive an ACK for the last packet 
            log_info("Timer alert: Retransmission of last packet \n");
            serverlen = sizeof(((struct args*)input)->serveraddr);
            //print_byte_array((unsigned char*) ((struct args*)input)->buf, BUFSIZE);
            n = sendto(((struct args*)input)->sockfd, (const char*) ((struct args*)input)->buf, BUFSIZE, 0, 
                (struct sockaddr *)&(((struct args*)input)->serveraddr), serverlen);
            if (n < 0)
            {
                log_error("Could not send datagram", __func__, __LINE__);
            }
            else
            {
                retran_c = clock(); // retransmission timer 
            }
        }

        difference = clock() - before1;
        msec = difference * 1000 / CLOCKS_PER_SEC;
        if(msec > trigger1 && nbevents == 0)
        {
            printf("Trying to send ACK transaction \n");
            // send new 
            //log_info("Timer alert: Sending now because queue was empty last time \n");
            nbevents = send_new(pts, (struct args*) input, lock_ack, lock_non_ack, selectedEvts, serverlen, true);
            before1 = clock();
        }


        /* Here we check if the events that don't need an ACK can be sent depending on their own timer 
         * (which is most likely to be different from the previous one)
        */
        //printf("a\n");
        difference = clock() - before2;
        msec = difference * 1000 / CLOCKS_PER_SEC;
        if(msec > trigger2)
        {
            printf("Start burst \n");
            before2 = clock();
            do 
            {
                bzero(((struct args*)input)->bufSecondary, BUFSIZE);
                int noa = send_new(pts, (struct args*) input, lock_ack, lock_non_ack, selectedEvtsNoAck, serverlen, false);

                if(noa > 0)
                {
                    pthread_mutex_lock(&lock_non_ack);
                    noack_p += noa;
                    free_buffer_waiting_events(selectedEvtsNoAck, eventsQNOACK);
                    pthread_mutex_unlock(&lock_non_ack);
                }
            }
            while(get_list_size(eventsQNOACK) > 0);
            //log_info("Timer alert: Sending from non essential queue \n");
            printf("End burst \n");
        }

        /* print the server's reply */
        n = recvfrom(((struct args*)input)->sockfd, ((struct args*)input)->buf, BUFSIZE, O_NONBLOCK, 
            (struct sockaddr *)&(((struct args*)input)->serveraddr), &serverlen);

        if (n > 0 && n <= BUFSIZE)
        {
            if(((struct args*)input)->buf[0] == 'A' && ((struct args*)input)->buf[1] == 'C' && ((struct args*)input)->buf[2] == 'K')
            {
                pthread_mutex_lock(&lock_ack);
                free_buffer_waiting_events(selectedEvts, eventsQACK); // remove from Q and selectedEvts array
                counter_ack += nbevents;
                pthread_mutex_unlock(&lock_ack);

                ((struct args*)input)->buf[n] = '\0';

                bzero(((struct args*)input)->buf, BUFSIZE);
                nbevents = send_new(pts,(struct args*) input, lock_ack, lock_non_ack, selectedEvts, serverlen, true);
            }
        }
    }

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
    
    int identifierNumber = 210; // same 210 was taken as an example but should correspond to some kind of mapping id -> (device name, interface)
    unsigned char version[VERSION_S] = {0x1}; // same v1.0

    eventsQACK = new_list();
    eventsQNOACK = new_list();


    struct args *in = (struct args *)malloc(sizeof(struct args));
    
    in->sockfd = sockfd;
    memcpy(in->buf, buf, BUFSIZE);
    memcpy(in->bufSecondary, bufSecondary, BUFSIZE);
    in->identifierNumber = identifierNumber;
    memcpy(in->version, version, VERSION_S);
    in->serveraddr = serveraddr;
    
    pthread_t tid;
    pthread_create(&tid, NULL, lambda_communication_thread, (void *)in); // This thread manages message exchanges between the switch and the lambda server

    if(pthread_mutex_init(&lock_ack, NULL) != 0 || pthread_mutex_init(&lock_non_ack, NULL) != 0)
    {
        log_fatal("Mutex init failed", __FILE__, __func__, __LINE__);
        return 1;
    }


    int sec_counter = 0;

    FILE* fptr = fopen("noack_200_data.csv","w");
    if(fptr == NULL)
    {
        printf("Error!");   
        exit(1);             
    }
    
    fprintf(fptr,"queue_size, nb_events_sent, nb_bytes_sent \n"); // header 

    while(1)
    {
        /* This loop plays the role of the polling system on the switch which is responsible every x miliseconds to check if conditions
        are met to trigger lambda events and thus send them to the lambda server 
        NB: Normally this is in this same loop that we add events too but for now they are pre-loaded for testing purposes
        */
        for(int s = 0; s < 10; s++)
        {
            usleep(200000);
            printf("----------------------------------\n\n\n\n");
            sec_counter++;

            printf("Size of standard queue : %ld \n", get_list_size(eventsQNOACK));
            printf("Number of standard event sent : %d \n", noack_p);
            //printf("Number of standard bytes sent : %d \n", noack_p*28);

            //printf("\n\n");
            //printf("Size of Acknowledgment queue : %ld \n", get_list_size(eventsQACK));
            //printf("Number of Acknowledgment event sent : %d \n", counter_ack);
            //printf("Number of Acknowledgment bytes sent : %d \n", counter_ack*28);
            printf("Time elapsed: %f \n", sec_counter*0.2);

            fprintf(fptr, "%ld,%d,%d\n", get_list_size(eventsQNOACK), noack_p, noack_p*28); 
            //fprintf(fptr, "%ld,%d,%d\n", get_list_size(eventsQACK), counter_ack, counter_ack*28);

        }
        

        for(int j = 0; j < 200; j++)
        {
            unsigned char *s = malloc(28*sizeof(unsigned char));
              if(!s)
                return -1;

            request_forge_flow_packet(167772161, 167772162, 222222222, 1289000, 6, s, 128);
            //print_byte_array(s, 28);
            struct event* newEvent = make_event(2, s, 0, 28); // change 1 to 0 or 0 to 1 to put ACK flag 
            pthread_mutex_lock(&lock_non_ack);
            insert_in_list(eventsQNOACK, newEvent); // change queue type to fit correct event 
            pthread_mutex_unlock(&lock_non_ack);
        }
        
        if(sec_counter >= 100)
        {
            fclose(fptr);
            exit(1);
        }
    }
    /* End of do not include in XDP */

    free_list_event(eventsQACK);
    free_list_event(eventsQNOACK);
    // Should refill with zero for next send because packet payload might be different 
    return 0;
}
