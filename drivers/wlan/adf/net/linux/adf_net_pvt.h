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

#ifndef __ADF_HST_NET_PVT_H
#define __ADF_HST_NET_PVT_H

#include <net/sch_generic.h>
#include <net/inet_ecn.h>
#include <linux/netdevice.h>
#include <linux/pci.h>
#include <linux/stddef.h>
#include <linux/err.h>
#include <asm/checksum.h>
#include <net/ip6_checksum.h>
#include <linux/crc32.h>

//#include <adf_os_types.h>
//#include <adf_net_types.h>
//#include <asf_queue.h>
#include <adf_net_wcmd.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/in.h>

#define ADF_NET_MAX_NAME            64
#define ADF_DEF_TX_TIMEOUT          5 /*Seconds I suppose*/
#define __ADF_NET_NULL              NULL

#define ADF_NET_ETHERNET_FCS_SIZE   4

/**
 * This is a per driver structure, that holds info about an adf_drv that has
 * registered with anet. This will be used to lookup the details when a probe
 * is called.
 */

typedef struct __adf_softc {
    adf_drv_handle_t    drv_hdl;
    struct net_device   *netdev;
    struct vlan_group   *vlgrp;/*Vlan group*/
    a_uint16_t		vid; /* vlan id */
    void                *poll_arg;   /*for poll_schedule_cpu */
    adf_dev_sw_t        sw;          /*cp of the switch*/
}__adf_softc_t;


a_status_t __adf_net_get_vlan_tag(struct sk_buff *skb, adf_net_vlanhdr_t *hdr);
a_status_t      __adf_net_indicate_packet(adf_net_handle_t hdl, 
                                           struct sk_buff *pkt, uint32_t len);
a_status_t       __adf_net_indicate_vlanpkt(adf_net_handle_t hdl,
                                            struct sk_buff *pkt, uint32_t len, 
                                            adf_net_vid_t *vid);
void             __adf_net_poll_schedule(adf_net_handle_t hdl);
a_status_t             __adf_net_poll_schedule_cpu(adf_net_handle_t hdl, 
                                             uint32_t cpu_msk, void *arg);
a_status_t       __adf_net_dev_tx(adf_net_handle_t hdl, struct sk_buff *pkt);
adf_net_handle_t __adf_net_dev_create(adf_drv_handle_t hdl, adf_dev_sw_t *op,
                                      adf_net_dev_info_t *info);
a_status_t             __adf_net_dev_delete(adf_net_handle_t hdl);


a_status_t       __adf_net_dev_open(adf_net_handle_t hdl);
a_status_t       __adf_net_dev_close(adf_net_handle_t hdl);
const uint8_t *      __adf_net_ifname(adf_net_handle_t  hdl);
a_status_t       __adf_net_fw_mgmt_to_app(adf_net_handle_t hdl, 
                                          struct sk_buff *pkt, uint32_t len);
a_status_t       __adf_net_send_wireless_event(adf_net_handle_t hdl,
                                    adf_net_wireless_event_t what, void *data, 
                                    adf_os_size_t data_len);

int             __adf_net_get_wioctl(struct net_device *ifnet, 
                                     adf_net_wcmd_t *wcmd);
int             __adf_net_set_wioctl(struct net_device *ifnet, 
                                     adf_net_wcmd_t *wcmd);
a_status_t      __adf_net_register_drv(adf_drv_info_t *drv);
void            __adf_net_unregister_drv(a_uint8_t *drv_name);

#define hdl_to_softc(_hdl)      ((__adf_softc_t *)(_hdl))
#define hdl_to_netdev(_hdl)     ((hdl_to_softc(_hdl))->netdev)
#define hdl_to_virt(_hdl)       (hdl_to_softc(_hdl)->virt)
#define hdl_to_sw(_hdl)         (hdl_to_softc(_hdl)->sw)
#define hdl_to_drv(_hdl)        (hdl_to_softc(_hdl)->drv_hdl)
/**
 * XXX:netdev->flags are in the the section which are not
 * exposed to the outside world, hence subjected to change.
 * Thats what comment says in netdev
 */
#define hdl_to_ifflags(_hdl)    (hdl_to_netdev(_hdl)->flags)

#define netdev_to_softc(_dev)   ((__adf_softc_t *)netdev_priv(_dev))
#define netdev_to_sw(_dev)      ((netdev_to_softc(_dev)->sw))
#define netdev_to_drv(_dev)     (netdev_to_softc(_dev)->drv_hdl)




#define adf_drv_open(_dev)  \
    netdev_to_sw(_dev).drv_open(netdev_to_drv(_dev))

#define adf_drv_close(_dev) \
    netdev_to_sw(_dev).drv_close(netdev_to_drv(_dev))

#define adf_drv_tx(_dev, _pkt)  \
    netdev_to_sw(_dev).drv_tx(netdev_to_drv(_dev), _pkt)

#define adf_drv_cmd(_dev, _cmd, _data)  \
    netdev_to_sw(_dev).drv_cmd(netdev_to_drv(_dev), _cmd, _data)

#define adf_drv_ioctl(_dev, _num, _data)    \
    netdev_to_sw(_dev).drv_ioctl(netdev_to_drv(_dev), _num, _data)

#define adf_drv_poll(_dev, _qouta, _work_done)  \
    netdev_to_sw(_dev).drv_poll(netdev_to_drv(_dev), _quota, _work_done)

#define adf_drv_poll_cpu(_dev, _quota, _work_done, _arg)    \
    netdev_to_sw(_dev).drv_poll_cpu(netdev_to_drv(_dev), _quota, _work_done, _arg)

#define adf_drv_poll_int_disable(_dev)  \
    netdev_to_sw(_dev).drv_open(netdev_to_drv(_dev))

#define adf_drv_tx_timeout(_dev)    \
    netdev_to_sw(_dev).drv_tx_timeout(netdev_to_drv(_dev))

#define adf_drv_wcmd(_dev, _wcmd, _data) \
    netdev_to_sw(_dev).drv_wcmd(netdev_to_drv(_dev), _wcmd, _data)


static inline int 
__a_status_to_os(a_status_t _status)
{
    switch(_status)
    {
    case A_STATUS_OK:
        return 0;
    case A_STATUS_ENOENT:
        return ENOENT;
    case A_STATUS_ENOMEM:
        return ENOMEM;
    case A_STATUS_EINVAL:
        return EINVAL;
    case A_STATUS_EINPROGRESS:
        return EINPROGRESS;
    case A_STATUS_EBUSY:
        return EBUSY;
    case A_STATUS_ENOTSUPP:
        return ENOTSUPP;
    case A_STATUS_EIO:
        return EIO;
    case A_STATUS_FAILED:
    default:
        printk("ADF_NET:error:unkown error status: 0x%x\n", _status);
        adf_os_assert(0);
        return ENOTSUPP;
    }
}

static inline void 
__adf_net_carrier_on(adf_net_handle_t hdl)
{
    return(netif_carrier_on(hdl_to_netdev(hdl)));
}

static inline void 
__adf_net_carrier_off(adf_net_handle_t hdl)
{
    return(netif_carrier_off(hdl_to_netdev(hdl)));
}

static inline int 
__adf_net_carrier_ok(adf_net_handle_t hdl)
{
    return (netif_carrier_ok(hdl_to_netdev(hdl)));
}

static inline void 
__adf_net_start_queue(adf_net_handle_t hdl)
{
    return(netif_start_queue(hdl_to_netdev(hdl)));
}

static inline void 
__adf_net_wake_queue(adf_net_handle_t hdl)
{
    return(netif_start_queue(hdl_to_netdev(hdl)));
}
    
static inline void 
__adf_net_stop_queue(adf_net_handle_t hdl)
{
    return(netif_stop_queue(hdl_to_netdev(hdl)));
}

static inline int 
__adf_net_queue_stopped(adf_net_handle_t hdl)
{
    return(netif_queue_stopped(hdl_to_netdev(hdl)));
}
/**
 * @brief check if the interface is running
 * @param hdl
 * 
 * @return a_bool_t
 */
static inline a_bool_t
__adf_net_is_running(adf_net_handle_t hdl)
{

    return (hdl_to_ifflags(hdl) & IFF_RUNNING);
}
/**
 * @brief check if the interface is UP
 * @param hdl
 * 
 * @return a_bool_t
 */
static inline a_bool_t
__adf_net_is_up(adf_net_handle_t hdl)
{
    return (hdl_to_ifflags(hdl) & IFF_UP);
}
/**
 * @brief check ALLMULTI flag
 * @param hdl
 * 
 * @return a_bool_t
 */
static inline a_bool_t
__adf_net_is_allmulti(adf_net_handle_t   hdl)
{
    return (hdl_to_ifflags(hdl) & IFF_ALLMULTI);
}
/**
 * @brief check the promisc flag
 * @param hdl
 * 
 * @return a_bool_t
 */
static inline a_bool_t
__adf_net_is_promisc(adf_net_handle_t   hdl)
{
    return (hdl_to_ifflags(hdl) & IFF_PROMISC);
}

static inline adf_os_handle_t
__adf_net_hdl_to_os(adf_net_handle_t hdl)
{
    return hdl;
}
static inline adf_os_handle_t
__adf_net_dev_to_os(__adf_os_device_t osdev)
{
        return ((adf_os_handle_t)osdev);
}

static inline void
__adf_net_get_mclist(adf_net_handle_t  hdl, adf_net_devaddr_t  *mclist)
{
    struct net_device    *netdev = hdl_to_netdev(hdl);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 34)
    struct netdev_hw_addr *ha;  
#else   
    struct dev_mc_list   *mc = netdev->mc_list;
#endif
    int cnt = 0;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 34)
    netdev_for_each_mc_addr(ha, netdev) {
        mclist->da_addr[cnt] = ha->addr;
    }
#else
    for(; mc; mc = mc->next, cnt++)
        mclist->da_addr[cnt] = mc->dmi_addr;
#endif

    mclist->num = cnt;
}



#endif /*__ADF_NET_PVT_H*/
