// SPDX-License-Identifier: (GPL-2.0 OR BSD-2-Clause)
// Copyright (c) 2018 Netronome Systems, Inc.

#include <stdbool.h>
#include <stddef.h>
#include <linux/bpf.h>
#include <linux/if_ether.h>
#include <linux/in.h>
#include <linux/ip.h>
#include <linux/ipv6.h>
#include <linux/string.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_endian.h>
#include "../dependencies/headers/jhash.h"
#include "common_kern_user.h"


#define bpf_debug(fmt, ...)                          \
    ({                                               \
        char ____fmt[] = fmt;                        \
        bpf_trace_printk(____fmt, sizeof(____fmt),   \
            ##__VA_ARGS__);                          \
    })


struct bpf_map_def SEC("maps") port_map = {
	.type = BPF_MAP_TYPE_ARRAY,
	.key_size = sizeof(__u32),
	.value_size = sizeof(struct scan),
	.max_entries = MAX_MAP_SIZE,
};

static __always_inline __u32 hash_tuples(struct scan *f, bool is_ip6)
{
	__u32 a;
	__u32 b;

	/* hash packet ip and ports to obtain a key for rss indirection tbl */
	if (is_ip6) {
		a = jhash2(f->srcv6, 4, 0xc55);
		b = jhash2(f->dstv6, 4, 0x1234);
		return jhash_3words(a, b, f->dstport, 0xeb0f);
	} else {
		return jhash_3words(f->src, f->dst, f->dstport, 0xeb0f);
	}
}

static __always_inline bool parse_udp(void *data, __u64 off, void *data_end,
				      struct scan *f)
{
	struct udphdr *udp;

	udp = data + off;
	if (udp + 1 > data_end)
		return false;

	f->dstport = bpf_ntohs(udp->dest);
	if(data_end - (void*)udp - 8 <= 0)
	{
		f->empty_udp++;
		bpf_debug("Increment udp empty counter \n");
	}
	
	//bpf_debug("UDP length: %d \n", data_end - (void*)udp); // minus UDP header size = 8  
	return true;
}

static __always_inline bool parse_tcp(void *data, __u64 off, void *data_end,
				      struct scan *f)
{
	struct tcphdr *tcp;

	tcp = data + off;
	if (tcp + 1 > data_end)
		return false;

	f->dstport= bpf_ntohs(tcp->dest);
	if(tcp->syn == 1)
	{
		f->csyn++;
		bpf_debug("tcp syn: %d \n", tcp->syn);
	}
	
	if(tcp->rst == 1)
	{
		f->crst++;
		bpf_debug("tcp rst: %d \n", tcp->rst);

	}
	return true;
}

static __always_inline bool parse_ip4(void *data, __u64 off, void *data_end,
				      struct scan *f)
{
	struct iphdr *iph;

	iph = data + off;
	if (iph + 1 > data_end)
		return false;

	if (iph->ihl != 5)
		return false;

	f->src = iph->saddr;
	f->dst = iph->daddr;
	f->protocol = iph->protocol;

	return true;
}

static __always_inline bool parse_ip6(void *data, __u64 off, void *data_end,
				      struct scan *f)
{
	struct ipv6hdr *ip6h;

	ip6h = data + off;
	if (ip6h + 1 > data_end)
		return false;

	memcpy(f->srcv6, ip6h->saddr.s6_addr32, 16);
	memcpy(f->dstv6, ip6h->daddr.s6_addr32, 16);
	f->protocol = ip6h->nexthdr;

	return true;
}


SEC("xdp_port")
int process_packet(struct xdp_md *ctx)
{
	void *data_end = (void *)(long)ctx->data_end;
	void *data = (void *)(long)ctx->data;
    struct scan *v;
	struct ethhdr *eth = data;
	struct scan f = {0};
	bool use_encap = false;
	bool is_ip6 = false;
	bool jhash = false;
	__u32 eth_proto;
	__u32 hash;
	__u32 key;
	__u32 off;
	/* determine hashing mode using map lookup */
	key = 0;
	jhash = true;

	/* parse packet for IP Addresses and Ports */
	off = sizeof(struct ethhdr);
	if (data + off > data_end)
		return XDP_PASS;

	eth_proto = eth->h_proto;

	if (eth_proto == bpf_htons(ETH_P_IP)) {
		if (!parse_ip4(data, off, data_end, &f))
			return XDP_PASS;
		off += sizeof(struct iphdr);
	} else if (eth_proto == bpf_htons(ETH_P_IPV6)) {
		if (!parse_ip6(data, off, data_end, &f))
			return XDP_PASS;
		is_ip6 = true;
		off += sizeof(struct ipv6hdr);
	} else {
		return XDP_PASS;
	}

	/* if IPinIP packet allow for second IP header */
	if (f.protocol == IPPROTO_IPIP) 
	{
		if (use_encap) 
		{
			if (!parse_ip4(data, off, data_end, &f))
				return XDP_PASS;
			is_ip6 = false;
		}
		off += sizeof(struct iphdr);
	} 
	else if (f.protocol == IPPROTO_IPV6) 
	{
		if (use_encap) 
		{
			if (!parse_ip6(data, off, data_end, &f))
				return XDP_PASS;
			is_ip6 = true;
		}
		off += sizeof(struct ipv6hdr);
	}

	if (data + off > data_end)
		return XDP_PASS;


	/* obtain port numbers for UDP and TCP traffic */
	if (f.protocol == IPPROTO_TCP) 
	{
		if (!parse_tcp(data, off, data_end, &f))
			return XDP_PASS;
	} 
	else if (f.protocol == IPPROTO_UDP) 
	{
		if (!parse_udp(data, off, data_end, &f))
			return XDP_PASS;
	} 
	else 
	{
		f.dstport = 0;
	}
	
	if (jhash) 
	{
		/* set map lookup key using 4 tuple hash */
		hash = hash_tuples(&f, is_ip6);
		key = hash % MAX_MAP_SIZE;
        bpf_debug("Hash: %d - key: %d \n", hash, key);
	}
	
	v = bpf_map_lookup_elem(&port_map, &key);
	if(!v)
	{
	  return XDP_PASS;
	}
	/* MODIFY THIS PART */
	if(v->csyn == 0 && v->crst == 0 && v->empty_udp == 0)
	{
	    bpf_debug("First time entry used \n");
			v->src = f.src;
		    v->dst = f.dst;
		    v->dstport = f.dstport;
		    v->protocol = f.protocol;
		    //v->empty_udp = f.empty_udp;
		v->timestamp_start = bpf_ktime_get_ns();
		v->timestamp_last_m = bpf_ktime_get_ns();
		    __sync_fetch_and_add(&v->csyn, f.csyn);
		    __sync_fetch_and_add(&v->crst, f.crst);
		    __sync_fetch_and_add(&v->empty_udp, f.empty_udp);
		//bpf_debug("Time of flow last modification %ld \n", v->timestamp_last_m);
	}
	else
	{
		if(v->src == f.src && v->dstport == f.dstport && v->dst == f.dst && v->protocol == f.protocol)
		{
			// we just update current flow, this was not a collision
			bpf_debug("Updating existing entry \n");
			__sync_fetch_and_add(&v->csyn, f.csyn);
			__sync_fetch_and_add(&v->crst, f.crst);
			__sync_fetch_and_add(&v->empty_udp, f.empty_udp);
			v->timestamp_last_m = bpf_ktime_get_ns();
		}
		else
		{
			// this is a collision 
			if(v->timestamp_last_m + NANO_GAP < bpf_ktime_get_ns())
			{
				// flow too old since last update - reuse entry
				bpf_debug("Reuse old entry \n");
				v->src = f.src;
				    v->dst = f.dst;
				    v->dstport = f.dstport;
				    v->protocol = f.protocol;
				v->timestamp_start = bpf_ktime_get_ns();
				v->timestamp_last_m = bpf_ktime_get_ns();
				__sync_fetch_and_add(&v->csyn, - v->csyn);
				__sync_fetch_and_add(&v->csyn, f.csyn);
				__sync_fetch_and_add(&v->crst, - v->crst);
				__sync_fetch_and_add(&v->crst, f.crst);
				__sync_fetch_and_add(&v->empty_udp, - v->empty_udp);
				__sync_fetch_and_add(&v->empty_udp, f.empty_udp);
				
			}
			else
			{
				bpf_debug("Flow ignored \n");
			}
		}
	}
	return XDP_PASS;
}
char _license[] SEC("license") = "GPL";

