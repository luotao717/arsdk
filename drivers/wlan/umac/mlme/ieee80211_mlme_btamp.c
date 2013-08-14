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

#include "ieee80211_mlme_priv.h"    /* Private to MLME module */

#if UMAC_SUPPORT_BTAMP

void ieee80211_mlme_join_complete_btamp(struct ieee80211_node *ni)
{
    struct ieee80211vap           *vap = ni->ni_vap;
    struct ieee80211_mlme_priv    *mlme_priv = vap->iv_mlme_priv;

    IEEE80211_DPRINTF(vap, IEEE80211_MSG_MLME, "%s\n", __func__);

    /* Request complete */
    //mlme_priv->im_request_type = MLME_REQ_NONE;

    /* Call MLME confirmation handler */
    IEEE80211_DELIVER_EVENT_MLME_JOIN_COMPLETE_INFRA(vap, IEEE80211_STATUS_SUCCESS);
}

int ieee80211_mlme_auth_request_btamp(wlan_if_t vaphandle, u_int8_t *peer_addr, u_int32_t timeout)
{
    struct ieee80211vap           *vap = vaphandle;
    struct ieee80211_mlme_priv    *mlme_priv = vap->iv_mlme_priv;
    struct ieee80211_node         *ni = NULL;
    int                           error = 0;

    IEEE80211_DPRINTF(vap, IEEE80211_MSG_MLME, "%s\n", __func__);
    
    ni = ieee80211_find_txnode(vap, peer_addr);
    //ni = ieee80211_find_node(&vap->iv_ic->ic_sta, peer_addr);

    if (ni == NULL) {
        return EINVAL;
    }

    /* Wait for auth seq number 2 (open response) */
    ASSERT(mlme_priv->im_request_type == MLME_REQ_NONE);
    mlme_priv->im_request_type = MLME_REQ_AUTH;
    mlme_priv->im_expected_auth_seq_number = IEEE80211_AUTH_OPEN_RESPONSE;

    /* Set the timeout timer for authenticate failure case */
    OS_SET_TIMER(&mlme_priv->im_timeout_timer, timeout);

    /*  Send the authentication packet */
    error = ieee80211_send_auth(ni, IEEE80211_AUTH_OPEN_REQUEST, 0, NULL, 0);
    ieee80211_free_node(ni);
    if (error) {
        goto err;
    }

    return error;

err:
    mlme_priv->im_request_type = MLME_REQ_NONE;
    OS_CANCEL_TIMER(&mlme_priv->im_timeout_timer);
    return error;
}

void mlme_recv_auth_btamp(struct ieee80211_node *ni,
                          u_int16_t algo, u_int16_t seq, u_int16_t status_code,
                          u_int8_t *challenge, u_int8_t challenge_length, wbuf_t wbuf)
{

    struct ieee80211vap           *vap = ni->ni_vap;
    struct ieee80211_mlme_priv    *mlme_priv = vap->iv_mlme_priv;
    struct ieee80211_frame        *wh;
    u_int16_t                     indication_status = IEEE80211_STATUS_SUCCESS,response_status = IEEE80211_STATUS_SUCCESS ;
    bool                          send_auth_response=true, indicate=true;

    wh = (struct ieee80211_frame *) wbuf_header(wbuf);
    /* AP must be up and running */
    if (!mlme_priv->im_connection_up || ieee80211_vap_ready_is_clear(vap)) {
        return;
    }

    IEEE80211_NOTE_MAC(vap, IEEE80211_MSG_AUTH, wh->i_addr2,
                       "recv auth frame with algorithm %d seq %d \n", algo, seq);

    do {

        /* Check node existance for the peer */
        if (ni == vap->iv_bss) {
            return;
        } else {
            ieee80211_ref_node(ni);
        }

        /* Validate algo */
        if (algo == IEEE80211_AUTH_ALG_OPEN) {
            if (mlme_priv->im_expected_auth_seq_number) {
                send_auth_response = false;
                indicate = false;
                if (seq == mlme_priv->im_expected_auth_seq_number) {
                    if (!OS_CANCEL_TIMER(&mlme_priv->im_timeout_timer)) {
                        IEEE80211_DPRINTF(vap, IEEE80211_MSG_MLME, "%s: Timed-out already\n", __func__);
                        break;
                    }

                    IEEE80211_DPRINTF(vap, IEEE80211_MSG_MLME, "%s: mlme_auth_complete\n", __func__);

                    /* Request complete */
                    mlme_priv->im_request_type = MLME_REQ_NONE;

                    /* Authentication complete (success or failure) */
                    IEEE80211_DELIVER_EVENT_MLME_AUTH_COMPLETE(vap, status_code);
                    vap->iv_mlme_priv->im_expected_auth_seq_number = 0;
                } else {
                    break;
                }
            } else {
                if (seq != IEEE80211_AUTH_OPEN_REQUEST) {
                    response_status = IEEE80211_STATUS_SEQUENCE;
                    indication_status = IEEE80211_STATUS_SEQUENCE;
                    break;
                } else {
                    indicate = true;
                    send_auth_response = true;
                }
            }
        } else if (algo == IEEE80211_AUTH_ALG_SHARED) {
            response_status = IEEE80211_STATUS_ALG;
            indication_status = IEEE80211_STATUS_ALG;
            break;
        } else {
            IEEE80211_DPRINTF(vap, IEEE80211_MSG_AUTH | IEEE80211_MSG_CRYPTO,
                              "[%s] auth: unsupported algorithm %d \n",ether_sprintf(wh->i_addr2),algo);
            vap->iv_stats.is_rx_auth_unsupported++;
            response_status = IEEE80211_STATUS_ALG;
            indication_status = IEEE80211_STATUS_ALG;
            break;
        }
    } while (FALSE);

    IEEE80211_DELIVER_EVENT_MLME_AUTH_INDICATION(vap, ni->ni_macaddr, indication_status);

    if (send_auth_response) {
        ieee80211_send_auth(ni, seq + 1, response_status, NULL, 0);
    }

    if (ni) {
        if (indication_status != IEEE80211_STATUS_SUCCESS ){
            /* auth is not success, remove the node from node table*/
            ieee80211_node_leave(ni);
        }
        /*
         * release the reference created at the begining of the case above
         * either by alloc_node or ref_node.
         */ 
        ieee80211_free_node(ni);
    }
}

static int mlme_assoc_reassoc_request_btamp(wlan_if_t vaphandle, u_int8_t *mac_addr,
                                            int reassoc, u_int8_t *prev_bssid, u_int32_t timeout)
{
    struct ieee80211vap           *vap = vaphandle;
    struct ieee80211_mlme_priv    *mlme_priv = vap->iv_mlme_priv;
    struct ieee80211_node         *ni = NULL;
    int                           error = 0;

    IEEE80211_DPRINTF(vap, IEEE80211_MSG_MLME, "%s\n", __func__);

    ni = ieee80211_find_txnode(vap, mac_addr);
    //ni = ieee80211_find_node(&vap->iv_ic->ic_sta, mac_addr);

    if (ni == NULL) {
        return EINVAL;
    }

    ASSERT(mlme_priv->im_request_type == MLME_REQ_NONE);
    mlme_priv->im_request_type = reassoc ? MLME_REQ_REASSOC : MLME_REQ_ASSOC;

    /* Set the timeout timer for association failure case */
    OS_SET_TIMER(&mlme_priv->im_timeout_timer, timeout);

    /* Transmit frame */
    error = ieee80211_send_assoc(ni, reassoc, prev_bssid);
    ieee80211_free_node(ni);
    if (error) {
        goto err;
    }

    return error;

err:
    mlme_priv->im_request_type = MLME_REQ_NONE;
    OS_CANCEL_TIMER(&mlme_priv->im_timeout_timer);
    return error;
}

int ieee80211_mlme_assoc_request_btamp(wlan_if_t vaphandle, u_int8_t *peer, u_int32_t timeout)
{
    struct ieee80211vap    *vap = vaphandle;

    IEEE80211_DPRINTF(vap, IEEE80211_MSG_MLME, "%s\n", __func__);
    return mlme_assoc_reassoc_request_btamp(vaphandle, peer, 0, NULL, timeout);
}

int ieee80211_mlme_reassoc_request_btamp(wlan_if_t vaphandle, u_int8_t *peer, u_int8_t *prev_bssid, u_int32_t timeout)
{
    struct ieee80211vap    *vap = vaphandle;

    IEEE80211_DPRINTF(vap, IEEE80211_MSG_MLME, "%s\n", __func__);
    return mlme_assoc_reassoc_request_btamp(vaphandle, peer, 1, prev_bssid, timeout);
}

#endif /* UMAC_SUPPORT_BTAMP */
