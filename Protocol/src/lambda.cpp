//
// Created by francois
//
#define CPPHTTPLIB_OPENSSL_SUPPORT

#include "json.hpp"
#include "Server.hpp"


#include <iostream>
#include <vector>
#include <arpa/inet.h>
#include <climits>
#include <string>

extern int total_events;


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


    clientlen = sizeof(clientaddr);
    vector<int> seqPerSwitchId(nbClients, 1); // create vector of size nbclient init with 1's


    using json = nlohmann::json;
    json j;
    ifstream ifs("config.json");
    if (!ifs.is_open())
    {
        cout << "[FATAL] Path to configuration file not found " << endl;
        cout << "[FATAL] Looking for config.json in current directory" << endl;
        return false;
    }
    ifs >> j;
    ifs.close();
    
    cout << "[INFO] Configuration imported .." << endl;
    cout << j.dump(3) << endl;
                                                                /* UNORDERED MAP */
    unordered_map<int, string> configmap; // string(event number) ------> string(corresponding lambda function name (service))
    cout << "[INFO] Config size: " << j.size() << endl;

    for (unsigned int i = 1; i <= j.size(); i++)
    {
        std::string x = std::to_string(i);
        if (j.find(x) != j.end()) 
            configmap[i] = j[x];
    }

    auto *s = new Server(portno, ip, configmap);

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
            print_byte_array(buf, 28);
            struct respacket *rp = s->process_packet(buf);
            if (!rp) 
            {
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

            s->free_packet_struct(rp->parsed);
            delete(rp);
        }
    } // end while server
}