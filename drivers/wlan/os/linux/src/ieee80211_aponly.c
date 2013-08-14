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

#include <ah.h>
#include "ath_internal.h"
#include "if_athrate.h"
#include "ratectrl.h"
#include "ath_aow.h"
#include "ath_internal.h"
#include <osdep.h>
#include <wbuf.h>
#include "../lmac/dfs/dfs.h"

#include "ieee80211_aponly.h"
#include "../lmac/ratectrl/ratectrl11n.h"
#include "if_athvar.h"
#include "ieee80211_txrx_priv.h"
#include "if_athproto.h"
#include "ath_cwm.h"
#include "if_ath_amsdu.h"
#include "if_ath_uapsd.h"
#include "if_ath_htc.h"
#include "if_llc.h"
#include "if_ath_aow.h"
 
#include "asf_amem.h"     /* asf_amem_setup */
#include "asf_print.h"    /* asf_print_setup */

#include "adf_os_mem.h"   /* adf_os_mem_alloc,free */
#include "adf_os_lock.h"  /* adf_os_spinlock_* */
#include "adf_os_types.h" /* adf_os_vprint */
#include "ath_internal.h" /* */

#include "osif_private.h"

#if UMAC_SUPPORT_APONLY
#if ATH_SUPPORT_VOWEXT
#include "ratectrl11n.h"
#endif

#ifndef REMOVE_PKT_LOG
#include "pktlog.h"
extern struct ath_pktlog_funcs *g_pktlog_funcs;
#endif

#ifndef ARRAY_LEN
#define ARRAY_LEN(x) (sizeof(x)/sizeof((x)[0]))
#endif
extern void bus_dma_sync_single(void *hwdev,
			dma_addr_t dma_handle,
			size_t size, int direction);


/*
 * To be included by ath_xmit.c so that  we can inlise some of the functions
 * for performance reasons.
 */
#if ATH_SUPPORT_HT
//copied from another file, _ht.c; we need create a common header
#define ADDBA_EXCHANGE_ATTEMPTS     10
#define ADDBA_TIMEOUT               200 /* 200 milliseconds */

/* Typical MPDU Length, used for all the rate control computations */
#define MPDU_LENGTH                 1544
#endif

#if ATH_VOW_EXT_STATS
void ath_add_ext_stats(struct ath_rx_status *rxs, wbuf_t wbuf, struct ath_softc *sc, 
                       struct ath_phy_stats *phy_stats, ieee80211_rx_status_t *rx_status);
#endif

#ifdef ATH_AMSDU

extern int ieee80211_8023frm_amsdu_check(wbuf_t wbuf);

extern void
ath_amsdu_stageq_flush(struct ath_softc_net80211 *scn, struct ath_amsdu_tx *amsdutx);

extern int
ieee80211_amsdu_check(struct ieee80211vap *vap, wbuf_t wbuf);

inline int
ath_get_amsdusupported_aponly(ath_dev_t dev, ath_node_t node, int tidno)
{
    struct ath_softc *sc = ATH_DEV_TO_SC(dev);
    struct ath_node *an = ATH_NODE(node);
    ath_atx_tid_t *tid = ATH_AN_2_TID(an, tidno);

    if (sc->sc_txamsdu && tid->addba_exchangecomplete) {
        return (tid->addba_amsdusupported);
    }

    return (FALSE);
}

#define ATH_RATE_OUT(x)            (((x) != ATH_RATE_DUMMY_MARKER) ? (ATH_EP_RND((x), ATH_RATE_EP_MULTIPLIER)) : ATH_RATE_DUMMY_MARKER)
/*
 * If the receive phy rate is lower than the threshold or our transmit queue has at least one
 * frame to work on, we keep building the AMSDU.
 */
static inline int
ath_amsdu_sched_check_aponly(struct ath_softc_net80211 *scn, struct ath_node_net80211 *anode,
                      int priority)
{
    if ((ATH_RATE_OUT(anode->an_avgrxrate) <= 162000) ||
        (scn->sc_ops->txq_depth(scn->sc_dev, scn->sc_ac2q[priority]) >= 1))
    {
        return 1;
    }
    return 0;
}

static inline int
ieee80211_amsdu_check_aponly(struct ieee80211vap *vap, wbuf_t wbuf)
{
    if (wbuf_is_uapsd(wbuf) || wbuf_is_moredata(wbuf)) {
        return 1;
    }
 
    return ieee80211_8023frm_amsdu_check(wbuf);
}


static inline wbuf_t
ath_amsdu_send_aponly(wbuf_t wbuf)
{
    struct ieee80211_node *ni = wbuf_get_node(wbuf);
    struct ieee80211com *ic = ni->ni_ic;
    struct ath_softc_net80211 *scn = ATH_SOFTC_NET80211(ic);
    struct ath_node_net80211 *anode = (struct ath_node_net80211 *)ni;
    struct ieee80211vap *vap = ni->ni_vap;
    wbuf_t amsdu_wbuf;
    u_int8_t tidno = wbuf_get_tid(wbuf);
    u_int32_t framelen;
    struct ath_amsdu_tx *amsdutx;
    int amsdu_deny;
    struct ieee80211_tx_status ts;
    
   
    /* AMSDU handling not initialized */
    if (unlikely(anode->an_amsdu == NULL)) {
        printk("ERROR: ath_amsdu_attach not called\n");
        return wbuf;
    }
    if (unlikely(ieee80211vap_get_fragthreshold(vap) < 2346 ||
        !(ieee80211vap_get_opmode(vap) == IEEE80211_M_STA ||
        ieee80211vap_get_opmode(vap) == IEEE80211_M_HOSTAP)))
    {
        return wbuf;
    }
    framelen = roundup(wbuf_get_pktlen(wbuf), 4);
    amsdu_deny = ieee80211_amsdu_check_aponly(vap, wbuf);
    /* Set the tx status flags */
    ts.ts_flags = 0;
    ts.ts_retries = 0;
#ifdef ATH_SUPPORT_TxBF
    ts.ts_txbfstatus=0;
#endif
    ATH_AMSDU_TXQ_LOCK(scn);
    amsdutx = &(anode->an_amsdu->amsdutx[tidno]);
    if (amsdutx->amsdu_tx_buf) {
        /* If AMSDU staging buffer exists, we need to add this wbuf
         * to it.
         */
        amsdu_wbuf = amsdutx->amsdu_tx_buf;
        /*
         * Is there enough room in the AMSDU staging buffer?
         * Is the newly arrived wbuf larger than our amsdu limit?
         * If not, dispatch the AMSDU and return the current wbuf.
         */
        if ((framelen > AMSDU_MAX_SUBFRM_LEN) || amsdu_deny) {
            /* Flush the amsdu q */
            ATH_AMSDU_TXQ_UNLOCK(scn);
            ath_amsdu_stageq_flush(scn, amsdutx);
            return wbuf;
        }
        if (ath_amsdu_sched_check_aponly(scn, anode, wbuf_get_priority(amsdu_wbuf)) &&
            (AMSDU_MAX_SUBFRM_LEN + framelen + wbuf_get_pktlen(amsdu_wbuf)) < ic->ic_amsdu_max_size)
        {
            /* We are still building the AMSDU */
            ieee80211_amsdu_encap(vap,amsdu_wbuf, wbuf, framelen, 0);
            ATH_AMSDU_TXQ_UNLOCK(scn);
            /* Free the tx buffer */
            ieee80211_complete_wbuf(wbuf, &ts);
            return NULL;
        } else {
            /*
             * This is the last wbuf to be added to the AMSDU
             * No pad for this frame.
             * Return the AMSDU wbuf back.
             */
            ieee80211_amsdu_encap(vap,amsdu_wbuf, wbuf, wbuf_get_pktlen(wbuf), 0);
            ATH_AMSDU_TXQ_UNLOCK(scn);
            /* Free the tx buffer */
            ieee80211_complete_wbuf(wbuf, &ts);
            ath_amsdu_stageq_flush(scn, amsdutx);
            return NULL;
        }
    } else {
        /* Begin building the AMSDU */
        /* AMSDU for small frames only */
        if ((framelen > AMSDU_MAX_SUBFRM_LEN) || amsdu_deny) {
            ATH_AMSDU_TXQ_UNLOCK(scn);
            return wbuf;
        }
        amsdu_wbuf = wbuf_alloc(scn->sc_osdev, WBUF_TX_DATA, AMSDU_MAX_BUFFER_SIZE);
        /* No AMSDU buffer available */
        if (amsdu_wbuf == NULL) {
            ATH_AMSDU_TXQ_UNLOCK(scn);
            return wbuf;
        }
        /* Perform 802.11 AMSDU encapsulation */
        ieee80211_amsdu_encap(vap,amsdu_wbuf, wbuf, framelen, 1);
        /*
         * Copy information from buffer
         * Bump reference count for the node.
         */
        wbuf_set_priority(amsdu_wbuf, wbuf_get_priority(wbuf));
        wbuf_set_tid(amsdu_wbuf, tidno);
        wbuf_set_node(amsdu_wbuf, ieee80211_ref_node(ni));
        wbuf_set_amsdu(amsdu_wbuf);
        amsdutx->amsdu_tx_buf = amsdu_wbuf;
        if (!amsdutx->sched) {
            amsdutx->sched = 1;
            TAILQ_INSERT_TAIL(&scn->sc_amsdu_txq, amsdutx, amsdu_qelem);
        }
        ATH_AMSDU_TXQ_UNLOCK(scn);
        /* Free the tx buffer */
        ieee80211_complete_wbuf(wbuf, &ts);
        if (!ath_timer_is_active(&scn->sc_amsdu_flush_timer))
            ath_start_timer(&scn->sc_amsdu_flush_timer);
        return NULL;
    }
}
#endif

static INLINE void
ath_tx_set_retry_aponly(struct ath_softc *sc, struct ath_buf *bf)
{
    wbuf_t wbuf;
    struct ieee80211_frame *wh;

    __11nstats(sc, tx_retries);

    bf->bf_isretried = 1;
    bf->bf_retries ++;

    wbuf = bf->bf_mpdu;
    wh = (struct ieee80211_frame *)wbuf_header(wbuf);
    wh->i_fc[1] |= IEEE80211_FC1_RETRY;
}



/*
 * Update block ack window
 */
static inline void
ath_tx_update_baw_aponly(struct ath_softc *sc, struct ath_atx_tid *tid, int seqno)
{
    int index, cindex;

    __11nstats(sc, tx_bawupdates);

    index  = ATH_BA_INDEX(tid->seq_start, seqno);
    cindex = (tid->baw_head + index) & (ATH_TID_MAX_BUFS - 1);

    TX_BUF_BITMAP_CLR(tid->tx_buf_bitmap, cindex);

    while (tid->baw_head != tid->baw_tail &&
           !TX_BUF_BITMAP_IS_SET(tid->tx_buf_bitmap, tid->baw_head)) {
        __11nstats(sc, tx_bawupdtadv);
        INCR(tid->seq_start, IEEE80211_SEQ_MAX);
        INCR(tid->baw_head, ATH_TID_MAX_BUFS);
    }
}

/*
 * queue up a dest/ac pair for tx scheduling
 * NB: must be called with txq lock held
 */
static INLINE void
ath_tx_queue_tid_aponly(struct ath_txq *txq, struct ath_atx_tid *tid)
{
    struct ath_atx_ac *ac = tid->ac;

    /*
     * if tid is paused, hold off
     */
    if (tid->paused)
        return;

    /*
     * add tid to ac atmost once
     */
    if (tid->sched)
        return;

#ifdef VOW_TIDSCHED
    if (TAILQ_EMPTY(&tid->buf_q))
        return;
#endif

    tid->sched = AH_TRUE;
    TAILQ_INSERT_TAIL(&ac->tid_q, tid, tid_qelem);

#ifdef VOW_TIDSCHED
    if(tid->ac->qnum<HAL_NUM_DATA_QUEUES) {
      ath_tid_cqw(sc,tid);
    }
#endif
    
    /*
     * add node ac to txq atmost once
     */
    if (ac->sched)
        return;

    ac->sched = AH_TRUE;
    TAILQ_INSERT_TAIL(&txq->axq_acq, ac, ac_qelem);
}





static void
ath_bar_tx_aponly(struct ath_softc *sc, struct ath_node *an, struct ath_atx_tid *tid)
{
    __11nstats(sc, tx_bars);

    /* pause TID until BAR completes */
    ATH_TX_PAUSE_TID(sc, tid);

    if (sc->sc_ieee_ops->send_bar) {
        if (sc->sc_ieee_ops->send_bar(an->an_node, tid->tidno, tid->seq_start)) {
            /* resume tid if send bar failed. */
            ATH_TX_RESUME_TID(sc, tid);
        }
    }
}



/*
 * Completion routine of an aggregate
 */
static inline void
ath_tx_complete_aggr_rifs_aponly(struct ath_softc *sc, struct ath_txq *txq, struct ath_buf *bf,
                          ath_bufhead *bf_q, struct ath_tx_status *ts, int txok)
{
    struct ath_node *an = bf->bf_node;
    struct ath_atx_tid *tid = ATH_AN_2_TID(an, bf->bf_tidno);
    struct ath_buf *bf_last = bf->bf_lastbf;
#if ATH_SUPPORT_IQUE
    struct ath_node *tan;
#endif
    struct ath_buf *bf_next, *bf_lastq = NULL;
    ath_bufhead bf_head, bf_pending;
    u_int16_t seq_st = 0;
    u_int32_t ba[WME_BA_BMP_SIZE >> 5];
    int isaggr, txfail, txpending, sendbar = 0, needreset = 0;
    int isnodegone= (an->an_flags & ATH_NODE_CLEAN);
    u_int sw_retry_limit = ATH_MAX_SW_RETRIES;

#ifdef ATH_RIFS
    int isrifs = 0;
    struct ath_buf *bar_bf = NULL;
#endif

    /* defer completeion on  atleast one buffer   */
    struct _defer_completion {
            struct ath_buf *bf;
            ath_bufhead bf_head;
            u_int8_t txfail;
    } defer_completion;

    OS_MEMZERO(&defer_completion, sizeof(defer_completion));

    OS_MEMZERO(ba, WME_BA_BMP_SIZE >> 3);

    isaggr = bf->bf_isaggr;
#ifdef ATH_RIFS
    isrifs = (ATH_RIFS_SUBFRAME_FIRST == bf->bf_rifsburst_elem) ? 1:0;


    if (isrifs) {
        bar_bf = bf->bf_lastbf;
        ASSERT(ATH_RIFS_BAR == bar_bf->bf_rifsburst_elem);
    }

    if (likely(isaggr || isrifs))
#else
    if (likely(isaggr))
#endif
    {
#ifdef ATH_RIFS
        isrifs ? __11nstats(sc, tx_comprifs) : __11nstats(sc, tx_compaggr);
#else
        __11nstats(sc, tx_compaggr);
#endif
        if (likely(txok)) {
            if (likely(ATH_DS_TX_BA(ts))) {
                /*
                 * extract starting sequence and block-ack bitmap
                 */
                seq_st = ATH_DS_BA_SEQ(ts);
                OS_MEMCPY(ba, ATH_DS_BA_BITMAP(ts), WME_BA_BMP_SIZE >> 3);
            } else {
#ifdef ATH_RIFS
                isrifs ? __11nstats(sc, txrifs_babug) :
                         __11nstats(sc, txaggr_babug);
#else
                __11nstats(sc, txaggr_babug);
#endif
                DPRINTF(sc, ATH_DEBUG_TX_PROC, "%s: BA bit not set.\n", __func__);

                /*
                 * Owl can become deaf/mute when BA bug happens.
                 * Chip needs to be reset. See bug 32789.
                 */
                needreset = 1;
            }
        } 
    }

    TAILQ_INIT(&bf_pending);

#ifdef ATH_RIFS
    while (bf && bf->bf_rifsburst_elem != ATH_RIFS_BAR)
#else
    while (bf)
#endif
    {
        txfail = txpending = 0;
        bf_next = bf->bf_next;

        if (ATH_BA_ISSET(ba, ATH_BA_INDEX(seq_st, bf->bf_seqno))) {
            /*
             * transmit completion, subframe is acked by block ack
             */
#ifdef ATH_RIFS
            isrifs ? __11nstats(sc, txrifs_compgood) :
                     __11nstats(sc, txaggr_compgood);
#else
            __11nstats(sc, txaggr_compgood);
#endif
        }
#ifdef ATH_RIFS
        else if ((!isaggr && !isrifs) && txok)
#else
        else if (!isaggr && txok)
#endif
        {
            /*
             * transmit completion
             */
#ifdef ATH_RIFS
            isrifs ? __11nstats(sc, tx_compnorifs) :
                     __11nstats(sc, tx_compunaggr);
#else
            __11nstats(sc, tx_compunaggr);
#endif
        } else {
            /*
             * retry the un-acked ones
             */
            if (ts->ts_flags & HAL_TXERR_XRETRY) {
                __11nstats(sc,tx_sf_hw_xretries);
            }

#ifdef ATH_RIFS
            isrifs ? __11nstats(sc, txrifs_compretries) :
                     __11nstats(sc, txaggr_compretries);
#else
            __11nstats(sc, txaggr_compretries);
#endif

#if ATH_SUPPORT_IQUE
            /* For the frames to be droped who block the headline of the AC_VI queue,
             * these frames should not be sw-retried. So mark them as already xretried.
             */
            tan = ATH_NODE(bf->bf_node);
            if (sc->sc_ieee_ops->get_hbr_block_state(tan->an_node) &&             
                            TID_TO_WME_AC(bf->bf_tidno) == WME_AC_VI) {
                bf->bf_retries = sw_retry_limit;
            }
#endif      

            if (likely(!tid->cleanup_inprogress && !isnodegone &&
                !bf_last->bf_isswaborted)) {

                if (ATH_ENAB_AOW(sc) && (TID_TO_WME_AC(bf->bf_tidno) == WME_AC_VO))
                {
                    sw_retry_limit = ATH_SW_RETRY_LIMIT(sc);
                }

                if ((bf->bf_retries < sw_retry_limit) || (atomic_read(&sc->sc_in_reset))) {
                    ath_tx_set_retry_aponly(sc, bf);


                    txpending = 1;
                } else {
                    __11nstats(sc, tx_xretries);
                    bf->bf_isxretried = 1;
                    txfail = 1;
                    sendbar = tid->addba_exchangecomplete;
                    DPRINTF(sc, ATH_DEBUG_TX_PROC, "%s drop tx frame tid %d bf_seqno %d\n", __func__, tid->tidno, bf->bf_seqno);
                }
            } else {
                /*
                 * the entire aggregate is aborted by software due to
                 * reset, channel change, node left and etc.
                 */
                if (bf_last->bf_isswaborted) {
                    __11nstats(sc, txaggr_comperror);
                }

                /*
                 * cleanup in progress, just fail
                 * the un-acked sub-frames
                 */
                txfail = 1;
            }
        }

        /*
         * Remove ath_buf's of this sub-frame from aggregate queue.
         */
        if (unlikely(bf_next == NULL && !sc->sc_enhanceddmasupport)) {  /* last subframe in the aggregate */
            ASSERT(bf->bf_lastfrm == bf_last);

            /*
             * The last descriptor of the last sub frame could be a holding descriptor
             * for h/w. If that's the case, bf->bf_lastfrm won't be in the bf_q.
             * Make sure we handle bf_q properly here.
             */
            bf_lastq = TAILQ_LAST(bf_q, ath_bufhead_s);
            if (bf_lastq) {
                TAILQ_REMOVE_HEAD_UNTIL(bf_q, &bf_head, bf_lastq, bf_list);
            } else {
                /*
                 * XXX: if the last subframe only has one descriptor which is also being used as
                 * a holding descriptor. Then the ath_buf is not in the bf_q at all.
                 */
                ASSERT(TAILQ_EMPTY(bf_q));
                TAILQ_INIT(&bf_head);
            }
        } else {
            ASSERT(!TAILQ_EMPTY(bf_q));
            TAILQ_REMOVE_HEAD_UNTIL(bf_q, &bf_head, bf->bf_lastfrm, bf_list);
        }

#ifndef REMOVE_PKT_LOG
        /* do pktlog */
        {
            struct log_tx log_data;
            struct ath_buf *tbf;

            TAILQ_FOREACH(tbf, &bf_head, bf_list) {
                log_data.firstds = tbf->bf_desc;
                log_data.bf = tbf;
                ath_log_txctl(sc, &log_data, 0);
            }

            if (bf->bf_next == NULL &&
                bf_last->bf_status & ATH_BUFSTATUS_STALE) {
                log_data.firstds = bf_last->bf_desc;
                log_data.bf = bf_last;
                ath_log_txctl(sc, &log_data, 0);
            }
        }
#endif

        if (!txpending) {
            /*
             * complete the acked-ones/xretried ones; update block-ack window
             */
            ATH_TXQ_LOCK(txq);
            ath_tx_update_baw_aponly(sc, tid, bf->bf_seqno);
            if (unlikely((isnodegone) && (tid->cleanup_inprogress))) {
                if (tid->baw_head == tid->baw_tail) {
                    tid->addba_exchangecomplete = 0;
                    tid->addba_exchangeattempts = 0;
                    tid->addba_exchangestatuscode = IEEE80211_STATUS_UNSPECIFIED;
                    /* resume the tid */
                    tid->paused--;
                    __11nstats(sc, tx_tidresumed);
                    tid->cleanup_inprogress = AH_FALSE;
                }
            }
            ATH_TXQ_UNLOCK(txq);

            if( defer_completion.bf ) {
#ifdef ATH_SUPPORT_TxBF
                ath_tx_complete_buf(sc, defer_completion.bf, &defer_completion.bf_head, !defer_completion.txfail, ts->ts_txbfstatus, ts->ts_tstamp);
#else
                ath_tx_complete_buf(sc, defer_completion.bf, &defer_completion.bf_head, !defer_completion.txfail);
#endif
            } 
            /* 
             * save this sub-frame to be completed at the end. this
             * will keep the node referenced till the end of the function
             * and prevent acces to the node memory after it is freed (note tid is part of node).
             */
            defer_completion.bf = bf;
            defer_completion.txfail = txfail;
            if (!TAILQ_EMPTY(&bf_head)) {
                defer_completion.bf_head = bf_head;
                TAILQ_INIT(&bf_head);
            } else {
                TAILQ_INIT(&defer_completion.bf_head);
            }
        } else {
            /*
             * retry the un-acked ones
             */
            if (unlikely(!sc->sc_enhanceddmasupport)) { /* holding descriptor support for legacy */
                /*
                 * XXX: if the last descriptor is holding descriptor, in order to requeue
                 * the frame to software queue, we need to allocate a new descriptor and
                 * copy the content of holding descriptor to it.
                 */
                if (bf->bf_next == NULL &&
                    bf_last->bf_status & ATH_BUFSTATUS_STALE) {
                    struct ath_buf *tbf;
                    int nmaps;

                    /* allocate new descriptor */
                    ATH_TXBUF_LOCK(sc);
                    tbf = TAILQ_FIRST(&sc->sc_txbuf);
                    if (tbf == NULL) {
                        /*
                         * We are short on memory, release the wbuf
                         * and bail out.
                         * Complete the packet with status *Not* OK.
                         */
                        ATH_TXBUF_UNLOCK(sc);

                        ATH_TXQ_LOCK(txq);
                        ath_tx_update_baw_aponly(sc, tid, bf->bf_seqno);
                        ATH_TXQ_UNLOCK(txq);

                        if( defer_completion.bf ) {
#ifdef ATH_SUPPORT_TxBF
                            ath_tx_complete_buf(sc, defer_completion.bf, &defer_completion.bf_head, !defer_completion.txfail,ts->ts_txbfstatus, ts->ts_tstamp);
#else
                            ath_tx_complete_buf(sc, defer_completion.bf, &defer_completion.bf_head, !defer_completion.txfail);
#endif
                        } 
                        /* 
                         * save this sub-frame to be completed later
                         * this is a  holding buffer, we do not want  to return this to
                         * the free list yet. clear the bf_head so that the ath_tx_complete_buf  will
                         * not return any thing to the sc_txbuf.
                         * also mark this subframe as an error.
                         * since the bf_head in cleared, ath_tx_complete_buf will
                         * just complete the wbuf for this subframe and will not return any  
                         * ath bufs to free list. 
                         */
                        defer_completion.bf = bf;
                        defer_completion.txfail = 1;
                        TAILQ_INIT(&defer_completion.bf_head);

                        // At this point, bf_next is NULL: We are done with this aggregate.
                        break;
                    }
                    TAILQ_REMOVE(&sc->sc_txbuf, tbf, bf_list);
                    if (tbf) {
	                    txq->axq_num_buf_used++;
                        sc->sc_txbuf_free--;
                    }
                    ATH_TXBUF_UNLOCK(sc);

                    ATH_TXBUF_RESET(tbf, sc->sc_num_txmaps);

                    /* copy descriptor content */
                    tbf->bf_mpdu = bf_last->bf_mpdu;
                    tbf->bf_node = bf_last->bf_node;
#ifndef REMOVE_PKT_LOG
                    tbf->bf_vdata = bf_last->bf_vdata;
#endif
                    for (nmaps = 0; nmaps < sc->sc_num_txmaps; nmaps++) {
                        tbf->bf_buf_addr[nmaps] = bf_last->bf_buf_addr[nmaps];
                        tbf->bf_buf_len[nmaps] = bf_last->bf_buf_len[nmaps];
                        tbf->bf_avail_buf--;
                    }
                    memcpy(tbf->bf_desc, bf_last->bf_desc, sc->sc_txdesclen);

                    /* link it to the frame */
                    if (bf_lastq) {
           	            ath_hal_setdesclink(sc->sc_ah, bf_lastq->bf_desc, tbf->bf_daddr);
                        bf->bf_lastfrm = tbf;
                        ath_hal_cleartxdesc(sc->sc_ah, bf->bf_lastfrm->bf_desc);
                    } else {
                        tbf->bf_state = bf_last->bf_state;
                        tbf->bf_lastfrm = tbf;
                        ath_hal_cleartxdesc(sc->sc_ah, tbf->bf_lastfrm->bf_desc);

                        /* copy the DMA context */
                        OS_COPY_DMA_MEM_CONTEXT(OS_GET_DMA_MEM_CONTEXT(tbf, bf_dmacontext),
                                                OS_GET_DMA_MEM_CONTEXT(bf_last, bf_dmacontext));
                    }
                    TAILQ_INSERT_TAIL(&bf_head, tbf, bf_list);
                } else {
                    /*
                     * Clear descriptor status words for software retry
                     */
                    ath_hal_cleartxdesc(sc->sc_ah, bf->bf_lastfrm->bf_desc);
                }
            }

            /*
             * Put this buffer to the temporary pending queue to retain ordering
             */
            TAILQ_CONCAT(&bf_pending, &bf_head, bf_list);
        }

        bf = bf_next;
    }

    /*
     * node is already gone. no more assocication
     * with the node. the node might have been freed
     * any  node acces can result in panic.note tid
     * is part of the node. 
     */  
    if (isnodegone) goto done;

    if (unlikely(tid->cleanup_inprogress)) {
        /* check to see if we're done with cleaning the h/w queue */
        ATH_TXQ_LOCK(txq);

        if (tid->baw_head == tid->baw_tail) {
            tid->addba_exchangecomplete = 0;
            tid->addba_exchangeattempts = 0;
            tid->addba_exchangestatuscode = IEEE80211_STATUS_UNSPECIFIED;

            ath_wmi_aggr_enable((ath_dev_t) sc, an, tid->tidno, 0);

            ATH_TXQ_UNLOCK(txq);
            
            tid->cleanup_inprogress = AH_FALSE;
 
            /* send buffered frames as singles */
            ATH_TX_RESUME_TID(sc, tid);
        } else {
            ATH_TXQ_UNLOCK(txq);
        }

        goto done;
    }
    
    if (unlikely(sendbar))
        ath_bar_tx_aponly(sc, an, tid);

#ifdef ATH_RIFS
    if (unlikely(isrifs))
        ath_rifsburst_bar_buf_free(sc, bar_bf);
#endif
    /*
     * prepend un-acked frames to the beginning of the pending frame queue
     */
    if (!TAILQ_EMPTY(&bf_pending)) {
        
#ifdef ATH_RIFS
        isrifs ? __11nstats(sc, txrifs_prepends) :
                 __11nstats(sc, txaggr_prepends);
#else
        __11nstats(sc, txaggr_prepends);
#endif
        
        ATH_TXQ_LOCK(txq);

        TAILQ_INSERTQ_HEAD(&tid->buf_q, &bf_pending, bf_list);

        ath_tx_queue_tid_aponly(txq, tid);
        ATH_TXQ_UNLOCK(txq);
    }

done:
    /*
     * complete the defrred buffer. 
     * at this point the associated node could be freed.
     */
    if (defer_completion.bf) {
#ifdef ATH_SUPPORT_TxBF
        ath_tx_complete_buf(sc, defer_completion.bf, &defer_completion.bf_head, !defer_completion.txfail,ts->ts_txbfstatus, ts->ts_tstamp);
#else
        ath_tx_complete_buf(sc, defer_completion.bf, &defer_completion.bf_head, !defer_completion.txfail);
#endif
    }

    return;
}

/*
 * Process completed xmit descriptors from the specified queue.
 */
int
ath_tx_processq_aponly(struct ath_softc *sc, struct ath_txq *txq)
{
#define PA2DESC(_sc, _pa)	\
		((struct ath_desc*)((caddr_t)(_sc)->sc_txdma.dd_desc +	\
				((_pa) - (_sc)->sc_txdma.dd_desc_paddr)))
    struct ath_hal *ah = sc->sc_ah;
    struct ath_buf *bf, *lastbf, *bf_held = NULL;
    ath_bufhead bf_head;
    struct ath_desc *ds;
    struct ath_node *an;
    HAL_STATUS status;
	struct ath_tx_status txstat;
#ifdef ATH_SUPPORT_UAPSD
    int uapsdq = 0;
#endif
    int nacked;
    int txok, nbad = 0;
    int isrifs = 0;
#ifdef ATH_SWRETRY
    struct ath_swretry_info *pInfo;
    struct ieee80211_frame  *wh;
    struct ath_buf *lfbf = NULL;
#endif
#if ATH_SUPPORT_VOWEXT
    u_int8_t n_head_fail = 0;
    u_int8_t n_tail_fail = 0;
#endif

#if ATH_SUPPORT_CFEND
    int cfendq = 0;
#endif
    

    if (unlikely(txq == sc->sc_uapsdq)) {
        DPRINTF(sc, ATH_DEBUG_UAPSD, "%s: reaping U-APSD txq\n", __func__);
#ifdef ATH_SUPPORT_UAPSD
        uapsdq = 1;
#endif
    }

    nacked = 0;
    for (;;) {
        ATH_TXQ_LOCK(txq);
        txq->axq_intrcnt = 0; /* reset periodic desc intr count */
        bf = TAILQ_FIRST(&txq->axq_q);
        if (bf == NULL) {
            txq->axq_link = NULL;
            txq->axq_linkbuf = NULL;
            ATH_TXQ_UNLOCK(txq);
            break;
        }

        /*
         * There is a race condition that DPC gets scheduled after sw writes TxE
         * and before hw re-load the last descriptor to get the newly chained one.
         * Software must keep the last DONE descriptor as a holding descriptor -
         * software does so by marking it with the STALE flag.
         */
        bf_held = NULL;
        if (bf->bf_status & ATH_BUFSTATUS_STALE) {
            bf_held = bf;
            bf = TAILQ_NEXT(bf_held, bf_list);
            if (bf == NULL) {
                ATH_TXQ_UNLOCK(txq);
                break;
            }
        }

        isrifs = (ATH_RIFS_SUBFRAME_FIRST == bf->bf_rifsburst_elem) ? 1 : 0;
        lastbf = bf->bf_lastbf;
        ds = lastbf->bf_desc;    /* NB: last decriptor */
		OS_SYNC_SINGLE(sc->sc_osdev, lastbf->bf_daddr, sc->sc_txdesclen, BUS_DMA_FROMDEVICE, NULL);

        status = ath_hal_txprocdesc(ah, ds);

        if (status == HAL_EINPROGRESS) {
            ATH_TXQ_UNLOCK(txq);
            break;
        }
        if (bf->bf_desc == txq->axq_lastdsWithCTS) {
            txq->axq_lastdsWithCTS = NULL;
        }
        if (ds == txq->axq_gatingds) {
            txq->axq_gatingds = NULL;
        }

		txstat = ds->ds_txstat; /* use a local cacheable copy to improce d-cache efficiency */
        /*
         * Remove ath_buf's of the same transmit unit from txq,
         * however leave the last descriptor back as the holding
         * descriptor for hw.
         */
        lastbf->bf_status |= ATH_BUFSTATUS_STALE;
        ATH_TXQ_MOVE_HEAD_BEFORE(txq, &bf_head, lastbf, bf_list);

        if (bf->bf_isaggr) {
            txq->axq_aggr_depth--;
        }

        txok = (txstat.ts_status == 0);

        /* workaround for Hardware corrupted TX TID
        * There are two ways we can handle this situation, either we
        * go over a ath_reset_internal path OR
        * corrupt the tx status in such a way that entire aggregate gets
        * re-transmitted, taking the second approach here.
        * Corrupt both tx_status and clear the ba bitmap.
        */

        if (unlikely(bf->bf_isaggr && txok && bf->bf_tidno != txstat.tid)) {
            txstat.ts_status |= HAL_TXERR_BADTID;
            txok = !txok;
            txstat.ba_low = txstat.ba_high = 0x0;
            DPRINTF(sc, ATH_DEBUG_XMIT,
                "%s:%d identified bad tid status (buf:desc  %d:%d)\n",
                __func__, __LINE__,bf->bf_tidno, txstat.tid);
        }

        ATH_TXQ_UNLOCK(txq);

        /* Put the old holding descriptor to the free queue */
        if (bf_held) {
            TAILQ_REMOVE(&bf_head, bf_held, bf_list);
#ifdef ATH_SUPPORT_UAPSD
            if (bf_held->bf_qosnulleosp) {
                ATH_UAPSD_LOCK_IRQ(sc);
                TAILQ_INSERT_TAIL(&sc->sc_uapsdqnulbf, bf_held, bf_list);
                sc->sc_uapsdqnuldepth--;
                ATH_UAPSD_UNLOCK_IRQ(sc);
            } else
#endif
            {
                ATH_TXBUF_LOCK(sc);
                txq->axq_num_buf_used--;
                sc->sc_txbuf_free++;
	            TAILQ_INSERT_TAIL(&sc->sc_txbuf, bf_held, bf_list);
                ATH_TXBUF_UNLOCK(sc);
            }
        }

        an = bf->bf_node;
        if (likely(an != NULL)) {
            int noratectrl;
            noratectrl = an->an_flags & (ATH_NODE_CLEAN | ATH_NODE_PWRSAVE);

            ath_tx_update_stats(sc, bf, txq->axq_qnum, &txstat);

            /*
             * Hand the descriptor to the rate control algorithm
             * if the frame wasn't dropped for filtering or sent
             * w/o waiting for an ack.  In those cases the rssi
             * and retry counts will be meaningless.
             */
            if (unlikely(! bf->bf_isampdu)) {
                /*
                 * This frame is sent out as a single frame. Use hardware retry
                 * status for this frame.
                 */
                bf->bf_retries = txstat.ts_longretry;
                if (txstat.ts_status & HAL_TXERR_XRETRY) {
                      __11nstats(sc,tx_sf_hw_xretries);
                     bf->bf_isxretried = 1;
                }
                nbad = 0;
#if ATH_SUPPORT_VOWEXT
                if ( ATH_IS_VOWEXT_AGGRSIZE_ENABLED(sc) || ATH_IS_VOWEXT_RCA_ENABLED(sc)) 
                    n_head_fail = n_tail_fail = 0;
#endif
            } else {
                nbad = ath_tx_num_badfrms(sc, bf, &txstat, txok);
#if ATH_SUPPORT_VOWEXT
                if ( ATH_IS_VOWEXT_AGGRSIZE_ENABLED(sc) || ATH_IS_VOWEXT_RCA_ENABLED(sc)) {
                    n_tail_fail = (nbad & 0xFF);
                    n_head_fail = ((nbad >> 8) & 0xFF);
                    nbad = ((nbad >> 16) & 0xFF);
                }
#endif
            }

            if (likely((txstat.ts_status & HAL_TXERR_FILT) == 0 &&
                (bf->bf_flags & HAL_TXDESC_NOACK) == 0)) {
                /*
                 * If frame was ack'd update the last rx time
                 * used to workaround phantom bmiss interrupts.
                 */
                if (txstat.ts_status == 0)
                    nacked++;

                if (likely(bf->bf_isdata && !noratectrl && !bf->bf_useminrate)) {
					if (isrifs)			
				        OS_SYNC_SINGLE(sc->sc_osdev, bf->bf_rifslast->bf_daddr, 
						        sc->sc_txdesclen, BUS_DMA_FROMDEVICE, NULL);

#if ATH_SUPPORT_VOWEXT
                    ath_rate_tx_complete(sc, an,
                                         isrifs ? bf->bf_rifslast->bf_desc : ds,
                                         bf->bf_rcs, TID_TO_WME_AC(bf->bf_tidno),
                                         bf->bf_nframes, nbad,
                                         n_head_fail ,
                                         n_tail_fail,
                                         ath_tx_get_rts_retrylimit(sc, txq));
#else
                    ath_rate_tx_complete(sc,
                                         an,
                                         isrifs ? bf->bf_rifslast->bf_desc : ds,
                                         bf->bf_rcs,
                                         TID_TO_WME_AC(bf->bf_tidno),
                                         bf->bf_nframes,
                                         nbad, 
                                         ath_tx_get_rts_retrylimit(sc, txq));
#endif
                }
            }

            /*
             * Complete this transmit unit
             *
             * Node cannot be referenced past this point since it can be freed
             * here.
             */
            if (likely(bf->bf_isampdu)) {
                if (unlikely(txstat.ts_flags & HAL_TX_DESC_CFG_ERR))
                    __11nstats(sc, txaggr_desc_cfgerr);
                if (unlikely(txstat.ts_flags & HAL_TX_DATA_UNDERRUN)) {
                    __11nstats(sc, txaggr_data_urun);
      }
                if (unlikely(txstat.ts_flags & HAL_TX_DELIM_UNDERRUN)) {
                    __11nstats(sc, txaggr_delim_urun);

                }

                ath_tx_complete_aggr_rifs_aponly(sc, txq, bf, &bf_head, &txstat, txok);
            } else {
#ifndef REMOVE_PKT_LOG
                /* do pktlog */
                {
                    struct log_tx log_data;
                    struct ath_buf *tbf;

                    TAILQ_FOREACH(tbf, &bf_head, bf_list) {
                        log_data.firstds = tbf->bf_desc;
                        log_data.bf = tbf;
                        ath_log_txctl(sc, &log_data, 0);
                    }

                    /* log the last descriptor. */
                    log_data.firstds = lastbf->bf_desc;
                    log_data.bf = lastbf;
                    ath_log_txctl(sc, &log_data, 0);
                }
#endif

#ifdef ATH_SUPPORT_UAPSD
                if (uapsdq) {
#ifdef ATH_SUPPORT_TxBF
                    ath_tx_uapsd_complete(sc, an, bf, &bf_head, txok, 0, 0);
#else
                    ath_tx_uapsd_complete(sc, an, bf, &bf_head, txok);
#endif
                } else {
                    if (bf->bf_isbar)
                        ath_tx_complete_bar(sc, bf, &bf_head, txok);
                    else
#ifdef ATH_SUPPORT_TxBF
                        ath_tx_complete_buf(sc, bf, &bf_head, txok, 0, 0);
#else
                        ath_tx_complete_buf(sc, bf, &bf_head, txok);
#endif

                }
#else
                if (bf->bf_isbar)
					ath_tx_complete_bar(sc, bf, &bf_head, txok);
                else
#ifdef ATH_SUPPORT_TxBF
                    ath_tx_complete_buf(sc, bf, &bf_head, txok, 0);
#else
                    ath_tx_complete_buf(sc, bf, &bf_head, txok);
#endif
#endif
            }

#ifndef REMOVE_PKT_LOG
            /* do pktlog */
            {
                struct log_tx log_data;
                log_data.lastds = ds;
				log_data.bf = bf;
				log_data.nbad = nbad;
                ath_log_txstatus(sc, &log_data, 0);
            }
#endif
        }

        /*
         * schedule any pending packets if aggregation is enabled
         */
        {
          ATH_TXQ_LOCK(txq);
          ath_txq_schedule(sc, txq);
          ATH_TXQ_UNLOCK(txq);
        }
		ds->ds_txstat = txstat;
    }
    return nacked;
}







/*
 * Deferred processing of transmit interrupt.
 */
static inline void
ath_tx_tasklet_aponly(ath_dev_t dev)
{
    struct ath_softc *sc = ATH_DEV_TO_SC(dev);
    int i, nacked=0, qdepth = 0;
    u_int32_t qcumask = ((1 << HAL_NUM_TX_QUEUES) - 1);
#if ATH_TX_POLL
    u_int32_t q_time=0;
#endif
#if ATH_TX_TO_RESET_ENABLE
    u_int32_t max_q_time=0;
#endif


    ath_hal_gettxintrtxqs(sc->sc_ah, &qcumask);
 

    /* Do the Beacon completion callback (if enabled) */
    if (unlikely((atomic_read(&sc->sc_has_tx_bcn_notify)) && (qcumask & (1 << sc->sc_bhalq)))) {
        /* Notify that a beacon has completed */
        ath_tx_bcn_notify(sc);
        /* 
         * beacon queue is not setup like a data queue and hence 
         * so for beacon queue the ATH_TXQ_SETUP will be false and
         * ath_tx_processq will not be called fro beacon queue.
         */
    }

    /*
     * Process each active queue.
     */
    ath_vap_pause_txq_use_inc(sc);
    for (i = 0; i < HAL_NUM_TX_QUEUES; i++) {
        if (ATH_TXQ_SETUP(sc, i)) {
            if (((qcumask & (1 << i)) 
#if ATH_TX_POLL
                || (sc->sc_txq[i].axq_depth &&
                (q_time = ATH_DIFF(sc->sc_txq[i].axq_lastq_tick,ticks))  > MSEC_TO_TICKS(ATH_TX_POLL_TIMER))
#endif
                    )) {
                nacked += ath_tx_processq_aponly(sc, &sc->sc_txq[i]);
            }
#if ATH_TX_TO_RESET_ENABLE
            if (q_time > max_q_time) {
                max_q_time = q_time;
            }
#endif
            qdepth += ath_txq_depth(sc, i);
        }
    }
  
    ath_vap_pause_txq_use_dec(sc);

#if ATH_TX_TO_RESET_ENABLE
    if (unlikely(max_q_time > MSEC_TO_TICKS(ATH_TXQ_MAX_TIME))) {
        DPRINTF(sc, ATH_DEBUG_RESET, "%s: timed out on TXQ \n",__func__);
#ifdef AR_DEBUG		
		ath_dump_descriptors(sc);
#endif
        ath_internal_reset(sc);
    }
#endif
}




/*
 * To complete a chain of buffers associated a frame
 */
#ifdef ATH_SUPPORT_TxBF
static inline void
ath_tx_complete_buf_aponly(struct ath_softc *sc, struct ath_buf *bf, ath_bufhead *bf_q, int txok, u_int8_t txbf_status)
#else
static inline void
ath_tx_complete_buf_aponly(struct ath_softc *sc, struct ath_buf *bf, ath_bufhead *bf_q, int txok)
#endif
{
    wbuf_t wbuf = bf->bf_mpdu;
    ieee80211_tx_status_t tx_status;
    
    struct ath_txq *txq = &sc->sc_txq[bf->bf_qnum];

    /*
     * Set retry information.
     * NB: Don't use the information in the descriptor, because the frame
     * could be software retried.
     */

    // Make sure wbuf is not NULL ! Potential Double Free.
    KASSERT((wbuf != NULL),
            ("%s: NULL wbuf: lastbf %p lastfrm %p next %p flags %x"
             " status %x desc %p framelen %d seq %d tid %d keytype %x",
             __func__, bf->bf_lastbf, bf->bf_lastfrm,
             bf->bf_next, bf->bf_flags, bf->bf_status,
             bf->bf_desc, bf->bf_state.bfs_frmlen,
             bf->bf_state.bfs_seqno, bf->bf_state.bfs_tidno,
             bf->bf_state.bfs_keytype));

#ifdef ATH_SUPPORT_TxBF
    /* bf status update* for TxBF*/
    tx_status.txbf_status = txbf_status;
#endif

    tx_status.retries = bf->bf_retries;
    tx_status.flags = 0;

    if (unlikely(bf->bf_state.bfs_ispaprd)) {
        ath_tx_paprd_complete(sc, bf, bf_q);
        //printk("%s[%d]: ath_tx_paprd_complete called txok %d\n", __func__, __LINE__, txok);
        return;
    }
    if (likely(txok)) {
        /* increment count of transmitted bytes */
        sc->sc_stats.ast_tx_bytes += bf->bf_frmlen;
    }
    else {
        tx_status.flags |= ATH_TX_ERROR;

        if (bf->bf_isxretried) {
            tx_status.flags |= ATH_TX_XRETRY;
        }
    }
    /* Unmap this frame */
    wbuf_unmap_sg(sc->sc_osdev, wbuf,
                  OS_GET_DMA_MEM_CONTEXT(bf, bf_dmacontext));

    sc->sc_ieee_ops->tx_complete(wbuf, &tx_status); /* complete this frame */
    bf->bf_mpdu = NULL;
    /*
     * Return the list of ath_buf of this mpdu to free queue
     */
    ATH_TXBUF_LOCK(sc);
    if(!TAILQ_EMPTY(bf_q)) {
        int num_buf = 0;
        ATH_NUM_BUF_IN_Q(&num_buf, bf_q);
        txq->axq_num_buf_used -= num_buf;
		sc->sc_txbuf_free += num_buf;
        TAILQ_CONCAT(&sc->sc_txbuf, bf_q, bf_list);
    }
    ATH_TXBUF_UNLOCK(sc);
}





/*
 * Deferred processing of transmit interrupt.
 * Tx Interrupts need to be disabled before entering this.
 */
static inline void
ath_tx_edma_tasklet_aponly(ath_dev_t dev)
{
    struct ath_softc *sc = ATH_DEV_TO_SC(dev);
    int txok, nacked=0, qdepth = 0, nbad = 0;
    struct ath_txq *txq;
    struct ath_tx_status ts;
    HAL_STATUS status;
    ath_bufhead bf_head;
    struct ath_buf *bf;
    struct ath_node *an;
#ifndef REMOVE_PKT_LOG
    u_int32_t txs_desc[9];
#endif
#if ATH_SUPPORT_VOWEXT
    u_int8_t n_head_fail = 0;
    u_int8_t n_tail_fail = 0;
#endif

#ifdef ATH_SUPPORT_TxBF
    struct atheros_node *oan;
#endif

    for (;;) {

        /* hold lock while accessing tx status ring */
        ATH_TXSTATUS_LOCK(sc);

#ifndef REMOVE_PKT_LOG
        ath_hal_getrawtxdesc(sc->sc_ah, txs_desc);
#endif

        /*
         * Process a completion event.
         */
        status = ath_hal_txprocdesc(sc->sc_ah, (void *)&ts);

        ATH_TXSTATUS_UNLOCK(sc);

        if (status == HAL_EINPROGRESS)
            break;

        if (unlikely(status == HAL_EIO)) {
            break;
        }

        /* Skip beacon completions */
        if (ts.queue_id == sc->sc_bhalq)
            continue;

        /* Make sure the event came from an active queue */
        ASSERT(ATH_TXQ_SETUP(sc, ts.queue_id));

        /* Get the txq for the completion event */
        txq = &sc->sc_txq[ts.queue_id];

        ATH_TXQ_LOCK(txq);

        txq->axq_intrcnt = 0; /* reset periodic desc intr count */
#if ATH_HW_TXQ_STUCK_WAR
        txq->tx_done_stuck_count = 0;
#endif
        bf = TAILQ_FIRST(&txq->axq_fifo[txq->axq_tailindex]);

        if (unlikely(bf == NULL)) {
            printk("ath_tx_edma_tasklet: TXQ[%d] tailindex %d\n",
                   ts.queue_id, txq->axq_tailindex);
            ATH_TXQ_UNLOCK(txq);
            return;
        }

        if (unlikely(txq == sc->sc_cabq || txq == sc->sc_uapsdq)) {
            ATH_EDMA_MCASTQ_MOVE_HEAD_UNTIL(txq, &bf_head, bf->bf_lastbf, bf_list);
        } else {
            ATH_EDMA_TXQ_MOVE_HEAD_UNTIL(txq, &bf_head, bf->bf_lastbf, bf_list);
        }

        if (likely(bf->bf_isaggr)) {
            txq->axq_aggr_depth--;
        }

        ATH_TXQ_UNLOCK(txq);

        an = bf->bf_node;
       
        if (likely(an != NULL)) {
            int noratectrl;

#ifdef ATH_SUPPORT_TxBF
            oan = ATH_NODE_ATHEROS(an);
            if (oan->txbf && (ts.ts_status == 0) && VALID_TXBF_RATE(ts.ts_ratecode, oan->usedNss)) {

                if (ts.ts_txbfstatus & ATH_TXBF_stream_missed) {
                    __11nstats(sc,bf_stream_miss);
                }
                if (ts.ts_txbfstatus & ATH_TxBF_BW_mismatched) {
                    __11nstats(sc,bf_bandwidth_miss);
                }
                if (ts.ts_txbfstatus & ATH_TXBF_Destination_missed ) {
                    __11nstats(sc,bf_destination_miss);
             }

            }
#endif
            noratectrl = an->an_flags & (ATH_NODE_CLEAN | ATH_NODE_PWRSAVE);
			OS_SYNC_SINGLE(sc->sc_osdev, bf->bf_daddr, sc->sc_txdesclen, BUS_DMA_FROMDEVICE, NULL);

            ath_hal_gettxratecode(sc->sc_ah, bf->bf_desc, (void *)&ts);

            ath_tx_update_stats(sc, bf, txq->axq_qnum, &ts);

            txok = (ts.ts_status == 0);

            /*
             * Hand the descriptor to the rate control algorithm
             * if the frame wasn't dropped for filtering or sent
             * w/o waiting for an ack.  In those cases the rssi
             * and retry counts will be meaningless.
             */
            if (unlikely(!bf->bf_isampdu)) {
                /*
                 * This frame is sent out as a single frame. Use hardware retry
                 * status for this frame.
                 */
                bf->bf_retries = ts.ts_longretry;
                if (ts.ts_status & HAL_TXERR_XRETRY) {
                    __11nstats(sc,tx_sf_hw_xretries);
                    bf->bf_isxretried = 1;
                }
                nbad = 0;
#if ATH_SUPPORT_VOWEXT
                if ( ATH_IS_VOWEXT_AGGRSIZE_ENABLED(sc) || ATH_IS_VOWEXT_RCA_ENABLED(sc)){
                    n_head_fail = n_tail_fail = 0;
                }
#endif
            } else {
                nbad = ath_tx_num_badfrms(sc, bf, &ts, txok);
#if ATH_SUPPORT_VOWEXT
                if ( ATH_IS_VOWEXT_AGGRSIZE_ENABLED(sc) || ATH_IS_VOWEXT_RCA_ENABLED(sc)) {
                    n_tail_fail = (nbad & 0xFF);
                    n_head_fail = ((nbad >> 8) & 0xFF);
                    nbad = ((nbad >> 16) & 0xFF);
                }
#endif
            }

            if (likely((ts.ts_status & HAL_TXERR_FILT) == 0 &&
                (bf->bf_flags & HAL_TXDESC_NOACK) == 0))
            {
                /*
                 * If frame was ack'd update the last rx time
                 * used to workaround phantom bmiss interrupts.
                 */
                if (likely(ts.ts_status == 0))
                    nacked++;

                if (likely(!bf->bf_useminrate)) {
#ifdef ATH_SUPPORT_VOWEXT
                    /* FIXME do not care Ospre related issues as on today, keep
                             this pending until we get to that
                    */
                    if(!wbuf_is_sa_train_packet(bf->bf_mpdu)) {
                        ath_rate_tx_complete_11n(sc,
                                             an,
                                             &ts,
                                             bf->bf_rcs,
                                             TID_TO_WME_AC(bf->bf_tidno),
                                             bf->bf_nframes,
                                             nbad, n_head_fail, n_tail_fail,
                                             ath_tx_get_rts_retrylimit(sc, txq));
                    }
#else
                    if (likely(bf->bf_isdata && !noratectrl)) {
                        ath_rate_tx_complete_11n(sc,
                                             an,
                                             &ts,
                                             bf->bf_rcs,
                                             TID_TO_WME_AC(bf->bf_tidno),
                                             bf->bf_nframes,
                                             nbad,
                                             ath_tx_get_rts_retrylimit(sc, txq));

                    }
#endif
                }
            }

            /*
             * Complete this transmit unit
             *
             * Node cannot be referenced past this point since it can be freed
             * here.
             */
            if (likely(bf->bf_isampdu)) {
                if (unlikely(ts.ts_flags & HAL_TX_DESC_CFG_ERR))
                    __11nstats(sc, txaggr_desc_cfgerr);
                if (unlikely(ts.ts_flags & HAL_TX_DATA_UNDERRUN)) {
                    __11nstats(sc, txaggr_data_urun);
                }
                if (unlikely(ts.ts_flags & HAL_TX_DELIM_UNDERRUN)) {
                    __11nstats(sc, txaggr_delim_urun);
                }
                ath_tx_complete_aggr_rifs(sc, txq, bf, &bf_head, &ts, txok);
            } else {
#ifndef REMOVE_PKT_LOG
                /* do pktlog */
                {
                    struct log_tx log_data;
                    struct ath_buf *tbf;

                    TAILQ_FOREACH(tbf, &bf_head, bf_list) {
                        log_data.firstds = tbf->bf_desc;
                        log_data.bf = tbf;
                        ath_log_txctl(sc, &log_data, 0);
                    }
                }
#endif

#ifdef ATH_SUPPORT_UAPSD
                if (txq == sc->sc_uapsdq)
                {
#ifdef ATH_SUPPORT_TxBF
                    ath_tx_uapsd_complete(sc, an, bf, &bf_head, txok, ts.ts_txbfstatus, ts.ts_tstamp);
#else
                    ath_tx_uapsd_complete(sc, an, bf, &bf_head, txok);
#endif
                }
                else
#endif
                {
                    if (unlikely(bf->bf_isbar))
                        ath_tx_complete_bar(sc, bf, &bf_head, txok);
                    else
#ifdef  ATH_SUPPORT_TxBF
                        ath_tx_complete_buf_aponly(sc, bf, &bf_head, txok, ts.ts_txbfstatus);
#else
                        ath_tx_complete_buf_aponly(sc, bf, &bf_head, txok);
#endif
                }
            }

#ifndef REMOVE_PKT_LOG
            /* do pktlog */
            {
                struct log_tx log_data;
                log_data.lastds = &txs_desc;
                ath_log_txstatus(sc, &log_data, 0);
            }
#endif
        } else {
           /* PAPRD has NULL an */
            if (bf->bf_state.bfs_ispaprd) {
                ath_tx_paprd_complete(sc, bf, &bf_head);
                //printk("%s[%d]: ath_tx_paprd_complete called txok %d\n", __func__, __LINE__, txok);
                return;
            }
        }

        /*
         * schedule any pending packets if aggregation is enabled
         */

        ATH_TXQ_LOCK(txq);
        ath_txq_schedule(sc, txq);
        ATH_TXQ_UNLOCK(txq);

        qdepth += ath_txq_depth(sc, txq->axq_qnum);
    }

#if 0
    //only for STA - notify the STA'a pm state machine
    if (sc->sc_ieee_ops->notify_txq_status)
        sc->sc_ieee_ops->notify_txq_status(sc->sc_ieee, qdepth);
#endif

    return;
}


extern void ath_tx_tasklet(ath_dev_t dev);


typedef enum {
    FILTER_STATUS_ACCEPT = 0,
    FILTER_STATUS_REJECT
} ieee80211_privasy_filter_status;

#define IS_SNAP(_llc) ((_llc)->llc_dsap == LLC_SNAP_LSAP && \
                        (_llc)->llc_ssap == LLC_SNAP_LSAP && \
                        (_llc)->llc_control == LLC_UI)
#define RFC1042_SNAP_NOT_AARP_IPX(_llc) \
            ((_llc)->llc_snap.org_code[0] == RFC1042_SNAP_ORGCODE_0 && \
            (_llc)->llc_snap.org_code[1] == RFC1042_SNAP_ORGCODE_1 && \
            (_llc)->llc_snap.org_code[2] == RFC1042_SNAP_ORGCODE_2 \
            && !((_llc)->llc_snap.ether_type == htons(ETHERTYPE_AARP) || \
                (_llc)->llc_snap.ether_type == htons(ETHERTYPE_IPX)))
#define IS_BTEP(_llc) ((_llc)->llc_snap.org_code[0] == BTEP_SNAP_ORGCODE_0 && \
            (_llc)->llc_snap.org_code[1] == BTEP_SNAP_ORGCODE_1 && \
            (_llc)->llc_snap.org_code[2] == BTEP_SNAP_ORGCODE_2)
#define IS_ORG_BTAMP(_llc) ((_llc)->llc_snap.org_code[0] == BTAMP_SNAP_ORGCODE_0 && \
                            (_llc)->llc_snap.org_code[1] == BTAMP_SNAP_ORGCODE_1 && \
                            (_llc)->llc_snap.org_code[2] == BTAMP_SNAP_ORGCODE_2)
#define IS_ORG_AIRONET(_llc) ((_llc)->llc_snap.org_code[0] == AIRONET_SNAP_CODE_0 && \
                               (_llc)->llc_snap.org_code[1] == AIRONET_SNAP_CODE_1 && \
                               (_llc)->llc_snap.org_code[2] == AIRONET_SNAP_CODE_2)

/* 
 * delivers the data to the OS .
 *  will deliver standard 802.11 frames (with qos control removed)
 *  if IEEE80211_DELIVER_80211 param is set. 
 *  will deliver ethernet frames (with 802.11 header decapped)
 *  if IEEE80211_DELIVER_80211 param is not set. 
 *  this funcction consumes the  passed in wbuf.
 */
static ieee80211_privasy_filter_status
ieee80211_check_privacy_filters_aponly(struct ieee80211_node *ni, wbuf_t wbuf, int is_mcast)
{
    struct ieee80211vap *vap = ni->ni_vap;
    struct llc *llc;
    u_int16_t ether_type = 0;
    u_int32_t hdrspace;
    u_int32_t i;
    struct ieee80211_frame *wh;
    ieee80211_privacy_filter_packet_type packet_type;
    u_int8_t is_encrypted;

    /* Safemode must avoid the PrivacyExemptionList and ExcludeUnencrypted checking */
    if (unlikely(IEEE80211_VAP_IS_SAFEMODE_ENABLED(vap))) {
        return FILTER_STATUS_ACCEPT;
    }
    wh = (struct ieee80211_frame *) wbuf_header(wbuf);

    hdrspace = ieee80211_hdrspace(vap->iv_ic, wbuf_header(wbuf));

    if (unlikely(wbuf_get_pktlen(wbuf)  < ( hdrspace + LLC_SNAPFRAMELEN))) {
        IEEE80211_DISCARD_MAC(vap, IEEE80211_MSG_INPUT,
                            wh->i_addr2, "data",
                            "%s: too small packet 0x%x len %u \n",__func__,
                            ether_type, wbuf_get_pktlen(wbuf));
        return FILTER_STATUS_REJECT; /* filter the packet */
    }

    llc = (struct llc *)(wbuf_header(wbuf) + hdrspace);
    if (IS_SNAP(llc) && (RFC1042_SNAP_NOT_AARP_IPX(llc) || IS_ORG_BTAMP(llc) ||
        IS_ORG_AIRONET(llc))) {
        ether_type = ntohs(llc->llc_snap.ether_type);
    } else {
        ether_type = htons(wbuf_get_pktlen(wbuf) - hdrspace);
    }

    is_encrypted = (wh->i_fc[1] & IEEE80211_FC1_WEP);
    wh->i_fc[1] &= ~IEEE80211_FC1_WEP; /* XXX: we don't need WEP bit from here */
    
    if (is_mcast) {
        packet_type = IEEE80211_PRIVACY_FILTER_PACKET_MULTICAST;
    } else {
        packet_type = IEEE80211_PRIVACY_FILTER_PACKET_UNICAST;
    }

    for (i=0; i < vap->iv_num_privacy_filters; i++) {
        /* skip if the ether type does not match */
        if (vap->iv_privacy_filters[i].ether_type != ether_type)
            continue;

        /* skip if the packet type does not match */
        if (vap->iv_privacy_filters[i].packet_type != packet_type &&
            vap->iv_privacy_filters[i].packet_type != IEEE80211_PRIVACY_FILTER_PACKET_BOTH) 
            continue;
        
        if (vap->iv_privacy_filters[i].filter_type == IEEE80211_PRIVACY_FILTER_ALLWAYS) {
            /*
             * In this case, we accept the frame if and only if it was originally
             * NOT encrypted.
             */
            if (is_encrypted) {
               IEEE80211_DISCARD_MAC(vap, IEEE80211_MSG_INPUT,
                            wh->i_addr2, "data",
                            "%s: packet encrypted ether type 0x%x len %u \n",__func__,
                            ether_type, wbuf_get_pktlen(wbuf));
                return FILTER_STATUS_REJECT;
            } else {
                return FILTER_STATUS_ACCEPT;
            }
        } else if (vap->iv_privacy_filters[i].filter_type  == IEEE80211_PRIVACY_FILTER_KEY_UNAVAILABLE) {
            /*
             * In this case, we reject the frame if it was originally NOT encrypted but 
             * we have the key mapping key for this frame.
             */
            if (!is_encrypted && !is_mcast && ni->ni_ucastkey.wk_valid) {
               IEEE80211_DISCARD_MAC(vap, IEEE80211_MSG_INPUT,
                            wh->i_addr2, "data",
                            "%s: node has a key ether type 0x%x len %u \n",__func__,
                            ether_type, wbuf_get_pktlen(wbuf));
                return FILTER_STATUS_REJECT;
            } else {
                return FILTER_STATUS_ACCEPT;
            }
        } else {
            /*
             * The privacy exemption does not apply to this frame.
             */
            break;
        }
    }

    /*
     * If the privacy exemption list does not apply to the frame, check ExcludeUnencrypted.
     * if ExcludeUnencrypted is not set, or if this was oringially an encrypted frame, 
     * it will be accepted.
     */
    if (!IEEE80211_VAP_IS_DROP_UNENC(vap) || is_encrypted) {
        /*
         * if the node is not authorized 
         * reject the frame.
         */
        if (!ieee80211_node_is_authorized(ni)) {
            IEEE80211_DISCARD_MAC(vap, IEEE80211_MSG_INPUT,
                                  wh->i_addr2, "data",
                                  "unauthorized port: ether type 0x%x len %u \n",
                                  ether_type, wbuf_get_pktlen(wbuf));
            vap->iv_stats.is_rx_unauth++;
            return FILTER_STATUS_REJECT;
        }
        return FILTER_STATUS_ACCEPT;
    }


    if (!is_encrypted && IEEE80211_VAP_IS_DROP_UNENC(vap)) {
        if (is_mcast) {
            vap->iv_multicast_stats.ims_rx_unencrypted++;
            vap->iv_multicast_stats.ims_rx_decryptcrc++;
        } else {
            vap->iv_unicast_stats.ims_rx_unencrypted++;
            vap->iv_unicast_stats.ims_rx_decryptcrc++;
        }
    }

    IEEE80211_DISCARD_MAC(vap, IEEE80211_MSG_INPUT,
                            wh->i_addr2, "data",
                          "%s: ether type 0x%x len %u \n",__func__,
                             ether_type, wbuf_get_pktlen(wbuf));
    return FILTER_STATUS_REJECT;
}

static inline wbuf_t
ieee80211_decap_aponly(struct ieee80211vap *vap, wbuf_t wbuf, size_t hdrspace)
{
    struct ieee80211_qosframe_addr4 wh;    /* max size address frame */
    struct ether_header *eh;
    struct llc *llc;
    u_int16_t ether_type = 0;

    if (unlikely(wbuf_get_pktlen(wbuf) < (hdrspace + sizeof(*llc)))) {
        /* XXX stat, msg */
        wbuf_free(wbuf);
        wbuf = NULL;
        goto done;
    }
    OS_MEMCPY(&wh, wbuf_header(wbuf), hdrspace);
    llc = (struct llc *)(wbuf_header(wbuf) + hdrspace);

    if (IS_SNAP(llc) && RFC1042_SNAP_NOT_AARP_IPX(llc)) {
       /* leave ether_tyep in  in network order */
        ether_type = llc->llc_un.type_snap.ether_type;
        wbuf_pull(wbuf, (u_int16_t) (hdrspace + sizeof(struct llc) - sizeof(*eh)));
        llc = NULL;
    } else if (IS_SNAP(llc) && IS_BTEP(llc)) {
        /* for bridge-tunnel encap, remove snap and 802.11 headers, keep llc ptr for type */
        wbuf_pull(wbuf,
                  (u_int16_t) (hdrspace + sizeof(struct llc) - sizeof(*eh)));
    } else {
        wbuf_pull(wbuf, (u_int16_t) (hdrspace - sizeof(*eh)));
    }
    eh = (struct ether_header *)(wbuf_header(wbuf));

    ieee80211_rptplacement_input(vap, wbuf, eh);

    switch (wh.i_fc[1] & IEEE80211_FC1_DIR_MASK) {
    case IEEE80211_FC1_DIR_NODS:
        IEEE80211_ADDR_COPY(eh->ether_dhost, wh.i_addr1);
        IEEE80211_ADDR_COPY(eh->ether_shost, wh.i_addr2);
        break;
    case IEEE80211_FC1_DIR_TODS:
        IEEE80211_ADDR_COPY(eh->ether_dhost, wh.i_addr3);
        IEEE80211_ADDR_COPY(eh->ether_shost, wh.i_addr2);
        break;
    case IEEE80211_FC1_DIR_FROMDS:
        IEEE80211_ADDR_COPY(eh->ether_dhost, wh.i_addr1);
        IEEE80211_ADDR_COPY(eh->ether_shost, wh.i_addr3);
        break;
    } 

    if (llc != NULL) {
        if (IS_BTEP(llc)) {
            /* leave ether_tyep in  in network order */
            eh->ether_type = llc->llc_snap.ether_type; 
        } else {
            eh->ether_type = htons(wbuf_get_pktlen(wbuf) - sizeof(*eh));
        }
    }
    else {
        eh->ether_type = ether_type;
    }
done:
    return wbuf;
}

static void 
ieee80211_deliver_data_aponly(struct ieee80211vap *vap, wbuf_t wbuf, struct ieee80211_node *ni, struct ieee80211_rx_status *rs,
                       u_int32_t hdrspace, int is_mcast, u_int8_t subtype) 
{
    if (!IEEE80211_VAP_IS_DELIVER_80211_ENABLED(vap)) {
        /*
         * if the OS is interested in ethernet frame,
         * decap the 802.11 frame and convert into 
         * ethernet frame.
         */
        wbuf = ieee80211_decap_aponly(vap, wbuf, hdrspace);
        if (unlikely(!wbuf)) {
         IEEE80211_DPRINTF(vap,
                          IEEE80211_MSG_INPUT,
                          "decap %s",
                          "failed");
            return;
        }

        /*
         * If IQUE is not enabled, the ops table is NULL and the following
         * steps will be skipped;
         * If IQUE is enabled, the packet will be checked to see whether it
         * is an IGMP packet or not, and update the mcast snoop table if necessary
         */
        if (vap->iv_ique_ops.me_inspect) {
            vap->iv_ique_ops.me_inspect(vap, ni, wbuf);
        }
    }
    
    /* perform as a bridge within the AP */
    if (!IEEE80211_VAP_IS_NOBRIDGE_ENABLED(vap)) {
        wbuf_t wbuf_cpy = NULL;

        if (is_mcast) {
            wbuf_cpy = wbuf_clone(vap->iv_ic->ic_osdev, wbuf);
#if ATH_RXBUF_RECYCLE
			wbuf_set_cloned(wbuf_cpy);
#endif
        } else {
            struct ieee80211_node *ni1;
            /*
             * Check if destination is associated with the
             * same vap and authorized to receive traffic.
             * Beware of traffic destined for the vap itself;
             * sending it will not work; just let it be
             * delivered normally.
             */
            struct ether_header *eh= (struct ether_header *) wbuf_header(wbuf);
            ni1 = ieee80211_find_node(&vap->iv_ic->ic_sta, eh->ether_dhost);
            if (unlikely(ni1 != NULL)) {
                if (ni1->ni_vap == vap &&
                    ieee80211_node_is_authorized(ni1) &&
                    ni1 != vap->iv_bss) {
                    wbuf_cpy = wbuf;
                    wbuf = NULL;
                }
                ieee80211_free_node(ni1);
            }
        }
        if (unlikely(wbuf_cpy != NULL)) {
            /*
             * send the frame copy back to the interface.
             * this frame is either multicast frame. or unicast frame
             * to one of the stations.
             */
            vap->iv_evtable->wlan_vap_xmit_queue(vap->iv_ifp, wbuf_cpy);
        }
    }
    if (likely(wbuf != NULL)) {

#if UMAC_SUPPORT_VI_DBG
        ieee80211_vi_dbg_input(vap, wbuf);
#endif

        /*
         * deliver the data frame to the os. the handler cosumes the wbuf.
         */
        __osif_deliver_data(vap->iv_ifp, wbuf);        
    }
}

 /*
  * processes data frames.
  * ieee80211_input_data consumes the wbuf .
  */
static void 
ieee80211_input_data_aponly(struct ieee80211_node *ni, wbuf_t wbuf, struct ieee80211_rx_status *rs, int subtype, int dir)
{
    struct ieee80211vap *vap = ni->ni_vap;
    struct ieee80211com *ic = ni->ni_ic;
    struct ieee80211_mac_stats *mac_stats;
    struct ieee80211_frame *wh;
    struct ieee80211_key *key;
    u_int16_t hdrspace;
    int is_amsdu = 0, is_mcast;

    wh = (struct ieee80211_frame *) wbuf_header(wbuf);
    is_mcast = IEEE80211_IS_MULTICAST(IEEE80211_WH4(wh)->i_addr3);
    mac_stats = is_mcast ? &vap->iv_multicast_stats : &vap->iv_unicast_stats;

    hdrspace = ieee80211_hdrspace(ic, wbuf_header(wbuf));
    if (unlikely(wbuf_get_pktlen(wbuf) < hdrspace)) {
        goto bad;
    }

    vap->iv_lastdata = OS_GET_TIMESTAMP();
    vap->iv_txrxbytes += wbuf_get_pktlen(wbuf);
    if (IEEE80211_CONTAIN_DATA(subtype)) {
        vap->iv_last_traffic_indication = vap->iv_lastdata;
    }

    if (unlikely(!(dir == IEEE80211_FC1_DIR_TODS))) {  
        goto bad;
    }

    /* check if source STA is associated */
    if (unlikely(ni == vap->iv_bss)) {
        IEEE80211_DISCARD(vap, IEEE80211_MSG_INPUT,
                          wh, "data", "%s", "unknown src");
        
        /* NB: caller deals with reference */
        if (ieee80211_vap_ready_is_set(vap)) {
            ni = ieee80211_tmp_node(vap, wh->i_addr2);
            if (ni != NULL) {
                ieee80211_send_deauth(ni, IEEE80211_REASON_NOT_AUTHED);

                /* claim node immediately */
                ieee80211_free_node(ni);
            }
        }
        goto bad;
    }

    if (unlikely(ni->ni_associd == 0)) {
        IEEE80211_DISCARD(vap, IEEE80211_MSG_INPUT,
                          wh, "data", "%s", "unassoc src");
        ieee80211_send_disassoc(ni, IEEE80211_REASON_NOT_ASSOCED);
        goto bad;
    }
    /*
     *  Safemode prevents us from calling decap.
     */
    if (!IEEE80211_VAP_IS_SAFEMODE_ENABLED(vap) &&
        (wh->i_fc[1] & IEEE80211_FC1_WEP)) {
        key = ieee80211_crypto_decap(ni, wbuf, hdrspace, rs);
        if (unlikely(key == NULL)) {
            mac_stats->ims_rx_decryptcrc++;
            IEEE80211_DISCARD (vap, IEEE80211_MSG_INPUT, wh,
                          "key is" , "%s"," null");
            goto bad;
        } 

        wh = (struct ieee80211_frame *) wbuf_header(wbuf);
        /* NB: We clear the Protected bit later */
    } else {
        key = NULL;
    }


    /*
     * Next up, any defragmentation. A list of wbuf will be returned.
     * However, do not defrag when in safe mode.
     */
    if (!IEEE80211_VAP_IS_SAFEMODE_ENABLED(vap) && !is_mcast) {
        wbuf = ieee80211_defrag(ni, wbuf, hdrspace);
        if (wbuf == NULL) {
            /* Fragment dropped or frame not complete yet */
            IEEE80211_DISCARD(vap,
                          IEEE80211_MSG_INPUT, wh,
                          "defarg","%s",
                          "failed");
            goto out;
        }
    }
    if (subtype == IEEE80211_FC0_SUBTYPE_QOS) {
        is_amsdu = ((struct ieee80211_qosframe *)wh)->i_qos[0] & IEEE80211_QOS_AMSDU;
    }

    /*
     * Next strip any MSDU crypto bits.
     */
    ASSERT(!IEEE80211_VAP_IS_SAFEMODE_ENABLED(vap) || (key == NULL));
    if (unlikely((key != NULL) && !ieee80211_crypto_demic(vap, key, wbuf, hdrspace, 0, rs))) {
        IEEE80211_DISCARD_MAC(vap, IEEE80211_MSG_INPUT,
                              ni->ni_macaddr, "data", "%s", "demic error");
        /* IEEE80211_NODE_STAT(ni, rx_demicfail); */
         IEEE80211_DISCARD(vap,
                          IEEE80211_MSG_INPUT, wh,
                          "demic","%s",
                          "failed");
        goto bad;
    }

    if (subtype == IEEE80211_FC0_SUBTYPE_NODATA) {
        /* no need to process the null data frames any further */
        goto bad;
    }
#if ATH_RXBUF_RECYCLE
	if (is_mcast) {
		wbuf_set_cloned(wbuf);
	} else {
 		wbuf_clear_cloned(wbuf);
	}
#endif
    if (!is_amsdu) {
        if (ieee80211_check_privacy_filters_aponly(ni, wbuf, is_mcast) == FILTER_STATUS_REJECT) {
             IEEE80211_DISCARD_MAC(vap,
                                   IEEE80211_MSG_INPUT, wh->i_addr2, "data",
                              "privacy filter check","%s \n",
                              "failed");
            goto bad;
        }
    } else {
        ieee80211_amsdu_input(ni, wbuf, rs, is_mcast, subtype);
        goto out;
    }

    mac_stats->ims_rx_packets++;
	mac_stats->ims_rx_bytes += wbuf_get_pktlen(wbuf);

    /* consumes the wbuf */
    ieee80211_deliver_data_aponly(vap, wbuf, ni, rs, hdrspace, is_mcast, subtype);
out:
    return;

bad:
/*  FIX ME: linux specific netdev struct iv_destats has to be replaced*/
    wbuf_free(wbuf);
}

/*
 * Process a received frame.  The node associated with the sender
 * should be supplied.  If nothing was found in the node table then
 * the caller is assumed to supply a reference to iv_bss instead.
 * The RSSI and a timestamp are also supplied.  The RSSI data is used
 * during AP scanning to select a AP to associate with; it can have
 * any units so long as values have consistent units and higher values
 * mean ``better signal''.  
 */

/*
 * This function is only called for unicast QoS data frames with AMPDU enabled node
 */
static int
ieee80211_input_aponly(struct ieee80211_node *ni, wbuf_t wbuf, struct ieee80211_rx_status *rs)
{
#define QOS_NULL   (IEEE80211_FC0_TYPE_DATA | IEEE80211_FC0_SUBTYPE_QOS_NULL)
#define HAS_SEQ(type, subtype)   (((type & 0x4) == 0) && ((type | subtype) != QOS_NULL))
    struct ieee80211com *ic = ni->ni_ic;
    struct ieee80211_frame *wh;
    struct ieee80211vap *vap = ni->ni_vap;
    int type = -1, subtype, dir;
    u_int16_t rxseq;
    u_int8_t *bssid;

    wbuf_set_node(wbuf, ni);

    if (wbuf_get_pktlen(wbuf) < ic->ic_minframesize) {
        goto bad1;
    }

    wh = (struct ieee80211_frame *) wbuf_header(wbuf);
    if (unlikely((wh->i_fc[0] & IEEE80211_FC0_VERSION_MASK) != IEEE80211_FC0_VERSION_0)) {
        /* XXX: no stats for it. */
        goto bad1;
    }
    type = wh->i_fc[0] & IEEE80211_FC0_TYPE_MASK;
    subtype = wh->i_fc[0] & IEEE80211_FC0_SUBTYPE_MASK;
    dir = wh->i_fc[1] & IEEE80211_FC1_DIR_MASK;


    if (OS_ATOMIC_CMPXCHG(&vap->iv_rx_gate, 0, 1) != 0) {
        goto bad1;
    }

    if (unlikely(!ieee80211_vap_active_is_set(vap))) {
        if (vap->iv_input_mgmt_filter && type == IEEE80211_FC0_TYPE_MGT && IEEE80211_ADDR_EQ(wh->i_addr1, vap->iv_myaddr) ) {
              vap->iv_input_mgmt_filter(ni,wbuf,subtype,rs) ;
        } 
        IEEE80211_DPRINTF(vap, IEEE80211_MSG_INPUT, "vap in not active, %s \n", "discard the frame");
        goto bad;
    }
    
    /*
     * XXX Validate received frame if we're not scanning.
     * why do we receive only data frames when we are scanning and 
     * current (foreign channel) channel is the bss channel ?
     * should we simplify this to if (vap->iv_bsschan == ic->ic_curchan) ?
     */
    
    if (vap->iv_evtable && vap->iv_evtable->wlan_receive_filter_80211) {
        if (vap->iv_evtable->wlan_receive_filter_80211(vap->iv_ifp, wbuf, type, subtype, rs)) {
            goto bad;
        }
    }

    /*
     * Data frame, validate the bssid.
     */
    if ((ieee80211_scan_in_home_channel(ic->ic_scanner)) ||
        ((vap->iv_bsschan == ic->ic_curchan) && (type == IEEE80211_FC0_TYPE_DATA))) {
 		
        if (likely(dir != IEEE80211_FC1_DIR_NODS))
            bssid = wh->i_addr1;
        else if (type == IEEE80211_FC0_TYPE_CTL)
            bssid = wh->i_addr1;
        else {
            if (wbuf_get_pktlen(wbuf) < sizeof(struct ieee80211_frame)) {
                goto bad;
            }
            bssid = wh->i_addr3;
        }
        if (likely(type == IEEE80211_FC0_TYPE_DATA)) {
            if (unlikely(!IEEE80211_ADDR_EQ(bssid, ieee80211_node_get_bssid(vap->iv_bss)) &&
                !IEEE80211_ADDR_EQ(bssid, IEEE80211_GET_BCAST_ADDR(ic)) &&
                subtype != IEEE80211_FC0_SUBTYPE_BEACON)) {
                    /* not interested in */
                    goto bad;
            }
        }

        ni->ni_rssi = rs->rs_rssi;

        /* Check duplicates */
        if (likely(HAS_SEQ(type, subtype))) {
            u_int8_t tid;
            if (likely(IEEE80211_QOS_HAS_SEQ(wh))) {
                tid = ((struct ieee80211_qosframe *)wh)->
                    i_qos[0] & IEEE80211_QOS_TID;
            } else {
                if (type == IEEE80211_FC0_TYPE_MGT)
                    tid = IEEE80211_TID_SIZE; /* use different pool for rx mgt seq number */
                else
                    tid = IEEE80211_NON_QOS_SEQ;
            }

            rxseq = le16toh(*(u_int16_t *)wh->i_seq);
            if (unlikely((wh->i_fc[1] & IEEE80211_FC1_RETRY) &&
                (rxseq == ni->ni_rxseqs[tid]))) {
                ni->ni_last_rxseqs[tid] = ni->ni_rxseqs[tid];
                goto bad;
            }
            ni->ni_rxseqs[tid] = rxseq;
        }
    }
    /*
     * Check for power save state change.
     */
    if (unlikely((ni != vap->iv_bss) && 
        !(type == IEEE80211_FC0_TYPE_MGT && subtype == IEEE80211_FC0_SUBTYPE_PROBE_REQ)))
    {
        if (unlikely((wh->i_fc[1] & IEEE80211_FC1_PWR_MGT) ^
            (ni->ni_flags & IEEE80211_NODE_PWR_MGT))) {
            ic->ic_node_psupdate(ni, wh->i_fc[1] & IEEE80211_FC1_PWR_MGT, 0);
            ieee80211_mlme_node_pwrsave(ni, wh->i_fc[1] & IEEE80211_FC1_PWR_MGT);
        }
    }

    if (likely(type == IEEE80211_FC0_TYPE_DATA)) {
        if (unlikely(!ieee80211_vap_ready_is_set(vap))) {
            goto bad;
        }
        /* ieee80211_input_data consumes the wbuf */
        ieee80211_node_activity(ni); /* node has activity */
        ieee80211_input_data_aponly(ni, wbuf, rs, subtype, dir); 
    } else if (unlikely(type == IEEE80211_FC0_TYPE_MGT)) {
        /* ieee80211_recv_mgmt does not consume the wbuf */
        if (subtype != IEEE80211_FC0_SUBTYPE_PROBE_REQ) {
            ieee80211_node_activity(ni); /* node has activity */
        }
        if (vap->iv_input_mgmt_filter == NULL || 
            (vap->iv_input_mgmt_filter && vap->iv_input_mgmt_filter(ni,wbuf,subtype,rs) == 0)) {
	        /*
             * if no filter function is installed (0)
             * if not filtered by the filter function
             * then process  management frame.
             */
            ieee80211_recv_mgmt(ni, wbuf, subtype, rs);
        }
        /*
         * deliver the frame to the os. the handler cosumes the wbuf.
         */
        if (vap->iv_evtable) {
            vap->iv_evtable->wlan_receive(vap->iv_ifp, wbuf, type, subtype, rs);
        }
    } else if (unlikely(type == IEEE80211_FC0_TYPE_CTL)) {
        vap->iv_stats.is_rx_ctl++;
        ieee80211_recv_ctrl(ni, wbuf, subtype, rs);
        /*
         * deliver the frame to the os. the handler cosumes the wbuf.
         */
        vap->iv_evtable->wlan_receive(vap->iv_ifp, wbuf, type, subtype, rs);
    } else {
        goto bad;
    }

    (void) OS_ATOMIC_CMPXCHG(&vap->iv_rx_gate, 1, 0);
    return type;

bad:
    (void) OS_ATOMIC_CMPXCHG(&vap->iv_rx_gate, 1, 0);
bad1:
    wbuf_free(wbuf);
    return type; 
#undef HAS_SEQ
#undef QOS_NULL
}

/*
 * This function is called only for unicast QoS data frames with AMPDU nodes
 */
static int
ath_net80211_input_aponly(ieee80211_node_t node, wbuf_t wbuf, ieee80211_rx_status_t *rx_status)
{
    struct ieee80211_node *ni = (struct ieee80211_node *)node;
    struct ieee80211_rx_status rs;
    rs.rs_flags =
        ((rx_status->flags & ATH_RX_FCS_ERROR) ? IEEE80211_RX_FCS_ERROR : 0) |
        ((rx_status->flags & ATH_RX_MIC_ERROR) ? IEEE80211_RX_MIC_ERROR : 0) |
        ((rx_status->flags & ATH_RX_DECRYPT_ERROR) ? IEEE80211_RX_DECRYPT_ERROR : 0)
    | ((rx_status->flags & ATH_RX_KEYMISS) ? IEEE80211_RX_KEYMISS : 0);

    rs.rs_phymode = ni->ni_ic->ic_curmode;
    rs.rs_freq = ni->ni_ic->ic_curchan->ic_freq;
    rs.rs_rssi = rx_status->rssi;
    rs.rs_abs_rssi = rx_status->abs_rssi;
    rs.rs_datarate = rx_status->rateKbps;
    rs.rs_isaggr = rx_status->isaggr;
    rs.rs_isapsd = rx_status->isapsd;
    rs.rs_noisefloor = rx_status->noisefloor;
    rs.rs_channel = rx_status->channel;   
    
    rs.rs_tstamp.tsf = rx_status->tsf;

    ath_txbf_update_rx_status(&rs, rx_status);
    rs.rs_rssi = rx_status->rssi;
//    OS_MEMCPY(rs.rs_rssictl, rx_status->rssictl, IEEE80211_MAX_ANTENNA);
//    OS_MEMCPY(rs.rs_rssiextn, rx_status->rssiextn, IEEE80211_MAX_ANTENNA);
#if ATH_VOW_EXT_STATS
    rs.vow_extstats_offset = rx_status->vow_extstats_offset;
#endif    

    return ieee80211_input_aponly(ni, wbuf, &rs);
}


/*
 * Function to handle a subframe of aggregation when HT is enabled
 */
static inline int
ath_ampdu_input_aponly(struct ath_softc *sc, struct ath_node *an, wbuf_t wbuf, ieee80211_rx_status_t *rx_status)
{
    struct ieee80211_frame             *wh;
    struct ieee80211_qosframe          *whqos;
    u_int8_t                           type, subtype;
    int                                ismcast;
    int                                tid;
    struct ath_arx_tid                 *rxtid;
    int                                index, cindex, rxdiff;
    u_int16_t                          rxseq;
    struct ath_rxbuf                   *rxbuf;
    wbuf_t                             wbuf_to_indicate;
    int                                dir;

    wh = (struct ieee80211_frame *) wbuf_header(wbuf);

    __11nstats(sc, rx_aggr);
    /*
     * collect stats of frames with non-zero version
     */
    if (unlikely((wh->i_fc[0] & IEEE80211_FC0_VERSION_MASK) != IEEE80211_FC0_VERSION_0)) {
        __11nstats(sc, rx_aggrbadver);
        wbuf_free(wbuf);
        return -1;
    }

    type = wh->i_fc[0] & IEEE80211_FC0_TYPE_MASK;
    subtype = wh->i_fc[0] & IEEE80211_FC0_SUBTYPE_MASK;
    dir = wh->i_fc[1] & IEEE80211_FC1_DIR_MASK;
    ismcast = IEEE80211_IS_MULTICAST(wh->i_addr1);

    /*
     * This argument also includes bcast case. For bcast, addr1 and addr3 both with LSB 1,
     * so ismcast=1
     */

    if (unlikely((type == IEEE80211_FC0_TYPE_CTL) &&
        (subtype == IEEE80211_FC0_SUBTYPE_BAR))) {
        return ath_bar_rx(sc, an, wbuf);
    }

    /*
     * special aggregate processing only for qos unicast data frames
     */
    /*
     * Call the fast path in ap-only thread only for aggregated QoS unicast data frames.
     * Otherwise, call the generic rx path
     */
    if (unlikely(type != IEEE80211_FC0_TYPE_DATA ||
        subtype != IEEE80211_FC0_SUBTYPE_QOS || (ismcast) )) {
        __11nstats(sc, rx_nonqos);
        return ath_net80211_input(an->an_node, wbuf, rx_status);
    }

    /*
     * lookup rx tid state
     */
    whqos = (struct ieee80211_qosframe *) wh;
    tid = whqos->i_qos[0] & IEEE80211_QOS_TID;
    rxtid = &an->an_aggr.rx.tid[tid];

    /* if there at least one frame completed in video class, make
     * sure that we disable the PHY restart on BB hang.
     * Also make sure that we avoid the many register writes in
     * rx-tx path
     */

    ATH_RXTID_LOCK(rxtid);


    /*
     * If the ADDBA exchange has not been completed by the source,
     * process via legacy path (i.e. no reordering buffer is needed)
     */
    if (unlikely(!rxtid->addba_exchangecomplete)) {
        ATH_RXTID_UNLOCK(rxtid);
        __11nstats(sc, rx_nonqos);
        return ath_net80211_input(an->an_node, wbuf, rx_status);
    }

    /*
     * extract sequence number from recvd frame
     */
    rxseq = le16toh(*(u_int16_t *)wh->i_seq) >> IEEE80211_SEQ_SEQ_SHIFT;

    if (unlikely(rxtid->seq_reset)) {
        __11nstats(sc, rx_seqreset);
        rxtid->seq_reset = 0;
        rxtid->seq_next = rxseq;
    }

    index = ATH_BA_INDEX(rxtid->seq_next, rxseq);

    /*
     * drop frame if old sequence (index is too large)
     */
    if (unlikely(index > (IEEE80211_SEQ_MAX - (rxtid->baw_size << 2)))) {
        /*
         * discard frame, ieee layer may not treat frame as a dup
         */
        ATH_RXTID_UNLOCK(rxtid);
        __11nstats(sc, rx_oldseq);
        wbuf_free(wbuf);
        return IEEE80211_FC0_TYPE_DATA;
    }

    /*
     * sequence number is beyond block-ack window
     */
    if (unlikely(index >= rxtid->baw_size)) {

        __11nstats(sc, rx_bareset);

        /*
         * complete receive processing for all pending frames
         */
        while (index >= rxtid->baw_size) {

            rxbuf = rxtid->rxbuf + rxtid->baw_head;
            
            // Increment ahead, in case there is a flush tid from within rx_subframe.
            INCR(rxtid->baw_head, ATH_TID_MAX_BUFS);
            INCR(rxtid->seq_next, IEEE80211_SEQ_MAX);

            if (rxbuf->rx_wbuf != NULL) {
                wbuf_to_indicate = rxbuf->rx_wbuf;
                rxbuf->rx_wbuf = NULL; 
                __11nstats(sc, rx_baresetpkts);
                ath_net80211_input_aponly(an->an_node, wbuf_to_indicate,
                                             &rxbuf->rx_status);
                __11nstats(sc, rx_recvcomp);                             
            }

            index --;
        }
    }

    /*
     * add buffer to the recv ba window
     */
    cindex = (rxtid->baw_head + index) & (ATH_TID_MAX_BUFS - 1);
    rxbuf = rxtid->rxbuf + cindex;
    
    if (unlikely(rxbuf->rx_wbuf != NULL)) {
        /*
         *duplicate frame
         */
        DPRINTF(sc, ATH_DEBUG_ANY, "%s[%d]:Dup frame tid %d, cindex %d, baw_head %d, baw_tail %d, seq_next %d\n", __func__, __LINE__, 
            tid, cindex, rxtid->baw_head, rxtid->baw_tail, rxtid->seq_next); 
        ATH_RXTID_UNLOCK(rxtid);
        __11nstats(sc, rx_dup);
        wbuf_free(wbuf);
        return IEEE80211_FC0_TYPE_DATA;
    }

    rxbuf->rx_wbuf = wbuf;
    rxbuf->rx_time = OS_GET_TIMESTAMP();
    rxbuf->rx_status.flags = rx_status->flags;    
    rxbuf->rx_status.rssi = rx_status->rssi;    

    rxdiff = (rxtid->baw_tail - rxtid->baw_head) &
             (ATH_TID_MAX_BUFS - 1);

    /*
     * advance tail if sequence received is newer than any received so far
     */
    if (unlikely(index >= rxdiff)) {
        __11nstats(sc, rx_baadvance);
        rxtid->baw_tail = cindex;
        INCR(rxtid->baw_tail, ATH_TID_MAX_BUFS);
    }

    /*
     * indicate all in-order received frames
     */
    while (rxtid->baw_head != rxtid->baw_tail) {
        rxbuf = rxtid->rxbuf + rxtid->baw_head;
        if (unlikely(!rxbuf->rx_wbuf))
            break;

        __11nstats(sc, rx_recvcomp);

        INCR(rxtid->baw_head, ATH_TID_MAX_BUFS);
        INCR(rxtid->seq_next, IEEE80211_SEQ_MAX);
        
        wbuf_to_indicate = rxbuf->rx_wbuf;
        rxbuf->rx_wbuf = NULL;
        ath_net80211_input_aponly(an->an_node, wbuf_to_indicate, &rxbuf->rx_status);
    }

    /*
     * start a timer to flush all received frames if there are pending
     * receive frames
     */
    if (unlikely(rxtid->baw_head != rxtid->baw_tail)) {
        if (!ath_timer_is_active(&rxtid->timer)) {
            __11nstats(sc,rx_timer_starts);
            ath_set_timer_period(&rxtid->timer, sc->sc_rxtimeout[TID_TO_WME_AC(tid)]);
            ath_start_timer(&rxtid->timer);
        }
    } else {
        if (ath_timer_is_active(&rxtid->timer)) {
            __11nstats(sc,rx_timer_stops);
        }
        ath_cancel_timer(&rxtid->timer, CANCEL_NO_SLEEP);
    }
    
    ATH_RXTID_UNLOCK(rxtid);
    return IEEE80211_FC0_TYPE_DATA;
}

static inline int
ath_net80211_rx_aponly(ieee80211_handle_t ieee, wbuf_t wbuf, ieee80211_rx_status_t *rx_status, u_int16_t keyix)
{
    struct ieee80211com *ic = NET80211_HANDLE(ieee);
    struct ath_softc_net80211 *scn = ATH_SOFTC_NET80211(ic);
    struct ieee80211_node *ni;
    struct ieee80211_frame *wh;
    int type;
    /*
     * From this point on we assume the frame is at least
     * as large as ieee80211_frame_min; verify that.
     */
    if (unlikely(wbuf_get_pktlen(wbuf) < (ic->ic_minframesize + IEEE80211_CRC_LEN))) {
        DPRINTF(scn, ATH_DEBUG_RECV, "%s: short packet %d\n",
                    __func__, wbuf_get_pktlen(wbuf));
        wbuf_free(wbuf);
        return -1;
    }
    
#ifdef ATH_SUPPORT_TxBF
    ath_net80211_bf_rx(ic, wbuf, rx_status);
#endif
    /*
     * Normal receive.
     */
    wbuf_trim(wbuf, IEEE80211_CRC_LEN);
    /*
     * Locate the node for sender, track state, and then
     * pass the (referenced) node up to the 802.11 layer
     * for its use.  If the sender is unknown spam the
     * frame; it'll be dropped where it's not wanted.
     */
    IEEE80211_KEYMAP_LOCK(scn);
    ni = (keyix != HAL_RXKEYIX_INVALID) ? scn->sc_keyixmap[keyix] : NULL;
    if (ni == NULL) {
        IEEE80211_KEYMAP_UNLOCK(scn);
    /*
     * No key index or no entry, do a lookup and
     * add the node to the mapping table if possible.
     */
        ni = ieee80211_find_rxnode(ic, (struct ieee80211_frame_min *)
                               wbuf_header(wbuf));
        if (ni == NULL) {
            struct ieee80211_rx_status rs;
            rs.rs_flags =
                ((rx_status->flags & ATH_RX_FCS_ERROR) ? IEEE80211_RX_FCS_ERROR : 0) |
                ((rx_status->flags & ATH_RX_MIC_ERROR) ? IEEE80211_RX_MIC_ERROR : 0) |
                ((rx_status->flags & ATH_RX_DECRYPT_ERROR) ? IEEE80211_RX_DECRYPT_ERROR : 0)
            | ((rx_status->flags & ATH_RX_KEYMISS) ? IEEE80211_RX_KEYMISS : 0);

            rs.rs_phymode = ic->ic_curmode;
            rs.rs_freq = ic->ic_curchan->ic_freq;
            rs.rs_rssi = rx_status->rssi;
            rs.rs_abs_rssi = rx_status->abs_rssi;
            rs.rs_datarate = rx_status->rateKbps;
            rs.rs_isaggr = rx_status->isaggr;
            rs.rs_isapsd = rx_status->isapsd;
            rs.rs_noisefloor = rx_status->noisefloor;
            rs.rs_channel = rx_status->channel;   

            rs.rs_tstamp.tsf = rx_status->tsf;

            ath_txbf_update_rx_status(&rs, rx_status);
            rs.rs_rssi = rx_status->rssi;
#if ATH_VOW_EXT_STATS
            rs.vow_extstats_offset = rx_status->vow_extstats_offset;
#endif    
            return ieee80211_input_all(ic, wbuf, &rs);
        }

    /*
     * If the station has a key cache slot assigned
     * update the key->node mapping table.
     */
        IEEE80211_KEYMAP_LOCK(scn);

        keyix = ni->ni_ucastkey.wk_keyix;
        if (keyix != IEEE80211_KEYIX_NONE && scn->sc_keyixmap[keyix] == NULL &&
             ni->ni_ucastkey.wk_valid )
        scn->sc_keyixmap[keyix] = ni;//ieee80211_ref_node(ni);

        IEEE80211_KEYMAP_UNLOCK(scn);
    } else {
        ieee80211_ref_node(ni);
        IEEE80211_KEYMAP_UNLOCK(scn);
    }
    /*
     * update node statistics
     */
    wh = (struct ieee80211_frame *)wbuf_header(wbuf);
    if (unlikely(IEEE80211_IS_DATA(wh) && rx_status->nomoreaggr)) {
        ATH_RATE_LPF(ATH_NODE_NET80211(ni)->an_avgrxrate, rx_status->rateKbps);
    }

    /* For STA, update RSSI info from associated BSSID only. Don't update RSSI, if we
       recv pkt from another BSSID(probe resp etc.)
    */
    if (unlikely((rx_status->flags & ATH_RX_RSSI_VALID) && (rx_status->nomoreaggr) && IEEE80211_IS_DATA(wh))) {
        int i;

        ATH_RSSI_LPF(ATH_NODE_NET80211(ni)->an_avgrssi, rx_status->rssi);
        ATH_RSSI_LPF(ATH_NODE_NET80211(ni)->an_avgdrssi, rx_status->rssi);
        
        if (rx_status->flags & ATH_RX_CHAIN_RSSI_VALID) {
            for(i=0;i<ATH_MAX_ANTENNA;++i) {
                ATH_RSSI_LPF(ATH_NODE_NET80211(ni)->an_avgchainrssi[i], rx_status->rssictl[i]);
                ATH_RSSI_LPF(ATH_NODE_NET80211(ni)->an_avgdchainrssi[i], rx_status->rssictl[i]);
            }
            if (rx_status->flags & ATH_RX_RSSI_EXTN_VALID) {
                for(i=0;i<ATH_MAX_ANTENNA;++i) {
                    ATH_RSSI_LPF(ATH_NODE_NET80211(ni)->an_avgchainrssiext[i], rx_status->rssiextn[i]);
                    ATH_RSSI_LPF(ATH_NODE_NET80211(ni)->an_avgdchainrssiext[i], rx_status->rssiextn[i]);
                }
            }
        }
            
    }

    /*
     * Let ath_dev do some special rx frame processing. If the frame is not
     * consumed by ath_dev, indicate it up to the stack.
     */
    if (likely(IEEE80211_NODE_USEAMPDU(ni) && ((struct ath_softc *)scn->sc_dev)->sc_rxaggr)) {
#ifdef ATH_EXT_AP
        if (IEEE80211_VAP_IS_WDS_ENABLED(ni->ni_vap) || IEEE80211_VAP_IS_EXT_AP_ENABLED(ni->ni_vap)) {
#else
        if (IEEE80211_VAP_IS_WDS_ENABLED(ni->ni_vap)) {
#endif
            type = ath_ampdu_input(scn->sc_dev, ATH_NODE(ATH_NODE_NET80211(ni)->an_sta), wbuf, rx_status);
        } else {
            type = ath_ampdu_input_aponly(scn->sc_dev, ATH_NODE(ATH_NODE_NET80211(ni)->an_sta), wbuf, rx_status);
        }        
    } else {
        type = ath_net80211_input(ni, wbuf, rx_status);
    }
    
    ieee80211_free_node(ni);
    return type;
}

/*
 * Setup and link descriptors.
 *
 * 11N: we can no longer afford to self link the last descriptor.
 * MAC acknowledges BA status as long as it copies frames to host
 * buffer (or rx fifo). This can incorrectly acknowledge packets
 * to a sender if last desc is self-linked.
 *
 * NOTE: Caller should hold the rxbuf lock.
 */
/*
 * Add a wbuf from the free list to the rx fifo.
 * Context: Interrupt
 * NOTE: Caller should hold the rxbuf lock.
 */
static inline void
ath_rx_buf_link_aponly(struct ath_softc *sc, struct ath_buf *bf, HAL_RX_QUEUE qtype)
{
    struct ath_hal *ah = sc->sc_ah;
    struct ath_rx_edma *rxedma;

    rxedma = &sc->sc_rxedma[qtype];

//    ATH_RXBUF_RESET(bf);
#ifdef ATH_RX_DESC_WAR
	bf->bf_status = 0;
#endif
    /* Reset the status part */
    OS_MEMZERO(wbuf_raw_data(bf->bf_mpdu), sc->sc_rxstatuslen);

	/*
	** Since the descriptor header (48 bytes, which is 64 bytes, 2-3 cache lines
	** depending on alignment) is cached, we need to sync to ensure harware sees
	** the proper information, and we don't get inconsistent cache data.  So sync
	*/

     OS_SYNC_SINGLE(sc->sc_osdev, bf->bf_buf_addr[0], sc->sc_rxstatuslen,
                    BUS_DMA_TODEVICE, OS_GET_DMA_MEM_CONTEXT(bf, bf_dmacontext));

    rxedma->rxfifo[rxedma->rxfifotailindex] = bf->bf_mpdu;

    /* advance the tail pointer */
    INCR(rxedma->rxfifotailindex, rxedma->rxfifohwsize);

    rxedma->rxfifodepth++;

#ifdef ATH_SUPPORT_SPECTRAL
    if (rxedma->rxfifodepth >= 64)
        ath_edma_enable_spectral(sc);
#endif


    /* push this buffer in the MAC Rx fifo */
    ath_hal_putrxbuf(ah, bf->bf_buf_addr[0], qtype);

}

/*
 * XXX TODO the following is for non-edma case, need to add it later
 */
#if 0

static void
ath_rx_buf_link_aponly(struct ath_softc *sc, struct ath_buf *bf, bool rxenable)
{
    struct ath_hal *ah = sc->sc_ah;
    struct ath_desc *ds;
    wbuf_t wbuf;

    ATH_RXBUF_RESET(bf); 
    
    /* Acquire lock to have mutual exclusion with the Reset code calling ath_hal_reset */
    ATH_RESET_LOCK(sc);

    ds = bf->bf_desc;
    ds->ds_link = 0;    /* link to null */
    ds->ds_data = bf->bf_buf_addr[0];
    /* XXX For RADAR?
     * virtual addr of the beginning of the buffer. */
    wbuf = bf->bf_mpdu;
    ASSERT(wbuf != NULL);
    bf->bf_vdata = wbuf_raw_data(wbuf);

    /* setup rx descriptors */
    ath_hal_setuprxdesc(ah, ds
                        , wbuf_get_len(wbuf)   /* buffer size */
                        , 0
        );

#if ATH_RESET_SERIAL
    if (atomic_read(&sc->sc_hold_reset)) { //hold lock
        ATH_RESET_UNLOCK(sc);
        return;
    } else {
        if (atomic_read(&sc->sc_rx_hw_en) > 0) {
            atomic_inc(&sc->sc_rx_return_processing);
            ATH_RESET_UNLOCK(sc);
        } else {
            ATH_RESET_UNLOCK(sc);
            return;
        }
    }
    
#else

    if (atomic_read(&sc->sc_rx_hw_en) <= 0) {
        /* ath_stoprecv() has already being called. Do not queue to hardware. */
        ATH_RESET_UNLOCK(sc);
        return;
    }
#endif

    if (sc->sc_rxlink == NULL)
        ath_hal_putrxbuf(ah, bf->bf_daddr, 0);
    else
        *sc->sc_rxlink = bf->bf_daddr;

    sc->sc_rxlink = &ds->ds_link;

    if (rxenable && !atomic_read(&sc->sc_in_reset)) {
#ifdef DBG
        if (ath_hal_getrxbuf(ah, 0) == 0)
        {
            /* This will cause an NMI since RXDP is 0 */
            printk("%s: FATAL ERROR: NULL RXDP while enabling RX.\n", __func__);
            ASSERT(FALSE);
        } else
#endif
        ath_hal_rxena(ah);
    }
#if ATH_RESET_SERIAL
    atomic_dec(&sc->sc_rx_return_processing);
#else
    ATH_RESET_UNLOCK(sc);
#endif
}
#endif

static void
ath_rx_removebuffer_aponly(struct ath_softc *sc, HAL_RX_QUEUE qtype)
{
    int i, size;
    struct ath_buf *bf;
    wbuf_t wbuf;
    struct ath_rx_edma *rxedma;

    rxedma = &sc->sc_rxedma[qtype];

    size = rxedma->rxfifohwsize;

    /* Remove all buffers from rx queue and insert in free queue */
    for (i = 0; i < size; i++) {
        wbuf = rxedma->rxfifo[i];
        if (wbuf) {
            bf = ATH_GET_RX_CONTEXT_BUF(wbuf);
            if (!bf) {
                printk("%s[%d] PANIC wbuf %p Index %d\n", __func__, __LINE__, wbuf, i);
            } else {
                TAILQ_INSERT_TAIL(&sc->sc_rxbuf, bf, bf_list);
            }
            rxedma->rxfifo[i] = NULL;
            rxedma->rxfifodepth--;
        }
    }

    /* reset head and tail indices */
    rxedma->rxfifoheadindex = 0;
    rxedma->rxfifotailindex = 0;
    if (rxedma->rxfifodepth)
        printk("PANIC depth non-zero %d\n", rxedma->rxfifodepth);
}
static void
ath_rx_addbuffer_aponly(struct ath_softc *sc, HAL_RX_QUEUE qtype, int size)
{
    int i;
    struct ath_buf *bf, *tbf;
    struct ath_rx_edma *rxedma;

    rxedma = &sc->sc_rxedma[qtype];


    if (TAILQ_EMPTY(&sc->sc_rxbuf)) {
		DPRINTF(sc, ATH_DEBUG_RX_PROC, "%s[%d]: Out of buffers\n", __func__, __LINE__);
		return;
	}

    /* Add free buffers to rx queue */
    i = 0;
    TAILQ_FOREACH_SAFE(bf, &sc->sc_rxbuf, bf_list, tbf) {
        if (i == size)
            break;

        TAILQ_REMOVE(&sc->sc_rxbuf, bf, bf_list);
        if (bf == NULL) {
			DPRINTF(sc, ATH_DEBUG_RX_PROC, "%s[%d]: Out of buffers\n", __func__, __LINE__);
            break;
        }
        i++;
        ath_rx_buf_link_aponly(sc, bf, qtype);
    }
}

#ifdef ATH_SUPPORT_UAPSD
static inline void
ath_rx_process_uapsd_aponly(struct ath_softc *sc, HAL_RX_QUEUE qtype, wbuf_t wbuf, struct ath_rx_status *rxs, bool isr_context)
{
    struct ieee80211_qosframe    *qwh;

    if (!sc->sc_hwuapsdtrig) {
         /* Adjust wbuf start addr to point to data, i.e skip past the RxS */ 
         qwh = (struct ieee80211_qosframe *)
             ((u_int8_t *) wbuf_raw_data(wbuf) + sc->sc_rxstatuslen);

         /* HW Uapsd trig is not supported - Process all recv frames for uapsd triggers */
         sc->sc_ieee_ops->check_uapsdtrigger(sc->sc_ieee, qwh, rxs->rs_keyix, isr_context);
    }
    else if (qtype == HAL_RX_QUEUE_HP) {
         /* Adjust wbuf start addr to point to data, i.e skip past the RxS */
         qwh = (struct ieee80211_qosframe *)
             ((u_int8_t *) wbuf_raw_data(wbuf) + sc->sc_rxstatuslen);

         /* HW Uapsd trig is supported - do uapsd processing only for HP queue */
         sc->sc_ieee_ops->uapsd_deliverdata(sc->sc_ieee, qwh, rxs->rs_keyix,
                                            rxs->rs_isapsd, isr_context);
    }
}
#endif /* ATH_SUPPORT_UAPSD */


void
ath_rx_intr_aponly(ath_dev_t dev, HAL_RX_QUEUE qtype)
{
    struct ath_softc *sc = ATH_DEV_TO_SC(dev);
    struct ath_rx_edma *rxedma;
    wbuf_t wbuf;
    struct ath_buf *bf;
    struct ath_rx_status *rxs;
    HAL_STATUS retval;
    struct ath_hal *ah = sc->sc_ah;
    int    frames;
    struct ieee80211_node *ni;
    struct ieee80211com *ic = NET80211_HANDLE(sc->sc_ieee);
    struct ath_softc_net80211 *scn = ATH_SOFTC_NET80211(ic);
    struct ieee80211_qosframe    *qwh;
    u_int16_t keyix;
    rxedma = &sc->sc_rxedma[qtype];

    do {
        wbuf = rxedma->rxfifo[rxedma->rxfifoheadindex];
        if (unlikely(wbuf == NULL))
            break;
        bf = ATH_GET_RX_CONTEXT_BUF(wbuf);

        /*
         * Invalidate the status bytes alone since we flush them (to clear status) 
         * after unmapping the buffer while queuing it to h/w.
         */
        OS_SYNC_SINGLE(sc->sc_osdev,
                       bf->bf_buf_addr[0], sc->sc_rxstatuslen, BUS_DMA_FROMDEVICE,
                       OS_GET_DMA_MEM_CONTEXT(bf, bf_dmacontext));
        bf->bf_status |= ATH_BUFSTATUS_SYNCED;

        rxs = bf->bf_desc;
        retval = ath_hal_rxprocdescfast(ah, NULL, 0, NULL, rxs, wbuf_raw_data(wbuf));
#ifdef ATH_RX_DESC_WAR
        if (unlikely(HAL_EINVAL == retval)) {
			struct ath_buf *next_bf;
			wbuf_t next_wbuf;
			u_int32_t next_idx = rxedma->rxfifoheadindex;

			bf->bf_status |= ATH_BUFSTATUS_WAR;

			INCR(next_idx, rxedma->rxfifohwsize);
			next_wbuf = rxedma->rxfifo[next_idx];

			if (next_wbuf == NULL)
				break;

			next_bf = ATH_GET_RX_CONTEXT_BUF(next_wbuf);
			next_bf->bf_status |= ATH_BUFSTATUS_WAR;
			DPRINTF(sc, ATH_DEBUG_RX_PROC, "%s: Marking first DP 0x%x for drop\n",
				    __func__, (unsigned) bf->bf_buf_addr[0]);
			DPRINTF(sc, ATH_DEBUG_RX_PROC, "%s: Marking second DP 0x%x for drop\n",
			        __func__, (unsigned) next_bf->bf_buf_addr[0]);
		}
#endif
        /* XXX Check for done bit in RxS */
        if (HAL_EINPROGRESS == retval) {
            break;
        }
        /*
         * Locate the node for sender
         */
        keyix = rxs->rs_keyix;
         /* Adjust wbuf start addr to point to data, i.e skip past the RxS */ 
        qwh = (struct ieee80211_qosframe *)
             ((u_int8_t *) wbuf_raw_data(wbuf) + sc->sc_rxstatuslen);
        IEEE80211_KEYMAP_LOCK(scn);
        ni = (keyix != HAL_RXKEYIX_INVALID) ? scn->sc_keyixmap[keyix] : NULL;
        if (ni == NULL) {
            IEEE80211_KEYMAP_UNLOCK(scn);
            /*
             * No key index or no entry, do a lookup
             */
            ni = ieee80211_find_rxnode_nolock(ic, (struct ieee80211_frame_min *)qwh);
        } else {
            ieee80211_ref_node(ni);
            IEEE80211_KEYMAP_UNLOCK(scn);
        }
        if (ni) {
            /*
             * deref the ref count of the node
             */
            ieee80211_free_node(ni);
        }
#ifdef ATH_SUPPORT_UAPSD
        /* Process UAPSD triggers */
        /* Skip frames with error - except HAL_RXERR_KEYMISS since
         * for static WEP case, all the frames will be marked with HAL_RXERR_KEYMISS,
         * since there is no key cache entry added for associated station in that case
         */
        if (ni && 
                (ni->ni_flags & IEEE80211_NODE_UAPSD) &&
                (!(ni->ni_flags & IEEE80211_NODE_UAPSD_SP))) {
            if (likely((rxs->rs_status & ~HAL_RXERR_KEYMISS) == 0))
            {
                /* UAPSD frames being processed from ISR context */
                ath_rx_process_uapsd_aponly(sc, qtype, wbuf, rxs, true);
            }
        }
#endif /* ATH_SUPPORT_UAPSD */

        /* add this ath_buf for deferred processing */
        TAILQ_INSERT_TAIL(&rxedma->rxqueue, bf, bf_list);

        /* clear this element before advancing */
        rxedma->rxfifo[rxedma->rxfifoheadindex] = NULL;

        /* advance the head pointer */
        INCR(rxedma->rxfifoheadindex, rxedma->rxfifohwsize);

        if (unlikely(rxedma->rxfifodepth == 0))
            printk("ath_rx_intr: depth 0 PANIC\n");

        rxedma->rxfifodepth--;
#ifdef ATH_SUPPORT_SPECTRAL
        ath_edma_disable_spectral(sc, rxs);
#endif
    } while (TRUE);

    /*
     * remove ath_bufs from free list and add it to fifo
     */
    frames = rxedma->rxfifohwsize - rxedma->rxfifodepth;
    if (frames > 0)
        ath_rx_addbuffer_aponly(sc, qtype, frames);
}

/*
 * Rx interrupt handler with EDMA for ap-only
 */
void
ath_rx_edma_intr_aponly(ath_dev_t dev, HAL_INT status, int *sched)
{
    struct ath_softc *sc = ATH_DEV_TO_SC(dev);
    struct ath_hal *ah = sc->sc_ah;

    if (unlikely(status & HAL_INT_RXORN)) {
        sc->sc_stats.ast_rxorn++;
    }
    if (unlikely(status & HAL_INT_RXEOL)) {
        sc->sc_stats.ast_rxeol++;
    }
    if (likely(status & (HAL_INT_RXHP | HAL_INT_RXEOL | HAL_INT_RXORN))) {
        ath_rx_intr_aponly(dev, HAL_RX_QUEUE_HP);
        *sched = ATH_ISR_SCHED;
    }
    if (likely(status & (HAL_INT_RXLP | HAL_INT_RXEOL | HAL_INT_RXORN))) {
        ath_rx_intr_aponly(dev, HAL_RX_QUEUE_LP);
        *sched = ATH_ISR_SCHED;
    }

    /* Check if RXEOL condition was resolved */
    if (unlikely(status & HAL_INT_RXEOL)) {
        /* TODO - check rx fifo threshold here */
        if (sc->sc_rxedma[HAL_RX_QUEUE_HP].rxfifodepth == 0 || 
            sc->sc_rxedma[HAL_RX_QUEUE_LP].rxfifodepth == 0) {
            /* No buffers available - disable RXEOL/RXORN to avoid interrupt storm 
             * Disable and then enable to satisfy global isr enable reference counter 
             */
            ; //For further investigation
        }
        //BUG EV# 66955 Interrupt storm fix
        //Interrup bits must be cleared
#ifdef ATH_SUPPORT_SPECTRAL
        if (sc->sc_spectral_scan) {
            ath_hal_stop_spectral_scan(sc->sc_ah);
        }
#endif
        ath_hal_intrset(ah, 0);
        sc->sc_imask &= ~(HAL_INT_RXEOL | HAL_INT_RXORN);
        ath_hal_intrset(ah, sc->sc_imask);
    }
}

static inline int ath_rx_indicate_aponly(struct ath_softc *sc, wbuf_t wbuf, ieee80211_rx_status_t *status, u_int16_t keyix)
{
    struct ath_buf *bf = ATH_GET_RX_CONTEXT_BUF(wbuf);
    wbuf_t nwbuf;
    int type=-1;
    
    /* indicate frame to the stack, which will free the old wbuf. only indicate when we can get new buffer */
    wbuf_set_next(wbuf, NULL);
#if ATH_RXBUF_RECYCLE
	/*
	 * if ATH_RXBUF_RECYCLE is enabled to recycle the skb, 
	 * do the rx_indicate before we recycle the skb to avoid 
	 * skb competition and headline block of the recycle queue.
	 */
    type = ath_net80211_rx_aponly(sc->sc_ieee, wbuf, status, keyix);    
    nwbuf = (wbuf_t)(sc->sc_osdev->rbr_ops.osdev_wbuf_recycle((void *)sc));
    if (likely(nwbuf != NULL)) {
        bf->bf_mpdu = nwbuf;
		/*
		 * do not invalidate the cache for the new/recycled skb,
		 * because the cache will be invalidated in rx ISR/tasklet
		 */	
		bf->bf_buf_addr[0] = bf->bf_dmacontext = virt_to_phys(nwbuf->data);	
        ATH_SET_RX_CONTEXT_BUF(nwbuf, bf);
        /* queue the new wbuf to H/W */
        ath_rx_requeue(sc, nwbuf);
    } 
    return type;
#else /* !ATH_RXBUF_RECYCLE */
     /* allocate a new wbuf and queue it to for H/W processing */
   	nwbuf = ath_rxbuf_alloc(sc, sc->sc_rxbufsize);
    if (likely(nwbuf != NULL)) {
        type = ath_net80211_rx_aponly(sc->sc_ieee, wbuf, status, keyix);    
        bf->bf_mpdu = nwbuf;
		/*
		 * do not invalidate the cache for the new/recycled skb,
		 * because the cache will be invalidated in rx ISR/tasklet
		 */	
		bf->bf_buf_addr[0] = bf->bf_dmacontext = virt_to_phys(nwbuf->data);	
        ATH_SET_RX_CONTEXT_BUF(nwbuf, bf);
        /* queue the new wbuf to H/W */
        ath_rx_requeue(sc, nwbuf);
    } 
	else {

         /* Could not allocate the buffer
         * give the wbuf back 
	 	*
		 * do not invalidate the cache for the new/recycled skb,
		 * because the cache will be invalidated in rx ISR/tasklet
		 */	
		bf->bf_buf_addr[0] = bf->bf_dmacontext = virt_to_phys(wbuf->data);	

         /* queue back the old wbuf to H/W */
        ath_rx_requeue(sc, wbuf);        
      }
     return type;
#endif /* !ATH_RXBUF_RECYCLE */
}

static inline void
ath_rx_process_aponly(struct ath_softc *sc, struct ath_buf *bf, struct ath_rx_status *rxs, u_int8_t frame_fc0, 
        ieee80211_rx_status_t *rx_status, u_int8_t *chainreset)
{
    u_int16_t buf_len;
	wbuf_t wbuf = bf->bf_mpdu;
    int type;
#if ATH_VOW_EXT_STATS
    struct ath_phy_stats *phy_stats = &sc->sc_phy_stats[sc->sc_curmode];
#endif
#if defined(ATH_ADDITIONAL_STATS) || ATH_SLOW_ANT_DIV || defined(ATH_SUPPORT_TxBF)
    struct ieee80211_frame *wh;
    wh = (struct ieee80211_frame *) wbuf_header(wbuf);
#endif

#ifdef ATH_SUPPORT_TxBF
    ath_rx_bf_process(sc, bf, rxs, rx_status);
    if (ath_txbf_chk_rpt_frm(wh)){
        /* get time stamp for txbf report frame only*/
        rx_status->rpttstamp = rxs->rs_tstamp;
    } else {
        rx_status->rpttstamp = 0;
    }    
#endif


    /*
     * Sync and unmap the frame.  At this point we're
     * committed to passing the sk_buff somewhere so
     * clear buf_skb; this means a new sk_buff must be
     * allocated when the rx descriptor is setup again
     * to receive another frame.
     */
    buf_len = wbuf_get_pktlen(wbuf);
    
    rx_status->tsf = 0;
    rx_status->rateieee = sc->sc_hwmap[rxs->rs_rate].ieeerate;
    rx_status->rateKbps = sc->sc_hwmap[rxs->rs_rate].rateKbps;
    rx_status->ratecode = rxs->rs_rate;
    rx_status->nomoreaggr = rxs->rs_moreaggr ? 0:1;

    rx_status->isapsd = rxs->rs_isapsd;
    rx_status->noisefloor = (sc->sc_noise_floor == 0) ?
                   ATH_DEFAULT_NOISE_FLOOR : sc->sc_noise_floor;
    rx_status->channel = sc->sc_curchan.channel;

    /* HT rate */
    if (rx_status->ratecode & 0x80) {
    /* TODO - add table to avoid division */
    /* For each case, do division only one time */
        if (rxs->rs_flags & HAL_RX_2040) {
            rx_status->flags |= ATH_RX_40MHZ;
            if (rxs->rs_flags & HAL_RX_GI) {
                rx_status->rateKbps = (rx_status->rateKbps * 30) / 13;
            } else {
                rx_status->rateKbps = (rx_status->rateKbps * 27) / 13;
                rx_status->flags |= ATH_RX_SHORT_GI;
            }
        } else {
            if (rxs->rs_flags & HAL_RX_GI) {
                rx_status->rateKbps = (rx_status->rateKbps * 10) / 9;
            } else {
                rx_status->flags |= ATH_RX_SHORT_GI;
            }
        }
    }
    /* sc->sc_noise_floor is only available when the station attaches to an AP,
     * so we use a default value if we are not yet attached.
     */
    /* XXX we should use either sc->sc_noise_floor or
     * ath_hal_getChanNoise(ah, &sc->sc_curchan) 
     * to calculate the noise floor.
     * However, the value returned by ath_hal_getChanNoise seems to be incorrect
     * (-31dBm on the last test), so we will use a hard-coded value until we 
     * figure out what is going on.
     */
    if (rxs->rs_rssi != ATH_RSSI_BAD) {
        rx_status->abs_rssi = rxs->rs_rssi + ATH_DEFAULT_NOISE_FLOOR;
    }    

    if (unlikely(!(bf->bf_status & ATH_BUFSTATUS_SYNCED))) {
        OS_SYNC_SINGLE(sc->sc_osdev,
                   bf->bf_buf_addr[0], wbuf_get_pktlen(wbuf), BUS_DMA_FROMDEVICE,
                   OS_GET_DMA_MEM_CONTEXT(bf, bf_dmacontext));
        bf->bf_status |= ATH_BUFSTATUS_SYNCED;
    }
    /*
     * ast_ant_rx can only accommodate 8 antennas 
     */
    sc->sc_stats.ast_ant_rx[rxs->rs_antenna & 0x7]++;

#if ATH_SUPPORT_GREEN_AP
    /* This is the debug feature to print out the RSSI. This is the only 
     * way to check if the Rx chains are disabled and enabled correctly.
     */
    {
        sc->green_ap_dbg_rx_cnt++;
        if( !(sc->green_ap_dbg_rx_cnt & 0xff)) {
            DPRINTF(sc, ATH_DEBUG_GREEN_AP, "Rx rssi0 %d rssi1 %d rssi2 %d\n", 
                    (int8_t)rxs->rs_rssi_ctl0, 
                    (int8_t)rxs->rs_rssi_ctl1, 
                    (int8_t)rxs->rs_rssi_ctl2);        
        }
    }
#endif    
    if (likely(sc->sc_hashtsupport)) {
        rx_status->isaggr = rxs->rs_isaggr;        
        if (rxs->rs_moreaggr == 0) {
            int rssi_chain_valid_count = 0;
            int numchains = sc->sc_rx_numchains;

            if (rxs->rs_rssi != ATH_RSSI_BAD) {
                rx_status->rssi = rxs->rs_rssi;
                rx_status->flags |= ATH_RX_RSSI_VALID;
                sc->sc_stats.ast_rx_rssi = rxs->rs_rssi;
            }
            if (rxs->rs_rssi_ctl0 != ATH_RSSI_BAD) {
                rx_status->rssictl[0]  = rxs->rs_rssi_ctl0;
                rssi_chain_valid_count++;
                sc->sc_stats.ast_rx_rssi_ctl0 = rxs->rs_rssi_ctl0;
            }

            if (rxs->rs_rssi_ctl1 != ATH_RSSI_BAD) {
                rx_status->rssictl[1]  = rxs->rs_rssi_ctl1;
                rssi_chain_valid_count++;
                sc->sc_stats.ast_rx_rssi_ctl1 = rxs->rs_rssi_ctl1;
            }

            if (rxs->rs_rssi_ctl2 != ATH_RSSI_BAD) {
                rx_status->rssictl[2]  = rxs->rs_rssi_ctl2;
                rssi_chain_valid_count++;
                sc->sc_stats.ast_rx_rssi_ctl2 = rxs->rs_rssi_ctl2;
            }

            if (rxs->rs_flags & HAL_RX_2040) {
                int rssi_extn_valid_count = 0;
                if (rxs->rs_rssi_ext0 != ATH_RSSI_BAD) {
                    rx_status->rssiextn[0]  = rxs->rs_rssi_ext0;
                    rssi_extn_valid_count++;
                    sc->sc_stats.ast_rx_rssi_ext0 = rxs->rs_rssi_ext0;
                }
                if (rxs->rs_rssi_ext1 != ATH_RSSI_BAD) {
                    rx_status->rssiextn[1]  = rxs->rs_rssi_ext1;
                    rssi_extn_valid_count++;
                    sc->sc_stats.ast_rx_rssi_ext1 = rxs->rs_rssi_ext1;
                }
                if (rxs->rs_rssi_ext2 != ATH_RSSI_BAD) {
                    rx_status->rssiextn[2]  = rxs->rs_rssi_ext2;
                    rssi_extn_valid_count++;
                    sc->sc_stats.ast_rx_rssi_ext2 = rxs->rs_rssi_ext2;
                }
                if (rssi_extn_valid_count == numchains) {
                    rx_status->flags |= ATH_RX_RSSI_EXTN_VALID;
                }
            }
            if (rssi_chain_valid_count == numchains) {
                rx_status->flags |= ATH_RX_CHAIN_RSSI_VALID;
            }
        }
    } else {
        /*
         * Need to insert the "combined" rssi into the status structure
         * for upper layer processing
         */

         rx_status->rssi = rxs->rs_rssi;
         rx_status->flags |= ATH_RX_RSSI_VALID;
         rx_status->isaggr = 0;         
    }

#ifdef ATH_ADDITIONAL_STATS
    do {
		u_int8_t frm_type;
        u_int8_t frm_subtype;
        frm_type = wh->i_fc[0] & IEEE80211_FC0_TYPE_MASK;
        frm_subtype = wh->i_fc[0] & IEEE80211_FC0_SUBTYPE_MASK;
        if (frm_type == IEEE80211_FC0_TYPE_DATA ) {
            if (frm_subtype == IEEE80211_FC0_SUBTYPE_QOS) {
                struct ieee80211_qosframe *whqos;
                int tid;
                whqos = (struct ieee80211_qosframe *) wh;
				tid = whqos->i_qos[0] & IEEE80211_QOS_TID;
				sc->sc_stats.ast_rx_num_qos_data[tid]++;
            } else {
                sc->sc_stats.ast_rx_num_nonqos_data++;
            }
        }
    } while(0);
#endif

    if (unlikely(sc->sc_diversity)) {
        /*
         * When using hardware fast diversity, change the default rx
         * antenna if rx diversity chooses the other antenna 3
         * times in a row.
         */
        
        /*
         * TODO: vicks discusses with team regarding this 
         * beacuse of rx diversity def antenna is changing ..
         */

        if (sc->sc_defant != rxs->rs_antenna) {
            if (++sc->sc_rxotherant >= 3) {
#if UMAC_SUPPORT_SMARTANTENNA                
                if (!sc->sc_smartant_enable)
                    ath_setdefantenna(sc, rxs->rs_antenna);
#else
                    ath_setdefantenna(sc, rxs->rs_antenna);
#endif                
            }
        }
        else {
            sc->sc_rxotherant = 0;
        }
    }

    /* increment count of received bytes */
    /*
     * Increment rx_pkts count.
     */
    __11nstats(sc, rx_pkts);


#if ATH_SLOW_ANT_DIV
    if (sc->sc_slowAntDiv && (rx_status->flags & ATH_RX_RSSI_VALID) && IEEE80211_IS_BEACON(wh)) {
        ath_slow_ant_div(&sc->sc_antdiv, wh, rxs);
    }
#endif

#if ATH_ANT_DIV_COMB
        if (sc->sc_antDivComb)
        {
            ath_ant_div_comb_scan(&sc->sc_antcomb, rxs);   
        }
#endif

#if ATH_VOW_EXT_STATS
    /* make sure we do not corrupt non-decrypted frame */
    if (sc->sc_vowext_stats && !(rx_status->flags & ATH_RX_KEYMISS))
        ath_add_ext_stats(rxs, wbuf, sc, phy_stats, rx_status);
#endif //

    /*
     * Pass frames up to the stack.
     * Note: After calling ath_rx_indicate(), we should not assumed that the
     * contents of wbuf and wh are valid.
     */
    type = ath_rx_indicate_aponly(sc, wbuf, rx_status, rxs->rs_keyix);

#ifdef ATH_ADDITIONAL_STATS
    if (type == IEEE80211_FC0_TYPE_DATA) {
        sc->sc_stats.ast_rx_num_data++;
    }
    else if (type == IEEE80211_FC0_TYPE_MGT) {
        sc->sc_stats.ast_rx_num_mgmt++;
    }
    else if (type == IEEE80211_FC0_TYPE_CTL) {
        sc->sc_stats.ast_rx_num_ctl++;
    }
    else {
        sc->sc_stats.ast_rx_num_unknown++;
    }
#endif

    /* report data flow to LED module */
#if ATH_SUPPORT_LED || defined(ATH_BT_COEX)
    if (type == IEEE80211_FC0_TYPE_DATA) {
        int subtype = frame_fc0 & IEEE80211_FC0_SUBTYPE_MASK;
        if (subtype != IEEE80211_FC0_SUBTYPE_NODATA &&
            subtype != IEEE80211_FC0_SUBTYPE_QOS_NULL)
        {
#if ATH_SUPPORT_LED
            ath_led_report_data_flow(&sc->sc_led_control, buf_len);
#endif
#ifdef ATH_BT_COEX
            sc->sc_btinfo.wlanRxPktNum++;
#endif
        }
    }
#endif    
}

#if ATH_VOW_EXT_STATS
/*
 * Insert some stats info into the test packet's header.
 * 
 * Test packets are Data type frames in the Clear or encrypted
 *   with WPA2-PSK CCMP, identified by a specific length && UDP 
 *   && RTP && RTP eXtension && magic number.
 */
void
ath_add_ext_stats(struct ath_rx_status *rxs, wbuf_t wbuf,
                    struct ath_softc *sc, struct ath_phy_stats *phy_stats,
                    ieee80211_rx_status_t *rx_status)
{   
    /*
     * TODO: 
     *  - packet size is hardcoded, should be configurable
     *  - assumes no security, or WPA2-PSK CCMP security
     *  - EXT_HDR_SIZE is hardcoded, should calc from hdr size field
     *  - EXT_HDR fields are hardcoded, should be defined
     *  - EXT_HDR src fields are hardcoded, should be read from hdr
     */

#define IPV4_PROTO_OFF  38
#define UDP_PROTO_OFF   47
#define UDP_CKSUM_OFF   64
#define RTP_HDR_OFF     66
#define EXT_HDR_OFF     78
#define EXT_HDR_SIZE    ((8+1) * 4)     // should determine from ext hdr
#define AR_RCCNT        0x80f4   // Profile count receive clear
#define AR_CCCNT        0x80f8   // Profile count cycle counter
#define ATH_EXT_STAT_DFL_LEN 1434
#define IP_VER4_N_NO_EXTRA_HEADERS 0x45
#define IP_PDU_PROTOCOL_UDP 0x11
#define UDP_PDU_RTP_EXT    ((2 << 6) | (1 << 4))  /* RTP Version 2 + X bit */
    unsigned char *bp;
    u_int32_t reg_rccnt;
    u_int32_t reg_cccnt;
    struct ieee80211_frame *wh;
    u_int16_t seqctrl;
    u_int16_t buf_len;
    int test_len = ATH_EXT_STAT_DFL_LEN;  
    int frm_type, frm_subtype;
    int offset;

    wh = (struct ieee80211_frame *)wbuf_raw_data(wbuf);
    frm_type = wh->i_fc[0] & IEEE80211_FC0_TYPE_MASK;
    frm_subtype = wh->i_fc[0] & IEEE80211_FC0_SUBTYPE_MASK;
    seqctrl = *(u_int16_t *)(wh->i_seq);
    bp = (unsigned char *)wbuf_raw_data(wbuf);
    buf_len = wbuf_get_pktlen(wbuf);

    /* Ignore non Data Types */
    if (!(frm_type & IEEE80211_FC0_TYPE_DATA)) {
        return;
    }

    /* Adjust for WDS */
    if ((wh->i_fc[1] & IEEE80211_FC1_DIR_MASK) == IEEE80211_FC1_DIR_DSTODS) {
        bp += ETHER_ADDR_LEN;
        test_len += ETHER_ADDR_LEN;
        offset = 4;
    } else {
        offset = 2;
    }

    /* Adjust for QoS Header */
    if (!(frm_subtype & IEEE80211_FC0_SUBTYPE_QOS)) {
        offset += 4;
    }

    bp -= offset;
    test_len -= offset;
    
    /* Adjust for AES security */
    /* Assumes WPA2-PSK CCMP only if security enabled, else open */
    if (wh->i_fc[1] & IEEE80211_FC1_WEP) {
        bp += IEEE80211_WEP_IVLEN + IEEE80211_WEP_KIDLEN 
                    + IEEE80211_WEP_CRCLEN;
        test_len += 16;
    }
    
    /* only mark very specifc packets */
    if ((buf_len == test_len) &&
        (*(bp+RTP_HDR_OFF) == UDP_PDU_RTP_EXT) &&
        (*(bp+UDP_PROTO_OFF) == IP_PDU_PROTOCOL_UDP) &&
        (*(bp+IPV4_PROTO_OFF) == IP_VER4_N_NO_EXTRA_HEADERS))
    {
        /* Check for magic number and header length */
        if ((*(bp+EXT_HDR_OFF+0) == 0x12) &&
            (*(bp+EXT_HDR_OFF+1) == 0x34) &&
            (*(bp+EXT_HDR_OFF+2) == 0x00) &&
            (*(bp+EXT_HDR_OFF+3) == 0x08))
        {
            if(wh->i_fc[1] & IEEE80211_FC1_WEP)
            {
                /* don't clear the udp checksum here. In case of security, we may need
                * to do SW MIC on this packet. clear the checksum in ieee layer after
                * passing thru crpto layer. Store the udp checksum offset value in rx_stats */
                rx_status->vow_extstats_offset = (bp - (uint8_t *)wh) + UDP_CKSUM_OFF;
            }
            else
            {
                /* clear udp checksum so we do not have to recalculate it after
                 * filling in status fields */
                *(bp+UDP_CKSUM_OFF) = 0x00;
                *(bp+UDP_CKSUM_OFF+1) = 0x00;
            }
            
            reg_rccnt = OS_REG_READ(sc->sc_ah, AR_RCCNT);
            reg_cccnt = OS_REG_READ(sc->sc_ah, AR_CCCNT);

            bp += EXT_HDR_OFF + 12;  // skip hdr and src fields
            
            /* Store the ext stats offset in rx_status which will be used at the time of SW MIC */
            rx_status->vow_extstats_offset =  (rx_status->vow_extstats_offset) | (((uint32_t)(bp - (uint8_t *)wh)) << 16);

            *bp++ = rxs->rs_rssi_ctl0;
            *bp++ = rxs->rs_rssi_ctl1;
            *bp++ = rxs->rs_rssi_ctl2;
            *bp++ = rxs->rs_rssi_ext0;
            *bp++ = rxs->rs_rssi_ext1;
            *bp++ = rxs->rs_rssi_ext2;
            *bp++ = rxs->rs_rssi;

            *bp++ = (unsigned char)(rxs->rs_flags & 0xff);

            *bp++ = (unsigned char)((rxs->rs_tstamp >> 8) & 0x7f);
            *bp++ = (unsigned char)(rxs->rs_tstamp & 0xff);

            *bp++ = (unsigned char)((phy_stats->ast_rx_phyerr >> 8) & 0xff);
            *bp++ = (unsigned char)(phy_stats->ast_rx_phyerr & 0xff);

            *bp++ = (unsigned char)((reg_rccnt >> 24) & 0xff);
            *bp++ = (unsigned char)((reg_rccnt >> 16) & 0xff);
            *bp++ = (unsigned char)((reg_rccnt >>  8) & 0xff);
            *bp++ = (unsigned char)(reg_rccnt & 0xff);

            *bp++ = (unsigned char)((reg_cccnt >> 24) & 0xff);
            *bp++ = (unsigned char)((reg_cccnt >> 16) & 0xff);
            *bp++ = (unsigned char)((reg_cccnt >>  8) & 0xff);
            *bp++ = (unsigned char)(reg_cccnt & 0xff);

            *bp++ = rxs->rs_rate;
            *bp++ = rxs->rs_moreaggr;

            *bp++ = (unsigned char)((seqctrl >> 8) & 0xff);
            *bp++ = (unsigned char)(seqctrl & 0xff);
        }
    }
#undef AR_RCCNT
#undef AR_CCCNT
}
#endif // EXT_STATS




/*
 * Helper routine for ath_rx_edma_requeue
 * Context: ISR
\ */
struct ath_rx_edma_requeue_request {
    struct ath_softc *sc;
    struct ath_buf *bf;
};

/*
 * This routine adds a new buffer to the free list
 * Context: Tasklet
 */
inline void
ath_rx_edma_requeue_aponly(ath_dev_t dev, wbuf_t wbuf)
{
    struct ath_softc *sc = ATH_DEV_TO_SC(dev);
    struct ath_buf *bf = ATH_GET_RX_CONTEXT_BUF(wbuf);
    struct ath_hal *ah = sc->sc_ah;

    ASSERT(bf != NULL);

    ATH_RXBUF_LOCK(sc);

    TAILQ_INSERT_TAIL(&sc->sc_rxbuf, bf, bf_list);

    /* If RXEOL interrupts were disabled (due to no buffers available), re-enable RXEOL interrupts. */
    if (!(sc->sc_imask & HAL_INT_RXEOL)) {
        if (sc->sc_edmarxdpc) {
            /* In rxdpc - so do not enable interrupt, just set the sc_imask
             * interrupt gets enabled at the end of DPC
             */
            sc->sc_imask |= HAL_INT_RXEOL | HAL_INT_RXORN;
        }
        else {
            /* Disable and then enable to satisfy the global isr enable reference counter */
            ath_hal_intrset(ah, 0);
            sc->sc_imask |= HAL_INT_RXEOL | HAL_INT_RXORN;
            ath_hal_intrset(ah, sc->sc_imask);
        }
    }
    ATH_RXBUF_UNLOCK(sc);
}
/*
 * Process receive queue, as well as LED, etc.
 * Arg "flush":
 * 0: Process rx frames in rx interrupt.
 * 1: Drop rx frames in flush routine.
 * 2: Flush and indicate rx frames, must be synchronized with other flush threads.
 */
static int
ath_rx_handler_aponly(ath_dev_t dev, int flush, HAL_RX_QUEUE qtype)
{
    struct ath_softc *sc = ATH_DEV_TO_SC(dev);
    struct ath_rx_edma *rxedma;
    struct ath_buf *bf;
#if defined(ATH_SUPPORT_DFS) || defined(ATH_SUPPORT_SPECTRAL)
    struct ath_hal *ah = sc->sc_ah;
#endif
    struct ath_rx_status *rxs;
    void *ds;
    u_int phyerr;
    struct ieee80211_frame *wh;
    wbuf_t wbuf = NULL;
    ieee80211_rx_status_t rx_status;
    struct ath_phy_stats *phy_stats = &sc->sc_phy_stats[sc->sc_curmode];
    u_int8_t chainreset = 0;
    int rx_processed = 0;
    unsigned long flags;


    rxedma = &sc->sc_rxedma[qtype];
    
    do {
        /* If handling rx interrupt and flush is in progress => exit */
        if (unlikely(sc->sc_rxflush)) {
            break;
        }

        /* Get completed ath_buf from rxqueue. Must synchronize with the ISR */
        bf = NULL;
        ATH_RXQ_LOCK(rxedma);
        ATH_LOCK_IRQ(sc->sc_osdev);    
        local_irq_save(flags);  
        bf = TAILQ_FIRST(&rxedma->rxqueue);
        if (likely(bf)) {
            TAILQ_REMOVE(&rxedma->rxqueue, bf, bf_list);
        }
        local_irq_restore(flags);
        ATH_UNLOCK_IRQ(sc->sc_osdev); 
        ATH_RXQ_UNLOCK(rxedma);
        if (bf == NULL) {
            break;
        }

        wbuf = bf->bf_mpdu;
        if (unlikely(wbuf == NULL)) {		/* XXX ??? can this happen */
            continue;
        }
        ++rx_processed;

        rxs = (struct ath_rx_status *)(bf->bf_desc);

        /*
         * Save RxS location for packetlog.
         */
        ds = (void *)wbuf_raw_data(wbuf);

#ifdef ATH_RX_DESC_WAR
		if (unlikely(bf->bf_status & ATH_BUFSTATUS_WAR)) {
			DPRINTF(sc, ATH_DEBUG_RX_PROC, "%s: Dropping DP 0x%x\n",
                __func__, (unsigned) bf->bf_buf_addr[0]);
            goto rx_next;
		}
#endif

        OS_MEMZERO(&rx_status, sizeof(ieee80211_rx_status_t));

        /* point to the beginning of actual frame */
        bf->bf_vdata = (void *)((u_int8_t *)ds + sc->sc_rxstatuslen);

#ifndef REMOVE_PKT_LOG
        /* do pktlog */
        {
            struct log_rx log_data;
            log_data.ds = ds;
            log_data.status = rxs;
            log_data.bf = bf;
            ath_log_rx(sc, &log_data, 0);
        }
#endif

#ifdef ATH_SUPPORT_TxBF
        {//Check if Have H, V/CV upload from HW
            int next_do = ath_rx_bf_handler(dev, wbuf, rxs, bf);
            
            if (next_do == TX_BF_DO_RX_NEXT) {
                goto rx_next;
            } else if (next_do == TX_BF_DO_CONTINUE) {
                continue;
            }
        }
#endif
        if (unlikely((rxs->rs_status == 0) && (rxs->rs_more))) {
                        /*
                         * Frame spans multiple descriptors; this
                         * cannot happen yet as we don't support
                         * jumbograms.    If not in monitor mode,
                         * discard the frame.
                         */
              goto rx_next;
        }
        else { // if (rxs->rs_status != 0)
            if (unlikely(rxs->rs_status & HAL_RXERR_CRC)) {
                rx_status.flags |= ATH_RX_FCS_ERROR;
                phy_stats->ast_rx_crcerr++;
            }
            if (unlikely(rxs->rs_status & HAL_RXERR_FIFO))
                phy_stats->ast_rx_fifoerr++;
            if (unlikely(rxs->rs_status & HAL_RXERR_PHY)) {
                phy_stats->ast_rx_phyerr++;
                phyerr = rxs->rs_phyerr & 0x1f;
                phy_stats->ast_rx_phy[phyerr]++;
#ifdef ATH_SUPPORT_DFS
                {
                    u_int64_t tsf = ath_hal_gettsf64(ah);
                    /* Process phyerrs */
                    ath_process_phyerr(sc, bf, rxs, tsf);
                }
#endif

#if ATH_SUPPORT_SPECTRAL
                {
                    u_int64_t tsf = ath_hal_gettsf64(ah);
                    if (is_spectral_phyerr(sc, bf, rxs)) {
                        SPECTRAL_LOCK(sc->sc_spectral);
                        ath_process_spectraldata(sc, bf, rxs, tsf);
                        SPECTRAL_UNLOCK(sc->sc_spectral);          
                    }

                }
#endif  /* ATH_SUPPORT_SPECTRAL */

                goto rx_next;
            }

            if (unlikely(rxs->rs_status & HAL_RXERR_DECRYPT)) {
                /*
                 * Decrypt error. We only mark packet status here
                 * and always push up the frame up to let NET80211 layer
                 * handle the actual error case, be it no decryption key
                 * or real decryption error.
                 * This let us keep statistics there.
                 */
                 phy_stats->ast_rx_decrypterr++;
                rx_status.flags |= ATH_RX_DECRYPT_ERROR;
            } else if (unlikely(rxs->rs_status & HAL_RXERR_MIC)) {
                    rx_status.flags |= ATH_RX_MIC_ERROR;
            } 

            /*
             * Reject error frames with the exception of decryption, MIC,
             * and key-miss failures.
             * For monitor mode, we also ignore the CRC error.
             */
                if (unlikely(rxs->rs_status &
                    ~(HAL_RXERR_DECRYPT | HAL_RXERR_MIC | HAL_RXERR_KEYMISS))) {
                    goto rx_next;
                } else {
                    if (unlikely(rxs->rs_status & HAL_RXERR_KEYMISS)) {
                        rx_status.flags |= ATH_RX_KEYMISS;
                    }
                }
        }

            
        /*
         * Initialize wbuf; the length includes packet length
         * and status length. The status length later deducted
         * from the total len by the wbuf_pull
         */
        wbuf_init(wbuf, (rxs->rs_datalen + sc->sc_rxstatuslen));

        /*
         * Adjust wbuf start addr to point to data, i.e skip past the RxS.
         */
        wbuf_pull(wbuf, sc->sc_rxstatuslen);

        wh = (struct ieee80211_frame *)wbuf_header(wbuf);
        ath_rx_process_aponly(sc, bf, rxs, wh->i_fc[0], &rx_status, &chainreset);

        /*
         * For frames successfully indicated, the buffer will be
         * returned to us by upper layers by calling ath_rx_mpdu_requeue,
         * either synchronusly or asynchronously.
         * So we don't want to do it here in this loop.
         */
        continue;

rx_next:
    
        ath_rx_edma_requeue_aponly(dev, wbuf);
    } while (TRUE);

#ifdef ATH_SUPPORT_DFS
    if (sc->sc_dfs != NULL) {
        if (!STAILQ_EMPTY(&sc->sc_dfs->dfs_arq))
            dfs_process_ar_event(sc, &sc->sc_curchan);
        if (!STAILQ_EMPTY(&sc->sc_dfs->dfs_radarq)) {
            sc->sc_rtasksched = 1;
            OS_SET_TIMER(&sc->sc_dfs->sc_dfs_task_timer, 0);
        }
    }
#endif

#ifdef ATH_ADDITIONAL_STATS
    if (rx_processed < ATH_RXBUF ) {
        sc->sc_stats.ast_pkts_per_intr[rx_processed]++;
    }
    else {
        sc->sc_stats.ast_pkts_per_intr[ATH_RXBUF]++;
    }
#endif

    if (unlikely(chainreset)) {
        printk("Reset rx chain mask. Do internal reset. (%s)\n", __func__);
        ath_internal_reset(sc);
    }

    return 0;
}

/*
 * Deferred interrupt processing
 */
void
ath_handle_intr_aponly(ath_dev_t dev)
{
    struct ath_softc *sc = ATH_DEV_TO_SC(dev);
    u_int32_t status = sc->sc_intrstatus;
    u_int32_t rxmask;
    struct hal_bb_panic_info hal_bb_panic;
    struct ath_bb_panic_info bb_panic;
    int i;

    sc->sc_intrstatus &= (~status);

    ATH_PS_WAKEUP(sc);

    do {
        if (unlikely(sc->sc_invalid)) {
            /*
             * The hardware is not ready/present, don't touch anything.
             * Note this can happen early on if the IRQ is shared.
             */
            DPRINTF(sc, ATH_DEBUG_INTR, "%s called when invalid.\n",__func__);
            break;
        }

        if (unlikely(status & HAL_INT_FATAL)) {
            /* need a chip reset */
            DPRINTF(sc, ATH_DEBUG_INTR, "%s: Got fatal intr\n", __func__);
            sc->sc_reset_type = ATH_RESET_NOLOSS;
            ath_internal_reset(sc);
            sc->sc_reset_type = ATH_RESET_DEFAULT;
            break;
        } else {
            if (unlikely(status & HAL_INT_BBPANIC)) {
                if (!ath_hal_get_bbpanic_info(sc->sc_ah, &hal_bb_panic)) {
                    bb_panic.status = hal_bb_panic.status;
                    bb_panic.tsf = hal_bb_panic.tsf;
                    bb_panic.wd = hal_bb_panic.wd;
                    bb_panic.det = hal_bb_panic.det;
                    bb_panic.rdar = hal_bb_panic.rdar;
                    bb_panic.rODFM = hal_bb_panic.rODFM;
                    bb_panic.rCCK = hal_bb_panic.rCCK;
                    bb_panic.tODFM = hal_bb_panic.tODFM;
                    bb_panic.tCCK = hal_bb_panic.tCCK;
                    bb_panic.agc = hal_bb_panic.agc;
                    bb_panic.src = hal_bb_panic.src;
                    bb_panic.phy_panic_wd_ctl1 = hal_bb_panic.phy_panic_wd_ctl1;
                    bb_panic.phy_panic_wd_ctl2 = hal_bb_panic.phy_panic_wd_ctl2;
                    bb_panic.phy_gen_ctrl = hal_bb_panic.phy_gen_ctrl;
                    bb_panic.cycles = hal_bb_panic.cycles;
                    bb_panic.rxc_pcnt = hal_bb_panic.rxc_pcnt;
                    bb_panic.rxf_pcnt = hal_bb_panic.rxf_pcnt;
                    bb_panic.txf_pcnt = hal_bb_panic.txf_pcnt;
                    bb_panic.valid = 1;

                    for (i = 0; i < MAX_BB_PANICS - 1; i++)
                        sc->sc_stats.ast_bb_panic[i] = sc->sc_stats.ast_bb_panic[i + 1];
                    sc->sc_stats.ast_bb_panic[MAX_BB_PANICS - 1] = bb_panic;
                }

                if (!(ath_hal_handle_radar_bbpanic(sc->sc_ah)) ){
                    /* reset to recover from the BB hang */
                    sc->sc_reset_type = ATH_RESET_NOLOSS;
                    ATH_RESET_LOCK(sc);
                    ath_hal_set_halreset_reason(sc->sc_ah, HAL_RESET_BBPANIC);
                    ATH_RESET_UNLOCK(sc);
                    ath_internal_reset(sc);
                    ATH_RESET_LOCK(sc);
                    ath_hal_clear_halreset_reason(sc->sc_ah);
                    ATH_RESET_UNLOCK(sc);
                    sc->sc_reset_type = ATH_RESET_DEFAULT;
                    sc->sc_stats.ast_resetOnError++;
                    /* EV92527 -- we are doing internal reset. break out */
                    break;
                }
                /* EV 92527 -- We are not doing any internal reset, continue normally */
            }
#ifdef ATH_BEACON_DEFERRED_PROC
            /* Handle SWBA first */
            if (unlikely(status & HAL_INT_SWBA)) {
                int needmark = 0;
                ath_beacon_tasklet(sc, &needmark);
            }
#endif

            if (unlikely(((AH_TRUE == sc->sc_hang_check) && ath_hw_hang_check(sc)) ||
                (!sc->sc_noreset && (sc->sc_bmisscount >= (BSTUCK_THRESH_PERVAP * sc->sc_nvaps))))) {
                ath_bstuck_tasklet(sc);
                ATH_CLEAR_HANGS(sc);
                break;
            }
            /*
             * Howl needs DDR FIFO flush before any desc/dma data can be read.
             */
            ATH_FLUSH_FIFO();
            if (likely(sc->sc_enhanceddmasupport)) {
                rxmask = (HAL_INT_RXHP | HAL_INT_RXLP | HAL_INT_RXEOL | HAL_INT_RXORN);
            } else {
                rxmask = (HAL_INT_RX | HAL_INT_RXEOL | HAL_INT_RXORN);
            }

            if (likely(status & rxmask)
#if ATH_SUPPORT_RX_PROC_QUOTA
                    || (sc->sc_rx_work)
#endif
               ) {
#if ATH_SUPPORT_RX_PROC_QUOTA
                sc->sc_rx_work=0;
#endif 
                if (sc->sc_enhanceddmasupport) {
                    sc->sc_edmarxdpc = 1;
                    if (status & (HAL_INT_RXHP | HAL_INT_RXEOL | HAL_INT_RXORN)) {
                       ath_rx_handler_aponly(dev, 0, HAL_RX_QUEUE_HP);
                }
                    if (status & HAL_INT_RXLP) {
                        ath_rx_handler_aponly(dev, 0, HAL_RX_QUEUE_LP);
                }
                    sc->sc_edmarxdpc = 0;
                } else {
                ath_handle_rx_intr(sc);
            }
            }
            else if (sc->sc_rxfreebuf != NULL) {
                DPRINTF(sc, ATH_DEBUG_INTR, "%s[%d] ---- Athbuf FreeQ Not Empty - Calling AllocRxbufs for FreeList \n", __func__, __LINE__);
                // There are athbufs with no associated mbufs. Let's try to allocate some mbufs for these.            
                if (sc->sc_enhanceddmasupport) {
                    ath_edmaAllocRxbufsForFreeList(sc);
                }
                else {
                    ath_allocRxbufsForFreeList(sc);
                }
            }
#if ATH_TX_POLL
            if (sc->sc_enhanceddmasupport) {
                ath_tx_edma_tasklet_aponly(sc);
            }
            else {
                ath_tx_tasklet(sc);
            }
#else
            if (likely(status & HAL_INT_TX)) {
#ifdef ATH_TX_INACT_TIMER
                sc->sc_tx_inact = 0;
#endif
            if (sc->sc_enhanceddmasupport) {
                ath_tx_edma_tasklet_aponly(sc);
            }
            else {
                ath_tx_tasklet(sc);
            }
            }
#endif
            if (unlikely(status & HAL_INT_BMISS)) {
                ath_bmiss_tasklet(sc);
            }
            if (unlikely(status & HAL_INT_CST)) {
                ath_txto_tasklet(sc);
            }
            if (unlikely(status & (HAL_INT_TIM | HAL_INT_DTIMSYNC))) {
                if (status & HAL_INT_TIM) {
                    if (sc->sc_ieee_ops->proc_tim)
                        sc->sc_ieee_ops->proc_tim(sc->sc_ieee);
                }
                if (status & HAL_INT_DTIMSYNC) {
                    DPRINTF(sc, ATH_DEBUG_INTR, "%s: Got DTIMSYNC intr\n", __func__);
                }
            }
            if (unlikely(status & HAL_INT_GPIO)) {
#ifdef ATH_RFKILL
                ath_rfkill_gpio_intr(sc);
#endif
#ifdef ATH_BT_COEX
                if (unlikely(sc->sc_btinfo.bt_gpioIntEnabled)) {
                    ath_bt_coex_gpio_intr(sc);
                }
#endif
            }

        if (unlikely(ATH_ENAB_AOW(sc) && (status & HAL_INT_GENTIMER))) {
            ath_gen_timer_isr(sc);
        }

#ifdef ATH_GEN_TIMER
        
            if (unlikely(status & HAL_INT_TSFOOR)) {
                /* There is a jump in the TSF time with this OUT OF RANGE interupt. */
                DPRINTF(sc, ATH_DEBUG_ANY, "%s: Got HAL_INT_TSFOOR intr\n", __func__);

                /* If the current mode is Station, then we need to reprogram the beacon timers. */
                if (sc->sc_opmode  ==  HAL_M_STA) {
                    ath_beacon_config(sc,ATH_BEACON_CONFIG_REASON_RESET,ATH_IF_ID_ANY);
                }

                ath_gen_timer_tsfoor_isr(sc);
            }
            
            if (unlikely(status & HAL_INT_GENTIMER)) {
                #ifdef TARGET_SUPPORT_TSF_TIMER
                ath_gen_timer_isr(sc,0,0,0);
                #else
                ath_gen_timer_isr(sc);
                #endif
            }
#endif
        }

        /* re-enable hardware interrupt */
        if (likely(sc->sc_enhanceddmasupport)) {
            /* For enhanced DMA, certain interrupts are already enabled (e.g. RXEOL),
             * but now re-enable _all_ interrupts.
             * Note: disable and then enable to satisfy the global isr enable reference counter. 
             */
            ath_hal_intrset(sc->sc_ah, 0);
            ath_hal_intrset(sc->sc_ah, sc->sc_imask);
        } else {
            ath_hal_intrset(sc->sc_ah, sc->sc_imask);
        }
    } while (FALSE);

    ATH_PS_SLEEP(sc);
}


#ifndef ATH_SUPPORT_HTC
irqreturn_t
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,19)
ath_isr_aponly(int irq, void *dev_id)
#else
ath_isr_aponly(int irq, void *dev_id, struct pt_regs *regs)
#endif
{
    struct net_device *dev = dev_id;
    struct ath_softc_net80211 *scn = ath_netdev_priv(dev);
    int sched, needmark = 0;

    /* always acknowledge the interrupt */
   
    
    sched = ath_intr_aponly(scn->sc_dev);

    if(unlikely(sched == ATH_ISR_NOSCHED)) 
        return  IRQ_HANDLED;
    if(unlikely(sched == ATH_ISR_NOTMINE))
        return  IRQ_NONE;
   
    if (unlikely((dev->flags & (IFF_RUNNING|IFF_UP)) != (IFF_RUNNING|IFF_UP)))
    {
        DPRINTF_INTSAFE(scn, ATH_DEBUG_INTR, "%s: flags 0x%x\n", __func__, dev->flags);
     
        scn->sc_ops->disable_interrupt(scn->sc_dev);     /* disable further intr's */
        return IRQ_HANDLED;
    }
 
    
    /*
    ** See if the transmit queue processing needs to be scheduled
    */
    
    ATH_SCHEDULE_TQUEUE(&scn->sc_osdev->intr_tq, &needmark);
    if (needmark)
        mark_bh(IMMEDIATE_BH);

    return IRQ_HANDLED;
}
#endif

#ifndef ATH_UPDATE_COMMON_INTR_STATS
#define ATH_UPDATE_COMMON_INTR_STATS(sc, status)
#endif
#ifndef ATH_UPDATE_INTR_STATS
#define ATH_UPDATE_INTR_STATS(sc, intr)
#endif


static inline int
ath_common_intr_aponly(ath_dev_t dev, HAL_INT status)
{
    struct ath_softc *sc = ATH_DEV_TO_SC(dev);
    struct ath_hal *ah = sc->sc_ah;
    int sched = ATH_ISR_NOSCHED;

    ATH_UPDATE_COMMON_INTR_STATS(sc, status);

    do {
#ifdef ATH_MIB_INTR_FILTER
        /* Notify the MIB interrupt filter that we received some other interrupt. */
        if (likely(! (status & HAL_INT_MIB))) {
            ath_filter_mib_intr(sc, AH_FALSE);
        }
#endif

        if (unlikely(status & HAL_INT_FATAL)) {
            /* need a chip reset */
            sc->sc_stats.ast_hardware++;
            sched = ATH_ISR_SCHED;
        } else if (unlikely((status & HAL_INT_RXORN) && !sc->sc_enhanceddmasupport)) {
            /* need a chip reset? */
#if ATH_SUPPORT_DESCFAST
            ath_rx_proc_descfast(dev);
#endif
            sc->sc_stats.ast_rxorn++;
            sched = ATH_ISR_SCHED;
        } else {
            if (unlikely(status & HAL_INT_SWBA)) {
#ifdef ATH_BEACON_DEFERRED_PROC
                /* Handle beacon transmission in deferred interrupt processing */
                sched = ATH_ISR_SCHED;
#else
                int needmark = 0;

                /*
                 * Software beacon alert--time to send a beacon.
                 * Handle beacon transmission directly; deferring
                 * this is too slow to meet timing constraints
                 * under load.
                 */
                ath_beacon_tasklet(sc, &needmark);

                if (needmark) {
                    /* We have a beacon stuck. Beacon stuck processing
                     * should be done in DPC instead of here. */
                    sched = ATH_ISR_SCHED;
                }
#endif /* ATH_BEACON_DEFERRED_PROC */
                ATH_UPDATE_INTR_STATS(sc, swba);
            }
            if (unlikely(status & HAL_INT_TXURN)) {
                sc->sc_stats.ast_txurn++;
                /* bump tx trigger level */
                ath_hal_updatetxtriglevel(ah, AH_TRUE);
            }
            if (likely(sc->sc_enhanceddmasupport)) {
                ath_rx_edma_intr_aponly(sc, status, &sched);                 
                    }
            else {         
                if (unlikely(status & HAL_INT_RXEOL)) {
                    /*
                     * NB: the hardware should re-read the link when
                     *     RXE bit is written, but it doesn't work at
                     *     least on older hardware revs.
                     */
#if ATH_SUPPORT_DESCFAST
                    ath_rx_proc_descfast(dev);
#endif
                    sc->sc_imask &= ~(HAL_INT_RXEOL | HAL_INT_RXORN);
                    ath_hal_intrset(ah, sc->sc_imask);
                    sc->sc_stats.ast_rxeol++;
                    sched = ATH_ISR_SCHED;
                }
                if (likely(status & HAL_INT_RX)) {
                    ATH_UPDATE_INTR_STATS(sc, rx);
#if ATH_SUPPORT_DESCFAST
                    ath_rx_proc_descfast(dev);
#endif
                    sched = ATH_ISR_SCHED;
                }
            }

            if (likely(status & HAL_INT_TX)) {
                ATH_UPDATE_INTR_STATS(sc, tx);
                sched = ATH_ISR_SCHED;
            }
            if (unlikely(status & HAL_INT_BMISS)) {
                sc->sc_stats.ast_bmiss++;
                sched = ATH_ISR_SCHED;
            }
            if (unlikely(status & HAL_INT_GTT)) { /* tx timeout interrupt */
                sc->sc_stats.ast_txto++;
            }
            if (unlikely(status & HAL_INT_CST)) { /* carrier sense timeout */
                sc->sc_stats.ast_cst++;
                sched = ATH_ISR_SCHED;
            }

            if (unlikely(status & HAL_INT_MIB)) {
                sc->sc_stats.ast_mib++;
                /*
                 * Disable interrupts until we service the MIB
                 * interrupt; otherwise it will continue to fire.
                 */
                ath_hal_intrset(ah, 0);

#ifdef ATH_MIB_INTR_FILTER
                /* Check for bursts of MIB interrupts */
                ath_filter_mib_intr(sc, AH_TRUE);
#endif

                /*
                 * Let the hal handle the event.  We assume it will
                 * clear whatever condition caused the interrupt.
                 */
                ath_hal_mibevent(ah, &sc->sc_halstats);
                ath_hal_intrset(ah, sc->sc_imask);
            }
            if (unlikely(status & HAL_INT_GPIO)) {
                ATH_UPDATE_INTR_STATS(sc, gpio);
                /* Check if this GPIO interrupt is caused by RfKill */
#ifdef ATH_RFKILL
                if (ath_rfkill_gpio_isr(sc))
                    sched = ATH_ISR_SCHED;
#endif
                if (sc->sc_wpsgpiointr) {
                    /* Check for WPS push button press (GPIO polarity low) */
                    if (ath_hal_gpioget(sc->sc_ah, sc->sc_reg_parm.wpsButtonGpio) == 0) {
                        sc->sc_wpsbuttonpushed = 1;

                        /* Disable associated GPIO interrupt to prevent flooding */
                        ath_hal_gpioSetIntr(ah, sc->sc_reg_parm.wpsButtonGpio, HAL_GPIO_INTR_DISABLE);
                        sc->sc_wpsgpiointr = 0;
                    }
                }
#ifdef ATH_BT_COEX
                if (sc->sc_btinfo.bt_gpioIntEnabled) {
                    sched = ATH_ISR_SCHED;
                }
#endif
            }
            if (unlikely(status & HAL_INT_TIM_TIMER)) {
                ATH_UPDATE_INTR_STATS(sc, tim_timer);
                if (! sc->sc_hasautosleep) {
                    /* Clear RxAbort bit so that we can receive frames */
                    ath_hal_setrxabort(ah, 0);
                    /* Set flag indicating we're waiting for a beacon */
                    sc->sc_waitbeacon = 1;

                    sched = ATH_ISR_SCHED;
                }
            }
#ifdef ATH_GEN_TIMER
            if (unlikely(status & HAL_INT_GENTIMER)) {
                ATH_UPDATE_INTR_STATS(sc, gentimer);
                /* generic TSF timer interrupt */
                sched = ATH_ISR_SCHED;
            }
#endif

            if (unlikely(status & HAL_INT_TSFOOR)) {
                ATH_UPDATE_INTR_STATS(sc, tsfoor);
                DPRINTF(sc, ATH_DEBUG_PWR_SAVE,
                        "%s: HAL_INT_TSFOOR - syncing beacon\n",
                        __func__);
                /* Set flag indicating we're waiting for a beacon */
                sc->sc_waitbeacon = 1;

                sched = ATH_ISR_SCHED;
            }

            if (unlikely(status & HAL_INT_BBPANIC)) {
                ATH_UPDATE_INTR_STATS(sc, bbevent);
                /* schedule the DPC to get bb panic info */
                sched = ATH_ISR_SCHED;
            }

        }
    } while (0);

    if (likely(sched == ATH_ISR_SCHED)) {
        DPRINTF_INTSAFE(sc, ATH_DEBUG_INTR, "%s: Scheduling BH/DPC\n",__func__);
        if (likely(sc->sc_enhanceddmasupport)) {
            /* For enhanced DMA turn off all interrupts except RXEOL, RXORN, SWBA.
             * Disable and then enable to satisfy the global isr enable reference counter. 
             */
            ath_hal_intrset(ah, 0);
            ath_hal_intrset(ah, sc->sc_imask & (HAL_INT_GLOBAL | HAL_INT_RXEOL | HAL_INT_RXORN | HAL_INT_SWBA));
        } else {
#ifdef ATH_BEACON_DEFERRED_PROC
            /* turn off all interrupts */
            ath_hal_intrset(ah, 0);
#else
            /* turn off every interrupt except SWBA */
            ath_hal_intrset(ah, (sc->sc_imask & HAL_INT_SWBA));
#endif
        }
    }

    return sched;
}


int
ath_intr_aponly(ath_dev_t dev)
{
    struct ath_softc *sc = ATH_DEV_TO_SC(dev);
    struct ath_hal *ah = sc->sc_ah;
    HAL_INT status;
    int    isr_status = ATH_ISR_NOTMINE;

    atomic_inc(&sc->sc_inuse_cnt);

    do {
        if (unlikely(!ath_hal_intrpend(ah))) {    /* shared irq, not for us */
            isr_status = ATH_ISR_NOTMINE;
            break;
        }

        if (unlikely(sc->sc_invalid)) {
            /*
             * The hardware is either not ready or is entering full sleep,
             * don't touch any RTC domain register.
             */
            ath_hal_intrset_nortc(ah, 0);
            ath_hal_getisr_nortc(ah, &status, 0, 0);
            isr_status = ATH_ISR_NOSCHED;
            DPRINTF_INTSAFE(sc, ATH_DEBUG_INTR, "%s: recv interrupts when invalid.\n",__func__);
            break;
        }

        /*
         * Figure out the reason(s) for the interrupt.  Note
         * that the hal returns a pseudo-ISR that may include
         * bits we haven't explicitly enabled so we mask the
         * value to insure we only process bits we requested.
         */
        ath_hal_getisr(ah, &status, HAL_INT_LINE, 0);       /* NB: clears ISR too */
        DPRINTF_INTSAFE(sc, ATH_DEBUG_INTR, "%s: status 0x%x  Mask: 0x%x\n",
                __func__, status, sc->sc_imask);

        status &= sc->sc_imask;            /* discard unasked-for bits */

        /*
        ** If there are no status bits set, then this interrupt was not
        ** for me (should have been caught above).
        */

        if(unlikely(!status))
        {
            DPRINTF_INTSAFE(sc, ATH_DEBUG_INTR, "%s: Not My Interrupt\n",__func__);
            isr_status = ATH_ISR_NOSCHED;
            break;
        }

        sc->sc_intrstatus |= status;

        isr_status = ath_common_intr_aponly(dev, status);
    } while (FALSE);

    atomic_dec(&sc->sc_inuse_cnt);

    return isr_status;
}



static inline void
ieee80211_set_tim_aponly(struct ieee80211_node *ni, int set)
{
    struct ieee80211vap *vap = ni->ni_vap;
    u_int16_t aid;

    KASSERT(vap->iv_opmode == IEEE80211_M_HOSTAP 
            || vap->iv_opmode == IEEE80211_M_IBSS ,
            ("operating mode %u", vap->iv_opmode));

    aid = IEEE80211_AID(ni->ni_associd);
    KASSERT(aid < vap->iv_max_aid,
            ("bogus aid %u, max %u", aid, vap->iv_max_aid));

    if (set != (isset(vap->iv_tim_bitmap, aid) != 0)) {
        if (set) {
            setbit(vap->iv_tim_bitmap, aid);
            vap->iv_ps_pending++;
        } else {
            clrbit(vap->iv_tim_bitmap, aid);
            vap->iv_ps_pending--;
        }
        IEEE80211_VAP_TIMUPDATE_ENABLE(vap);
    }
}


static inline void
ath_uapsd_pwrsave_check_aponly(wbuf_t wbuf, struct ieee80211_node *ni)
{
    wlan_if_t vap = ni->ni_vap;
    if (WME_UAPSD_AC_ISDELIVERYENABLED(wbuf_get_priority(wbuf), ni))
    {
        /* U-APSD power save queue for delivery enabled AC */
        wbuf_set_uapsd(wbuf);
        wbuf_set_moredata(wbuf);
        IEEE80211_NODE_STAT(ni, tx_uapsd);

        if ((vap->iv_set_tim != NULL) && IEEE80211_NODE_UAPSD_USETIM(ni)) {
            ieee80211_set_tim_aponly(ni, 1);
        }
    }
}


static inline struct ieee80211_node *
#ifdef IEEE80211_DEBUG_REFCNT
_ieee80211_find_node_debug_aponly(struct ieee80211_node_table *nt, const u_int8_t *macaddr,
                     const char *func, int line)
#else
_ieee80211_find_node_aponly(struct ieee80211_node_table *nt, const u_int8_t *macaddr)
#endif
{
    struct ieee80211_node *ni;
    int hash;

    hash = IEEE80211_NODE_HASH(macaddr);
    LIST_FOREACH(ni, &nt->nt_hash[hash], ni_hash) {
        if (IEEE80211_ADDR_EQ(ni->ni_macaddr, macaddr)) {
            ieee80211_ref_node(ni);	/* mark referenced */
#ifdef IEEE80211_DEBUG_REFCNT
            ieee80211_note(ni->ni_vap,"%s ,line %u: increase node %p <%s> refcnt to %d\n",
                   func, line, ni,
                   ether_sprintf(ni->ni_macaddr),
                   ieee80211_node_refcnt(ni));
#endif
            return ni;
        }       
    }
    return NULL;
}


/*
 * Check if an ADDBA is required.
 */
static inline int
ath_aggr_check_aponly(ath_dev_t dev, ath_node_t node, u_int8_t tidno)
{
    struct ath_softc *sc = ATH_DEV_TO_SC(dev);
    struct ath_node *an = ATH_NODE(node);
    struct ath_atx_tid *tid;

#ifdef ATH_RIFS
    if (!sc->sc_txaggr && !sc->sc_txrifs)
#else
    if (!sc->sc_txaggr)
#endif
        return 0;
    
    /* ADDBA exchange must be completed before sending aggregates */
    tid = ATH_AN_2_TID(an, tidno);

    if (tid->cleanup_inprogress)
        return 0;
    
    if (!tid->addba_exchangecomplete) {
        if (!tid->addba_exchangeinprogress &&
            (tid->addba_exchangeattempts < ADDBA_EXCHANGE_ATTEMPTS)) {
            tid->addba_exchangeattempts++;
            return 1;
        }
    }
    return 0;
}

/*
 *  Get transmit rate index from ieee rate
 */
static inline
u_int8_t ath_rate_findrix_aponly(const HAL_RATE_TABLE *rt , u_int8_t ieee_rate)
{
    u_int8_t i, rix = 0;

    for(i=0; i<rt->rateCount; i++) {
        if ((rt->info[i].dot11Rate & IEEE80211_RATE_VAL) == ieee_rate) {
            rix = i;
            break;
        }
    }
    return rix;
}

/*
 * Get transmit rate index using rate in Kbps
 */
static inline int
ath_tx_findindex_aponly(const HAL_RATE_TABLE *rt, int rate)
{
    int i;
    int ndx = 0;

    for (i = 0; i < rt->rateCount; i++) {
        if (rt->info[i].rateKbps == rate) {
            ndx = i;
            break;
        }
    }

    return ndx;
}

/*
 * Insert a chain of ath_buf (descriptors) on a multicast txq
 * but do NOT start tx DMA on this queue.
 * NB: must be called with txq lock held
 */
static inline void
ath_tx_mcastqaddbuf_internal_aponly(struct ath_softc *sc, struct ath_txq *txq, ath_bufhead *head)
{
#define DESC2PA(_sc, _va)	\
		((caddr_t)(_va) - (caddr_t)((_sc)->sc_txdma.dd_desc) + \
				(_sc)->sc_txdma.dd_desc_paddr)		
    struct ath_hal *ah = sc->sc_ah;
    struct ath_buf *bf, *tbf;

    /*
     * Insert the frame on the outbound list and
     * pass it on to the hardware.
     */
    bf = TAILQ_FIRST(head);
    if (bf == NULL)
        return;

    /*
     * The CAB queue is started from the SWBA handler since
     * frames only go out on DTIM and to avoid possible races.
     */
    ath_hal_intrset(ah, 0);

    /*
    ** If there is anything in the mcastq, we want to set the "more data" bit
    ** in the last item in the queue to indicate that there is "more data".  This
    ** is an alternate implementation of changelist 289513 put within the code
    ** to add to the mcast queue.  It makes sense to add it here since you are
    ** *always* going to have more data when adding to this queue, no matter where
    ** you call from.
    */

    if (txq->axq_depth) {
        struct ath_buf *lbf;
        struct ieee80211_frame  *wh;

        /*
        ** Add the "more data flag" to the last frame
        */

        lbf = TAILQ_LAST(&txq->axq_q,ath_bufhead_s);
        wh = (struct ieee80211_frame *)wbuf_header(lbf->bf_mpdu);
        wh->i_fc[1] |= IEEE80211_FC1_MORE_DATA;
    }
    
    
    TAILQ_FOREACH(tbf, head, bf_list) {		
		OS_SYNC_SINGLE(sc->sc_osdev, tbf->bf_daddr, 
						sc->sc_txdesclen, BUS_DMA_TODEVICE, NULL);
	}	

    /*
    ** Now, concat the frame onto the queue
    */
    ATH_TXQ_CONCAT(txq, head);
    DPRINTF(sc, ATH_DEBUG_TX_PROC, "%s: txq depth = %d\n", __func__, txq->axq_depth);
    if (!sc->sc_enhanceddmasupport) {
        if (txq->axq_link != NULL) {
#ifdef AH_NEED_DESC_SWAP
            *txq->axq_link = cpu_to_le32(bf->bf_daddr);
#else
            *txq->axq_link = bf->bf_daddr;
#endif
			OS_SYNC_SINGLE(sc->sc_osdev, (dma_addr_t)(DESC2PA(sc, txq->axq_link)), 
						sizeof(u_int32_t *), BUS_DMA_TODEVICE, NULL);
            DPRINTF(sc, ATH_DEBUG_XMIT, "%s: link[%u](%p)=%llx (%p)\n",
                    __func__,
                    txq->axq_qnum, txq->axq_link,
                    ito64(bf->bf_daddr), bf->bf_desc);
        }
        ath_hal_getdesclinkptr(ah, bf->bf_lastbf->bf_desc, &(txq->axq_link));
    } else {
        if (txq->axq_link != NULL) {
            ath_hal_setdesclink(ah, txq->axq_link, bf->bf_daddr);
			OS_SYNC_SINGLE(sc->sc_osdev, (dma_addr_t)(DESC2PA(sc, txq->axq_link)), 
						sc->sc_txdesclen, BUS_DMA_TODEVICE, NULL);
            DPRINTF(sc, ATH_DEBUG_XMIT, "%s: link[%u](%p)=%llx (%p)\n",
                    __func__,
                    txq->axq_qnum, txq->axq_link,
                    ito64(bf->bf_daddr), bf->bf_desc);
        }
        txq->axq_link = bf->bf_lastbf->bf_desc;
    }
    ath_hal_intrset(ah, sc->sc_imask);
#undef DESC2PA
}


static ath_get_buf_status_t
ath_tx_get_buf_aponly(struct ath_softc *sc, sg_t *sg, struct ath_buf **pbf,
               ath_bufhead *bf_head, u_int32_t *buf_used)
{
    struct ath_buf *bf = *pbf;

    if (likely(!bf || !bf->bf_avail_buf)) {
        ATH_TXBUF_LOCK(sc);
        bf = TAILQ_FIRST(&sc->sc_txbuf);
        if (bf == NULL) {
            ATH_TXBUF_UNLOCK(sc);
            return ATH_BUF_NONE;
        }
        *pbf = bf;
        TAILQ_REMOVE(&sc->sc_txbuf, bf, bf_list);
        sc->sc_txbuf_free--;
		(*buf_used)++;
        ATH_TXBUF_UNLOCK(sc);
        TAILQ_INSERT_TAIL(bf_head, bf, bf_list);

        /* set up this buffer */
        ATH_TXBUF_RESET(bf, sc->sc_num_txmaps);
    }

    bf->bf_buf_addr[sc->sc_num_txmaps - bf->bf_avail_buf] = sg_dma_address(sg);
    bf->bf_buf_len[sc->sc_num_txmaps - bf->bf_avail_buf] = sg_dma_len(sg);

    bf->bf_avail_buf--;

    if (likely(bf->bf_avail_buf))
        return ATH_BUF_CONT;
    else
        return ATH_BUF_LAST;
}



#define MIN_BUF_RESV 16     /* Minimum buffers reserved per AC. This is to
                             * provide some breathing space for low priority
                             * traffic when high priority traffic is flooding */


/*
 * The function that actually starts the DMA.
 * It will either be called by the wbuf_map() function,
 * or called in a different thread if asynchronus DMA
 * mapping is used (NDIS 6.0).
 */
static inline int
ath_tx_start_dma_aponly(wbuf_t wbuf, sg_t *sg, u_int32_t n_sg, void *arg)
{
    ieee80211_tx_control_t *txctl = (ieee80211_tx_control_t *)arg;
    struct ath_softc *sc = (struct ath_softc *)txctl->dev;
    struct ath_node *an = txctl->an;
    struct ath_buf *bf = NULL, *firstbf=NULL;
    ath_bufhead bf_head;
    void *ds, *firstds = NULL, *lastds = NULL;
    struct ath_hal *ah = sc->sc_ah;
    struct ath_txq *txq = &sc->sc_txq[txctl->qnum];
    size_t i;
    struct ath_rc_series *rcs;
    int send_to_cabq = 0;
    struct ath_vap *avp = sc->sc_vaps[txctl->if_id];

#ifdef ATH_SUPPORT_TxBF
#ifdef TXBF_TODO
	if (sc->only_bf_cal_allow) {
		struct ieee80211_frame *wh;
		wh = (struct ieee80211_frame *) wbuf_header(wbuf);
		DPRINTF(sc, ATH_DEBUG_ANY,"Flag On==only_bf_cal_allow(%x)============== \n",
            sc->only_bf_cal_allow);
		if ((wh->i_fc[1]&IEEE80211_FC1_ORDER) && (wh->i_fc[0]&IEEE80211_FC0_SUBTYPE_QOS_NULL)) {
			DPRINTF(sc, ATH_DEBUG_ANY,"%s:Allowed Packet--------------\n", __func__);
		} else {
			DPRINTF(sc, ATH_DEBUG_ANY,"%s:+++++++Packet to be Skipped+++\n", __func__);
			return -ENOMEM;
		}			
	}
#endif
#endif

    u_int32_t *buf_used;
    buf_used = &txq->axq_num_buf_used;
    atomic_inc(&an->an_active_tx_cnt);
    if (unlikely(an->an_flags & ATH_NODE_CLEAN)) {
        atomic_dec(&an->an_active_tx_cnt);
        return -EIO;
    }

    if (unlikely(txctl->ismcast)) {
        /*
         * When servicing one or more stations in power-save mode (or)
         * if there is some mcast data waiting on mcast queue
         * (to prevent out of order delivery of mcast,bcast packets)
         * multicast frames must be buffered until after the beacon.
         * We use the private mcast queue for that.
         */
        if (txctl->ps || avp->av_mcastq.axq_depth) {
            send_to_cabq = 1;
            buf_used = &sc->sc_cabq->axq_num_buf_used;
        }
    }
    /*
     * This the using of tx_buf flow control for different priority 
     * queue. It is critical for WMM. Without this flow control,
     * at lease for Linux and Maverick STA, WMM will fail even HW WMM queue
     * works properly. Also the sc_txbuf_free counter must be count
     * precisely, otherwise, tx_buf leak may happen or this flow control
     * may not work.
     */
    if (unlikely((*buf_used > MIN_BUF_RESV) &&
            (sc->sc_txbuf_free < txq->axq_minfree)))
    {
		{
#if ATH_SUPPORT_FLOWMAC_MODULE
            /* check if OS can be told to stop sending frames */
            if (!sc->sc_osnetif_flowcntrl) {
#endif
                sc->sc_stats.ast_tx_nobuf++;
                sc->sc_stats.ast_txq_nobuf[txctl->qnum]++;
                atomic_dec(&an->an_active_tx_cnt);
                return -ENOMEM;
#if ATH_SUPPORT_FLOWMAC_MODULE
            } else {
                /* inform kernel to stop sending the frames down to ath
                 * layer and try to send this frame alone.
                 */
                if (sc->sc_osnetif_flowcntrl) {
                    ath_netif_stop_queue(sc);
                }
            }
#endif
            }
        }

    /* For each sglist entry, allocate an ath_buf for DMA */
    TAILQ_INIT(&bf_head);
    for (i = 0; i < n_sg; i++, sg++) {
        int more_maps;
        ath_get_buf_status_t retval;

        more_maps = (n_sg - i) > 1;//more than one descriptor
        retval = ath_tx_get_buf_aponly(sc, sg, &bf, &bf_head, buf_used);
        if (unlikely(more_maps && (ATH_BUF_CONT == retval))) {
            continue;
        } else if (unlikely(ATH_BUF_NONE == retval)) {
            DPRINTF(sc, ATH_DEBUG_ANY,"%s no more ath bufs. num phys frags %d \n",
                    __func__,n_sg);
            goto bad;
        }

        bf->bf_frmlen = txctl->frmlen;
        bf->bf_isdata = txctl->isdata;
        bf->bf_ismcast = txctl->ismcast;
        bf->bf_useminrate = txctl->use_minrate;
        bf->bf_isbar = txctl->isbar;
        bf->bf_ispspoll = txctl->ispspoll;
        bf->bf_calcairtime = txctl->calcairtime;
        bf->bf_flags = txctl->flags;
        bf->bf_shpreamble = txctl->shortPreamble;
        bf->bf_keytype = txctl->keytype;
        bf->bf_tidno = txctl->tidno;
        bf->bf_qnum = (!send_to_cabq) ? txctl->qnum : sc->sc_cabq->axq_qnum;

        rcs = (struct ath_rc_series *)&txctl->priv[0];
        bf->bf_rcs[0] = rcs[0];
        bf->bf_rcs[1] = rcs[1];
        bf->bf_rcs[2] = rcs[2];
        bf->bf_rcs[3] = rcs[3];
        bf->bf_node = an;
        bf->bf_mpdu = wbuf;
        bf->bf_reftxpower = txctl->txpower;

        /* setup descriptor */
        ds = bf->bf_desc;
        ath_hal_setdesclink(ah, ds, 0);
#ifndef REMOVE_PKT_LOG
        bf->bf_vdata = wbuf_header(wbuf);
#endif
        ASSERT(sc->sc_num_txmaps);

        if (likely(0 == (i/sc->sc_num_txmaps))) {

            /*
             * Save the DMA context in the first ath_buf
             */
            OS_COPY_DMA_MEM_CONTEXT(OS_GET_DMA_MEM_CONTEXT(bf, bf_dmacontext),
                                    OS_GET_DMA_MEM_CONTEXT(txctl, dmacontext));

            /*
             * Formulate first tx descriptor with tx controls.
             */
            ath_hal_set11n_txdesc(ah, ds
                                  , bf->bf_frmlen           /* frame length */
                                  , txctl->atype            /* Atheros packet type */
                                  , MIN(txctl->txpower, 60) /* txpower */
                                  , txctl->keyix            /* key cache index */
                                  , txctl->keytype          /* key type */
                                  , txctl->flags            /* flags */
                );

            firstds = ds;
            firstbf = bf;

            ath_hal_filltxdesc(ah, ds
                               , (bf->bf_buf_addr) 	/* buffer address */
                               , bf->bf_buf_len		/* buffer length */
                               , 0    				/* descriptor id */
                               , bf->bf_qnum  		/* QCU number */
                               , txctl->keytype     /* key type */
                               , AH_TRUE            /* first segment */
                               , (n_sg <= sc->sc_num_txmaps) ? AH_TRUE : AH_FALSE /* last segment */
                               , ds                 /* first descriptor */
                );
        } else {
            /* chain descriptor together */
            ath_hal_setdesclink(ah, lastds, bf->bf_daddr);

            ath_hal_filltxdesc(ah, ds
                               , bf->bf_buf_addr 	    /* buffer address */
                               , (u_int32_t *)bf->bf_buf_len		/* buffer length */
                               , 0    				                /* descriptor id */
                               , bf->bf_qnum  		                /* QCU number */
                               , txctl->keytype                     /* key type */
                               , AH_FALSE                           /* first segment */
                               , (i == n_sg-1) ? AH_TRUE : AH_FALSE /* last segment */
                               , firstds                            /* first descriptor */
                );
        }

        lastds = ds;
    }

    if (firstbf) {
        struct ath_atx_tid *tid = ATH_AN_2_TID(an, txctl->tidno);

        firstbf->bf_lastfrm = bf;
        firstbf->bf_ht = txctl->ht;
#ifdef ATH_SUPPORT_UAPSD
        if (txctl->isuapsd) {
            ath_tx_queue_uapsd(sc, txq, &bf_head, txctl);
            atomic_dec(&an->an_active_tx_cnt);
            return 0;
        }
#endif
        ath_vap_pause_txq_use_inc(sc);
        ATH_TXQ_LOCK(txq);

        if (likely(txctl->ht && sc->sc_txaggr))
        {
            if (likely(ath_aggr_query(tid))) {
                /*
                 * Try aggregation if it's a unicast data frame
                 * and the destination is HT capable.
                 */
               ath_tx_send_ampdu(sc, txq, tid, &bf_head, txctl);
            } else {
                /*
                 * Send this frame as regular when ADDBA exchange
                 * is neither complete nor pending.
                 */
                ath_tx_send_normal(sc, txq, tid, &bf_head, txctl);
            }
#if defined(ATH_ADDITIONAL_STATS) || ATH_SUPPORT_IQUE
            sc->sc_stats.ast_txq_packets[txq->axq_qnum]++;
#endif

        } else {
            firstbf->bf_lastbf = bf;
            firstbf->bf_nframes = 1;

            if (txctl->isbar) {
                /* This is required for resuming tid during BAR completion */
                firstbf->bf_tidno = wbuf_get_tid(wbuf);
            }

            if (!send_to_cabq) {
#if ATH_SUPPORT_HT
                ath_tx_send_normal(sc, txq, tid, &bf_head, txctl);
#else
                if (sc->sc_enhanceddmasupport && txctl->ismcast &&
                    txq->axq_depth >= ATH_SINGLES_MIN_QDEPTH)
                {
                    /* For sc_enhanceddmasupport dropping the packets
                     * if ATH_SUPPORT_HT is not defined.
                     */
                     ATH_TXQ_UNLOCK(txq);
                     ath_vap_pause_txq_use_dec(sc);
                     goto bad;
                }
                ath_buf_set_rate(sc, firstbf);
                if (ath_tx_txqaddbuf(sc, txq, &bf_head) != 0) {
                    // No HW resources. Dump the packet. 
		    ATH_TXQ_UNLOCK(txq);
                    ath_vap_pause_txq_use_dec(sc);
                    goto bad;
                }
#endif
            }
            else {
                /* reserving minimum buffer for unicast packets */
                if (sc->sc_txbuf_free < MCAST_MIN_FREEBUF) {
                    ATH_TXQ_UNLOCK(txq);
                    goto bad;
                }
                atomic_inc(&avp->av_beacon_cabq_use_cnt);
                if (atomic_read(&avp->av_stop_beacon) ||
                    avp->av_bcbuf == NULL) {
                    ATH_TXQ_UNLOCK(txq);
                    ath_vap_pause_txq_use_dec(sc);
                    atomic_dec(&avp->av_beacon_cabq_use_cnt);
                    goto bad;
                }

                ath_buf_set_rate(sc, firstbf);
                ATH_TXQ_LOCK(&avp->av_mcastq);
                ath_tx_mcastqaddbuf_internal_aponly(sc, &avp->av_mcastq, &bf_head);
                ATH_TXQ_UNLOCK(&avp->av_mcastq);
                atomic_dec(&avp->av_beacon_cabq_use_cnt);
            }
        }

        ATH_TXQ_UNLOCK(txq);
        ath_vap_pause_txq_use_dec(sc);
        atomic_dec(&an->an_active_tx_cnt);
        return 0;
    }
bad:
    /*
     * XXX: In other OS's, we can probably drop the frame. But in de-serialized
     * windows driver (NDIS6.0), we're not allowd to tail drop frame when out
     * of resources. So we just return NOMEM here and let OS shim to do whatever
     * OS wants.
     */
    ATH_TXBUF_LOCK(sc);
    if(!TAILQ_EMPTY(&bf_head)) {
        int num_buf = 0;
        ATH_NUM_BUF_IN_Q(&num_buf, &bf_head);
        (*buf_used)-= num_buf;
		sc->sc_txbuf_free += num_buf;
        TAILQ_CONCAT(&sc->sc_txbuf, &bf_head, bf_list);
    }

    ATH_TXBUF_UNLOCK(sc);

    sc->sc_stats.ast_tx_nobuf++;
    sc->sc_stats.ast_txq_nobuf[txctl->qnum]++;

    atomic_dec(&an->an_active_tx_cnt);
    return -ENOMEM;
}
EXPORT_SYMBOL(ath_tx_start_dma_aponly);

/*
 * Sets the min-rate for non-data packets or for data packets where
 * use min-rate is set (e.g. EAPOL packets)
 */
static inline void
ath_rate_set_minrate_aponly(struct ath_softc *sc, 
        ieee80211_tx_control_t *txctl, const HAL_RATE_TABLE *rt,
        struct ath_rc_series *rcs)
{ 
        if (txctl->min_rate != 0)
            rcs[0].rix = ath_rate_findrix_aponly(rt, txctl->min_rate);
        else
            rcs[0].rix = sc->sc_minrateix;
        rcs[0].tries = ATH_MGT_TXMAXTRY;
}

static inline int
__ath_tx_prepare_aponly(struct ath_softc *sc, wbuf_t wbuf, ieee80211_tx_control_t *txctl)
{
    struct ath_node *an;
    u_int8_t rix;
#ifdef ATH_SUPERG_COMP
    int comp = ATH_COMP_PROC_NO_COMP_NO_CCS;
#endif
    struct ath_txq *txq = NULL;
    struct ieee80211_frame *wh;
    const HAL_RATE_TABLE *rt;
#ifdef USE_LEGACY_HAL
    u_int8_t antenna;
#endif
    struct ath_rc_series *rcs;
    //int subtype;

    txctl->dev = sc;

    wh = (struct ieee80211_frame *)wbuf_header(wbuf);
    rt = sc->sc_currates;
    KASSERT(rt != NULL, ("no rate table, mode %u", sc->sc_curmode));

    an = txctl->an;
    txq = &sc->sc_txq[txctl->qnum];

    /*
     * Setup for rate calculations.
     */
    rcs = (struct ath_rc_series *)&txctl->priv[0];
    OS_MEMZERO(rcs, sizeof(struct ath_rc_series) * 4);

    if (likely(txctl->isdata)) {
        if (unlikely(txctl->ismcast)) {
            rcs[0].rix = (u_int8_t)ath_tx_findindex_aponly(rt, txctl->mcast_rate);

            /*
             * mcast packets are not re-tried.
             */
            rcs[0].tries = 1;
        }
        else {
            /*
             * For aggregation enabled nodes there is no need to do rate find
             * on each of these frames.
             */
            txctl->tidno = wbuf_get_tid(wbuf);     
            if (unlikely(
#ifdef ATH_RIFS
                !txctl->ht || (!sc->sc_txaggr && !sc->sc_txrifs) ||
#else
                !txctl->ht || !sc->sc_txaggr ||
#endif
                !ath_aggr_query( ATH_AN_2_TID(an,txctl->tidno)))) {

                if (likely(!txctl->use_minrate)) {
#if !(ATH_SUPPORT_HT)
                    int isProbe;
                    /*
                     * Data frames; consult the rate control module.
                     */
                    sc->sc_log_rcfind = 1;
                    ath_rate_findrate(sc, an, txctl->shortPreamble, txctl->frmlen,
                              ATH_11N_TXMAXTRY, ATH_RC_PROBE_ALLOWED,
                              TID_TO_WME_AC(txctl->tidno),
                              rcs, &isProbe, AH_FALSE,txctl->flags);

                    /* Ratecontrol sometimes returns invalid rate index */
                    if (rcs[0].rix != 0xff) {
                        an->an_prevdatarix = rcs[0].rix;
                        sc->sc_lastdatarix = rcs[0].rix;
                        sc->sc_lastrixflags = rcs[0].flags;
                    } else {
                        rcs[0].rix = an->an_prevdatarix;
                    }
#endif
                } else {
                    ath_rate_set_minrate_aponly(sc, txctl, rt, rcs);
                }

                if (
#ifdef ATH_RIFS
                txctl->ht && (sc->sc_txaggr || sc->sc_txrifs)
#else
                txctl->ht && sc->sc_txaggr
#endif
                ) {
                    if (likely(!(txctl->flags & HAL_TXDESC_FRAG_IS_ON))) {
                        struct ath_atx_tid *tid;

                        tid = ATH_AN_2_TID(an, txctl->tidno);
                        ATH_TXQ_LOCK(txq);
                        *(u_int16_t *)wh->i_seq = htole16(tid->seq_next << IEEE80211_SEQ_SEQ_SHIFT);
                        txctl->seqno = tid->seq_next;
                        INCR(tid->seq_next, IEEE80211_SEQ_MAX);
                        ATH_TXQ_UNLOCK(txq);
                    }
                }
            } else {
                 //case for aggregates
                /*
                 * For HT capable stations, we save tidno for later use.
                 * We also override seqno set by upper layer with the one
                 * in tx aggregation state.
                 * 
                 * First, the fragmentation stat is determined.  If fragmentation
                 * is on, the sequence number is not overridden, since it has been
                 * incremented by the fragmentation routine.
                 */
#if ATH_SUPPORT_IQUE
                /* If this frame is a HBR (headline block removal) probing QoSNull frame,
                 * it should be sent at the min rate which is cached in ath_node->an_minRate[ac]
                 */
                if (wbuf_is_probing(wbuf)) {
                    int isProbe;    
                    int ac = TID_TO_WME_AC(txctl->tidno);
                    ath_rate_findrate(sc, an, AH_FALSE, txctl->frmlen,
                         1, 0, ac, rcs, &isProbe, AH_FALSE,txctl->flags);
                    rcs[0].tries = 1;
                    rcs[1].tries = 0;
                    rcs[2].tries = 0;
                    rcs[3].tries = 0;
                } else 
#endif
                if (unlikely(txctl->use_minrate)) {
                    ath_rate_set_minrate_aponly(sc, txctl, rt, rcs);
                }
         
                if (likely(!(txctl->flags & HAL_TXDESC_FRAG_IS_ON))) {
                    struct ath_atx_tid *tid;

                    tid = ATH_AN_2_TID(an, txctl->tidno);
                    ATH_TXQ_LOCK(txq);
                    *(u_int16_t *)wh->i_seq = htole16(tid->seq_next << IEEE80211_SEQ_SEQ_SHIFT);
                    txctl->seqno = tid->seq_next;
                    INCR(tid->seq_next, IEEE80211_SEQ_MAX);
                    ATH_TXQ_UNLOCK(txq);
                }
            }
        }
    }
    else {
        txctl->tidno = WME_MGMT_TID;

        ath_rate_set_minrate_aponly(sc, txctl, rt, rcs);

#ifdef ATH_SUPPORT_TxBF
        { 
           /*
            * Force the Rate of Delay Report to be within 6 ~54 Mbps
            * Use the Rate less than one the sounding request used, so the report will be delivered reliably
            */
           u_int8_t *v_cv_data = (u_int8_t *)(wbuf_header(wbuf) + sizeof(struct ieee80211_frame));
               
           if ((wh->i_fc[0] == IEEE80211_FC0_SUBTYPE_ACTION) && (*v_cv_data == IEEE80211_ACTION_CAT_HT)) {
               if ((*(v_cv_data+1) == IEEE80211_ACTION_HT_COMP_BF) || (*(v_cv_data+1) == IEEE80211_ACTION_HT_NONCOMP_BF)) {
                   static u_int8_t map_rate[] = {12, 18, 24, 36, 48, 72, 96, 108};/*dot11 Rate*/
                   u_int8_t rate_index = ARRAY_LENGTH(map_rate);
                   /*
                    * transfer Kbps to dot11 Rate for compare, dot11 rate units is 500 Kbps 
                    * (c.f. IEEE802.11-2007 7.3.2.2) so divide Kbps by 500 to get dot11 rate units 
                    */
                   u_int16_t used_rate = sc->sounding_rx_kbps/500;
                                      
                   do {
                       rate_index--;
                       if (rate_index == 0) {
                           break;
                       }
                   } while (used_rate <= map_rate[rate_index]);
               
                   rcs[0].rix = ath_rate_findrix_aponly(rt, map_rate[rate_index]);
                   rcs[0].tries = 1;/*Retry not need*/
               }
           }
        }
#endif
        /* check and adjust the tsf for  probe response */
        if(txctl->atype == HAL_PKT_TYPE_PROBE_RESP) {
            struct ath_vap *avp = sc->sc_vaps[txctl->if_id];
            OS_MEMCPY(&wh[1], &avp->av_tsfadjust, sizeof(avp->av_tsfadjust));
        }
    }
    rix = rcs[0].rix;

    /*
     * Calculate duration.  This logically belongs in the 802.11
     * layer but it lacks sufficient information to calculate it.
     */
    if (likely((txctl->flags & HAL_TXDESC_NOACK) == 0 &&
        (wh->i_fc[0] & IEEE80211_FC0_TYPE_MASK) != IEEE80211_FC0_TYPE_CTL)) {
        u_int16_t dur;
        /*
         * XXX not right with fragmentation.
         */
        //11g and 11n - short preamble, more likely
        if (likely(txctl->shortPreamble))
            dur = rt->info[rix].spAckDuration;
        else
            dur = rt->info[rix].lpAckDuration;

        if (unlikely(wh->i_fc[1] & IEEE80211_FC1_MORE_FRAG)) {
            dur += dur;  /* Add additional 'SIFS + ACK' */

            /*
            ** Compute size of next fragment in order to compute
            ** durations needed to update NAV.
            ** The last fragment uses the ACK duration only.
            ** Add time for next fragment.
            */
            dur += ath_hal_computetxtime(sc->sc_ah, rt, txctl->nextfraglen, 
                                         rix, txctl->shortPreamble);
        }

        if (unlikely(txctl->istxfrag)) {
            /*
            **  Force hardware to use computed duration for next
            **  fragment by disabling multi-rate retry, which
            **  updates duration based on the multi-rate
            **  duration table.
            */
            rcs[1].tries = rcs[2].tries = rcs[3].tries = 0;
            rcs[1].rix = rcs[2].rix = rcs[3].rix = 0;
            rcs[0].tries = ATH_TXMAXTRY; /* reset tries but keep rate index */
        }

        *(u_int16_t *)wh->i_dur = cpu_to_le16(dur);
    }

    /*
     * Determine if a tx interrupt should be generated for
     * this descriptor.  We take a tx interrupt to reap
     * descriptors when the h/w hits an EOL condition or
     * when the descriptor is specifically marked to generate
     * an interrupt.  We periodically mark descriptors in this
     * way to insure timely replenishing of the supply needed
     * for sending frames.  Defering interrupts reduces system
     * load and potentially allows more concurrent work to be
     * done but if done to aggressively can cause senders to
     * backup.
     *
     * NB: use >= to deal with sc_txintrperiod changing
     *     dynamically through sysctl.
     */
    ATH_TXQ_LOCK(txq);
    if (
#ifdef ATH_SUPPORT_UAPSD
       (!txctl->isuapsd) &&
#endif
       (++txq->axq_intrcnt >= sc->sc_txintrperiod)) {
        txctl->flags |= HAL_TXDESC_INTREQ;
        txq->axq_intrcnt = 0;
    }
    ATH_TXQ_UNLOCK(txq);
    
    if (unlikely(txctl->ismcast))
        sc->sc_mcastantenna = (sc->sc_mcastantenna + 1) & 0x1;

#ifndef ATH_SWRETRY 
        txctl->flags |= HAL_TXDESC_CLRDMASK;
#endif

    /*
     * XXX: Update some stats ???
     */
    if (likely(txctl->shortPreamble))
        sc->sc_stats.ast_tx_shortpre++;
    if (unlikely(txctl->flags & HAL_TXDESC_NOACK))
        sc->sc_stats.ast_tx_noack++;
    
    return 0;
}


#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,22))
  #define  UNI_SKB_END_POINTER(skb)   (skb)->end
#else   
  #define  UNI_SKB_END_POINTER(skb)    skb_end_pointer(skb)
#endif
static inline int
__wbuf_map_sg_aponly(osdev_t osdev, adf_nbuf_t nbf, dma_addr_t *pa, void *arg)
{
    struct scatterlist sg;
    int ret;

    *pa = bus_map_single(osdev, nbf->data, UNI_SKB_END_POINTER(nbf) - nbf->data, BUS_DMA_TODEVICE);

    /* setup S/G list */
    memset(&sg, 0, sizeof(struct scatterlist));
    sg_dma_address(&sg) = *pa;
    sg_dma_len(&sg) = nbf->len;

    ret = ath_tx_start_dma_aponly(nbf, &sg, 1, arg);
    if (ret) {
        /*
         * NB: common code doesn't tail drop frame
         * because it's not allowed in NDIS 6.0.
         * For Linux, we have to do it here.
         */
        bus_unmap_single(osdev, *pa, UNI_SKB_END_POINTER(nbf) - nbf->data, BUS_DMA_TODEVICE);
    }

    return ret;
}


static inline int
ath_tx_start_aponly(ath_dev_t dev, wbuf_t wbuf, ieee80211_tx_control_t *txctl)
{
    struct ath_softc *sc = ATH_DEV_TO_SC(dev);
    int error = 0;

#ifdef PROFILE_LMAC_1
    int     i = 0;


   if(wbuf_is_encap_done(wbuf)) {
    if (!lmac_root) {
            lmac_root = adf_os_perf_init(0, "lmac1_tx_path_ap_only", EVENT_GROUP);

        perf_cntr[0] = adf_os_perf_init(umac_root, "cpu_cycles", EVENT_CPU_CYCLES);
        perf_cntr[1] = adf_os_perf_init(umac_root, "dcache_miss", EVENT_ICACHE_MISS);
        perf_cntr[2] = adf_os_perf_init(umac_root, "icache_miss", EVENT_DCACHE_MISS);
    }
    
     for (i = 0; i < 3; i++)
       adf_os_perf_start(perf_cntr[i]);
   }
#endif

    error = __ath_tx_prepare_aponly(sc, wbuf, txctl);

    //almost end of lmac
#ifdef PROFILE_LMAC_1
   if(wbuf_is_encap_done(wbuf)) {
     for (i = 0; i < 3; i++)
       adf_os_perf_start(perf_cntr[i]);
   }
#endif

    if (likely(error == 0)) {
        /*
         * Start DMA mapping.
         * ath_tx_start_dma() will be called either synchronously
         * or asynchrounsly once DMA is complete.
         */
        error = __wbuf_map_sg_aponly(sc->sc_osdev, wbuf,
                    OS_GET_DMA_MEM_CONTEXT(txctl, dmacontext),
                    txctl);

       if (error) {
#if ATH_RIFS
           if (txctl->ht && (sc->sc_txaggr || sc->sc_txrifs)) {
#else
           if (txctl->ht && sc->sc_txaggr) {
#endif
               struct ath_txq *txq = &sc->sc_txq[txctl->qnum];
               struct ath_atx_tid *tid;

               /* reclaim the seqno */
               ATH_TXQ_LOCK(txq);
               tid = ATH_AN_2_TID((struct ath_node *)txctl->an, txctl->tidno);
               DECR(tid->seq_next, IEEE80211_SEQ_MAX);
               ATH_TXQ_UNLOCK(txq);
        }
      }
    }
    /* failed packets will be dropped by the caller */
    return error;
}


struct ieee80211_txctl_cap {
	u_int8_t ismgmt;
	u_int8_t ispspoll;
	u_int8_t isbar;
	u_int8_t isdata;
	u_int8_t isqosdata;
	u_int8_t use_minrate;
	u_int8_t atype;
	u_int8_t ac;
	u_int8_t use_ni_minbasicrate;
	u_int8_t use_mgt_rate;
};
enum {
	IEEE80211_MGMT_DEFAULT	= 0,
	IEEE80211_MGMT_BEACON	= 1,
	IEEE80211_MGMT_PROB_RESP = 2,
	IEEE80211_MGMT_PROB_REQ = 3,
	IEEE80211_MGMT_ATIM	= 4,
	IEEE80211_CTL_DEFAULT	= 5,
	IEEE80211_CTL_PSPOLL	= 6,
	IEEE80211_CTL_BAR	= 7,
	IEEE80211_DATA_DEFAULT	= 8,
	IEEE80211_DATA_NODATA	= 9,
	IEEE80211_DATA_QOS	= 10,
	IEEE80211_TYPE4TXCTL_MAX= 11,
};

static struct ieee80211_txctl_cap txctl_cap[IEEE80211_TYPE4TXCTL_MAX] = {
		{ 1, 0, 0, 0, 0, 1, HAL_PKT_TYPE_NORMAL, WME_AC_VO, 1, 1}, 	/*default for mgmt*/
		{ 1, 0, 0, 0, 0, 1, HAL_PKT_TYPE_BEACON, WME_AC_VO, 1, 1}, 	/*beacon*/
		{ 1, 0, 0, 0, 0, 1, HAL_PKT_TYPE_PROBE_RESP, WME_AC_VO, 1, 1}, /*prob resp*/
		{ 1, 0, 0, 0, 0, 1, HAL_PKT_TYPE_NORMAL, WME_AC_VO, 0, 1}, 	/*prob req*/
		{ 1, 0, 0, 0, 0, 1, HAL_PKT_TYPE_ATIM, WME_AC_VO, 1, 1},  		/*atim*/
		{ 0, 0, 0, 0, 0, 1, HAL_PKT_TYPE_NORMAL, WME_AC_VO, 0, 0}, 	/*default for ctl*/
		{ 0, 1, 0, 0, 0, 1, HAL_PKT_TYPE_PSPOLL, WME_AC_VO, 0, 0}, 	/*pspoll*/
		{ 0, 0, 1, 0, 0, 1, HAL_PKT_TYPE_NORMAL, WME_AC_VO, 0, 0}, 	/*bar*/
		{ 0, 0, 0, 1, 0, 0, HAL_PKT_TYPE_NORMAL, WME_AC_BE, 0, 1}, 	/*default for data*/
		{ 1, 0, 0, 0, 0, 1, HAL_PKT_TYPE_NORMAL, WME_AC_VO, 1, 1},		/*nodata*/
		{ 0, 0, 0, 1, 1, 0, HAL_PKT_TYPE_NORMAL, WME_AC_BE, 0, 1}, 	/*qos data, the AC to be modified based on pkt's ac*/
};

static const HAL_KEY_TYPE keytype_table[IEEE80211_CIPHER_MAX] = {
        HAL_KEY_TYPE_WEP,   /*IEEE80211_CIPHER_WEP*/
            HAL_KEY_TYPE_TKIP,  /*IEEE80211_CIPHER_TKIP*/
        HAL_KEY_TYPE_AES,   /*IEEE80211_CIPHER_AES_OCB*/
            HAL_KEY_TYPE_AES,   /*IEEE80211_CIPHER_AES_CCM*/
#if ATH_SUPPORT_WAPI
        HAL_KEY_TYPE_WAPI,  /*IEEE80211_CIPHER_WAPI*/
#else
        HAL_KEY_TYPE_CLEAR,
#endif
            HAL_KEY_TYPE_WEP,   /*IEEE80211_CIPHER_CKIP*/
        HAL_KEY_TYPE_CLEAR, /*IEEE80211_CIPHER_NONE*/
};


static inline int
ath_tx_prepare_aponly(struct ath_softc_net80211 *scn, wbuf_t wbuf, int nextfraglen,
               ieee80211_tx_control_t *txctl)
{
    struct ieee80211_node *ni = wbuf_get_node(wbuf);
    struct ieee80211com *ic = &scn->sc_ic;
    struct ieee80211vap *vap = ni->ni_vap;
    struct ieee80211_frame *wh;
    int keyix, hdrlen, pktlen;
    int type, subtype;
	int txctl_tab_index;
	u_int32_t txctl_flag_mask = 0;
	u_int8_t acnum, use_ni_minbasicrate, use_mgt_rate;
	
    HAL_KEY_TYPE keytype = HAL_KEY_TYPE_CLEAR;

    OS_MEMZERO(txctl, sizeof(ieee80211_tx_control_t));

    wh = (struct ieee80211_frame *)wbuf_header(wbuf);

    txctl->ismcast = IEEE80211_IS_MULTICAST(wh->i_addr1);
    txctl->istxfrag = (wh->i_fc[1] & IEEE80211_FC1_MORE_FRAG) ||
        (((le16toh(*((u_int16_t *)&(wh->i_seq[0]))) >>
           IEEE80211_SEQ_FRAG_SHIFT) & IEEE80211_SEQ_FRAG_MASK) > 0);
    type = wh->i_fc[0] & IEEE80211_FC0_TYPE_MASK;
    subtype = wh->i_fc[0] & IEEE80211_FC0_SUBTYPE_MASK;

    /*
     * Packet length must not include any
     * pad bytes; deduct them here.
     */
    hdrlen = ieee80211_anyhdrsize(wh);
    pktlen = wbuf_get_pktlen(wbuf);
    pktlen -= (hdrlen & 3);

    if (IEEE80211_VAP_IS_SAFEMODE_ENABLED(vap)) {
        /* For Safe Mode, the encryption and its encap is already done
           by the upper layer software. Driver do not modify the packet. */
        keyix = HAL_TXKEYIX_INVALID;
    }
    else if (wh->i_fc[1] & IEEE80211_FC1_WEP) {
        const struct ieee80211_cipher *cip;
        struct ieee80211_key *k;

        /*
         * Construct the 802.11 header+trailer for an encrypted
         * frame. The only reason this can fail is because of an
         * unknown or unsupported cipher/key type.
         */

        /* FFXXX: change to handle linked wbufs */
        k = ieee80211_crypto_encap(ni, wbuf);
        if (k == NULL) {
            /*
             * This can happen when the key is yanked after the
             * frame was queued.  Just discard the frame; the
             * 802.11 layer counts failures and provides
             * debugging/diagnostics.
             */
            return -EIO;
        }
        /* update the value of wh since encap can reposition the header */
        wh = (struct ieee80211_frame *)wbuf_header(wbuf);

        /*
         * Adjust the packet + header lengths for the crypto
         * additions and calculate the h/w key index. When
         * a s/w mic is done the frame will have had any mic
         * added to it prior to entry so wbuf pktlen above will
         * account for it. Otherwise we need to add it to the
         * packet length.
         */
        cip = k->wk_cipher;
        hdrlen += cip->ic_header;
#ifndef __CARRIER_PLATFORM__
        pktlen += cip->ic_header + cip->ic_trailer;
#else
#error "carrier platform defined!!"
        if (wbuf_is_encap_done(wbuf))
            pktlen += cip->ic_trailer;
        else
            pktlen += cip->ic_header + cip->ic_trailer;
#endif

        if (likely((k->wk_flags & IEEE80211_KEY_SWMIC) == 0)) {
            if ( ! txctl->istxfrag)
                pktlen += cip->ic_miclen;
            else {
                if (cip->ic_cipher != IEEE80211_CIPHER_TKIP)
                    pktlen += cip->ic_miclen;
            }
        }
        else{
            pktlen += cip->ic_miclen;
        }
		if (cip->ic_cipher < IEEE80211_CIPHER_MAX) {
			keytype = keytype_table[cip->ic_cipher];
		}
        if (unlikely(((k->wk_flags & IEEE80211_KEY_MFP) && IEEE80211_IS_MFP_FRAME(wh)))) {	
			if (cip->ic_cipher == IEEE80211_CIPHER_TKIP) {
            	DPRINTF(scn, ATH_DEBUG_KEYCACHE, "%s: extend MHDR IE\n", __func__);
	            /* mfp packet len could be extended by MHDR IE */
    	        pktlen += sizeof(struct ieee80211_ccx_mhdr_ie);
			}

            keyix = k->wk_clearkeyix;
            keytype = HAL_KEY_TYPE_CLEAR;
        }
        else 
            keyix = k->wk_keyix;


    }  else if (ni->ni_ucastkey.wk_cipher == &ieee80211_cipher_none) {
        /*
         * Use station key cache slot, if assigned.
         */
        keyix = ni->ni_ucastkey.wk_keyix;
        if (keyix == IEEE80211_KEYIX_NONE)
            keyix = HAL_TXKEYIX_INVALID;
    } else
        keyix = HAL_TXKEYIX_INVALID;

    pktlen += IEEE80211_CRC_LEN;

    txctl->frmlen = pktlen;
    txctl->keyix = keyix;
    txctl->keytype = keytype;
    txctl->txpower = ieee80211_node_get_txpower(ni);
    txctl->nextfraglen = nextfraglen;
#ifdef USE_LEGACY_HAL
    txctl->hdrlen = hdrlen;
#endif
#if ATH_SUPPORT_IQUE
    txctl->tidno = wbuf_get_tid(wbuf);
#endif
    /*
     * NB: the 802.11 layer marks whether or not we should
     * use short preamble based on the current mode and
     * negotiated parameters.
     */
    if (IEEE80211_IS_SHPREAMBLE_ENABLED(ic) &&
        !IEEE80211_IS_BARKER_ENABLED(ic) &&
        ieee80211node_has_cap(ni, IEEE80211_CAPINFO_SHORT_PREAMBLE)) {
        txctl->shortPreamble = 1;
    }

#ifndef ATH_SWRETRY 
    txctl->flags = HAL_TXDESC_CLRDMASK;    /* XXX needed for crypto errs */
#endif

    /*
     * Calculate Atheros packet type from IEEE80211
     * packet header and select h/w transmit queue.
     */
	if (type == IEEE80211_FC0_TYPE_MGT) {
		if (subtype == IEEE80211_FC0_SUBTYPE_BEACON) {
			txctl_tab_index = IEEE80211_MGMT_BEACON;
		} else if (subtype == IEEE80211_FC0_SUBTYPE_PROBE_RESP) {
			txctl_tab_index = IEEE80211_MGMT_PROB_RESP;
		} else if (subtype == IEEE80211_FC0_SUBTYPE_PROBE_REQ) {
			txctl_tab_index = IEEE80211_MGMT_PROB_REQ;
		} else if (subtype == IEEE80211_FC0_SUBTYPE_ATIM) {
			txctl_tab_index = IEEE80211_MGMT_ATIM;
		} else {
			txctl_tab_index = IEEE80211_MGMT_DEFAULT;
		}
	} else if (type == IEEE80211_FC0_TYPE_CTL) {
		if (subtype == IEEE80211_FC0_SUBTYPE_PS_POLL) {
			txctl_tab_index = IEEE80211_CTL_PSPOLL;
		} else if (subtype == IEEE80211_FC0_SUBTYPE_BAR) {
			txctl_tab_index = IEEE80211_CTL_BAR;
		} else {
			txctl_tab_index = IEEE80211_CTL_DEFAULT;
		}
	} else if (type == IEEE80211_FC0_TYPE_DATA) {
		if (subtype == IEEE80211_FC0_SUBTYPE_NODATA) {
			txctl_tab_index = IEEE80211_DATA_NODATA;
		} else if (subtype & IEEE80211_FC0_SUBTYPE_QOS) {
			txctl_tab_index = IEEE80211_DATA_QOS;
		} else {
			txctl_tab_index = IEEE80211_DATA_DEFAULT;
		}
	} else {
        printk("bogus frame type 0x%x (%s)\n",
               wh->i_fc[0] & IEEE80211_FC0_TYPE_MASK, __func__);
        /* XXX statistic */
        return -EIO;
	}
	txctl->ismgmt = txctl_cap[txctl_tab_index].ismgmt;
	txctl->ispspoll = txctl_cap[txctl_tab_index].ispspoll;
	txctl->isbar = txctl_cap[txctl_tab_index].isbar;
	txctl->isdata = txctl_cap[txctl_tab_index].isdata;
	txctl->isqosdata = txctl_cap[txctl_tab_index].isqosdata;
	txctl->use_minrate = txctl_cap[txctl_tab_index].use_minrate;
	txctl->atype = txctl_cap[txctl_tab_index].atype;
	acnum = txctl_cap[txctl_tab_index].ac;
	use_ni_minbasicrate = txctl_cap[txctl_tab_index].use_ni_minbasicrate;
	use_mgt_rate = txctl_cap[txctl_tab_index].use_mgt_rate;

	/*
	 * Update some txctl fields
	 */
	if (likely(type == IEEE80211_FC0_TYPE_DATA && subtype != IEEE80211_FC0_SUBTYPE_NODATA)) {
        if (unlikely(wbuf_is_eapol(wbuf))) {
            txctl->use_minrate = 1;
		}
        if (unlikely(txctl->ismcast)) {
            txctl->mcast_rate = vap->iv_mcast_rate;
		}
        if (likely(subtype & IEEE80211_FC0_SUBTYPE_QOS)) {
            /* XXX validate frame priority, remove mask */
            acnum = wbuf_get_priority(wbuf) & 0x03;
            
            if (ieee80211com_wmm_chanparams(ic, acnum)->wmep_noackPolicy)
                txctl_flag_mask |= HAL_TXDESC_NOACK;

#ifdef ATH_SUPPORT_TxBF
            /* Qos frame with Order bit set indicates an HTC frame */
            if (wh->i_fc[1] & IEEE80211_FC1_ORDER) {
                int is4addr;
                u_int8_t *htc;
                u_int8_t  *tmpdata;

                is4addr = ((wh->i_fc[1] & IEEE80211_FC1_DIR_MASK) ==
                            IEEE80211_FC1_DIR_DSTODS) ? 1 : 0;
                if (!is4addr) {
                    htc = ((struct ieee80211_qosframe_htc *)wh)->i_htc;
        		} else {
                    htc= ((struct ieee80211_qosframe_htc_addr4 *)wh)->i_htc;
                }
      
                tmpdata=(u_int8_t *) wh;
                /* This is a sounding frame */
                if ((htc[2] == IEEE80211_HTC2_CSI_COMP_BF) ||   
                    (htc[2] == IEEE80211_HTC2_CSI_NONCOMP_BF) ||
                    ((htc[2] & IEEE80211_HTC2_CalPos)==3))
                {
                    //printk("==>%s,txctl flag before attach sounding%x,\n",__func__,txctl->flags);
                    if (ic->ic_txbf.tx_staggered_sounding &&
                        ni->ni_txbf.rx_staggered_sounding)
                    {
                        //txctl->flags |= HAL_TXDESC_STAG_SOUND;
                        txctl_flag_mask|=(HAL_TXDESC_STAG_SOUND<<HAL_TXDESC_TXBF_SOUND_S);
                    } else {
                        txctl_flag_mask |= (HAL_TXDESC_SOUND<<HAL_TXDESC_TXBF_SOUND_S);
                    }
                    txctl_flag_mask |= (ni->ni_txbf.channel_estimation_cap<<HAL_TXDESC_CEC_S);
                    //printk("==>%s,txctl flag %x,tx staggered sounding %x, rx staggered sounding %x\n"
                      //  ,__func__,txctl->flags,ic->ic_txbf.tx_staggered_sounding,ni->ni_txbf.rx_staggered_sounding);
                }

                if ((htc[2] & IEEE80211_HTC2_CalPos)!=0)    // this is a calibration frame
                {
                     txctl_flag_mask|=HAL_TXDESC_CAL;
                }    
            }
#endif

        } else {
            /*
             * Default all non-QoS traffic to the best-effort queue.
             */
            wbuf_set_priority(wbuf, WME_AC_BE);
        }

        ath_uapsd_txctl_update(scn, wbuf, txctl);

        txctl_flag_mask |=
        	   (ieee80211com_has_htcap(ic, IEEE80211_HTCAP_C_ADVCODING) &&
               (ni->ni_htcap & IEEE80211_HTCAP_C_ADVCODING)) ?
               HAL_TXDESC_LDPC : 0;
                            
        /*
         * For HT capable stations, we save tidno for later use.
         * We also override seqno set by upper layer with the one
         * in tx aggregation state.
         */
        if (!txctl->ismcast && ieee80211node_has_flag(ni, IEEE80211_NODE_HT))
            txctl->ht = 1;
	}
	/*
	 * Set min rate and qnum in txctl based on acnum
	 */
	if (txctl->use_minrate) {
        /*
         * if management rate is set, then use it.
         */
        if (use_mgt_rate) {
			if (vap->iv_mgt_rate) {
    	        txctl->min_rate = vap->iv_mgt_rate;
        	}
		}
	}
    txctl->qnum = scn->sc_ac2q[acnum];
    /* Update the uapsd ctl for all frames */
    ath_uapsd_txctl_update(scn, wbuf, txctl);


    /*
     * If we are servicing one or more stations in power-save mode.
     */
    txctl->if_id = (ATH_VAP_NET80211(vap))->av_if_id;
    if (ieee80211vap_has_pssta(vap))
        txctl->ps = 1;
    
    /*
     * Calculate miscellaneous flags.
     */
    if (txctl->ismcast) {
        txctl_flag_mask |= HAL_TXDESC_NOACK;	/* no ack on broad/multicast */
    } else if (pktlen > ieee80211vap_get_rtsthreshold(vap)) { 
            txctl_flag_mask |= HAL_TXDESC_RTSENA;	/* RTS based on frame length */
    }

    /* Frame to enable SM power save */
    if (wbuf_is_smpsframe(wbuf)) {
        txctl_flag_mask |= HAL_TXDESC_LOWRXCHAIN;
    }

	/*
	 * Update txctl->flags based on the flag mask
	 */
	txctl->flags |= txctl_flag_mask;
    IEEE80211_HTC_SET_NODE_INDEX(txctl, wbuf);

    return 0;
}


static inline wbuf_t 
ieee80211_encap_80211_aponly(struct ieee80211_node *ni, wbuf_t wbuf)
{
    struct ieee80211com *ic = ni->ni_ic;
    struct ieee80211vap *vap = ni->ni_vap;
	struct ieee80211_key *key=NULL;
    int key_mapping_key = 0;
    struct ieee80211_frame *wh;
    int type, subtype;
    int useqos=0, use4addr=0, usecrypto=0;
    int hdrsize, datalen, pad, addlen; /* additional header length we want to append */
    int ac = wbuf_get_priority(wbuf);
    
    use4addr = 0;
    
    wh = (struct ieee80211_frame *)wbuf_header(wbuf);
    type = wh->i_fc[0] & IEEE80211_FC0_TYPE_MASK;
    subtype = wh->i_fc[0] & IEEE80211_FC0_SUBTYPE_MASK;
    hdrsize = sizeof(struct ieee80211_frame);
    datalen = wbuf_get_pktlen(wbuf) - (hdrsize + sizeof(struct llc));  /* NB: w/o 802.11 header */

        if(IEEE80211_VAP_IS_PRIVACY_ENABLED(vap) &&    /* crypto is on */
        (type == IEEE80211_FC0_TYPE_DATA)) {        /* only for data frame */
        /*
         * Find the key that would be used to encrypt the frame if the 
         * frame were to be encrypted. For unicast frame, search the 
         * matching key in the key mapping table first. If not found,
         * used default key. For multicast frame, only use the default key.
         */
        if(!IEEE80211_IS_MULTICAST(wh->i_addr1)){
            /* use unicast key */
            key = &ni->ni_ucastkey;
        }
        if (key && key->wk_valid) {
            key_mapping_key = 1;
        } else {
            if (vap->iv_def_txkey != IEEE80211_KEYIX_NONE) {
                key = &vap->iv_nw_keys[vap->iv_def_txkey];
                if (!key->wk_valid) {
                    key = NULL;
                }
            } else {
                key = NULL;
            }
        }
        /*
         * Assert our Exemption policy.  We assert it blindly at first, then
         * take the presence/absence of a key into acct.
         *
         * Lookup the ExemptionActionType in the send context info of this frame
         * to determine if we need to encrypt the frame.
         */
        switch (wbuf_get_exemption_type(wbuf)) {
        case WBUF_EXEMPT_NO_EXEMPTION:
            /*
             * We want to encrypt this frame.
             */
            usecrypto = 1;
            break;

        case WBUF_EXEMPT_ALWAYS:
            /*
             * We don't want to encrypt this frame.
             */
            break;

        case WBUF_EXEMPT_ON_KEY_MAPPING_KEY_UNAVAILABLE:
            /*
             * We encrypt this frame if and only if a key mapping key is set.
             */
            if (key_mapping_key) {
                usecrypto = 1;
            }
            break;

        default:
            ASSERT(0);
            usecrypto = 1;
            break;
        }

        /*
         * If the frame is to be encrypted, but no key is not set, either reject the frame 
         * or clear the WEP bit.
         */
        if (usecrypto && !key) {
            /*
             * If this is a unicast frame or if the BSSPrivacy is on, reject the frame.
             * Otherwise, clear the WEP bit so we will not encrypt the frame. In other words,
             * we'll send multicast frame in clear if multicast key hasn't been setup.
             */
            if (!IEEE80211_IS_MULTICAST(wh->i_addr1)) {
                goto bad;
            }
            else
                usecrypto = 0; /* XXX: is this right??? */
        }

        if (usecrypto)
            wh->i_fc[1] |= IEEE80211_FC1_WEP;
        else
            wh->i_fc[1] &= ~IEEE80211_FC1_WEP;
        }
    /*
     * XXX: If it's an EAPOL frame:
     * Some 11n APs drop non-QoS frames after ADDBA sequence. For example,
     * bug 31812: Connection failure with Buffalo AMPG144NH. To fix it,
     * seq. number in the same tid space, as requested in ADDBA, need to be
     * used for the EAPOL frames. Therefore, wb_eapol cannot be set.
     * 
     * if (((struct llc *)&wh[1])->llc_snap.ether_type == htobe16(ETHERTYPE_PAE))
     *    wbuf_set_eapol(wbuf);
     */

    /*
     * Figure out additional header length we want to append after the wireless header.
     * - Add Qos Control field if necessary
     *   XXX: EAPOL frames will be encapsulated as QoS frames as well.
     * - Additional QoS control field for OWL WDS workaround
     * - IV will be added in ieee80211_crypto_encap().
     */
    addlen = 0;
    pad = 0;
    if (wh->i_fc[0] & IEEE80211_FC0_SUBTYPE_QOS) {
        useqos = 1;
        hdrsize += sizeof(struct ieee80211_qoscntl);
        
          /* For TxBF CV cache update add +HTC field */
#ifdef ATH_SUPPORT_TxBF
        if (ni->ni_bf_update_cv) {
            hdrsize += sizeof(struct ieee80211_htc);
        }
#endif
        
        /*
         * XXX: we assume a QoS frame must come from ieee80211_encap_8023() function,
         * meaning it's already padded. If OS sends a QoS frame (thus without padding),
         * then it'll break.
         */
        pad = roundup(hdrsize, sizeof(u_int32_t)) - hdrsize;
        /*if (ic->ic_flags & IEEE80211_F_DATAPAD) {
            pad = roundup(hdrsize, sizeof(u_int32_t)) - hdrsize;
        }*/
    } else if (likely(type == IEEE80211_FC0_TYPE_DATA &&
               ((ni->ni_flags & IEEE80211_NODE_QOS) ||
                IEEE80211_NODE_USEAMPDU(ni)))) {
        useqos = 1;
        addlen += sizeof(struct ieee80211_qoscntl);
        /* For TxBF CV cache update add +HTC field */
#ifdef ATH_SUPPORT_TxBF
        if (ni->ni_bf_update_cv) {
            addlen += sizeof(struct ieee80211_htc);
        }
#endif
    } 

    if (likely(addlen)) {       

       /*
        * XXX: if we already have enough padding, then
        * don't need to push in more bytes, otherwise,
        * put in bytes after the original padding.
        */
       if (addlen > pad)
           addlen = roundup((hdrsize + addlen), sizeof(u_int32_t)) - hdrsize - pad;
       else
           addlen = 0;

        if (likely(addlen)) {
            struct ieee80211_frame *wh0;

            wh0 = wh;
            wh = (struct ieee80211_frame *)wbuf_push(wbuf, addlen);
            if (unlikely(wh == NULL))
            {
                goto bad;
            }
            memmove(wh, wh0, hdrsize);
        }
    }
    
    if (likely(useqos)) {
        u_int8_t *qos;
        int tid;

        ac = wbuf_get_priority(wbuf);
        tid = wbuf_get_tid(wbuf);

        qos = ((struct ieee80211_qosframe *)wh)->i_qos;

        qos[0] = tid & IEEE80211_QOS_TID;
        if (ic->ic_wme.wme_wmeChanParams.cap_wmeParams[ac].wmep_noackPolicy)
            qos[0] |= (1 << IEEE80211_QOS_ACKPOLICY_S);
#ifdef ATH_AMSDU
        if (wbuf_is_amsdu(wbuf)) {
            qos[0] |= (1 << IEEE80211_QOS_AMSDU_S) & IEEE80211_QOS_AMSDU;
        }
#endif
        qos[1] = 0;
        wh->i_fc[0] |= IEEE80211_FC0_SUBTYPE_QOS;
        
        /* Fill in the sequence number from the TID sequence space. */
        *(u_int16_t *)&wh->i_seq[0] =
            htole16(ni->ni_txseqs[tid] << IEEE80211_SEQ_SEQ_SHIFT);
        ni->ni_txseqs[tid]++;

#ifdef ATH_SUPPORT_TxBF
        IEEE80211_DPRINTF(vap, IEEE80211_MSG_DEBUG,"==>%s:CV update\n",__func__);
        if (ni->ni_bf_update_cv) {

            ieee80211_request_cv_update(ic,ni, wbuf, use4addr);
            /* clear flag */
           // ni->ni_bf_update_cv = 0;
        }       
#endif
       
    } else {
        *(u_int16_t *)wh->i_seq =
            htole16(ni->ni_txseqs[IEEE80211_NON_QOS_SEQ] << IEEE80211_SEQ_SEQ_SHIFT);
        ni->ni_txseqs[IEEE80211_NON_QOS_SEQ]++;
    }

        
    if (!ieee80211_check_and_fragment(vap, wbuf, wh, usecrypto,key,hdrsize)) {
            goto bad;
    }

    IEEE80211_NODE_STAT(ni, tx_data);
    if (IEEE80211_IS_MULTICAST(wh->i_addr1))
        IEEE80211_NODE_STAT(ni, tx_mcast);
    else
        IEEE80211_NODE_STAT(ni, tx_ucast);
    IEEE80211_NODE_STAT_ADD(ni, tx_bytes, datalen);

    return wbuf;

bad:
    while (wbuf != NULL) {
        wbuf_t wbuf1 = wbuf_next(wbuf);
        IEEE80211_TX_COMPLETE_WITH_ERROR(wbuf);
        wbuf = wbuf1;
    }
    return NULL;
}


static inline wbuf_t
ieee80211_encap_8023_aponly(struct ieee80211_node *ni, wbuf_t wbuf)
{
    struct ieee80211vap *vap = ni->ni_vap;
    struct ieee80211_rsnparms *rsn = &vap->iv_rsn;
    struct ether_header eh;
    struct ieee80211_frame *wh;
    struct llc *llc;
    int hdrsize, hdrspace, addqos, use4addr, isMulticast;
    int is_amsdu = wbuf_is_amsdu(wbuf);
#ifdef ATH_SUPPORT_TxBF
    int addhtc;
#endif

    /*
     * Copy existing Ethernet header to a safe place.  The
     * rest of the code assumes it's ok to strip it when
     * reorganizing state for the final encapsulation.
     */
    KASSERT(wbuf_get_pktlen(wbuf)>= sizeof(eh), ("no ethernet header!"));
    OS_MEMCPY(&eh, wbuf_header(wbuf), sizeof(struct ether_header));
    addqos = (IEEE80211_NODE_USEAMPDU(ni) || ni->ni_flags & IEEE80211_NODE_QOS);

#ifdef ATH_SUPPORT_TxBF   
    addhtc = (ni->ni_bf_update_cv == 1);

    if (addhtc) {
        hdrsize = sizeof(struct ieee80211_qosframe_htc);
    } else  if (likely(addqos))
#else
    if (likely(addqos))
#endif  
        hdrsize = sizeof(struct ieee80211_qosframe);
    else
        hdrsize = sizeof(struct ieee80211_frame);
    
    isMulticast = (IEEE80211_IS_MULTICAST(eh.ether_dhost)) ? 1 : 0 ;
    use4addr = 0;
    hdrspace = roundup(hdrsize, sizeof(u_int32_t));

    if (likely(!is_amsdu && htons(eh.ether_type) >= IEEE8023_MAX_LEN)) {
           
        /* 
         * push the data by
         * required total bytes for 802.11 header (802.11 header + llc - ether header).
         */
        if (wbuf_push(wbuf, (u_int16_t) (hdrspace 
                  + sizeof(struct llc) - sizeof(struct ether_header))) == NULL) {
            IEEE80211_DPRINTF(vap, IEEE80211_MSG_OUTPUT,
                          "%s:  %s::wbuf_push failed \n",
                          __func__, ether_sprintf(eh.ether_dhost));
            goto bad;
        }

        wh = (struct ieee80211_frame *)wbuf_header(wbuf);
        llc = (struct llc *)((u_int8_t *)wh + hdrspace);
        llc->llc_dsap = llc->llc_ssap = LLC_SNAP_LSAP;
        llc->llc_control = LLC_UI;
        llc->llc_snap.org_code[0] = RFC1042_SNAP_ORGCODE_0; /* 0x0 */
        llc->llc_snap.org_code[1] = RFC1042_SNAP_ORGCODE_1; /* 0x0 */
        llc->llc_snap.org_code[2] = RFC1042_SNAP_ORGCODE_2; /* 0x0 */
        llc->llc_snap.ether_type = eh.ether_type;
    }
    else {
        /* 
         * push the data by
         * required total bytes for 802.11 header (802.11 header - ether header).
         */
        if (wbuf_push(wbuf, (u_int16_t)(hdrspace-sizeof(struct ether_header))) == NULL) {
            IEEE80211_DPRINTF(vap, IEEE80211_MSG_OUTPUT,
                          "%s:  %s::wbuf_push failed \n",
                          __func__, ether_sprintf(eh.ether_dhost));
            goto bad;
        }
        wh = (struct ieee80211_frame *)wbuf_header(wbuf);
    }

    
    wh->i_fc[0] = IEEE80211_FC0_VERSION_0 | IEEE80211_FC0_TYPE_DATA;
    *(u_int16_t *)wh->i_dur = 0;
    /** WDS FIXME */  
            wh->i_fc[1] = IEEE80211_FC1_DIR_FROMDS;
            IEEE80211_ADDR_COPY(wh->i_addr1, eh.ether_dhost);
            IEEE80211_ADDR_COPY(wh->i_addr2, ni->ni_bssid);
            IEEE80211_ADDR_COPY(wh->i_addr3, eh.ether_shost);
            if (wbuf_is_moredata(wbuf)) {
                    wh->i_fc[1] |= IEEE80211_FC1_MORE_DATA;
            }

    if (likely(addqos)) {
        /*
         * Just mark the frame as QoS, and QoS control filed will be filled
         * in ieee80211_encap_80211().
         */
        wh->i_fc[0] |= IEEE80211_FC0_SUBTYPE_QOS;
    }

    /*
     * Set per-packet exemption type
     */
    if (unlikely(eh.ether_type == htons(ETHERTYPE_PAE))) {
        /* 
         * IEEE 802.1X: send EAPOL frames always in the clear.
         * WPA/WPA2: encrypt EAPOL keys when pairwise keys are set.
         */
        if (RSN_AUTH_IS_WPA(rsn) || RSN_AUTH_IS_WPA2(rsn)) {
            wbuf_set_exemption_type(wbuf, WBUF_EXEMPT_ON_KEY_MAPPING_KEY_UNAVAILABLE);
        }
        else {
            wbuf_set_exemption_type(wbuf, WBUF_EXEMPT_ALWAYS);
        }
    }
#if ATH_SUPPORT_WAPI
    else if (eh.ether_type == htons(ETHERTYPE_WAI)) {
            wbuf_set_exemption_type(wbuf, WBUF_EXEMPT_ALWAYS);
    }
#endif

    else {
        wbuf_set_exemption_type(wbuf, WBUF_EXEMPT_NO_EXEMPTION);
    }
    
    return ieee80211_encap_80211_aponly(ni, wbuf);
    
bad:
    /* complete the failed wbuf here */
    IEEE80211_TX_COMPLETE_WITH_ERROR(wbuf);
    return NULL;
}






static inline u_int32_t
ath_txq_depth_aponly(ath_dev_t dev, int qnum)
{
    struct ath_softc *sc = ATH_DEV_TO_SC(dev);

    return sc->sc_txq[qnum].axq_depth;
#ifdef ATH_SWRETRY
    /* XXX TODO the num of frames present in SW Retry queue
     * are not reported. No problems are forseen at this
     * moment due to this. Need to revisit this if problem
     * occurs
     */
#endif
}


static inline u_int32_t
ath_txq_aggr_depth_aponly(ath_dev_t dev, int qnum)
{
    struct ath_softc *sc = ATH_DEV_TO_SC(dev);

    return sc->sc_txq[qnum].axq_aggr_depth;
}



static inline void
ath_net80211_addba_status_aponly(struct ieee80211_node *ni, u_int8_t tidno, u_int16_t *status)
{
    struct ieee80211com *ic = ni->ni_ic;
    struct ath_softc_net80211 *scn = ATH_SOFTC_NET80211(ic);

    *status = scn->sc_ops->addba_status(scn->sc_dev, ATH_NODE_NET80211(ni)->an_sta, tidno);
}


/*
 * The function to send a frame (i.e., hardstart). The wbuf should already be
 * associated with the actual frame, and have a valid node instance.
 */

static inline int
ath_tx_send_aponly(wbuf_t wbuf)
{
    struct ieee80211_node *ni = wbuf_get_node(wbuf);
    struct ieee80211com *ic = ni->ni_ic;
    struct ath_softc_net80211 *scn = ATH_SOFTC_NET80211(ic);
    wbuf_t next_wbuf;

    //begin of umac-2
    //
#ifdef PROFILE_UMAC_2
    int     i = 0;


   if(wbuf_is_encap_done(wbuf)) {
    if (!umac_root) {
            umac_root = adf_os_perf_init(0, "umac2_tx_path_ap_only", EVENT_GROUP);

        perf_cntr[0] = adf_os_perf_init(umac_root, "cpu_cycles", EVENT_CPU_CYCLES);
        perf_cntr[1] = adf_os_perf_init(umac_root, "dcache_miss", EVENT_ICACHE_MISS);
        perf_cntr[2] = adf_os_perf_init(umac_root, "icache_miss", EVENT_DCACHE_MISS);
    }
    
     for (i = 0; i < 3; i++)
       adf_os_perf_start(perf_cntr[i]);
   }
#endif


#if defined(ATH_SUPPORT_DFS)
    /*
     * If we detect radar on the current channel, stop sending data
     * packets. There is a DFS requirment that the AP should stop 
     * sending data packet within 200 ms of radar detection
     */

    if (ic->ic_curchan->ic_flags & IEEE80211_CHAN_RADAR) {
        DPRINTF(scn, ATH_DEBUG_STATE, "%s: RADAR FOUND ON CHANNEL\n", __func__);
        goto bad;
    }
#endif
    
    /*
     * XXX TODO: Fast frame here
     */

    ath_uapsd_pwrsave_check_aponly(wbuf, ni);

#ifdef ATH_AMSDU

    if (IEEE80211_NODE_USEAMPDU(ni) &&
        (ath_get_amsdusupported_aponly(scn->sc_dev,
                                        ATH_NODE_NET80211(ni)->an_sta,
                                        wbuf_get_tid(wbuf))))

    {
        wbuf = ath_amsdu_send_aponly(wbuf);
        if (wbuf == NULL)
            return 0;
    }
#endif

    /*
     * Encapsulate the packet for transmission
     */ 
    wbuf = ieee80211_encap_8023_aponly(ni, wbuf);
    if (unlikely(wbuf == NULL)) {
        goto bad;
    }

    /*
     * If node is HT capable, then send out ADDBA if
     * we haven't done so.
     *
     * XXX: send ADDBA here to avoid re-entrance of other
     * tx functions.
     */
    if (likely(IEEE80211_NODE_USEAMPDU(ni) &&
        ic->ic_addba_mode == ADDBA_MODE_AUTO)) {
        u_int8_t tidno = wbuf_get_tid(wbuf);
        struct ieee80211_action_mgt_args actionargs;

        if (
#ifdef ATH_SUPPORT_UAPSD
           (!IEEE80211_NODE_AC_UAPSD_ENABLED(ni, TID_TO_WME_AC(tidno))) &&
#endif
           (unlikely(ath_aggr_check_aponly(scn->sc_dev, ATH_NODE_NET80211(ni)->an_sta, tidno)) &&
           /* don't allow EAPOL frame to cause addba to avoid auth timeouts */
           !wbuf_is_eapol(wbuf)))
        {
            /* Send ADDBA request */
            actionargs.category = IEEE80211_ACTION_CAT_BA;
            actionargs.action   = IEEE80211_ACTION_BA_ADDBA_REQUEST;
            actionargs.arg1     = tidno;
            actionargs.arg2     = WME_MAX_BA;
            actionargs.arg3     = 0;

            ieee80211_send_action(ni, &actionargs, NULL);
        }
    }

    /* send down each fragment */
    while (wbuf != NULL) {
        int nextfraglen = 0;
        int error = 0;
        ATH_DEFINE_TXCTL(txctl, wbuf);
        HTC_WBUF_TX_DELCARE

        next_wbuf = wbuf_next(wbuf);
        if (next_wbuf != NULL)
            nextfraglen = wbuf_get_pktlen(next_wbuf);

#ifdef ENCAP_OFFLOAD
        if (ath_tx_data_prepare(scn, wbuf, nextfraglen, txctl) != 0)
            goto bad;
#else
        if (ath_tx_prepare_aponly(scn, wbuf, nextfraglen, txctl) != 0)
            goto bad;
#endif
        /* send this frame to hardware */
        txctl->an = (ATH_NODE_NET80211(ni))->an_sta;

        HTC_WBUF_TX_DATA_PREPARE(ic, scn);

        if (likely(error == 0)) {
#ifdef PROFILE_UMAC_2
            if(wbuf_is_encap_done(wbuf)) {
                for (i = 0; i < 3; i++)
                    adf_os_perf_end(perf_cntr[i]);
            }
#endif
            if ((ath_tx_start_aponly(scn->sc_dev, wbuf, txctl)) != 0)
                goto bad;
            else {
                HTC_WBUF_TX_DATA_COMPLETE_STATUS(ic);
            }
        }

        wbuf = next_wbuf;
    }
    
    return 0;

bad:
    /* drop rest of the un-sent fragments */
    while (wbuf != NULL) {
        next_wbuf = wbuf_next(wbuf);
        IEEE80211_TX_COMPLETE_WITH_ERROR(wbuf);

        wbuf = next_wbuf;
    }
    
    return -EIO;
}


int
ath_netdev_hardstart_aponly(struct sk_buff *skb, struct net_device *dev)
{
    struct ath_softc_net80211 *scn = ath_netdev_priv(dev);
#ifdef notyet
#ifndef ATH_SUPPORT_LINUX_STA
    struct ath_softc *sc = ATH_DEV_TO_SC(scn->sc_dev);
#endif
#endif
    struct ieee80211_cb *cb;
    struct ieee80211_node *ni;
    int error = 0;
#ifndef ATH_SUPPORT_HTC
    struct ether_header *eh = (struct ether_header *)skb->data;
    int ismulti = IEEE80211_IS_MULTICAST(eh->ether_dhost) ? 1 : 0;
    u_int16_t addba_status;
    u_int32_t txq_depth, txq_aggr_depth;
#endif
    
    
    //begin of shim 1
   
#ifdef PROFILE_SHIM_1
 
    int     i = 0;

    if(wbuf_is_encap_done(skb)) { 

    if (!shim_root) {
        shim_root = adf_os_perf_init(0, "ath_netdev_hardstart", EVENT_GROUP);

        perf_cntr[0] = adf_os_perf_init(umac_root, "cntr0", EVENT_CPU_CYCLES);
        perf_cntr[1] = adf_os_perf_init(umac_root, "cntr1", EVENT_ICACHE_MISS);
        perf_cntr[2] = adf_os_perf_init(umac_root, "cntr2", EVENT_DCACHE_MISS);
    }

    for (i = 0; i < 3; i++)
        adf_os_perf_start(perf_cntr[i]);
    }

    
#endif
    
    cb = (struct ieee80211_cb *)skb->cb;
    ni = cb->ni;

    /*
     * NB: check for valid node in case kernel directly sends packets
     * on wifiX interface (such as broadcast packets generated by ipv6)
     */
    if (unlikely(ni == NULL)) {
        dev_kfree_skb(skb);
        return 0;
    }

#ifndef ATH_SUPPORT_HTC

    txq_depth = ath_txq_depth_aponly(scn->sc_dev, scn->sc_ac2q[skb->priority]);
    txq_aggr_depth = ath_txq_aggr_depth_aponly(scn->sc_dev, scn->sc_ac2q[skb->priority]);
    ath_net80211_addba_status_aponly(ni,cb->u_tid,&addba_status);

    /*
     * This logic throttles legacy and unaggregated HT frames if they share the hardware
     * queue with aggregates. This improves the transmit throughput performance to
     * aggregation enabled nodes when they coexist with legacy nodes.
     */
    /* Do not throttle EAPOL packets - this causes the REKEY packets
     * to be dropped and station disconnects.
     */
    DPRINTF(scn, ATH_DEBUG_RESET, "skb->priority=%d cb->u_tid=%d addba_status=%d txq_aggr_depth=%d txq_depth=%d\n",skb->priority, cb->u_tid, addba_status, txq_aggr_depth, txq_depth);

    if ((addba_status != IEEE80211_STATUS_SUCCESS) 
        && (txq_aggr_depth > 0)
        && !(eh->ether_type == ETHERTYPE_PAE)) 
    {
        /* Discard this packet if existing q depth is >= 25 */
        if (txq_depth >= 25) {
            goto bad;
        } 

#ifdef notyet /* This should be ported from 7.3 */
#ifndef ATH_SUPPORT_LINUX_STA
        /* Bug 38437 - If the current q depth is greater than the limit we have set for 
           legacy frames, we can put this frame in the holding Q for legacy frames or 
           discard it if the holding Q is also full */

        if (txq_depth >= scn->sc_limit_legacy_frames)  {

            if(scn->sc_limit_legacy_txq_count < (scn->sc_limit_legacy_frames)) {
                enq_legacy_wbuf(scn, skb);
                return 0;
            } else {
                // Holding Q is full, discard it
                goto bad;
            }
        }
        /* Bug 38437 - Holding Q contains older frames which must be sent out first
           so Q this one and send out an older frame from the holding Q*/
        if (!(TAILQ_EMPTY(&scn->sc_limit_legacy_txq)))  {
            error = deq_legacy_wbuf(scn);
            enq_legacy_wbuf(scn, skb);
            return 0;
        }   
#endif
#endif
    }
    /*
     * Try to avoid running out of descriptors 
     */
    if (ismulti) {
        if (txq_depth >= (ATH_TXBUF - 40)) {
            goto bad;
        }
    }
#endif

    //almost end of shim 1
#ifdef PROFILE_SHIM_1
    if(wbuf_is_encap_done(skb)) {
        
        for (i = 0; i < 3; i++)
            adf_os_perf_end(perf_cntr[i]);
    }
#endif 

    error = ath_tx_send_aponly(skb);

#ifdef notyet /* This should be ported from 7.3 */
#ifndef ATH_SUPPORT_LINUX_STA
    if(scn->sc_limit_legacy_txq_count > (txq_depth)) {
        if ((addba_status == IEEE80211_STATUS_SUCCESS) && !(TAILQ_EMPTY(&scn->sc_limit_legacy_txq)))  {
                /* Bug 38437 - For non legacy frames, send from the holding Q if not empty
                   this prevents starvation of the legacy node */
                DPRINTF(scn, ATH_DEBUG_RESET, "txq_saved=%d txq_aggr_depth=%d txq_depth=%d\n",scn->sc_limit_legacy_txq_count, txq_aggr_depth, txq_depth);
                deq_legacy_wbuf(scn);
        }
    } 
#endif
#endif
    
    if (unlikely(error)) {
        DPRINTF(scn, ATH_DEBUG_XMIT, "%s: Tx failed with error %d\n", 
            __func__, error);
    }
    return 0;  

bad:
    IEEE80211_TX_COMPLETE_WITH_ERROR(skb);
    DPRINTF(scn, ATH_DEBUG_XMIT, "%s: Tx failed with error %d\n", 
            __func__, error);
    return 0;  
}


static inline int
ieee80211_send_wbuf_internal_aponly(struct ieee80211vap *vap, wbuf_t wbuf)
{
    int retval;

    vap->iv_lastdata = OS_GET_TIMESTAMP();
    vap->iv_txrxbytes += wbuf_get_pktlen(wbuf);
    /*
     * call back to shim layer to queue it to hardware device.
     */
    retval = vap->iv_evtable->wlan_dev_xmit_queue(vap->iv_ifp, wbuf);

    return retval;
}





static inline int
ieee80211_classify_aponly(struct ieee80211_node *ni, wbuf_t wbuf)
{
    int                                 ac = WME_AC_BE;
    int                                 tid;
#if ATH_SUPPORT_VLAN
    struct ieee80211vap                 *vap = ni->ni_vap;
    int 								v_wme_ac=0;
    int									v_pri = 0;
#endif


    /*
     * Call wbuf_classify(wbuf) function before the
     * "(ni->ni_flags & IEEE80211_NODE_QOS)" check. The reason is that
     * wbuf_classify() is overloaded with setting EAPOL flag in addition to
     * returning TOS for Maverick and Linux platform, where as for Windows it
     * just returns TOS.
     */
    tid = (wbuf_classify(wbuf) & 0x7);
    ac = TID_TO_WME_AC(tid);

    
    /* default priority */
    if (!(ni->ni_flags & IEEE80211_NODE_QOS)) {
	    wbuf_set_priority(wbuf, WME_AC_BE);
        wbuf_set_tid(wbuf, 0);
        return 0;
    }

#if ATH_SUPPORT_VLAN
	/*
	** If this is a QoS node (set after the above comparison, and there is a
	** VLAN tag associated with the packet, we need to ensure we set the
	** priority correctly for the VLAN
	*/

    if ( unlikely(adf_net_vlan_tag_present(wbuf)) )
    {
        unsigned short tag;
        unsigned short vlanID = adf_net_get_vlan((osdev_t) vap->iv_ifp);

        if ( !adf_net_is_vlan_defined((osdev_t) vap->iv_ifp) )
            return 1;

        if (((tag = adf_net_get_vlan_tag(wbuf)) & VLAN_VID_MASK) != (vlanID & VLAN_VID_MASK))
            return 1;

        v_pri = (tag >> VLAN_PRI_SHIFT) & VLAN_PRI_MASK;
    }
    else
    {
        /*
         * If not a member of a VLAN, check if VLAN type and TCI are present in packet.
         * If so, obtain VLAN priority from TCI.
         * Use for determining 802.1p priority.
         */
        v_pri = wbuf_8021p(wbuf);
        
    }

	/*
	** Determine the VLAN AC
	*/

	v_wme_ac = TID_TO_WME_AC(v_pri);


    /* Choose higher priority of implicit VLAN tag or IP DSCP */
    /* TODO: check this behaviour */
    if (v_wme_ac > ac)
    {
		tid = v_pri;
		ac = v_wme_ac;
    }

#endif

    wbuf_set_priority(wbuf, ac);
    wbuf_set_tid(wbuf, tid);
    
    return 0;
}




/*
 * Return a reference to the appropriate node for sending
 * a data frame.  This handles node discovery in adhoc networks.
 */
struct ieee80211_node *
#ifdef IEEE80211_DEBUG_REFCNT
ieee80211_find_txnode_debug_aponly(struct ieee80211vap *vap, const u_int8_t *macaddr,
                            const char *func, int line)
#else
ieee80211_find_txnode_aponly(struct ieee80211vap *vap, const u_int8_t *macaddr)
#endif
{
    struct ieee80211_node_table *nt = &vap->iv_ic->ic_sta;
    struct ieee80211_node *ni = NULL;
    rwlock_state_t lock_state;
#ifndef ATH_BEACON_DEFERRED_PROC
    unsigned long flags = 0;
    OS_RWLOCK_READ_LOCK_IRQSAVE(&nt->nt_nodelock, &lock_state,flags);
#else
    OS_RWLOCK_READ_LOCK(&nt->nt_nodelock, &lock_state);
#endif
    if  (unlikely(IEEE80211_IS_MULTICAST(macaddr))) {
            if (likely(vap->iv_sta_assoc > 0)) {
                ni = ieee80211_ref_node(vap->iv_bss);
            }
            else {
                /* No station associated to AP */
                vap->iv_stats.is_tx_nonode++;
                ni = NULL;
            }
    }
    else {
        ni = _ieee80211_find_node_aponly(nt, macaddr);
    }
#ifndef ATH_BEACON_DEFERRED_PROC
    OS_RWLOCK_READ_UNLOCK_IRQRESTORE(&nt->nt_nodelock, &lock_state,flags);
#else
    OS_RWLOCK_READ_UNLOCK(&nt->nt_nodelock, &lock_state);
#endif
    return ni;
}




/*
 * the main xmit data entry point from OS 
 */
static inline int
wlan_vap_send_aponly(wlan_if_t vap, wbuf_t wbuf)
{
    struct ieee80211_node *ni=NULL;
    u_int8_t *daddr;
    int is_data,retval;

    /* 
     * Find the node for the destination so we can do
     * things like power save and fast frames aggregation.
     */
        struct ether_header *eh;
        eh = (struct ether_header *)wbuf_header(wbuf);
        daddr = eh->ether_dhost;
        is_data = 1;            /* ethernet frame */

#if ATH_SUPPORT_IQUE 
        /*
         * If IQUE is NOT enabled, the ops table is empty and
         * the follow step will be skipped;
         * If IQUE is enabled, and if the packet is a mcast one
         * (and NOT a bcast one), the packet will be converted
         * into ucast packets if the destination in found in the 
         * snoop table, in either Translate way or Tunneling way
         * depending on the mode of mcast enhancement
         */
        if (vap->iv_ique_ops.me_convert &&
            IEEE80211_IS_MULTICAST(eh->ether_dhost) &&
		    vap->iv_sta_assoc > 0 &&
    		!IEEE80211_IS_BROADCAST(eh->ether_dhost))
        {  
                /*
                 * if the convert function returns some value larger
                 * than 0, it means that one or more frames have been
                 * transmitted and we are safe to return from here.
                 */
	    		if (vap->iv_ique_ops.me_convert(vap, wbuf) > 0) {
		    	    return 0;
                }
        }
#endif
    	ni = ieee80211_find_txnode_aponly(vap, daddr);
    	if (unlikely(ni == NULL)) {
		IEEE80211_DPRINTF(vap, IEEE80211_MSG_OUTPUT,
                          "%s: could not send packet, NI equal to NULL for %s\n", 
                          __func__, ether_sprintf(daddr));
        /* NB: ieee80211_find_txnode does stat+msg */
        goto bad;
    }

    /* calculate priority so driver can find the tx queue */
    if (unlikely(ieee80211_classify_aponly(ni, wbuf))) {
        IEEE80211_DPRINTF(vap, IEEE80211_MSG_OUTPUT,
                          "%s: discard, classification failure (%s)\n",
                          __func__, ether_sprintf(daddr));
        goto bad;
    }

        if(ni != vap->iv_bss) {
            if (unlikely(ieee80211_node_get_associd(ni) == 0 || (
                !ieee80211_node_is_authorized(ni)
                && !wbuf_is_eapol(wbuf)))) {
                /*
                * Destination is not authenticated
                */
                IEEE80211_DPRINTF(vap, IEEE80211_MSG_OUTPUT,
                                "%s: could not send packet, DA (%s) is not yet authorized\n", 
                                __func__, ether_sprintf(daddr));
                goto bad;
            }
        }
#if ATH_SUPPORT_IQUE
    /*
	 *	Headline block removal: if the state machine is in 
	 *	BLOCKING or PROBING state, transmision of UDP data frames 
	 *	are blocked untill swtiches back to ACTIVE state.
	 */
      if (vap->iv_ique_ops.hbr_dropblocked) {
          if (vap->iv_ique_ops.hbr_dropblocked(vap, ni, wbuf)) {
               IEEE80211_DPRINTF(vap, IEEE80211_MSG_IQUE,
                                 "%s: packet dropped coz it blocks the headline\n",
                                 __func__);
               goto bad;
          }
      }
#endif
    
    wbuf_set_node(wbuf, ni);    /* associate node with wbuf */

    /* power-save checks */
    if (unlikely((!WME_UAPSD_AC_ISDELIVERYENABLED(wbuf_get_priority(wbuf), ni)) && 
        (ieee80211node_is_paused(ni)) && 
        !ieee80211node_has_flag(ni, IEEE80211_NODE_TEMP))) {
        /*
         * Station in power save mode; pass the frame
         * to the 802.11 layer and continue.  We'll get
         * the frame back when the time is right.
         * XXX lose WDS vap linkage?
         */
        ieee80211node_pause(ni); /* pause it to make sure that no one else unpaused it after the node_is_paused check above, pause operation is ref counted */  
        IEEE80211_DPRINTF(vap, IEEE80211_MSG_OUTPUT,
                          "%s: could not send packet, STA (%s) powersave %d paused %d\n", 
                          __func__, ether_sprintf(daddr), (ni->ni_flags & IEEE80211_NODE_PWR_MGT) ?1 : 0, ieee80211node_is_paused(ni));
        ieee80211_node_saveq_queue(ni, wbuf, (is_data ? IEEE80211_FC0_TYPE_DATA : IEEE80211_FC0_TYPE_MGT));
        ieee80211node_unpause(ni); /* unpause it if we are the last one, the frame will be flushed out */  
        ieee80211_free_node(ni);
        return 0;
    }

    ieee80211_vap_pause_update_xmit_stats(vap,wbuf); /* update the stats for vap pause module */
    retval = ieee80211_send_wbuf_internal_aponly(vap, wbuf);
    
    return retval;
bad:   
    if (IEEE80211_IS_MULTICAST(daddr))
        vap->iv_multicast_stats.ims_tx_discard++;
    else
        vap->iv_unicast_stats.ims_tx_discard++;

    if (ni != NULL)
        ieee80211_free_node(ni);

    /* NB: callee's responsibilty to complete the packet */
    wbuf_set_status(wbuf, WB_STATUS_TX_ERROR);
    wbuf_complete(wbuf);
    
    return -EIO;
}




int
osif_vap_hardstart_aponly(struct sk_buff *skb, struct net_device *dev)
{
    osif_dev  *osdev = ath_netdev_priv(dev);
    wlan_if_t vap = osdev->os_if;
#ifdef PROFILE_UMAC_1
    int i;
    struct ether_header *eh;
    u_int8_t *daddr;
    u_int8_t my_peer_addr[] = {0x00,0x26,0xc6,0x79,0xa0,0x00};
    u_int8_t prof_flag = 0;
    
    eh = (struct ether_header *)wbuf_header(skb);
    daddr = eh->ether_dhost;
    prof_flag = (!memcmp(daddr,my_peer_addr,IEEE80211_ADDR_LEN));   
   
    if(prof_flag)
        wbuf_set_encap_done(skb);
    else
        wbuf_clr_encap_done(skb);

    //move ifdef here to control enabling/disabling umac-1
    //begin of umac-1
    if (!umac_root && prof_flag) {
       umac_root = adf_os_perf_init(0, "umac1_tx_path_ap_only", EVENT_GROUP);

       perf_cntr[0] = adf_os_perf_init(umac_root, "cpu_cycles", EVENT_CPU_CYCLES);
       perf_cntr[1] = adf_os_perf_init(umac_root, "dcache_miss", EVENT_DCACHE_MISS);
       perf_cntr[2] = adf_os_perf_init(umac_root, "icache_miss", EVENT_ICACHE_MISS);
   }
   
   if(prof_flag) { 
    for (i = 0; i < 3; i++)
        adf_os_perf_start(perf_cntr[i]);
   }
#endif 
    spin_lock(&osdev->tx_lock);


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
    //check skb_unshare() to see if it checks for clone function... 
    skb = skb_unshare(skb, GFP_ATOMIC);
    if (unlikely(skb == NULL)) {
        goto bad;
    }
#endif /* ATH_SUPPORT_HTC */

    if (skb_headroom(skb) < dev->hard_header_len) {
        struct sk_buff *tmp = skb;
        skb = skb_realloc_headroom(tmp, dev->hard_header_len);
        dev_kfree_skb(tmp);

        if (skb == NULL) {
            IEEE80211_DPRINTF(vap, IEEE80211_MSG_OUTPUT,
                "%s: cannot expand skb\n", __func__);
            goto bad;
        }
    }

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
    if (wlan_get_powersave(vap) != IEEE80211_PWRSAVE_NONE)
        ath_put_txbuf(skb);
    else
#endif
    wlan_vap_send_aponly(vap, (wbuf_t)skb);
    spin_unlock(&osdev->tx_lock);

#ifdef PROFILE_UMAC_1
   if(prof_flag) { 
    for (i = 0; i < 3; i++)
       adf_os_perf_end(perf_cntr[i]);
   }
#endif
    return 0;

bad:
    spin_unlock(&osdev->tx_lock);
    if (skb != NULL)
        dev_kfree_skb(skb);
    return 0;
}
#endif
