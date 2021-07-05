// SPDX-License-Identifier: (GPL-2.0 OR BSD-2-Clause)
// Copyright (c) 2018 Netronome Systems, Inc.

#define MAX_MAP_SIZE 262144
#define NANO_GAP 2000000000

// 2^18 = 262144
// Equal to 2seconds

struct flow_meta {
	__u32 src;
	__u32 dst;
	__u32 srcv6[4];
	__u32 dstv6[4];
	__u32 ports;
	__u16 port16[2];
	__u8 protocol;
    __u32 count;
	__u64 bytes;
	__u64 timestamp_start;
 	__u64 timestamp_last_m;
};
