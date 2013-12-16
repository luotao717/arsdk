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


#include "ieee80211_mlme_priv.h"

#if UMAC_SUPPORT_IBSS

#if ATH_SUPPORT_IBSS_DFS
/*
*   In this function , we compare the channel list and delete those one that are not in our original IBSS_DFS channel list  
*/
static u_int8_t
ieee80211_check_and_update_ibss_dfs(struct ieee80211vap * vap, struct ieee80211_ibssdfs_ie * ibss_dfs_ie)
{
    struct ieee80211_ibssdfs_ie     *old_ibss_dfs_ie = &vap->iv_ibssdfs_ie_data;
    u_int8_t                         unit_len         = sizeof(struct channel_map_field);
    int                             i,j              = 0;
    u_int8_t                        changed          = 0;
    u_int8_t                        foundunit        = 0;
    
    if(ibss_dfs_ie->len < MIN_IBSS_DFS_IE_CONTENT_SIZE || old_ibss_dfs_ie->len < MIN_IBSS_DFS_IE_CONTENT_SIZE )
        return changed;
    
    for( i = (old_ibss_dfs_ie->len -IBSS_DFS_ZERO_MAP_SIZE)/unit_len; i >0; i--) {
        for( j = (ibss_dfs_ie->len -IBSS_DFS_ZERO_MAP_SIZE)/unit_len ; j > 0 ; j--) {
            if(old_ibss_dfs_ie->ch_map_list[i-1].ch_num == ibss_dfs_ie->ch_map_list[j-1].ch_num) {
                foundunit ++;
                /* found the entry, check radar bit */
                if(!ibss_dfs_ie->ch_map_list[j-1].ch_map.unmeasured) {
                    if(old_ibss_dfs_ie->ch_map_list[i-1].chmap_in_byte != ibss_dfs_ie->ch_map_list[j-1].chmap_in_byte) {
                       old_ibss_dfs_ie->ch_map_list[i-1].chmap_in_byte |= ibss_dfs_ie->ch_map_list[j-1].chmap_in_byte;
                       old_ibss_dfs_ie->ch_map_list[j-1].ch_map.unmeasured = 0;
                       changed = 1;
                    }
                }
                break;
            }
        }
        /* failed to find the entry, we need to delete this entry */
        if( j == 0 ) {
            OS_MEMCPY(&old_ibss_dfs_ie->ch_map_list[i-1], &old_ibss_dfs_ie->ch_map_list[i],foundunit*unit_len);
            old_ibss_dfs_ie->len -= unit_len;
            changed = 1;
        }
    }
    return changed;
}
#endif /* ATH_SUPPORT_IBSS_DFS */
void ieee80211_recv_beacon_ibss(struct ieee80211_node *ni, wbuf_t wbuf, int subtype, 
                                struct ieee80211_rx_status *rs, ieee80211_scan_entry_t  scan_entry)
{
    struct ieee80211vap                          *vap = ni->ni_vap;
    struct ieee80211com                          *ic = ni->ni_ic;
    u_int16_t                                    capinfo;
    int                                          ibssmerge = 0;
    struct ieee80211_channelswitch_ie            *chanie = NULL;
    struct ieee80211_extendedchannelswitch_ie    *echanie = NULL;
    struct ieee80211_frame                       *wh;
    u_int64_t                                    tsf;
    u_int8_t                                     *quiet_elm = NULL; 
    bool                                         free_node = FALSE;
#if ATH_SUPPORT_IBSS_DFS
    struct ieee80211_ibssdfs_ie                  *ibssdfsie = NULL;
#endif /* ATH_SUPPORT_IBSS_DFS */

    wh = (struct ieee80211_frame *)wbuf_header(wbuf);
    capinfo = ieee80211_scan_entry_capinfo(scan_entry);

    if (!(capinfo & IEEE80211_CAPINFO_IBSS))
        return;

    OS_MEMCPY((u_int8_t *)&tsf, ieee80211_scan_entry_tsf(scan_entry), sizeof(tsf));
    if (ni != ni->ni_bss_node) {
        OS_MEMCPY(ni->ni_tstamp.data, &tsf, sizeof(ni->ni_tstamp));
        /* update activity indication */
        ni->ni_beacon_rstamp = OS_GET_TIMESTAMP();
        ni->ni_probe_ticks   = 0;
    }

    /*
     * Check BBSID before updating our beacon configuration to make 
     * sure the received beacon really belongs to our Adhoc network.
     */
    if ((subtype == IEEE80211_FC0_SUBTYPE_BEACON) &&
        ieee80211_vap_ready_is_set(vap) &&
        (IEEE80211_ADDR_EQ(wh->i_addr3, ieee80211_node_get_bssid(vap->iv_bss)))) {
        /*
         * if the neighbor is not in the list, add it to the list.
         */
        if (ni == ni->ni_bss_node) {
            ni = ieee80211_add_neighbor(ni, scan_entry);
            if (ni == NULL) {
                return;
            }
            OS_MEMCPY(ni->ni_tstamp.data, &tsf, sizeof(ni->ni_tstamp));
            free_node = TRUE;
        }
        ni->ni_rssi = rs->rs_rssi;

        /*
         * record tsf of last beacon into bss node
         */
        OS_MEMCPY(ni->ni_bss_node->ni_tstamp.data, &tsf, sizeof(ni->ni_tstamp));

        /*
         * record absolute time of last beacon
         */
        vap->iv_last_beacon_time = OS_GET_TIMESTAMP();

        ic->ic_beacon_update(ni, rs->rs_rssi);

#if ATH_SUPPORT_IBSS_WMM
        /*
         * check for WMM parameters
         */
        if ((ieee80211_scan_entry_wmeparam_ie(scan_entry) != NULL) ||
            (ieee80211_scan_entry_wmeinfo_ie(scan_entry)  != NULL)) {

            /* Node is WMM-capable if WME IE (either subtype) is present */
            ni->ni_ext_caps |= IEEE80211_NODE_C_QOS;

            /* QOS-enable */
            ieee80211node_set_flag(ni, IEEE80211_NODE_QOS);
        } else {
            /* If WME IE not present node is not WMM capable */
            ni->ni_ext_caps &= ~IEEE80211_NODE_C_QOS;
            
            ieee80211node_clear_flag(ni, IEEE80211_NODE_QOS);
        }
#endif  
#if ATH_SUPPORT_IBSS_DFS

        if (ic->ic_curchan->ic_flagext & IEEE80211_CHAN_DFS) {
            //check and see if we need to update other info for ibss_dfsie
            ibssdfsie = (struct ieee80211_ibssdfs_ie *)ieee80211_scan_entry_ibssdfs_ie(scan_entry);
            if(ibssdfsie) {
               if (ieee80211_check_and_update_ibss_dfs(vap, ibssdfsie)) {
                   ieee80211_ibss_beacon_update_start(ic);
               }
            }

            /* update info from DFS owner */
            if(IEEE80211_ADDR_EQ(wh->i_addr2, vap->iv_ibssdfs_ie_data.owner)) {
                if (ibssdfsie) {
                    if(OS_MEMCMP(ibssdfsie, &vap->iv_ibssdfs_ie_data, MIN_IBSS_DFS_IE_CONTENT_SIZE)) {
                        IEEE80211_ADDR_COPY(vap->iv_ibssdfs_ie_data.owner, ibssdfsie->owner);
                        vap->iv_ibssdfs_ie_data.rec_interval = ibssdfsie->rec_interval;
                        ieee80211_ibss_beacon_update_start(ic);
                    }
                } else {
                    IEEE80211_ADDR_COPY(vap->iv_ibssdfs_ie_data.owner, vap->iv_myaddr);
                    vap->iv_ibssdfs_state = IEEE80211_IBSSDFS_OWNER;
                    ieee80211_ibss_beacon_update_start(ic);            
                }
            } 
        }
   
#endif /* ATH_SUPPORT_IBSS_DFS */

        /*
         * check for spectrum management
         */
        if (capinfo & IEEE80211_CAPINFO_SPECTRUM_MGMT) {
            chanie  = (struct ieee80211_channelswitch_ie *)         ieee80211_scan_entry_csa(scan_entry);
            echanie = (struct ieee80211_extendedchannelswitch_ie *) ieee80211_scan_entry_xcsa(scan_entry);
            if ((!chanie) && (!echanie)) {
                quiet_elm = ieee80211_scan_entry_quiet(scan_entry);
            }
        }
    }

    /*
     * Handle ibss merge as needed; check the tsf on the
     * frame before attempting the merge.  The 802.11 spec
     * says the station should change its bssid to match
     * the oldest station with the same ssid, where oldest
     * is determined by the tsf.  Note that hardware
     * reconfiguration happens through callback to
     * ath as the state machine will go from
     * RUN -> RUN when this happens.
     */
    if (ieee80211_vap_ready_is_set(vap) &&
        (le64_to_cpu(tsf) >= rs->rs_tstamp.tsf)) {
        ibssmerge = ieee80211_ibss_merge(ni,scan_entry);
    }

    if (ibssmerge) {
        ieee80211_mlme_adhoc_merge_start(ni);
    }

    /*
     * RNWF-specific: indicate to NDIS about the potential association.
     * It should be done after IBSS merge, which is called from ath_beacon_update().
     */
    if (IEEE80211_ADDR_EQ(wh->i_addr3, ieee80211_node_get_bssid(ni))) {
        /* Indicate node joined IBSS */
        ieee80211_mlme_adhoc_join_indication(ni, wbuf);

        /* notify mlme of beacon reception */
        ieee80211_mlme_join_complete_adhoc(ni);
    }

    if (ibssmerge) {
        ieee80211_mlme_adhoc_merge_completion(ni);
    }

    if (quiet_elm) {
        ic->ic_set_quiet(ni, quiet_elm);
    }

    if (free_node == TRUE) {
        ieee80211_free_node(ni);
    }

}

#endif
