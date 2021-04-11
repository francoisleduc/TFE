#include <iostream>
#include <arpa/inet.h>
#include <cstring>

/*
 * udpserver.c - A simple UDP echo server
 * usage: udpserver <port>
 */

#include "Server.h"

using namespace std;

int total_events = 0;


Server::Server(int port, char* address)
{
    this->portno = port;
    this->ip = address;
    /*
    * socket: create the parent socket
    */
    this->sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (this->sockfd < 0)
        cout << "Could not open socket " << __func__ << "+ " << __LINE__ << endl;

    /* setsockopt: Handy debugging trick that lets
    * us rerun the server immediately after we kill it;
    * otherwise we have to wait about 20 secs.
    * Eliminates "ERROR on binding: Address already in use" error.
    */
    int optval = 1;
    setsockopt(this->sockfd, SOL_SOCKET, SO_REUSEADDR,
               (const void *)&optval , sizeof(int));

    /*
    * build the server's Internet address
    */
    bzero((char *) &this->serveraddr, sizeof(this->serveraddr));
    this->serveraddr.sin_family = AF_INET;
    this->serveraddr.sin_addr.s_addr = inet_addr(this->ip); // Server listens on that ip
    this->serveraddr.sin_port = htons((unsigned short)this->portno);

    if (bind(this->sockfd, (struct sockaddr *) &this->serveraddr,
             sizeof(this->serveraddr)) < 0)
    {
        cout << "Could not bind socket " << __func__ << "+ " << __LINE__ << endl;
        exit(1);
    }
}


Server::~Server()
{
    close(this->sockfd);
}

pair<socklen_t, struct sockaddr_in> Server::receive(unsigned char* buf, socklen_t clientlen, struct sockaddr_in clientaddr) const
{
    /*
     * recvfrom: receive a UDP datagram from a client
     */
    int n = recvfrom(this->sockfd, buf, BUFSIZE, 0,
                 (struct sockaddr *) &clientaddr, &clientlen);
    if (n < 0)
        cout << "Could not read received bytes " << __func__ << "+ " << __LINE__ << endl;
    else
        cout << "Server received " << n << " bytes" << endl;
    return make_pair(clientlen, clientaddr);
}


pair<socklen_t, struct sockaddr_in> Server::send(const unsigned char* buf, socklen_t clientlen, struct sockaddr_in clientaddr) const
{
    /*
     * sendto: sends back a response to the initial sender of the datagram
     */
    cout << "Sending back: " << buf << endl;
    int n = sendto(this->sockfd, (const char *) buf, BUFSIZE, 0, (struct sockaddr *) &clientaddr, clientlen);
    if (n < 0)
        cout << "Could not send response datagram " << __func__ << "+ " << __LINE__ << endl;

    return make_pair(clientlen, clientaddr);
}


int Server::process_description_event_id1(struct pdescription* d, unsigned char* buf)
{
    memcpy(d->ack, buf+DLENGTH_S+EVENTID_S, ACK_S);
    d->textlen = bytes_to_int(d->len) - DESCRI_HEADER_SIZE;
    //d->textd = (unsigned char*) malloc(d->textlen*sizeof(unsigned char));
    d->textd = new unsigned char[d->textlen];
    memcpy(d->textd, buf+DLENGTH_S+EVENTID_S+ACK_S, d->textlen);

    return (d->textlen + DESCRI_HEADER_SIZE);
}

struct respacket* Server::process_packet(unsigned char* buf)
{
    auto* rp = new spacket;

    rp->eventdescri = vector<pdescription*>();

    memcpy(rp->version, buf, VERSION_S);
    memcpy(rp->srcip, buf+VERSION_S, SRCIP_S); // +1
    memcpy(rp->sidentifier, buf+VERSION_S+SRCIP_S, SRCID_S);
    memcpy(rp->seq, buf+VERSION_S+SRCIP_S+SRCID_S, SEQ_S);
    memcpy(rp->nbevents, buf+VERSION_S+SRCIP_S+SRCID_S+SEQ_S, NBEVENTS_S);

    int index = VERSION_S+SRCIP_S+SRCID_S+SEQ_S+NBEVENTS_S;

    int hasToBeAck = 0;
    total_events = total_events + (int) rp->nbevents[0];

    for(int i = 0; i < (int) rp->nbevents[0]; i++)
    {
        int dlen = 0;
        auto *d = new pdescription;
        memcpy(d->len, buf+index, DLENGTH_S);
        memcpy(d->eventid, buf+index+DLENGTH_S, EVENTID_S);

        int eid = bytes_to_int(d->eventid);
        switch(eid)
        {
            case 1:
                dlen = process_description_event_id1(d, buf+index);
                hasToBeAck = d->ack[0] & 1; // Since we only put events with same ACKflag together we can make the assumption of checking last one only
                break;
            case 2:
                dlen = process_description_event_id1(d, buf+index); // for the moment parse event id2 the same way of id1
                hasToBeAck = d->ack[0] & 1;
                break;
            case 3:
                // id 3
                break;
            case 4:
                // id 4 and so on
                break;
            default:
                cout << "Default id parsing "<< endl;
        }

        index = index + dlen;
        rp->eventdescri.push_back(d);
    }

    auto* rs = new respacket;
    rs->parsed = rp;
    rs->response = hasToBeAck;

    cout << "Processed the whole packet" << endl;
    return rs;
}

void Server::free_packet_struct(struct spacket* p)
{
    if(p)
    {
        for (pdescription* d : p->eventdescri)
        {
            if(d)
            {
                delete[] (d->textd);
                delete(d);
            }
        }
        p->eventdescri.clear();
        delete(p);
    }
}

int bytes_to_int(const unsigned char* bytes)
{
    int a = (int)(unsigned char)(bytes[0]) << 24 | (unsigned char)(bytes[1]) << 16 |
            (unsigned char)(bytes[2]) << 8 | (unsigned char)(bytes[3]);
    return a;
}