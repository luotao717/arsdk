/*
 *  Copyright (c) 2005 Atheros Communications Inc.  All rights reserved.
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

#ifndef _WBUF_ADF_PRIVATE_LINUX_H
#define _WBUF_ADF_PRIVATE_LINUX_H

#include <osdep_adf.h>

typedef __adf_nbuf_t __wbuf_t;

/*
 * WBUF private API's for Linux
 */

/*
 * NB: This function shall only be called for wbuf
 * with type WBUF_RX or WBUF_RX_INTRENAL.
 */
#define __wbuf_init(_nbf, _pktlen)  do {              \
    __adf_nbuf_put_tail(_nbf, _pktlen);               \
    __adf_nbuf_set_protocol(_nbf, ETH_P_CONTROL);     \
} while (0)

#define __wbuf_header                 __adf_nbuf_data

/*
 * NB: The following two API's only work when nbf's header
 * has not been ajusted, i.e., no one calls skb_push or skb_pull
 * on this skb yet.
 */
#define __wbuf_raw_data               __adf_nbuf_data
#define __wbuf_get_len                __adf_nbuf_tailroom
#define __wbuf_get_datalen_temp       __adf_nbuf_len


#define __wbuf_get_pktlen             __adf_nbuf_len
#define __wbuf_get_tailroom           __adf_nbuf_tailroom
#define __wbuf_get_priority           __adf_nbuf_get_priority
#define __wbuf_set_priority           __adf_nbuf_set_priority

#define __wbuf_hdrspace               __adf_nbuf_headroom

#define __wbuf_next                   __adf_nbuf_queue_next
#define __wbuf_next_buf               __adf_nbuf_queue_next
#define __wbuf_set_next               __adf_nbuf_set_next
#define __wbuf_setnextpkt(_nbf,_next)

#define __wbuf_free                   __adf_nbuf_free
#define __wbuf_push                   __adf_nbuf_push_head
#define __wbuf_clone(_dev, _nbf)      __adf_nbuf_clone(_nbf)
#define __wbuf_trim                   __adf_nbuf_trim_tail
#define __wbuf_pull                   __adf_nbuf_pull_head
#define __wbuf_set_age                __adf_nbuf_set_age
#define __wbuf_get_age                __adf_nbuf_get_age
#define __wbuf_complete               __adf_nbuf_free

#define __wbuf_copydata               __adf_nbuf_copy_bits
#define __wbuf_copy                   __adf_nbuf_copy

#define __wbuf_set_pktlen             __adf_nbuf_set_pktlen
#define __wbuf_set_smpsactframe         N_SMPSACTM_SET
#define __wbuf_is_smpsactframe          N_SMPSACTM_IS
#define __wbuf_set_moredata             N_MOREDATA_SET
#define __wbuf_is_moredata              N_MOREDATA_IS
#define __wbuf_set_smpsframe            N_NULL_PWR_SAV_SET
#define __wbuf_is_smpsframe             N_NULL_PWR_SAV_IS
#define __wbuf_set_qosframe             N_QOS_SET
#define __wbuf_is_qosframe              N_QOS_IS
#define __wbuf_set_pwrsaveframe         N_PWR_SAV_SET
#define __wbuf_is_pwrsaveframe          N_PWR_SAV_IS
#define __wbuf_set_status               N_STATUS_SET
#define __wbuf_get_status               N_STATUS_GET
#define __wbuf_set_type                 N_TYPE_SET
#define __wbuf_get_type                 N_TYPE_GET
#define __wbuf_set_node                 N_NODE_SET
#define __wbuf_get_node                 N_NODE_GET
#define __wbuf_set_exemption_type       N_EXMTYPE_SET
#define __wbuf_get_exemption_type       N_EXMTYPE_GET
#define __wbuf_set_tid                  N_TID_SET
#define __wbuf_get_tid                  N_TID_GET
#define __wbuf_set_eapol                N_EAPOL_SET
#define __wbuf_is_eapol                 N_EAPOL_IS

#ifdef ATH_SUPPORT_WAPI
#define __wbuf_set_wai                  N_WAI_SET
#define __wbuf_is_wai                   N_WAI_IS
#endif

#define __wbuf_set_qosframe             N_QOS_SET
#define __wbuf_get_context              N_CONTEXT_GET
#define __wbuf_set_amsdu                N_AMSDU_SET
#define __wbuf_is_amsdu                 N_AMSDU_IS
#define __wbuf_set_probing              N_PROBING_SET
#define __wbuf_is_probing               N_PROBING_IS
#define __wbuf_clear_probing            N_PROBING_CLR
#define __wbuf_set_fastframe            N_FF_SET
#define __wbuf_is_fastframe             N_FF_IS
#define __wbuf_set_cloned              	N_CLONED_SET
#define __wbuf_is_cloned               	N_CLONED_IS
#define __wbuf_clear_cloned            	N_CLONED_CLR
#ifdef ATH_SUPPORT_UAPSD
#define __wbuf_set_uapsd                N_UAPSD_SET
#define __wbuf_clear_uapsd              N_UAPSD_CLR
#define __wbuf_is_uapsd                 N_UAPSD_IS
#endif

#define __wbuf_is_encap_done            N_ENCAP_DONE_IS
#define __wbuf_set_encap_done           N_ENCAP_DONE_SET
#define __wbuf_clr_encap_done           N_ENCAP_DONE_CLR

#if UMAC_SUPPORT_SMARTANTENNA
#define __wbuf_sa_get_antenna              N_ANT_GET
#define __wbuf_sa_get_rateidx              N_RATE_INDEX_GET
#define __wbuf_is_sa_train_packet          N_TRAIN_PKT_IS
#define __wbuf_is_sa_train_lastpacket      N_TRAIN_LASTPKT_IS 

#define __wbuf_sa_set_antenna              N_ANT_SET
#define __wbuf_sa_set_rateidx              N_RATE_INDEX_SET
#define __wbuf_sa_set_train_packet         N_TRAIN_PKT_SET
#define __wbuf_sa_set_train_lastpacket     N_TRAIN_LASTPKT_SET 
#endif


#define wbuf_set_initimbf(nbf)
#define wbuf_set_pktlen                 __wbuf_set_pktlen
#define wbuf_set_smpsactframe           __wbuf_set_smpsactframe
#define wbuf_is_smpsactframe            __wbuf_is_smpsactframe
#define wbuf_set_smpsframe              __wbuf_set_smpsframe
#define wbuf_set_status                 __wbuf_set_status
#define wbuf_set_type                   __wbuf_set_type
#define wbuf_get_type                   __wbuf_get_type
#define wbuf_set_moredata               __wbuf_set_moredata
#define wbuf_is_moredata                __wbuf_is_moredata
#define wbuf_set_len                    __wbuf_set_pktlen

int __wbuf_map_sg(osdev_t osdev, adf_nbuf_t nbf, dma_context_t context, void *arg);
void __wbuf_unmap_sg(osdev_t osdev, adf_nbuf_t nbf, dma_context_t context);
dma_addr_t __wbuf_map_single(osdev_t osdev, adf_nbuf_t nbf, int direction, dma_context_t context);
void __wbuf_unmap_single(osdev_t osdev, adf_nbuf_t nbf, int direction, dma_context_t context);
void __wbuf_uapsd_update(adf_nbuf_t nbf);

int wbuf_start_dma(adf_nbuf_t nbf, sg_t *sg, u_int32_t n_sg, void *arg);

static INLINE int
__wbuf_append(adf_nbuf_t nbf, u_int16_t size)
{
    if (__adf_nbuf_put_tail(nbf, size) == NULL) {
        KASSERT(0, ("No enough tailroom for wbuf, failed to alloc"));
    }
    return 0;
}

static INLINE __wbuf_t 
__wbuf_coalesce(osdev_t os_handle, adf_nbuf_t nbf)
{
    /* The sk_buff is already contiguous in memory, no need to coalesce. */
    return nbf;
}

static INLINE int
__wbuf_is_initimbf(adf_nbuf_t nbf)
{
    return 0;
}

static INLINE int
wbuf_classify(adf_nbuf_t nbf)
{
    struct ether_header *eh = (struct ether_header *) nbf->data;
    u_int8_t tos = 0;
    /*
     * Find priority from IP TOS DSCP field
     */
    if (eh->ether_type == __constant_htons(ETHERTYPE_IP))
    {
        const struct iphdr *ip = (struct iphdr *)
                    (nbf->data + sizeof (struct ether_header));
        /*
         * IP frame: exclude ECN bits 0-1 and map DSCP bits 2-7
         * from TOS byte.
         */
        tos = (ip->tos & (~INET_ECN_MASK)) >> IP_PRI_SHIFT;
    }
    else if (eh->ether_type == htons(ETHERTYPE_IPV6)) {
        /* TODO
	 * use flowlabel
	 */
        unsigned long ver_pri_flowlabel;
	unsigned long pri;
	/*
            IPv6 Header.
            +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
            |Version| TrafficClass. |                   Flow Label          |
            +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
            |         Payload Length        |  Next Header  |   Hop Limit   |
            +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
            |                                                               |
            +                                                               +
            |                                                               |
            +                         Source Address                        +
            |                                                               |
            +                                                               +
            |                                                               |
            +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
            |                                                               |
            +                                                               +
            |                                                               |
            +                      Destination Address                      +
            |                                                               |
            +                                                               +
            |                                                               |
            +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*/

        ver_pri_flowlabel = *(unsigned long*)(eh + 1);
        pri = (ntohl(ver_pri_flowlabel) & IPV6_PRIORITY_MASK) >> IPV6_PRIORITY_SHIFT;
        tos = (pri & (~INET_ECN_MASK)) >> IP_PRI_SHIFT;
    }
    else if (eh->ether_type == __constant_htons(ETHERTYPE_PAE)) {
        /* mark as EAPOL frame */
         N_EAPOL_SET(nbf);
         tos = OSDEP_EAPOL_TID;  /* send it on VO queue */;
    }
#ifdef ATH_SUPPORT_WAPI
    else if (eh->ether_type == __constant_htons(ETHERTYPE_WAI)) {
        /* mark as WAI frmae */
        N_WAI_SET(nbf);
        tos = OSDEP_EAPOL_TID; /* send it on VO queue */
    }
#endif

    return tos;
}


#if ATH_SUPPORT_VLAN

#define VLAN_PRI_SHIFT  13
#define VLAN_PRI_MASK   7

/*
** Public Prototypes
*/

unsigned short	adf_net_get_vlan(osdev_t os_handle);
int 			adf_net_is_vlan_defined(osdev_t os_handle);

static INLINE int
wbuf_8021p(adf_nbuf_t nbf)
{
    struct vlan_ethhdr *veth = (struct vlan_ethhdr *) nbf->data;
    u_int8_t tos = 0;

	/*
	** Determine if this is an 802.1p frame, and get the proper
	** priority information as required
	*/
	
    if ( veth->h_vlan_proto == __constant_htons(ETH_P_8021Q) )
	{
    	tos = (veth->h_vlan_TCI >> VLAN_PRI_SHIFT) & VLAN_PRI_MASK;
	}

    return tos;
}

static INLINE int
adf_net_get_vlan_tag(adf_nbuf_t nbf)
{
	return ( vlan_tx_tag_get(nbf) );
}

static INLINE int
adf_net_vlan_tag_present(adf_nbuf_t nbf)
{
	return ( vlan_tx_tag_present(nbf) );
}

#endif


static INLINE void
wbuf_concat(adf_nbuf_t head, adf_nbuf_t nbf)
{
    if (__adf_nbuf_is_nonlinear(head))
    {
        KASSERT(0,("wbuf_concat: skb is nonlinear\n"));
    }
    if (__adf_nbuf_tailroom(head) < nbf->len)
    {
        KASSERT(0,("wbuf_concat: Not enough space to concat\n"));
    }
    /* copy the skb data to the head */
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,22))
    memcpy(head->tail, nbf->data, nbf->len);
#else
    memcpy(skb_tail_pointer(head),nbf->data, nbf->len),
#endif
    /* Update tail and length */
    __adf_nbuf_put_tail(head, nbf->len);
    /* free the skb */
    __adf_nbuf_free(nbf);
}

#if defined(ATH_SUPPORT_VOWEXT) || defined(ATH_SUPPORT_IQUE) || defined(VOW_LOGLATENCY) || UMAC_SUPPORT_NAWDS != 0
static INLINE u_int8_t
__wbuf_get_firstxmit(struct sk_buff *skb)
{
    if (!skb) return 0;
    return ((struct ieee80211_cb *)skb->cb)->firstxmit;
}
static INLINE void
__wbuf_set_firstxmit(struct sk_buff *skb, u_int8_t firstxmit)
{
    if (!skb) return;
    ((struct ieee80211_cb *)skb->cb)->firstxmit = firstxmit;
}

static INLINE u_int32_t
__wbuf_get_firstxmitts(struct sk_buff *skb)
{
    if (!skb) return 0;
    return ((struct ieee80211_cb *)skb->cb)->firstxmitts;
}
static INLINE void
__wbuf_set_firstxmitts(struct sk_buff *skb, u_int32_t firstxmitts)
{
    if (!skb) return;
    ((struct ieee80211_cb *)skb->cb)->firstxmitts = firstxmitts;
}

static INLINE void
__wbuf_clear_flags(struct sk_buff *skb)
{
    N_FLAG_CLR(skb, 0xFFFF);
}
#endif /* ATH_SUPPORT_VOWEXT*/

static INLINE void 
__wbuf_set_complete_handler(struct sk_buff *skb,void *handler, void *arg)
{
#if defined(ATH_SUPPORT_P2P)
    struct ieee80211_cb *ctx = (struct ieee80211_cb *)skb->cb;
    ctx->complete_handler = handler;
    ctx->complete_handler_arg = arg;
#endif
}

static INLINE void 
__wbuf_get_complete_handler(struct sk_buff *skb,void **phandler, void **parg)
{
#if defined(ATH_SUPPORT_P2P)
    struct ieee80211_cb *ctx = (struct ieee80211_cb *)skb->cb;
    *phandler = ctx->complete_handler;
    *parg = ctx->complete_handler_arg;
#else
    *phandler = NULL;
    *parg = NULL;
#endif
}

#if defined(VOW_TIDSCHED) || defined (ATH_SUPPORT_IQUE) || defined(VOW_LOGLATENCY)
static INLINE u_int32_t
__wbuf_get_qin_timestamp(struct sk_buff *skb)
{
    if (!skb) return 0;
    return ((struct ieee80211_cb *)skb->cb)->qin_timestamp;
}
static INLINE void
__wbuf_set_qin_timestamp(struct sk_buff *skb, u_int32_t qin_timestamp)
{
    if (!skb) return;
    ((struct ieee80211_cb *)skb->cb)->qin_timestamp = qin_timestamp;
}
#endif

#if UMAC_SUPPORT_RPTPLACEMENT  || ATH_SUPPORT_AOW 
static INLINE void
__wbuf_set_netlink_pid(struct sk_buff *skb, u_int32_t val)
{
    NETLINK_CB(skb).pid = val;
}
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,20)	
static INLINE void
__wbuf_set_netlink_dst_pid(struct sk_buff *skb, u_int32_t val)
{
    NETLINK_CB(skb).dst_pid = val;
}
#endif    
static INLINE void
__wbuf_set_netlink_dst_group(struct sk_buff *skb, u_int32_t val)
{
    NETLINK_CB(skb).dst_group = val;
}
#endif /*UMAC_SUPPORT_RPTPLACEMENT  || ATH_SUPPORT_AOW */

static INLINE u_int8_t *
__wbuf_get_scatteredbuf_header(__wbuf_t wbuf, u_int16_t len)
{
    u_int8_t *pHead = NULL;
    return pHead;
}

#endif
