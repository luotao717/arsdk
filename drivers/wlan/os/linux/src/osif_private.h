/*
 * Copyright (c) 2010, Atheros Communications Inc.
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


#ifndef _OSIF_PRIVATE_H_ 
#define _OSIF_PRIVATE_H_ 

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,38)
#ifndef AUTOCONF_INCLUDED
#include <linux/config.h>
#endif
#endif
#include <linux/version.h>
#include <linux/module.h>
#include <linux/kmod.h>
#include <linux/init.h>
#include <linux/skbuff.h>
#include <linux/sysctl.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/if_vlan.h>

#include <net/iw_handler.h>
#include <linux/wireless.h>
#include <linux/if_arp.h>		/* XXX for ARPHRD_* */
#if ATH_SUPPORT_FLOWMAC_MODULE
#include <net/pkt_sched.h>
#endif
#include <asm/uaccess.h>

#include "if_media.h"
#include "if_upperproto.h"

#include "ieee80211.h"
#include "_ieee80211.h"
#include <ieee80211_api.h>
#include "ieee80211_defines.h"
#include <ieee80211_ioctl.h>
#include <ieee80211_sme_api.h>
#include <ieee80211P2P_api.h>

#ifdef ATH_SUPPORT_LINUX_VENDOR
#include <ioctl_vendor.h>      /* Vendor Include */
#endif

#define OS_SIWSCAN_TIMEOUT ((12000 * HZ) / 1000)
#define OSIF_MAX_CONNECTION_ATTEMPT      3
#define OSIF_MAX_CONNECTION_STOP_TIMEOUT 20
#define OSIF_MAX_CONNECTION_TIMEOUT      0xFFFFFFFF
#define OSIF_MAX_DELETE_VAP_TIMEOUT      30

#define OSIF_CONNECTION_TIMEOUT          CONVERT_SEC_TO_SYSTEM_TIME(1)
#define OSIF_DELETE_VAP_TIMEOUT          CONVERT_SEC_TO_SYSTEM_TIME(1)
#define OSIF_DISCONNECT_TIMEOUT       ((CONVERT_SEC_TO_SYSTEM_TIME(1)/100) + 1) /* 10 msec */
/* make sure the above constant does not return 0 */

int osif_ioctl_create_vap(struct net_device *dev, struct ifreq *ifr, osdev_t os_handle);
int osif_ioctl_delete_vap(struct net_device *dev);
int osif_ioctl_switch_vap(struct net_device *dev, enum ieee80211_opmode opmode);
void osif_attach(struct net_device *dev);
void osif_detach(struct net_device *dev);
void osif_notify_push_button(struct net_device *dev, u_int32_t push_time);
int osif_vap_init(struct net_device *dev, int forcescan);
int osif_vap_stop(struct net_device *dev);
void osif_deliver_data(os_if_t osif, struct sk_buff *skb);
void osif_forward_mgmt_to_app(os_if_t osif, wbuf_t wbuf,
                                        u_int16_t type, u_int16_t subtype);
#if ATH_SUPPORT_SPECTRAL
int osif_ioctl_eacs(struct net_device *dev, struct ifreq *ifr, osdev_t os_handle);
#endif
#if ATH_SUPPORT_VLAN
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,30)
void			adf_net_vlan_attach(struct net_device *dev,struct net_device_ops *osif_dev_ops);
#else
void			adf_net_vlan_attach(struct net_device *dev);
#endif
void			adf_net_vlan_detach(struct net_device *dev);
#endif

#if ATH_RXBUF_RECYCLE
void ath_rxbuf_recycle_init(osdev_t osdev);
void ath_rxbuf_recycle_destroy(osdev_t osdev);
#endif /* ATH_RXBUF_RECYCLE */

/* TGf l2_update_frame  format */
struct l2_update_frame
{
    struct ether_header eh;
    u_int8_t dsap;
    u_int8_t ssap;
    u_int8_t control;
    u_int8_t xid[3];
}  __packed;

#define IEEE80211_L2UPDATE_CONTROL 0xf5
#define IEEE80211_L2UPDATE_XID_0 0x81
#define IEEE80211_L2UPDATE_XID_1 0x80
#define IEEE80211_L2UPDATE_XID_2 0x0


typedef struct _osif_dev {
    osdev_t  os_handle;
    struct net_device *netdev;
    struct list_head pending_rx_frames; /* stuff too big for wext events, send by ioctl */
    spinlock_t list_lock;
    wlan_if_t os_if;
    wlan_dev_t os_devhandle;
    struct net_device *os_comdev;
    struct ifmedia os_media;    /* interface media config */
    u_int8_t  os_unit;
#if ATHEROS_LINUX_P2P_DRIVER
    wlan_p2p_t                  p2p_handle; /* always need a device for scan */
    wlan_p2p_go_t               p2p_go_handle;      /* ptr if a GO */
    wlan_p2p_client_t           p2p_client_handle;  /* ptr if a STA client */
#endif
#if ATH_SUPPORT_VLAN
	struct vlan_group	*vlgrp;
	unsigned short		vlanID;
#endif
    wlan_connection_sm_t sm_handle; 
    wlan_ibss_sm_t       sm_ibss_handle; 
    enum ieee80211_opmode os_opmode;
    struct net_device_stats os_devstats;
    struct iw_statistics    os_iwstats; /* wireless statistics block */

    /* cache for the security config */
    ieee80211_cipher_type uciphers[IEEE80211_CIPHER_MAX];
    u_int16_t u_count;
    ieee80211_cipher_type mciphers[IEEE80211_CIPHER_MAX];
    u_int16_t m_count;
    IEEE80211_SCAN_REQUESTOR    scan_requestor;
    IEEE80211_SCAN_ID           scan_id;
    u_int32_t       user_scan_wait:1,
                    is_up:1,
                    is_deleted:1,
                    is_ibss_create:1,
                    is_delete_in_progress:1,
                    is_restart:1,
                    is_vap_pending:1,
                    use_p2p_go:1,
                    use_p2p_client:1,
                    is_p2p_interface:1, /* use_p2p_go | use_p2p_client */
                    is_bss_started:1,
                    disable_ibss_create:1;
    u_int8_t        os_giwscan_count;
    unsigned long   os_last_siwscan;    /* time last set scan request */
#define OSIF_SCAN_BAND_ALL            (0)
#define OSIF_SCAN_BAND_2G_ONLY        (1)
#define OSIF_SCAN_BAND_5G_ONLY        (2)
    u_int8_t        os_scan_band;       /* only scan channels of requested band */
        
    u_int8_t authmode;
    u_int32_t app_filter;
#ifdef ATHEROS_LINUX_PERIODIC_SCAN
#define OSIF_PERIODICSCAN_DEF_PERIOD        (0)
#define OSIF_PERIODICSCAN_MIN_PERIOD        (30000)  

    u_int32_t   os_periodic_scan_period;    /* 0 means off Periodic Scan */
    os_timer_t  os_periodic_scan_timer;
#endif
    spinlock_t  tx_lock; /* lock for the tx path */
#ifdef ATH_SUPPORT_LINUX_VENDOR
#define OSIF_VENDOR_RAWDATA_SIZE        (24)
    u_int8_t os_vendor_specific[OSIF_VENDOR_RAWDATA_SIZE];    /* Some vendor ioctl() maybe cache something. */
#endif
#if ATH_SUPPORT_IWSPY
    struct 	iw_spy_data 		  spy_data;	/* iwspy support */
#endif
#if ATH_SUPPORT_WAPI 
    u_int32_t    os_wapi_rekey_period;
    os_timer_t    os_wapi_rekey_timer;
#endif
} osif_dev;

#define OSIF_WAPI_REKEY_TIMEOUT 5000  /*ms*/

#ifndef ETH_P_80211_RAW
#define ETH_P_80211_RAW (ETH_P_ECONET + 1)
#endif

enum
{
        DIDmsg_lnxind_wlansniffrm               = 0x00000044,
        DIDmsg_lnxind_wlansniffrm_hosttime      = 0x00010044,
        DIDmsg_lnxind_wlansniffrm_mactime       = 0x00020044,
        DIDmsg_lnxind_wlansniffrm_channel       = 0x00030044,
        DIDmsg_lnxind_wlansniffrm_rssi          = 0x00040044,
        DIDmsg_lnxind_wlansniffrm_sq            = 0x00050044,
        DIDmsg_lnxind_wlansniffrm_signal        = 0x00060044,
        DIDmsg_lnxind_wlansniffrm_noise         = 0x00070044,
        DIDmsg_lnxind_wlansniffrm_rate          = 0x00080044,
        DIDmsg_lnxind_wlansniffrm_istx          = 0x00090044,
        DIDmsg_lnxind_wlansniffrm_frmlen        = 0x000A0044
};

enum
{
    P80211ENUM_msgitem_status_no_value  = 0x00
};
enum
{
    P80211ENUM_truth_false              = 0x00
};

/*
 * Some data is too big to return in a wireless extension event.
 * So instead, the event signals the availability of data an IOCTL is used
 * to fetch data - which calls the fetch_p2p_mgmt list access routine.
 */
struct pending_rx_frames_list {
    struct list_head list;
    struct wlan_p2p_rx_frame rx_frame; /* data to be returned via ioctl */
    u_int32_t  freq;        /* pre-pend frequency to data on fx_frames */
    u_int32_t  frame_type;  /* pre-pend type to data */
    int    extra[1];        /* variable length extra data */
};
struct pending_rx_frames_list *osif_fetch_p2p_mgmt(struct net_device *dev);
void osif_p2p_rx_frame_handler(osif_dev *osifp, wlan_p2p_event *event,
                                int frame_type);

#ifdef LIMIT_MTU_SIZE
#define LIMITED_MTU 1368
#include <net/icmp.h>
#include <linux/icmp.h>
#include <net/route.h>


#define LIMIT_PATH_MTU_TX(_skb,_Fake_rtable)                            \
        if (_skb->len >=  LIMITED_MTU + ETH_HLEN) {                     \
            _skb->h.raw = _skb->nh.raw = _skb->data +ETH_HLEN ;         \
            _skb->dst = (struct dst_entry *)&_Fake_rtable;              \
            _skb->pkt_type = PACKET_HOST;                               \
            dst_hold(_skb->dst);                                        \
            icmp_send(_skb, ICMP_DEST_UNREACH,                          \
                     ICMP_FRAG_NEEDED, htonl(LIMITED_MTU - 4 ));        \
            dev_kfree_skb_any(_skb);                                    \
            return 0;                                                   \
        }                                                         

#define LIMIT_PATH_MTU_RX(_skb,_Fake_rtable)                            \
        if (_skb->len >=  LIMITED_MTU) {                                \
            _skb->h.raw = _skb->nh.raw = _skb->data  ;                  \
            _skb->dst = (struct dst_entry *)&_Fake_rtable;              \
            _skb->pkt_type = PACKET_HOST;                               \
            dst_hold(_skb->dst);                                        \
            icmp_send(_skb, ICMP_DEST_UNREACH,                          \
                     ICMP_FRAG_NEEDED, htonl(LIMITED_MTU - 4 ));        \
            dev_kfree_skb_any(_skb);                                    \
            return;                                                     \
        }                                                         
#endif



#endif
