#include <iostream>
#include <arpa/inet.h>
#include <cstring>

/*
 * udpserver.c - A simple UDP echo server
 * usage: udpserver <port>
 */

#include "Server.hpp"

using namespace std;

int total_events = 0;



int bytes_to_int(const unsigned char* bytes)
{
    int a = (int)(unsigned char)(bytes[0]) << 24 | (unsigned char)(bytes[1]) << 16 |
            (unsigned char)(bytes[2]) << 8 | (unsigned char)(bytes[3]);
    return a;
}


int64_t bytes_to_int64(const unsigned char* bytes)
{
    int64_t a = (int64_t) (int64_t) (bytes[0]) << 56 | (int64_t) (bytes[1]) << 48 | 
        (int64_t) (bytes[2]) << 40 | (int64_t) (bytes[3]) << 32 | (int64_t) (bytes[4]) << 24 
        | (int64_t) (bytes[5]) << 16 | (int64_t) (bytes[6]) << 8 | (int64_t) (bytes[7]);

    return a;
}

// Read 16bits from int32
int get_src_port(const int ports)
{
    int src = ports >> 16;
    return src;
}

int get_dst_port(const int ports)
{
    int dst = ports & 0x0000FFFF;
    return dst;
}

Server::Server(int port, char* address, unordered_map<int, string> m)
{
    this->portno = port;
    this->ip = address;
    this->ev_lam_map = m;
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


void Server::execute_lambda_function_id2(int src, int dst, int srcport, int dstport, int protocol, int count, int64_t size)
{

    string s = to_string(src);
    string d = to_string(dst);
    string sp = to_string(srcport);
    string dp = to_string(dstport);
    string pro = to_string(protocol);
    string c = to_string(count);
    string si = to_string(size);

    const char* path = "/api/v1/namespaces/default/services/";
    const char* serviceName = this->ev_lam_map[2].c_str();
    const char* end = ":http-function-port/proxy/";
    httplib::Client cli("localhost", 8001);

    string jsonStringified = "{\"srcip\":" + s + ", " + "\"dstip\":" + d + ", " + "\"srcport\":" + sp + ", " + "\"dstport\":" + dp + ", " +
        "\"protocol\":" + pro + ", " + "\"count\":" + c + ", " + "\"size\":" + si + "}";

    char result[BUFSIZE];   // array to hold the result.
    bzero(result, BUFSIZE);
    strcat(result, path);
    strcat(result, (char*)serviceName); // copy string one into the result.
    strcat(result, end);
    cout << "FULL URL: " << result << endl;
    // hello is the service deployed corresponding to the lambda function 
    auto p = cli.Post(result, jsonStringified, "application/json");

    // Output the response of the lambda function 
    cout << "Status code : " << p->status << endl;
    cout << "Text : " << p->body << endl;
}


int Server::process_description_event_id1(struct pdescription* d, unsigned char* buf)
{
    memcpy(d->ack, buf+DLENGTH_S+EVENTID_S, ACK_S);
    d->textlen = bytes_to_int(d->len) - DESCRI_HEADER_SIZE;
    d->textd = new unsigned char[d->textlen];
    memcpy(d->textd, buf+DLENGTH_S+EVENTID_S+ACK_S, d->textlen);

    int src = bytes_to_int(d->textd);
    int dst = bytes_to_int((d->textd)+4);
    int dstport = bytes_to_int((d->textd)+8);
    int protocol = bytes_to_int((d->textd)+12);
    int nbempty_udp = bytes_to_int((d->textd)+16);
    int nbsyn = bytes_to_int((d->textd)+20);

    printf("Received: src: %d , dst: %d , port %d , protocol %d , nbempty %d , nbsyn %d \n", src, dst, dstport, protocol, nbempty_udp, nbsyn);
    //execute_lambda_function(1);
    return (d->textlen + DESCRI_HEADER_SIZE);
}



int Server::process_description_event_id2(struct pdescription* d, unsigned char* buf)
{
    memcpy(d->ack, buf+DLENGTH_S+EVENTID_S, ACK_S);
    d->textlen = bytes_to_int(d->len) - DESCRI_HEADER_SIZE;
    d->textd = new unsigned char[d->textlen];
    memcpy(d->textd, buf+DLENGTH_S+EVENTID_S+ACK_S, d->textlen);

    int src = bytes_to_int(d->textd);
    int dst = bytes_to_int((d->textd)+4);
    int ports = bytes_to_int((d->textd)+8);
    int protocol = bytes_to_int((d->textd)+12);
    int count = bytes_to_int((d->textd)+16);
    int64_t size = bytes_to_int64((d->textd)+20);

    int dstp = get_dst_port(ports);
    int srcp = get_src_port(ports);
    printf("Received: src: %d , dst: %d , ports %d , protocol %d , count %d , size %ld \n", src, dst, ports, protocol, count, size);
    execute_lambda_function_id2(src, dst, srcp, dstp, protocol, count, size);

	// print description to show event was correctly sent from switch to lambda server
	 
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
                dlen = process_description_event_id2(d, buf+index);
                hasToBeAck = 1;
			    cout << "Received id 2 " << endl;
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

