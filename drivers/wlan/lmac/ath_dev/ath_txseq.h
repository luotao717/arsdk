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

/*
 * Definitions for the ATH layer internal API's.
 */
#ifndef ATH_TXSEQ_H
#define ATH_TXSEQ_H

#ifdef ATH_LMAC_TXSEQ

/* Function to set tx seq no for beacon frames */
static inline void
ath_beacon_seqno_set(struct ath_node *an, wbuf_t wbuf)
{
    struct ieee80211_frame *wh;
    wh = (struct ieee80211_frame *)wbuf_header(wbuf);
    *(u_int16_t *)&wh->i_seq[0] = 
        htole16(an->ni_txseqs[WME_MGMT_TID] << IEEE80211_SEQ_SEQ_SHIFT);
    an->ni_txseqs[WME_MGMT_TID]++;
}

/* Function to set tx seq no for Non-beacon frames */
static inline void
ath_tx_seqno_set(struct ath_node *an, u_int16_t tidno, wbuf_t wbuf, bool isqosdata, bool isctl)
{
    struct ieee80211_frame *wh;
    wh = (struct ieee80211_frame *)wbuf_header(wbuf);
    u_int16_t buf_seq;
    /* Do not set the seq no for ctl frames */
    if (isctl) {
        return;
    }
    if (!isqosdata) {
        tidno = WME_MGMT_TID;
    }
    buf_seq = ((an->ni_txseqs[tidno] << IEEE80211_SEQ_SEQ_SHIFT) |
               ((le16toh(*(u_int16_t *)&wh->i_seq[0]) >> IEEE80211_SEQ_FRAG_SHIFT) &
                 IEEE80211_SEQ_FRAG_MASK));
    *(u_int16_t *)&wh->i_seq[0] = htole16(buf_seq);
    an->ni_txseqs[tidno]++;
}

#else

#define ath_beacon_seqno_set(an, wbuf) /**/
#define ath_tx_seqno_set(an, tidno, wbuf, isqosdata, isctl)  /* */

#endif /* ATH_LMAC_TXSEQ */

#endif /* ATH_TXSEQ_H */
