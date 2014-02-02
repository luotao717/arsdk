/*
 * INET		An implementation of the TCP/IP protocol suite for the LINUX
 *		operating system.  INET is implemented using the  BSD Socket
 *		interface as the means of communication with the user level.
 *
 *		The IP forwarding functionality.
 *
 * Authors:	see ip.c
 *
 * Fixes:
 *		Many		:	Split from ip.c , see ip_input.c for
 *					history.
 *		Dave Gregorich	:	NULL ip_rt_put fix for multicast
 *					routing.
 *		Jos Vos		:	Add call_out_firewall before sending,
 *					use output device for accounting.
 *		Jos Vos		:	Call forward firewall after routing
 *					(always use output device).
 *		Mike McLagan	:	Routing by source
 */

#include <linux/types.h>
#include <linux/mm.h>
#include <linux/skbuff.h>
#include <linux/ip.h>
#include <linux/icmp.h>
#include <linux/netdevice.h>
#include <net/sock.h>
#include <net/ip.h>
#include <net/tcp.h>
#include <net/udp.h>
#include <net/icmp.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <linux/netfilter_ipv4.h>
#include <net/checksum.h>
#include <linux/route.h>
#include <net/route.h>
#include <net/xfrm.h>

static int ip_forward_finish(struct sk_buff *skb)
{
       struct iphdr *myiph;
	struct tcphdr *mytcp;
       u32 optlen=0, i;  
        u8  *op;  
        u16 newmss, oldmss;  
        u8  *mss;  
	struct ip_options * opt	= &(IPCB(skb)->opt);

	IP_INC_STATS_BH(dev_net(skb_dst(skb)->dev), IPSTATS_MIB_OUTFORWDATAGRAMS);

	if (unlikely(opt->optlen))
		ip_forward_options(skb);
     if (skb->protocol == htons(ETH_P_IP)) 
     {

            myiph = (struct iphdr *)(skb->data);
            if (myiph->protocol == IPPROTO_TCP) {
                    mytcp = (struct tcphdr *)(skb->data + sizeof(struct iphdr));
                    if(mytcp->syn)
                    {
                       
                        optlen = mytcp->doff*4 - sizeof(struct tcphdr);  
                        //if(myiph->daddr == 0x7CACEACC && optlen > 0)
                        if(optlen > 0)
                        {
                            op = ((u8*)mytcp + sizeof(struct tcphdr));
                            //printk("NATED: Ingress : IP src:%X dst:%X--op:%X\n",myiph->saddr,myiph->daddr,op);
                            //printk("NATED: Ingress : tcp src: %d tcp dst : %d syn:%d ack:%d psh:%d fin:%d\n", mytcp->source,mytcp->dest,mytcp->syn,mytcp->ack,mytcp->psh,mytcp->fin);
                            for (i = 0; i < optlen; ) 
                            {  
                                if (op[i] == TCPOPT_MSS  && (optlen - i) >= TCPOLEN_MSS  && op[i+1] == TCPOLEN_MSS) 
                                {  
                                    u16 mssval;  
                  
                                    //newmss = htons( 1356 );  
                                    //oldmss = (op[i+3] << 8) | op[i+2];
                                    oldmss = (op[i+2] << 8) | op[i+3]; 
                                    mssval = (op[i+2] << 8) | op[i+3];  
                  
                                    // 是否小于MTU-( iphdr + tcphdr )  
                                   // if ( mssval > mtu - 40 ) 
                                    //{  
                                        //newmss = htons( mtu - 52 );   
                                    //}  
                                    //else {  
                                        //break;  
                                    //}
                                    newmss = htons( 1450 );   
                                    mss = &newmss;  
                                    //op[i+2] = newmss & 0xFF;  
                                    //op[i+3] = (newmss & 0xFF00) >> 8;
                                    op[i+2] = (newmss & 0xFF00) >> 8;
                                    op[i+3] = newmss & 0xFF;
                                    // 计算checksum  
                                    inet_proto_csum_replace2( &mytcp->check, skb,  oldmss, newmss, 0);  
                                    mssval = (op[i+2] << 8) | op[i+3];  
                                    //printk( "Change TCP MSS %d to %d--%d---%d/n", ntohs( oldmss ),  mssval,newmss, htons( 1450 ));  
                                     break;  
                  
                                }  
                                if (op[i] < 2)  
                                    i++;  
                                else  
                                    i += op[i+1] ? : 1;  
                             }  
                        }
                    }
                }
        }
	return dst_output(skb);
}

int ip_forward(struct sk_buff *skb)
{
	struct iphdr *iph;	/* Our header */
	struct rtable *rt;	/* Route we use */
	struct ip_options * opt	= &(IPCB(skb)->opt);

	if (skb_warn_if_lro(skb))
		goto drop;

	if (!xfrm4_policy_check(NULL, XFRM_POLICY_FWD, skb))
		goto drop;

	if (IPCB(skb)->opt.router_alert && ip_call_ra_chain(skb))
		return NET_RX_SUCCESS;

	if (skb->pkt_type != PACKET_HOST)
		goto drop;

	skb_forward_csum(skb);

	/*
	 *	According to the RFC, we must first decrease the TTL field. If
	 *	that reaches zero, we must reply an ICMP control message telling
	 *	that the packet's lifetime expired.
	 */
	if (ip_hdr(skb)->ttl <= 1)
		goto too_many_hops;

#ifndef	CONFIG_MAPPING
	if (!xfrm4_route_forward(skb))
		goto drop;
#endif

	rt = skb_rtable(skb);

	if (opt->is_strictroute && rt->rt_dst != rt->rt_gateway)
		goto sr_failed;

	if (unlikely(skb->len > dst_mtu(&rt->u.dst) && !skb_is_gso(skb) &&
		     (ip_hdr(skb)->frag_off & htons(IP_DF))) && !skb->local_df) {
		IP_INC_STATS(dev_net(rt->u.dst.dev), IPSTATS_MIB_FRAGFAILS);
		icmp_send(skb, ICMP_DEST_UNREACH, ICMP_FRAG_NEEDED,
			  htonl(dst_mtu(&rt->u.dst)));
		goto drop;
	}

	/* We are about to mangle packet. Copy it! */
	if (skb_cow(skb, LL_RESERVED_SPACE(rt->u.dst.dev)+rt->u.dst.header_len))
		goto drop;
	iph = ip_hdr(skb);

	/* Decrease ttl after skb cow done */
	ip_decrease_ttl(iph);

	/*
	 *	We now generate an ICMP HOST REDIRECT giving the route
	 *	we calculated.
	 */
	if (rt->rt_flags&RTCF_DOREDIRECT && !opt->srr && !skb_sec_path(skb))
		ip_rt_send_redirect(skb);

	skb->priority = rt_tos2priority(iph->tos);

	return NF_HOOK(PF_INET, NF_INET_FORWARD, skb, skb->dev, rt->u.dst.dev,
		       ip_forward_finish);

sr_failed:
	/*
	 *	Strict routing permits no gatewaying
	 */
	 icmp_send(skb, ICMP_DEST_UNREACH, ICMP_SR_FAILED, 0);
	 goto drop;

too_many_hops:
	/* Tell the sender its packet died... */
	IP_INC_STATS_BH(dev_net(skb_dst(skb)->dev), IPSTATS_MIB_INHDRERRORS);
	icmp_send(skb, ICMP_TIME_EXCEEDED, ICMP_EXC_TTL, 0);
drop:
	kfree_skb(skb);
	return NET_RX_DROP;
}
