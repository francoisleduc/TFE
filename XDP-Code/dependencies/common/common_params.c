#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <getopt.h>
#include <errno.h>

#include <net/if.h>
#include <linux/if_link.h> /* XDP_FLAGS_* depend on kernel-headers installed */
#include <linux/if_xdp.h>

#include "common_params.h"



#define BUFSIZE 30


int option_wrappers_to_options(const struct option_wrapper *wrapper,
				struct option **options)
{
	int i, num;
	struct option *new_options;
	for (i = 0; wrapper[i].option.name != 0; i++) {}
	num = i;

	new_options = malloc(sizeof(struct option) * num);
	if (!new_options)
		return -1;
	for (i = 0; i < num; i++) {
		memcpy(&new_options[i], &wrapper[i], sizeof(struct option));
	}

	*options = new_options;
	return 0;
}

void parse_cmdline_args(int argc, char **argv,
			const struct option_wrapper *options_wrapper,
                        struct config *cfg)
{
	struct option *long_options;
	int longindex = 0;
	int opt;
	if (option_wrappers_to_options(options_wrapper, &long_options)) {
		fprintf(stderr, "Unable to malloc()\n");
		exit(EXIT_FAIL_OPTION);
	}

	/* Parse commands line args */
	while ((opt = getopt_long(argc, argv, "l:p:s:n:hd:F",
				  long_options, &longindex)) != -1) {
		switch (opt) {
		case 'd':
			if (strlen(optarg) >= IF_NAMESIZE) {
				fprintf(stderr, "ERR: --dev name too long\n");
				goto error;
			}
			cfg->ifname = (char *)&cfg->ifname_buf;
			strncpy(cfg->ifname, optarg, IF_NAMESIZE);
			cfg->ifindex = if_nametoindex(cfg->ifname);
			if (cfg->ifindex == 0) {
				fprintf(stderr,
					"ERR: --dev name unknown err(%d):%s\n",
					errno, strerror(errno));
				goto error;
			}
			break;
		case 's':
			cfg->ipstr = malloc(16*sizeof(char));
			if(strlen(optarg) > 15)
			{
				fprintf(stderr,
					"ERR: Length of -s IP string format too long\n");
				goto error;
			}
			strncpy(cfg->ipstr, optarg, strlen(optarg));
			cfg->ipstr[strlen(optarg)] = '\0';
			break;
		case 'n':
			cfg->deviceid = atoi(optarg);
			break;
		case 'p':
			cfg->serverport = atoi(optarg);
			break;
		case 'l':
			cfg->desipstr = malloc(16*sizeof(char));
			if(strlen(optarg) > 15)
			{
				fprintf(stderr,
					"ERR: Length of -l IP string format too long\n");
				goto error;
			}
			strncpy(cfg->desipstr, optarg, strlen(optarg));
			cfg->desipstr[strlen(optarg)] = '\0';
			break;
		case 'F':
			cfg->xdp_flags &= ~XDP_FLAGS_UPDATE_IF_NOEXIST;
			break;
		case 'h':
			/* fall-through */
		error:
		default:
			free(long_options);
			exit(EXIT_FAIL_OPTION);
		}
	}
	free(long_options);
}
