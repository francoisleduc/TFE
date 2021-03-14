/* 
 * udpclient.c - A simple UDP client
 * usage: udpclient <host> <port>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "client.h"
#include <ctype.h>
#include <uuid/uuid.h>


#define NETWORK_MAX_NODES 10000 /* The number of switches and routers combined in the network */
LinkedList*eventsQ = NULL;

struct event* make_event(int id, char* textdescription, int ack)
{
    struct event* e = malloc(sizeof(struct event));
    if(!e)
    {
        log_error("Could not allocate event client side", __func__, __LINE__);
        return NULL;
    }

    e->id = id;
    e->ack = ack;
    strcpy(e->textde, textdescription);

    return e;
}


struct pdescription* create_description_struct(struct pdescription* d, int length, unsigned char* uid, char* desc, unsigned char* ack, unsigned char *id)
{
    unsigned char len1[DLENGTH_S];
    int_to_bytes(len1, length);

    if(!d)
        return NULL;

    memcpy(d->len, len1, DLENGTH_S);
    memcpy(d->ack, ack, ACK_S);
    memcpy(d->uid,  uid, UID_S);
    memcpy(d->eventid, id, EVENTID_S);
    d->textd = desc;
    d->textlen = strlen(desc);
    return d;
}


struct spacket* create_dummy_packet_struct(struct spacket* p)
{
    p->eventdescri = newLinkedList();

    p->version[0] = 0x1;
    unsigned char myip[SRCIP_S] = {0x7F , 0x0, 0x0, 0x1};
    memcpy(p->srcip, myip, SRCIP_S);
    
    unsigned char srcid[SRCID_S];
    int_to_bytes(srcid, 100); // id 1
    memcpy(p->sidentifier, srcid, SRCID_S);

    unsigned char seqn[SEQ_S];
    int seqNo = rand();
    int_to_bytes(seqn, seqNo);
    memcpy(p->seq, seqn, SEQ_S);

    p->nbevents[0] = 0x2;

    
    unsigned char id1[EVENTID_S];
    int_to_bytes(id1, 6);

    unsigned char id2[EVENTID_S];
    int_to_bytes(id2, 8);

    char *descr1 = "number of connection for destination port 8080 on time interval of 10seconds\n";
    char *descr2 = "number of bytes received on interface 2 over last 20 seconds\n";

    unsigned char uuid1[16]; // here uuid_t is a typdef for char[16]
    unsigned char uuid2[16];
    uuid_generate_time(uuid1);
    uuid_generate_time(uuid2);

    unsigned char ackevent1[ACK_S] = {0x1}; // true 
    unsigned char ackevent2[ACK_S] = {0x0}; // false 

    struct pdescription *d1 = malloc(sizeof(struct pdescription));
    struct pdescription *d2 = malloc(sizeof(struct pdescription));
    if(!d1 || !d2)
        log_fatal("Could not allocate memory dummy packet", __FILE__, __func__, __LINE__);

    int length1 = get_description_header_size() + strlen(descr1);
    int length2 = get_description_header_size() + strlen(descr2);

    d1 = create_description_struct(d1, length1, uuid1, descr1, ackevent1, id1);
    d2 = create_description_struct(d2, length2, uuid2, descr2, ackevent2, id2);

    insertInLinkedList(p->eventdescri, d1);
    insertInLinkedList(p->eventdescri, d2);
    
    print_packet(p);
    return p;
}

void create_packet_buf(char* buf, struct spacket* p) {

    // memcpy(destination, source, length)
    if(!p)
    	return;
    memcpy(buf, p->version, VERSION_S);
    memcpy(buf+VERSION_S, p->srcip, SRCIP_S); // 1
    memcpy(buf+VERSION_S+SRCIP_S,  p->sidentifier, SRCID_S);
    memcpy(buf+VERSION_S+SRCIP_S+SRCID_S,  p->seq, SEQ_S);
    memcpy(buf+VERSION_S+SRCIP_S+SRCID_S+SEQ_S,  p->nbevents, NBEVENTS_S); 

    
    LLNode* headdescri = p->eventdescri->head;


    int index = VERSION_S+SRCIP_S+SRCID_S+SEQ_S+NBEVENTS_S;

    LLNode *current = p->eventdescri->head;
    for(int i = 0; i < sizeOfLinkedList(p->eventdescri); i++)
    {
        if(!current)
            log_error("Could not read event description from buffer", __func__, __LINE__);

        struct pdescription *cpd = (struct pdescription*)current->value;
        memcpy(buf+index, cpd->len, DLENGTH_S);
        index = index + DLENGTH_S;
        memcpy(buf+index, cpd->eventid, EVENTID_S);
        index = index + EVENTID_S;
        memcpy(buf+index, cpd->ack, ACK_S);
        index = index + ACK_S;
        memcpy(buf+index, cpd->uid, UID_S);
        index = index + UID_S;
        memcpy(buf+index, cpd->textd, cpd->textlen);
        index = index + cpd->textlen;
        current = current->next;
    }
    p->eventdescri->head = headdescri;
}


