#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "common.h"
#include "log.h"
#include <stdbool.h> 

struct event{
    /* Common field of event to be sent on the network */
    int id;
    char* textde;
    int ack;
    int textlen;
    /* Fields used for queues and buffering decisions */
    bool hasBeenAcked;
    int assocSeqNb; // associated sequence number 
};

void free_list_event(List* list);
struct event* make_event(int id, char* textdescription, int ack);
struct event** organize_packet(List* queueACK, List* queueNOACK, bool ackq, int* nbevents);

void create_packet_buf(char* buf, struct spacket* p); // client only
struct spacket* create_dummy_packet_struct(struct spacket* p); // client only 
struct pdescription* create_description_struct(struct pdescription* d, int length, unsigned char *uid, char* desc, unsigned char* ack, unsigned char *id);
struct spacket* create_packet_struct(struct event** ev, int id, unsigned char* srcip, unsigned char* version, int nbevents);
void free_buffer_waiting_events(struct event** ev, List* queue, int nbevents);
void free_packet_struct(struct spacket* p);
#endif