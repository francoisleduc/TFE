/* SPDX-License-Identifier: GPL-2.0 */
#include <linux/bpf.h>
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_endian.h>
#include "common_kern_user.h" /* defines: struct datarec; */

#include <stdbool.h>
#include <stddef.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <linux/ip.h>
#include <linux/ipv6.h>
#include <linux/icmp.h>
#include <linux/icmpv6.h>
#include <linux/udp.h>
#include <linux/tcp.h>
#include <linux/in.h> // IPPROTO_TCP
#include "../dependencies/headers/jhash.h"

#define ETH_LEN 14
#define FLAG_BYTE_OFFSET_ETH 47


struct bpf_map_def SEC("maps") flow_map = {
	.type        = BPF_MAP_TYPE_ARRAY,
	.key_size    = sizeof(uint32_t),
	.value_size  = sizeof(struct five_tuple),
	.max_entries = 65536, // 2^16 entries
};

/* LLVM maps __sync_fetch_and_add() as a built-in function to the BPF atomic add
 * instruction (that is BPF_STX | BPF_XADD | BPF_W for word sizes)
 */
#ifndef lock_xadd
#define lock_xadd(ptr, val)	((void) __sync_fetch_and_add(ptr, val))
#endif


static __always_inline __u32 hash_five_tuple(struct five_tuple *ft)
{
	//hash packet ip and ports to obtain a key
	return jhash_3words(ft->saddr, ft->daddr, ft->sport, ft->dport);
}

static __always_inline int parse_udp(void *data, __u64 off, void *data_end,
                      struct five_tuple *ft)
{
    struct udphdr *udp;

    udp = data + off;
    if (udp + 1 > data_end)
        return 0;

    ft->sport = bpf_ntohs(udp->source);
    ft->dport = bpf_ntohs(udp->dest);

    return 1;
}


static __always_inline int parse_tcp(void *data, __u64 off, void *data_end,
                      struct five_tuple *ft)
{
    struct tcphdr *tcp;

    tcp = data + off;
    if (tcp + 1 > data_end)
        return 0;

    ft->sport = bpf_ntohs(tcp->source);
    ft->dport = bpf_ntohs(tcp->dest);
	 
    bpf_printk("Source port: %d - Destination port: %d \n\n", ft->sport, ft->dport);
    return 1;
}


static __always_inline int parse_ip4(void *data, __u64 l3off, void *data_end, uint8_t *ip_proto, struct five_tuple *ft)
{
    struct iphdr *iph;

    iph = data + l3off;
    if (iph + 1 > data_end)
        return 0;

    if (iph->ihl != 5)
        return 0;

    ft->saddr = iph->saddr;
    ft->daddr = iph->daddr;
    bpf_printk("Source ip: %d - Destination ip: %d \n\n", ft->saddr, ft->daddr);
	 bpf_printk("Protocol parsed: %d \n\n", iph->protocol);
    *ip_proto = iph->protocol;

    return 1;
}


SEC("xdp_flow")
int  xdp_flow_func(struct xdp_md *ctx)
{
	 // action is XDP_PASS 
	 void *data_end = (void *)(long)ctx->data_end;
	 void *data     = (void *)(long)ctx->data;

	 //__u32 len = 0;
	 bpf_printk("ADVANCED6 \n\n");
    struct five_tuple *value;
    struct ethhdr *eth = data;
    uint8_t ip_proto = 0;
    __u32 off;
    __u32 eth_proto;
    __u32 temp;
    bool jhash = true;
	 __u32 hash;
	 __u32 key;
    struct five_tuple ft;


    off = sizeof(struct ethhdr);
    if(data + off > data_end)
        return XDP_PASS;

    eth_proto = eth->h_proto;
	 
    if (eth_proto == bpf_htons(ETH_P_IP)) 
    {
        if (!parse_ip4(data, off, data_end, &ip_proto, &ft))
            return XDP_PASS;
        off += sizeof(struct iphdr);
    } 
    else 
        return XDP_PASS;
    
    ft.proto = ip_proto;

    if (data + off > data_end)
        return XDP_PASS;

    
	 if(ip_proto == IPPROTO_TCP)
	 {
        if(!parse_tcp(data, off, data_end, &ft))
        {
            bpf_printk("parse_tcp = false? \n\n");
            return XDP_PASS;
        }
        bpf_printk("parse_tcp = true \n\n");
    }
    if (ip_proto == IPPROTO_UDP) 
    {
        if(!parse_udp(data, off, data_end, &ft))
        {
            bpf_printk("parse_udp = false?\n\n");
            return XDP_PASS;
        }
        bpf_printk("parse_udp = true \n\n");
    }
   

   if(jhash)
   {
       hash = hash_five_tuple(&ft);
       //key = hash % 65535;
       key = 1;
		 //bpf_printk("Hash: %d, key: %d \n\n", hash, key);

   }
   temp = ft.saddr;
   value = bpf_map_lookup_elem(&flow_map, &key);
	if (!value)
       return XDP_PASS;

   //lock_xadd(&(value->flow_c), 1);
   //__sync_fetch_and_add(&value->flow_c, 1);
   
   //bpf_printk("Value of flow count: %d  \n", value->flow_c);
   
   if(value->flow_c == 0)
   {
       bpf_printk("New flow because flow packet counter = 0 \n\n");
		 value->flow_c += 1;
       value->saddr += ft.saddr;
		 value->daddr += ft.daddr;
   	 //value->sport += ft.sport;
   	 //value->dport += ft.dport;
   	 //value->proto += ft.proto;
   }
   else
   {
       bpf_printk("Not a new flow, flow packet counter ++ \n\n");
		 bpf_printk("Source addr: %d  \n", value->saddr);
	    value->flow_c += 1;
   }
   
	//value->saddr = ft.saddr;
   //value->daddr = ft.daddr;
   //value->sport = ft.sport;
   //value->dport = ft.dport;
   //value->proto = ft.proto;
   bpf_printk("End pass \n");
   return XDP_PASS;
}

char _license[] SEC("license") = "GPL";
/*

 * user accessible metadata for XDP packet hook
 * new fields must be added to the end of this structure
 *
struct xdp_md {
	// (Note: type __u32 is NOT the real-type)
	__u32 data;
	__u32 data_end;
	__u32 data_meta;
	// Below access go through struct xdp_rxq_info
	__u32 ingress_ifindex; // rxq->dev->ifindex
	__u32 rx_queue_index;  // rxq->queue_index
};
*/
