#ifndef _CLIENT_H_
#define _CLIENT_H_

void create_packet(char* buf, struct spacket* p); // client only

struct spacket* create_dummy(struct spacket* p); // client only 

#endif