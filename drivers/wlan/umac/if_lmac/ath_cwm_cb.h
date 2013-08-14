/*
 *  Copyright (c) 2008 Atheros Communications Inc.
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
 
/*
 * CWM (Channel Width Management) 
 *
 */
#ifndef _ATH_CWM_CB_H_
#define _ATH_CWM_CB_H_

#include "ath_cwm_project.h"

int ath_cwm_ht40allowed(struct ath_softc_net80211 *scn);
void ath_cwm_rate_updatenode(void *arg);
void ath_cwm_change_channel(struct ath_softc_net80211 *scn);
void ath_cwm_sendactionmgmt(struct ath_softc_net80211 *scn);
void ath_cwm_rate_updateallnodes(struct ath_softc_net80211 *scn);
int ath_cwm_ht40allowed(struct ath_softc_net80211 *scn);
void ath_cwm_set_macmode(struct ath_softc_net80211 *scn, enum cwm_ht_macmode cw_macmode);
void ath_cwm_set11nmac2040(struct ath_softc_net80211 *scn, enum cwm_ht_macmode cw_macmode);
void ath_cwm_set_curchannelflags_ht20(struct ath_softc_net80211 *scn);
void ath_cwm_set_curchannelflags_ht40(struct ath_softc_net80211 *scn);
void ath_cwm_stoptxdma(struct ath_softc_net80211 *scn);
void ath_cwm_resumetxdma(struct ath_softc_net80211 *scn);
void ath_cwm_requeue(void *arg);
void ath_cwm_process_tx_pkts(struct ath_softc_net80211 *scn);
void ath_cwm_requeue_tx_pkts(struct ath_softc_net80211 *scn);
enum cwm_phymode ath_cwm_get_curchannelmode(struct ath_softc_net80211 *scn);
enum cwm_opmode ath_cwm_get_icopmode(struct ath_softc_net80211 *scn);
int ath_cwm_get_extchbusyper(struct ath_softc_net80211 *scn);

/* Callback defines */
#define CWM_GET_IC_OPMODE_CB(g_cwm) ath_cwm_get_icopmode(g_cwm->wlandev)
#define CWM_RATE_UPDATENODE_CB(g_cwm, arg) ath_cwm_rate_updatenode(arg)
#define CWM_CHANGE_CHANNEL_CB(g_cwm) ath_cwm_change_channel(g_cwm->wlandev)
#define CWM_SENDACTIONMGMT_CB(g_cwm) ath_cwm_sendactionmgmt(g_cwm->wlandev)
#define CWM_RATE_UPDATEALLNODES_CB(g_cwm) ath_cwm_rate_updateallnodes(g_cwm->wlandev)
#define CWM_HT40_ALLOWED_CB(g_cwm) ath_cwm_ht40allowed(g_cwm->wlandev)
#define CWM_SET_MACMODE_CB(g_cwm, mode) ath_cwm_set_macmode(g_cwm->wlandev, mode)
#define CWM_SET_CURCHAN_FLAGS_HT20_CB(g_cwm) ath_cwm_set_curchannelflags_ht20(g_cwm->wlandev)
#define CWM_SET_CURCHAN_FLAGS_HT40_CB(g_cwm) ath_cwm_set_curchannelflags_ht40(g_cwm->wlandev)
#define CWM_GET_CURCH_MODE_CB(g_cwm) ath_cwm_get_curchannelmode(g_cwm->wlandev)
#define CWM_GET_EXTCHBUSYPER_CB(g_cwm) ath_cwm_get_extchbusyper(g_cwm->wlandev)

#ifndef ATH_CWM_MAC_DISABLE_REQUEUE
#define CWM_STOPTXDMA_CB(g_cwm) ath_cwm_stoptxdma(g_cwm->wlandev)
#define CWM_RESUMETXDMA_CB(g_cwm) ath_cwm_resumetxdma(g_cwm->wlandev)
#define CWM_REQUEUE_CB(g_cwm, arg) ath_cwm_requeue(arg)
#define CWM_PROCESS_TX_PKTS_CB(g_cwm) ath_cwm_process_tx_pkts(g_cwm->wlandev)
#define CWM_REQUEUE_TX_PKTS_CB(g_cwm) ath_cwm_requeue_tx_pkts(g_cwm->wlandev)
#endif
 



#endif


