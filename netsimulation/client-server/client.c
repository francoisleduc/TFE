/* 
 * udpclient.c - A simple UDP client
 * usage: udpclient <host> <port>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <ctype.h>
#include "common.h"
#include "client.h"
#include "log.h"

struct spacket* create_dummy(struct spacket* p)
{
	p->eventids = newLinkedList();
    p->eventdescri = newLinkedList();

    p->version[0] = 0x1;
    unsigned char myip[SRCIP_S] = {0x7F , 0x0, 0x0, 0x1};
    memcpy(p->srcip, myip, SRCIP_S);
    
    char* dv = "switch-1";
    strcpy(p->devname, dv);
    
    p->port[0] = 0x4; // 4th port of switch 
    p->nbevents[0] = 0x2;

    
    unsigned char id1[EVENTID_S];
    int_to_bytes(id1, 6);

    unsigned char id2[EVENTID_S];
    int_to_bytes(id2, 8);

    insertInLinkedList(p->eventids, id1);
    insertInLinkedList(p->eventids, id2);


    char *descr1 = "number of connection for destination port 8080 on time interval of 10seconds\n";
    char *descr2 = "number of bytes received on interface 2 over last 20 seconds\n";
    if(strlen(descr1) >= DESCRI_S|| strlen(descr2) >= DESCRI_S)
    {
        log_error("Event descriptions don't respect size constraints", __func__, __LINE__);
        return NULL; // should not exit so abruptly 
    }
    
    insertInLinkedList(p->eventdescri, descr1);
    insertInLinkedList(p->eventdescri, descr2);

    print_packet(p);
    return p;
}

void create_packet(char* buf, struct spacket* p) {

    // memcpy(destination, source, length)
    if(!p)
    	return;
    memcpy(buf, p->version, VERSION_S);
    memcpy(buf+VERSION_S,  p->srcip, SRCIP_S); // 1
    memcpy(buf+VERSION_S+SRCIP_S,  p->devname, DEVNAME_S); // 5
    memcpy(buf+VERSION_S+SRCIP_S+DEVNAME_S,  p->port, PORT_S); // 25
    memcpy(buf+VERSION_S+SRCIP_S+DEVNAME_S+PORT_S,  p->nbevents, NBEVENTS_S); //26

    
    LLNode* headevents = p->eventids->head;
    LLNode* headdescri = p->eventdescri->head;

    LLNode* current = p->eventids->head;

    int index = VERSION_S+SRCIP_S+DEVNAME_S+PORT_S+NBEVENTS_S; // 27
    for(int i = 0; i < sizeOfLinkedList(p->eventids); i++)
    {
        if(!current)
            log_error("Could not read event ids from buffer", __func__, __LINE__);
        memcpy(buf+index, current->value, EVENTID_S);
        index = index + EVENTID_S;
        current = current->next;
    }

    current = p->eventdescri->head;
    for(int i = 0; i < sizeOfLinkedList(p->eventdescri); i++)
    {
        if(!current)
            log_error("Could not read event description from buffer", __func__, __LINE__);
        memcpy(buf+index, current->value, DESCRI_S);
        index = index + DESCRI_S;
        current = current->next;
    }

    p->eventids->head = headevents;
    p->eventdescri->head = headdescri;

    
}


