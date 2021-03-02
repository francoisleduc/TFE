#ifndef _COMMON_H_
#define _COMMON_H_

#include "linkedlist.h"
#include "log.h"
#include <stdio.h>
#include <stdlib.h>

#define BUFSIZE 1024


#define VERSION_S 1
#define SRCIP_S 4
#define DEVNAME_S 20
#define PORT_S 1
#define NBEVENTS_S 1

#define DESCRI_S 100
#define EVENTID_S 4


struct spacket{
    unsigned char version[VERSION_S];
    unsigned char srcip[SRCIP_S];
    char devname[DEVNAME_S];
    unsigned char port[PORT_S];
    unsigned char nbevents[NBEVENTS_S];
    LinkedList* eventids;
    LinkedList* eventdescri;
};


void print_byte_array(unsigned char* buf, int x);
void int_to_bytes(unsigned char* bytes, unsigned long n);
void print_packet(struct spacket* p);
void print_ip_int(unsigned char* ip);

#endif