#include "common.h"



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
        printf("%02X", buf[i]);
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


void print_packet(struct spacket* p)
{
	log_info("Version:");
    print_byte_array(p->version, VERSION_S);

	log_info("Port of switch: ");
    print_byte_array(p->port, PORT_S);

    log_info("Device name: ");
    printf("%s \n", p->devname);

    log_info("Switch ip address: ");
    print_ip_int(p->srcip);

    log_info("Number of events: ");
    print_byte_array(p->nbevents, NBEVENTS_S);

    LLNode* headevents = p->eventids->head;
    LLNode* headdescri = p->eventdescri->head;

    LLNode* current = p->eventids->head;

    for(int i = 0; i < sizeOfLinkedList(p->eventids); i++)
    {
        if(!current)
            log_error("Couldn't printout events", __func__, __LINE__);
        printf("Event ID #%d: ", i);
        print_byte_array((unsigned char*)current->value, EVENTID_S);
        current = current->next;
    }

    current = p->eventdescri->head;
    for(int i = 0; i < sizeOfLinkedList(p->eventdescri); i++)
    {
        if(!current)
            log_error("Couldn't printout event descriptions", __func__, __LINE__);
        printf("Description #%d : %s \n", i, (char*) current->value);
        current = current->next;
    }

    p->eventids->head = headevents;
    p->eventdescri->head = headdescri;

}
