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


List* eventsQACK = NULL;
List* eventsQNOACK = NULL;

pthread_mutex_t lock;

bool terminated = false;

int seqNB = 1; // own sequence number


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
        ev[i] = NULL;
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
struct event** organize_packet(List* queueACK, List* queueNOACK, bool ackq, int* nbevents, struct event** selected)
{
    List* q;
    *nbevents = 0;
    if(ackq)
        q = queueACK;
    else
        q = queueNOACK;

    if(get_list_size(q) == 0)
        return NULL;


    int pheadersize = PACKET_HEADER_SIZE;
    int sum = pheadersize;

    Element* saveh = get_list_head(q);
    Element* current = get_list_head(q); // make a copy 

    unsigned int i = 0;
    for(i = 0; i < MAX_EVENT_P; i++)
    {
        if(i < get_list_size(q))
        {
            struct event* ev = (struct event*)get_element_value(current);
            if(sum + DESCRI_HEADER_SIZE + ev->textlen  < BUFSIZE)
            {
                selected[i] = ev;
                sum += ev->textlen + DESCRI_HEADER_SIZE;
                current = get_element_next(current);
                (*nbevents)++;
            }
            else
                selected[i] = NULL;
        }
        else // fill up the end of the array with NULL 
            selected[i] = NULL;
    }

    

    printf("Total sum : %d - Number of events: %d \n", sum, *nbevents);
    set_head_list(q, saveh);

    return selected;
}





struct spacket* create_packet_struct(struct event** ev, int id, unsigned char* srcip, unsigned char* version, int nbevents, int seq)
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
    int_to_bytes(seqn, seq);
    memcpy(p->seq, seqn, SEQ_S);


    p->nbevents[0] = (unsigned char) nbevents;
    for(int i = 0; i < nbevents; i++)
    {   
        struct event* cur = ev[i];
        struct pdescription *d = malloc(sizeof(struct pdescription));
        if(!d)
            return NULL;

        int length = DESCRI_HEADER_SIZE + cur->textlen;

        unsigned char ida[EVENTID_S];
        int_to_bytes(ida, cur->id);

        unsigned char ackevent[ACK_S];
        ackevent[0] = (cur->ack == 1) ? 0x1 : 0x0;
        d = create_description_struct(d, length, cur->textde, ackevent, ida, cur->textlen);
        insert_in_list(p->eventdescri, d);
    }

    //print_packet(p);
    return p;

}

struct pdescription* create_description_struct(struct pdescription* d, int length, unsigned char* desc, unsigned char* ack, unsigned char *id, int descrilen)
{
    unsigned char len1[DLENGTH_S];
    int_to_bytes(len1, length);

    if(!d)
        return NULL;

    memcpy(d->len, len1, DLENGTH_S);
    memcpy(d->ack, ack, ACK_S);
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
    for(unsigned int i = 0; i < get_list_size(p->eventdescri); i++)
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
        memcpy(buf+index, cpd->textd, cpd->textlen);
        index = index + cpd->textlen;
        current = get_element_next(current);
    }
    set_head_list(p->eventdescri, headdescri);
}


int send_new(struct spacket* pts, struct args* input, pthread_mutex_t lock, 
    struct event** selected, socklen_t serverlen, bool tag)
{

    char* b;
    if(tag)
        b = input->buf;
    else
        b = input->bufSecondary;

    pthread_mutex_lock(&lock);
    int n = 0;
    int nbevents = 0;
    
    // Send the next packet 
    selected = organize_packet(eventsQACK, eventsQNOACK, tag, &nbevents, selected); // selected events, true means we focus on ACK Queue only
    // make buf from packet
    if(selected) // If there is no event to send on the line then just ignore making a packet 
    {
        pts = create_packet_struct(selected, input->identifierNumber, input->myip , input->version, nbevents, seqNB);
        // Send this packet 
        create_packet_buf(b, pts);
        free_packet_struct(pts); // free last packet structure 
        serverlen = sizeof(input->serveraddr);
        n = sendto(input->sockfd, (const char*) b, BUFSIZE, 0, (struct sockaddr *)&(input->serveraddr), serverlen);
        if (n < 0)
        {
            log_error("Could not send datagram", __func__, __LINE__);
        }
        else
            if(tag == true)
                seqNB = (seqNB % INT_MAX) + 1;
        // Should release the lock here 
    }
    else
    {
        //log_info("Send new packets from Q but no events is available \n");
    }

    pthread_mutex_unlock(&lock);
    return nbevents;
}



/* Utils */

int bytes_to_single_int(unsigned char* bytes)
{
    int a = (int)(unsigned char)(bytes[0]) << 24 | (unsigned char)(bytes[1]) << 16 | (unsigned char)(bytes[2]) << 8 | (unsigned char)(bytes[3]);
    return a;
}


void print_ip_int(unsigned char* ip)
{
    printf("%d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]);   
}



void print_byte_array(unsigned char* buf, int x) 
{
    int i;
    for (i = 0; i < x; i++)
    {
        if (i > 0) printf(":");
        printf("%02X", (unsigned char)buf[i]);
    }
    printf("\n");
}


void int_to_bytes(unsigned char* bytes, unsigned long n) 
{

    bytes[0] = (n >> 24) & 0xFF;
    bytes[1] = (n >> 16) & 0xFF;
    bytes[2] = (n >> 8) & 0xFF;
    bytes[3] = n & 0xFF;
}


void int64_to_bytes(unsigned char* bytes, unsigned long n)
{
    bytes[0] = (n >> 56) & 0xFF;
    bytes[1] = (n >> 48) & 0xFF;
    bytes[2] = (n >> 40) & 0xFF;
    bytes[3] = (n >> 32) & 0xFF;
    bytes[4] = (n >> 24) & 0xFF;
    bytes[5] = (n >> 16) & 0xFF;
    bytes[6] = (n >> 8) & 0xFF; 
    bytes[7] = n & 0xFF;
}


void print_description_struct(struct pdescription *d)
{
    //log_info("\t\t Length of description: ");
    //print_byte_array(d->len, DLENGTH_S);

    log_info("Length of description (decimal): ");
    printf("%d \n", bytes_to_single_int(d->len));


    //log_info("\t\t Event identifier: ");
    //print_byte_array(d->eventid, EVENTID_S);

    log_info("Event identifier (decimal): ");
    printf("%d \n", bytes_to_single_int(d->eventid));


    //log_info("Ack flag: ");
    //print_byte_array(d->ack, ACK_S);

    log_info("Ack meaning: ");
    if(d->ack[0] & 1)
        log_info("true \n");
    else
        log_info("false \n");

    log_info("Description additional bytes: ");
    print_byte_array(d->textd, d->textlen); 

    printf("\n\n");
    return;
}

void print_packet(struct spacket* p)
{
    log_info("Version:");
    print_byte_array(p->version, VERSION_S);

    //log_info("Switch identifier number: ");
    //print_byte_array(p->sidentifier, SRCID_S);

    log_info("Switch identifier number (decimal): ");
    printf("%d \n", bytes_to_single_int(p->sidentifier));

    log_info("Switch ip address: ");
    print_ip_int(p->srcip);

    //log_info("Sequence number: ");
    //print_byte_array(p->seq, SEQ_S);
    log_info("Sequence number (decimal): ");
    printf("%d \n", bytes_to_single_int(p->seq));

    log_info("Number of events: ");
    print_byte_array(p->nbevents, NBEVENTS_S);

    if(!p->eventdescri)
        return;

    Element* headdescri = get_list_head(p->eventdescri);
    Element* current = get_list_head(p->eventdescri);

    printf("\n\n");
    for(unsigned int i = 0; i < get_list_size(p->eventdescri); i++)
    {
        if(!current)
            log_error("Couldn't printout event descriptions", __func__, __LINE__);
        print_description_struct((struct pdescription*)get_element_value(current));
        current = get_element_next(current);
    }
    set_head_list(p->eventdescri, headdescri);
}
