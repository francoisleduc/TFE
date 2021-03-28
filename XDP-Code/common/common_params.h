/* This common_user.h is used by userspace programs */
#ifndef __COMMON_PARAMS_H
#define __COMMON_PARAMS_H

#include <getopt.h>
#include "common_defines.h"

struct option_wrapper {
  struct option option;
  char *help;
  char *metavar;
  bool required;
};

void parse_cmdline_args(int argc, char **argv,
			const struct option_wrapper *long_options,
                        struct config *cfg);

#endif /* __COMMON_PARAMS_H */
