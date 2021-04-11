//
// Created by francois on 11/04/21.
//

#ifndef COMMON_H
#define COMMON_H

#define BUFSIZE 1024


#define VERSION_S 1
#define SRCIP_S 4
#define NBEVENTS_S 1
#define SRCID_S 4
#define SEQ_S 4

#define EVENTID_S 4
#define DLENGTH_S 4
#define ACK_S 1 // Should not be changed

#define DESCRI_HEADER_SIZE (EVENTID_S + DLENGTH_S + ACK_S)
#define PACKET_HEADER_SIZE (VERSION_S+SRCIP_S+SRCID_S+SEQ_S+NBEVENTS_S)

#endif //COMMON_H