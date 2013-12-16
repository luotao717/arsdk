/*
 *  Copyright (c) 2008 Atheros Communications Inc.  All rights reserved.
 */

#include "ieee80211_node_priv.h"
#include "ieee80211_wds.h"

struct ieee80211_node *
ieee80211_find_tdlsnode(struct ieee80211vap *vap, const u_int8_t *macaddr)
{
    struct ieee80211_node_table *nt = &vap->iv_ic->ic_sta;
    struct ieee80211_node *ni = NULL;

    ni = ieee80211_find_node(nt, macaddr);

    /* TDLS: search for destn from WDS Table.
     * return ni = AP's ref for nodes behind AP && First frame to TDLS node 
     *           = TDLS Node for subsequent frames to TDLS node 
     *           = NULL for no matching entries found 
     */
    if (ni == NULL) {
        ni = ieee80211_find_wds_node(nt, macaddr);
        //printk("tdls_ref: node referenced here %s: %d\n", __FUNCTION__, __LINE__);
    }
    
    /* Use the TDLS node, only if it is enabled and mode is STA*/
    if (ni) {
    	if(IEEE80211_IS_TDLS_NODE(ni) 
#ifdef CONFIG_RCPI
    	&& IEEE80211_IS_TDLSRCPI_PATH_SET(ni)
#endif /* CONFIG_RCPI */
    	) {
            IEEE80211_DPRINTF(vap, IEEE80211_MSG_TDLS,
                "Selecting node: %s ",ether_sprintf(macaddr));
            IEEE80211_DPRINTF(vap, IEEE80211_MSG_TDLS,
                "thro TDLS node: %s \n",ether_sprintf(ni->ni_macaddr));
			return ni;
        } else {
           ieee80211_free_node(ni);
          IEEE80211_DPRINTF(vap,IEEE80211_MSG_TDLS,"tdls_ref: node freed here %s: %d\n",
                             __FUNCTION__, __LINE__);
           ni = NULL; 
        }
    }

	return ni;
}
