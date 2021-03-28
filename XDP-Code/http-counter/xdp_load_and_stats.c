/* SPDX-License-Identifier: GPL-2.0 */


/* Includes for UDP socket */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

#define BUFSIZE 1024


/* End includes for UDP packets */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>

#include <locale.h>
#include <unistd.h>

#include <bpf/bpf.h>
#include <bpf/libbpf.h>

#include <net/if.h>
#include <linux/if_link.h> /* depend on kernel-headers installed */

#include "../dependencies/common/common_params.h"
#include "../dependencies/common/common_user_bpf_xdp.h"


#include "common_kern_user.h" // current directory data structure for records

static const char *default_filename = "xdp_prog_kern.o";
static const char *default_progsec = "xdp_stats1";


static const struct option_wrapper long_options[] = {
	{{"help",        no_argument,		NULL, 'h' },
	 "Show help", false},

	{{"dev",         required_argument,	NULL, 'd' },
	 "Operate on device <ifname>", "<ifname>", true},

	{{"force",       no_argument,		NULL, 'F' },
	 "Force install, replacing existing program on interface"}
};



/* 
 * error - wrapper for perror
 */
void error(char *msg) {
    perror(msg);
    exit(0);
}


int find_map_fd(struct bpf_object *bpf_obj, const char *mapname)
{
	struct bpf_map *map;
	int map_fd = -1;

	map = bpf_object__find_map_by_name(bpf_obj, mapname);
        if (!map) {
		fprintf(stderr, "ERR: cannot find map by name: %s\n", mapname);
		goto out;
	}

	map_fd = bpf_map__fd(map);
 out:
	return map_fd;
}


void udp_client()
{
	int sockfd, portno, n;
    socklen_t serverlen;
    struct sockaddr_in serveraddr;
    struct hostent *server;
    char *hostname;
    unsigned char buf[BUFSIZE];
  
    hostname = "127.0.0.1";
    portno = 8080;

    /* socket: create the socket */
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");

    /* gethostbyname: get the server's DNS entry */
    server = gethostbyname(hostname);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host as %s\n", hostname);
        exit(0);
    }

    /* build the server's Internet address */
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
	  (char *)&serveraddr.sin_addr.s_addr, server->h_length);
    serveraddr.sin_port = htons(portno);

    /* get a message from the user */
    bzero(buf, BUFSIZE);

    unsigned char payload[235] = { 0x01, 0x7F, 0x00, 0x00, 0x01, 0x73, 0x77, 0x69, 0x74, 0x63, 0x68, 0x2D, 0x31,
    	0x00, 0x00, 0x00, 0x50, 0xDA, 0xD2, 0x1F, 0x86, 0x55, 0x00, 0x00, 0x50, 0x04, 0x02, 0xD0, 0x6C, 0xD3, 0x5F, 0xFF, 0x7F, 0x00, 0x00,
    	0x6E, 0x75, 0x6D, 0x62, 0x65, 0x72 ,0x20, 0x6F, 0x66, 0x20, 0x63, 0x6F, 0x6E, 0x6E, 0x65, 0x63, 0x74, 0x69, 0x6F, 0x6E,
    	0x20, 0x66, 0x6F, 0x72, 0x20, 0x64, 0x65, 0x73, 0x74, 0x69, 0x6E, 0x61, 0x74, 0x69, 0x6F, 0x6E, 0x20, 0x70, 0x6F, 0x72, 0x74,
    	0x20, 0x38, 0x30, 0x38, 0x30, 0x20, 0x6F, 0x6E, 0x20, 0x74, 0x69, 0x6D, 0x65, 0x20, 0x69, 0x6E, 0x74, 0x65, 0x72, 0x76, 0x61, 0x6C,
    	0x20, 0x6F, 0x66, 0x20, 0x31, 0x30, 0x73, 0x65, 0x63, 0x6F, 0x6E, 0x64, 0x73, 0x0A, 0x00, 0x00, 0x00, 0x6E, 0x75, 0x6D, 0x62, 0x65, 0x72,
    	0x20, 0x6F, 0x66, 0x20, 0x62, 0x79, 0x74, 0x65, 0x73, 0x20, 0x72 ,0x65, 0x63, 0x65, 0x6E, 0x75, 0x6D, 0x62, 0x65, 0x72, 0x20, 0x6F, 0x66, 0x20,
    	0x62, 0x79, 0x74, 0x65, 0x73, 0x20, 0x72, 0x65, 0x63, 0x65, 0x69, 0x76, 0x65, 0x64, 0x20, 0x6F, 0x6E, 0x20, 0x69, 0x6E, 0x74, 0x65 ,0x72, 0x66, 0x61, 0x63, 0x65,
    	0x20, 0x32, 0x20, 0x6F, 0x76, 0x65, 0x72, 0x20, 0x6C, 0x61, 0x73, 0x74, 0x20, 0x32, 0x30, 0x20 ,0x73, 0x65 ,0x63 ,0x6F ,0x6E, 0x64, 0x73 ,0x0A, 0x00 ,0x00 ,0x00, 0x45,
    	0x76, 0x65, 0x6E, 0x74, 0x20, 0x64, 0x65, 0x73 ,0x63, 0x72, 0x69, 0x70, 0x74, 0x69, 0x6F, 0x6E ,0x73, 0x20, 0x64, 0x6F, 0x6E ,0x27 ,0x74 ,0x20 ,0x72 ,0x65 ,0x73 ,0x70,
    	0x65, 0x63, 0x74, 0x20, 0x73, 0x69, 0x7A}; // test payload 
    memcpy(buf, payload, 235);
    /* send the message to the server */

    //printf("Sending: %s", buf);
    serverlen = sizeof(serveraddr);
    n = sendto(sockfd, (const char*) buf, BUFSIZE, 0, (struct sockaddr *)&serveraddr, serverlen);
    if (n < 0) 
      error("ERROR in sendto");
    
    /* print the server's reply */
    n = recvfrom(sockfd, buf, BUFSIZE, 0, (struct sockaddr *)&serveraddr, &serverlen);
	printf("Received %d bytes \n", n);
    if (n < 0)
      error("ERROR in recvfrom");


    buf[n] = '\0';

    printf("Echo from server: %s \n", buf);
}


struct stats_record {
	struct datarec stats[1];
};

static void stats_print_header()
{
	/* Print stats "header" */
	printf("%-12s\n", "XDP-action");
}


static void stats_print(struct stats_record *stats_rec)
{
	struct datarec *rec;

	stats_print_header(); /* Print stats "header" */

	
	char *fmt = "%-12s %'11lld pkts"
		" %'11lld bytes "
		" %'11lld Nb. SYN+ACK "
		" %'11lld Cumulativ. tcp payload bytes ";
	const char *action = "PASS"; // print only XDP_PASS action to have smaller print stat statement 

	rec  = &stats_rec->stats[0];

	printf(fmt, action, rec->rx_packets,
	       rec->rx_bytes, rec->rx_tcp_open, rec->rx_tcp_payload);
	printf("\n");
}


static bool stats_collect(int map_fd, __u32 map_type,
			  struct stats_record *stats_rec)
{

	__u32 key = 0;
	struct datarec *rec = &stats_rec->stats[key];
	switch (map_type) {
	case BPF_MAP_TYPE_ARRAY:
		if ((bpf_map_lookup_elem(map_fd, &key, rec)) != 0) {
			fprintf(stderr,
				"ERR: bpf_map_lookup_elem failed key:0x%X\n", key);
		}
		break;
	default:
		fprintf(stderr, "ERR: Unknown map_type(%u) cannot handle\n",
			map_type);
		return false;
		break;
	}

	return true;
}

static void stats_poll(int map_fd, __u32 map_type, int interval)
{
	struct stats_record record = { 0 };

	struct datarec *rec;

	bool flag_pkts = false;
	bool flag_bytes = false;

	/* Get initial reading quickly */

	while (1) {
		stats_collect(map_fd, map_type, &record);
		stats_print(&record);

		rec  = &record.stats[0]; // Here we are interested in XDP_PASS packet increment XDP_PASS = 2

		if(rec->rx_packets > 20 && flag_pkts == false)
		{
			printf("Threshold reached for number of packets: should trigger a remote lambda function \n");
			udp_client();
			flag_pkts = true;
		}

		if(rec->rx_bytes > 5000 && flag_bytes == false)
		{
			printf("Threshold reached for number of bytes: should trigger a remote lambda function \n");
			udp_client();
			flag_bytes = true;
		}

		sleep(interval); // so that user space does not poll constantly but every 2seconds for example

	}
}


static int __check_map_fd_info(int map_fd, struct bpf_map_info *info,
			       struct bpf_map_info *exp)
{
	__u32 info_len = sizeof(*info);
	int err;

	if (map_fd < 0)
		return EXIT_FAIL;

        /* BPF-info via bpf-syscall */
	err = bpf_obj_get_info_by_fd(map_fd, info, &info_len);
	if (err) {
		fprintf(stderr, "ERR: %s() can't get info - %s\n",
			__func__,  strerror(errno));
		return EXIT_FAIL_BPF;
	}

	if (exp->key_size && exp->key_size != info->key_size) {
		fprintf(stderr, "ERR: %s() "
			"Map key size(%d) mismatch expected size(%d)\n",
			__func__, info->key_size, exp->key_size);
		return EXIT_FAIL;
	}
	if (exp->value_size && exp->value_size != info->value_size) {
		fprintf(stderr, "ERR: %s() "
			"Map value size(%d) mismatch expected size(%d)\n",
			__func__, info->value_size, exp->value_size);
		return EXIT_FAIL;
	}
	if (exp->max_entries && exp->max_entries != info->max_entries) {
		fprintf(stderr, "ERR: %s() "
			"Map max_entries(%d) mismatch expected size(%d)\n",
			__func__, info->max_entries, exp->max_entries);
		return EXIT_FAIL;
	}
	if (exp->type && exp->type  != info->type) {
		fprintf(stderr, "ERR: %s() "
			"Map type(%d) mismatch expected type(%d)\n",
			__func__, info->type, exp->type);
		return EXIT_FAIL;
	}

	return 0;
}





int main(int argc, char **argv)
{
	struct bpf_map_info map_expect = { 0 };
	struct bpf_map_info info = { 0 };
	struct bpf_object *bpf_obj;
	int stats_map_fd;
	int interval = 2;
	int err;

	struct config cfg = {
		.xdp_flags = XDP_FLAGS_UPDATE_IF_NOEXIST | XDP_FLAGS_DRV_MODE,
		.ifindex   = -1,
	};
	/* Set default BPF-ELF object file and BPF program name */
	strncpy(cfg.filename, default_filename, sizeof(cfg.filename));
	strncpy(cfg.progsec,  default_progsec,  sizeof(cfg.progsec));
	
	/* Cmdline options can change progsec */
	parse_cmdline_args(argc, argv, long_options, &cfg);

	/* Required option */
	if (cfg.ifindex == -1) {
		fprintf(stderr, "ERR: required option --dev missing\n");
		//usage(argv[0], __doc__, long_options, (argc == 1));
		return EXIT_FAIL_OPTION;
	}


	bpf_obj = load_bpf_and_xdp_attach(&cfg);
	if (!bpf_obj)
		return EXIT_FAIL_BPF;

	
	printf("Success: Loaded BPF-object(%s) and used section(%s)\n",
	       cfg.filename, cfg.progsec);
	printf(" - XDP prog attached on device:%s(ifindex:%d)\n",
	       cfg.ifname, cfg.ifindex);
	

	// Look for map file, file descriptor
	stats_map_fd = find_map_fd(bpf_obj, "xdp_stats_map");
	if (stats_map_fd < 0) { // could not find map fd
		xdp_link_detach(cfg.ifindex, cfg.xdp_flags, 0);
		return EXIT_FAIL_BPF;
	}

	/* check map info, e.g. datarec is expected size */
	map_expect.key_size    = sizeof(__u32);
	map_expect.value_size  = sizeof(struct datarec);
	map_expect.max_entries = 1;
	err = __check_map_fd_info(stats_map_fd, &info, &map_expect);
	if (err) {
		fprintf(stderr, "ERR: map via FD not compatible\n");
		return err;
	}
	
	printf("\nCollecting stats from BPF map\n");
	printf(" - BPF map (bpf_map_type:%d) id:%d name:%s"
	       " key_size:%d value_size:%d max_entries:%d\n",
	       info.type, info.id, info.name,
	       info.key_size, info.value_size, info.max_entries
	       );
	

	//udp_client();
	stats_poll(stats_map_fd, info.type, interval);
	return EXIT_OK;
}
