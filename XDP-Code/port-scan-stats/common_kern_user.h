// SPDX-License-Identifier: (GPL-2.0 OR BSD-2-Clause)
// Copyright (c) 2018 Netronome Systems, Inc.

#define MAX_MAP_SIZE 200000
#define NANO_GAP 2000000000

// 2^18 = 262144
// Equal to 2seconds

struct scan {
    __u32 src;
    __u32 dst;
    __u32 srcv6[4];
    __u32 dstv6[4];
    __u32 dstport;
    __u32 csyn;
    __u32 crst;
    __u32 empty_udp;
    __u64 timestamp_start;
 	__u64 timestamp_last_m;
 	__u8 protocol;
};