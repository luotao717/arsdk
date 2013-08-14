/*
 * Copyright (c) 2008, Atheros Communications Inc.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <linux/autoconf.h>
#include <linux/types.h>
#include <linux/in.h>
#include <linux/ip.h>
#include <net/ip.h>
#include <linux/tcp.h>
#include <linux/icmp.h>
#include <linux/skbuff.h>
#include <linux/if_arp.h>
#include <linux/notifier.h>
#include <linux/inetdevice.h>
#include <linux/netdevice.h>
#include <linux/netfilter/nf_conntrack_common.h>
#include <net/netfilter/nf_conntrack.h>


//#include "athrs_hwaccels.h"
#include "athrs_mac.h"
#ifdef CONFIG_ATHRS_S27_HW_NAT
#include "athrs_s27_hw_nat.h"
#include "athrs_s27_nf_nat.h"
#endif

#undef CONFIG_ATHRS_HW_NAT_DEBUG
extern spinlock_t athrs_nat_op_lock;
extern uint8_t athrs_wan_if_mac_addr[ETH_ALEN];
extern uint32_t athrs_wan_if_ip_addr[4];

static int nat_ingress_session_count;
static int nat_egress_session_count;
static unsigned long ath_hw_nat_ingress_pkt_count;
static unsigned long ath_hw_nat_egress_pkt_count;

static unsigned long athr_get_ingress_pkt_count(void)
{
        return ath_hw_nat_ingress_pkt_count;
}

static unsigned long athr_get_egress_pkt_count(void)
{
        return ath_hw_nat_egress_pkt_count;
}


static int athr_nat_session_count_check(int dir)
{

        if (dir == EGRESS) {
                if (nat_egress_session_count < ATHR_HW_NAT_EGRESS_MAX_ENTRIES)
                        return 0;

        }
        else if (dir == INGRESS) {
                if (nat_ingress_session_count < ATHR_HW_NAT_INGRESS_MAX_ENTRIES)
                        return 0;
        }

        return -1;
}

static void athr_nat_session_nos_update(int dir, int op)
{
        spin_lock(&athrs_nat_op_lock);

        if (dir == EGRESS) {
                if (op == CMD_INS && nat_egress_session_count >= 0) {
                             nat_egress_session_count ++;
                }
                else if (op == CMD_DEL && nat_egress_session_count > 0) {
                             nat_egress_session_count --;
                }
        }
        else if (dir == INGRESS) {
                if (nat_ingress_session_count >= 0 &&  op == CMD_INS) {
                        nat_ingress_session_count ++;
                }
                else if (op == CMD_DEL && nat_ingress_session_count > 0) {
                             nat_ingress_session_count --;
                }
        }

        spin_unlock(&athrs_nat_op_lock);
}

static int athrs_wan_ipaddr_to_idx(uint32_t newip)
{
        int i;
        for (i=0; i < 4; i++) {
                if (athrs_wan_if_ip_addr[i] == newip) {
#ifdef CONFIG_ATHRS_HW_NAT_DEBUG
                        printk("%s: found match ip:"NIPQUAD_FMT" in %d\n", __func__, NIPQUAD(newip), i);
#endif
                        return i;
                }
        }

#ifdef CONFIG_ATHRS_HW_NAT_DEBUG
        printk("%s: wan ip:"NIPQUAD_FMT" not found\n", __func__, NIPQUAD(newip));
#endif
        return -1;
}

static int athrs_nat_ops(uint32_t oldip, uint16_t oldport,
			 uint32_t newip, uint16_t newport,
			 u_int8_t protocol, u_int8_t maniptype,
                         int cmd)
{
	ath_egrs_t egrs;
	ath_ingrs_t ingrs;
	int ret;

        if (cmd != CMD_LKP) {
                if (oldip == 0 || ((protocol != ICMP_PROT) && (oldport == 0)) || protocol == 0) {
                        printk("%s: oldip/port/proto invalid ip/port/proto combination\n", __func__);
                        printk("%s: oldip = 0x%x, port = %d, newip = 0x%x, port = %d prot = %d\n",
                               __func__, oldip, oldport, newip, newport, protocol);
                        return -1;
                }

                if (cmd == CMD_INS) {
                    if (newip == 0 || ((protocol != ICMP_PROT) && (newport == 0))) {
                            printk("%s: newip/port invalid ip/port combination\n", __func__);
                            return -1;
                    }
                }
        }


	switch (maniptype) {
	case EGRESS:
		egrs.prot_key = protocol;
		egrs.lan_ip_addr = oldip;
		egrs.lan_port = oldport;
		egrs.info.wan_ip_idx = athrs_wan_ipaddr_to_idx(newip);

                if (egrs.info.wan_ip_idx < 0) {
#ifdef CONFIG_ATHRS_HW_NAT_DEBUG
                        printk("%s: athrs_wan_ipaddr_to_idx returned -1\n", __func__);
#endif
                        return -1;
                }

		egrs.info.wan_port = newport;
                if (cmd == CMD_INS) {
#ifdef CONFIG_ATHRS_HW_NAT_DEBUG
                        printk("Egress : adding lan ip :"NIPQUAD_FMT":%d to "NIPQUAD_FMT":%d proto:%d\n",
                               NIPQUAD(oldip), oldport, NIPQUAD(newip), newport, protocol);
#endif
                }

                if (cmd == CMD_LKP) {
                        //ret = athrs_nat_hwtbl_lkup((void *)&egrs, maniptype, cmd);
                        ret = athrs_nat_hwtbl_lkup((void *)&egrs, maniptype);
                }
                else
                {
                        ret = athrs_nat_hwtbl_ops((void *)&egrs, maniptype, cmd);
                }
		break;

	case INGRESS:
		ingrs.prot_key = protocol;
		ingrs.wan_ip_idx = athrs_wan_ipaddr_to_idx(oldip);

                if (ingrs.wan_ip_idx < 0) {
#ifdef CONFIG_ATHRS_HW_NAT_DEBUG
                        printk("%s: athrs_wan_ipaddr_to_idx returned -1\n", __func__);
#endif
                        return -1;
                }

		ingrs.wan_port = oldport;
		ingrs.info.lan_ip_addr = newip;
		ingrs.info.lan_port = newport;

                if (cmd == CMD_INS) {
#ifdef CONFIG_ATHRS_HW_NAT_DEBUG
                        printk("Ingress : adding wan ip "NIPQUAD_FMT":%d to "NIPQUAD_FMT":%d proto:%d\n",
                        NIPQUAD(oldip), oldport, NIPQUAD(newip), newport, protocol);
#endif
                }

                /* Not really needed */
		memcpy(ingrs.info.lan_mac_addr, athrs_wan_if_mac_addr, ETH_ALEN);

                if (cmd == CMD_LKP) {
                        //ret = athrs_nat_hwtbl_lkup((void *)&ingrs, maniptype, cmd);
                        ret = athrs_nat_hwtbl_lkup((void *)&ingrs, maniptype);
                }
                else
                {
                        ret = athrs_nat_hwtbl_ops((void *)&ingrs, maniptype, cmd);
                }

		break;

	default:
		printk("%s: invalid maniptype\n", __func__);
		return -1;
	}
	return ret;
}

int athr_hw_nat_add_entry(uint32_t oldip, uint16_t oldport,
			    uint32_t newip, uint16_t newport,
			    u_int8_t protocol, int maniptype)
{
	int ret;

        if (athr_nat_session_count_check(maniptype) != 0) {
                printk("%s: %s nat h/w table full \n", __func__,
                       (maniptype == EGRESS ? "Egress" : "Ingress"));
        }

	ret = athrs_nat_ops(oldip, oldport, newip,
			    newport, protocol, maniptype, CMD_INS);

        if (ret == 0) {
                athr_nat_session_nos_update(maniptype, CMD_INS);
        }
#ifdef CONFIG_ATHRS_HW_NAT_DEBUG
	printk("%s: athrs hw nat add ret value : %d\n", __func__, ret);
#endif
        return ret;

}

void athr_hw_nat_del_entry(uint32_t oldip, uint16_t oldport,uint32_t newip,uint16_t newport,
			    u_int8_t protocol, int maniptype)
{
	int ret;
#ifdef CONFIG_ATHRS_HW_NAT_DEBUG
        printk("%s: lan ip :"NIPQUAD_FMT" port:%d  proto:%d maniptype=%d newip=%x newport =%x  \n",
                       __func__, NIPQUAD(oldip), oldport, protocol, maniptype,newip,newport);
#endif

	ret = athrs_nat_ops(oldip, oldport, newip,
			    newport,protocol, maniptype, CMD_DEL);

        if (ret == 0) {
                athr_nat_session_nos_update(maniptype, CMD_DEL);
        }
#ifdef CONFIG_ATHRS_HW_NAT_DEBUG
	printk("%s: athrs hw nat del ret value success : %d\n", __func__, ret);
#endif

}

int athr_hw_nat_lookup_entry(uint32_t ipaddr, uint16_t port, u_int8_t maniptype,
				     u_int8_t protocol, uint32_t *newipaddr, uint16_t *newport)
{
	ath_egrs_t egrs;
	ath_ingrs_t ingrs;
	int ret;
        /*
         * Try the lookup on egress lut
         */
	switch (maniptype) {
	case EGRESS:
		egrs.prot_key = protocol;
		egrs.lan_ip_addr = ipaddr;
		egrs.lan_port = port;
                //ret = athrs_nat_hwtbl_lkup((void *)&egrs, maniptype, CMD_LKP);
                ret = athrs_nat_hwtbl_lkup((void *)&egrs, maniptype);
		*newipaddr = egrs.lan_ip_addr;
		*newport = egrs.lan_port;
		break;

	case INGRESS:
		ingrs.prot_key = protocol;
		ingrs.wan_ip_idx = athrs_wan_ipaddr_to_idx(ipaddr);
                if (ingrs.wan_ip_idx < 0) {
#ifdef CONFIG_ATHRS_HW_NAT_DEBUG
                        printk("%s: athrs_wan_ipaddr_to_idx returned -1\n", __func__);
#endif
                        return -1;
                }
		ingrs.wan_port = port;
                //ret = athrs_nat_hwtbl_lkup((void *)&ingrs, maniptype, CMD_LKP);
                ret = athrs_nat_hwtbl_lkup((void *)&ingrs, maniptype);
		*newipaddr = ingrs.info.lan_ip_addr;
		*newport = ingrs.info.lan_port;
		break;

	default:
#ifdef CONFIG_ATHRS_HW_NAT_DEBUG
		printk("%s: invalid maniptype\n", __func__);
#endif
		return -1;
	}

//	printk("%s: athrs hw nat lookup ret value : %d\n", __func__, ret);
	return ret;
}

static int check_hw_entry(const struct nf_conntrack_tuple *tuple) {

        uint32_t ipaddr;
        uint16_t port;
        int ret = -1;

        ret = athr_hw_nat_lookup_entry(tuple->src.u3.ip, tuple->src.u.all,
                                        tuple->dst.dir, tuple->dst.protonum,
                                        &ipaddr, &port);
        return ret;
}

int athr_hwnat_print_tuple(struct seq_file *s, struct nf_conn *ct, uint8_t pkt_cnt)
{
        const struct nf_conntrack_tuple *tuple;
        int ret = -1;
        struct nf_ct_hwnat *nf_hwnat;

        nf_hwnat = ct->hwnat_priv;
        tuple = &nf_hwnat->tuple[0];
        //tuple = &ct->tuple[0];
        ret = seq_printf(s, "\n[EGRESS]  src=%pI4 sport=%hu dst=%pI4 dport=%hu proto=%hhu hwflag = %d",
                          &tuple->src.u3.ip, ntohs(tuple->src.u.all), &tuple->dst.u3.ip, \
                          ntohs(tuple->dst.u.all), tuple->dst.protonum, check_hw_entry(tuple));
        if (ret)
            return 1;

        //tuple = &ct->tuple[1];
        tuple = &nf_hwnat->tuple[1];
        ret = seq_printf(s, "\n[INGRESS]  src=%pI4 sport=%hu dst=%pI4 dport=%hu proto=%hhu hwflag = %d",
                          &tuple->src.u3.ip, ntohs(tuple->src.u.all), &tuple->dst.u3.ip, \
                          ntohs(tuple->dst.u.all), tuple->dst.protonum, check_hw_entry(tuple));
        if (ret)
            return 1;

        if (pkt_cnt) {
                ret = seq_printf(s, "\nHW Nat'd Tx Pkt cnt = %lu, Rx Pkt cnt = %lu ",
                                 athr_get_egress_pkt_count(), athr_get_ingress_pkt_count());
                if (ret)
                        return 1;
        }

        return 0;
}


void athr_nat_process_egress_pkts(int unit, struct sk_buff *skb,
                                  athr_gmac_desc_t *ds)
{
        struct iphdr *iph;
#ifdef CONFIG_ATHRS_HW_NAT_DEBUG
	struct tcphdr *tcp;
#endif
        struct nf_conn *ct;

        if ((unit == 0) && (skb->protocol == ETH_P_IP)) {
                iph = ip_hdr(skb);
                if (iph != NULL) {
                        /*
                         * Enable NAT for tcp/udp/icmp packets
                         * if we have a entry in the h/w nat lut
                         */
                        if ((iph->protocol == IPPROTO_TCP ||
                             iph->protocol == IPPROTO_UDP  ||
                             iph->protocol == IPPROTO_ICMP))  {
                                    //tcp = (struct tcphdr *)((void *)iph + iph->ihl*4);
                                    if (skb->ath_hw_nat_fw_flags == 2) {
#ifdef CONFIG_ATHRS_HW_NAT_DEBUG
                                            printk("NATED: Egress : IP src: %X dst: %X skb->ath_hw_nat_fw_flags: %d\n",
                                                   iph->saddr, iph->daddr, skb->ath_hw_nat_fw_flags);

                                            if (iph->protocol == IPPROTO_TCP) {
                                                    tcp = (struct tcphdr *)((void *)iph + iph->ihl*4);
                                                    printk("NATED: Egress : tcp src: %d tcp dst : %d syn:%d ack:%d fin:%d skb->ath_hw_nat_fw_flags: %d\n",
                                                           tcp->source, tcp->dest, tcp->syn, tcp->ack, tcp->fin, skb->ath_hw_nat_fw_flags);
                                            }
#endif
                                            ath_hw_nat_egress_pkt_count ++;
                                            ATHR_TX_HW_NAT_ENABLE(ds);
#ifdef CONFIG_ATHRS_HW_NAT_DEBUG
                                             if (iph->protocol == IPPROTO_UDP &&
                                                 (iph->frag_off & htons(IP_MF|IP_OFFSET)))
                                                 printk("%s: src addr : 0x%x, dst addr : 0x%x\n", __func__,
                                                        iph->saddr, iph->daddr);
#endif
                                    }
                                    else {
#ifdef CONFIG_ATHRS_HW_NAT_DEBUG
                                            printk("UNNATED: Egress : IP src: %X dst: %X skb->ath_hw_nat_fw_flags: %d\n",
                                                   iph->saddr,iph->daddr, skb->ath_hw_nat_fw_flags);

                                            if (iph->protocol == IPPROTO_TCP) {
                                                    //tcp = tcp_hdr(skb);
                                                    tcp = (struct tcphdr *)((void *)iph + iph->ihl*4);
                                                    printk("UNNATED: Egress : tcp src: %d tcp dst: %d syn:%d ack:%d psh:%d fin:%d\n",
                                                           tcp->source,tcp->dest,tcp->syn,tcp->ack,tcp->psh, tcp->fin);
                                            }
#endif
                                            ct = (struct nf_conn *)skb->nfct;

                                            if(ct) {
                                                    if (test_bit(IPS_ATHR_HW_NAT_ADDED_BIT, &ct->status)) {
                                                            if(iph->frag_off) {
                                                                    ath_hw_nat_egress_pkt_count ++;
                                                                    ATHR_TX_HW_NAT_ENABLE(ds);
                                                            } else {
                                                                    printk("%s: nf nat disable check failed %d %x\n", __func__,
                                                                           skb->ath_hw_nat_fw_flags, iph->frag_off);
                                                            }
                                                    }
                                            }

                                    }
                        }
                        else
                                printk("%s: non ip protocol\n", __func__);
                }
                else
                    printk("%s: null ip header\n",__func__);
         }
}

void athr_nat_process_ingress_pkts(int unit, struct sk_buff *skb,
                                   athr_gmac_desc_t *ds)
{
        struct iphdr *iph;
#ifdef CONFIG_ATHRS_HW_NAT_DEBUG
	struct tcphdr *tcp;
#endif

        if (unit == 0) {

                skb->ath_hw_nat_fw_flags = 0;

                if (skb->protocol == htons(ETH_P_IP)) {

                        /* based on skb_reset_network_header function */

                        iph = (struct iphdr *)(skb->data);

                        if (ATHR_RX_HW_NAT_STATUS(ds)) {
                                /*
                                 * if we have done the nat'ing in the hardware,
                                 * mark the skb, so that it won't go through the
                                 * netfilter nat
                                 */
#ifdef CONFIG_ATHRS_HW_NAT_DEBUG
                                printk("NATED: Ingress : IP src:%X dst:%X\n",iph->saddr,iph->daddr);

                                if (iph->protocol == IPPROTO_TCP) {
                                        tcp = (struct tcphdr *)(skb->data + sizeof(struct iphdr));
                                        printk("NATED: Ingress : tcp src: %d tcp dst : %d syn:%d ack:%d psh:%d fin:%d\n",
                                                tcp->source,tcp->dest,tcp->syn,tcp->ack,tcp->psh,tcp->fin);
                                }

#endif
                                skb->ath_hw_nat_fw_flags = 3;
				ath_hw_nat_ingress_pkt_count ++;
                        }
                        else
                        {
                                /* Unnated packet */
                                skb->ath_hw_nat_fw_flags = 100;
#ifdef CONFIG_ATHRS_HW_NAT_DEBUG
                                printk("UNNATED: Ingress : IP src:%X dst:%X\n",iph->saddr,iph->daddr);
                                if (iph->protocol == IPPROTO_TCP) {
                                        tcp = (struct tcphdr *)(skb->data + sizeof(struct iphdr));
                                        printk("UNNATED: Ingress : tcp src: %d tcp dst : %d syn:%d ack:%d psh:%d fin:%d\n",
                                                tcp->source,tcp->dest,tcp->syn,tcp->ack,tcp->psh,tcp->fin);
                                }
#endif
                        }
                }
        }
        else
        {
                /* set it to 1 to indicate that its a lan packet */
                skb->ath_hw_nat_fw_flags = 1;
        }
}

