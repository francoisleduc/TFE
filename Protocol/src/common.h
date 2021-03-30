#ifndef _COMMON_H_
#define _COMMON_H_

#include "linkedlist.h"
#include "log.h"
#include <stdio.h>
#include <stdlib.h>

#define BUFSIZE 1024

// 14

#define VERSION_S 1
#define SRCIP_S 4
#define NBEVENTS_S 1
#define SRCID_S 4
#define SEQ_S 4

// 25
#define EVENTID_S 4
#define DLENGTH_S 4
#define UID_S 16
#define ACK_S 1



struct spacket{
    unsigned char version[VERSION_S];
    unsigned char srcip[SRCIP_S];
    unsigned char sidentifier[SRCIP_S];
    unsigned char seq[SEQ_S];
    unsigned char nbevents[NBEVENTS_S];
    List* eventdescri;
};


struct pdescription{
    unsigned char len[DLENGTH_S]; // total description struct length in byte 
    unsigned char eventid[EVENTID_S];
    unsigned char ack[ACK_S];
    unsigned char uid[UID_S];
    unsigned char* textd; // Does not have to be text 
    int textlen;
};


void print_byte_array(unsigned char* buf, int x);
void int_to_bytes(unsigned char* bytes, unsigned long n);
void print_packet(struct spacket* p);
void print_ip_int(unsigned char* ip);
int get_description_header_size(void);
int get_p_header_size(void);
void print_description_struct(struct pdescription *d);
int bytes_to_single_int(unsigned char* bytes);
#endif