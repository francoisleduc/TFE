#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "common.h"
#include "log.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>  // pthread 
#include <fcntl.h> // IO rcv()
#include <stdbool.h> 
#include <netinet/in.h>

/* 
* Maximum number of event per packet (not really an important parameter but this could be tweaked to avoid packet loss with
* smaller packets overall
*/
#define MAX_EVENT_P 50 

struct event{
    /* Common field of event to be sent on the network */
    int id; // An event has an ID 
    unsigned char* textde; // Has a byte array. Its structure is defined per ID and can be of any content
    int ack; // If it is an event that needs an ACK from the server (no loss)
    int textlen; // The length of 'textde' byte array
};


// Arguments passsed to initialize the thread communicating with lambda server(s)
struct args {
    unsigned char myip[SRCIP_S]; // The current device IP 
    char buf[BUFSIZE]; // The buffer used to read/write data
    int identifierNumber; // The unique ID of the device
    unsigned char version[VERSION_S]; // The version of the protocol that will be used 
    int sockfd; // The socket number (only one used in our case)
    struct sockaddr_in serveraddr; // Server addr structure used 
};

void free_list_event(List* list);
void free_buffer_waiting_events(struct event** ev, List* queue);
void free_packet_struct(struct spacket* p);

struct event* make_event(int id, unsigned char* textdescription, int ack, int descrilen);
struct event** organize_packet(List* queueACK, List* queueNOACK, bool ackq, int* nbevents, struct event** selectedEvts);
void create_packet_buf(char* buf, struct spacket* p); // client only


struct pdescription* create_description_struct(struct pdescription* d, int length, unsigned char* desc, unsigned char* ack, unsigned char *id, int descrilen);
struct spacket* create_packet_struct(struct event** ev, int id, unsigned char* srcip, unsigned char* version, int nbevents, int seq);



int send_new(struct spacket* pts, struct args* input, pthread_mutex_t lock, 
    struct event** selectedEvts, socklen_t serverlen, bool tag);
#endif