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


#include <ieee80211_aponly.h>
#include "osif_private.h"
#include <wlan_opts.h>
#include <ieee80211_var.h>
#include <ieee80211_extap.h>
#include "htc_thread.h"
#include "if_athvar.h"



#if ATH_SUPPORT_IBSS_NETLINK_NOTIFICATION
#include "ath_netlink.h"
#endif

#define OSIF_TO_NETDEV(_osif) (((osif_dev *)(_osif))->netdev)

#define IEEE80211_MSG_IOCTL   IEEE80211_MSG_DEBUG

#ifdef OSIF_DEBUG
#define IEEE80211_ADD_MEDIA(_media, _s, _o) \
    do { \
        printk("adding media word 0x%x \n",IFM_MAKEWORD(IFM_IEEE80211, (_s), (_o), 0)); \
        ifmedia_add(_media, IFM_MAKEWORD(IFM_IEEE80211, (_s), (_o), 0), 0, NULL); \
    } while(0);
#else
#define IEEE80211_ADD_MEDIA(_media, _s, _o) \
        ifmedia_add(_media, IFM_MAKEWORD(IFM_IEEE80211, (_s), (_o), 0), 0, NULL);
#endif
#ifdef LIMIT_MTU_SIZE



static struct net_device __fake_net_device = {
    .hard_header_len    = ETH_HLEN
};

static struct rtable __fake_rtable = {
    .u = {
        .dst = {
            .__refcnt       = ATOMIC_INIT(1),
            .dev            = &__fake_net_device,
            .path           = &__fake_rtable.u.dst,
            .metrics        = {[RTAX_MTU - 1] = 1500},
        }
    },
    .rt_flags   = 0,
};
#else
#define __fake_rtable 0
#endif


extern
int osif_vap_hardstart_aponly(struct sk_buff *skb, struct net_device *dev);

static void osif_check_pending_ap_vaps(wlan_dev_t comhandle, wlan_if_t vap);

extern void ieee80211_ioctl_vattach(struct net_device *dev);

#ifdef __CARRIER_PLATFORM__
extern void wlan_pltfrm_attach(struct net_device *dev);
extern void wlan_pltfrm_detach(struct net_device *dev);
extern void osif_pltfrm_receive (os_if_t osif, wbuf_t wbuf,
                        u_int16_t type, u_int16_t subtype,
                        ieee80211_recv_status *rs);
extern void osif_pltfrm_create_vap(osif_dev *osifp);
extern void osif_pltfrm_delete_vap(osif_dev *osifp);
extern void osif_pltfrm_record_macinfor(unsigned char unit, unsigned char* mac);
#endif

#if ATH_SUPPORT_BYPASS_BRIDGE
extern struct net_device *eth0_dev;
extern struct net_device *ath0_dev;
#endif

#if UMAC_SUPPORT_IBSS
static int
osif_ibss_init(struct net_device *dev);
#endif

#ifdef USE_HEADERLEN_RESV
/*
* The kernel version of this function alters the skb in a manner
* inconsistent with dev->hard_header_len header reservation. This
* is a rewrite of the portion of eth_type_trans() that we need.
*/
static unsigned short
ath_eth_type_trans(struct sk_buff *skb, struct net_device *dev)
{
    struct ethhdr *eth;

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,22)
    skb->mac.raw = skb->data;
#else
    skb_reset_mac_header(skb);
#endif
    skb_pull(skb, ETH_HLEN);
    /*
    * NB: mac.ethernet is replaced in 2.6.9 by eth_hdr but
    *     since that's an inline and not a define there's
    *     no easy way to do this cleanly.
    */
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,22))
    eth= (struct ethhdr *)skb->mac.raw;
#else
    eth= (struct ethhdr *)skb_mac_header(skb);
#endif

    if(*eth->h_dest&1)
    {
        if(memcmp(eth->h_dest,dev->broadcast, ETH_ALEN)==0)
            skb->pkt_type=PACKET_BROADCAST;
        else
            skb->pkt_type=PACKET_MULTICAST;
    }

    /*
    *  This ALLMULTI check should be redundant by 1.4
    *  so don't forget to remove it.
    *
    *  Seems, you forgot to remove it. All silly devices
    *  seems to set IFF_PROMISC.
    */
    else if(1 /*dev->flags&IFF_PROMISC*/)
    {
        if(memcmp(eth->h_dest,dev->dev_addr, ETH_ALEN))
            skb->pkt_type=PACKET_OTHERHOST;
    }

    return eth->h_proto;
}
#endif

#ifdef ATHEROS_LINUX_PERIODIC_SCAN
static void osif_periodic_scan_start(os_if_t osif)
{
    osif_dev  *osdev = (osif_dev *) osif;
    wlan_if_t vap;
    
    vap = osdev->os_if;
    
    if (osdev->os_periodic_scan_period){
        IEEE80211_DPRINTF(vap, IEEE80211_MSG_SCAN, "%s: Periodic Scan Timer Start. %d msec \n", 
                            __func__, osdev->os_periodic_scan_period);
        OS_SET_TIMER(&osdev->os_periodic_scan_timer, osdev->os_periodic_scan_period);
    }
    
    return;
}

static void osif_periodic_scan_stop(os_if_t osif)
{
    osif_dev  *osdev = (osif_dev *) osif;
    wlan_if_t vap;
    
    vap = osdev->os_if;
    
    IEEE80211_DPRINTF(vap, IEEE80211_MSG_SCAN, "%s: Periodic Scan Timer Stop. \n", __func__);
    OS_CANCEL_TIMER(&osdev->os_periodic_scan_timer);
    
    return;
}

static OS_TIMER_FUNC(periodic_scan_timer_handler)
{
    osif_dev *osifp;
    wlan_if_t vap;
    struct net_device *dev;
    ieee80211_scan_params scan_params;

    OS_GET_TIMER_ARG(osifp, osif_dev *);
    vap = osifp->os_if;
    dev = OSIF_TO_NETDEV(osifp);
        
    if (!(dev->flags & IFF_UP)) {
        return;
    }

    IEEE80211_DPRINTF(vap, IEEE80211_MSG_SCAN, "%s: Periodic Scan Timer Go. \n", __func__);
    
    if (wlan_vap_get_opmode(vap) == IEEE80211_M_STA){
        if (osifp->sm_handle && 
            wlan_connection_sm_is_connected(osifp->sm_handle) &&
            !wlan_scan_in_progress(vap)){
        
            OS_MEMZERO(&scan_params,sizeof(ieee80211_scan_params));
            
            /* Fill scan parameter */
            wlan_set_default_scan_parameters(vap,&scan_params,IEEE80211_M_STA,true,false,true,true,0,NULL,1);
            
            scan_params.max_dwell_time_active = 2 * scan_params.min_dwell_time_active;
            scan_params.idle_time = 610;
            scan_params.repeat_probe_time = 0;
            scan_params.min_beacon_count = 0;
            scan_params.max_scan_time = OSIF_PERIODICSCAN_MIN_PERIOD;
            scan_params.type = IEEE80211_SCAN_BACKGROUND;
            scan_params.flags |= (IEEE80211_SCAN_ALLBANDS | IEEE80211_SCAN_ACTIVE | IEEE80211_SCAN_ADD_BCAST_PROBE);

            if (osifp->os_scan_band != OSIF_SCAN_BAND_ALL) {
                scan_params.flags &= ~IEEE80211_SCAN_ALLBANDS;
                if (osifp->os_scan_band == OSIF_SCAN_BAND_2G_ONLY)
                    scan_params.flags |= IEEE80211_SCAN_2GHZ;
                else if (osifp->os_scan_band == OSIF_SCAN_BAND_5G_ONLY)
                    scan_params.flags |= IEEE80211_SCAN_5GHZ;
                else {
                    IEEE80211_DPRINTF(vap, IEEE80211_MSG_SCAN,
                                      "%s: unknow scan band, scan all bands.\n", __func__);
                    scan_params.flags |= IEEE80211_SCAN_ALLBANDS;
                }
            }
            
            if (wlan_scan_start(vap, &scan_params, osifp->scan_requestor, IEEE80211_SCAN_PRIORITY_LOW, &(osifp->scan_id)) != 0 ) {
                IEEE80211_DPRINTF(vap, IEEE80211_MSG_SCAN,
                    "%s: Issue a scan fail.\n",
                    __func__);
            }
        }

        /* restart the timer */
        osif_periodic_scan_start((os_if_t)osifp); 
    }

    return;
}
#endif

#if ATH_SUPPORT_WAPI
static void
osif_wapi_rekeytimer_start (os_if_t osif)
{
    osif_dev  *osdev = (osif_dev *) osif;
//  IEEE80211_DPRINTF(vap, IEEE80211_MSG_DEBUG, "%s: WAPI Rekeying Timer Start. \n", __func__);	
    OS_SET_TIMER(&osdev->os_wapi_rekey_timer, osdev->os_wapi_rekey_period);

}

static void
osif_wapi_rekeytimer_stop (os_if_t osif)
{
    osif_dev  *osdev = (osif_dev *) osif;
//  IEEE80211_DPRINTF(vap, IEEE80211_MSG_DEBUG, "%s: WAPI Rekeying Timer Stop. \n", __func__);
    OS_CANCEL_TIMER(&osdev->os_wapi_rekey_timer);

}
static OS_TIMER_FUNC(osif_wapi_rekey_timeout )
{
    osif_dev *osifp;
    wlan_if_t vap;
    struct net_device *dev;

    OS_GET_TIMER_ARG(osifp, osif_dev *);
    vap = osifp->os_if;
    dev = OSIF_TO_NETDEV(osifp);
        
    if (!(dev->flags & IFF_UP)) {
        return;
    }

    if(ieee80211_vap_active_is_set(vap) && 
		ieee80211_vap_wapi_is_set(vap))
    {
        if(vap->iv_wapi_urekey_pkts)
        {
            wlan_iterate_station_list(vap, 
				(ieee80211_sta_iter_func)wlan_wapi_unicast_rekey, (void*)vap);
        }
        if(vap->iv_wapi_mrekey_pkts)
        {
            wlan_wapi_multicast_rekey(vap,vap->iv_bss);
         }
    }
	
    osif_wapi_rekeytimer_start((os_if_t)osifp); 
    return;	
}

#endif


void
osif_deliver_data(os_if_t osif, struct sk_buff *skb)
{
    struct net_device *dev = OSIF_TO_NETDEV(osif);
#if ATH_SUPPORT_VLAN
    osif_dev  *osifp = (osif_dev *) osif;
#endif
#if ATH_RXBUF_RECYCLE
    struct net_device *comdev;
    struct ath_softc_net80211 *scn;
    struct ath_softc *sc;
#endif /* ATH_RXBUF_RECYCLE */

    skb->dev = dev;

#ifdef USE_HEADERLEN_RESV
    skb->protocol = ath_eth_type_trans(skb, dev);
#else
    skb->protocol = eth_type_trans(skb, dev);
#endif
#if ATH_RXBUF_RECYCLE
    comdev = ((osif_dev *)osif)->os_comdev;
    scn = ath_netdev_priv(comdev);
    sc = ATH_DEV_TO_SC(scn->sc_dev);
	/*
	 * Do not recycle the received mcast frame becasue it will be cloned twice
	 */
    if (sc->sc_osdev->rbr_ops.osdev_wbuf_collect && !(wbuf_is_cloned(skb))) {    
        sc->sc_osdev->rbr_ops.osdev_wbuf_collect((void *)sc, (void *)skb);    
    }
#endif /* ATH_RXBUF_RECYCLE */
#if ATH_SUPPORT_VLAN
    if ( osifp->vlanID != 0 && osifp->vlgrp != NULL)
    {
        /* attach vlan tag */
        vlan_hwaccel_rx(skb, osifp->vlgrp, osifp->vlanID);
    }
    else
#endif
#ifdef ATH_SUPPORT_HTC
    if (in_interrupt())
        netif_rx(skb);
    else
        netif_rx_ni(skb);
#else
#if ATH_SUPPORT_BYPASS_BRIDGE
	skb->dev = eth0_dev;
	skb_push(skb, ETH_HLEN);
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,31)
   	eth0_dev->hard_start_xmit(skb, eth0_dev);	
#else
    eth0_dev->netdev_ops->ndo_start_xmit(skb, eth0_dev);           
#endif
#else    
    netif_rx(skb);
#endif
#endif
    dev->last_rx = jiffies;
}

static void
osif_deliver_l2uf(os_handle_t osif, u_int8_t *macaddr)
{
    struct net_device *dev = OSIF_TO_NETDEV(osif);
    osif_dev *osdev = ath_netdev_priv(dev);
    wlan_if_t vap;
    struct sk_buff *skb;
    struct l2_update_frame *l2uf;
    struct ether_header *eh;
    
    vap = osdev->os_if;

    /* add 2 more bytes to meet minimum packet size allowed with LLC headers */
    skb = dev_alloc_skb(sizeof(*l2uf) + 2);
    if (!skb)
    {
        IEEE80211_DPRINTF(vap, IEEE80211_MSG_INPUT, "%s\n",
                            "ieee80211_deliver_l2uf: no buf available");
        return;
    }
    skb_put(skb, sizeof(*l2uf) + 2);
    l2uf = (struct l2_update_frame *)(skb->data);
    eh = &l2uf->eh;
    /* dst: Broadcast address */
    IEEE80211_ADDR_COPY(eh->ether_dhost, dev->broadcast);
    /* src: associated STA */
    IEEE80211_ADDR_COPY(eh->ether_shost, macaddr);
    eh->ether_type = htons(skb->len - sizeof(*eh));

    l2uf->dsap = 0;
    l2uf->ssap = 0;
    l2uf->control = IEEE80211_L2UPDATE_CONTROL;
    l2uf->xid[0] = IEEE80211_L2UPDATE_XID_0;
    l2uf->xid[1] = IEEE80211_L2UPDATE_XID_1;
    l2uf->xid[2] = IEEE80211_L2UPDATE_XID_2;
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,22)
    skb->mac.raw = skb->data;
#else
    skb_reset_mac_header(skb);
#endif

    __osif_deliver_data(osif, skb);
}

static void osif_get_active_vap_iter_func(void *arg, wlan_if_t vap)
{
    osif_dev *osifp;
    u_int16_t *active_vaps = (u_int16_t *) arg;
    struct net_device *dev;

    osifp = (osif_dev *)wlan_vap_get_registered_handle(vap);
    dev = osifp->netdev;
    if (dev->flags & IFF_UP) {
        ++(*active_vaps);
    }
}

static void osif_get_running_vap_iter_func(void *arg, wlan_if_t vap)
{
    osif_dev *osifp;
    u_int16_t *running_vaps = (u_int16_t *) arg;

    osifp = (osif_dev *)wlan_vap_get_registered_handle(vap);
    if (osifp->is_up) {
        ++(*running_vaps);
    }
}

static u_int32_t osif_get_num_active_vaps( wlan_dev_t  comhandle)
{
    u_int16_t num_active_vaps=0;
    wlan_iterate_vap_list(comhandle,osif_get_active_vap_iter_func,(void *)&num_active_vaps);
    return num_active_vaps;
}

static u_int16_t osif_get_num_running_vaps( wlan_dev_t  comhandle)
{
    u_int16_t num_running_vaps=0;
    wlan_iterate_vap_list(comhandle,osif_get_running_vap_iter_func,(void *)&num_running_vaps);
    return num_running_vaps;
}

static void
osif_notify_scan_done(struct net_device *dev)
{
    union iwreq_data wreq;
    osif_dev *osdev = ath_netdev_priv(dev);
    wlan_if_t vap;

    vap = osdev->os_if;
    IEEE80211_DPRINTF(vap, IEEE80211_MSG_SCAN, "%s\n", "notify scan done");

    /* dispatch wireless event indicating scan completed */
    wreq.data.length = 0;
    wreq.data.flags = 0;
    wireless_send_event(dev, SIOCGIWSCAN, &wreq, NULL);
}

/*
 * scan handler used by the ioctl.
 */
static void osif_scan_evhandler(wlan_if_t vap, ieee80211_scan_event *event, void *arg)
{
    osif_dev  *osifp = (osif_dev *) arg;
    //IEEE80211_DPRINTF(vap, IEEE80211_MSG_SCAN, "%s scan_id %08X event %d reason %d \n",
    //                  __func__, event->scan_id, event->type, event->reason);
#if ATH_SUPPORT_MULTIPLE_SCANS
    /*
     * Ignore notifications received due to scans requested by other modules
     * and handle new event IEEE80211_SCAN_DEQUEUED.
     */
    ASSERT(0);

    if (osifp->scan_id != event->scan_id) {
        return;
    }
#endif    /* ATH_SUPPORT_MULTIPLE_SCANS */

    if ((event->type == IEEE80211_SCAN_COMPLETED) &&
        (event->reason != IEEE80211_REASON_CANCELLED) &&
        (event->requestor == osifp->scan_requestor)) {

        IEEE80211_DPRINTF(vap, IEEE80211_MSG_IOCTL, "%s SCAN DONE  reason %d \n",
                    __func__, event->reason);
        osif_notify_scan_done(osifp->netdev);
        /* The AP vaps might not have come up due to this scan ... */
        if (wlan_get_param(vap, IEEE80211_FEATURE_WDS) &&
            !wlan_get_param(vap, IEEE80211_FEATURE_VAP_IND) &&
            wlan_is_connected(vap) /* sta-vap is ready */) {
            osif_check_pending_ap_vaps(wlan_vap_get_devhandle(vap), vap);
        }
    }
}

static int osif_acs_start_bss(wlan_if_t vap, wlan_chan_t channel)
{
    int error = 0;

    if ((error = wlan_mlme_start_bss(vap))!=0) {
        IEEE80211_DPRINTF(vap, IEEE80211_MSG_ACS,
                        "%s : failed start bss with error code %d\n",
                        __func__, error);
    } else {
        wlan_mlme_connection_up(vap);
        IEEE80211_DPRINTF(vap, IEEE80211_MSG_ACS,
                        "%s :vap up \n", __func__);
    }

    wlan_determine_cw(vap, channel);
    return error;
}

static void osif_ht40_event_handler(void *arg, wlan_chan_t channel)
{
    int error;
    osif_dev *osifp = (osif_dev *) arg;
    wlan_if_t vap = osifp->os_if;

    if ((error = osif_acs_start_bss(vap, channel))!=0) {
        osifp->is_vap_pending = 1;
        spin_lock(&osifp->tx_lock);
        osifp->is_up = 0;
        spin_unlock(&osifp->tx_lock);
        IEEE80211_DPRINTF(vap, IEEE80211_MSG_IOCTL, "%s :mlme busy mostly scanning \n", __func__);
    } else {
        osifp->is_up = 1;
    }

    wlan_autoselect_unregister_event_handler(vap,
                                            &osif_ht40_event_handler,
                                            osifp);
}

/*
* Auto Channel Select handler used for interface up.
*/
static void osif_acs_event_handler(void *arg, wlan_chan_t channel)
{
    osif_dev *osifp = (osif_dev *) arg;
    wlan_if_t vap = osifp->os_if;
    int chan = wlan_channel_ieee(channel);
    int error = 0;

    error = wlan_set_channel(vap, chan);
    if (error !=0) {
        IEEE80211_DPRINTF(vap, IEEE80211_MSG_ACS,
                        "%s : failed to set channel with error code %d\n",
                        __func__, error);
        goto done;
    }

    osif_acs_start_bss(vap, channel);

    osifp->is_up = 1;

done:
    wlan_autoselect_unregister_event_handler(vap, &osif_acs_event_handler, (void *)osifp);
}

static void osif_vap_down(struct net_device *dev)
{
    osif_dev  *osdev = ath_netdev_priv(dev);
    wlan_if_t vap = osdev->os_if;
    int i;

    spin_lock(&osdev->tx_lock);
    osdev->is_up=0;
    spin_unlock(&osdev->tx_lock);

    if (osdev->os_opmode == IEEE80211_M_HOSTAP ||
        osdev->os_opmode == IEEE80211_M_IBSS   ||
        (u_int8_t)osdev->os_opmode ==  IEEE80211_M_P2P_GO) {
        IEEE80211_DPRINTF(vap, IEEE80211_MSG_IOCTL, "%s : sending MLME Event \n",__func__);

        if( osdev->is_delete_in_progress )
            wlan_mlme_stop_bss(vap,WLAN_MLME_STOP_BSS_F_FORCE_STOP_RESET);
        else
            wlan_mlme_stop_bss(vap,0);
    }

    OS_DELAY(1000);
    if (osdev->is_delete_in_progress) {
        OS_DELAY(5000);
        /*
        * delete any default keys, if the vap is to be deleted.
        */
        for (i=0;i<IEEE80211_WEP_NKID; ++i) {
            u_int8_t macaddr[] = {0xff,0xff,0xff,0xff,0xff,0xff };
            wlan_del_key(vap,i,macaddr);
        }

        IEEE80211_DPRINTF(vap, IEEE80211_MSG_IOCTL, "%s : Keys Deleted \n",__func__);
    }
#if ATH_SUPPORT_WAPI
    if (osdev->os_opmode == IEEE80211_M_HOSTAP)
        osif_wapi_rekeytimer_stop((os_if_t)osdev); 
#endif
}

static void
osif_assoc_indication_ap(os_handle_t osif, u_int8_t *macaddr, u_int16_t result, wbuf_t wbuf, wbuf_t resp_buf)
{
    struct net_device *dev = ((osif_dev *)osif)->netdev;
    osif_dev *osifp = (osif_dev *)osif;
    union iwreq_data wreq;
#if ATH_SUPPORT_WAPI
    wlan_if_t vap = ((osif_dev *)osif)->os_if;
    union iwreq_data wapi_wreq;
    u_int8_t *sta_msg;
    int msg_len;
#endif
    if ((u_int8_t)osifp->os_opmode != IEEE80211_M_P2P_GO) {
        OS_MEMSET(&wreq, 0, sizeof(wreq));
        IEEE80211_ADDR_COPY(wreq.addr.sa_data, macaddr);
        wreq.addr.sa_family = ARPHRD_ETHER;
        wireless_send_event(dev, IWEVREGISTERED, &wreq, NULL);
#if ATH_SUPPORT_WAPI
        sta_msg = wlan_wapi_callback_begin(vap, macaddr, &msg_len, WAPI_WAI_REQUEST);
        if (sta_msg) {
            OS_MEMSET(&wapi_wreq, 0, sizeof(wapi_wreq));
            wapi_wreq.data.length = msg_len;
            wireless_send_event(dev, IWEVCUSTOM, &wapi_wreq, sta_msg);
            wlan_wapi_callback_end(sta_msg);
        }
#endif
    }
    else { /* For P2P supplicant needs entire stuff */
        char *concat_buf;
        int wbuf_len =  wbuf_get_pktlen(wbuf);

        concat_buf = OS_MALLOC(osifp->os_handle, wbuf_len + ETHER_ADDR_LEN,
                                GFP_ATOMIC);
        if (!concat_buf) {
            return;
        }
        OS_MEMSET(&wreq, 0, sizeof(wreq));
        wreq.data.length = wbuf_len + ETHER_ADDR_LEN;
        OS_MEMCPY(concat_buf, macaddr, ETHER_ADDR_LEN);
        OS_MEMCPY(concat_buf+ETHER_ADDR_LEN, wbuf_header(wbuf), wbuf_len);

        wireless_send_event(dev, IWEVGENIE, &wreq, concat_buf);
        OS_FREE(concat_buf);
    }

    /* Send TGf L2UF frame on behalf of newly associated station */
    osif_deliver_l2uf(osif, macaddr);
}

static void osif_deauth_indication_ap(os_handle_t osif, u_int8_t *macaddr,u_int16_t reason)
{
    struct net_device *dev = ((osif_dev *)osif)->netdev;
    union iwreq_data wreq;
#if ATH_SUPPORT_WAPI
    wlan_if_t vap = ((osif_dev *)osif)->os_if;
    union iwreq_data wapi_wreq;
    u_int8_t *sta_msg;
    int msg_len;
#endif

    /* fire off wireless event station leaving */
    memset(&wreq, 0, sizeof(wreq));
    IEEE80211_ADDR_COPY(wreq.addr.sa_data, macaddr);
    wreq.addr.sa_family = ARPHRD_ETHER;
    wireless_send_event(dev, IWEVEXPIRED, &wreq, NULL);
#if ATH_SUPPORT_WAPI
    sta_msg = wlan_wapi_callback_begin(vap, macaddr, &msg_len, WAPI_STA_AGING);
    if (sta_msg) {
        OS_MEMSET(&wapi_wreq, 0, sizeof(wapi_wreq));
        wapi_wreq.data.length = msg_len;
        printk("%s size:%d *\n", __func__, wreq.data.length);
        wireless_send_event(dev, IWEVCUSTOM, &wapi_wreq, sta_msg);
        wlan_wapi_callback_end(sta_msg);
    }
#endif
}

static void osif_leave_indication_ap(os_handle_t osif, u_int8_t *macaddr,u_int32_t reason)
{
    struct net_device *dev = ((osif_dev *)osif)->netdev;
    union iwreq_data wreq;
#if ATH_SUPPORT_WAPI
    wlan_if_t vap = ((osif_dev *)osif)->os_if;
    union iwreq_data wapi_wreq;
    u_int8_t *sta_msg;
    int msg_len;
#endif

    /* fire off wireless event station leaving */
    memset(&wreq, 0, sizeof(wreq));
    IEEE80211_ADDR_COPY(wreq.addr.sa_data, macaddr);
    wreq.addr.sa_family = ARPHRD_ETHER;
    wireless_send_event(dev, IWEVEXPIRED, &wreq, NULL);
#if ATH_SUPPORT_WAPI
    sta_msg = wlan_wapi_callback_begin(vap, macaddr, &msg_len, WAPI_STA_AGING);
    if (sta_msg) {
        OS_MEMSET(&wapi_wreq, 0, sizeof(wapi_wreq));
        wapi_wreq.data.length = msg_len;
        printk("%s size:%d *\n", __func__, wreq.data.length);
        wireless_send_event(dev, IWEVCUSTOM, &wapi_wreq, sta_msg);
        wlan_wapi_callback_end(sta_msg);
    }
#endif
}

static void  osif_disassoc_indication_ap(os_handle_t osif, u_int8_t *macaddr, u_int32_t reason, IEEE80211_STATUS status)
{
    struct net_device *dev = ((osif_dev *)osif)->netdev;
    union iwreq_data wreq;

    /* fire off wireless event station leaving */
    memset(&wreq, 0, sizeof(wreq));
    IEEE80211_ADDR_COPY(wreq.addr.sa_data, macaddr);
    wreq.addr.sa_family = ARPHRD_ETHER;
    wireless_send_event(dev, IWEVEXPIRED, &wreq, NULL);
}

#if ATH_SUPPORT_WAPI
static void osif_rekey_indication_ap(os_handle_t osif, u_int8_t *macaddr)
{
    struct net_device *dev = ((osif_dev *)osif)->netdev;
    wlan_if_t vap = ((osif_dev *)osif)->os_if;
    union iwreq_data wapi_wreq;
    u_int8_t *sta_msg;
    int msg_len;

    if (IEEE80211_IS_MULTICAST(macaddr))
        sta_msg = wlan_wapi_callback_begin(vap, macaddr, &msg_len, WAPI_MULTI_REKEY);
    else
        sta_msg = wlan_wapi_callback_begin(vap, macaddr, &msg_len, WAPI_UNICAST_REKEY);
    if (sta_msg) {
        OS_MEMSET(&wapi_wreq, 0, sizeof(wapi_wreq));
        wapi_wreq.data.length = msg_len;
        wireless_send_event(dev, IWEVCUSTOM, &wapi_wreq, sta_msg);
        wlan_wapi_callback_end(sta_msg);
    }
}
#endif

/*
 * handler called when the AP vap comes up asynchronously.
 * (happens only when resource  manager is present).
 */ 
static void  osif_create_infra_complete( os_handle_t osif, IEEE80211_STATUS status)
{
    osif_dev *osifp = (osif_dev *)osif;
    wlan_if_t vap = ((osif_dev *)osif)->os_if;

    wlan_mlme_connection_up(vap);
    IEEE80211_DPRINTF(vap, IEEE80211_MSG_IOCTL, "%s :vap up \n", __func__);
    osifp->is_up = 1;
}


static void osif_bringup_vap_iter_func(void *arg, wlan_if_t vap)
{
    osif_dev  *osifp;
    int error;

    if (wlan_vap_get_opmode(vap) != IEEE80211_M_HOSTAP) {
        return;
    }
    osifp  = (osif_dev *)wlan_vap_get_registered_handle(vap);
    if (osifp->is_vap_pending && !osifp->is_up) {
        if ((error = wlan_mlme_start_bss(vap))!=0) {
            IEEE80211_DPRINTF(vap, IEEE80211_MSG_STATE,
                        "Error %s : failed start bss with error code %d\n",
                        __func__, error);
        } else {
            wlan_mlme_connection_up(vap);
            IEEE80211_DPRINTF(vap, IEEE80211_MSG_STATE,
                        "%s :vap up \n", __func__);
            osifp->is_up = 1;
        }
    }
}

static void osif_bringdown_vap_iter_func(void *arg, wlan_if_t vap)
{
    osif_dev  *osifp;

    osifp  = (osif_dev *)wlan_vap_get_registered_handle(vap);
    if(wlan_vap_get_opmode(vap) != IEEE80211_M_HOSTAP) {
        return;
    }

    wlan_mlme_stop_bss(vap,0);
    wlan_mlme_connection_down(vap);
    IEEE80211_DPRINTF(vap, IEEE80211_MSG_STATE,
            "%s :vap down \n", __func__);
    spin_lock(&osifp->tx_lock);
    osifp->is_up = 0;
    spin_unlock(&osifp->tx_lock);

}



static void osif_check_pending_ap_vaps(wlan_dev_t comhandle, wlan_if_t vap)
{
    wlan_iterate_vap_list(comhandle, osif_bringup_vap_iter_func, NULL);
}

static void osif_sta_sm_evhandler(wlan_connection_sm_t smhandle, os_if_t osif,
                                  wlan_connection_sm_event *smevent)
{
    osif_dev  *osdev = (osif_dev *) osif;
    struct net_device *dev = osdev->netdev;
    wlan_if_t vap = osdev->os_if;
    wlan_dev_t comhandle = wlan_vap_get_devhandle(vap);
    u_int8_t bssid[IEEE80211_ADDR_LEN];
    union iwreq_data wreq;

    memset(&wreq, 0, sizeof(wreq));

    IEEE80211_DPRINTF(vap, IEEE80211_MSG_IOCTL, "%s : event %d reason %d \n",
                      __func__, smevent->event,smevent->disconnect_reason);
    if (smevent->event == WLAN_CONNECTION_SM_EVENT_CONNECTION_UP) {
        wlan_mlme_connection_up(vap);
        wlan_vap_get_bssid(vap,bssid);
        wlan_node_authorize(vap,1,bssid);
        IEEE80211_DPRINTF(vap, IEEE80211_MSG_IOCTL, "%s :vap up ,connection up"
                          " to bssid=%s, ifname=%s\n",
                          __func__, ether_sprintf(bssid), dev->name);
        netif_carrier_on(dev);
        osdev->is_up = 1;
        memset(&wreq, 0, sizeof(wreq));
        IEEE80211_ADDR_COPY(wreq.addr.sa_data, bssid);
        wreq.addr.sa_family = ARPHRD_ETHER;
        wireless_send_event(dev, SIOCGIWAP, &wreq, NULL);
	if (wlan_get_param(vap, IEEE80211_FEATURE_WDS) && !wlan_get_param(vap, IEEE80211_FEATURE_VAP_IND)) {
            /* Note: This should be moved to resource manager later */
            osif_check_pending_ap_vaps(comhandle,vap);
        }
#ifdef ATHEROS_LINUX_PERIODIC_SCAN
        else {
            osif_periodic_scan_start(osif);
        }
#endif
    } else if (smevent->event == WLAN_CONNECTION_SM_EVENT_CONNECTION_DOWN) {
        IEEE80211_DPRINTF(vap, IEEE80211_MSG_IOCTL, "%s :sta disconnected \n", __func__);

        if (((smevent->disconnect_reason == WLAN_CONNECTION_SM_RECONNECT_TIMEDOUT) ||
             (smevent->disconnect_reason == WLAN_CONNECTION_SM_CONNECT_TIMEDOUT)) &&
            (vap->auto_assoc)) {
            /* Connection timed out - retry connection */
            if (osdev->authmode == IEEE80211_AUTH_AUTO) {
                /* If the auth mode is set to AUTO
                 * switch between SHARED and AUTO mode
                 */
                ieee80211_auth_mode modes[2];
                u_int nmodes;
                nmodes=wlan_get_auth_modes(vap,modes,IEEE80211_AUTH_MAX);
                nmodes=1;
                if (modes[0] == IEEE80211_AUTH_SHARED) {
                    modes[0] = IEEE80211_AUTH_OPEN;
                }
                else {
                    modes[0] = IEEE80211_AUTH_SHARED;
                }
                wlan_set_authmodes(vap,modes,nmodes);
            }
            if (wlan_get_param(vap, IEEE80211_FEATURE_WDS)  && !wlan_get_param(vap, IEEE80211_FEATURE_VAP_IND)) {
                /* Note: This should be moved to resource manager later */
                wlan_iterate_vap_list(comhandle, osif_bringdown_vap_iter_func, NULL);
            }
	    if(vap->iv_ic->ic_roaming != IEEE80211_ROAMING_MANUAL)
                wlan_connection_sm_start(osdev->sm_handle);
        }
        else {
            if (wlan_get_param(vap, IEEE80211_FEATURE_WDS)  && !wlan_get_param(vap, IEEE80211_FEATURE_VAP_IND)) {
                /* Note: This should be moved to resource manager later */
                wlan_iterate_vap_list(comhandle, osif_bringdown_vap_iter_func, NULL);
            }
#ifdef ATHEROS_LINUX_PERIODIC_SCAN
            else {
                osif_periodic_scan_stop(osif);
            }
#endif     
            if (!osdev->is_restart) { /* bring down the vap only if it is not a restart */
                if (osdev->is_up) {
                    memset(wreq.ap_addr.sa_data, 0, ETHER_ADDR_LEN);
                    wreq.ap_addr.sa_family = ARPHRD_ETHER;
                    wireless_send_event(dev, SIOCGIWAP, &wreq, NULL);
                }
                netif_carrier_off(dev);
                osif_vap_down(dev);
            }
            else {
                /* clear the restart and is_up flag */
                spin_lock(&osdev->tx_lock);
                osdev->is_up = 0;
                spin_unlock(&osdev->tx_lock);
                osdev->is_restart = 0;
            }
        }
    } else if (smevent->event == WLAN_CONNECTION_SM_EVENT_CONNECTION_LOST) {
	/* Repeater dependancy implemented here  */
        if (wlan_get_param(vap, IEEE80211_FEATURE_WDS) && !wlan_get_param(vap, IEEE80211_FEATURE_VAP_IND)) {    
            /* Note: This should be moved to resource manager later */
            wlan_iterate_vap_list(comhandle, osif_bringdown_vap_iter_func, NULL);
        }
#ifdef ATHEROS_LINUX_PERIODIC_SCAN
        else {
            osif_periodic_scan_stop(osif);
        }
#endif       
        if (osdev->is_up) {
            memset(wreq.ap_addr.sa_data, 0, ETHER_ADDR_LEN);
            wreq.ap_addr.sa_family = ARPHRD_ETHER;
            wireless_send_event(dev, SIOCGIWAP, &wreq, NULL);
        }
        spin_lock(&osdev->tx_lock);
        osdev->is_up = 0;
        spin_unlock(&osdev->tx_lock);
        IEEE80211_DPRINTF(vap, IEEE80211_MSG_IOCTL, "%s :connection lost \n", __func__);
    } else if (smevent->event == WLAN_CONNECTION_SM_EVENT_CONNECTION_RESET) {
        netif_carrier_on(dev);
        IEEE80211_DPRINTF(vap, IEEE80211_MSG_IOCTL, "%s :vap reset \n", __func__);
        wlan_mlme_connection_up(vap);
        osdev->is_up = 1;
        wlan_vap_get_bssid(vap,bssid);
	if (wlan_get_param(vap, IEEE80211_FEATURE_WDS) && !wlan_get_param(vap, IEEE80211_FEATURE_VAP_IND)) {
            /* Note: This should be moved to resource manager later */
            wlan_iterate_vap_list(comhandle, osif_bringdown_vap_iter_func, NULL);
            osif_check_pending_ap_vaps(comhandle,vap);
        }
        memset(&wreq, 0, sizeof(wreq));
        IEEE80211_ADDR_COPY(wreq.addr.sa_data, bssid);
        wreq.addr.sa_family = ARPHRD_ETHER;
        wireless_send_event(dev, SIOCGIWAP, &wreq, NULL);
        wlan_node_authorize(vap,1,bssid);
    } else if ((smevent->event == WLAN_CONNECTION_SM_EVENT_REJOINING) &&
               (wlan_vap_get_opmode(vap) == IEEE80211_M_STA) &&
               !vap->auto_assoc) {
        ieee80211_auth_mode modes[IEEE80211_AUTH_MAX];
        int count, i;
        int iswpa = 0;

        count = wlan_get_auth_modes(vap,modes,IEEE80211_AUTH_MAX);
        for (i = 0; i < count; i++) {
            if (modes[i] == IEEE80211_AUTH_WPA || modes[i] == IEEE80211_AUTH_RSNA) {
                iswpa = 1;
                break;
            }
        }
        if (iswpa) {
            if (osdev->is_up) {
                memset(wreq.ap_addr.sa_data, 0, ETHER_ADDR_LEN);
                wreq.ap_addr.sa_family = ARPHRD_ETHER;
                wireless_send_event(dev, SIOCGIWAP, &wreq, NULL);
            }
        }
    }
}

#if UMAC_SUPPORT_IBSS
static void osif_ibss_scan_evhandler(wlan_if_t vaphandle, ieee80211_scan_event *event, void *arg)
{
    osif_dev  *osifp = (osif_dev *) arg;
    wlan_if_t vap = osifp->os_if;

    if ((event->type == IEEE80211_SCAN_COMPLETED) &&
        (event->reason != IEEE80211_REASON_CANCELLED) &&
        (event->requestor == osifp->scan_requestor)) {
        int error = 0;
        int chan = vap->iv_des_ibss_chan;
        struct ath_softc_net80211 *scn;
        struct net_device *comdev = ((osif_dev *)osifp)->os_comdev;

        IEEE80211_DPRINTF(vaphandle, IEEE80211_MSG_IOCTL, "%s SCAN DONE  reason %d \n",
                    __func__, event->reason);

        scn = ath_netdev_priv(comdev);

        /* Work around for WMM parameter update. Since we will issue scan to work
           around problems in creating IBSS, and WMM parameters will be clear. */
        ieee80211_wme_updateparams(vap);

        /* Work around for IBSS ACS functions. In IBSS ACS, the beacon timer will
           not be updated, thus, use set channel to make the timer update. */
        scn->sc_syncbeacon = 0;
        error = wlan_set_channel(vap, chan);

        /* Show message for debug if set channel fail */
        if (error != 0) {
            IEEE80211_DPRINTF(vap, IEEE80211_MSG_IOCTL,
                            "%s : failed to set channel with error code %d\n",
                            __func__, error);
        }

        wlan_scan_unregister_event_handler(vap, 
                                           &osif_ibss_scan_evhandler, 
                                           (void *) osifp);

        wlan_scan_register_event_handler(vap, &osif_scan_evhandler, (void*)osifp);
        osif_notify_scan_done(osifp->netdev);
    }
}

static void osif_ibss_scan(osif_dev *osdev)
{
    wlan_if_t vap = osdev->os_if;
    ieee80211_scan_params scan_params;
    enum ieee80211_opmode opmode = wlan_vap_get_opmode(vap);

    OS_MEMZERO(&scan_params,sizeof(ieee80211_scan_params));

    /* Fill scan parameter */
    wlan_set_default_scan_parameters(vap,&scan_params,opmode,true,false,true,
              true,0,NULL,1);

    scan_params.max_dwell_time_active = 2 * scan_params.min_dwell_time_active;
    scan_params.idle_time = 610;
    scan_params.repeat_probe_time = 0;
    scan_params.min_beacon_count = 0;
    scan_params.max_scan_time = OSIF_PERIODICSCAN_MIN_PERIOD;
    scan_params.type = IEEE80211_SCAN_FOREGROUND;
    scan_params.flags |= (IEEE80211_SCAN_ALLBANDS | IEEE80211_SCAN_PASSIVE);

    wlan_scan_unregister_event_handler(vap, 
                                       &osif_scan_evhandler, 
                                       (void *) osdev);

    wlan_scan_register_event_handler(vap, &osif_ibss_scan_evhandler, (void*)osdev);

    if (wlan_scan_start(vap, &scan_params, osdev->scan_requestor,
            IEEE80211_SCAN_PRIORITY_LOW, &(osdev->scan_id)) != 0) {
        IEEE80211_DPRINTF(vap, IEEE80211_MSG_IOCTL, "%s: Issue a scan fail.\n",
                             __func__);
    }
}

static void osif_ibss_sm_evhandler(wlan_ibss_sm_t smhandle, os_if_t osif, 
                                   wlan_ibss_sm_event *smevent)   
{    
    osif_dev  *osdev = (osif_dev *) osif; 
    struct net_device *dev = osdev->netdev;
    wlan_if_t vap = osdev->os_if;
    u_int8_t bssid[IEEE80211_ADDR_LEN];
#if !ATH_SUPPORT_IBSS_NETLINK_NOTIFICATION
    union iwreq_data wreq;
#endif
    
    IEEE80211_DPRINTF(vap, IEEE80211_MSG_IOCTL, "%s : event %d reason %d \n",
                      __func__, smevent->event, smevent->reason);
    if (smevent->event == WLAN_IBSS_SM_EVENT_SUCCESS) {
        wlan_mlme_connection_up(vap);
        wlan_vap_get_bssid(vap, bssid);
        wlan_node_authorize(vap, 1, bssid);
        IEEE80211_DPRINTF(vap, IEEE80211_MSG_IOCTL, "%s :vap up \n", __func__);
        netif_carrier_on(dev);
        osdev->is_up = 1;

       // join success: update WMM
        if (!osdev->is_ibss_create) {          
            ieee80211_wme_updateparams(vap);
        }       

        osdev->is_ibss_create = 0;
        
#if !ATH_SUPPORT_IBSS_NETLINK_NOTIFICATION
        memset(&wreq, 0, sizeof(wreq));
        IEEE80211_ADDR_COPY(wreq.addr.sa_data, bssid);
        wreq.addr.sa_family = ARPHRD_ETHER;
        wireless_send_event(dev, SIOCGIWAP, &wreq, NULL);        
#endif
    } else if (smevent->event == WLAN_IBSS_SM_EVENT_FAILED) {
        /*
         * If join fail, switch ibss to create.
         */
        if (!osdev->is_ibss_create) {
            if (!osdev->disable_ibss_create) {
                IEEE80211_DPRINTF(vap, IEEE80211_MSG_IOCTL, "%s :join fail, switch to create  \n", __func__);
                osdev->is_ibss_create = 1;
            } else {
                IEEE80211_DPRINTF(vap, IEEE80211_MSG_IOCTL, "%s :join fail, retry join  \n", __func__);
            }
            osif_ibss_init(dev);
        } else {
            IEEE80211_DPRINTF(vap, IEEE80211_MSG_IOCTL, "%s :create fail! vap down!  \n", __func__);
            osdev->is_ibss_create = 0;
            netif_carrier_off(dev);
            osif_vap_down(dev);             
        }
    } else if (smevent->event == WLAN_IBSS_SM_EVENT_DISCONNECT) {
        IEEE80211_DPRINTF(vap, IEEE80211_MSG_IOCTL, "%s :ibss disconnected \n", __func__);
        osdev->is_ibss_create = 0;
        netif_carrier_off(dev);
        osif_vap_down(dev); 
    }
}
#endif

void
osif_replay_failure_indication(os_handle_t osif, const u_int8_t *frm, u_int keyix)
{
    static const char * tag = "MLME-REPLAYFAILURE.indication";
    struct net_device *dev = ((osif_dev *)osif)->netdev;    
    osif_dev *osdev = ath_netdev_priv(dev);
    wlan_if_t vap;
    struct ieee80211_frame *wh = (struct ieee80211_frame *)frm;
    union iwreq_data wrqu;
    char buf[128];
    
    vap = osdev->os_if;

    IEEE80211_DPRINTF(vap, IEEE80211_MSG_CRYPTO,
        "%s: replay detected <keyix %d>\n",
        ether_sprintf(wh->i_addr2), keyix);

    /* TODO: needed parameters: count, keyid, key type, src address, TSC */
    snprintf(buf, sizeof(buf), "%s(keyid=%d %scast addr=%s)", tag,
        keyix, IEEE80211_IS_MULTICAST(wh->i_addr1) ?  "broad" : "uni",
        ether_sprintf(wh->i_addr1));
    memset(&wrqu, 0, sizeof(wrqu));
    wrqu.data.length = strlen(buf);
    wireless_send_event(dev, IWEVCUSTOM, &wrqu, buf);
}

void
osif_michael_failure_indication(os_handle_t osif, const u_int8_t *frm, u_int keyix)
{
    static const char *tag = "MLME-MICHAELMICFAILURE.indication";
    struct net_device *dev = ((osif_dev *)osif)->netdev;
    osif_dev *osdev = ath_netdev_priv(dev);
    wlan_if_t vap;
    struct ieee80211_frame *wh = (struct ieee80211_frame *)frm;
    union iwreq_data wrqu;
    char buf[128];
    
    vap = osdev->os_if;

    IEEE80211_DPRINTF(vap, IEEE80211_MSG_CRYPTO,
        "%s: Michael MIC verification failed <keyix %d>",
        ether_sprintf(wh->i_addr2), keyix);

    /* TODO: needed parameters: count, keyid, key type, src address, TSC */
    snprintf(buf, sizeof(buf), "%s(keyid=%d addr=%s)", tag,
        keyix, ether_sprintf(wh->i_addr2));
    memset(&wrqu, 0, sizeof(wrqu));
    wrqu.data.length = strlen(buf);
    wireless_send_event(dev, IWEVCUSTOM, &wrqu, buf);
}

void
osif_notify_push_button(struct net_device *dev, u_int32_t push_time)
{
    static char *tag = "PUSH-BUTTON.indication";
    union iwreq_data wrqu;
    char buf[128];

    snprintf(buf, sizeof(buf), "%s Push dur=%d", tag, push_time);
    memset(&wrqu, 0, sizeof(wrqu));
    wrqu.data.length = strlen(buf);
    wireless_send_event(dev, IWEVCUSTOM, &wrqu, buf);
}

int
ieee80211_load_module(const char *modname)
{
    request_module(modname);
    return 0;
}

static void osif_channel_change (os_handle_t osif, wlan_chan_t chan)
{

}

static void osif_country_changed (os_handle_t osif, char *country)
{

}

static void osif_receive (os_if_t osif, wbuf_t wbuf,
                        u_int16_t type, u_int16_t subtype,
                        ieee80211_recv_status *rs)
{
    struct sk_buff *skb = (struct sk_buff *)wbuf;

    if (type != IEEE80211_FC0_TYPE_DATA) {
        wbuf_free(wbuf);
        return;
    }
    /* deliver the data to network interface */
    __osif_deliver_data(osif, skb);
}

#ifdef ATH_WPS_IE
/* From Atheros version:
* //depot/sw/releases/linuxsrc/src/802_11/madwifi/madwifi/net80211/ieee80211_input.c
*
* This is an entirely different approach which uses "wireless events"...
* also we do it for WPS containing packets only.
*/

void osif_forward_mgmt_to_app(os_if_t osif, wbuf_t wbuf,
                                        u_int16_t type, u_int16_t subtype)
{
    struct net_device *dev = OSIF_TO_NETDEV(osif);
    struct sk_buff *skb = (struct sk_buff *)wbuf;
    osif_dev  *osifp = (osif_dev *) osif;

    int filter_flag =0;
#define WPS_FRAM_TAG_SIZE 30 /* hardcoded in atheros_wireless_event_wireless_custom */
    char *tag=NULL;
    #if 0       /* should be, but IW_CUSTOM_MAX is only 256 */
    const int bufmax = IW_CUSTOM_MAX;
    #else       /* HACK use size for IWEVASSOCREQIE instead */
    const int bufmax = IW_GENERIC_IE_MAX;
    #endif

    /* HACK: forward WPS frames only */
    if (!wlan_frm_haswscie(wbuf))
    {
        return;
    }

    switch (subtype)
    {
    case IEEE80211_FC0_SUBTYPE_BEACON:
        filter_flag = osifp->app_filter & IEEE80211_FILTER_TYPE_BEACON;
        tag = "Manage.beacon";
        break;
    case IEEE80211_FC0_SUBTYPE_PROBE_REQ:
        filter_flag = osifp->app_filter & IEEE80211_FILTER_TYPE_PROBE_REQ;
        tag = "Manage.prob_req";
        break;
    case IEEE80211_FC0_SUBTYPE_PROBE_RESP:
        filter_flag = osifp->app_filter & IEEE80211_FILTER_TYPE_PROBE_RESP;
        tag = "Manage.prob_resp";
        break;
    case IEEE80211_FC0_SUBTYPE_ASSOC_REQ:
    case IEEE80211_FC0_SUBTYPE_REASSOC_REQ:
        filter_flag = osifp->app_filter & IEEE80211_FILTER_TYPE_ASSOC_REQ;
        tag = "Manage.assoc_req";
        break;
    case IEEE80211_FC0_SUBTYPE_ASSOC_RESP:
    case IEEE80211_FC0_SUBTYPE_REASSOC_RESP:
        filter_flag = osifp->app_filter & IEEE80211_FILTER_TYPE_ASSOC_RESP;
        tag = "Manage.assoc_resp";
        break;
    case IEEE80211_FC0_SUBTYPE_AUTH:
        filter_flag = osifp->app_filter & IEEE80211_FILTER_TYPE_AUTH;
        tag = "Manage.auth";
        break;
    case IEEE80211_FC0_SUBTYPE_DEAUTH:
        filter_flag = osifp->app_filter & IEEE80211_FILTER_TYPE_DEAUTH;
        tag = "Manage.deauth";
        break;
    case IEEE80211_FC0_SUBTYPE_DISASSOC:
        filter_flag = osifp->app_filter & IEEE80211_FILTER_TYPE_DISASSOC;
        tag = "Manage.disassoc";
        break;
    default:
        break;
    }
    if (filter_flag)
    {
        union iwreq_data wrqu;
        char *buf;
        size_t bufsize = WPS_FRAM_TAG_SIZE + skb->len;

        if(bufsize > bufmax)
            return;
        buf = OS_MALLOC(osifp->os_handle, bufsize, GFP_KERNEL);
        if (buf == NULL) return;
        OS_MEMZERO(&wrqu, sizeof(wrqu));
        wrqu.data.length = bufsize;
        OS_MEMZERO(buf, bufsize);
        snprintf(buf, WPS_FRAM_TAG_SIZE, "%s %d", tag, skb->len);
        OS_MEMCPY(buf+WPS_FRAM_TAG_SIZE, skb->data, skb->len);
        #if 0   /* the way it should? be */
        wireless_send_event(dev, IWEVCUSTOM, &wrqu, buf);
        #else   /* HACK to get around 256 byte limit of IWEVCUSTOM */
        /* Note: application should treat IWEVASSOCREQIE same as IWEVCUSTOM
        * and should check for both.
        * IWEVASSOCREQIE is not used for anything (else) at least
        * not for Atheros chip driver.
        */

//        IEEE80211_DPRINTF(NULL, IEEE80211_MSG_IOCTL, "%s : filter_flag=%x, subtype=%x\n",
//                                      __func__,osifp->app_filter,subtype);
        wireless_send_event(dev, IWEVASSOCREQIE, &wrqu, buf);
        #endif
        OS_FREE(buf);
    }
#undef  WPS_FRAM_TAG_SIZE
}
#endif


static int  osif_receive_filter_80211 (os_if_t osif, wbuf_t wbuf,
                                        u_int16_t type, u_int16_t subtype,
                                        ieee80211_recv_status *rs)
{
#ifdef ATH_WPS_IE
    if (type == IEEE80211_FC0_TYPE_MGT) {
        osif_forward_mgmt_to_app(osif, wbuf, type, subtype);
    }
#endif
    return 0;
}

static void osif_receive_monitor_80211 (os_if_t osif, wbuf_t wbuf,
                                        ieee80211_recv_status *rs)
{
    wlan_ng_prism2_header template = {
        .msgcode = (DIDmsg_lnxind_wlansniffrm),
        .msglen = (sizeof(wlan_ng_prism2_header)),

        .hosttime.did = (DIDmsg_lnxind_wlansniffrm_hosttime),
        .hosttime.status = 0,
        .hosttime.len = (4),

        .mactime.did = (DIDmsg_lnxind_wlansniffrm_mactime),
        .mactime.status = 0,
        .mactime.len = (4),

        .istx.did = (DIDmsg_lnxind_wlansniffrm_istx),
        .istx.status = 0,
        .istx.len = (4),
        .istx.data = (P80211ENUM_truth_false),

        .frmlen.did = (DIDmsg_lnxind_wlansniffrm_frmlen),
        .frmlen.status = 0,
        .frmlen.len = (4),

        .channel.did = (DIDmsg_lnxind_wlansniffrm_channel),
        .channel.status = 0,
        .channel.len = (4),

        .rssi.did = (DIDmsg_lnxind_wlansniffrm_rssi),
        .rssi.status = P80211ENUM_msgitem_status_no_value,
        .rssi.len	= (4),

        .signal.did = (DIDmsg_lnxind_wlansniffrm_signal),
        .signal.status = 0,
        .signal.len = (4),

        .rate.did = (DIDmsg_lnxind_wlansniffrm_noise),
        .rate.status = 0,
        .rate.len = (4),
    };
    struct sk_buff *skb, *tskb;
    wlan_ng_prism2_header *phdr;
    u_int32_t len;
    osif_dev *osifp = (osif_dev *)osif;
    struct ieee80211vap *vap = osifp->os_if;
    struct net_device *dev = osifp->netdev;
    struct net_device_stats *stats = &osifp->os_devstats;
    struct ieee80211com *ic = wlan_vap_get_devhandle(vap);

    skb = (struct sk_buff *)wbuf;

    skb->len -= 4;

    len = skb->len;

    if (skb_headroom(wbuf) < sizeof(wlan_ng_prism2_header)) {
        tskb = skb_realloc_headroom(skb, sizeof(wlan_ng_prism2_header));
        dev_kfree_skb(skb);
        skb = tskb;
    }

    phdr = (wlan_ng_prism2_header *) skb_push(skb, sizeof(wlan_ng_prism2_header));
    OS_MEMZERO(phdr, sizeof(wlan_ng_prism2_header));
    *phdr = template;

    phdr->hosttime.data = (jiffies);
    phdr->mactime.data = (rs->rs_tstamp.tsf);
    phdr->frmlen.data = (len);
    phdr->channel.data = wlan_mhz2ieee(ic, rs->rs_freq, 0);
    phdr->rssi.data = 0;
    phdr->signal.data = (rs->rs_rssi);
    phdr->rate.data = (rs->rs_datarate/500);

    strncpy(phdr->devname, dev->name, sizeof(phdr->devname));

    skb->dev = dev;
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,22)
    skb->mac.raw = skb->data;
#else
    skb_reset_mac_header(skb);
#endif
    skb->ip_summed = CHECKSUM_NONE;
    skb->pkt_type = PACKET_OTHERHOST;
    skb->protocol = __constant_htons(ETH_P_80211_RAW);

    netif_rx(skb);

    stats->rx_packets++;
    stats->rx_bytes += skb->len;
}
#if ATH_SUPPORT_FLOWMAC_MODULE
int
dev_queue_status(struct net_device *dev, int margin)
{
	struct Qdisc *fifoqdisc;
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,15))
    struct netdev_queue *txq; 
#endif
    int qlen = 0;

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,15))
    txq = netdev_get_tx_queue(dev, 0);
    ASSERT((txq != NULL));
    fifoqdisc = txq->qdisc;
#else
	fifoqdisc = rcu_dereference(dev->qdisc);
#endif

    qlen = skb_queue_len(&fifoqdisc->q);
    if ((dev->tx_queue_len > margin) && 
            (qlen > dev->tx_queue_len - margin)) {
        return -ENOBUFS;
    }
    return 0;
}
int
dev_queue_length(struct net_device *dev)
{
	struct Qdisc *fifoqdisc;
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,15))
    struct netdev_queue *txq; 
#endif
    int qlen = 0;

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,15))
    txq = netdev_get_tx_queue(dev, 0);
    ASSERT((txq != NULL));
    fifoqdisc = txq->qdisc;
#else
	fifoqdisc = rcu_dereference(dev->qdisc);
#endif
    qlen = skb_queue_len(&fifoqdisc->q);
    return qlen;
}
#endif
/*
* hand over the wbuf to the com device (ath dev).
*/
static int
osif_dev_xmit_queue (os_if_t osif, wbuf_t wbuf)
{
    int err;
    struct net_device *comdev;
    struct sk_buff *skb = (struct sk_buff *)wbuf;
    int rval=0;
    comdev = ((osif_dev *)osif)->os_comdev;

    skb->dev = comdev;
    /*
     * rval would be -ENOMEM if there are 1000-128 frames left in the backlog
     * queue. We return this value out to the caller. The assumption of
     * caller is to make sure that he stalls the queues just after this. If
     * we do not let this frame out, node_ref issues might show up.
     */
#if ATH_SUPPORT_FLOWMAC_MODULE
    rval = dev_queue_status(comdev, 128);
#endif
    err = dev_queue_xmit(skb);
#if ATH_SUPPORT_FLOWMAC_MODULE
    if (err == NET_XMIT_DROP) {
        /* TODO FIXME to stop the queue here with flow control enabled
        */
        rval = -ENOBUFS;
    }
    /* if there is no space left in below queue, make sure that we pause the
     * vap queue also
     */
    if (err == -ENOMEM) rval = -ENOBUFS;
#endif
    return rval;
}

/*
* hand over the wbuf to the vap if (queue back into the umac).
*/
static void osif_vap_xmit_queue (os_if_t osif, wbuf_t wbuf)
{
    int err;
    struct net_device *dev = OSIF_TO_NETDEV(osif);
    osif_dev *osdev = ath_netdev_priv(dev);
    wlan_if_t vap;
    struct sk_buff *skb = (struct sk_buff *)wbuf;

    vap = osdev->os_if;
    skb->dev = dev;
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,22))
    skb->mac.raw = skb->data;
    skb->nh.raw = skb->data + sizeof(struct ether_header);
#else
    skb_reset_mac_header(skb);
    skb_set_network_header(skb, sizeof(struct ether_header));
#endif
    skb->protocol = __constant_htons(ETH_P_802_2);
    /* XXX inser`t vlan tage before queue it? */
    err = dev_queue_xmit(skb);

    if (err != 0) {
        IEEE80211_DPRINTF(vap, IEEE80211_MSG_ANY, "%s: wbuf xmit failed \n", __func__);
    }

}

#if ATH_SUPPORT_FLOWMAC_MODULE
static void
osif_pause_queue (os_if_t osif, int pause, unsigned int pctl)
{
    struct net_device *dev = OSIF_TO_NETDEV(osif);
    struct ath_softc_net80211 *scn = ath_netdev_priv(((osif_dev*)osif)->os_comdev);
    /* if puase, call the pause other wise wake */

    if (!dev || !scn) return ;
    
    if (pause) {
        netif_stop_queue(dev);
        /* Literally we should have stopped the ethernet just over 2/3 queue
         * is full. But for this to happen this should get called through
         * network stack first. As soon as we stopped the queue, we would
         * not be getting any calls to hard_start. That way we never able to
         * pause the Ethernet effectivly. The only simple way is to let us
         * pause the both. Otherwise kernel code need to change, but that
         * becomes more non-portable code.
         * Or else, the vap should have queue of frames and when reaches
         * limit pause the Ethernet. 
         */
        
        if (scn->sc_ops->flowmac_pause && (pctl & 0x2)) {
            scn->sc_ops->flowmac_pause(scn->sc_dev, 1);

        }


    } else {
        netif_wake_queue(dev);

        if (scn->sc_ops->flowmac_pause && (pctl & 0x2)) {
            scn->sc_ops->flowmac_pause(scn->sc_dev, 0);
        
        }

    }
}

#endif

static void osif_xmit_update_status(os_if_t osif, wbuf_t wbuf,
                                    ieee80211_xmit_status *ts)
{

}
#if ATH_SUPPORT_IWSPY

#define RSSI_QUAL_MIN		1
#define RSSI_QUAL_THRESH1	5
#define RSSI_QUAL_THRESH2	30
#define RSSI_QUAL_MAX		42
#define QUAL_VALUE_MIN		0
#define QUAL_VALUE_THRESH1	5
#define QUAL_VALUE_THRESH2	85
#define QUAL_VALUE_MAX	94

static void
osif_set_linkquality(struct iw_quality *iq, u_int rssi)
{
    if(rssi >= RSSI_QUAL_MAX)
        iq->qual = QUAL_VALUE_MAX ;
    else if(rssi >= RSSI_QUAL_THRESH2)
        iq->qual = QUAL_VALUE_THRESH2 + ((QUAL_VALUE_MAX-QUAL_VALUE_THRESH2)*(rssi-RSSI_QUAL_THRESH2) + (RSSI_QUAL_MAX-RSSI_QUAL_THRESH2)/2) \
					/ (RSSI_QUAL_MAX-RSSI_QUAL_THRESH2) ;
    else if(rssi >= RSSI_QUAL_THRESH1)
        iq->qual = QUAL_VALUE_THRESH1 + ((QUAL_VALUE_THRESH2-QUAL_VALUE_THRESH1)*(rssi-RSSI_QUAL_THRESH1) + (RSSI_QUAL_THRESH2-RSSI_QUAL_THRESH1)/2) \
					/ (RSSI_QUAL_THRESH2-RSSI_QUAL_THRESH1) ;
    else if(rssi >= RSSI_QUAL_MIN)
        iq->qual = rssi;
    else
        iq->qual = QUAL_VALUE_MIN;

    iq->noise = 161;        /* -95dBm */
    iq->level = iq->noise + rssi ;
    iq->updated = 0xf;
}

/*
 * Call for the driver to update the spy data.
 * For now, the spy data is a simple array. As the size of the array is
 * small, this is good enough. If we wanted to support larger number of
 * spy addresses, we should use something more efficient...
 */
void osif_iwspy_update(os_if_t osif,
			 u_int8_t *address,
			 int8_t rssi)
{
    osif_dev  *osifp = (osif_dev *) osif;
    struct iw_spy_data *spydata = &osifp->spy_data;
	int	                i;
	int                 match = -1;
    struct iw_quality   iq;
    
	/* Make sure driver is not buggy or using the old API */
	if(!spydata)
		return;

	/* Update all records that match */
	for(i = 0; i < spydata->spy_number; i++)
		if(!memcmp(address, spydata->spy_address[i], ETH_ALEN)) {
            osif_set_linkquality(&iq, rssi);
			memcpy(&(spydata->spy_stat[i]), &iq, sizeof(struct iw_quality));
			match = i;
		}

#if 0 /* for SIOCSIWTHRSPY/SIOCGIWTHRSPY, not to implement now */
	/* Generate an event if we cross the spy threshold.
	 * To avoid event storms, we have a simple hysteresis : we generate
	 * event only when we go under the low threshold or above the
	 * high threshold. */
	if(match >= 0) {
		if(spydata->spy_thr_under[match]) {
			if(wstats->level > spydata->spy_thr_high.level) {
				spydata->spy_thr_under[match] = 0;
				iw_send_thrspy_event(dev, spydata,
						     address, wstats);
			}
		} else {
			if(wstats->level < spydata->spy_thr_low.level) {
				spydata->spy_thr_under[match] = 1;
				iw_send_thrspy_event(dev, spydata,
						     address, wstats);
			}
		}
	}
#endif
}
/*
 * Call for reset the spy data by mac address
 */
void osif_iwspy_reset_data(os_if_t osif,
			 u_int8_t *address)
{
    osif_dev  *osifp = (osif_dev *) osif;
    struct iw_spy_data *spydata = &osifp->spy_data;
	int	                i;
    
	/* Make sure driver is not buggy or using the old API */
	if(!spydata)
		return;

	/* Reset all records that match */
	for(i = 0; i < spydata->spy_number; i++)
		if(!memcmp(address, spydata->spy_address[i], ETH_ALEN)) {
            memset(&(spydata->spy_stat[i]), 0x0, sizeof(struct iw_quality));
		}
}
#endif

static void osif_linkspeed(os_handle_t osif, u_int32_t rxlinkspeed, u_int32_t txlinkspeed)
{

}

static void osif_beacon_miss(os_handle_t osif)
{
    struct net_device *dev = OSIF_TO_NETDEV(osif);
    osif_dev *osdev = ath_netdev_priv(dev);
    wlan_if_t vap;
    
    vap = osdev->os_if;

    IEEE80211_DPRINTF(vap, IEEE80211_MSG_IOCTL, "%s: beacon miss \n", __func__);

}

/*
 * caller gets first element of list. Caller must free the element.
 */
struct pending_rx_frames_list *osif_fetch_p2p_mgmt(struct net_device *dev)
{
    osif_dev  *osifp = ath_netdev_priv(dev);
    unsigned long flags=0;
    struct pending_rx_frames_list *pending = NULL;
    
    spin_lock_irqsave(&osifp->list_lock, flags);
    if (!list_empty(&osifp->pending_rx_frames)) {
        pending = (void *) osifp->pending_rx_frames.next;
        list_del(&pending->list);
    }
    spin_unlock_irqrestore(&osifp->list_lock, flags);

    return pending;
}
/*
 * queue a big event for later delivery to user space
 * Caller must provide event->u.rx_frame.frame_len and
 * event->u.rx_frame.frame_buf for the data to be queued for later delivery
 */
void osif_p2p_rx_frame_handler(osif_dev *osifp, 
                               wlan_p2p_event *event,
                               int frame_type)
{
    struct pending_rx_frames_list *pending;
    unsigned long flags;

    pending = (struct pending_rx_frames_list *)OS_MALLOC(osifp->os_handle, 
                                 sizeof(*pending) + event->u.rx_frame.frame_len,
                                 GFP_ATOMIC);
    if (pending) {
        pending->frame_type = frame_type;
        pending->rx_frame = event->u.rx_frame;

        memcpy(pending->extra, event->u.rx_frame.frame_buf,
                                event->u.rx_frame.frame_len);
                                
        INIT_LIST_HEAD (&pending->list);
        spin_lock_irqsave(&osifp->list_lock, flags);
        list_add_tail(&pending->list, &osifp->pending_rx_frames);
        spin_unlock_irqrestore(&osifp->list_lock, flags);
    } else {
        printk(KERN_CRIT "%s OS_MALLOC failed, frame lost", __func__);
    }
}

struct event_data_chan_start {
    u_int32_t freq; 
    u_int32_t duration; 
    u_int32_t req_id; 
};

struct event_data_chan_end {
    u_int32_t freq; 
    u_int32_t reason; 
    u_int32_t duration; 
    u_int32_t req_id; 
};

static void osif_p2p_dev_event_handler(void *arg, wlan_p2p_event *event)
{
    osif_dev *osifp = arg;
    union iwreq_data wreq;

    memset(&wreq, 0, sizeof(wreq));

    switch (event->type) {
    case WLAN_P2PDEV_CHAN_START:
        do {
            struct event_data_chan_start event_data;
            wreq.data.flags = IEEE80211_EV_CHAN_START;
            wreq.data.length = sizeof(struct event_data_chan_start);
            event_data.freq = event->u.chan_start.freq; 
            event_data.duration = event->u.chan_start.duration; 
            event_data.req_id = event->req_id; 
            wireless_send_event(osifp->netdev, IWEVCUSTOM, &wreq,
                                (void *) &event_data);
        } while(0);
        break;
    case WLAN_P2PDEV_CHAN_END:
        do {
            struct event_data_chan_end event_data;
            wreq.data.flags = IEEE80211_EV_CHAN_END;
            wreq.data.length = sizeof(struct event_data_chan_end);
            event_data.freq = event->u.chan_end.freq; 
            event_data.reason = event->u.chan_end.reason; 
            event_data.duration = event->u.chan_end.duration; 
            event_data.req_id = event->req_id; 
            wireless_send_event(osifp->netdev, IWEVCUSTOM, &wreq,
                                (void *) &event_data);
        } while(0);
        break;
    case WLAN_P2PDEV_RX_FRAME:
        /* actually send that data is available */
        wreq.data.flags = IEEE80211_EV_RX_MGMT;
        osif_p2p_rx_frame_handler(osifp, event, IEEE80211_EV_RX_MGMT);
        wireless_send_event(osifp->netdev, IWEVCUSTOM, &wreq, NULL);
        break;
    case WLAN_P2PDEV_SCAN_END:
        break;
    default:
        printk(" *Unhandled P2P EVENT: %d *\n", event->type);
        break;
    }
}

#if ATH_SUPPORT_IBSS_NETLINK_NOTIFICATION
/*
 * WLAN MLME ibss event handler support function 
 */
void osif_join_complete_adhoc(os_handle_t osif, IEEE80211_STATUS ieeeStatus)
{
    osif_dev *osifp = (osif_dev *)osif;
#if ATH_DEBUG
    wlan_if_t vap = osifp->os_if;
#endif           
    IEEE80211_DPRINTF(vap, IEEE80211_MSG_IOCTL, "%s: Status(%d)\n", __func__, ieeeStatus);
    
    /*
     * Linux IBSS mode re-register mlme event handle which replace original handle in UMAC-SME, 
     * so notify UMAC-SME when adhoc join complete.
     * reference funcion : sm_join_complete()  umac\sme\ieee80211_ibss_sm.c
     */
    wlan_ibss_sm_join_complete(osifp->sm_ibss_handle, ieeeStatus);
}

//remove after sagem finish
void osif_auth_complete(os_handle_t osif, IEEE80211_STATUS ieeeStatus)
{
#if ATH_DEBUG
    osif_dev *osifp = (osif_dev *)osif;
    wlan_if_t vap = osifp->os_if;
#endif
    IEEE80211_DPRINTF(vap, IEEE80211_MSG_IOCTL, "%s: Status(%d)\n", __func__, ieeeStatus);
}

//remove after sagem finish
void osif_assoc_req(os_handle_t osif, wbuf_t wbuf)
{
#if ATH_DEBUG
    osif_dev *osifp = (osif_dev *)osif;
    wlan_if_t vap = osifp->os_if;
#endif
    IEEE80211_DPRINTF(vap, IEEE80211_MSG_IOCTL, "%s: \n", __func__);
}

//remove after sagem finish
void osif_assoc_complete(os_handle_t osif, IEEE80211_STATUS ieeeStatus, u_int16_t aid, wbuf_t wbuf)
{
#if ATH_DEBUG
    osif_dev *osifp = (osif_dev *)osif;
    wlan_if_t vap = osifp->os_if;
#endif
    IEEE80211_DPRINTF(vap, IEEE80211_MSG_IOCTL, "%s: Status(%d) aid(%d)\n", __func__, ieeeStatus, aid);
}

//remove after sagem finish
void osif_reassoc_complete(os_handle_t osif, IEEE80211_STATUS ieeeStatus, u_int16_t aid, wbuf_t wbuf)
{
#if ATH_DEBUG
    osif_dev *osifp = (osif_dev *)osif;
    wlan_if_t vap = osifp->os_if;
#endif
    IEEE80211_DPRINTF(vap, IEEE80211_MSG_IOCTL, "%s: Status(%d) aid(%d)\n", __func__, ieeeStatus, aid);
}

void osif_deauth_complete(os_handle_t osif, u_int8_t *macaddr, IEEE80211_STATUS reason_code)
{
    osif_dev *osifp = (osif_dev *)osif;
#if ATH_DEBUG
    wlan_if_t vap = osifp->os_if;
#endif
    union iwreq_data wreq;
    struct net_device *dev = osifp->netdev;
    ath_netlink_event_t event;

    IEEE80211_DPRINTF(vap, IEEE80211_MSG_IOCTL, "%s: macaddr(%02x:%02x:%02x:%02x:%02x:%02x) reason(%d)\n", 
                      __func__, macaddr[0], macaddr[1], macaddr[2], macaddr[3], macaddr[4], macaddr[5], reason_code);
    /*
     * notify upper layor application an IBSS leave
     */
    memset(&wreq, 0, sizeof(wreq));
    IEEE80211_ADDR_COPY(wreq.addr.sa_data, macaddr);
    wreq.addr.sa_family = ARPHRD_ETHER;
    wireless_send_event(dev, IWEVEXPIRED, &wreq, NULL);

    memset(&event, 0x0, sizeof(event));
    event.type = ATH_EVENT_NODE_LEAVE;
    IEEE80211_ADDR_COPY(event.mac, macaddr);
    ath_adhoc_netlink_send(&event, NULL, 0);     
}

void osif_deauth_indication_ibss(os_handle_t osif, u_int8_t *macaddr, u_int16_t reason_code)
{
    osif_dev *osifp = (osif_dev *)osif;
#if ATH_DEBUG
    wlan_if_t vap = osifp->os_if;
#endif
    union iwreq_data wreq;
    struct net_device *dev = osifp->netdev;
    ath_netlink_event_t event;

    IEEE80211_DPRINTF(vap, IEEE80211_MSG_IOCTL, "%s: macaddr(%02x:%02x:%02x:%02x:%02x:%02x) reason(%d)\n", 
                      __func__, macaddr[0], macaddr[1], macaddr[2], macaddr[3], macaddr[4], macaddr[5], reason_code);
    /*
     * notify upper layor application an IBSS leave
     */
    memset(&wreq, 0, sizeof(wreq));
    IEEE80211_ADDR_COPY(wreq.addr.sa_data, macaddr);
    wreq.addr.sa_family = ARPHRD_ETHER;
    wireless_send_event(dev, IWEVEXPIRED, &wreq, NULL);


    memset(&event, 0x0, sizeof(event));
    event.type = ATH_EVENT_NODE_LEAVE;
    IEEE80211_ADDR_COPY(event.mac, macaddr);
    ath_adhoc_netlink_send(&event, NULL, 0);     
}

void osif_assoc_indication_ibss(os_handle_t osif, u_int8_t *macaddr, u_int16_t result, wbuf_t wbuf, wbuf_t resp_wbuf)
{
    osif_dev *osifp = (osif_dev *)osif;
#if ATH_DEBUG
    wlan_if_t vap = osifp->os_if;
#endif
    union iwreq_data wreq;
    struct net_device *dev = osifp->netdev;
    ath_netlink_event_t event;

    IEEE80211_DPRINTF(vap, IEEE80211_MSG_IOCTL, "%s: macaddr(%02x:%02x:%02x:%02x:%02x:%02x) result(%d)\n", 
                      __func__, macaddr[0], macaddr[1], macaddr[2], macaddr[3], macaddr[4], macaddr[5], result);
    /*
     * notify upper layor application(s) including supplicant, an IBSS join
     */
    memset(&wreq, 0, sizeof(wreq));
    IEEE80211_ADDR_COPY(wreq.addr.sa_data, macaddr);
    wreq.addr.sa_family = ARPHRD_ETHER;
    wireless_send_event(dev, IWEVREGISTERED, &wreq, NULL);

    memset(&event, 0x0, sizeof(event));
    event.type = ATH_EVENT_NODE_JOIN;
    IEEE80211_ADDR_COPY(event.mac, macaddr);
    ath_adhoc_netlink_send(&event, NULL, 0);     
}

void osif_disassoc_indication_ibss(os_handle_t osif, u_int8_t *macaddr, u_int32_t reason_code)
{
    osif_dev *osifp = (osif_dev *)osif;
#if ATH_DEBUG
    wlan_if_t vap = osifp->os_if;
#endif
    union iwreq_data wreq;
    struct net_device *dev = osifp->netdev;
    ath_netlink_event_t event;

    IEEE80211_DPRINTF(vap, IEEE80211_MSG_IOCTL, "%s: macaddr(%02x:%02x:%02x:%02x:%02x:%02x) reason_code(%d)\n", 
                      __func__, macaddr[0], macaddr[1], macaddr[2], macaddr[3], macaddr[4], macaddr[5], reason_code);     

#if ATH_SUPPORT_IWSPY
	/* Reset iwspy data */
	osif_iwspy_reset_data(osif, macaddr);
#endif
	
    /*
     * notify upper layor application an IBSS leave
     */
    memset(&wreq, 0, sizeof(wreq));
    IEEE80211_ADDR_COPY(wreq.addr.sa_data, macaddr);
    wreq.addr.sa_family = ARPHRD_ETHER;
    wireless_send_event(dev, IWEVEXPIRED, &wreq, NULL);

    memset(&event, 0x0, sizeof(event));
    event.type = ATH_EVENT_NODE_LEAVE;
    IEEE80211_ADDR_COPY(event.mac, macaddr);
    ath_adhoc_netlink_send(&event, NULL, 0);     
}

void osif_ibss_merge_start_indication(os_handle_t osif, u_int8_t *bssid)
{
#if ATH_DEBUG
    osif_dev *osifp = (osif_dev *)osif;
    wlan_if_t vap = osifp->os_if;
#endif
    IEEE80211_DPRINTF(vap, IEEE80211_MSG_IOCTL, "%s: \n", __func__);     
}

void osif_ibss_merge_completion_indication(os_handle_t osif, u_int8_t *bssid)
{
#if ATH_DEBUG
    osif_dev *osifp = (osif_dev *)osif;
    wlan_if_t vap = osifp->os_if;
#endif
    IEEE80211_DPRINTF(vap, IEEE80211_MSG_IOCTL, "%s: \n", __func__);     
}
#endif /* end of #if ATH_SUPPORT_IBSS_NETLINK_NOTIFICATION */

#if ATH_SUPPORT_IBSS_NETLINK_NOTIFICATION
void osif_ibss_rssi_monitor(os_handle_t osif, u_int8_t *macaddr, u_int32_t rssi_class)
{
#if ATH_DEBUG
  osif_dev *osifp = (osif_dev *)osif;
  wlan_if_t vap = osifp->os_if;
#endif
  ath_netlink_event_t event;

  IEEE80211_DPRINTF(vap, IEEE80211_MSG_IOCTL, "%s: macaddr(%02x:%02x:%02x:%02x:%02x:%02x) RSSI level(%d)\n",
		    __func__, macaddr[0], macaddr[1], macaddr[2], macaddr[3], macaddr[4], macaddr[5], rssi_class);

  memset(&event, 0x0, sizeof(event));
  event.type = ATH_EVENT_NODE_RSSI_MONITOR;
  IEEE80211_ADDR_COPY(event.mac, macaddr);
  ath_adhoc_netlink_send(&event, (char *)&rssi_class, sizeof(u_int32_t));
}
#endif

static wlan_misc_event_handler_table sta_misc_evt_handler = {
    osif_channel_change,                   /* wlan_channel_change */
    osif_country_changed,                  /* wlan_country_changed */
    osif_linkspeed,                        /* wlan_linkspeed */
    osif_michael_failure_indication,       /* wlan_michael_failure_indication */
    osif_replay_failure_indication,        /* wlan_replay_failure_indication */
    osif_beacon_miss,                      /* wlan_beacon_miss_indication */
    NULL,                                  /* wlan_device_error_indication */
    NULL,                                  /* wlan_sta_clonemac_indication */
    NULL,                                  /* wlan_sta_scan_entry_update */
    NULL,                                  /* wlan_ap_stopped */
#if ATH_SUPPORT_WAPI
    NULL,                                  /*wlan_sta_rekey_indication*/
#endif
#if ATH_SUPPORT_IBSS_NETLINK_NOTIFICATION
    NULL,
#endif
};

static wlan_event_handler_table common_evt_handler = {
#ifndef __CARRIER_PLATFORM__
    osif_receive,
#else
    osif_pltfrm_receive,
#endif
    osif_receive_filter_80211,
    osif_receive_monitor_80211,
    osif_dev_xmit_queue,
    osif_vap_xmit_queue,
    osif_xmit_update_status,
#if ATH_SUPPORT_IWSPY
	osif_iwspy_update,
#endif
#if ATH_SUPPORT_FLOWMAC_MODULE
    osif_pause_queue,
#endif
};

static wlan_mlme_event_handler_table sta_mlme_evt_handler = {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
#if 0
    osif_leave_indication_sta,
    osif_assoc_indication_sta,
    osif_reassoc_indication_sta,
    osif_leave_indication_sta,
#endif
    NULL,
    NULL
} ;

#if ATH_SUPPORT_IBSS_NETLINK_NOTIFICATION
/* MLME handlers */
static wlan_mlme_event_handler_table ibss_mlme_evt_handler = {
    /* MLME confirmation handler */
    NULL,                                  /* mlme_join_complete_infra */
    osif_join_complete_adhoc,              /* mlme_join_complete_adhoc */
    osif_auth_complete,                    /* mlme_auth_complete */
    osif_assoc_req,                        /* mlme_assoc_req */
    osif_assoc_complete,                   /* mlme_assoc_complete */
    osif_reassoc_complete,                 /* mlme_reassoc_complete */
    osif_deauth_complete,                  /* mlme_deauth_complete */
    NULL,                                  /* mlme_deassoc_complete */

    /* MLME indication handler */
    NULL,                                  /* mlme_auth_indication */
    osif_deauth_indication_ibss,           /* mlme_deauth_indication */
    osif_assoc_indication_ibss,            /* mlme_assoc_indication */
    NULL,                                  /* mlme_reassoc_indication */
    osif_disassoc_indication_ibss,         /* mlme_disassoc_indication */
    osif_ibss_merge_start_indication,      /* mlme_ibss_merge_start_indication */
    osif_ibss_merge_completion_indication, /* mlme_ibss_merge_completion_indication */
    NULL,                                  /* wlan_radar_detected */
};
#endif /* end of #if ATH_SUPPORT_IBSS_NETLINK_NOTIFICATION */

#if ATH_SUPPORT_IBSS_NETLINK_NOTIFICATION
static wlan_misc_event_handler_table ibss_misc_evt_handler = {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,                               /* wlan_ap_stopped */
#if ATH_SUPPORT_WAPI
    NULL,
#endif
#if ATH_SUPPORT_IBSS_NETLINK_NOTIFICATION
    osif_ibss_rssi_monitor,
#endif
};
#endif

static wlan_mlme_event_handler_table ap_mlme_evt_handler = {
    osif_create_infra_complete,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    osif_disassoc_indication_ap,
    NULL,
    osif_deauth_indication_ap,
    osif_assoc_indication_ap,
    osif_assoc_indication_ap,
    osif_leave_indication_ap,
    NULL,
    NULL
} ;

static wlan_misc_event_handler_table ap_misc_evt_handler = {
    osif_channel_change,
    osif_country_changed,
    osif_linkspeed,
    osif_michael_failure_indication,
    osif_replay_failure_indication,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,                               /* wlan_ap_stopped */
#if ATH_SUPPORT_WAPI    
    osif_rekey_indication_ap,
#endif
#if ATH_SUPPORT_IBSS_NETLINK_NOTIFICATION
    NULL,
#endif
};

static void osif_vap_setup( wlan_if_t vap, struct net_device *dev, enum ieee80211_opmode opmode)
{
    osif_dev  *osifp = ath_netdev_priv(dev);
    switch((u_int8_t)opmode) {
    case IEEE80211_M_STA:
    case IEEE80211_M_P2P_CLIENT:
        wlan_vap_set_registered_handle(vap,(os_if_t)osifp);
        wlan_vap_register_event_handlers(vap,&common_evt_handler);
        wlan_vap_register_mlme_event_handlers(vap,(os_handle_t)osifp,&sta_mlme_evt_handler);
        wlan_vap_register_misc_event_handlers(vap,(os_handle_t)osifp,&sta_misc_evt_handler);
        osifp->sm_handle = wlan_connection_sm_create(osifp->os_handle,vap);
        if (!osifp->sm_handle) {
            IEEE80211_DPRINTF(vap, IEEE80211_MSG_IOCTL, "%s : sm creation failed\n",
                            __func__);
            return ;
        }
        wlan_connection_sm_register_event_handlers(osifp->sm_handle,(os_if_t)osifp,
                                            osif_sta_sm_evhandler);
        wlan_connection_sm_set_param(osifp->sm_handle,
                                    WLAN_CONNECTION_PARAM_RECONNECT_TIMEOUT, 4*60*1000 /* 4 minutes */);
#ifdef ATH_SUPPORT_P2P
        if (osifp->p2p_client_handle ) {
            wlan_connection_sm_set_param(osifp->sm_handle, WLAN_CONNECTION_PARAM_BGSCAN_MIN_DWELL_TIME, 105);
            wlan_connection_sm_set_param(osifp->sm_handle, WLAN_CONNECTION_PARAM_BGSCAN_MAX_DWELL_TIME, 105);
            wlan_connection_sm_set_param(osifp->sm_handle,
                                    WLAN_CONNECTION_PARAM_RECONNECT_TIMEOUT, 0);
            wlan_connection_sm_set_param(osifp->sm_handle,
                                    WLAN_CONNECTION_PARAM_CONNECT_TIMEOUT, 10);
            wlan_p2p_client_register_event_handlers(osifp->p2p_client_handle, (void *)osifp, osif_p2p_dev_event_handler);
        }
#endif
        break;

#if UMAC_SUPPORT_IBSS
    case IEEE80211_M_IBSS:
        wlan_vap_set_registered_handle(vap,(os_if_t)osifp);
        wlan_vap_register_event_handlers(vap,&common_evt_handler);
        
        osifp->sm_ibss_handle = wlan_ibss_sm_create(osifp->os_handle,vap);
        if (!osifp->sm_ibss_handle) {
            IEEE80211_DPRINTF(vap, IEEE80211_MSG_IOCTL, "%s : sm creation failed\n",
                              __func__);
            return ;
        } 

#if ATH_SUPPORT_IBSS_NETLINK_NOTIFICATION
        /* re-register ibss mlme event handler */
        wlan_vap_register_mlme_event_handlers(vap,(os_handle_t)osifp, &ibss_mlme_evt_handler);
#endif
#if ATH_SUPPORT_IBSS_NETLINK_NOTIFICATION
        wlan_vap_register_misc_event_handlers(vap,(os_handle_t)osifp, &ibss_misc_evt_handler);
#endif

        wlan_ibss_sm_register_event_handlers(osifp->sm_ibss_handle,(os_if_t)osifp,
                                              osif_ibss_sm_evhandler); 
        break;
#endif /* end of #if UMAC_SUPPORT_IBSS */

    case IEEE80211_M_HOSTAP:
    case IEEE80211_M_P2P_GO:
        wlan_vap_set_registered_handle(vap,(os_if_t)osifp);
        wlan_vap_register_event_handlers(vap,&common_evt_handler);
        wlan_vap_register_mlme_event_handlers(vap,(os_handle_t)osifp,&ap_mlme_evt_handler);
        wlan_vap_register_misc_event_handlers(vap,(os_handle_t)osifp,&ap_misc_evt_handler);
#ifdef ATH_SUPPORT_P2P
        if (osifp->p2p_go_handle) {
            wlan_p2p_GO_register_event_handlers(osifp->p2p_go_handle, (void *)osifp, osif_p2p_dev_event_handler);
        }
#endif
        break;
#ifdef ATH_SUPPORT_P2P
    case IEEE80211_M_P2P_DEVICE:
        wlan_vap_set_registered_handle(vap,(os_if_t)osifp);
        wlan_vap_register_event_handlers(vap,&common_evt_handler);
        wlan_vap_register_mlme_event_handlers(vap,(os_handle_t)dev,&ap_mlme_evt_handler);
        wlan_vap_register_misc_event_handlers(vap,(os_handle_t)dev,&ap_misc_evt_handler);
        wlan_p2p_register_event_handlers(osifp->p2p_handle, osifp, osif_p2p_dev_event_handler);
        break;
#endif
    case IEEE80211_M_MONITOR:
        wlan_vap_set_registered_handle(vap,(os_if_t)osifp);
        wlan_vap_register_event_handlers(vap,&common_evt_handler);
        break;
    default:
        break;
    }
}

static void
osif_set_multicast_list(struct net_device *dev)
{
    osif_dev  *osifp = ath_netdev_priv(dev);
    struct net_device *parent = osifp->os_comdev;

    if (dev->flags & IFF_PROMISC)
    {
        parent->flags |= IFF_PROMISC;
    } else {
        parent->flags &= ~IFF_PROMISC;
    }
    if (dev->flags & IFF_ALLMULTI)
    {
        parent->flags |= IFF_ALLMULTI;
    } else {
        parent->flags &= ~IFF_ALLMULTI;
    }

    /* XXX merge multicast list into parent device */
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,30)
    parent->netdev_ops->ndo_set_multicast_list(parent);
#else
    parent->set_multicast_list(parent);
#endif
}


static int
osif_change_mtu(struct net_device *dev, int mtu)
{
    if (!(IEEE80211_MTU_MIN < mtu && mtu <= IEEE80211_MTU_MAX))
        return -EINVAL;
    dev->mtu = mtu;
    /* XXX coordinate with parent device */
    return 0;
}

#if UMAC_SUPPORT_IBSS

#if ATH_SUPPORT_IBSS_ACS
static void osif_ibss_acs_event_handler(void *arg, wlan_chan_t channel)
{
    osif_dev *osifp = (osif_dev *) arg;
    wlan_if_t vap = osifp->os_if;
    int chan = wlan_channel_ieee(channel);
    int error = 0;
 
    printk("%s chan[%d]\n", __func__, chan);
    
    error = wlan_set_desired_ibsschan(vap, chan);

    if (error !=0) {
        IEEE80211_DPRINTF(vap, IEEE80211_MSG_ACS,
                        "%s : failed to set ibss channel with error code %d\n",
                        __func__, error);
        goto done;
    }

    /* Work around soultion to avoid the problem that we might not be able to
       receive frames when going to create the IBSS */
    osif_ibss_scan(osifp);

    /* adhoc create */
    if ( wlan_ibss_sm_start(osifp->sm_ibss_handle, IEEE80211_IBSS_CREATE_NETWORK ) != 0) {
        IEEE80211_DPRINTF(vap, IEEE80211_MSG_IOCTL, "%s : sm start failed\n",
                              __func__);
        //return -EINVAL;
    }
    
done:
    wlan_autoselect_unregister_event_handler(vap, &osif_ibss_acs_event_handler, (void *)osifp);
}
#endif  /* end of #if ATH_SUPPORT_IBSS_ACS */

static int
osif_ibss_init(struct net_device *dev)
{
    osif_dev  *osifp = ath_netdev_priv(dev); 
    wlan_if_t vap = osifp->os_if;    
    int waitcnt,error,connection_attempt = 0;
    
    ieee80211_ssid ssid;

    IEEE80211_DPRINTF(vap, IEEE80211_MSG_IOCTL, "%s \n", __func__);

    if (osifp->is_up) return 0;

    osifp->is_up = 0;

    ssid.len = 0;
    wlan_get_desired_ssidlist(vap, &ssid, 1); 
    if (ssid.len) {
        if (osifp->sm_ibss_handle) {
            printk("%s: wlan_ibss_sm_stop\n", __func__);
            /* stop if it is running */ 
            if (wlan_ibss_sm_stop(osifp->sm_ibss_handle, IEEE80211_IBSS_SM_STOP_ASYNC) == 0) {
                IEEE80211_DPRINTF(vap, IEEE80211_MSG_IOCTL, "%s : wait for connection sm to stop \n",
                                  __func__);
            }
        }
        if (!osifp->is_ibss_create) {   
            /* adhoc join */
            do {
                error =   wlan_ibss_sm_start(osifp->sm_ibss_handle, 0 );
                if (!error) {
                    break;
                }
                /* we are checking error code and resecheduling start as 
                * stop may already be in progress 
                */ 
                if (error == -EINPROGRESS) 
                {
                    waitcnt = 0;
                    while(waitcnt < 2) {
                        schedule_timeout_interruptible(OSIF_CONNECTION_TIMEOUT);
                        waitcnt++;
                    }
                }
                else {
                      IEEE80211_DPRINTF(vap, IEEE80211_MSG_IOCTL, "Error %s : sm start failed\n",
                      __func__);
                      return -EINVAL;
                      }
                } while (connection_attempt++ < OSIF_MAX_CONNECTION_ATTEMPT);
        } else {
#if ATH_SUPPORT_IBSS_ACS
            /* 
             * ACS : ieee80211_ioctl_siwfreq = 0
             *       wlan_set_desired_ibsschan = 255 for IBSS-create ACS start. 
             */
            printk("%s iv_des_ibss_chan[%d]\n", __func__, vap->iv_des_ibss_chan);
            
            if (vap->iv_des_ibss_chan == IEEE80211_CHAN_MAX) {
                /* 
                 * start ACS module to get channel 
                 */
                wlan_autoselect_register_event_handler(vap,
                        &osif_ibss_acs_event_handler, (void *)osifp);
                wlan_autoselect_find_infra_bss_channel(vap);
                return 0;
            }
#endif

            /* Work around soultion to avoid the problem that we might not be able to
               receive frames when going to create the IBSS */
            osif_ibss_scan(osifp);

            /* adhoc create */
            if ( wlan_ibss_sm_start(osifp->sm_ibss_handle, IEEE80211_IBSS_CREATE_NETWORK ) != 0) {
                IEEE80211_DPRINTF(vap, IEEE80211_MSG_IOCTL, "%s : sm start failed\n",
                                      __func__);
                return -EINVAL;
            }
        }

    } /* end of if (ssid.len) */
    
    return 0;
}
#endif /* end of #if UMAC_SUPPORT_IBSS */

int
osif_vap_init(struct net_device *dev, int forcescan)
{
#define IS_RUNNING(_dev)                                            \
    ((_dev->flags & (IFF_RUNNING|IFF_UP)) == (IFF_RUNNING|IFF_UP))
    osif_dev  *osifp = ath_netdev_priv(dev);
    wlan_if_t vap = osifp->os_if;
    struct net_device *parent = osifp->os_comdev;
    enum ieee80211_opmode opmode;
    wlan_dev_t comhandle = wlan_vap_get_devhandle(vap);
    int waitcnt;
    int error = 0;
    wlan_chan_t chan;
    int connection_attempt = 0;

    int return_val=0;
    IEEE80211_DPRINTF(vap,
                      IEEE80211_MSG_STATE | IEEE80211_MSG_DEBUG,
                      "%s, ifname=%s, opmode=%d\n", "start running", osifp->netdev->name, vap->iv_opmode);


    if ((dev->flags & IFF_RUNNING) == 0)
    {
        if (osif_get_num_active_vaps(comhandle) == 0 &&
            (parent->flags & IFF_RUNNING) == 0){
            return_val=dev_open(parent);
            if (return_val!=0)
                return return_val;
        }
        /*
         * Mark us running.  Note that we do this after
         * opening the parent device to avoid recursion.
         */
        dev->flags |= IFF_RUNNING;      /* mark us running */
    }

    if ((u_int8_t)osifp->os_opmode == IEEE80211_M_P2P_DEVICE) {
        osifp->is_vap_pending = 0;
        osifp->is_up = 1;
        printk(" %s: called...for P2P_DEVICE, ifname=%s\n", __func__, osifp->netdev->name);
        return 0;
    }

#if ATH_SUPPORT_WAPI
    if (osifp->os_opmode == IEEE80211_M_HOSTAP)
        osif_wapi_rekeytimer_start((os_if_t)osifp); 
#endif

    /*
     * initialize scan variable
     */
    osifp->os_last_siwscan = OS_GET_TICKS();

    /*
     * If the parent is up and running, then kick the
     * 802.11 state machine as appropriate.
     * XXX parent should always be up+running
     */
    if (IS_RUNNING(parent) && ((osifp->os_opmode == IEEE80211_M_HOSTAP) 
                               || (osifp->os_if->iv_ic->ic_roaming != IEEE80211_ROAMING_MANUAL) 
                               || (osifp->os_if->iv_mlmeconnect))) {
        osifp->os_if->iv_mlmeconnect=0;
        opmode = wlan_vap_get_opmode(vap);
        if (opmode == IEEE80211_M_STA ||
            (u_int8_t)osifp->os_opmode == IEEE80211_M_P2P_CLIENT) {
            ieee80211_ssid ssid;
            int desired_bssid = 0;
            ssid.len=0;
            wlan_get_desired_ssidlist(vap,&ssid,1);
            desired_bssid = wlan_aplist_get_desired_bssid_count(vap);
            /* check if there is only one desired bssid and it is broadcast , ignore the setting if it is */
            if (desired_bssid == 1) {
                u_int8_t des_bssid[IEEE80211_ADDR_LEN];
                wlan_aplist_get_desired_bssidlist(vap, &des_bssid);
                if (IEEE80211_IS_BROADCAST(des_bssid)) {
                    desired_bssid=0;
                }
            }
            if ((desired_bssid || ssid.len) && osifp->sm_handle) {
                IEEE80211_DPRINTF(vap, IEEE80211_MSG_IOCTL, "%s: desired bssid %d ssid len %d \n",__func__,
                              desired_bssid, ssid.len);
                if (forcescan) {
                    osifp->is_restart = 1;
                }
                else {
                    if (wlan_get_param(vap, IEEE80211_FEATURE_WDS)) {
                        /* Disable bgscan  for WDS STATION */
                        wlan_connection_sm_set_param(osifp->sm_handle,
                                                     WLAN_CONNECTION_PARAM_BGSCAN_POLICY,
                                                     WLAN_CONNECTION_BGSCAN_POLICY_NONE);
                        /* Set the connect timeout as infinite for WDS STA*/
                        wlan_connection_sm_set_param(osifp->sm_handle,
                                                     WLAN_CONNECTION_PARAM_CONNECT_TIMEOUT,10);
                        /* Set the connect timeout as infinite for WDS STA*/
                        wlan_connection_sm_set_param(osifp->sm_handle,
                                                     WLAN_CONNECTION_PARAM_RECONNECT_TIMEOUT,10);
                        /*
                         * Bad AP Timeout value should be specified in ms
                         * so converting seconds to ms
                         */
                        /* Set BAD AP timeout to 0 for linux repeater config
                         * as AUTH_AUTO mode in WEP needs connection SM to alternate AUTH mode
                         * from open to shared continously for reconnection
                         */  
                        wlan_connection_sm_set_param(osifp->sm_handle,
                                                     WLAN_CONNECTION_PARAM_BAD_AP_TIMEOUT, 0);
                        wlan_aplist_set_bad_ap_timeout(vap, 0);
                       
                        /* Bring down other VAPs if already running, as station is going to SCAN state*/
                        if ((opmode == IEEE80211_M_STA)  && !wlan_get_param(vap, IEEE80211_FEATURE_VAP_IND)) {
                            wlan_iterate_vap_list(comhandle, osif_bringdown_vap_iter_func, NULL);
                        }
                    }
                }
                /* stop if it is running */
                if (wlan_connection_sm_stop(osifp->sm_handle, IEEE80211_CONNECTION_SM_STOP_ASYNC ) == 0) {
                    waitcnt = 0;
                    schedule_timeout_interruptible(OSIF_DISCONNECT_TIMEOUT); /* atleast wait for one iteration */
                    while(osifp->is_up && waitcnt < OSIF_MAX_CONNECTION_STOP_TIMEOUT) {
                        schedule_timeout_interruptible(OSIF_DISCONNECT_TIMEOUT);
                        waitcnt++;
                    }
                    if (osifp->is_up) {
                        IEEE80211_DPRINTF(vap, IEEE80211_MSG_IOCTL,
                                          "%s : wait for connection SM stop failed \n",
                                          __func__);
                    } else {
                        IEEE80211_DPRINTF(vap, IEEE80211_MSG_IOCTL,
                                          "%s : wakeup from wait for connection SM to stop \n",
                                          __func__);
                    }
                }
                if (osifp->authmode == IEEE80211_AUTH_AUTO) {
                    /* If the auth mode is set to AUTO, set the auth mode
                     * to SHARED for first connection try */
                    ieee80211_auth_mode modes[1];
                    u_int nmodes=1;
                    modes[0] = IEEE80211_AUTH_SHARED;
                    wlan_set_authmodes(vap,modes,nmodes);
                }
                do {
                    error = wlan_connection_sm_start(osifp->sm_handle);
                    if (!error) {
                        break;
                    }
                    if (error == -EINPROGRESS) {
                        /* wait for 2 secs for connection to be stopped completely */
                        waitcnt = 0;
                        while(waitcnt < 2) {
                            schedule_timeout_interruptible(OSIF_CONNECTION_TIMEOUT);
                            waitcnt++;
                        }
                    }
                    else {
                        IEEE80211_DPRINTF(vap, IEEE80211_MSG_IOCTL, "Error %s : sm start failed\n",
                                          __func__);
                        return -EINVAL;
                    }
                } while (connection_attempt++ < OSIF_MAX_CONNECTION_ATTEMPT);
                if (error) {
                    IEEE80211_DPRINTF(vap, IEEE80211_MSG_IOCTL, "Error %s : all sm start attempt failed\n",
                                      __func__);
                    return -EINVAL;
                }

                /* Wait for the connection up */
                waitcnt = 0;
                while(!osifp->is_up && waitcnt < 3) {
                    schedule_timeout_interruptible(OSIF_CONNECTION_TIMEOUT) ;
                    waitcnt++;
                }

                if (!osifp->is_up) {
                    IEEE80211_DPRINTF(vap, IEEE80211_MSG_IOCTL, "%s : wait for connection SM start failed \n",
                                      __func__);
                }
            }
            else {
                IEEE80211_DPRINTF(vap, IEEE80211_MSG_STATE,
                                  "Did not start connection SM : num desired_bssid %d ssid len %d \n",
                                  desired_bssid, ssid.len);
            }
            /*
             * TBD: Try to be intelligent about clocking the state
             * machine.  If we're currently in RUN state then
             * we should be able to apply any new state/parameters
             * simply by re-associating.  Otherwise we need to
             * re-scan to select an appropriate ap.
             */
#if UMAC_SUPPORT_IBSS
        } else if (opmode == IEEE80211_M_IBSS) {
            osif_ibss_init(dev);
#endif
        } else if (opmode == IEEE80211_M_MONITOR) {
            wlan_mlme_start_monitor(vap);
        } else {
            enum ieee80211_phymode des_mode = wlan_get_desired_phymode(vap);
            ieee80211_ssid ssid;

            ssid.len=0;
            wlan_get_desired_ssidlist(vap,&ssid,1);
            if (ssid.len == 0) {
                /* do not start AP if no ssid is set */
                return 0;
            }
            /*
             * TBD: Assuming AP mode. No monitor mode support yet
             */
            wlan_mlme_stop_bss(vap, 0);
            OS_DELAY(1000);

            chan = wlan_get_current_channel(vap, false);
            if ((!chan) || (chan == IEEE80211_CHAN_ANYC)) {
                /* start ACS module to get channel */
                wlan_autoselect_register_event_handler(vap,
                        &osif_acs_event_handler, (void *)osifp);
                wlan_autoselect_find_infra_bss_channel(vap);
                return 0;
            }

            if (((des_mode == IEEE80211_MODE_11NG_HT40PLUS) ||
                (des_mode == IEEE80211_MODE_11NG_HT40MINUS) ||
                (des_mode == IEEE80211_MODE_11NG_HT40)) &&
                (wlan_coext_enabled(vap) &&
                (osif_get_num_running_vaps(comhandle) == 0))) {
                wlan_autoselect_register_event_handler(vap,
                                                       &osif_ht40_event_handler,
                                                       (void *)osifp);
                wlan_attempt_ht40_bss(vap);
                return 0;
            }

            error = wlan_mlme_start_bss(vap);
            if (error != 0) {
                IEEE80211_DPRINTF(vap, IEEE80211_MSG_IOCTL, "%s :mlme returned error %d \n", __func__, error);
                if (error == EAGAIN) {
                    /* Radio resource is busy on scanning, try later */
                    IEEE80211_DPRINTF(vap, IEEE80211_MSG_IOCTL, "%s :mlme busy mostly scanning \n", __func__);
                    osifp->is_vap_pending = 1;
                    spin_lock(&osifp->tx_lock);
                    osifp->is_up = 0;
                    spin_unlock(&osifp->tx_lock);
                    return 0;
                } else if (error == EBUSY) {
                    /* resource manager is asynchronously bringing up the vap */
                    /* Wait for the connection up */
                    waitcnt = 0;
                    while(!osifp->is_up && waitcnt < 3) {
                        schedule_timeout_interruptible(HZ) ;
                        waitcnt++;
                    }

                    if (!osifp->is_up) {
                        IEEE80211_DPRINTF(vap, IEEE80211_MSG_IOCTL, "%s : timed out waitinfor AP to come up \n", __func__);
                        return error;
                    }
                    error=0;
                }
                else {
                    IEEE80211_DPRINTF(vap, IEEE80211_MSG_IOCTL, "%s : failed with error code %d\n",
                                      __func__, error);
                    return error;
                }
            } else {
                wlan_mlme_connection_up(vap);
                IEEE80211_DPRINTF(vap, IEEE80211_MSG_IOCTL, "%s :vap up \n", __func__);
            }
            osifp->is_up = 1;
        }
    }
    return 0;
#undef IS_RUNNING
}

int
osif_vap_open(struct net_device *dev)
{
    osif_dev  *osifp = ath_netdev_priv(dev); 
#ifdef ATH_BUS_PM
    struct net_device *comdev = osifp->os_comdev;
    struct ath_softc_net80211 *scn = ath_netdev_priv(comdev);
#endif

#ifdef ATH_BUS_PM
    if (scn->sc_osdev->isDeviceAsleep)
	return -EPERM;
#endif /* ATH_BUS_PM */

    osifp->is_vap_pending = 1;

    return osif_vap_init(dev, 0);
}

/*
* Return netdevice statistics.
*/
static struct net_device_stats *
    osif_getstats(struct net_device *dev)
{
    osif_dev  *osifp = ath_netdev_priv(dev);
    wlan_if_t vap = osifp->os_if;
    struct net_device_stats *stats = &osifp->os_devstats;
    struct ieee80211_stats *vapstats;
    const struct ieee80211_mac_stats *unimacstats ;
    const struct ieee80211_mac_stats *multimacstats;

    if (!vap) {
	return stats;
    }

    vapstats = wlan_get_stats(vap);
    unimacstats = wlan_mac_stats(vap, 0);
    multimacstats = wlan_mac_stats(vap, 1);
    stats->rx_packets = (unsigned long)(unimacstats->ims_rx_packets + multimacstats->ims_rx_packets);
    stats->tx_packets = (unsigned long)(unimacstats->ims_tx_packets + multimacstats->ims_tx_packets);

    /* XXX total guess as to what to count where */
    /* update according to private statistics */

    stats->tx_bytes = (unsigned long)(unimacstats->ims_tx_bytes+ multimacstats->ims_tx_bytes);

    stats->tx_errors = vapstats->is_tx_nodefkey
        + vapstats->is_tx_noheadroom
        + vapstats->is_crypto_enmicfail
        ;
    stats->tx_dropped = vapstats->is_tx_nobuf
        + vapstats->is_tx_nonode
        + vapstats->is_tx_unknownmgt
        + vapstats->is_tx_badcipher
        + vapstats->is_tx_nodefkey
        ;

    stats->rx_bytes = (unsigned long)(unimacstats->ims_rx_bytes+ multimacstats->ims_rx_bytes);
	
	stats->rx_errors = vapstats->is_rx_tooshort
#ifdef notyet /* TBD: to be added to umac */
			+ vapstats->is_rx_wepfail
#endif
			+ vapstats->is_rx_decap
			+ vapstats->is_rx_nobuf
#ifdef notyet /* TBD: to be added to umac */
			+ vapstats->is_rx_decryptcrc
			+ vapstats->is_rx_ccmpmic
			+ vapstats->is_rx_tkipmic
			+ vapstats->is_rx_tkipicv
#endif
			;
	stats->rx_crc_errors = 0;
    stats->rx_dropped = stats->rx_errors;



    return stats;
}


static void osif_com_vap_event_handler(void *event_arg, wlan_dev_t devhandle, os_if_t osif, ieee80211_dev_vap_event event)
{
    osif_dev  *osifp = (osif_dev *) osif;

    switch(event) {
    case IEEE80211_VAP_CREATED:
        break;
    case IEEE80211_VAP_DELETED:
        osifp->is_deleted=1;
        break;
    default:
        break;
    }

}

static wlan_dev_event_handler_table com_evtable = {
    osif_com_vap_event_handler
};
void osif_attach(struct net_device *comdev)
{
    wlan_dev_t devhandle = ath_netdev_priv(comdev);
    wlan_device_register_event_handlers(devhandle,comdev,&com_evtable);
#ifdef __CARRIER_PLATFORM__
    wlan_pltfrm_attach(comdev);
#endif /*__CARRIER_PLATFORM__*/
}

void osif_detach(struct net_device *comdev)
{
    wlan_dev_t devhandle = ath_netdev_priv(comdev);
    wlan_device_unregister_event_handlers(devhandle,(void *)comdev,&com_evtable);
#ifdef __CARRIER_PLATFORM__
    wlan_pltfrm_detach(comdev);
#endif
}
    
int
osif_vap_hardstart_generic(struct sk_buff *skb, struct net_device *dev)
{
    osif_dev  *osdev = ath_netdev_priv(dev);
    wlan_if_t vap = osdev->os_if;
    struct net_device *comdev = osdev->os_comdev;
    //struct ieee80211_cb *cb;
    struct ether_header *eh = (struct ether_header *)skb->data;
    int send_err=0;

    spin_lock(&osdev->tx_lock);
    if (!osdev->is_up) {
        goto bad;
    }

    /* NB: parent must be up and running */
    if ((comdev->flags & (IFF_RUNNING|IFF_UP)) != (IFF_RUNNING|IFF_UP))
        goto bad;

#ifdef notyet
    /* if the vap is configured in 3 address forwarding
    * mode, then clone the MAC address if necessary
    */
    if ((vap->iv_opmode == IEEE80211_M_STA) &&
        (vap->iv_flags_ext & IEEE80211_C_STA_FORWARD))
    {
        if (IEEE80211_ADDR_EQ(eh->ether_shost, dev->dev_addr))
        {
            /* Station is in forwarding mode - Drop the packets
            * originated from station except EAPOL packets
            */
            if (eh->ether_type == ETHERTYPE_PAE)
                IEEE80211_ADDR_COPY(eh->ether_shost,vap->iv_myaddr);
            else {
                /* Drop the packet */
        goto bad;
            }
        }
        if (!IEEE80211_ADDR_EQ(eh->ether_shost, vap->iv_myaddr))
        {
            /* Clone the mac address with the mac address end-device */
            IEEE80211_ADDR_COPY(vap->iv_myaddr, eh->ether_shost);
            IEEE80211_ADDR_COPY(ic->ic_myaddr, eh->ether_shost);
            ic->ic_clone_macaddr(ic, eh->ether_shost);
            /* Mac address is cloned - re-associate with the AP
            * again
            */
            ieee80211_new_state(vap, IEEE80211_S_SCAN, -1);
            goto bad;
        }
    }
#endif


#ifdef ATH_SUPPORT_HTC
    /*In some STA platforms like PB44-small-xx, skb_unshare will introduce
          extra copies if there is no need to reallocate headroom. By skipping the 
          skb_unshare, we can reduce 3~5% CPU utilization on pb44-usb(300MHz)
          for Newma. However, we already verified this won't happen in every platform.
          It depends on how the upper layer allocate skb_headroom. In the repeater mode
          we should enable check for multicast packets, to avoid modifing shared packets*/
    if((vap->iv_opmode != IEEE80211_M_STA) ||
       IEEE80211_IS_MULTICAST(eh->ether_dhost)) 
    {
        skb = skb_unshare(skb, GFP_ATOMIC);
        if (skb == NULL) {
            goto bad;
        }
    }

#else  
    skb = skb_unshare(skb, GFP_ATOMIC);
    if (skb == NULL) {
        goto bad;
    }
#endif /* ATH_SUPPORT_HTC */

#ifdef ATH_EXT_AP
    if ((vap->iv_opmode == IEEE80211_M_STA) &&
        IEEE80211_VAP_IS_EXT_AP_ENABLED(vap)) {
        eh = (struct ether_header *)skb->data;
        if (ieee80211_extap_output(vap, eh)) {
            goto bad;
        }
    } else {
#ifdef EXTAP_DEBUG
        extern char *arps[];
        eth_arphdr_t *arp = (eth_arphdr_t *)(eh + 1);
        if(eh->ether_type == ETHERTYPE_ARP) {
            printk("\tOuT %s\t" eaistr "\t" eamstr "\t" eaistr "\t" eamstr "\n"
                    "\ts: " eamstr "\td: " eamstr "\n",
                arps[arp->ar_op],
                eaip(arp->ar_sip), eamac(arp->ar_sha),
                eaip(arp->ar_tip), eamac(arp->ar_tha),
                eamac(eh->ether_shost), eamac(eh->ether_dhost));
        }
#endif
    }
#endif /* ATH_EXT_AP */

    if (skb_headroom(skb) < dev->hard_header_len) {
        struct sk_buff *tmp = skb;

#ifdef ATH_SUPPORT_HTC
        if((vap->iv_opmode == IEEE80211_M_STA) &&
           (!IEEE80211_IS_MULTICAST(eh->ether_dhost))) {
            skb = skb_unshare(skb, GFP_ATOMIC);
            if (skb == NULL) {
                goto bad;
            }
            tmp = skb;
        }
#endif /* ATH_SUPPORT_HTC */
        
        skb = skb_realloc_headroom(tmp, dev->hard_header_len);
        dev_kfree_skb(tmp);

        if (skb == NULL) {
            IEEE80211_DPRINTF(vap, IEEE80211_MSG_OUTPUT,
                "%s: cannot expand skb\n", __func__);
            goto bad;
        }
    }

    /*
    * Find the node for the destination so we can do
    * things like power save.
    */
    eh = (struct ether_header *)skb->data;

#if ADF_SUPPORT
    N_FLAG_KEEP_ONLY(skb, N_PWR_SAV);
#else
    M_FLAG_KEEP_ONLY(skb, M_PWR_SAV);
#endif

#ifdef ATH_SUPPORT_HTC
    /*
     * In power save mode, tx pkt will send wmi command to wake up.
     * Tasklet can't schedule while waiting wmi response to release mutex. 
     * Defer to tx thread when power save is on.
     */
    if ((wlan_get_powersave(vap) != IEEE80211_PWRSAVE_NONE) ||
        (vap->iv_txrx_event_info.iv_txrx_event_filter & IEEE80211_VAP_OUTPUT_EVENT_DATA)) {
        if (ath_put_txbuf(skb))
            goto bad;
    }
    else
#endif
    send_err = wlan_vap_send(vap, (wbuf_t)skb);

#if ATH_SUPPORT_FLOWMAC_MODULE
    if (send_err == -ENOBUFS && vap->iv_flowmac) {
        /* pause the Ethernet and the queues as well */
        if (!((struct ieee80211vap*)vap)->iv_dev_stopped) {
            if (((struct ieee80211vap*)vap)->iv_evtable->wlan_pause_queue) {
                ((struct ieee80211vap*)vap)->iv_evtable->wlan_pause_queue(
                                               vap->iv_ifp, 1, vap->iv_flowmac);
                ((struct ieee80211vap*)vap)->iv_dev_stopped = 1;
            }
        }
    }
#endif
    spin_unlock(&osdev->tx_lock);

    return 0;

bad:
    spin_unlock(&osdev->tx_lock);
    if (skb != NULL)
        dev_kfree_skb(skb);
    return 0;
}
    

int
osif_vap_hardstart(struct sk_buff *skb, struct net_device *dev)
{
   do_osif_vap_hardstart(skb,dev);
}
/*
* Convert a media specification to an 802.11 phy mode.
*/
static int
media2mode(const struct ifmedia_entry *ime, enum ieee80211_phymode *mode)
{

    switch (IFM_MODE(ime->ifm_media)) {
    case IFM_IEEE80211_11A:
        *mode = IEEE80211_MODE_11A;
        break;
    case IFM_IEEE80211_11B:
        *mode = IEEE80211_MODE_11B;
        break;
    case IFM_IEEE80211_11G:
        *mode = IEEE80211_MODE_11G;
        break;
    case IFM_IEEE80211_FH:
        *mode = IEEE80211_MODE_FH;
        break;
    case IFM_IEEE80211_11NA:
            *mode = IEEE80211_MODE_11NA_HT20;
            break;
    case IFM_IEEE80211_11NG:
            *mode = IEEE80211_MODE_11NG_HT20;
            break;
    case IFM_AUTO:
        *mode = IEEE80211_MODE_AUTO;
        break;
    default:
        return 0;
    }

    if (ime->ifm_media & IFM_IEEE80211_HT40PLUS) {
            if (*mode == IEEE80211_MODE_11NA_HT20)
                    *mode = IEEE80211_MODE_11NA_HT40PLUS;
            else if (*mode == IEEE80211_MODE_11NG_HT20)
                    *mode = IEEE80211_MODE_11NG_HT40PLUS;
            else
                    return 0;
    }

    if (ime->ifm_media & IFM_IEEE80211_HT40MINUS) {
            if (*mode == IEEE80211_MODE_11NA_HT20)
                    *mode = IEEE80211_MODE_11NA_HT40MINUS;
            else if (*mode == IEEE80211_MODE_11NG_HT20)
                    *mode = IEEE80211_MODE_11NG_HT40MINUS;
            else
                    return 0;
    }

    return 1;
}

/*
 * convert IEEE80211 ratecode to ifmedia subtype.
 */
static int
rate2media(struct ieee80211vap *vap,int rate,enum ieee80211_phymode mode)
{
#define N(a)    (sizeof(a) / sizeof(a[0]))
    struct ieee80211com    *ic = wlan_vap_get_devhandle(vap);
    static const struct
    {
        u_int   m;      /* rate + mode */
        u_int   r;      /* if_media rate */
    } rates[] = {
                {   0x1b | IFM_IEEE80211_FH, IFM_IEEE80211_FH1 },
                {   0x1a | IFM_IEEE80211_FH, IFM_IEEE80211_FH2 },
                {   0x1b | IFM_IEEE80211_11B, IFM_IEEE80211_DS1 },
                {   0x1a | IFM_IEEE80211_11B, IFM_IEEE80211_DS2 },
                {   0x19 | IFM_IEEE80211_11B, IFM_IEEE80211_DS5 },
                {   0x18 | IFM_IEEE80211_11B, IFM_IEEE80211_DS11 },
                {   0x1a | IFM_IEEE80211_11A, IFM_IEEE80211_OFDM2_25 },
                {   0x0b | IFM_IEEE80211_11A, IFM_IEEE80211_OFDM6 },
                {   0x0f | IFM_IEEE80211_11A, IFM_IEEE80211_OFDM9 },
                {   0x0a | IFM_IEEE80211_11A, IFM_IEEE80211_OFDM12 },
                {   0x0e | IFM_IEEE80211_11A, IFM_IEEE80211_OFDM18 },
                {   0x09 | IFM_IEEE80211_11A, IFM_IEEE80211_OFDM24 },
                {   0x0d | IFM_IEEE80211_11A, IFM_IEEE80211_OFDM36 },
                {   0x08 | IFM_IEEE80211_11A, IFM_IEEE80211_OFDM48 },
                {   0x0c | IFM_IEEE80211_11A, IFM_IEEE80211_OFDM54 },
                {   0x1b | IFM_IEEE80211_11G, IFM_IEEE80211_DS1 },
                {   0x1a | IFM_IEEE80211_11G, IFM_IEEE80211_DS2 },
                {   0x19 | IFM_IEEE80211_11G, IFM_IEEE80211_DS5 },
                {   0x18 | IFM_IEEE80211_11G, IFM_IEEE80211_DS11 },
                {   0x0b | IFM_IEEE80211_11G, IFM_IEEE80211_OFDM6 },
                {   0x0f | IFM_IEEE80211_11G, IFM_IEEE80211_OFDM9 },
                {   0x0a | IFM_IEEE80211_11G, IFM_IEEE80211_OFDM12 },
                {   0x0e | IFM_IEEE80211_11G, IFM_IEEE80211_OFDM18 },
                {   0x09 | IFM_IEEE80211_11G, IFM_IEEE80211_OFDM24 },
                {   0x0d | IFM_IEEE80211_11G, IFM_IEEE80211_OFDM36 },
                {   0x08 | IFM_IEEE80211_11G, IFM_IEEE80211_OFDM48 },
                {   0x0c | IFM_IEEE80211_11G, IFM_IEEE80211_OFDM54 },
                /* NB: OFDM72 doesn't realy exist so we don't handle it */
        };
    u_int mask, i, phytype;

    mask = rate & IEEE80211_RATE_VAL;
    switch (mode)
    {
    case IEEE80211_MODE_11A:
    case IEEE80211_MODE_TURBO_A:
    case IEEE80211_MODE_11NA_HT20:
    case IEEE80211_MODE_11NA_HT40MINUS:
    case IEEE80211_MODE_11NA_HT40PLUS:
    case IEEE80211_MODE_11NA_HT40:
        mask |= IFM_IEEE80211_11A;
        break;
    case IEEE80211_MODE_11B:
        mask |= IFM_IEEE80211_11B;
        break;
    case IEEE80211_MODE_FH:
        mask |= IFM_IEEE80211_FH;
        break;
    case IEEE80211_MODE_AUTO:
        phytype = wlan_get_current_phytype(ic);

        if (phytype == IEEE80211_T_FH)
        {
            mask |= IFM_IEEE80211_FH;
            break;
        }
        /* NB: hack, 11g matches both 11b+11a rates */
        /* fall thru... */
    case IEEE80211_MODE_11G:
    case IEEE80211_MODE_TURBO_G:
    case IEEE80211_MODE_11NG_HT20:
    case IEEE80211_MODE_11NG_HT40MINUS:
    case IEEE80211_MODE_11NG_HT40PLUS:
    case IEEE80211_MODE_11NG_HT40:
        mask |= IFM_IEEE80211_11G;
        break;
    }
    for (i = 0; i < N(rates); i++) {
        if (rates[i].m == mask) {
            return rates[i].r;
        }
    }
    return IFM_AUTO;
#undef N
}


static int
osif_media_change(struct net_device *dev)
{
    osif_dev  *osifp = ath_netdev_priv(dev);
    wlan_if_t vap = osifp->os_if;
    enum ieee80211_phymode newphymode;
    struct ifmedia_entry *ime = osifp->os_media.ifm_cur;

    /*
    * First, identify the phy mode.
    */
    if (!media2mode(ime, &newphymode))
        return EINVAL;
    if (newphymode != wlan_get_desired_phymode(vap)) {
        return wlan_set_desired_phymode(vap,newphymode);
    }
    return 0;
}

static const u_int mopts[] = {
        IFM_AUTO,
        IFM_IEEE80211_11A,
        IFM_IEEE80211_11B,
        IFM_IEEE80211_11G,
        IFM_IEEE80211_FH,
        IFM_IEEE80211_11A | IFM_IEEE80211_TURBO,
        IFM_IEEE80211_11G | IFM_IEEE80211_TURBO,
        IFM_IEEE80211_11NA,
        IFM_IEEE80211_11NG,
        IFM_IEEE80211_11NA | IFM_IEEE80211_HT40PLUS,
        IFM_IEEE80211_11NA | IFM_IEEE80211_HT40MINUS,
        IFM_IEEE80211_11NG | IFM_IEEE80211_HT40PLUS,
        IFM_IEEE80211_11NG | IFM_IEEE80211_HT40MINUS,
};

/*
* Common code to calculate the media status word
* from the operating mode and channel state.
*/
static int
media_status(enum ieee80211_opmode opmode, wlan_chan_t chan)
{
    int status;
    enum ieee80211_phymode mode = IEEE80211_MODE_AUTO;    /* autoselect */

    status = IFM_IEEE80211;
    switch ((u_int8_t)opmode) {
    case IEEE80211_M_STA:
        break;
    case IEEE80211_M_IBSS:
        status |= IFM_IEEE80211_ADHOC;
        break;
    case IEEE80211_M_HOSTAP:
    case IEEE80211_M_P2P_GO:
        status |= IFM_IEEE80211_HOSTAP;
        break;
    case IEEE80211_M_MONITOR:
        status |= IFM_IEEE80211_MONITOR;
        break;
    case IEEE80211_M_AHDEMO:
    case IEEE80211_M_WDS:
        /* should not come here */
        break;
    default:
        break;
    }
    if (chan) {
        mode = wlan_channel_phymode(chan);
    }
    status |= mopts[mode];

    /* XXX else complain? */

    return status;
}

static void
osif_media_status(struct net_device *dev, struct ifmediareq *imr)
{

    int rate;
    osif_dev  *osdev = ath_netdev_priv(dev);
    wlan_if_t vap = osdev->os_if;
    wlan_chan_t chan;
    enum ieee80211_phymode mode;

    imr->ifm_status = IFM_AVALID;
    if ((dev->flags & IFF_UP))
        imr->ifm_status |= IFM_ACTIVE;
    chan = wlan_get_current_channel(vap,true);
    if (chan != (wlan_chan_t)0 && chan != (wlan_chan_t)-1) {
        imr->ifm_active = media_status(osdev->os_opmode, chan);
    }

    if(wlan_is_connected(vap)) {
         mode = wlan_get_bss_phymode(vap);
    }
    else {
         mode = IEEE80211_MODE_AUTO;
    }

    rate = wlan_get_param(vap, IEEE80211_FIXED_RATE);
    if (rate != IEEE80211_FIXED_RATE_NONE)
    {
        /*
        * A fixed rate is set, report that.
        */
        imr->ifm_active &= ~IFM_TMASK;
        if ( rate & 0x80) {
            imr->ifm_active |= IFM_IEEE80211_HT_MCS;
        }
        else {
            imr->ifm_active |= rate2media(vap,rate,mode);
        }
    }
    else {
        imr->ifm_active |= IFM_AUTO;
    }
    imr->ifm_current = imr->ifm_active;
}


static int os_if_media_init(osif_dev *osdev)
{
    wlan_if_t vap;
    int i,mopt;
    u_int16_t nmodes;
    enum ieee80211_phymode modes[IEEE80211_MODE_MAX];
    struct ifmedia *media = &osdev->os_media;
    struct ifmediareq imr;
    
    vap = osdev->os_if;
    ifmedia_init(media, 0, osif_media_change, osif_media_status);
    if (wlan_get_supported_phymodes(osdev->os_devhandle,modes,
                                    &nmodes,IEEE80211_MODE_MAX) != 0 ) {
        IEEE80211_DPRINTF(vap, IEEE80211_MSG_IOCTL, "%s : get_supported_phymodes failed \n", __func__);
        return -EINVAL;
    }
    for (i=0;i<nmodes;++i) {
        mopt = mopts[(u_int32_t)modes[i]];
        IEEE80211_ADD_MEDIA(media, IFM_AUTO, mopt); /* e.g. 11a auto */
        IEEE80211_ADD_MEDIA(media, IFM_AUTO, mopt | IFM_IEEE80211_ADHOC);
        IEEE80211_ADD_MEDIA(media, IFM_AUTO, mopt | IFM_IEEE80211_HOSTAP);
        IEEE80211_ADD_MEDIA(media, IFM_AUTO, mopt | IFM_IEEE80211_ADHOC | IFM_FLAG0);
        IEEE80211_ADD_MEDIA(media, IFM_AUTO, mopt | IFM_IEEE80211_MONITOR);
    }

    osif_media_status(osdev->netdev, &imr);
    ifmedia_set(media, imr.ifm_active);
    return 0;
}

int osif_vap_stop(struct net_device *dev)
{
    osif_dev *osifp = ath_netdev_priv(dev);
    wlan_if_t vap = osifp->os_if;
    int waitcnt;
    int error = 0;
    u_int32_t is_up = osifp->is_up;

    enum ieee80211_opmode opmode;
    wlan_dev_t  comhandle;
    u_int32_t numvaps;
    struct net_device *comdev;
    comdev = osifp->os_comdev;

    if (!vap) {
       return 0;	
    }

    if (wlan_scan_in_progress(vap)) {
        printk(" Scan in progress.. Cancelling it \n");
        wlan_scan_cancel(vap, osifp->scan_requestor, IEEE80211_ALL_SCANS, true);
        OS_DELAY (1000);
    }

    opmode = wlan_vap_get_opmode(vap);

    switch((u_int8_t)opmode) {
    case IEEE80211_M_STA:
    case IEEE80211_M_P2P_CLIENT:
        IEEE80211_DPRINTF(vap, IEEE80211_MSG_IOCTL, "%s : stopping %s vap \n",__func__, opmode==IEEE80211_M_STA?"STA":"P2P client");
        if (!osifp->sm_handle) {
            osif_vap_down(dev);
        } else {
            if (!vap->auto_assoc)
            {
                ieee80211_ssid   tmpssid;
                u_int8_t des_bssid[IEEE80211_ADDR_LEN];

                /* reset desired ssid */
                tmpssid.ssid[0] = '\0';
                tmpssid.len = 0;
                wlan_set_desired_ssidlist(vap,1,&tmpssid);

                /* To reset desired bssid after vap stop. 
                   Otherwise, after interface down and up, it will try to unexpectedly connect to the previous AP. */
                memset(des_bssid, 0xff, IEEE80211_ADDR_LEN);
                wlan_aplist_set_desired_bssidlist(vap, 1, &des_bssid);  
            }
            /* vap down will be called asynchronously */
            if (wlan_connection_sm_stop(osifp->sm_handle, IEEE80211_CONNECTION_SM_STOP_ASYNC ) == 0) {
                waitcnt = 0;
                while((is_up ? osifp->is_up : 1) && waitcnt < 3) {
                    schedule_timeout_interruptible(OSIF_CONNECTION_TIMEOUT);
                    waitcnt++;
                }
                if (osifp->is_up) {
                    IEEE80211_DPRINTF(vap, IEEE80211_MSG_IOCTL, "%s : connection stop failed \n",
                                    __func__);
                    error = -EBUSY;
                }
            }
            /* if vap is not down force it down */
            if (osifp->is_up) {
                osif_vap_down(dev);
            }
        }
        break;
#if UMAC_SUPPORT_IBSS
    case IEEE80211_M_IBSS:
        if (!osifp->sm_ibss_handle) {
            osif_vap_down(dev); 
        } else {
            /* vap down will be called asynchronously */
            IEEE80211_DPRINTF(vap, IEEE80211_MSG_IOCTL, "%s : stopping IBSS vap \n",
                                      __func__);
            if (wlan_ibss_sm_stop(osifp->sm_ibss_handle, IEEE80211_IBSS_SM_STOP_ASYNC) == 0) {

                    IEEE80211_DPRINTF(vap, IEEE80211_MSG_IOCTL, "%s : IBSS stop failed \n",
                                      __func__);

            } else {
               osif_vap_down(dev); 
            }
        }
        break;
#endif
    case IEEE80211_M_HOSTAP:
    case IEEE80211_M_P2P_GO:
        osifp->is_vap_pending=0;
        IEEE80211_DPRINTF(vap, IEEE80211_MSG_IOCTL, "%s : stopping %s vap \n",__func__, opmode==IEEE80211_M_HOSTAP?"AP":"P2P GO");
        osif_vap_down(dev);
        break;
    default:
        IEEE80211_DPRINTF(vap, IEEE80211_MSG_IOCTL, "%s : mode not suported \n",__func__);
        break;
    }

    comhandle = wlan_vap_get_devhandle(vap);

    if (dev->flags & IFF_RUNNING)
    {
        dev->flags &= ~IFF_RUNNING;     /* mark us stopped */

#ifdef IEEE80211_DEBUG_NODELEAK
            printk("\n\n\n####################### DUMP NODES BEGIN ################## \n");
            wlan_dump_alloc_nodes(comhandle);
            printk("####################### DUMP NODES  END ################## \n");
#endif
        numvaps = osif_get_num_active_vaps(comhandle);
        if (numvaps == 0 && (comdev->flags & IFF_RUNNING)) {
            dev_close(comdev);
        }
    }

    return error;
}

static const u_int8_t ieee80211broadcastaddr[IEEE80211_ADDR_LEN] =
    { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

static int osif_setup_vap( osif_dev *osifp, enum ieee80211_opmode opmode,
                            u_int32_t flags, char *bssid)
{
    wlan_dev_t devhandle = osifp->os_devhandle;
    int error;
    wlan_if_t vap;
    ieee80211_privacy_exemption privacy_filter;
    int scan_priority_mapping_base;

    switch ((u_int8_t)opmode) {

#ifdef  ATH_SUPPORT_P2P
    case IEEE80211_M_P2P_GO:
        printk(" %s: Creating NEW P2P GO interface\n", __func__);
        osifp->p2p_go_handle = wlan_p2p_GO_create((wlan_p2p_t) devhandle, NULL);
        if (osifp->p2p_go_handle == NULL) {
            return EIO;
        }
        vap=wlan_p2p_GO_get_vap_handle(osifp->p2p_go_handle);
        break;

    case IEEE80211_M_P2P_CLIENT:
        printk(" %s: Creating NEW P2P Client interface\n", __func__);
        osifp->p2p_client_handle = wlan_p2p_client_create((wlan_p2p_t) devhandle, NULL);
        if (osifp->p2p_client_handle == NULL) {
            return EIO;
        }
        vap=wlan_p2p_client_get_vap_handle(osifp->p2p_client_handle);
        break;

    case IEEE80211_M_P2P_DEVICE:
        printk(" %s: Creating NEW P2P Device interface\n", __func__);
        osifp->p2p_handle = wlan_p2p_create(devhandle, NULL);
        if (osifp->p2p_handle == NULL) {
            return EIO;
        }
        vap=wlan_p2p_get_vap_handle(osifp->p2p_handle);
        break;
#endif
    default:
        printk(" %s: Creating NEW DEFAULT interface\n", __func__);
        if (opmode == IEEE80211_M_STA) {
            scan_priority_mapping_base = DEF_VAP_SCAN_PRI_MAP_OPMODE_STA_BASE;
        }
        else if (opmode == IEEE80211_M_IBSS) {
            scan_priority_mapping_base = DEF_VAP_SCAN_PRI_MAP_OPMODE_IBSS_BASE;
        }
        else if (opmode == IEEE80211_M_HOSTAP) {
            scan_priority_mapping_base = DEF_VAP_SCAN_PRI_MAP_OPMODE_AP_BASE;
        }
        else if (opmode == IEEE80211_M_MONITOR) {
            scan_priority_mapping_base = DEF_VAP_SCAN_PRI_MAP_OPMODE_STA_BASE;
        }
        else {
            scan_priority_mapping_base = DEF_VAP_SCAN_PRI_MAP_OPMODE_STA_BASE;
        }
        vap = wlan_vap_create(devhandle, opmode, scan_priority_mapping_base, flags, bssid);

        break;
    }

    if (!vap) {
        return ENOMEM;
    }

    osifp->os_opmode = opmode;
    osifp->os_if = vap;            /* back pointer */
    osif_vap_setup(vap, osifp->netdev, opmode);

    if ((u_int8_t)opmode == IEEE80211_M_P2P_DEVICE)
       wlan_set_desired_phymode(vap,IEEE80211_MODE_11G);
    else
       wlan_set_desired_phymode(vap, IEEE80211_MODE_AUTO);

    /* always setup a privacy filter to allow receiving unencrypted EAPOL frame */
    privacy_filter.ether_type = ETHERTYPE_PAE;
    privacy_filter.packet_type = IEEE80211_PRIVACY_FILTER_PACKET_BOTH;
    privacy_filter.filter_type = IEEE80211_PRIVACY_FILTER_KEY_UNAVAILABLE;
    wlan_set_privacy_filters(vap,&privacy_filter,1);
    do {
            ieee80211_auth_mode modes[1];
            ieee80211_cipher_type ctypes[1];
            u_int nmodes=1;

            /*
            * set default mode to OPEN.
            * default cipher set to NONE.
            */
            modes[0] = IEEE80211_AUTH_OPEN;
            error = wlan_set_authmodes(vap,modes,nmodes);

            ctypes[0] = IEEE80211_CIPHER_NONE;
            osifp->uciphers[0] = osifp->mciphers[0] = IEEE80211_CIPHER_NONE;
            osifp->u_count = osifp->m_count = 1;
            error = wlan_set_ucast_ciphers(vap,ctypes,1);
            error = wlan_set_mcast_ciphers(vap,ctypes,1);
    } while(0);

    /* register scan event handler */
    wlan_scan_register_event_handler(vap, &osif_scan_evhandler, (void*)osifp);
    wlan_scan_get_requestor_id(vap,(u_int8_t*)"osif_umac", &osifp->scan_requestor);

    return 0;
}


static void osif_delete_vap(struct net_device *dev)
{
    osif_dev *osifp = ath_netdev_priv(dev);
    wlan_if_t vap = osifp->os_if;
    int waitcnt=0;
    //wlan_dev_t comhandle = wlan_vap_get_devhandle(vap); //FIXME

    /* unregister scan event handler */
#if UMAC_SUPPORT_IBSS
    wlan_scan_unregister_event_handler(vap, &osif_ibss_scan_evhandler, (void *) osifp);
#endif
    wlan_scan_unregister_event_handler(vap, &osif_scan_evhandler, (void *) osifp);
    wlan_scan_clear_requestor_id(vap,osifp->scan_requestor);

  #ifdef ATH_SUPPORT_HTC
    /* delay a while for timer events handled by HTCThread */
    schedule_timeout_interruptible(HTC_THREAD_DELAY);
  #endif

    if (osifp->sm_handle) {
        wlan_connection_sm_delete(osifp->sm_handle);
        osifp->sm_handle = NULL;
    }

#if UMAC_SUPPORT_IBSS
    if (osifp->sm_ibss_handle) {
        wlan_ibss_sm_delete(osifp->sm_ibss_handle);
        osifp->sm_ibss_handle = NULL;
    }
#endif

    osifp->is_deleted = 0;
    /*
    * flush the frames belonging to this vap from osdep queues.
    */
    // ath_flush_txqueue(osifp->os_devhandle, vap);
    switch((u_int8_t)osifp->os_opmode) {
#ifdef  ATH_SUPPORT_P2P
    case IEEE80211_M_P2P_GO:
        printk(" Deleting  P2PGO vap \n");
        wlan_p2p_GO_delete(osifp->p2p_go_handle);
        osifp->p2p_go_handle = NULL;
        break;

    case IEEE80211_M_P2P_CLIENT:
        printk(" Deleting  P2P Client  \n");
        wlan_mlme_cancel(vap);
        wlan_p2p_client_delete(osifp->p2p_client_handle);
        osifp->p2p_client_handle = NULL;
        break;

    case IEEE80211_M_P2P_DEVICE:
        printk(" Deleting  P2P device  \n");
        wlan_p2p_delete(osifp->p2p_handle);
        osifp->p2p_handle = NULL;
        break;
#endif
    default:
        wlan_vap_delete(vap);
    }

    while(!osifp->is_deleted && waitcnt < OSIF_MAX_DELETE_VAP_TIMEOUT) {
        schedule_timeout_interruptible(HZ);
        waitcnt++;
    }
    if (!osifp->is_deleted) {
        IEEE80211_DPRINTF(vap, IEEE80211_MSG_IOCTL, "%s : tsleep failed \n",
                        __func__);
        msleep(2000);
    }
    //IEEE80211_DPRINTF(vap, IEEE80211_MSG_IOCTL, " %s: is_deleted : %d \n", 
    //       __func__, osifp->is_deleted);
    osifp->is_delete_in_progress = 0;
}

/*
* Extract a unit number from an interface name.  If no unit
* number is specified then -1 is returned for the unit number.
* Return 0 on success or an error code.
*/
static int
ifc_name2unit(const char *name, int *unit)
{
    const char *cp;

    for (cp = name; *cp != '\0' && !('0' <= *cp && *cp <= '9'); cp++)
        ;
    if (*cp != '\0')
    {
        *unit = 0;
        for (; *cp != '\0'; cp++)
        {
            if (!('0' <= *cp && *cp <= '9'))
                return -EINVAL;
            *unit = (*unit * 10) + (*cp - '0');
        }
    }
    else
        *unit = -1;
    return 0;
}

static u_int8_t wlan_units[32];     /* enough for 256 */

/*
* Allocate a new unit number.  If the map is full return -1;
* otherwise the allocate unit number is returned.
*/
static int
ieee80211_new_wlanunit(void)
{
#define N(a)    (sizeof(a)/sizeof(a[0]))
    u_int unit;
    u_int8_t b;
    int i;

    /* NB: covered by rtnl_lock */
    unit = 0;
    for (i = 0; i < N(wlan_units) && wlan_units[i] == 0xff; i++)
        unit += NBBY;
    if (i == N(wlan_units))
        return -1;
    for (b = wlan_units[i]; b & 1; b >>= 1)
        unit++;
    setbit(wlan_units, unit);

    return unit;
#undef N
}

/*
* Check if the specified unit number is available and, if
* so, mark it in use.  Return 1 on success, 0 on failure.
*/
static int
ieee80211_alloc_wlanunit(u_int unit)
{
    /* NB: covered by rtnl_lock */
    if (unit < sizeof(wlan_units)*NBBY && isclr(wlan_units, unit))
    {
        setbit(wlan_units, unit);
        return 1;
    }
    else
        return 0;
}

/*
* Reclaim the specified unit number.
*/
static void
osif_delete_wlanunit(u_int unit)
{
    /* NB: covered by rtnl_lock */
    KASSERT(unit < sizeof(wlan_units)*NBBY, ("invalid wlan unit %u", unit));
    KASSERT(isset(wlan_units, unit), ("wlan unit %u not allocated", unit));
    clrbit(wlan_units, unit);
}

#if ATH_SUPPORT_SPECTRAL
int
osif_ioctl_eacs(struct net_device *dev, struct ifreq *ifr, osdev_t os_handle)
{

    osif_dev *osifp = ath_netdev_priv(dev);
    wlan_if_t vap = osifp->os_if;
    ieee80211_scan_params scan_params;
    enum ieee80211_opmode opmode = wlan_vap_get_opmode(vap);
    ieee80211_ssid    ssid_list[IEEE80211_SCAN_MAX_SSID];
    int               n_ssid;

    if (!(dev->flags & IFF_UP)) {
        return -EINVAL;     /* XXX */
    }
    if (wlan_scan_in_progress(vap)) {
        wlan_scan_cancel(vap, osifp->scan_requestor, IEEE80211_ALL_SCANS, true);
        OS_DELAY (1000);
    }
    /* Fill scan parameter */
    n_ssid = wlan_get_desired_ssidlist(vap, ssid_list, IEEE80211_SCAN_MAX_SSID);
    OS_MEMZERO(&scan_params,sizeof(ieee80211_scan_params));
    wlan_set_default_scan_parameters(vap,&scan_params,IEEE80211_M_HOSTAP,
                                    false,true,false,true,0,NULL,0);

    switch (opmode)
    {
    case IEEE80211_M_HOSTAP:
        scan_params.type = IEEE80211_SCAN_BACKGROUND;
        scan_params.flags = IEEE80211_SCAN_PASSIVE | IEEE80211_SCAN_2GHZ;
        /* XXX tunables */
        scan_params.min_dwell_time_passive = 200;
        scan_params.max_dwell_time_passive = 300;

        break;
    default:break;
    }

    if (wlan_scan_start(vap, &scan_params, osifp->scan_requestor, IEEE80211_SCAN_PRIORITY_LOW, &(osifp->scan_id)) != 0 ) {
            printk("%s: Issue a scan fail.\n", __func__);
    }
    return 0;
}
#endif

extern int ieee80211_ioctl(struct net_device *dev, struct ifreq *ifr, int cmd);

#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,30)
static const struct net_device_ops osif_dev_ops = {
    .ndo_get_stats = osif_getstats,
    .ndo_open = osif_vap_open,
    .ndo_stop = osif_vap_stop,
    .ndo_start_xmit = osif_vap_hardstart,
    .ndo_set_multicast_list = osif_set_multicast_list,
    .ndo_do_ioctl = ieee80211_ioctl,
    .ndo_change_mtu = osif_change_mtu,
};
#endif
/*
* Create a virtual ap.  This is public as it must be implemented
* outside our control (e.g. in the driver).
*/
int
osif_ioctl_create_vap(struct net_device *comdev, struct ifreq *ifr, osdev_t os_handle)
{
    struct ieee80211_clone_params cp;
#if ATH_SUPPORT_WAPI
/* for WAPI support, ETHERTYPE_WAPI should be added to the privacy filter */
    ieee80211_privacy_exemption privacy_filter[2];
#else
    ieee80211_privacy_exemption privacy_filter;
#endif
    wlan_dev_t devhandle = ath_netdev_priv(comdev);

    struct net_device *dev;
    char name[IFNAMSIZ];
    int error, unit;
    osif_dev  *osifp;
    wlan_if_t vap;
    u_int8_t *vap_macaddr;
    int scan_priority_mapping_base;

    if (!capable(CAP_NET_ADMIN))
        return -EPERM;
    if (_copy_from_user(&cp, ifr->ifr_data, sizeof(cp)))
        return -EFAULT;
    error = ifc_name2unit(cp.icp_name, &unit);
    if (error)
        return error;

    if (unit == -1)
    {
        unit = ieee80211_new_wlanunit();
        if (unit == -1)
            return -EIO;        /* XXX */
        /* XXX verify name fits */
        snprintf(name, sizeof(name), "%s%d", cp.icp_name, unit);
    }
    else
    {
        if (!ieee80211_alloc_wlanunit(unit))
            return -EINVAL;
        strncpy(name, cp.icp_name, sizeof(name));
    }

    dev = alloc_netdev(sizeof(osif_dev), name, ether_setup);
    if(dev == NULL) {
        osif_delete_wlanunit(unit);
        return EIO;
    }
#if ATH_SUPPORT_BYPASS_BRIDGE
	ath0_dev = dev;
#endif
    /*
    ** Clear and initialize the osif structure
    */
    osifp = ath_netdev_priv(dev);

    memset(osifp,0,sizeof(osif_dev));

    osifp->netdev = dev;

    spin_lock_init(&osifp->list_lock);

    spin_lock_init(&osifp->tx_lock);

    switch ((u_int8_t)cp.icp_opmode) {
#ifdef ATH_SUPPORT_P2P
    case IEEE80211_M_P2P_GO:
        printk(" IEEE80211_M_P2P_GO created \n");
        osifp->p2p_go_handle = wlan_p2p_GO_create((wlan_p2p_t) devhandle, NULL);
        if (osifp->p2p_go_handle == NULL) {
            osif_delete_wlanunit(unit);
            //OS_FREE(osifp);
            free_netdev(dev);
            return EIO;
        }
        vap=wlan_p2p_GO_get_vap_handle(osifp->p2p_go_handle);
        /* enable UAPSD by default for GO Vap */
        /*  wlan_set_param(vap, IEEE80211_FEATURE_UAPSD, 0x1); */
        break;

    case IEEE80211_M_P2P_CLIENT:
        printk(" IEEE80211_M_P2P_CLIENT created \n");
        osifp->p2p_client_handle = wlan_p2p_client_create((wlan_p2p_t) devhandle, NULL);
        if (osifp->p2p_client_handle == NULL) {
            osif_delete_wlanunit(unit);
            //OS_FREE(osifp);
            free_netdev(dev);
            return EIO;
        }
        vap=wlan_p2p_client_get_vap_handle(osifp->p2p_client_handle);
        break;

    case IEEE80211_M_P2P_DEVICE:
        printk(" IEEE80211_M_P2P_DEVICE created \n");
        osifp->p2p_handle = wlan_p2p_create(devhandle, NULL);
        if (osifp->p2p_handle == NULL) {
            osif_delete_wlanunit(unit);
            //OS_FREE(osifp);
            free_netdev(dev);
            return EIO;
        }
        vap=wlan_p2p_get_vap_handle(osifp->p2p_handle);
        break;
#endif
    default:
        if (cp.icp_opmode == IEEE80211_M_STA) {
            scan_priority_mapping_base = DEF_VAP_SCAN_PRI_MAP_OPMODE_STA_BASE;
        }
        else if (cp.icp_opmode == IEEE80211_M_IBSS) {
            scan_priority_mapping_base = DEF_VAP_SCAN_PRI_MAP_OPMODE_IBSS_BASE;
        }
        else if (cp.icp_opmode == IEEE80211_M_HOSTAP) {
            scan_priority_mapping_base = DEF_VAP_SCAN_PRI_MAP_OPMODE_AP_BASE;
        }
        else if (cp.icp_opmode == IEEE80211_M_MONITOR) {
            scan_priority_mapping_base = DEF_VAP_SCAN_PRI_MAP_OPMODE_STA_BASE;
            dev->type = ARPHRD_IEEE80211_PRISM;
        }
        else {
            scan_priority_mapping_base = DEF_VAP_SCAN_PRI_MAP_OPMODE_STA_BASE;
        }
        vap = wlan_vap_create(devhandle, cp.icp_opmode, scan_priority_mapping_base, cp.icp_flags, cp.icp_bssid);
        break;
    }

    if (vap == NULL) {
        osif_delete_wlanunit(unit);
        //OS_FREE(osifp);
        free_netdev(dev);
        return EIO;
    }

    INIT_LIST_HEAD(&osifp->pending_rx_frames);
    spin_lock_init(&osifp->list_lock);

    osifp->os_if = vap;
    osifp->os_handle = os_handle;
    osifp->os_devhandle = devhandle;
    osifp->os_comdev = comdev;
    osifp->os_opmode = cp.icp_opmode;
    osifp->os_unit  = unit;
#ifdef ATHEROS_LINUX_PERIODIC_SCAN
    osifp->os_periodic_scan_period = OSIF_PERIODICSCAN_DEF_PERIOD;
    OS_INIT_TIMER(os_handle, &(osifp->os_periodic_scan_timer), periodic_scan_timer_handler, (void *)osifp);
#endif
#ifdef ATH_SUPPORT_WAPI
    osifp->os_wapi_rekey_period = OSIF_WAPI_REKEY_TIMEOUT;
    OS_INIT_TIMER(os_handle, &(osifp->os_wapi_rekey_timer), osif_wapi_rekey_timeout, (void *)osifp);
#endif
    osif_vap_setup(vap, dev, cp.icp_opmode);
    vap_macaddr = wlan_vap_get_macaddr(vap);
    IEEE80211_ADDR_COPY(dev->dev_addr, vap_macaddr);

#ifdef __CARRIER_PLATFORM__
	osif_pltfrm_record_macinfor(unit,dev->dev_addr);
#endif    

    if (name != NULL)   /* XXX */
        strncpy(dev->name, name, sizeof(dev->name));
// fixme where is this done in the new scheme?    dev->priv = vap;
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,30)
    dev->netdev_ops = &osif_dev_ops;
#if ATH_SUPPORT_VLAN
    adf_net_vlan_attach(dev,(struct net_device_ops *) dev->netdev_ops);
#endif
#else
    dev->get_stats = osif_getstats;
    dev->open = osif_vap_open;
    dev->stop = osif_vap_stop;
    dev->hard_start_xmit = osif_vap_hardstart;
    dev->set_multicast_list = osif_set_multicast_list;
#if ATH_SUPPORT_VLAN
    adf_net_vlan_attach(dev);
#endif
#if 0
    dev->set_mac_address = ieee80211_set_mac_address;
#endif
    dev->change_mtu = osif_change_mtu;
#endif
    dev->tx_queue_len = 0;          /* NB: bypass queueing */
    dev->hard_header_len = comdev->hard_header_len;
#ifdef __CARRIER_PLATFORM__
    osif_pltfrm_create_vap(osifp);
#endif

    /*
    * The caller is assumed to allocate the device with
    * alloc_etherdev or similar so we arrange for the
    * space to be reclaimed accordingly.
    */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
    /* in 2.4 things are done differently... */
    dev->features |= NETIF_F_DYNALLOC;
#else
    dev->destructor = free_netdev;
#endif

    /* return final device name */
    strncpy(ifr->ifr_name, name, IFNAMSIZ);
    // printk(KERN_ERR "VAP device %s created \n", name);

    os_if_media_init(osifp);
    if ((u_int8_t)cp.icp_opmode == IEEE80211_M_P2P_DEVICE)
        wlan_set_desired_phymode(vap, IEEE80211_MODE_11G);
    else
        wlan_set_desired_phymode(vap, IEEE80211_MODE_AUTO);
#if ATH_SUPPORT_WAPI
    /* always setup a privacy filter to allow receiving unencrypted EAPOL frame and/or WAI frame for WAPI*/
    privacy_filter[0].ether_type = ETHERTYPE_PAE;
    privacy_filter[0].packet_type = IEEE80211_PRIVACY_FILTER_PACKET_BOTH;
    privacy_filter[0].filter_type = IEEE80211_PRIVACY_FILTER_KEY_UNAVAILABLE;
    privacy_filter[1].ether_type = ETHERTYPE_WAI;
    privacy_filter[1].packet_type = IEEE80211_PRIVACY_FILTER_PACKET_BOTH;
    privacy_filter[1].filter_type =IEEE80211_PRIVACY_FILTER_ALLWAYS;
    wlan_set_privacy_filters(vap,privacy_filter,2);
#else
    /* always setup a privacy filter to allow receiving unencrypted EAPOL frame */
    privacy_filter.ether_type = ETHERTYPE_PAE;
    privacy_filter.packet_type = IEEE80211_PRIVACY_FILTER_PACKET_BOTH;
    privacy_filter.filter_type = IEEE80211_PRIVACY_FILTER_KEY_UNAVAILABLE;
    wlan_set_privacy_filters(vap,&privacy_filter,1);
#endif /* ATH_SUPPORT_WAPI */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,31)
    dev->do_ioctl = ieee80211_ioctl;
#endif
    ieee80211_ioctl_vattach(dev);
    do {
            ieee80211_auth_mode modes[1];
            u_int nmodes=1;

            /*
            * set default mode to OPEN.
            */
            modes[0] = IEEE80211_AUTH_OPEN;
            error = wlan_set_authmodes(vap,modes,nmodes);

    } while(0);

    /* register scan event handler */
    wlan_scan_register_event_handler(vap, &osif_scan_evhandler, (void *)osifp);
    wlan_scan_get_requestor_id(vap,(u_int8_t*)"osif_umac", &osifp->scan_requestor);
    /* NB: rtnl is held on entry so don't use register_netdev */
    if (register_netdevice(dev))
    {
        printk(KERN_ERR "%s: unable to register device\n", dev->name);
        return 0;
    }
    return 0;
}


int
osif_ioctl_delete_vap(struct net_device *dev)
{
       osif_dev *osnetdev = ath_netdev_priv(dev);
       wlan_if_t vap = osnetdev->os_if;

       osnetdev->is_delete_in_progress=1;

#ifdef ATHEROS_LINUX_PERIODIC_SCAN
        OS_FREE_TIMER(&(osnetdev->os_periodic_scan_timer));
#endif
#ifdef ATH_SUPPORT_WAPI
        OS_FREE_TIMER(&(osnetdev->os_wapi_rekey_timer));
#endif
        ifmedia_removeall(&osnetdev->os_media);
        /* unregister scan event handler */
#if UMAC_SUPPORT_IBSS
        wlan_scan_unregister_event_handler(vap, &osif_ibss_scan_evhandler, (void *)osnetdev);
#endif
        wlan_scan_unregister_event_handler(vap, &osif_scan_evhandler, (void *)osnetdev);
        wlan_scan_clear_requestor_id(vap, osnetdev->scan_requestor);

        osif_vap_stop(dev);    /* force state before cleanup */
        osif_delete_vap(dev);

        spin_lock_destroy(&osnetdev->list_lock);
        spin_lock_destroy(&osnetdev->tx_lock);
#ifdef __CARRIER_PLATFORM__
        osif_pltfrm_delete_vap(osnetdev);
#endif
        osnetdev->os_if = NULL;
        osif_delete_wlanunit(osnetdev->os_unit);
        unregister_netdevice(dev);
        //fixme is this needed? OS_FREE(osnetdev);
        return 0;
}
int
osif_ioctl_switch_vap(struct net_device *dev, enum ieee80211_opmode opmode)
{
        osif_dev *osifp = ath_netdev_priv(dev);
        wlan_if_t vap = osifp->os_if;
        u_int8_t *macaddr;
        u_int32_t msg_flags;

        IEEE80211_DPRINTF(vap, IEEE80211_MSG_IOCTL, "%s : from %d to %d  \n",
                        __func__, osifp->os_opmode, opmode);


        if (osifp->os_opmode == opmode) {
            return 0;
        }

        macaddr = wlan_vap_get_macaddr(vap);
        IEEE80211_DPRINTF(vap, IEEE80211_MSG_IOCTL, "%s : old mac addr %s   \n", __func__, ether_sprintf(macaddr));

        netif_carrier_off(dev);
        msg_flags = wlan_get_param(vap, IEEE80211_MSG_FLAGS);
        osifp->is_delete_in_progress = 1;
        osif_vap_stop(dev);
        osif_delete_vap(dev);
        osifp->os_if = NULL;
        osif_setup_vap(osifp,opmode,IEEE80211_CLONE_BSSID,NULL);
        vap = osifp->os_if;
        if (!vap) {
           return -EINVAL;
        }
        wlan_set_param(vap, IEEE80211_MSG_FLAGS, msg_flags);
        netif_carrier_on(dev);

        macaddr = wlan_vap_get_macaddr(vap);
        IEEE80211_DPRINTF(vap, IEEE80211_MSG_IOCTL, "%s : new mac addr %s   \n", __func__, ether_sprintf(macaddr));

        return 0;
}
