/* SPDX-License-Identifier: GPL-2.0 */


/* Includes for UDP socket */

#include <string.h>
#include <unistd.h>
#include <netdb.h> 

/* End includes for UDP packets */
#include <getopt.h>

#include <locale.h>
#include <unistd.h>

#include <bpf/bpf.h>
#include <bpf/libbpf.h>

#include <net/if.h>
#include <linux/if_link.h> /* depend on kernel-headers installed */
#include <pthread.h>  // pthread 

#include "../dependencies/common/common_params.h"
#include "../dependencies/common/common_user_bpf_xdp.h"


#include "common_kern_user.h" // current directory data structure for records

#include "../protocol/common.h"
#include "../protocol/log.h"
#include "../protocol/client.h"


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


/* Additional global variable used by the protocol */

extern List* eventsQACK;
extern List* eventsQNOACK;

extern pthread_mutex_t lock;
extern bool terminated;
extern int seqNB;

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


void add_custom_event(int id)
{
    /* To avoid race conditions when adding events to a queue that is used 
    *  by both the communication thread and the polling thread 
    * 
    */
    pthread_mutex_lock(&lock);
        
    unsigned char s[20] = {0};
    memcpy(s, "Added Event for now\0", 20);
    if(id == 1)
    {
        struct event* newEvent = make_event(id, s, 1, 20);
        insert_in_list(eventsQACK, newEvent);
    }
    else
    {
        struct event* newEvent = make_event(id, s, 0, 20);
        insert_in_list(eventsQNOACK, newEvent);
    }
    pthread_mutex_unlock(&lock);
	return;
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
	const char *action = "PASS"; // print only XDP_PASS action to have smaller print statistic statement 

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
	//bool flag_bytes = false; // uncomment if needed

	/* Get initial reading quickly */

	while (1) {
		stats_collect(map_fd, map_type, &record);
		stats_print(&record);

		rec  = &record.stats[0]; // Here we are interested in XDP_PASS packet increment XDP_PASS = 2

		if(rec->rx_packets > 10 && flag_pkts == false)
		{
			printf("Threshold reached for number of packets: should trigger a remote lambda function \n");
			add_custom_event(1);
			flag_pkts = true;
		}

		if(rec->rx_bytes > 1200)
		{
			printf("Threshold reached for number of bytes: should trigger a remote lambda function \n");
			add_custom_event(2);
			//flag_bytes = true;
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


static void* lambda_communication_thread(void* input)
{
    socklen_t serverlen = 0;
    int n = 0;
    bool t = true;
    int nbevents = 0;

    int msec = 0, trigger1 = 500, trigger2 = 1000; /* 500ms */ /* and */ /* 1000ms */
    clock_t before1 = clock(); // associated to trigger1
    clock_t before2 = clock(); // associated to trigger2

    struct spacket* pts = NULL;
    struct event** selectedEvts = malloc(MAX_EVENT_P*sizeof(struct event));
    struct event** selectedEvtsNoAck = malloc(MAX_EVENT_P*sizeof(struct event));

    for(int x = 0; x < MAX_EVENT_P; x++)
    {
        selectedEvts[x] = NULL;
        selectedEvtsNoAck[x] = NULL;
    }

    log_info("Setting flags \n");

    // Non blocking receive method flags 
    int flags = fcntl(((struct args*)input)->sockfd, F_GETFL);
    flags |= O_NONBLOCK;
    fcntl(((struct args*)input)->sockfd, F_SETFL, flags);

    while(t)
    {
        clock_t difference1 = clock() - before1;
        msec = difference1 * 1000 / CLOCKS_PER_SEC;
        if(msec > trigger1 && nbevents > 0)
        {
            // Resend because timer expired and still did not receive an ACK for the last packet 
            log_info("Timer alert: Retransmission of last packet \n");
            serverlen = sizeof(((struct args*)input)->serveraddr);
            n = sendto(((struct args*)input)->sockfd, (const char*) ((struct args*)input)->buf, BUFSIZE, 0, 
                (struct sockaddr *)&(((struct args*)input)->serveraddr), serverlen);
            if (n < 0)
            {
                log_error("Could not send datagram", __func__, __LINE__);
            }
            else
            {
                before1 = clock(); // reset timer 
            }
        }

        if(msec > trigger1 && nbevents == 0)
        {
            // send new 
            //log_info("Timer alert: Sending now because queue was empty last time \n");
            nbevents = send_new(pts, (struct args*) input, lock, selectedEvts, serverlen, true);
            before1 = clock();
        }


        /* Here we check if the events that don't need an ACK can be sent depending on their own timer 
         * (which is most likely to be different from the previous one)
        */
        clock_t difference2 = clock() - before2;
        msec = difference2 * 1000 / CLOCKS_PER_SEC;
        if(msec > trigger2)
        {
            //log_info("Timer alert: Sending from non essential queue \n");
            bzero(((struct args*)input)->bufSecondary, BUFSIZE);
            int noa = send_new(pts, (struct args*) input, lock, selectedEvtsNoAck, serverlen, false);
            if(noa > 0)
            {
                pthread_mutex_lock(&lock);
                free_buffer_waiting_events(selectedEvtsNoAck, eventsQNOACK);
                pthread_mutex_unlock(&lock);
            }
            
            before2 = clock();
        }
        
        /* print the server's reply */
        n = recvfrom(((struct args*)input)->sockfd, ((struct args*)input)->buf, BUFSIZE, O_NONBLOCK, 
            (struct sockaddr *)&(((struct args*)input)->serveraddr), &serverlen);

        if (n > 0 && n <= 1024)
        {
            if(((struct args*)input)->buf[0] == 'A' && ((struct args*)input)->buf[1] == 'C' && ((struct args*)input)->buf[2] == 'K')
            {
                pthread_mutex_lock(&lock);
                free_buffer_waiting_events(selectedEvts, eventsQACK); // remove from Q and selectedEvts array
                pthread_mutex_unlock(&lock);

                ((struct args*)input)->buf[n] = '\0';
                printf("Echo from server: %s \n", ((struct args*)input)->buf);
                log_info("Acknowledgment received: sending new packet \n");

                bzero(((struct args*)input)->buf, BUFSIZE);
                nbevents = send_new(pts,(struct args*) input, lock, selectedEvts, serverlen, true);
                before1 = clock();
                before2 = clock();
            }
        }
    }

    free(selectedEvts);
    free(selectedEvtsNoAck);

    free((struct args*)input);
    terminated = true;
    pthread_detach(pthread_self()); // To avoid memory leaks 
    pthread_exit(NULL);
}




int main(int argc, char **argv)
{
    /* Lambda Protocol */
    int sockfd;
    struct sockaddr_in serveraddr;
    struct hostent *server;
    char buf[BUFSIZE];
    char bufSecondary[BUFSIZE];
    /* BPF */

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
		return EXIT_FAIL_OPTION;
	}


	bpf_obj = load_bpf_and_xdp_attach(&cfg);
	if (!bpf_obj)
		return EXIT_FAIL_BPF;

	
	printf("Success: Loaded BPF-object(%s) and used section(%s)\n",
	       cfg.filename, cfg.progsec);
	printf(" - XDP prog attached on device:%s(ifindex:%d)\n",
	       cfg.ifname, cfg.ifindex);
	
    printf("Success: Parsed source IP address (%s) \n", cfg.ipstr);
    printf("Success: Parsed unique identifier of the device (%d) \n", cfg.deviceid);
    printf("Success: Parsed lambda server port number (%d) \n", cfg.serverport);
    printf("Success: Parsed destination IP address (%s) \n", cfg.desipstr);


    /** Network **/


    /* socket: create the socket */
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) 
       log_error("Could not open socket", __func__, __LINE__);

    /* gethostbyname: get the server's DNS entry */
    server = gethostbyname(cfg.desipstr);
    if (server == NULL) {
       log_fatal("No such host", __FILE__, __func__, __LINE__);
       exit(0);
    }

    /* build the server's Internet address */
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
      (char *)&serveraddr.sin_addr.s_addr, server->h_length);
    serveraddr.sin_port = htons(cfg.serverport);

    /* get a message from the user */
    bzero(buf, BUFSIZE);
    bzero(bufSecondary, BUFSIZE);

    // Instantiation of the two lists used to send events 
    eventsQACK = new_list();
    eventsQNOACK = new_list();

    unsigned char myip[SRCIP_S] = {0x7F, 0x0, 0x0, 0x1}; // this should be converted from the ip string and checked && cfg.ipstr
    unsigned char version[VERSION_S] = {0x1}; // same v1.0

    /* BPF */


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
	

	
    /* Network (extended.) init communication thread */

    struct args *in = (struct args *)malloc(sizeof(struct args));
    
    in->sockfd = sockfd;
    memcpy(in->buf, buf, BUFSIZE);
    memcpy(in->bufSecondary, bufSecondary, BUFSIZE);
    memcpy(in->myip, myip, SRCIP_S);
    in->identifierNumber = cfg.deviceid;
    memcpy(in->version, version, VERSION_S);
    in->serveraddr = serveraddr;

    pthread_t tid; 

    if(pthread_mutex_init(&lock, NULL) != 0)
    {
        log_fatal("Mutex init failed", __FILE__, __func__, __LINE__);
        return 1;
    }

    pthread_create(&tid, NULL, lambda_communication_thread, (void *)in); // This thread is dedicated communicate with the lambda server

    
	stats_poll(stats_map_fd, info.type, interval);

    free_list_event(eventsQACK);
    free_list_event(eventsQNOACK);
	return EXIT_OK;
}
