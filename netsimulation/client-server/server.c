/* 
 * udpserver.c - A simple UDP echo server 
 * usage: udpserver <port>
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdbool.h>


#include "common.h"

static struct spacket* process_packet(char* buf)
{
  struct spacket* rp = malloc(sizeof(struct spacket));
  if(!rp)
    return NULL;

  rp->eventids = newLinkedList();
  rp->eventdescri = newLinkedList();

  if(!rp->eventids || !rp->eventdescri)
    return NULL;

  // memcpy(destination, source, length)
  memcpy(rp->version, buf, VERSION_S);
  memcpy(rp->srcip, buf+VERSION_S, SRCIP_S); // +1
  memcpy(rp->devname, buf+VERSION_S+SRCIP_S, DEVNAME_S); // +5 
  memcpy(rp->port, buf+VERSION_S+SRCIP_S+DEVNAME_S, PORT_S); // + 25
  memcpy(rp->nbevents, buf+VERSION_S+SRCIP_S+DEVNAME_S+PORT_S, NBEVENTS_S); // +26

  int index = VERSION_S+SRCIP_S+DEVNAME_S+PORT_S+NBEVENTS_S;
  for(int i = 0; i < (int) rp->nbevents[0]; i++)
  {
      unsigned char* currentid = malloc(EVENTID_S*sizeof(unsigned char));
      memcpy(currentid, buf+index, EVENTID_S);
      insertInLinkedList(rp->eventids, currentid);
      index = index + EVENTID_S;
  }

  for(int i = 0; i < (int) rp->nbevents[0]; i++)
  {
      char* currentdescri = malloc(DESCRI_S*sizeof(char));
      memcpy(currentdescri, buf+index, DESCRI_S);
      insertInLinkedList(rp->eventdescri, currentdescri);
      index = index + DESCRI_S;
  }

  return rp;
}


int main(int argc, char **argv) {
  int sockfd; /* socket */
  int portno; /* port to listen on */
  socklen_t clientlen; /* byte size of client's address */
  struct sockaddr_in serveraddr; /* server's addr */
  struct sockaddr_in clientaddr; /* client addr */
  unsigned char buf[BUFSIZE]; /* message buf */
  char *hostaddrp = ""; /* dotted decimal host addr string */
  int optval; /* flag value for setsockopt */
  int n; /* message byte size */
  bool responding = true; // FALSE 
  /* 
   * check command line arguments 
   */
  if (argc != 2) {
  	log_fatal("usage: %s <port>", __FILE__, __func__, __LINE__);
    exit(1);
  }
  portno = atoi(argv[1]);

  /* 
   * socket: create the parent socket 
   */
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0)
  	log_error("Could not open socket", __func__, __LINE__);

  /* setsockopt: Handy debugging trick that lets 
   * us rerun the server immediately after we kill it; 
   * otherwise we have to wait about 20 secs. 
   * Eliminates "ERROR on binding: Address already in use" error. 
   */
  optval = 1;
  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, 
	     (const void *)&optval , sizeof(int));

  /*
   * build the server's Internet address
   */
  bzero((char *) &serveraddr, sizeof(serveraddr));
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Server listens on that ip 
  serveraddr.sin_port = htons((unsigned short)portno);


  if (bind(sockfd, (struct sockaddr *) &serveraddr, 
	   sizeof(serveraddr)) < 0)
  	log_error("Could not bind socket", __func__, __LINE__); // I guess I could put this error as fatal and exit()

  /* 
   * main loop: wait for a datagram, then send ACK 
   */
  clientlen = sizeof(clientaddr);
  while (1) {

    /*
     * recvfrom: receive a UDP datagram from a client
     */
    bzero(buf, BUFSIZE);
    n = recvfrom(sockfd, buf, BUFSIZE, 0,
		 (struct sockaddr *) &clientaddr, &clientlen);
    if (n < 0)
      log_error("Could not read received bytes", __func__, __LINE__);


    hostaddrp = inet_ntoa(clientaddr.sin_addr);
    if (hostaddrp == NULL)
      log_error("Could not parse datagram source address", __func__, __LINE__);
    printf("server received datagram from (%s)\n", hostaddrp);
    printf("server received %d bytes \n", n);
    
    struct spacket* rp = process_packet(buf);
    if(rp)
    {
      print_packet(rp);
    }
    
    /* 
     * sendto: sends back a response to the initial sender of the datagram
     */
    
    print_byte_array(buf, BUFSIZE); 
    if(responding) 
    {
      char* response = "ACK\n";
      bzero(buf, BUFSIZE); // We use the same buffer so it seems to erase what's inside each time we're done with it
      bcopy(response, buf, strlen(response));

      printf("Sending back: %s", buf);
      n = sendto(sockfd, buf, strlen(buf), 0, 
           (struct sockaddr *) &clientaddr, clientlen);
      if (n < 0) 
        log_error("Could not send response datagram", __func__, __LINE__);
    }

  } // end while server 

  return 0;
}