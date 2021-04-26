//
// Created by francois
//
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"

#include <iostream>
#include <vector>
#include <arpa/inet.h>
#include <cstring>
#include <climits>
#include "Server.h"


extern int total_events;

int main(int argc, char** argv) {
    int portno; /* port to listen on */
    socklen_t clientlen; /* byte size of client's address */
    struct sockaddr_in clientaddr{}; /* client addr */
    unsigned char buf[BUFSIZE]; /* message buf */
    char *hostaddrp; /* dotted decimal host addr string */
    bool responding = true;
    float droprate = 0.0;
    int nbClients = 0;
    char const *response = "ACK\n";


    if (argc != 5) 
    {
        cout << "usage: <ip> <port> <droprate> <max client nb> " << endl;
        exit(1);
    }
    char *ip = argv[1];
    portno = atoi(argv[2]);
    droprate = atof(argv[3]);
    nbClients = atoi(argv[4]);
    cout << "[INFO] Listening on port: " << portno << " with droprate: " << droprate << endl;

    auto *s = new Server(portno, ip);

    clientlen = sizeof(clientaddr);
    vector<int> seqPerSwitchId(nbClients, 1); // create vector of size nbclient init with 1's


    

    
    while (true) {
        bzero(buf, BUFSIZE);
        pair<socklen_t, struct sockaddr_in> p = s->receive(buf, clientlen, clientaddr); // Store original sender info

        hostaddrp = inet_ntoa(p.second.sin_addr);
        if (hostaddrp == nullptr)
            cout << "Could not parse datagram source address " << __func__ << "+ " << __LINE__ << endl;
        cout << "server received datagram from " << hostaddrp << endl;


        double rd = drand48();
        if ((droprate < rd) && responding) 
        {
            struct respacket *rp = s->process_packet(buf);
            if (!rp) {
                exit(1);
            }

            int receivedId = bytes_to_int(rp->parsed->sidentifier);
            int receivedSeq = bytes_to_int(rp->parsed->seq);
            bzero(buf, BUFSIZE);
            bcopy(response, buf, strlen(response));

            if (rp->response == 1) // Requires an acknowledgment
            {
                if (seqPerSwitchId[receivedId] == receivedSeq) 
                {
                    // IF SEQUENCE NUMBER MATCHES - INCREMENT EXPECTED SQN BY 1
                    seqPerSwitchId[receivedId] = (seqPerSwitchId[receivedId] % INT_MAX) + 1;
                    cout << "Expected sequence number" << endl;
                    cout << "Switch id : " << receivedId << endl;
                }
                s->send(buf, p.first, p.second); // send back to original sender
            } 
            else 
            {
                cout << "Packet received does not require an acknowledgment " << endl;
            }

            httplib::Client cli("localhost", 8001);
            httplib::Params params{
            { "data", "This is sent from the lambda server to the cluster for exectution via http trigger, if sent back then function is working" }
            };
            auto p = cli.Post("/api/v1/namespaces/default/services/hello:http-function-port/proxy/", params);
            cout << "Status code : " << p->status << endl;
            cout << "Text : " << p->body << endl;
            cout << "total_events : " << total_events << endl;
            
            s->free_packet_struct(rp->parsed);
            delete(rp);

            
        }
    } // end while server
}