//
// Created by francois
//

#ifndef LAMBDA_SERVER_H
#define LAMBDA_SERVER_H
#include "httplib.h"

#include "common.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unordered_map>

#include <vector>
#include <unistd.h>
#include <netdb.h>

using namespace std;

struct pdescription{
    unsigned char len[DLENGTH_S]; // total description struct length in byte
    unsigned char eventid[EVENTID_S];
    unsigned char ack[ACK_S];
    unsigned char* textd; // Does not have to be text
    int textlen;
};

struct spacket{
    unsigned char version[VERSION_S];
    unsigned char srcip[SRCIP_S];
    unsigned char sidentifier[SRCID_S];
    unsigned char seq[SEQ_S];
    unsigned char nbevents[NBEVENTS_S];
    vector<pdescription*> eventdescri; // small difference 
};

struct respacket{
    /* Common field of event to be sent on the network */
    struct spacket *parsed;
    int response;
};

int bytes_to_int(const unsigned char* bytes);

class Server
{
    private:
        int sockfd;
        int portno;
        sockaddr_in serveraddr{};
        char* ip;
        unordered_map<int, string> ev_lam_map;

    public:
        Server(int port, char* address, unordered_map<int, string> m);
        ~Server();

        int get_sock();
        pair<socklen_t, struct sockaddr_in> receive(unsigned char* buf, socklen_t clientlen, struct sockaddr_in clientaddr) const;
        pair<socklen_t, struct sockaddr_in> send(const unsigned char* buf, socklen_t clientlen, struct sockaddr_in clientaddr) const;
        struct respacket* process_packet(unsigned char* buf);
        int process_description_event_id1(struct pdescription* d, unsigned char* buf);
        int process_description_event_id2(struct pdescription* d, unsigned char* buf);
        void execute_lambda_function_id2(int src, int dst, int srcport, int dstport, int protocol, int count, int64_t size);
        static void free_packet_struct(struct spacket* p);
};
#endif //LAMBDA_SERVER_H
