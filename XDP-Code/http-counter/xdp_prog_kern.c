/* SPDX-License-Identifier: GPL-2.0 */
#include <linux/bpf.h>
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_endian.h>
#include "common_kern_user.h" /* defines: struct datarec; */

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


#define ETH_LEN 14
#define FLAG_BYTE_OFFSET_ETH 47


struct bpf_map_def SEC("maps") xdp_stats_map = {
	.type        = BPF_MAP_TYPE_ARRAY,
	.key_size    = sizeof(__u32),
	.value_size  = sizeof(struct datarec),
	.max_entries = 1,
};

/* LLVM maps __sync_fetch_and_add() as a built-in function to the BPF atomic add
 * instruction (that is BPF_STX | BPF_XADD | BPF_W for word sizes)
 */
#ifndef lock_xadd
#define lock_xadd(ptr, val)	((void) __sync_fetch_and_add(ptr, val))
#endif

SEC("xdp_stats1")
int  xdp_stats1_func(struct xdp_md *ctx)
{
	// action is XDP_PASS 
	void *data_end = (void *)(long)ctx->data_end;
	void *data     = (void *)(long)ctx->data;

	//__u32 len = 0;

	__u32 action = 0;
	/* Lookup in kernel BPF-side return pointer to actual data record */
	struct datarec *rec = bpf_map_lookup_elem(&xdp_stats_map, &action);
	if (!rec)
		return XDP_ABORTED;

	/* Calculate packet length */
	__u64 bytes = data_end - data;

	rec->rx_packets++;
	rec->rx_bytes += bytes;

	
	unsigned int payload_size;
    struct ethhdr *eth = data;
    unsigned char *payload;
    unsigned int payload_offset;
    struct iphdr *ip;
	struct tcphdr *tcp;

	eth = data;

	if ((void *)eth + sizeof(*eth) > data_end)
        return XDP_PASS;

    ip = data + sizeof(*eth);
    if ((void *)ip + sizeof(*ip) > data_end)
        return XDP_PASS;

    if (ip->protocol != IPPROTO_TCP)
        return XDP_PASS;

    tcp = (void *)ip + sizeof(*ip);
    if ((void *)tcp + sizeof(*tcp) > data_end)
        return XDP_PASS;


    payload_size = bpf_ntohs(ip->tot_len) - (tcp->doff*4) - (ip->ihl*4); // IP TOT PACKET - tcp header len - ip header len 
    payload_offset = ETH_LEN + (ip->ihl*4) + (tcp->doff*4); // ETH_LEN + ip header len + tcp header len 
    bpf_printk("payload_size %u - , proto: %u\n", payload_size, bpf_ntohs(eth->h_proto));


    payload = (unsigned char *)tcp + sizeof(*tcp);
    if ((void *)payload + payload_size > data_end)
    	return XDP_PASS;

    unsigned char *byte = (unsigned char *)eth + FLAG_BYTE_OFFSET_ETH;
    if((void* )byte + 1 > data_end)
    	return XDP_PASS;


    // fourth bit starting from the right with LSB 0 index
	if((*byte & (1 << 4)) && (*byte & (1 << 1))) { // check if fourth bit of FLAG_BYTE is set to 1 or not (SYN + ACK FLAG)
		rec->rx_tcp_open = rec->rx_tcp_open + 1;
		bpf_printk("Matching bits SYN+ACK\n");
    }

    rec->rx_tcp_payload = rec->rx_tcp_payload + payload_size;

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
