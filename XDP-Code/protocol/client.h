#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "common.h"
#include "log.h"
#include <stdbool.h> 
#define MAX_EVENT_P 50 
struct event{
    /* Common field of event to be sent on the network */
    int id;
    unsigned char* textde;
    int ack;
    int textlen;
};

void free_list_event(List* list);
struct event* make_event(int id, unsigned char* textdescription, int ack, int descrilen);
struct event** organize_packet(List* queueACK, List* queueNOACK, bool ackq, int* nbevents, struct event** selectedEvts);

void create_packet_buf(char* buf, struct spacket* p); // client only
struct spacket* create_dummy_packet_struct(struct spacket* p); // client only 
struct pdescription* create_description_struct(struct pdescription* d, int length, unsigned char* desc, unsigned char* ack, unsigned char *id, int descrilen);
struct spacket* create_packet_struct(struct event** ev, int id, unsigned char* srcip, unsigned char* version, int nbevents, int seq);
void free_buffer_waiting_events(struct event** ev, List* queue);
void free_packet_struct(struct spacket* p);
#endif