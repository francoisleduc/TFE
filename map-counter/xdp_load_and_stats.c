/* SPDX-License-Identifier: GPL-2.0 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>

#include <locale.h>
#include <unistd.h>
#include <time.h>

#include <bpf/bpf.h>
#include <bpf/libbpf.h>

#include <net/if.h>
#include <linux/if_link.h> /* depend on kernel-headers installed */

#include "../common/common_params.h"
#include "../common/common_user_bpf_xdp.h"


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


int find_map_fd(struct bpf_object *bpf_obj, const char *mapname)
{
	struct bpf_map *map;
	int map_fd = -1;

	/* Lesson#3: bpf_object to bpf_map */
	map = bpf_object__find_map_by_name(bpf_obj, mapname);
        if (!map) {
		fprintf(stderr, "ERR: cannot find map by name: %s\n", mapname);
		goto out;
	}

	map_fd = bpf_map__fd(map);
 out:
	return map_fd;
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

	/* Print for each XDP actions stats */
	
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
	/* Collect all XDP actions stats  */
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
			printf("Threshold reached for number of packets: lambda triggering \n");
			flag_pkts = true;
		}

		if(rec->rx_bytes > 2000 && flag_bytes == false)
		{
			printf("Threshold reached for number of bytes: lambda triggering \n");
			flag_bytes = true;
		}

		sleep(interval);
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
	

	stats_poll(stats_map_fd, info.type, interval);
	return EXIT_OK;
}
