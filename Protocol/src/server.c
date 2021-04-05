/* 
 * udpserver.c - A simple UDP echo server 
 * usage: udpserver <port>
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdbool.h>


#include "common.h"

struct respacket{
    /* Common field of event to be sent on the network */
    struct spacket *parsed;
    int response;
};

int total_events = 0;

static int process_description_event_id1(struct pdescription* d, unsigned char* buf)
{
    memcpy(d->ack, buf+DLENGTH_S+EVENTID_S, ACK_S);    
    d->textlen = bytes_to_single_int(d->len) - get_description_header_size();
    d->textd = (unsigned char*) malloc(d->textlen*sizeof(unsigned char));
    memcpy(d->textd, buf+DLENGTH_S+EVENTID_S+ACK_S, d->textlen);
    return get_description_header_size() + d->textlen;
}


static void free_packet_struct(struct spacket* p)
{
    if(p)
    {
        Element* curr = get_list_head(p->eventdescri);
        while (curr != NULL)
        {
            struct pdescription *d = get_element_value(curr);
            if(d)
            {
                free(d->textd);
                free(d);
            }

            curr = get_element_next(curr);
        }
        free_list(p->eventdescri);
        free(p);
    }
}

static struct respacket* process_packet(unsigned char* buf)
{
    struct spacket* rp = malloc(sizeof(struct spacket));
    if(!rp)
        return NULL;

    rp->eventdescri = new_list();

    if(!rp->eventdescri)
        return NULL;

    // memcpy(destination, source, length)
    memcpy(rp->version, buf, VERSION_S);
    memcpy(rp->srcip, buf+VERSION_S, SRCIP_S); // +1
    memcpy(rp->sidentifier, buf+VERSION_S+SRCIP_S, SRCID_S); 
    memcpy(rp->seq, buf+VERSION_S+SRCIP_S+SRCID_S, SEQ_S);
    memcpy(rp->nbevents, buf+VERSION_S+SRCIP_S+SRCID_S+SEQ_S, NBEVENTS_S); 

    int index = VERSION_S+SRCIP_S+SRCID_S+SEQ_S+NBEVENTS_S;  

    int hasToBeAck = 0;
    total_events = total_events + (int) rp->nbevents[0];

    for(int i = 0; i < (int) rp->nbevents[0]; i++)
    {
        int dlen = 0;
        struct pdescription *d = malloc(sizeof(struct pdescription));
        memcpy(d->len, buf+index, DLENGTH_S);
        memcpy(d->eventid, buf+index+DLENGTH_S, EVENTID_S);

        int eid = bytes_to_single_int(d->eventid);
        switch(eid)
        {
            case 1:
                dlen = process_description_event_id1(d, buf+index);
                hasToBeAck = d->ack[0] & 1; // Since we only put events with same ACKflag together we can make the assumption of checking last one only
                break;
            case 2:
                dlen = process_description_event_id1(d, buf+index); // for the moment parse event id2 the same way of id1
                hasToBeAck = d->ack[0] & 1;
                break;
            case 3:
                // id 3
                break;
            case 4:
                // id 4 and so on 
                break;
            default:
                log_info("Default id parsing \n");
        }
    
        index = index + dlen;
        insert_in_list(rp->eventdescri, d);
    }

    struct respacket* rs = malloc(sizeof(struct respacket));
    if(rs)
    {
        rs->parsed = rp;
        rs->response = hasToBeAck;
    }
    log_info("Processed the whole packet \n");

    return rs;
}


int main(int argc, char **argv) {
    int sockfd; /* socket */
    int portno; /* port to listen on */
    socklen_t clientlen; /* byte size of client's address */
    struct sockaddr_in serveraddr; /* server's addr */
    struct sockaddr_in clientaddr; /* client addr */
    unsigned char buf[BUFSIZE]; /* message buf */
    char *hostaddrp = ""; /* dotted decimal host addr string */
    int optval; /* flag value for setsockopt */
    int n = 0; /* message byte size */
    bool responding = true;
    float droprate = 0.0;
    int nbClients = 0;
    /* 
    * check command line arguments 
    */
    if (argc != 4) 
    {
    	log_fatal("usage: %s <port> <droprate> <max client nb>", __FILE__, __func__, __LINE__);
        exit(1);
    }
    portno = atoi(argv[1]);
    droprate = atof(argv[2]);
    nbClients =  atoi(argv[3]);
    printf("[INFO] Listening on port: %d with droprate: %f \n", portno, droprate);
    /* 
    * socket: create the parent socket 
    */
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    	log_error("Could not open socket", __func__, __LINE__);

    /* setsockopt: Handy debugging trick that lets 
    * us rerun the server immediately after we kill it; 
    * otherwise we have to wait about 20 secs. 
    * Eliminates "ERROR on binding: Address already in use" error. 
    */
    optval = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, 
         (const void *)&optval , sizeof(int));

    /*
    * build the server's Internet address
    */
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Server listens on that ip 
    serveraddr.sin_port = htons((unsigned short)portno);


    if (bind(sockfd, (struct sockaddr *) &serveraddr, 
       sizeof(serveraddr)) < 0)
    	log_error("Could not bind socket", __func__, __LINE__); // I guess I could put this error as fatal and exit()

    /* 
    * main loop: wait for a datagram, then send ACK 
    */
    clientlen = sizeof(clientaddr);
    int seqPerSwitchId[nbClients];
    for(int i = 0; i < nbClients; i++)
        seqPerSwitchId[i] = 1;

    while (true) {

        /*
         * recvfrom: receive a UDP datagram from a client
         */
        bzero(buf, BUFSIZE);
        n = recvfrom(sockfd, buf, BUFSIZE, 0,
        	 (struct sockaddr *) &clientaddr, &clientlen);
        if (n < 0)
          log_error("Could not read received bytes", __func__, __LINE__);


        hostaddrp = inet_ntoa(clientaddr.sin_addr);
        if (hostaddrp == NULL)
          log_error("Could not parse datagram source address", __func__, __LINE__);
        printf("server received datagram from (%s)\n", hostaddrp);
        printf("server received %d bytes \n", n);

        
        /* 
         * sendto: sends back a response to the initial sender of the datagram
         */
        float rd = drand48();
        if((droprate < rd) && responding)
        {
            struct respacket* rp = process_packet(buf);
            if(!rp)
            {
                return -1;
            }

            //print_packet(rp->parsed);
            int receivedId = 210; //bytes_to_single_int(rp->parsed->sidentifier);
            int receivedSeq = bytes_to_single_int(rp->parsed->seq);
            char* response = "ACK\n";
            bzero(buf, BUFSIZE);
            bcopy(response, buf, strlen(response));

            if(rp->response == 1) // Requires an acknowledgment
            {
                if(seqPerSwitchId[receivedId] == receivedSeq)
                {
                    // IF SEQUENCE NUMBER MATCH - INCREMENT EXPECTED SQN BY 1
                    seqPerSwitchId[receivedId] = (seqPerSwitchId[receivedId] % INT_MAX) + 1;
                    printf("Expected sequence number \n");
                } 

                printf("Sending back: %s", buf);
                n = sendto(sockfd, (const char*) buf, BUFSIZE, 0, (struct sockaddr *) &clientaddr, clientlen);
                if (n < 0) 
                    log_error("Could not send response datagram", __func__, __LINE__);
            }
            else
            {
                printf("Packet received does not require an acknowledgment \n");
            }
            
            printf("total_events : %d \n", total_events);
            free_packet_struct(rp->parsed);
            free(rp);
        }
    } // end while server 

    return 0;
}