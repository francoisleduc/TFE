#include "common.h"
#include <time.h>

int bytes_to_single_int(unsigned char* bytes)
{
    int a = (int)(unsigned char)(bytes[0]) << 24 | (unsigned char)(bytes[1]) << 16 | (unsigned char)(bytes[2]) << 8 | (unsigned char)(bytes[3]);
    return a;
}

int get_description_header_size(void)
{
    return EVENTID_S + DLENGTH_S + ACK_S;
}


int get_p_header_size(void)
{
    return VERSION_S+SRCIP_S+SRCID_S+SEQ_S+NBEVENTS_S;
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
    for(int i = 0; i < get_list_size(p->eventdescri); i++)
    {
        if(!current)
            log_error("Couldn't printout event descriptions", __func__, __LINE__);
        print_description_struct((struct pdescription*)get_element_value(current));
        current = get_element_next(current);
    }
    set_head_list(p->eventdescri, headdescri);
}
