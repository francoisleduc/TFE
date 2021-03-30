/* 
 * udpclient.c - A simple UDP client
 * usage: udpclient <host> <port>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include "client.h"
#include <ctype.h>
#include <uuid/uuid.h>


List* eventsQACK = NULL;
List* eventsQNOACK = NULL;



void free_list_event(List* list)
{
    if(list)
    {
        Element* curr = get_list_head(list);
        while(curr != NULL)
        {
            void* val = get_element_value(curr);
            if(val)
                free(val);
            curr = get_element_next(curr);
        }

        free_list(list);
    }   
}

struct event* make_event(int id, unsigned char* textdescription, int ack, int descrilen)
{
    struct event* e = malloc(sizeof(struct event));
    if(!e)
    {
        log_error("Could not allocate event client side", __func__, __LINE__);
        return NULL;
    }

    e->id = id;
    e->ack = ack;
    e->textde = textdescription;
    e->textlen = descrilen;
    return e;
}

// returns an array of event* 
struct event** organize_packet(List* queueACK, List* queueNOACK, bool ackq, int* nbevents, struct event** selectedEvts)
{
    List* q;
    *nbevents = 0;
    if(ackq)
        q = queueACK;
    else
        q = queueNOACK;

    if(get_list_size(q) == 0)
        return NULL;


    int pheadersize = get_p_header_size();
    int sum = pheadersize;

    Element* saveh = get_list_head(q);
    Element* current = get_list_head(q); // make a copy 

    //struct event** extracted = malloc(MAX_EVENT_P*sizeof(struct event)); // events to be put in next packet and extracted from the list 
    int i = 0;
    for(i = 0; i < MAX_EVENT_P; i++)
    {
        if(i < get_list_size(q))
        {
            struct event* ev = (struct event*)get_element_value(current);
            if(sum + get_description_header_size() + ev->textlen  < BUFSIZE)
                selectedEvts[i] = ev;
            else
                break;

            sum += ev->textlen + get_description_header_size();
            current = get_element_next(current);
            (*nbevents)++;
        }
        else
        {
            selectedEvts[i] = NULL;
        }
    }

    printf("Total sum : %d - Number of events: %d \n", sum, *nbevents);
    set_head_list(q, saveh);

    // For now we have an array with the events we'd like to put in the next packet.
    // Add these into the pending ACK Q (buffering them before deleting them as soon as we receive an ACK from the lambda server)
    // create_packet(from list of events?)
    return selectedEvts;
}

void free_buffer_waiting_events(struct event** ev, List* queue)
{
    if(!ev || !queue)
        return;

    int i = 0;
    while(i < MAX_EVENT_P && ev[i] != NULL)
    {
        Element* el = find_element_from_value(queue, ev[i]); // ev[i] is the value (event*)
        delete_in_list(queue, el);
        free(ev[i]);
        i++;
    }
}


void free_packet_struct(struct spacket* p)
{
    if(p)
    {
        Element* curr = get_list_head(p->eventdescri);
        while (curr != NULL)
        {
            struct pdescription *d = get_element_value(curr);
            if(d)
                free(d);

            curr = get_element_next(curr);
        }
        free_list(p->eventdescri);
        free(p);
    }
}


struct spacket* create_packet_struct(struct event** ev, int id, unsigned char* srcip, unsigned char* version, int nbevents)
{
    struct spacket* p = malloc(sizeof(struct spacket));
    if(!p)
        return NULL;


    p->eventdescri = new_list();

    p->version[0] = version[0];
    memcpy(p->srcip, srcip, SRCIP_S);
    
    unsigned char srcid[SRCID_S];
    int_to_bytes(srcid, id);
    memcpy(p->sidentifier, srcid, SRCID_S);


    unsigned char seqn[SEQ_S];
    int seqNo = rand();
    int_to_bytes(seqn, seqNo);
    memcpy(p->seq, seqn, SEQ_S);


    p->nbevents[0] = (unsigned char) nbevents;
    for(int i = 0; i < nbevents; i++)
    {   
        struct event* cur = ev[i];
        struct pdescription *d = malloc(sizeof(struct pdescription));
        if(!d)
            return NULL;

        int length = get_description_header_size() + cur->textlen;
        unsigned char uuid[16]; // here uuid_t is a typdef for char[16]
        uuid_generate_time(uuid);

        unsigned char ida[EVENTID_S];
        int_to_bytes(ida, cur->id);

        unsigned char ackevent[ACK_S];
        ackevent[0] = (cur->ack == 1) ? 0x1 : 0x0;
        d = create_description_struct(d, length, uuid, cur->textde, ackevent, ida, cur->textlen);
        insert_in_list(p->eventdescri, d);
    }

    print_packet(p);
    return p;

}

struct pdescription* create_description_struct(struct pdescription* d, int length, unsigned char* uid, unsigned char* desc, unsigned char* ack, unsigned char *id, int descrilen)
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
    d->textlen = descrilen;
    return d;
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

    
    Element* headdescri = get_list_head(p->eventdescri);


    int index = VERSION_S+SRCIP_S+SRCID_S+SEQ_S+NBEVENTS_S;

    Element *current = get_list_head(p->eventdescri);
    for(int i = 0; i < get_list_size(p->eventdescri); i++)
    {
        if(!current)
            log_error("Could not read event description from buffer", __func__, __LINE__);

        struct pdescription *cpd = (struct pdescription*) get_element_value(current);
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
        current = get_element_next(current);
    }
    set_head_list(p->eventdescri, headdescri);
}


