#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "common.h"
#include "log.h"

struct event{
    int id;
    char* textde;
    int ack;
};


struct event* make_event(int id, char* textdescription, int ack);


void create_packet_buf(char* buf, struct spacket* p); // client only
struct spacket* create_dummy_packet_struct(struct spacket* p); // client only 
struct pdescription* create_description_struct(struct pdescription* d, int length, unsigned char *uid, char* desc, unsigned char* ack, unsigned char *id);
#endif