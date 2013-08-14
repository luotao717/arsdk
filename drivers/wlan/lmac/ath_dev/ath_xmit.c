/*
 * Copyright (c) 2009, Atheros Communications Inc.
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

#include "ath_internal.h"
#ifdef ATH_SUPPORT_DFS
#include "dfs.h"
#endif
#include "if_athrate.h"
#include "ratectrl.h"
#include "ath_aow.h"
#include "ath_txseq.h"

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

void
ath_buf_set_rate(struct ath_softc *sc, struct ath_buf *bf);

#if ATH_SUPPORT_IQUE && ATH_SUPPORT_IQUE_EXT
void
ath_rateseries_update_retry(struct ath_softc *sc, struct ath_buf *bf, HAL_11N_RATE_SERIES *series, int retry_duration);
#endif


#if ATH_SUPPORT_VOWEXT
int  ath_deq_single_buf(struct ath_softc *, struct ath_txq *);
#endif

const u_int32_t bits_per_symbol[][2] = {
    /* 20MHz 40MHz */
    {    26,   54 },     //  0: BPSK
    {    52,  108 },     //  1: QPSK 1/2
    {    78,  162 },     //  2: QPSK 3/4
    {   104,  216 },     //  3: 16-QAM 1/2
    {   156,  324 },     //  4: 16-QAM 3/4
    {   208,  432 },     //  5: 64-QAM 2/3
    {   234,  486 },     //  6: 64-QAM 3/4
    {   260,  540 },     //  7: 64-QAM 5/6
    {    52,  108 },     //  8: BPSK
    {   104,  216 },     //  9: QPSK 1/2
    {   156,  324 },     // 10: QPSK 3/4
    {   208,  432 },     // 11: 16-QAM 1/2
    {   312,  648 },     // 12: 16-QAM 3/4
    {   416,  864 },     // 13: 64-QAM 2/3
    {   468,  972 },     // 14: 64-QAM 3/4
    {   520, 1080 },     // 15: 64-QAM 5/6
    {    78,  162 },     // 16: BPSK
    {   156,  324 },     // 17: QPSK 1/2
    {   234,  486 },     // 18: QPSK 3/4
    {   312,  648 },     // 19: 16-QAM 1/2
    {   468,  972 },     // 20: 16-QAM 3/4
    {   624, 1296 },     // 21: 64-QAM 2/3
    {   702, 1458 },     // 22: 64-QAM 3/4
    {   780, 1620 },     // 23: 64-QAM 5/6
    {   104,  216 },     // 24: BPSK
    {   208,  432 },     // 25: QPSK 1/2
    {   312,  648 },     // 26: QPSK 3/4
    {   416,  864 },     // 27: 16-QAM 1/2
    {   624, 1296 },     // 28: 16-QAM 3/4
    {   832, 1728 },     // 29: 64-QAM 2/3
    {   936, 1944 },     // 30: 64-QAM 3/4
    {  1040, 2160 },     // 31: 64-QAM 5/6
};

#if defined(ATH_SUPPORT_HTC) && defined(ATH_HTC_TX_SCHED)
#else
/*
 * Initialize TX queue and h/w
 */
int
ath_tx_init(ath_dev_t dev, int nbufs)
{
    struct ath_softc *sc = ATH_DEV_TO_SC(dev);
    int error = 0;

    do {
        ATH_TXBUF_LOCK_INIT(sc);

        /* Setup tx descriptors */
        error = ath_descdma_setup(sc, &sc->sc_txdma, &sc->sc_txbuf,
                                  "tx", nbufs, ATH_TXDESC, 1, ATH_FRAG_PER_MSDU);
#if ATH_TX_BUF_FLOW_CNTL
        sc->sc_txbuf_free = nbufs;
#endif

        if (error != 0) {
            printk("failed to allocate tx descriptors: %d\n", error);
            break;
        }

        /* XXX allocate beacon state together with vap */
        error = ath_descdma_setup(sc, &sc->sc_bdma, &sc->sc_bbuf,
                                  "beacon", ATH_BCBUF, 1, 1, ATH_FRAG_PER_MSDU);
        if (error != 0) {
            printk("failed to allocate beacon descripotrs: %d\n", error);
            break;
        }

#ifdef ATH_SUPPORT_UAPSD
        /* Initialize uapsd descriptors */
        error = ath_tx_uapsd_init(sc);
        if (error != 0) {
            printk("failed to allocate UAPSD descripotrs: %d\n", error);
            break;
        }
#endif
#if ATH_SUPPORT_CFEND
        /* initialise CF-END descriptor*/
        error = ath_tx_cfend_init(sc);

        if (error != 0) {
            break;
        } 
#endif
        /* initialise PAPRD descriptor*/
        error = ath_tx_paprd_init(sc);
        if (error != 0) {
            break;
        } 
        error = ath_tx_edma_init(sc);
        if (error != 0) {
            printk("failed to allocate tx status descriptors: %d\n", error);
            break;
        }

    } while (0);

    if (error != 0) {        
        ath_tx_cleanup(sc);
    }
    
    return error;
}

/*
 * Reclaim all tx queue resources.
 */
int
ath_tx_cleanup(ath_dev_t dev)
{
    struct ath_softc *sc = ATH_DEV_TO_SC(dev);

    ATH_PS_WAKEUP(sc);

#ifdef ATH_SUPPORT_UAPSD
    /* cleanup uapsd descriptors */
    ath_tx_uapsd_cleanup(sc);
#endif

    /* cleanup beacon descriptors */
    if (sc->sc_bdma.dd_desc_len != 0)
        ath_descdma_cleanup(sc, &sc->sc_bdma, &sc->sc_bbuf);

    /* cleanup tx descriptors */
    if (sc->sc_txdma.dd_desc_len != 0)
        ath_descdma_cleanup(sc, &sc->sc_txdma, &sc->sc_txbuf);
#if ATH_TX_BUF_FLOW_CNTL
    sc->sc_txbuf_free = 0;
#endif
#if ATH_SUPPORT_CFEND
    /* cleanup cfend descriptor */
    ath_tx_cfend_cleanup(sc);
#endif
    /* cleanup paprd descriptor */
    ath_tx_paprd_cleanup(sc);
    ath_tx_edma_cleanup(sc);

    ATH_TXBUF_LOCK_DESTROY(sc);
    ATH_PS_SLEEP(sc);
    return 0;
}

#endif
/*
 * Setup a h/w transmit queue.
 */
struct ath_txq *
ath_txq_setup(struct ath_softc *sc, int qtype, int subtype)
{
#define    N(a)    (sizeof(a)/sizeof(a[0]))
    struct ath_hal *ah = sc->sc_ah;
    HAL_TXQ_INFO qi;
    int qnum;
#ifdef ATH_SUPERG_COMP
    u_int compbufsz = 0;
    char *compbuf = NULL;
    dma_addr_t compbufp = 0;
    OS_DMA_MEM_CONTEXT(dmacontext)
#endif
    OS_MEMZERO(&qi, sizeof(qi));
    qi.tqi_subtype = subtype;
    qi.tqi_aifs = HAL_TXQ_USEDEFAULT;
    qi.tqi_cwmin = HAL_TXQ_USEDEFAULT;
    qi.tqi_cwmax = HAL_TXQ_USEDEFAULT;
    qi.tqi_compBuf = 0;
#ifdef ATH_SUPERG_XR
    if(subtype == HAL_XR_DATA) {
        qi.tqi_aifs  = XR_DATA_AIFS;
        qi.tqi_cwmin = XR_DATA_CWMIN;
        qi.tqi_cwmax = XR_DATA_CWMAX;
    }
#endif

#ifdef ATH_SUPERG_COMP
    /* allocate compression scratch buffer for data queues */
    if (((qtype == HAL_TX_QUEUE_DATA)|| (qtype == HAL_TX_QUEUE_UAPSD)) 
        && ath_hal_compressionsupported(ah)) {
        compbufsz = roundup(HAL_COMP_BUF_MAX_SIZE, 
                            HAL_COMP_BUF_ALIGN_SIZE) + HAL_COMP_BUF_ALIGN_SIZE;
        compbuf = (char *)OS_MALLOC_CONSISTENT(sc->sc_osdev,
                            compbufsz, &compbufp, dmacontext,
                                sc->sc_reg_parm.shMemAllocRetry);
        if (compbuf == NULL) {
            /* clear compression flag */
            sc->sc_hascompression = 0;
        } else {
            qi.tqi_compBuf = (u_int32_t)compbufp;
        }
    } 
#endif
    if (sc->sc_enhanceddmasupport) {
        /* 
         * The DESC based interrupts are not available.
         */
        qi.tqi_qflags = TXQ_FLAG_TXOKINT_ENABLE | TXQ_FLAG_TXERRINT_ENABLE;
    } else {
        /*
         * Enable interrupts only for EOL and DESC conditions.
         * We mark tx descriptors to receive a DESC interrupt
         * when a tx queue gets deep; otherwise waiting for the
         * EOL to reap descriptors.  Note that this is done to
         * reduce interrupt load and this only defers reaping
         * descriptors, never transmitting frames.  Aside from
         * reducing interrupts this also permits more concurrency.
         * The only potential downside is if the tx queue backs
         * up in which case the top half of the kernel may backup
         * due to a lack of tx descriptors.
         *
         * The UAPSD & CFEBDQ queue is an exception, since we take a desc-
         * based intr on the EOSP frames.
         */
        if ((qtype == HAL_TX_QUEUE_UAPSD) || 
            (qtype == HAL_TX_QUEUE_CFEND) ||
            (qtype == HAL_TX_QUEUE_PAPRD)) {
            qi.tqi_qflags = TXQ_FLAG_TXDESCINT_ENABLE;
        } else {
            qi.tqi_qflags = TXQ_FLAG_TXEOLINT_ENABLE | TXQ_FLAG_TXDESCINT_ENABLE;
        }
    }
    qnum = ath_hal_setuptxqueue(ah, qtype, &qi);
    if (qnum == -1) {
        /*
         * NB: don't print a message, this happens
         * normally on parts with too few tx queues
         */
#ifdef ATH_SUPERG_COMP
        if (compbuf) {
            OS_FREE_CONSISTENT(sc->sc_osdev, compbufsz,
                               compbuf, compbufp, dmacontext);
        }
#endif
        return NULL;
    }
    if (qnum >= N(sc->sc_txq)) {
        printk("hal qnum %u out of range, max %u!\n",
               qnum, (unsigned int)N(sc->sc_txq));
#ifdef ATH_SUPERG_COMP
        if (compbuf) {
            OS_FREE_CONSISTENT(sc->sc_bdev, compbufsz,
                               compbuf, compbufp, dmacontext);
        }
#endif
        ath_hal_releasetxqueue(ah, qnum);
        return NULL;
    }
    if (!ATH_TXQ_SETUP(sc, qnum)) {
        struct ath_txq *txq = &sc->sc_txq[qnum];
        txq->axq_qnum = qnum;
        txq->axq_link = NULL;
        TAILQ_INIT(&txq->axq_q);
        TAILQ_INIT(&txq->axq_acq);
        ATH_TXQ_LOCK_INIT(txq);
        txq->axq_depth = 0;
#if ATH_TX_BUF_FLOW_CNTL
        txq->axq_num_buf_used = 0;


        if (qtype == HAL_TX_QUEUE_DATA) {
            switch (subtype) {
                case HAL_WME_AC_BK:
                    txq->axq_minfree = (sc->sc_reg_parm.ACBKMinfree > 0 && sc->sc_reg_parm.ACBKMinfree <= ATH_TXBUF) ? \
                                       sc->sc_reg_parm.ACBKMinfree : 16 * (HAL_WME_AC_VO - subtype);
                    break;
                case HAL_WME_AC_BE:
                    txq->axq_minfree = (sc->sc_reg_parm.ACBEMinfree > 0 && sc->sc_reg_parm.ACBEMinfree <= ATH_TXBUF) ? \
                                       sc->sc_reg_parm.ACBEMinfree : 16 * (HAL_WME_AC_VO - subtype);
                    break;
                case HAL_WME_AC_VI:
                    txq->axq_minfree = (sc->sc_reg_parm.ACVIMinfree > 0 && sc->sc_reg_parm.ACVIMinfree <= ATH_TXBUF) ? \
                                       sc->sc_reg_parm.ACVIMinfree : 16 * (HAL_WME_AC_VO - subtype);
                    break;
                case HAL_WME_AC_VO:
                    txq->axq_minfree = (sc->sc_reg_parm.ACVOMinfree > 0 && sc->sc_reg_parm.ACVOMinfree <= ATH_TXBUF) ? \
                                       sc->sc_reg_parm.ACVOMinfree : 16 * (HAL_WME_AC_VO - subtype);
                    break;
                default:
                    txq->axq_minfree = 0;
            }
        } else if (qtype == HAL_TX_QUEUE_CAB) {
            txq->axq_minfree = (sc->sc_reg_parm.CABMinfree > 0 && sc->sc_reg_parm.CABMinfree <= ATH_TXBUF) ? \
                               sc->sc_reg_parm.CABMinfree : MCAST_MIN_FREEBUF;
#ifdef ATH_SUPPORT_UASPD
        } else if (qtype == HAL_TX_QUEUE_UASPD) {
            txq->axq_minfree = (sc->sc_reg_parm.UASPDMinfree > 0 && sc->sc_reg_parm.UASPDMinfree <= ATH_TXBUF) ? \
                               sc->sc_reg_parm.UASPDMinfree : 0;
#endif
        } else {
            /* XXX: Do we need flow control for other queues? */
            txq->axq_minfree = 0;
        }
#endif
        txq->irq_shared = 0;
        txq->axq_aggr_depth = 0;
        txq->axq_totalqueued = 0;
        txq->axq_intrcnt = 0;
        txq->axq_linkbuf = NULL;
        //TAILQ_INIT(&txq->axq_stageq);
        sc->sc_txqsetup |= 1<<qnum;
#ifdef ATH_SUPERG_COMP
        txq->axq_compbuf = compbuf;
        txq->axq_compbufsz = compbufsz;
        txq->axq_compbufp = compbufp;
#ifdef ATH_SWRETRY
        txq->axq_destmask = AH_TRUE;
#endif
        for (int i = 0; i < HAL_TXFIFO_DEPTH; i++) {
            TAILQ_INIT(&txq->axq_fifo[i]);
        }
        txq->axq_headindex = 0;
        txq->axq_tailindex = 0;
        OS_COPY_DMA_MEM_CONTEXT(OS_GET_DMA_MEM_CONTEXT(txq, axq_dmacontext),
                                dmacontext);
#endif
    }
    return &sc->sc_txq[qnum];
#undef N
}

/*
 * Reclaim resources for a setup queue.
 */
void
ath_tx_cleanupq(struct ath_softc *sc, struct ath_txq *txq)
{
#ifdef ATH_SUPERG_COMP
    /* Release compression buffer */
    if (txq->axq_compbuf) {
        OS_FREE_CONSISTENT(sc->sc_bdev, txq->axq_compbufsz,
                           txq->axq_compbuf, txq->axq_compbufp,
                           OS_GET_DMA_MEM_CONTEXT(txq, axq_dmacontext));
        txq->axq_compbuf = NULL;
    }
#endif
    if (!sc->sc_invalid) {         /* if the device is invalid or removed */
        ath_hal_releasetxqueue(sc->sc_ah, txq->axq_qnum);
    }
    ATH_TXQ_LOCK_DESTROY(txq);
    sc->sc_txqsetup &= ~(1<<txq->axq_qnum);
}

int
ath_set_tx(struct ath_softc *sc)
{
    if (!ath_tx_setup(sc, HAL_WME_AC_BK)) {
        printk("unable to setup xmit queue for BE traffic!\n");

        return 0;
    }

    if (!ath_tx_setup(sc, HAL_WME_AC_BE) ||
        !ath_tx_setup(sc, HAL_WME_AC_VI) ||
        !ath_tx_setup(sc, HAL_WME_AC_VO)) {
        /*
         * Not enough hardware tx queues to properly do WME;
         * just punt and assign them all to the same h/w queue.
         * We could do a better job of this if, for example,
         *  ath_hal_getmcastkeysearch(ah)
         * we allocate queues when we switch from station to
         * AP mode.
         */
        if (sc->sc_haltype2q[HAL_WME_AC_VI] != -1)
            ath_tx_cleanupq(sc, &sc->sc_txq[sc->sc_haltype2q[HAL_WME_AC_VI]]);
        if (sc->sc_haltype2q[HAL_WME_AC_BE] != -1)
            ath_tx_cleanupq(sc, &sc->sc_txq[sc->sc_haltype2q[HAL_WME_AC_BE]]);
        sc->sc_haltype2q[HAL_WME_AC_BE] = sc->sc_haltype2q[HAL_WME_AC_BK];
        sc->sc_haltype2q[HAL_WME_AC_VI] = sc->sc_haltype2q[HAL_WME_AC_BK];
        sc->sc_haltype2q[HAL_WME_AC_VO] = sc->sc_haltype2q[HAL_WME_AC_BK];
    } else {
        /*
         * Mark WME capability since we have sufficient
         * hardware queues to do proper priority scheduling.
         */
        sc->sc_haswme = 1;
#ifdef ATH_SUPPORT_UAPSD
        sc->sc_uapsdq = ath_txq_setup(sc, HAL_TX_QUEUE_UAPSD, 0);
        if (sc->sc_uapsdq == NULL) {
            DPRINTF(sc, ATH_DEBUG_UAPSD,
                    "%s: unable to setup UAPSD xmit queue!\n", __func__);
        }
        else {
            /*
             * default UAPSD on if HW capable
             */
            sc->sc_uapsdsupported = 1;
        }
#endif
    }
#if ATH_SUPPORT_PAPRD
    sc->sc_paprdq = ath_txq_setup(sc, HAL_TX_QUEUE_PAPRD, 0);
    if (sc->sc_paprdq == NULL) {
        DPRINTF(sc, ATH_DEBUG_CALIBRATE,
                "%s: unable to setup PAPRD xmit queue!\n", __func__);
    }
    else {
    }
#endif

    return 1;
}

/*
 * Setup a hardware data transmit queue for the specified
 * access control.  The hal may not support all requested
 * queues in which case it will return a reference to a
 * previously setup queue.  We record the mapping from ac's
 * to h/w queues for use by ath_tx_start and also track
 * the set of h/w queues being used to optimize work in the
 * transmit interrupt handler and related routines.
 */
int
ath_tx_setup(struct ath_softc *sc, int haltype)
{
#define    N(a)    (sizeof(a)/sizeof(a[0]))
    struct ath_txq *txq;

    if (haltype >= N(sc->sc_haltype2q)) {
        printk("HAL AC %u out of range, max %zu!\n",
               haltype, N(sc->sc_haltype2q));
        return 0;
    }
    txq = ath_txq_setup(sc, HAL_TX_QUEUE_DATA, haltype);
    if (txq != NULL) {
        sc->sc_haltype2q[haltype] = txq->axq_qnum;
        return 1;
    } else
        return 0;
#undef N
}

int
ath_tx_get_qnum(ath_dev_t dev, int qtype, int haltype)
{
#define    N(a)    (sizeof(a)/sizeof(a[0]))
    struct ath_softc *sc = ATH_DEV_TO_SC(dev);
    int qnum;
    
    switch (qtype) {
    case HAL_TX_QUEUE_DATA:
        if (haltype >= N(sc->sc_haltype2q)) {
            printk("HAL AC %u out of range, max %zu!\n",
                   haltype, N(sc->sc_haltype2q));
            return -1;
        }
        qnum = sc->sc_haltype2q[haltype];
        break;
    case HAL_TX_QUEUE_BEACON:
        qnum = sc->sc_bhalq;
        break;
#ifdef ATH_SUPPORT_UAPSD
    case HAL_TX_QUEUE_UAPSD:
        qnum = sc->sc_uapsdq->axq_qnum;
        break;
#endif
    case HAL_TX_QUEUE_CAB:
        qnum = sc->sc_cabq->axq_qnum;
        break;
    default:
        qnum = -1;
    }
    return qnum;
#undef N
}

/*
 * Update parameters for a transmit queue.
 */
int
ath_txq_update(ath_dev_t dev, int qnum, HAL_TXQ_INFO *qi0)
{
    struct ath_softc *sc = ATH_DEV_TO_SC(dev);
    struct ath_hal *ah = sc->sc_ah;
    int error = 0;
    HAL_TXQ_INFO qi;

    if (qnum == sc->sc_bhalq) {
        /*
         * XXX:
         * for beacon queue, we just save the parameter. It will be picked
         * up by ath_beaconq_config when it's necessary.
         */
        sc->sc_beacon_qi = *qi0;
        return 0;
    }
    
    ATH_PS_WAKEUP(sc);

    ASSERT(sc->sc_txq[qnum].axq_qnum == qnum);

    ath_hal_gettxqueueprops(ah, qnum, &qi);
    qi.tqi_aifs = qi0->tqi_aifs;
    qi.tqi_cwmin = qi0->tqi_cwmin;
    qi.tqi_cwmax = qi0->tqi_cwmax;
    qi.tqi_burstTime = qi0->tqi_burstTime;
    qi.tqi_readyTime = qi0->tqi_readyTime;

#if ATH_SUPPORT_AGGR_BURST
    if (sc->sc_aggr_burst) {
        if (qnum == HAL_WME_AC_BE) {
            qi.tqi_burstTime = sc->sc_aggr_burst_duration; 
        }
    }
#endif

    ath_htc_txq_update(dev, qnum, &qi);

    if (!ath_hal_settxqueueprops(ah, qnum, &qi)) {
        printk("%s: unable to update hardware queue %u!\n",
               __func__, qnum);
        error = -EIO;
    } else {
        ath_hal_resettxqueue(ah, qnum); /* push to h/w */
#if ATH_SUPPORT_DFS
        /* in DFS channel in case above function overwrites interframe spacing adjustment */
        if((sc->sc_curchan.privFlags & CHANNEL_DFS) && (!sc->sc_scanning) &&(!sc->sc_nostabeacons)) {
            ath_adjust_difs(sc);
        }
#endif
    }

    ATH_PS_SLEEP(sc);
    return error;
}

#if ATH_SUPPORT_AGGR_BURST
void
ath_txq_burst_update(ath_dev_t dev, int qnum, u_int32_t duration)
{
    struct ath_softc *sc = ATH_DEV_TO_SC(dev);
    HAL_TXQ_INFO qi;

    ath_hal_gettxqueueprops(sc->sc_ah, qnum, &qi);
    qi.tqi_burstTime = duration;
    ath_txq_update(sc, qnum, &qi);
}
#endif

int
ath_cabq_update(struct ath_softc *sc)
{
    HAL_TXQ_INFO qi;
    int qnum = sc->sc_cabq->axq_qnum;
    ieee80211_beacon_config_t conf;

    ath_hal_gettxqueueprops(sc->sc_ah, qnum, &qi);
    /*
     * Ensure the readytime % is within the bounds.
     */
    if (sc->sc_config.cabqReadytime < HAL_READY_TIME_LO_BOUND) {
        sc->sc_config.cabqReadytime = HAL_READY_TIME_LO_BOUND;
    } else if (sc->sc_config.cabqReadytime > HAL_READY_TIME_HI_BOUND) {
        sc->sc_config.cabqReadytime = HAL_READY_TIME_HI_BOUND;
    }

    sc->sc_ieee_ops->get_beacon_config(sc->sc_ieee, ATH_IF_ID_ANY, &conf);
    qi.tqi_readyTime = (conf.beacon_interval * sc->sc_config.cabqReadytime)/100;
    ath_txq_update(sc, qnum, &qi);

    return 0;
}

void
ath_set_protmode(ath_dev_t dev, PROT_MODE mode)
{
    ATH_DEV_TO_SC(dev)->sc_protmode = mode;
}

void
ath_tx_mcast_draintxq(struct ath_softc *sc, struct ath_txq *txq)
{
    struct ath_buf *bf, *lastbf;
    ath_bufhead bf_head;

    /*
     * NB: this assumes output has been stopped and
     *     we do not need to block ath_tx_tasklet
     */
    for (;;) {
        ATH_TXQ_LOCK(txq);
        bf = TAILQ_FIRST(&txq->axq_q);
        if (bf == NULL) {
            if (!sc->sc_enhanceddmasupport) {
                txq->axq_link = NULL;
                txq->axq_linkbuf = NULL;
            }
            ATH_TXQ_UNLOCK(txq);
            break;         }

        lastbf = bf->bf_lastbf;
        lastbf->bf_isswaborted = 1;

        ATH_TXQ_MOVE_HEAD_UNTIL(txq, &bf_head, lastbf, bf_list);

        ATH_TXQ_UNLOCK(txq);

#ifdef ATH_SUPPORT_TxBF
        ath_tx_complete_buf(sc, bf, &bf_head, 0, 0, 0);
#else
        ath_tx_complete_buf(sc, bf, &bf_head, 0);
#endif
    }
}

/*
 * Insert a chain of ath_buf (descriptors) on a multicast txq
 * but do NOT start tx DMA on this queue.
 * NB: must be called with txq lock held
 */
static INLINE void
ath_tx_mcastqaddbuf_internal(struct ath_softc *sc, struct ath_txq *txq, ath_bufhead *head)
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
    ** *always* going to have more data when adding to this queue, no matter
    ** where you call from.
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

    /*
    ** Now, concat the frame onto the queue
    */

	TAILQ_FOREACH(tbf, head, bf_list) {		
		OS_SYNC_SINGLE(sc->sc_osdev, tbf->bf_daddr, 
						sc->sc_txdesclen, BUS_DMA_TODEVICE, NULL);
	}	
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

    //sc->sc_devstats.tx_packets++;
    //sc->sc_devstats.tx_bytes += framelen;
#undef DESC2PA
}

/*
 * external/public function (exposed to outside this module) to
 * add a buf to the mcast queue.
 * could cause increse in code size.
 */
int
ath_tx_mcastqaddbuf(struct ath_softc *sc, struct ath_vap *avp,
        ath_bufhead *head)
{
     atomic_inc(&avp->av_beacon_cabq_use_cnt);
     if (atomic_read(&avp->av_stop_beacon) ||
         avp->av_bcbuf == NULL) {
         atomic_dec(&avp->av_beacon_cabq_use_cnt);
         return EINVAL;
      }

      ath_tx_mcastqaddbuf_internal(sc, &avp->av_mcastq, head);
      atomic_dec(&avp->av_beacon_cabq_use_cnt);
      return EOK;
}

#if ATH_RESET_SERIAL
int
__ath_queue_txbuf(struct ath_softc *sc, int qnum, ath_bufhead *head)
{
    TAILQ_CONCAT(&(sc->sc_queued_txbuf[qnum].txbuf_head), head, bf_list);
    return 0;
}

int
ath_queue_txbuf(struct ath_softc *sc, int qnum, ath_bufhead *head)
{
    ATH_RESET_LOCK_Q(sc, qnum);
    __ath_queue_txbuf(sc, qnum, head);
    ATH_RESET_UNLOCK_Q(sc, qnum);

    return 0;
        
}

#endif

#if ATH_RESET_SERIAL
int
_ath_tx_txqaddbuf(struct ath_softc *sc, struct ath_txq *txq, ath_bufhead *head)
{
    struct ath_hal *ah = sc->sc_ah;
    struct ath_buf *bf;

    bf = TAILQ_FIRST(head);
    if (bf == NULL)
        return 0;
    
    ATH_TXQ_CONCAT(txq, head);
    DPRINTF(sc, ATH_DEBUG_TX_PROC, "%s: txq depth = %d\n",
                __func__, txq->axq_depth);

    if (txq->axq_link == NULL) {
        ath_hal_puttxbuf(ah, txq->axq_qnum, bf->bf_daddr);
        DPRINTF(sc, ATH_DEBUG_XMIT, "%s: TXDP[%u] = %llx (%p)\n",
                __func__, txq->axq_qnum, ito64(bf->bf_daddr), bf->bf_desc);
    } else {
#ifdef AH_NEED_DESC_SWAP
        *txq->axq_link = cpu_to_le32(bf->bf_daddr);
#else
        *txq->axq_link = bf->bf_daddr;
#endif
        DPRINTF(sc, ATH_DEBUG_XMIT, "%s: link[%u] (%p)=%llx (%p)\n",
                __func__,
                txq->axq_qnum, txq->axq_link,
                ito64(bf->bf_daddr), bf->bf_desc);
    }
    ath_hal_getdesclinkptr(ah, bf->bf_lastbf->bf_desc, &(txq->axq_link));
#ifdef DBG
    if (ath_hal_gettxbuf(ah, txq->axq_qnum) == 0) {
        /* This will cause an NMI since TXDP is 0 */
        printk("%s: FATAL ERROR: NULL TXDP while enabling TX.\n", __func__);
        ASSERT(FALSE);
    } else 
#endif
       ath_hal_txstart(ah, txq->axq_qnum);

#if ATH_TX_POLL
    txq->axq_lastq_tick = OS_GET_TICKS();
#endif
    return 0;
    // sc->sc_dev->trans_start = jiffies; 
}
#endif

/*
 * Insert a chain of ath_buf (descriptors) on a txq and
 * assume the descriptors are already chained together by caller.
 * NB: must be called with txq lock held
 */
int
ath_tx_txqaddbuf(struct ath_softc *sc, struct ath_txq *txq, ath_bufhead *head)
{
#define DESC2PA(_sc, _va)	\
		((caddr_t)(_va) - (caddr_t)((_sc)->sc_txdma.dd_desc) + \
				(_sc)->sc_txdma.dd_desc_paddr)	
    struct ath_hal *ah = sc->sc_ah;
    struct ath_buf *bf;

	struct ath_buf *tbf;
#ifdef ATH_SWRETRY
    struct ath_node *an;
    struct ieee80211_frame *wh;
#endif
    /*
     * Insert the frame on the outbound list and
     * pass it on to the hardware.
     */

    bf = TAILQ_FIRST(head);
    if (bf == NULL)
        return 0;

#ifdef ATH_SWRETRY
    an = ATH_NODE(bf->bf_node);
    wh = (struct ieee80211_frame *)wbuf_header(bf->bf_mpdu);

    /* backward compatible with Merlin SW retry code */
    if (!sc->sc_enhanceddmasupport) {
        if (an && an->an_total_swrtx_pendfrms && !(bf->bf_status & ATH_BUFSTATUS_MARKEDSWRETRY)) {

            ATH_NODE_SWRETRY_TXBUF_LOCK(an);
            if (!(an->an_flags & ATH_NODE_SWRETRYQ_FLUSH)) {
                /* the sw retry queue allows no more than HAL_TXFIFO_DEPTH packets buffered */
                if (an->an_softxmit_qdepth >= HAL_TXFIFO_DEPTH) {
                    ATH_NODE_SWRETRY_TXBUF_UNLOCK(an);
                    return -ENOMEM;
                }
                TAILQ_CONCAT(&(an->an_softxmit_q), head, bf_list);
                an->an_softxmit_qdepth++;

                DPRINTF(sc, ATH_DEBUG_SWR, "%s: Queuing frm with SeqNo%d"
                        "to SxQ: SxQdepth %d pendfrms %d\n",__func__,
                            (*(u_int16_t *)&wh->i_seq[0]) >> 4,
                            an->an_softxmit_qdepth,
                            an->an_total_swrtx_pendfrms);

                ATH_NODE_SWRETRY_TXBUF_UNLOCK(an);

                return 0;
            }
            ATH_NODE_SWRETRY_TXBUF_UNLOCK(an);
        }
    }

    if (bf->bf_isswretry)
        DPRINTF(sc, ATH_DEBUG_SWR, "%s: Queuing frm with SeqCtrl 0x%02X%02X to HwQ\n",
                __func__, wh->i_seq[0], wh->i_seq[1]);
#endif

#ifdef ATH_C3_WAR
    /* Work around for low PCIe bus utilization when CPU is in C3 state */
    if (sc->sc_reg_parm.c3WarTimerPeriod &&
        bf->bf_isdata &&
        sc->sc_fifo_underrun &&
        sc->sc_c3_war_timer) {
        spin_lock(&sc->sc_c3_war_lock);
        if (!sc->c3_war_timer_active) {
            sc->c3_war_timer_active = 1;
            ath_gen_timer_start(sc, 
                                sc->sc_c3_war_timer,
                                ath_gen_timer_gettsf32(sc, sc->sc_c3_war_timer),
                                sc->sc_reg_parm.c3WarTimerPeriod);
            printk("%s: sc_c3_war_timer is started!!\n", __func__);
        }
        spin_unlock(&sc->sc_c3_war_lock);
    }
#endif

    if (sc->sc_enhanceddmasupport) {
        /* FIFO is full, return error */
        if (txq->axq_depth == HAL_TXFIFO_DEPTH) {
            printk("[%d] TXQ MAX DEPTH!!!\n", txq->axq_qnum);
            return -ENOMEM;
        }

#ifdef ATH_SWRETRY
        /*
         * clear the dest mask if this is the first frame scheduled after all swr eligible
         * frames have been popped out from txq.
         * NOTE: papard frame has NULL an, just ignore it.
        */
        if (an) {
            if ((an->an_swretry_info[txq->axq_qnum]).swr_state_filtering &&
                !(an->an_swretry_info[txq->axq_qnum]).swr_num_eligible_frms) {
                ATH_NODE_SWRETRY_TXBUF_LOCK(an);
#if ATH_SWRETRY_MODIFY_DSTMASK
                (an->an_swretry_info[txq->axq_qnum]).swr_need_cleardest = AH_TRUE;
                ath_tx_modify_cleardestmask(sc, txq, head);
#endif
                (an->an_swretry_info[txq->axq_qnum]).swr_state_filtering = AH_FALSE;
                ATH_NODE_SWRETRY_TXBUF_UNLOCK(an);
                DPRINTF(sc, ATH_DEBUG_SWR, "%s: CLR DEST MASK dst=%s SeqCtrl=0x%02X%02X qnum=%d swr_num_eligible_frms=%d\n",
                        __func__, ether_sprintf(wh->i_addr1), wh->i_seq[0], wh->i_seq[1],
                        txq->axq_qnum, (an->an_swretry_info[txq->axq_qnum]).swr_num_eligible_frms);
            }
        }
#endif
		TAILQ_FOREACH(tbf, head, bf_list) {		
			OS_SYNC_SINGLE(sc->sc_osdev, tbf->bf_daddr, 
							sc->sc_txdesclen, BUS_DMA_TODEVICE, NULL);
		}	

        ATH_EDMA_TXQ_CONCAT(txq, head);

        /* Write the tx descriptor address into the hw fifo */
        ASSERT(bf->bf_daddr != 0);
        OS_WMB();
        ath_hal_puttxbuf(ah, txq->axq_qnum, bf->bf_daddr);

        DPRINTF(sc, ATH_DEBUG_XMIT, "%s: TXDP[%u] = %llx == %u (%p)\n",
                __func__, txq->axq_qnum, ito64(bf->bf_daddr),
                ath_hal_gettxbuf(sc->sc_ah, txq->axq_qnum), bf->bf_desc);
    } else {
#if ATH_RESET_SERIAL
         ATH_RESET_LOCK(sc);
         if (atomic_read(&sc->sc_hold_reset)) { /* in reset */
            if (atomic_read(&sc->sc_reset_queue_flag)) {
                ath_queue_txbuf(sc, txq->axq_qnum, head);
                ATH_RESET_UNLOCK(sc);
                return 0;
            }
            else {
                ATH_RESET_UNLOCK(sc);
                ATH_TXQ_UNLOCK(txq);
                if (bf->bf_isampdu) {

                    struct ath_buf *lastbf = bf->bf_lastbf;

                    if (!bf->bf_isaggr) {
                        __11nstats(sc,tx_unaggr_comperror);
                    }
                    ath_tx_complete_aggr_rifs(sc, txq, bf, head,
                        &((struct ath_desc *)(lastbf->bf_desc))->ds_txstat, 0);
                } else {
                    if (bf->bf_isbar) {
                        ath_tx_complete_bar(sc, bf, head, 0);
                    }
                    else
#ifdef ATH_SUPPORT_TxBF
                        ath_tx_complete_buf(sc, bf, head, 0, 0, 0);
#else                
                        ath_tx_complete_buf(sc, bf, head, 0);
#endif
                }
                ATH_TXQ_LOCK(txq);

                return 0;
            }
        } 
        
        atomic_inc(&sc->sc_tx_add_processing);
        ATH_RESET_UNLOCK(sc);
#else
        
        /* Acquire lock to have mutual exclusion with the Reset code calling ath_hal_reset */
        ATH_RESET_LOCK(sc);
       
        if (atomic_read(&sc->sc_in_reset)) {
            /* We are in the middle of a reset. Dump this Tx packet. */
            ATH_RESET_UNLOCK(sc);

            DPRINTF(sc, ATH_DEBUG_TX_PROC, "%s: in reset, dump tx packet.\n", __func__);

            ATH_TXQ_UNLOCK(txq);
            if (bf->bf_isampdu) {

                struct ath_buf *lastbf = bf->bf_lastbf;

                if (!bf->bf_isaggr) {
                    __11nstats(sc,tx_unaggr_comperror);
                }
                ath_tx_complete_aggr_rifs(sc, txq, bf, head,
                                          &((struct ath_desc *)(lastbf->bf_desc))->ds_txstat, 0);
            } else {
                if (bf->bf_isbar) {
                    ath_tx_complete_bar(sc, bf, head, 0);
                }
                else
#ifdef ATH_SUPPORT_TxBF
                    ath_tx_complete_buf(sc, bf, head, 0, 0, 0);
#else                
                    ath_tx_complete_buf(sc, bf, head, 0);
#endif
            }
            ATH_TXQ_LOCK(txq);

            return 0;
        }
#endif    
		tbf = TAILQ_FIRST(head);
        ATH_TXQ_CONCAT(txq, head);
        DPRINTF(sc, ATH_DEBUG_TX_PROC, "%s: txq depth = %d\n",
                __func__, txq->axq_depth);

        if (txq->axq_link == NULL) {
            ath_hal_puttxbuf(ah, txq->axq_qnum, bf->bf_daddr);
            DPRINTF(sc, ATH_DEBUG_XMIT, "%s: TXDP[%u] = %llx (%p)\n",
                    __func__, txq->axq_qnum, ito64(bf->bf_daddr), bf->bf_desc);
        } else {
#ifdef AH_NEED_DESC_SWAP
            *txq->axq_link = cpu_to_le32(bf->bf_daddr);
#else
            *txq->axq_link = bf->bf_daddr;
#endif
			OS_SYNC_SINGLE(sc->sc_osdev, (dma_addr_t)(DESC2PA(sc, txq->axq_link)), 
							sizeof(u_int32_t *), BUS_DMA_TODEVICE, NULL);
            DPRINTF(sc, ATH_DEBUG_XMIT, "%s: link[%u] (%p)=%llx (%p)\n",
                    __func__,
                    txq->axq_qnum, txq->axq_link,
                    ito64(bf->bf_daddr), bf->bf_desc);
        }
        ath_hal_getdesclinkptr(ah, bf->bf_lastbf->bf_desc, &(txq->axq_link));
#ifdef DBG
        if (ath_hal_gettxbuf(ah, txq->axq_qnum) == 0)
        {
            /* This will cause an NMI since TXDP is 0 */
            printk("%s: FATAL ERROR: NULL TXDP while enabling TX.\n", __func__);
            ASSERT(FALSE);
        } else
#endif


		for(; tbf; tbf = TAILQ_NEXT(tbf, bf_list)) {		
			OS_SYNC_SINGLE(sc->sc_osdev, tbf->bf_daddr, 
							sc->sc_txdesclen, BUS_DMA_TODEVICE, NULL);
		}

        ath_hal_txstart(ah, txq->axq_qnum);
#if ATH_RESET_SERIAL
        atomic_dec(&sc->sc_tx_add_processing); 
#else
        ATH_RESET_UNLOCK(sc);
#endif
    }

#if ATH_TX_POLL
    txq->axq_lastq_tick = OS_GET_TICKS();
#endif
#if ATH_SUPPORT_FLOWMAC_MODULE
    if (sc->sc_osnetif_flowcntrl) {
        ath_netif_update_trans(sc);
    }
#endif
    //sc->sc_devstats.tx_packets++;
    //sc->sc_devstats.tx_bytes += framelen;
#ifdef ATH_SWRETRY
    /* paprd frame has NULL an, ignore it */
    if (an) {
        if (!bf->bf_isampdu && bf->bf_isdata) {
            ATH_NODE_SWRETRY_TXBUF_LOCK(an);
            if (txq == sc->sc_uapsdq)
                (an->an_swretry_info[txq->axq_qnum]).swr_num_eligible_frms += an->an_uapsd_num_addbuf;
            else
                (an->an_swretry_info[txq->axq_qnum]).swr_num_eligible_frms ++;
            ATH_NODE_SWRETRY_TXBUF_UNLOCK(an);
            DPRINTF(sc, ATH_DEBUG_SWR, "%s: FRMS INC dst=%s SeqCtrl=0x%02X%02X qnum=%d swr_num_eligible_frms=%d\n",
                    __func__, ether_sprintf(wh->i_addr1), wh->i_seq[0], wh->i_seq[1],
                    txq->axq_qnum, (an->an_swretry_info[txq->axq_qnum]).swr_num_eligible_frms);
        }
    }
#endif
    return 0;
#undef DESC2PA
}

/*
 * Get transmit rate index using rate in Kbps
 */
static INLINE int
ath_tx_findindex(const HAL_RATE_TABLE *rt, int rate)
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
 * Get transmit rate index from ieee rate 
 */
u_int8_t ath_rate_findrix(const HAL_RATE_TABLE *rt , u_int8_t ieee_rate)
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
 * Sets the min-rate for non-data packets or for data packets where
 * use min-rate is set (e.g. EAPOL packets)
 */
static INLINE void
ath_rate_set_minrate(struct ath_softc *sc, 
        ieee80211_tx_control_t *txctl, const HAL_RATE_TABLE *rt,
        struct ath_rc_series *rcs)
{ 
        if (txctl->min_rate != 0)
            rcs[0].rix = ath_rate_findrix(rt, txctl->min_rate);
        else
            rcs[0].rix = sc->sc_minrateix;
        rcs[0].tries = ATH_MGT_TXMAXTRY;
}

/*
 * This function will setup additional txctl information, mostly rate stuff
 */
static INLINE int
__ath_tx_prepare(struct ath_softc *sc, wbuf_t wbuf, ieee80211_tx_control_t *txctl)
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
    int subtype;

    txctl->dev = sc;

    wh = (struct ieee80211_frame *)wbuf_header(wbuf);

#ifdef ATH_SUPERG_XR
    if (ieee80211vap_has_flags(vap, IEEE80211_F_XR))
        rt = sc->sc_xr_rates;
    else
        rt = sc->sc_currates;
#else
    rt = sc->sc_currates;
#endif
    KASSERT(rt != NULL, ("no rate table, mode %u", sc->sc_curmode));

    an = txctl->an;
    txq = &sc->sc_txq[txctl->qnum];

    /*
     * Setup for rate calculations.
     */
    rcs = (struct ath_rc_series *)&txctl->priv[0];
    OS_MEMZERO(rcs, sizeof(struct ath_rc_series) * 4);

    if (txctl->isdata) {
        if (txctl->ismcast) {
            rcs[0].rix = (u_int8_t)ath_tx_findindex(rt, txctl->mcast_rate);

            /*
             * mcast packets are not re-tried.
             */
            rcs[0].tries = 1;
            ath_tx_seqno_set(an, WME_MGMT_TID, wbuf, 0, 0);
        } else {
            /*
             * For aggregation enabled nodes there is no need to do rate find
             * on each of these frames.
             */
            txctl->tidno = wbuf_get_tid(wbuf);
            if (
#ifdef ATH_RIFS
                !txctl->ht || (!sc->sc_txaggr && !sc->sc_txrifs) ||
#else
                !txctl->ht || !sc->sc_txaggr ||
#endif
                !ath_aggr_query( ATH_AN_2_TID(an,txctl->tidno))) {

                if (likely(!txctl->use_minrate)) {
#if !(ATH_SUPPORT_HT)
                    int isProbe;
                    /*
                     * Data frames; consult the rate control module.
                     */
                    sc->sc_log_rcfind = 1;

#if UMAC_SUPPORT_SMARTANTENNA                
                    if (wbuf_is_sa_train_packet(wbuf)) {
                        ath_rate_fixedrate(sc, an, txctl->shortPreamble,wbuf_sa_get_rateidx(wbuf),
                                  wbuf_sa_get_antenna(wbuf), ATH_RC_PROBE_ALLOWED,
                                  TID_TO_WME_AC(txctl->tidno),
                                  rcs, &isProbe, AH_FALSE,txctl->flags);
                    } else {
                        ath_rate_findrate(sc, an, txctl->shortPreamble, txctl->frmlen,
                                  ATH_11N_TXMAXTRY, ATH_RC_PROBE_ALLOWED,
                                  TID_TO_WME_AC(txctl->tidno),
                                  rcs, &isProbe, AH_FALSE,txctl->flags);
                    }
#else                
                    ath_rate_findrate(sc, an, txctl->shortPreamble, txctl->frmlen,
                              ATH_11N_TXMAXTRY, ATH_RC_PROBE_ALLOWED,
                              TID_TO_WME_AC(txctl->tidno),
                              rcs, &isProbe, AH_FALSE,txctl->flags);
#endif                

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
                    ath_rate_set_minrate(sc, txctl, rt, rcs);
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
                } else {
                    ath_tx_seqno_set(an, txctl->tidno, wbuf, txctl->isqosdata, 0);
                }
            } else {
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
                    ath_rate_set_minrate(sc, txctl, rt, rcs);
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

        /* set the seq number for mgmt frames only */
        ath_tx_seqno_set(an, txctl->tidno, wbuf, 0, !txctl->ismgmt);
        ath_rate_set_minrate(sc, txctl, rt, rcs);
#ifdef ATH_SUPPORT_TxBF
        { 
           /*
            * Force the Rate of Delay Report to be within 6 ~54 Mbps
            * Use the Rate less than one the sounding request used, so the report will be delivered reliably
            */
            if (txctl->isdelayrpt){
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
               
                rcs[0].rix = ath_rate_findrix(rt, map_rate[rate_index]);
                rcs[0].tries = 1;/*Retry not need*/
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
    if ((txctl->flags & HAL_TXDESC_NOACK) == 0 &&
        (wh->i_fc[0] & IEEE80211_FC0_TYPE_MASK) != IEEE80211_FC0_TYPE_CTL) {
        u_int16_t dur;
        /*
         * XXX not right with fragmentation.
         */
        if (txctl->shortPreamble)
            dur = rt->info[rix].spAckDuration;
        else
            dur = rt->info[rix].lpAckDuration;

        if (wh->i_fc[1] & IEEE80211_FC1_MORE_FRAG) {
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

        if (txctl->istxfrag) {
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

#ifdef ATH_SUPERG_COMP
    if (ATH_NODE(ni)->an_decomp_index != INVALID_DECOMP_INDEX && 
        !ismcast &&
        ((wh->i_fc[0] & IEEE80211_FC0_TYPE_MASK) == IEEE80211_FC0_TYPE_DATA)
        && ((wh->i_fc[0] & IEEE80211_FC0_SUBTYPE_MASK) 
            != IEEE80211_FC0_SUBTYPE_NODATA)) {
        if (txctl->frmlen > ATH_COMP_THRESHOLD) {
            comp = ATH_COMP_PROC_COMP_OPTIMAL;
        } else {
            comp = ATH_COMP_PROC_NO_COMP_ADD_CCS;
        }
    } 
#endif

#ifdef USE_LEGACY_HAL
    if (txctl->ismcast) 
        antenna = sc->sc_mcastantenna + 1;
    else
        antenna = sc->sc_txantenna;

    txctl->antenna = antenna;
    txctl->compression = comp;
#endif
    if (txctl->ismcast)
        sc->sc_mcastantenna = (sc->sc_mcastantenna + 1) & 0x1;

    /* Allow modifying destination mask only if ATH_SWRETRY_MODIFY_DSTMASK
     * is enabled. This will enable a HW optimization to filter out pkts
     * to particular destination after HW retry-failure.
     */
#if defined(ATH_SWRETRY) && defined(ATH_SWRETRY_MODIFY_DSTMASK)
    /* Management frames will not go for SW Retry
     * process unless they are failed with filtered
     * error.
     */
    if (!(an) || (an && !(an->an_swrenabled))) {
        txctl->flags |= HAL_TXDESC_CLRDMASK;
    } else {
        ATH_TXQ_LOCK(txq);
        if (txq->axq_destmask) {
            txctl->flags |= HAL_TXDESC_CLRDMASK;
            if (txctl->isdata) {
                txq->axq_destmask = AH_FALSE; /*Turn-off destmask only for subsequent data frames*/
            }   
        }
        ATH_TXQ_UNLOCK(txq);
    }
#endif
    
    /* report LED module about byte count */
    subtype = wh->i_fc[0] & IEEE80211_FC0_SUBTYPE_MASK;
#if ATH_SUPPORT_LED
    if (txctl->isdata &&
        subtype != IEEE80211_FC0_SUBTYPE_NODATA &&
        subtype != IEEE80211_FC0_SUBTYPE_QOS_NULL)
        ath_led_report_data_flow(&sc->sc_led_control, txctl->frmlen);
#endif
   
    /*
     * XXX: Update some stats ???
     */
    if (txctl->shortPreamble)
        sc->sc_stats.ast_tx_shortpre++;
    if (txctl->flags & HAL_TXDESC_NOACK)
        sc->sc_stats.ast_tx_noack++;
    
    return 0;
}
#ifndef ATH_SUPPORT_HTC
int
ath_tx_start(ath_dev_t dev, wbuf_t wbuf, ieee80211_tx_control_t *txctl)
{
    struct ath_softc *sc = ATH_DEV_TO_SC(dev);
    int err;


    err = __ath_tx_prepare(sc, wbuf, txctl);
    if (err)
        return err;

    /*
     * Start DMA mapping.
     *
     * ath_tx_start_dma() will be called either synchronously or
     * asynchrounsly once DMA is complete.
     */
    err = wbuf_map_sg(sc->sc_osdev, wbuf,
                      OS_GET_DMA_MEM_CONTEXT(txctl, dmacontext), txctl);
    if (err) {
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

    /* failed packets will be dropped by the caller */
    return err;
}
#endif





#if ATH_TX_BUF_FLOW_CNTL
#if ATH_SUPPORT_VOWEXT
static ath_get_buf_status_t
ath_tx_get_buf(struct ath_softc *sc, sg_t *sg, struct ath_buf **pbf,
               ath_bufhead *bf_head, int ac, u_int32_t *buf_used)
#else
static ath_get_buf_status_t
ath_tx_get_buf(struct ath_softc *sc, sg_t *sg, struct ath_buf **pbf,
               ath_bufhead *bf_head, u_int32_t *buf_used)
#endif
#else
#if ATH_SUPPORT_VOWEXT
static ath_get_buf_status_t
ath_tx_get_buf(struct ath_softc *sc, sg_t *sg, struct ath_buf **pbf,
               ath_bufhead *bf_head, int ac)
#else
static ath_get_buf_status_t
ath_tx_get_buf(struct ath_softc *sc, sg_t *sg, struct ath_buf **pbf,
               ath_bufhead *bf_head)
#endif
#endif
{
    struct ath_buf *bf = *pbf;

    if (!bf || !bf->bf_avail_buf) {
        ATH_TXBUF_LOCK(sc);
        bf = TAILQ_FIRST(&sc->sc_txbuf);
#if ATH_SUPPORT_VOWEXT
        if (bf == NULL)  {
            if ( (ATH_IS_VOWEXT_FAIRQUEUE_ENABLED(sc)) &&
                (ac == WME_AC_VI)) {
                
                if (ath_deq_single_buf(sc,
                            &sc->sc_txq[sc->sc_haltype2q[HAL_WME_AC_BK]]))
                {
                    if (ath_deq_single_buf(sc,
                                &sc->sc_txq[sc->sc_haltype2q[HAL_WME_AC_BE]]))
                    {
                        ATH_TXBUF_UNLOCK(sc);
                        return ATH_BUF_NONE;
                    } else {
                        bf = TAILQ_FIRST(&sc->sc_txbuf);
                        sc->sc_stats.ast_vow_ath_bk_drop++;
                    }
                } else {
                    bf = TAILQ_FIRST(&sc->sc_txbuf);
                    sc->sc_stats.ast_vow_ath_be_drop++;
                }
            } else {
                /* fall through the original code*/
                ATH_TXBUF_UNLOCK(sc);
                return ATH_BUF_NONE;
            }
        }
#else
        if (bf == NULL) {
            ATH_TXBUF_UNLOCK(sc);
            return ATH_BUF_NONE;
        }
#endif
        *pbf = bf;
        TAILQ_REMOVE(&sc->sc_txbuf, bf, bf_list);
#if ATH_TX_BUF_FLOW_CNTL
        sc->sc_txbuf_free--;
		(*buf_used)++;
#endif
        ATH_TXBUF_UNLOCK(sc);
        TAILQ_INSERT_TAIL(bf_head, bf, bf_list);

        /* set up this buffer */
        ATH_TXBUF_RESET(bf, sc->sc_num_txmaps);
#ifdef ATH_SWRETRY
        ATH_TXBUF_SWRETRY_RESET(bf);
#endif
    }

    bf->bf_buf_addr[sc->sc_num_txmaps - bf->bf_avail_buf] = sg_dma_address(sg);
    bf->bf_buf_len[sc->sc_num_txmaps - bf->bf_avail_buf] = sg_dma_len(sg);

    bf->bf_avail_buf--;

    if (bf->bf_avail_buf)
        return ATH_BUF_CONT;

    return ATH_BUF_LAST;
}



#if ATH_SUPPORT_VOWEXT
static ath_get_buf_status_t
ath_tx_get_vibuf (struct ath_softc *sc, sg_t * sg, struct ath_buf **pbf,
		  ath_bufhead * bf_head, ieee80211_tx_control_t * txctl,
          struct ath_txq *txq, u_int32_t *buf_used)
{
    struct ath_buf *bf = NULL;
    struct ath_atx_tid *tid = NULL;
    struct ath_atx_ac *ac = NULL;
    struct ath_node *an = (struct ath_node *)(txctl->an);
    struct ath_atx_tid *targettid = NULL;
    struct ath_atx_tid *targettid_gput = NULL;
    struct ath_atx_tid *targettid_aid = NULL;
    uint16_t temp_aid;
    uint16_t maxaid = sc->sc_ieee_ops->get_aid(an->an_node);    
    uint32_t mingput = an->an_rc_node->txRateCtrlViVo.rptGoodput;
    uint32_t maxgput = mingput;

	sc->vsp_prevevaltime = OS_GET_TICKS();

    ATH_TXQ_LOCK (txq);

    /* Loop through each destination AC (ac-node pair) and try to find the candidate stream either goodput based or aid based*/
    TAILQ_FOREACH(ac, &txq->axq_acq, ac_qelem) {
       TAILQ_FOREACH(tid, &ac->tid_q, tid_qelem) {
           bf = TAILQ_LAST (&tid->buf_q, ath_bufhead_s);
           if (!bf || bf->bf_isretried) {
	           continue;	/* can't remove bf from this tid */
	        }
            else
	        {
			    /* Try to identify the stream with min gput, max gput & last associated */
                if(tid->an->an_rc_node->txRateCtrlViVo.rptGoodput < mingput)
	            {
		            mingput = tid->an->an_rc_node->txRateCtrlViVo.rptGoodput;
    				targettid_gput = tid;
	            }
    	        else
	            {
                    if(tid->an->an_rc_node->txRateCtrlViVo.rptGoodput > maxgput)
			    	{
				        maxgput = tid->an->an_rc_node->txRateCtrlViVo.rptGoodput;
    				}
	    		}

                temp_aid = sc->sc_ieee_ops->get_aid(((struct ath_node *)(tid->an))->an_node);
                if(temp_aid > maxaid)
			    {
                    maxaid = temp_aid;
    				targettid_aid = tid;
			    }
    			break;
	    	}
	    }
	}

    /* Derive the target tid */
    if ((maxgput - mingput) > sc->vsp_threshold)
    {
        if (targettid_gput)
		{
		    /* we found suitable candidate stream thru gput */
			targettid = targettid_gput;

			targettid->ac->max_sch_penality = VSP_MAX_SCH_PENALITY;	
		}
		else
		{
		    /* current tid stream is the candidate stream based on gput - So mark this as candidate stream and impose the penality */
            (&an->an_tx_tid[txctl->tidno])->ac->max_sch_penality = VSP_MAX_SCH_PENALITY;	
		}
	}
    else
    {
		/* All streams are good (at same level) so choose the candidate stream based on aid */
		if (targettid_aid)
		{
			targettid = targettid_aid;
            targettid->ac->max_sch_penality = VSP_MAX_SCH_PENALITY;
		}
		else
		{
			  /* current tid stream is the candidate stream based on aid - So mark this as candidate stream and impose the penality */
            (&an->an_tx_tid[txctl->tidno])->ac->max_sch_penality = VSP_MAX_SCH_PENALITY;
		}
    }
    
    if (targettid)
    {
        ath_bufhead bf_head_temp;

		/* Get the target ac & target buffer to release */
        bf = TAILQ_LAST (&targettid->buf_q, ath_bufhead_s);
		ac = targettid->ac;

        sc->vsp_vipendrop++;

        /* Remove the buffer from this tid */
        TAILQ_REMOVE (&targettid->buf_q, bf, bf_list);

        /* Update the next seq no for this tis as we recliamed the buf */
        if (targettid->seq_next == 0)
        	targettid->seq_next = IEEE80211_SEQ_MAX - 1;
        else
        	targettid->seq_next--;

        if (TAILQ_EMPTY (&targettid->buf_q))
	    {
            /* No buffers associated to this tid. 
               So remove this tid from tid_q and mark sched as FALSE */
    	    targettid->sched = AH_FALSE;

            TAILQ_REMOVE (&ac->tid_q, targettid, tid_qelem);
		    if (TAILQ_EMPTY (&ac->tid_q))
	        {
                /* No tds from this ac for scheduling. 
                   So, remove this ac from axq_acq and mark sched as FALSE */
	            ac->sched = AH_FALSE;
	            TAILQ_REMOVE (&txq->axq_acq, ac, ac_qelem);
	        }
	    }

        ATH_TXQ_UNLOCK (txq);

        TAILQ_INIT (&bf_head_temp);
        TAILQ_INSERT_TAIL (&bf_head_temp, bf, bf_list);

        /* Increment the no buf stats */
        /* we do not need to stop the netif queue here, as it is trying
         * to pull the buffer from bad stream probably. 
         */

        sc->sc_stats.ast_tx_nobuf++;
        sc->sc_stats.ast_txq_nobuf[txctl->qnum]++;


#ifdef ATH_SUPPORT_TxBF
        ath_tx_complete_buf (sc, bf, &bf_head_temp, 0, 0, 0);
#else        
        ath_tx_complete_buf (sc, bf, &bf_head_temp, 0);
#endif

        /* update statistics */
        ATH_TXBUF_LOCK (sc);

		/* Get free bf from buf pool */
        bf = TAILQ_FIRST (&sc->sc_txbuf);

        if (bf == NULL)
	    {
	        ATH_TXBUF_UNLOCK (sc);
			return ATH_BUF_NONE;
	    }

        *pbf = bf;

        TAILQ_REMOVE (&sc->sc_txbuf, bf, bf_list);

#if ATH_TX_BUF_FLOW_CNTL        
         sc->sc_txbuf_free--;
		(*buf_used)++;
#endif

        ATH_TXBUF_UNLOCK (sc);
        TAILQ_INSERT_TAIL (bf_head, bf, bf_list);

        /* set up this buffer */
        ATH_TXBUF_RESET (bf, sc->sc_num_txmaps);
#if ATH_SWRETRY
        ATH_TXBUF_SWRETRY_RESET (bf);
#endif

        bf->bf_buf_addr[sc->sc_num_txmaps - bf->bf_avail_buf] = sg_dma_address(sg);
        bf->bf_buf_len[sc->sc_num_txmaps - bf->bf_avail_buf] = sg_dma_len (sg);

        bf->bf_avail_buf--;

        if (bf->bf_avail_buf)
        {
	        return ATH_BUF_CONT;
        }

        return ATH_BUF_LAST;
    }
    else
    {
        ATH_TXQ_UNLOCK (txq);
        return ATH_BUF_NONE;
    }
}
#endif


/*
 * A platform-specific value for ATH_TXBUF may have already been defined
 * in osdep.h.  If so, use the platform-specific value.  Provide a default
 * value here for platforms that don't need a customized value of ATH_TXBUF.
 */
#ifndef ATH_TXBUF
#define ATH_TXBUF   512
#endif

#define MIN_BUF_RESV 16     /* Minimum buffers reserved per AC. This is to
                             * provide some breathing space for low priority
                             * traffic when high priority traffic is flooding */

/*
 * The function that actually starts the DMA.
 * It will either be called by the wbuf_map() function,
 * or called in a different thread if asynchronus DMA
 * mapping is used (NDIS 6.0).
 */
int
ath_tx_start_dma(wbuf_t wbuf, sg_t *sg, u_int32_t n_sg, void *arg)
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
#if defined(VOW_TIDSCHED) || ATH_SUPPORT_IQUE_EXT || defined(VOW_LOGLATENCY)
    int readtsf = 0;
#endif

#if defined(ATH_SUPPORT_VOWEXT) || ATH_SUPPORT_IQUE_EXT || defined(VOW_LOGLATENCY)
    u_int32_t ac_stats[HAL_NUM_TX_QUEUES] = {0, 0, 0, 0, 0, 0, 0}; 
    uint8_t vicontention = 0;
    uint8_t vsprequired = 0;
#endif

#if ATH_TX_BUF_FLOW_CNTL
    u_int32_t *buf_used;
    buf_used = &txq->axq_num_buf_used;
#endif

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
    atomic_inc(&an->an_active_tx_cnt);
    if (an->an_flags & ATH_NODE_CLEAN) {
        atomic_dec(&an->an_active_tx_cnt);
        return -EIO;
    }

    if (txctl->ismcast) {
        /*
         * When servicing one or more stations in power-save mode (or)
         * if there is some mcast data waiting on mcast queue
         * (to prevent out of order delivery of mcast,bcast packets)
         * multicast frames must be buffered until after the beacon.
         * We use the private mcast queue for that.
         */
        if (txctl->ps || avp->av_mcastq.axq_depth) {
            send_to_cabq = 1;
#if ATH_TX_BUF_FLOW_CNTL
            buf_used = &sc->sc_cabq->axq_num_buf_used;
#endif
        }
    }

#if ATH_TX_BUF_FLOW_CNTL
#if ATH_SUPPORT_VOWEXT
    if ((txctl->qnum == WME_AC_VI) && sc->vsp_enable)
    {
        /* VSP is applicable only for vi streams and if vsp is enabled */
        vsprequired = 1;
    }
#endif
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
#if ATH_SUPPORT_VOWEXT
        if (vsprequired) {
            /* Don't drop the packet. Try to get buf from bad video streams */
            vicontention = 1;
            sc->vsp_vicontention++;

        } else
#endif
        { /* *WARN* do not remove this '{', this is else part of above */

            //printk("%s: Drop buf_used = %d, txbuf_free = %d, minfree = %d, qnum = %d, totalqueued = %d, depth = %d\n",
            //       __func__, *buf_used, sc->sc_txbuf_free, txq->axq_minfree, txctl->qnum, txq->axq_totalqueued, txq->axq_depth);

#if ATH_SUPPORT_FLOWMAC_MODULE
            /* let us get the flowcontrol support for only video traffic,
             * rest of the traffic would continue to work in old way. If
             * there is a mixed traffic, and there is no buffer for Video,
             * rest of the traffic also would stall.
             *
             * This path is covered if
             *  - VSP is disabled and Video traffic is coming through here
             *  - Irrespective of VSP and traffic is not Video traffic
             *
             */
            if (!sc->sc_osnetif_flowcntrl || (txctl->qnum != WME_AC_VI)) {
                sc->sc_stats.ast_tx_nobuf++;
                sc->sc_stats.ast_txq_nobuf[txctl->qnum]++;
                atomic_dec(&an->an_active_tx_cnt);
                return -ENOMEM;
            } else {
                /* inform kernel to stop sending the frames down to ath
                 * layer and try to send this frame alone.
                 */
                if (sc->sc_osnetif_flowcntrl) {
                    ath_netif_stop_queue(sc);
                }
            }
#else
            sc->sc_stats.ast_tx_nobuf++;
            sc->sc_stats.ast_txq_nobuf[txctl->qnum]++;
            atomic_dec(&an->an_active_tx_cnt);
            return -ENOMEM;
#endif
        }
    }
#if ATH_SUPPORT_VOWEXT
    else
    {
        if (vsprequired)
	    {
            if(txq->axq_num_buf_used < (ATH_TXBUF >> 1))
			{
                /* Case - Buffer usage is in Safe zone (0-50%) so remove the
                 * sch penality */
                (&an->an_tx_tid[txctl->tidno])->ac->max_sch_penality = 0;
			}
			else
			{
                if((txq->axq_num_buf_used << 2) > (3 * ATH_TXBUF))
                {
				    /* Case - Buffer usage is in problem zone [>75%] */
					int currenttime = OS_GET_TICKS();

                    /* Run vsp algo if elapsed time is greater than the
                     * configured value  no need to bother about the wrap
                     * around cases */
                    if(CONVERT_SYSTEM_TIME_TO_MS(ATH_DIFF(sc->vsp_prevevaltime,
                                    currenttime))  > sc->vsp_evalinterval)
					{
					    int mingput = 0, maxgput = 0;
                        struct ath_atx_ac *ac = NULL;
                        struct ath_atx_tid *targettid = NULL;
                        struct ath_atx_tid *tid = NULL;

                        /* loop through all ACs and find the candidate node
                         * (goodput based) and impose scheduling penality
                         */
                        ATH_TXQ_LOCK(txq);

						TAILQ_FOREACH(ac, &txq->axq_acq, ac_qelem) {
                            /* Reset the vsp parameters */
                            ac->max_sch_penality = 0;
                            ac->sch_penality_cnt = 0;

							tid = TAILQ_FIRST(&ac->tid_q);
							if (tid)
							{
							    if(!mingput)
								{
								    mingput = maxgput = tid->an->an_rc_node->txRateCtrlViVo.rptGoodput;
                                    targettid = tid;
								}
								else
								{
								    if(tid->an->an_rc_node->txRateCtrlViVo.rptGoodput < mingput)
									{
									    mingput = tid->an->an_rc_node->txRateCtrlViVo.rptGoodput;
										targettid = tid;
									}
									else
									{
									    if(tid->an->an_rc_node->txRateCtrlViVo.rptGoodput > maxgput)
										{
										    maxgput = tid->an->an_rc_node->txRateCtrlViVo.rptGoodput;
										}
									}
								}
							}
						}
						   
						if ((maxgput - mingput) > sc->vsp_threshold)
						{
						    /* Penalize this node */
							targettid->ac->max_sch_penality = VSP_MAX_SCH_PENALITY;
						}
                            
                        ATH_TXQ_UNLOCK(txq);

						/* Reset the eval time */
						sc->vsp_prevevaltime = currenttime;
				    }
			    }
			    else
				{
				    /* Case - Buffer usage is Caution Zone (50% - 75%) */
					/* Do Nothing for now */
				}
			}
	    }
    }
#endif
#endif

#if ATH_SUPPORT_VOWEXT
    /* update vow statistics */
    sc->sc_stats.ast_vow_ul_tx_calls[txctl->qnum]++;

#endif
    /* For each sglist entry, allocate an ath_buf for DMA */
    TAILQ_INIT(&bf_head);
    for (i = 0; i < n_sg; i++, sg++) {
        int more_maps;
        ath_get_buf_status_t retval;

        more_maps = (n_sg - i) > 1;

#if ATH_TX_BUF_FLOW_CNTL
#if ATH_SUPPORT_VOWEXT
      /* If it is contention among video streams then no need to look for
       * other queues for buffers Else look for buffer in the legacy way -
       * try to get buffer from global pool if not avilable then try fo grab
       * buffer from BE/BK queued packets
       */
        if (!vicontention)
	    {
            retval = ath_tx_get_buf(sc, sg, &bf, &bf_head,
                    txctl->qnum, buf_used);
        }
        else
        {
            u_int8_t stall_queue = 0;

            /* try to grab the buffer from be/bk ac's first if not available
             * then try to grab the buffer from vi stream
             */
            if(ATH_IS_VOWEXT_FAIRQUEUE_ENABLED(sc)) {

                ATH_TXBUF_LOCK(sc);

                /* do not stall the queues here, Video still can run as
                 * long as we can de queue the frames from BE/BK.
                 */
                if (ath_deq_single_buf(sc,
                            &sc->sc_txq[sc->sc_haltype2q[HAL_WME_AC_BK]]))
                {
                    /* no buf is available from BK ac, so try BE ac */
                    if (ath_deq_single_buf(sc,
                                &sc->sc_txq[sc->sc_haltype2q[HAL_WME_AC_BE]]))
                    {
                        /* no buf is available from BE ac also, so try to
                         * get buf from vi stream
                         */
                        ATH_TXBUF_UNLOCK(sc);
                        retval = ath_tx_get_vibuf(sc, sg, &bf, &bf_head,
                                txctl, txq, buf_used);
                        stall_queue = 1;
                    }
                    else
                    {
                        /* successfully dequeud one buf so get buf from buf
                         * pool
                         */
                        sc->vsp_bkpendrop++;
                        ATH_TXBUF_UNLOCK(sc);
                        retval = ath_tx_get_buf(sc, sg, &bf, &bf_head,
                                txctl->qnum, buf_used);
                        /* do we really require to stop queue here ?.  What
                         * if I have overwhelming BK/BE and less VI. If we
                         * stop the queue here, we might end-up filling up
                         * the backlog queues with BE data and VI might
                         * loose, I think, it is worth leaving out the BE to
                         * leak from top until video hits no-buffer, then
                         * let BE leak from bottom.
                         * If both present, WMM and aggregate scheduling
                         * shall give some fairness to Video.
                         * */
                        stall_queue = 0;
                    }
                }
                else
                {
                    /* successfully dequeud one buf so get buf from buf 
                     * pool
                     */
                    sc->vsp_bependrop++;
                    ATH_TXBUF_UNLOCK(sc);
                    retval = ath_tx_get_buf(sc, sg, &bf, &bf_head,
                                    txctl->qnum, buf_used);
                    /* see above comment */
                    stall_queue = 0;
                }
            }
            else
            {
                retval = ath_tx_get_vibuf(sc, sg, &bf, &bf_head, txctl,
                                txq, buf_used);
                stall_queue = 1;
            }
#if ATH_SUPPORT_FLOWMAC_MODULE
            /*
             * Either the fair queue is not enabled OR
             * If enabled, there is a case of video trying to grab the
             * buffers of video. Stall the queues and enable the queueing
             * for the traffic.
             * In case if we stall the traffic if BE frame can be grabbed,
             * there could be chance of video getting stalled becuase there
             * is much BE. So just do only when VI contends itself. Let BE
             * and BK progress/leak as much as they can.
             * 
             */
            /* This path is reached only when the traffic is video traffic
             * and there is video cotention
             */
            if (stall_queue && sc->sc_osnetif_flowcntrl) {
                ath_netif_stop_queue(sc);
            }
#endif
        }
#else
        retval = ath_tx_get_buf(sc, sg, &bf, &bf_head, buf_used);
#endif
#else
#if ATH_SUPPORT_VOWEXT
        retval = ath_tx_get_buf(sc, sg, &bf, &bf_head, txctl->qnum);
#else
        retval = ath_tx_get_buf(sc, sg, &bf, &bf_head);
#endif
#endif
        if (more_maps && (ATH_BUF_CONT == retval)) {
            continue;
        } else if (ATH_BUF_NONE == retval) {
            DPRINTF(sc, ATH_DEBUG_XMIT, 
                    "%s no more ath bufs. num phys frags %d \n",
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

#if defined(ATH_SUPPORT_VOWEXT) || ATH_SUPPORT_IQUE_EXT || defined(VOW_LOGLATENCY)
        if (txctl->qnum < sizeof(ac_stats)/sizeof(ac_stats[0])) {
            ac_stats[txctl->qnum]++;
        }
        wbuf_set_firstxmit(bf->bf_mpdu, 1);
#endif

#if defined(VOW_TIDSCHED)
        if(sc->tidsched) {
          readtsf = 1;
        }
#endif

#if ATH_SUPPORT_IQUE_EXT
        if(sc->sc_retry_duration || sc->total_delay_timeout) {
          readtsf = 1;
        }
#endif

#if defined(VOW_LOGLATENCY)
        if(sc->loglatency) {
          readtsf = 1;
        }
#endif

#if defined(VOW_TIDSCHED) || ATH_SUPPORT_IQUE_EXT || defined(VOW_LOGLATENCY)
        if(readtsf) {
          wbuf_set_qin_timestamp(bf->bf_mpdu,  ath_hal_gettsf32(sc->sc_ah));
        }
        else
        {
          wbuf_set_qin_timestamp(bf->bf_mpdu,  0);
        }
#endif

#if ATH_SUPPORT_IQUE && ATH_SUPPORT_IQUE_EXT
        if(!bf->bf_isretried) {
            bf->bf_txduration = 0;
        }
#endif
        /* setup descriptor */
        ds = bf->bf_desc;
        ath_hal_setdesclink(ah, ds, 0);
#ifndef REMOVE_PKT_LOG
        bf->bf_vdata = wbuf_header(wbuf);
#endif
        ASSERT(sc->sc_num_txmaps);

        if (0 == (i/sc->sc_num_txmaps)) {

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
                        /* last segment */
                       , (n_sg <= sc->sc_num_txmaps) ? AH_TRUE : AH_FALSE 
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
#ifdef ATH_SUPPORT_TxBF
#ifdef TXBF_TODO
		/* fill the Tx Descriptor of BF */
		{
			struct ieee80211_qosframe_htc *wh1;

			wh1 = (struct ieee80211_qosframe_htc *)wbuf_header(bf->bf_mpdu);
			if (wh1->i_fc[1] & IEEE80211_FC1_ORDER) {	
                /* contain +HTC.*/	
				if ((wh1->i_fc[0] & IEEE80211_FC0_SUBTYPE_MASK) == 
                    IEEE80211_FC0_SUBTYPE_QOS_NULL) {             
				    if ((wh1->i_htc[2] & IEEE80211_HTC2_CalPos) == 0x01) {
                       //cal 1
					   DPRINTF(sc, ATH_DEBUG_ANY,"CAL1==========Normaltxctl->flags(%x)\n", txctl->flags);
					   /* set burst*/
					   ath_hal_set11nTxBFCal(ah, ds,0x01,0,0x2,HAL_TXDESC_STAG_SOUND);				
				    }
				    else if ((wh1->i_htc[2] & IEEE80211_HTC2_CalPos) == 0x03) {
                       //cal 3
					   DPRINTF(sc, ATH_DEBUG_ANY,"CAL3==========Normal txctl->flags(%x)\n",txctl->flags);
					   ath_hal_set11nTxBFCal(ah, ds,0x03,0,0x2,HAL_TXDESC_STAG_SOUND);
				    }
				}
			}
		}
#endif
#endif 				
        /* NB: The desc swap function becomes void,
         * if descriptor swapping is not enabled
         */
        ath_desc_swap(ds);

        lastds = ds;
    }

    if (firstbf) {
        struct ath_atx_tid *tid = ATH_AN_2_TID(an, txctl->tidno);

#ifdef ATH_C3_WAR
        if (sc->sc_reg_parm.c3WarTimerPeriod && bf->bf_isdata) {
            atomic_inc(&sc->sc_pending_tx_data_frames);
        }
#endif

        firstbf->bf_lastfrm = bf;
        firstbf->bf_ht = txctl->ht;

#ifdef ATH_SUPPORT_UAPSD
        if (txctl->isuapsd) {
            ath_tx_queue_uapsd(sc, txq, &bf_head, txctl);
#if ATH_SUPPORT_VOWEXT
            goto good;
#else
            atomic_dec(&an->an_active_tx_cnt);
            return 0;
#endif
        }
#endif
        ath_vap_pause_txq_use_inc(sc);
        ATH_TXQ_LOCK(txq);

#ifdef ATH_RIFS
        if (txctl->ht && (sc->sc_txaggr || sc->sc_txrifs))
#else
        if (txctl->ht && sc->sc_txaggr)
#endif
        {
            if (ath_aggr_query(tid)) {
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
                     DPRINTF(sc, ATH_DEBUG_XMIT, 
                             "%s txq->axq_depth %d ATH_SINGLES_MIN_QDEPTH %d\n",
                             __func__, txq->axq_depth, ATH_SINGLES_MIN_QDEPTH);
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
            } else {
#ifdef ATH_TX_BUF_FLOW_CNTL
                /* reserving minimum buffer for unicast packets */
                if (sc->sc_txbuf_free < MCAST_MIN_FREEBUF) {
                    ATH_TXQ_UNLOCK(txq);
                    goto bad;
                }
#endif
                atomic_inc(&avp->av_beacon_cabq_use_cnt);
                if (atomic_read(&avp->av_stop_beacon) ||
                    avp->av_bcbuf == NULL) {
                    ATH_TXQ_UNLOCK(txq);
                    ath_vap_pause_txq_use_dec(sc);
                    atomic_dec(&avp->av_beacon_cabq_use_cnt);
                    goto bad;
                }

                ath_buf_set_rate(sc, firstbf);

#ifdef ATH_SWRETRY
                /*
                 * clear the dest mask if this is the first frame scheduled after all swr eligible
                 * frames have been popped out from txq
                */
                if (sc->sc_enhanceddmasupport) {
                    if ((an->an_swretry_info[sc->sc_cabq->axq_qnum]).swr_state_filtering &&
                        !(an->an_swretry_info[sc->sc_cabq->axq_qnum]).swr_num_eligible_frms) {
                        DPRINTF(sc, ATH_DEBUG_SWR, "%s: clear dest mask\n", __func__);
                        ATH_NODE_SWRETRY_TXBUF_LOCK(an);
#if ATH_SWRETRY_MODIFY_DSTMASK
                        (an->an_swretry_info[sc->sc_cabq->axq_qnum]).swr_need_cleardest = AH_TRUE;
                        ath_tx_modify_cleardestmask(sc, sc->sc_cabq, &bf_head);
#endif
                        (an->an_swretry_info[sc->sc_cabq->axq_qnum]).swr_state_filtering = AH_FALSE;
                        ATH_NODE_SWRETRY_TXBUF_UNLOCK(an);
                    }
                }
#endif

                ATH_TXQ_LOCK(&avp->av_mcastq);
                ath_tx_mcastqaddbuf_internal(sc, &avp->av_mcastq, &bf_head);
                ATH_TXQ_UNLOCK(&avp->av_mcastq);
                atomic_dec(&avp->av_beacon_cabq_use_cnt);

#ifdef ATH_SWRETRY
                if (!firstbf->bf_isampdu && firstbf->bf_isdata) {
                    struct ieee80211_frame * wh;
                    wh = (struct ieee80211_frame *)wbuf_header(firstbf->bf_mpdu);
                    ATH_NODE_SWRETRY_TXBUF_LOCK(an);
                    (an->an_swretry_info[sc->sc_cabq->axq_qnum]).swr_num_eligible_frms ++;
                    ATH_NODE_SWRETRY_TXBUF_UNLOCK(an);
                    DPRINTF(sc, ATH_DEBUG_SWR, "%s: dst=%s SeqCtrl=0x%02X%02X qnum=%d swr_num_eligible_frms=%d\n",
                            __func__, ether_sprintf(wh->i_addr1), wh->i_seq[0], wh->i_seq[1], sc->sc_cabq->axq_qnum,
                            (an->an_swretry_info[sc->sc_cabq->axq_qnum]).swr_num_eligible_frms);
                }
#endif

            }
        }

        ATH_TXQ_UNLOCK(txq);
        ath_vap_pause_txq_use_dec(sc);
#if ATH_SUPPORT_VOWEXT
        goto good;
#else
        atomic_dec(&an->an_active_tx_cnt);
        return 0;
#endif
    }
        
#if ATH_SUPPORT_VOWEXT
good:
    sc->sc_stats.ast_vow_ath_txq_calls[0] += ac_stats[0];
    sc->sc_stats.ast_vow_ath_txq_calls[1] += ac_stats[1];
    sc->sc_stats.ast_vow_ath_txq_calls[2] += ac_stats[2];
    sc->sc_stats.ast_vow_ath_txq_calls[3] += ac_stats[3];
    atomic_dec(&an->an_active_tx_cnt);
    return 0;
#endif 
bad:
    /*
     * XXX: In other OS's, we can probably drop the frame. But in de-serialized
     * windows driver (NDIS6.0), we're not allowd to tail drop frame when out
     * of resources. So we just return NOMEM here and let OS shim to do whatever
     * OS wants.
     */
    ATH_TXBUF_LOCK(sc);

#if ATH_TX_BUF_FLOW_CNTL
    if(!TAILQ_EMPTY(&bf_head)) {
        int num_buf = 0;
        ATH_NUM_BUF_IN_Q(&num_buf, &bf_head);
        (*buf_used)-= num_buf;
		sc->sc_txbuf_free += num_buf;
        TAILQ_CONCAT(&sc->sc_txbuf, &bf_head, bf_list);
    }
#else
    TAILQ_CONCAT(&sc->sc_txbuf, &bf_head, bf_list);
#endif

    ATH_TXBUF_UNLOCK(sc);

    sc->sc_stats.ast_tx_nobuf++;
    sc->sc_stats.ast_txq_nobuf[txctl->qnum]++;

    atomic_dec(&an->an_active_tx_cnt);

    return -ENOMEM;
}

/*
 * To complete a chain of buffers associated a frame
 */
#ifdef ATH_SUPPORT_TxBF
void
ath_tx_complete_buf(struct ath_softc *sc, struct ath_buf *bf, ath_bufhead *bf_q, int txok, u_int8_t txbf_status, u_int32_t tstamp)
#else
void
ath_tx_complete_buf(struct ath_softc *sc, struct ath_buf *bf, ath_bufhead *bf_q, int txok)
#endif
{
    wbuf_t wbuf = bf->bf_mpdu;
    ieee80211_tx_status_t tx_status;
#if ATH_TX_BUF_FLOW_CNTL
    struct ath_txq *txq = &sc->sc_txq[bf->bf_qnum];
#endif

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
    tx_status.tstamp = tstamp;
#endif

#ifdef ATH_C3_WAR
    if (sc->sc_reg_parm.c3WarTimerPeriod && bf->bf_isdata) {
        atomic_dec(&sc->sc_pending_tx_data_frames);

        if (!atomic_read(&sc->sc_pending_tx_data_frames)) {
            spin_lock(&sc->sc_c3_war_lock);
            if (sc->c3_war_timer_active) {
                ath_gen_timer_stop(sc, sc->sc_c3_war_timer);
                sc->c3_war_timer_active = 0;
                printk("%s: sc_c3_war_timer is stopped!!\n", __func__);
            }
            spin_unlock(&sc->sc_c3_war_lock);
        }
    }
#endif

#ifdef ATH_SWRETRY
    if (bf->bf_isswretry)
        tx_status.retries = bf->bf_totaltries;
    else
        tx_status.retries = bf->bf_retries;
#else
    tx_status.retries = bf->bf_retries;
#endif
    tx_status.flags = 0;

    if (bf->bf_state.bfs_ispaprd) {
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
#if ATH_TX_BUF_FLOW_CNTL
    if(!TAILQ_EMPTY(bf_q)) {
        int num_buf = 0;
        ATH_NUM_BUF_IN_Q(&num_buf, bf_q);
        txq->axq_num_buf_used -= num_buf;
		sc->sc_txbuf_free += num_buf;
        TAILQ_CONCAT(&sc->sc_txbuf, bf_q, bf_list);
    }
#else
    TAILQ_CONCAT(&sc->sc_txbuf, bf_q, bf_list);
#endif
    ATH_TXBUF_UNLOCK(sc);
#if ATH_SUPPORT_FLOWMAC_MODULE
    if (sc->sc_osnetif_flowcntrl) {
        ath_netif_wake_queue(sc);
    }
#endif
}

/*
 * Update statistics from completed xmit descriptors from the specified queue.
 */
void
ath_tx_update_stats(struct ath_softc *sc, struct ath_buf *bf, u_int qnum, struct ath_tx_status *ts)
{
    ieee80211_tx_stat_t txstatus;
    u_int8_t txant, rateCode;
    struct ath_node *an = bf->bf_node;
#ifdef ATH_SWRETRY
    struct ath_swretry_info *pInfo;
#endif
    void *ds = bf->bf_lastbf->bf_desc;


    if (ts->ts_status == 0)  {
        /* successful completion */

        txant = ts->ts_antenna;
        sc->sc_cur_txant = txant; /* save current tx antenna */
    
        /* ast_ant_tx can only accommodate 8 antennas */
        sc->sc_stats.ast_ant_tx[txant & 0x7]++;
        sc->sc_ant_tx[txant & 0x7]++;
        if (ts->ts_rateindex != 0)
            sc->sc_stats.ast_tx_altrate++;
        sc->sc_stats.ast_tx_rssi = ts->ts_rssi;
        rateCode = ts->ts_ratecode;
        txstatus.rssi = ts->ts_rssi;
        txstatus.flags=0;

        if (sc->sc_hashtsupport) {
           /*
            * Update 11n stats
            */
            sc->sc_stats.ast_tx_rssi_ctl0 = ts->ts_rssi_ctl0;
            sc->sc_stats.ast_tx_rssi_ctl1 = ts->ts_rssi_ctl1;
            sc->sc_stats.ast_tx_rssi_ctl2 = ts->ts_rssi_ctl2;
            sc->sc_stats.ast_tx_rssi_ext0 = ts->ts_rssi_ext0;
            sc->sc_stats.ast_tx_rssi_ext1 = ts->ts_rssi_ext1;
            sc->sc_stats.ast_tx_rssi_ext2 = ts->ts_rssi_ext2;
           /*
            * Update rate average
            */
            txstatus.rssictl[0] = ts->ts_rssi_ctl0;
            txstatus.rssictl[1] = ts->ts_rssi_ctl1;
            txstatus.rssictl[2] = ts->ts_rssi_ctl2;
            txstatus.rssiextn[0] = ts->ts_rssi_ext0;
            txstatus.rssiextn[1] = ts->ts_rssi_ext1;
            txstatus.rssiextn[2] = ts->ts_rssi_ext2;
            txstatus.flags |= ATH_TX_CHAIN_RSSI_VALID;
        }

        if (bf->bf_isdata && !bf->bf_useminrate) {
            txstatus.rateieee = sc->sc_hwmap[rateCode].ieeerate;
            txstatus.rateKbps = sc->sc_hwmap[rateCode].rateKbps;
            txstatus.ratecode = rateCode;

            if (IS_HT_RATE(rateCode)) {
                u_int8_t rateFlags = bf->bf_rcs[ts->ts_rateindex].flags;

                /* TODO - add table to avoid division */
                if (rateFlags & ATH_RC_CW40_FLAG) {
                    txstatus.rateKbps = (txstatus.rateKbps * 27) / 13;
                }
                if (rateFlags & ATH_RC_SGI_FLAG) {
                    txstatus.rateKbps = (txstatus.rateKbps * 10) / 9;
                }
            }
            /*
             * Calculate the average of the IEEE rates, which
             * is in 500kbps units
             */
        } else {
            /*
             * Filter out management frames sent at the lowest rate, 
             * especially ProbeRequests sent during scanning.
             */
            txstatus.rateieee = 0;
            txstatus.rateKbps = 0;
            txstatus.ratecode = 0;
        }
        /*
         * Calculate air time of packet on air only when requested.
         * Tx status is not part of bf_daddr in case of enhanced dma.
         */

        if (!sc->sc_enhanceddmasupport) {
		    OS_SYNC_SINGLE(sc->sc_osdev, bf->bf_lastbf->bf_daddr, 
                    sc->sc_txdesclen, BUS_DMA_FROMDEVICE, NULL);
        }

        txstatus.airtime = bf->bf_calcairtime ? ath_hal_txcalcairtime(sc->sc_ah, ds, ts, AH_FALSE, 1, 1) : 0;

        sc->sc_ieee_ops->tx_status(an->an_node, &txstatus);

        if (bf->bf_flags & HAL_TXDESC_RTSENA) {
            /* XXX: how to get RTS success count ???
             * We assume once the RTS gets an CTS, normally the frame would
             * transmit succesfully coz no one would contend with us. */
            sc->sc_phy_stats[sc->sc_curmode].ast_tx_rts++;
        }
#ifdef ATH_SWRETRY
        if (sc->sc_enhanceddmasupport) {
            /* swr eligible frame succeeds */
            if (!bf->bf_isampdu && bf->bf_isdata) {
                struct ieee80211_frame * wh;
                wh = (struct ieee80211_frame *)wbuf_header(bf->bf_mpdu);
                pInfo = &an->an_swretry_info[qnum];

                // temp debug
                DPRINTF(sc, ATH_DEBUG_SWR, "%s: UPDATE dst=%s isswretry %d SeqCtrl=0x%02X%02X qnum=%d swr_num_eligible_frms=%d\n",
                        __func__, ether_sprintf(wh->i_addr1), bf->bf_isswretry, wh->i_seq[0], wh->i_seq[1],
                        qnum, pInfo->swr_num_eligible_frms);

                ASSERT(pInfo->swr_num_eligible_frms);
                ATH_NODE_SWRETRY_TXBUF_LOCK(an);
                pInfo->swr_num_eligible_frms --;
                ATH_NODE_SWRETRY_TXBUF_UNLOCK(an);

                DPRINTF(sc, ATH_DEBUG_SWR, "%s: SUCCESS dst=%s isswretry %d SeqCtrl=0x%02X%02X qnum=%d swr_num_eligible_frms=%d\n",
                        __func__, ether_sprintf(wh->i_addr1), bf->bf_isswretry, wh->i_seq[0], wh->i_seq[1],
                        qnum, pInfo->swr_num_eligible_frms);
            }
        }
        /* backward compatible with Merlin SW retry */
        else {
            if (bf->bf_isswretry) {
                pInfo = &an->an_swretry_info[qnum];
                ASSERT(pInfo->swr_num_pendfrms || an->an_total_swrtx_pendfrms);

                /* Decrement the counters, as we successfully
                 * transmitted the swretry frame
                 */
                pInfo->swr_num_pendfrms--;
                an->an_total_swrtx_pendfrms--;
                an->an_total_swrtx_successfrms++;

                if (!an->an_total_swrtx_pendfrms)
                    ath_tx_drain_sxmitq(sc, an);
            }
        }
#endif
    } else {

        if (ts->ts_status & HAL_TXERR_XRETRY) {
            sc->sc_stats.ast_tx_xretries++;
#if defined(ATH_ADDITIONAL_STATS) || ATH_SUPPORT_IQUE
            sc->sc_stats.ast_txq_xretries[qnum]++;
#endif
        }
        if (ts->ts_status & HAL_TXERR_FIFO) {
            sc->sc_stats.ast_tx_fifoerr++;
#if defined(ATH_ADDITIONAL_STATS) || ATH_SUPPORT_IQUE
            sc->sc_stats.ast_txq_fifoerr[qnum]++;
#endif
        }
        if (ts->ts_status & HAL_TXERR_FILT) {
            sc->sc_stats.ast_tx_filtered++;
#if defined(ATH_ADDITIONAL_STATS) || ATH_SUPPORT_IQUE
            sc->sc_stats.ast_txq_filtered[qnum]++;
#endif
        }
        if (ts->ts_status & HAL_TXERR_XTXOP)
            __11nstats(sc, txaggr_xtxop);
        if (ts->ts_status & HAL_TXERR_TIMER_EXPIRED)
            __11nstats(sc, txaggr_timer_exp);

        if (ts->ts_status & HAL_TXERR_BADTID)
            __11nstats(sc, txaggr_badtid);


#ifdef ATH_SWRETRY
        if (sc->sc_enhanceddmasupport) {
            /* this swr eligible frame fails but was not marked as ATH_BUFSTATUS_MARKEDSWRETRY.
             * For example, an_swrenabled changes to 0 at this time. In this case, we do not
             * requeue this eligible frame but decrease number of eligible frame only.
             */
            if (!bf->bf_isampdu && bf->bf_isdata &&
                !(bf->bf_status & ATH_BUFSTATUS_MARKEDSWRETRY)) {
                pInfo = &an->an_swretry_info[qnum];

                ASSERT(pInfo->swr_num_eligible_frms);
                ATH_NODE_SWRETRY_TXBUF_LOCK(an); 
                pInfo->swr_num_eligible_frms --;
                ATH_NODE_SWRETRY_TXBUF_UNLOCK(an);
            }
        }
        /* backward compatible with Merlin SW retry */
        else {
            /* Decrement the number of pendfrms when we are
             * trying to come out of sw retrying mechanism
             */
            if ((sc->sc_scanning || (!an->an_swrenabled)) && /*If scanning or Bmiss happens*/
                 bf->bf_isswretry && /*If frm passed through swRetry mechanism*/
                 !(bf->bf_status & ATH_BUFSTATUS_MARKEDSWRETRY))
            {
                pInfo = &an->an_swretry_info[qnum];
                pInfo->swr_num_pendfrms--;
                an->an_total_swrtx_pendfrms--;
                sc->sc_halstats.ns_swretryfailcount++;
            }
        }
#endif

    }
    sc->sc_phy_stats[sc->sc_curmode].ast_tx_shortretry += ts->ts_shortretry;
    sc->sc_phy_stats[sc->sc_curmode].ast_tx_longretry += ts->ts_longretry;

    if (ATH_ENAB_AOW(sc) && (ATH_ENAB_AOW_ES(sc) || ATH_ENAB_AOW_ESS(sc))) {
        /* AoW Extended Statistics:
           Update transmission related status information */
        ath_aow_update_txstatus(sc, bf, ts);
    }
}

/*
 * Process completed xmit descriptors from the specified queue.
 */
#ifdef VOW_TIDSCHED
int
ath_tx_processq(struct ath_softc *sc, struct ath_txq *txq, int sched)
#else
int
ath_tx_processq(struct ath_softc *sc, struct ath_txq *txq)
#endif
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
	struct ath_tx_status *txstat;
#ifdef ATH_SUPPORT_UAPSD
    int uapsdq = 0;
#endif
    int nacked;
    int txok, nbad = 0;
    int isrifs = 0;
#ifdef ATH_SWRETRY
    struct ieee80211_frame  *wh;
#endif
#if ATH_SUPPORT_VOWEXT
    u_int8_t n_head_fail = 0;
    u_int8_t n_tail_fail = 0;
#endif
#if ATH_SUPPORT_CFEND
    int cfendq = 0;
#endif
    
    DPRINTF(sc, ATH_DEBUG_TX_PROC, "%s: tx queue %d (%x), link %p\n", __func__,
            txq->axq_qnum, ath_hal_gettxbuf(sc->sc_ah, txq->axq_qnum),
            txq->axq_link);

    if (txq == sc->sc_uapsdq) {
        DPRINTF(sc, ATH_DEBUG_UAPSD, "%s: reaping U-APSD txq\n", __func__);
#ifdef ATH_SUPPORT_UAPSD
        uapsdq = 1;
#endif
    }
    
#if ATH_SUPPORT_CFEND
    if (txq == sc->sc_cfendq) {
        cfendq = 1;
    }
#endif
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

        
#if ATH_SUPPORT_CFEND
        if (cfendq) {
            /* process CF End packet */
            if (bf && bf->bf_state.bfs_iscfend) {
                TAILQ_INIT(&bf_head);
                
                ATH_TXQ_MOVE_HEAD_UNTIL(txq, &bf_head, bf, bf_list);
                ath_tx_cfend_complete (sc, bf, &bf_head);
                TAILQ_INIT(&bf_head);
                ATH_TXQ_UNLOCK(txq);
                continue; /* process rest of the buffers */
            }
        }
#endif
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
#if defined(ATH_SWRETRY) && defined(ATH_SWRETRY_MODIFY_DSTMASK)
                if (sc->sc_swRetryEnabled)
                    txq->axq_destmask = AH_TRUE;
#endif
                ATH_TXQ_UNLOCK(txq);

#if ATH_SUPPORT_FLOWMAC_MODULE
                if (sc->sc_osnetif_flowcntrl) {
                    ath_netif_wake_queue(sc);
                }
#endif
                break;
            }
        }

        isrifs = (ATH_RIFS_SUBFRAME_FIRST == bf->bf_rifsburst_elem) ? 1 : 0;
        lastbf = bf->bf_lastbf;
        ds = lastbf->bf_desc;    /* NB: last decriptor */

		OS_SYNC_SINGLE(sc->sc_osdev, lastbf->bf_daddr, sc->sc_txdesclen,
                BUS_DMA_FROMDEVICE, NULL);

        status = ath_hal_txprocdesc(ah, ds);

#ifdef AR_DEBUG
        if (sc->sc_debug & ATH_DEBUG_XMIT_DESC)
            ath_printtxbuf(bf, status == HAL_OK);
#endif
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
        /* use a local cacheable copy to improce d-cache efficiency */
		txstat = &(ds->ds_txstat);
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

        txok = (txstat->ts_status == 0);

        /* workaround for Hardware corrupted TX TID
        * There are two ways we can handle this situation, either we
        * go over a ath_reset_internal path OR
        * corrupt the tx status in such a way that entire aggregate gets
        * re-transmitted, taking the second approach here.
        * Corrupt both tx_status and clear the ba bitmap.
        */

        if (bf->bf_isaggr && txok && bf->bf_tidno != txstat->tid) {
            txstat->ts_status |= HAL_TXERR_BADTID;
            txok = !txok;
            txstat->ba_low = txstat->ba_high = 0x0;
            DPRINTF(sc, ATH_DEBUG_XMIT,
                "%s:%d identified bad tid status (buf:desc  %d:%d)\n",
                __func__, __LINE__,bf->bf_tidno, txstat->tid);
        }

#ifdef ATH_SWRETRY
        if (txok || (ath_check_swretry_req(sc, bf) == AH_FALSE) || txq == sc->sc_uapsdq) {
            /* Change the status of the frame and complete
             * this frame as normal frame
             */
            bf->bf_status &= ~ATH_BUFSTATUS_MARKEDSWRETRY;

        } else {
            /* This frame is going through SW retry mechanism
             */
            bf->bf_status |= ATH_BUFSTATUS_MARKEDSWRETRY;

            bf = ath_form_swretry_frm(sc, txq, &bf_head, bf);
            /* Here bf will be changed only when there is single
             * buffer for the current frame.
             */
            lastbf = bf->bf_lastfrm;
            ds = lastbf->bf_desc;
            OS_SYNC_SINGLE(sc->sc_osdev, lastbf->bf_daddr, sc->sc_txdesclen, BUS_DMA_FROMDEVICE, NULL);
			
			txstat = &(ds->ds_txstat);
        }
#endif

        ATH_TXQ_UNLOCK(txq);

        /* Put the old holding descriptor to the free queue */
        if (bf_held) {
            TAILQ_REMOVE(&bf_head, bf_held, bf_list);
#ifdef ATH_SUPPORT_UAPSD
            if (bf_held->bf_qosnulleosp) {

                ath_tx_uapsdqnulbf_complete(sc, bf_held, false);

            } else
#endif
            {
                ATH_TXBUF_LOCK(sc);
#if ATH_TX_BUF_FLOW_CNTL
                txq->axq_num_buf_used--;
                sc->sc_txbuf_free++;
#endif
                TAILQ_INSERT_TAIL(&sc->sc_txbuf, bf_held, bf_list);
                ATH_TXBUF_UNLOCK(sc);
            }
        }

        an = bf->bf_node;
        if (an != NULL) {
            int noratectrl;
            noratectrl = an->an_flags & (ATH_NODE_CLEAN | ATH_NODE_PWRSAVE);

            ath_tx_update_stats(sc, bf, txq->axq_qnum, txstat);

            /*
             * Hand the descriptor to the rate control algorithm
             * if the frame wasn't dropped for filtering or sent
             * w/o waiting for an ack.  In those cases the rssi
             * and retry counts will be meaningless.
             */
            if (! bf->bf_isampdu) {
                /*
                 * This frame is sent out as a single frame. Use hardware retry
                 * status for this frame.
                 */
                bf->bf_retries = txstat->ts_longretry;
                if (txstat->ts_status & HAL_TXERR_XRETRY) {
                      __11nstats(sc,tx_sf_hw_xretries);
                     bf->bf_isxretried = 1;
                }
                nbad = 0;
#if ATH_SUPPORT_VOWEXT
                if ( ATH_IS_VOWEXT_AGGRSIZE_ENABLED(sc) || ATH_IS_VOWEXT_RCA_ENABLED(sc)) 
                    n_head_fail = n_tail_fail = 0;
#endif
            } else {
                nbad = ath_tx_num_badfrms(sc, bf, txstat, txok);
                
#if ATH_SUPPORT_VOWEXT
                if ( ATH_IS_VOWEXT_AGGRSIZE_ENABLED(sc) || ATH_IS_VOWEXT_RCA_ENABLED(sc)) {
                    n_tail_fail = (nbad & 0xFF);
                    n_head_fail = ((nbad >> 8) & 0xFF);
                    nbad = ((nbad >> 16) & 0xFF);
                }
#endif
            }

            if ((txstat->ts_status & HAL_TXERR_FILT) == 0 &&
                (bf->bf_flags & HAL_TXDESC_NOACK) == 0) {
                /*
                 * If frame was ack'd update the last rx time
                 * used to workaround phantom bmiss interrupts.
                 */
                if (txstat->ts_status == 0)
                    nacked++;

                if (bf->bf_isdata && !noratectrl && likely(!bf->bf_useminrate)) {
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

#ifdef ATH_SWRETRY
            if ((sc->sc_debug & ATH_DEBUG_SWR) && (txstat->ts_status || bf->bf_isswretry)) {
                wh = (struct ieee80211_frame *)wbuf_header(bf->bf_mpdu);
                DPRINTF(sc, ATH_DEBUG_SWR, "%s: SeqNo%d --> rate %02X, swretry %d retrycnt %d totaltries %d\n",__func__,
                                                (*(u_int16_t *)&wh->i_seq[0]) >> 4, txstat->ts_ratecode, (bf->bf_isswretry)?1:0,
                                                bf->bf_swretries, bf->bf_totaltries + txstat->ts_longretry + txstat->ts_shortretry);
                DPRINTF(sc, ATH_DEBUG_SWR, "%s: SeqNo%d --> status %08X\n",__func__,
                                                (*(u_int16_t *)&wh->i_seq[0]) >> 4, txstat->ts_status);
            }

            if (bf->bf_status & ATH_BUFSTATUS_MARKEDSWRETRY) {
                ath_tx_mpdu_resend(sc, txq, &bf_head, *txstat);
                /* We have completed the buf in resend in case of
                 * failure and hence not needed and will be fatal
                 * if we fall through this
                 */
                //XXX TBD txFFDrain();
               continue;
            }
#endif
            /*
             * Complete this transmit unit
             *
             * Node cannot be referenced past this point since it can be freed
             * here.
             */
            if (bf->bf_isampdu) {
                if (txstat->ts_flags & HAL_TX_DESC_CFG_ERR)
                    __11nstats(sc, txaggr_desc_cfgerr);
                if (txstat->ts_flags & HAL_TX_DATA_UNDERRUN) {
                    __11nstats(sc, txaggr_data_urun);
#ifdef ATH_C3_WAR
                    sc->sc_fifo_underrun = 1;
#endif
                }
                if (txstat->ts_flags & HAL_TX_DELIM_UNDERRUN) {
                    __11nstats(sc, txaggr_delim_urun);
#ifdef ATH_C3_WAR
                    sc->sc_fifo_underrun = 1;
#endif
                }

                ath_tx_complete_aggr_rifs(sc, txq, bf, &bf_head, txstat, txok);
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
                    ath_tx_complete_buf(sc, bf, &bf_head, txok, 0, 0);
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
#ifdef VOW_TIDSCHED
        if(sched)
#endif
        {
          ATH_TXQ_LOCK(txq);
          ath_txq_schedule(sc, txq);
          ATH_TXQ_UNLOCK(txq);
        }
    }
    return nacked;
}

#ifdef VOW_TIDSCHED
static INLINE 
int get_queue_weight(struct ath_softc *sc, struct ath_atx_tid *tid, u_int32_t currts) {
  struct ath_buf *bf;
  u_int32_t lapsed, qin_ts;
  int qw;

  if(sc->tidsched) {
    bf = TAILQ_FIRST(&tid->buf_q);
    qin_ts = wbuf_get_qin_timestamp(bf->bf_mpdu);
    lapsed = (currts>=qin_ts) ? (currts - qin_ts) : 
                                ((0xffffffff - qin_ts) + currts);
    qw = lapsed * sc->acqw[tid->ac->qnum];
  }
  else {
    qw = 0;
  }
  return qw;
}

void
ath_tid_cqw(struct ath_softc *sc, struct ath_atx_tid *tid) {
  struct ath_buf *bf;
  int upd;
  int posfound;
  int timeout;

  struct ath_atx_tid *wrr_tid;
  u_int32_t currts;

  if(tid == NULL) return;

  if(TAILQ_EMPTY(&tid->buf_q) || tid->ac->qnum>=4) {
    tid->qw = 0;
    return;
  }

  bf = TAILQ_FIRST(&tid->buf_q);

  if(sc->tidsched) {
    u_int32_t lapsed, qin_ts;
    currts = ath_hal_gettsf32(sc->sc_ah);
    qin_ts = wbuf_get_qin_timestamp(bf->bf_mpdu);
    lapsed = (currts>=qin_ts) ? (currts - qin_ts) : 
                                ((0xffffffff - qin_ts) + currts);
    timeout = (lapsed>=sc->acto[tid->ac->qnum] && 
               sc->acto[tid->ac->qnum]!=0) ? 1 : 0;
  }
  else {
    currts = 0;
    timeout = 0;
  }

  upd = 0;
  TAILQ_FOREACH(wrr_tid,&sc->tid_q,wrr_tid_qelem) {
    /* Compute queue weight */
    wrr_tid->qw = get_queue_weight(sc, wrr_tid, currts);
    /* Check if tid is already inserted, and if so, flag for update */  
    if(wrr_tid == tid) {
      upd = 1;
    }
  }
  
  if(sc->tidsched) {
    /* If flagged for update, remove and re-insert in new position */
    if(upd) TAILQ_REMOVE(&sc->tid_q, tid, wrr_tid_qelem);
  
    if(!timeout) {
      /* Compute queue weight */
      if(!upd) tid->qw = get_queue_weight(sc, tid, currts);
      
      /* Insertion sort */
      if(tid->ac->qnum<4) {
        posfound = 0;
        TAILQ_FOREACH(wrr_tid,&sc->tid_q,wrr_tid_qelem) {
          if(wrr_tid->qw<=tid->qw) {
            posfound = 1;
            break;
          }
        }
        if(posfound==0) {
          TAILQ_INSERT_TAIL(&sc->tid_q, tid, wrr_tid_qelem);
        }
        else {
          TAILQ_INSERT_BEFORE(wrr_tid, tid, wrr_tid_qelem);
        }
      }
    }
    else {
      TAILQ_INSERT_HEAD(&sc->tid_q, tid, wrr_tid_qelem);
    }
  }
  else {
    if(!upd) TAILQ_INSERT_TAIL(&sc->tid_q, tid, wrr_tid_qelem);
  }
}
#endif

#ifdef AR_DEBUG
static void dump_desc(struct ath_softc *sc,struct ath_buf *bf,int qnum)
{
	struct ath_desc *ds;
	int  i = 0;

	DPRINTF(sc, ATH_DEBUG_ANY, "%s:  bf_daddr %08x len %d"
	    "TXDP %08x  \n",__func__, 
	    (int)bf->bf_daddr, bf->bf_buf_len[0],ath_hal_gettxbuf(sc->sc_ah,qnum));
		ds = (struct ath_desc *)bf->bf_desc;
		DPRINTF(sc, ATH_DEBUG_ANY, "%s: desc :  "
		    "%08x %08x %08x %08x \n",
		  __func__,ds->ds_link, ds->ds_data,
		  ds->ds_ctl0, ds->ds_ctl1);
		DPRINTF(sc, ATH_DEBUG_ANY, "%s", "Control words ");
		for (i=0;i<(sizeof(ds->ds_hw)/sizeof(u_int32_t));++i) {
		   if(i%4 == 0) DPRINTF(sc, ATH_DEBUG_ANY, "%s", "\n");
		   DPRINTF(sc, ATH_DEBUG_ANY, "%08x ",ds->ds_hw[i]);
		}
		DPRINTF(sc, ATH_DEBUG_ANY, "%s", "\n");

}

static void dump_txq_desc(struct ath_softc *sc, struct ath_txq *txq)
{
	struct ath_hal *ah = sc->sc_ah;
	struct ath_buf *bf;
	struct ath_desc *ds, *ds0;
	HAL_STATUS status;

    DPRINTF(sc, ATH_DEBUG_ANY, "%s: TXQ[%d] TXDP 0x%08x txq->axq_depth %d pending %d\n",__func__,
            txq->axq_qnum, ath_hal_gettxbuf(sc->sc_ah,txq->axq_qnum), txq->axq_depth, ath_hal_numtxpending(sc->sc_ah,txq->axq_qnum));
	ATH_TXQ_LOCK(txq);
	bf = TAILQ_FIRST(&txq->axq_q);
	ATH_TXQ_UNLOCK(txq);
	if (bf == NULL) {
		DPRINTF(sc, ATH_DEBUG_ANY, "%s: no descriptos for queue "
		    "%d \n",__func__,txq->axq_qnum);
		return;
	}
	DPRINTF(sc, ATH_DEBUG_ANY, "%s: Stuck descriptor dump for queue "
		"%d \n",__func__,txq->axq_qnum);
	TAILQ_FOREACH(bf, &txq->axq_q, bf_list) {
		ATH_TXQ_LOCK(txq);
		ds0 = (struct ath_desc *)bf->bf_desc;
		ds = ds0;
		status = ath_hal_txprocdesc(ah, ds);
		if ((int) bf->bf_daddr == ath_hal_gettxbuf(sc->sc_ah,txq->axq_qnum)){
			dump_desc(sc,bf,txq->axq_qnum);
			DPRINTF(sc, ATH_DEBUG_ANY, "%s", "\n\n");
		}
		if (status == HAL_EINPROGRESS) {
			dump_desc(sc,bf,txq->axq_qnum);
		}
		ATH_TXQ_UNLOCK(txq);
		if (status == HAL_EINPROGRESS) {
			return;
		}
	}
	DPRINTF(sc, ATH_DEBUG_ANY, "%s: no inprogress descriptors for "
		" queue %d \n",__func__,txq->axq_qnum);

}

void  ath_dump_descriptors(ath_dev_t dev)
{
    struct ath_softc *sc = ATH_DEV_TO_SC(dev);
	dump_txq_desc(sc, &sc->sc_txq[0]);
	dump_txq_desc(sc, &sc->sc_txq[1]);
	dump_txq_desc(sc, &sc->sc_txq[2]);
	dump_txq_desc(sc, &sc->sc_txq[3]);
	dump_txq_desc(sc, sc->sc_cabq);
    ath_dump_rx_desc(dev);

}
#endif

/*
 * Deferred processing of transmit interrupt.
 */
void
ath_tx_tasklet(ath_dev_t dev)
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
    if ((atomic_read(&sc->sc_has_tx_bcn_notify)) && (qcumask & (1 << sc->sc_bhalq))) {
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
#ifdef VOW_TIDSCHED // {
    for (i = HAL_NUM_DATA_QUEUES; i < HAL_NUM_TX_QUEUES; i++) {
        if (ATH_TXQ_SETUP(sc, i)) {
            if (((qcumask & (1 << i)) 
#if ATH_TX_POLL // {
                || (sc->sc_txq[i].axq_depth &&
                (q_time = ATH_DIFF(sc->sc_txq[i].axq_lastq_tick,ticks))  > MSEC_TO_TICKS(ATH_TX_POLL_TIMER))
#endif // }
                    )) {
                nacked += ath_tx_processq(sc, &sc->sc_txq[i], 1);
            }
#if ATH_TX_TO_RESET_ENABLE // {
            if (q_time > max_q_time) {
                max_q_time = q_time;
            }
#endif // }
            qdepth += ath_txq_depth(sc, i);
        }
    }

    for (i = 0; i < HAL_NUM_DATA_QUEUES; i++) {
        if (ATH_TXQ_SETUP(sc, i)) {
            if (((qcumask & (1 << i)) 
#if ATH_TX_POLL // {
                || (sc->sc_txq[i].axq_depth &&
                (q_time = ATH_DIFF(sc->sc_txq[i].axq_lastq_tick,ticks))  > MSEC_TO_TICKS(ATH_TX_POLL_TIMER))
#endif // }
                    )) {
                nacked += ath_tx_processq(sc, &sc->sc_txq[i], 0);
            }
#if ATH_TX_TO_RESET_ENABLE // {
            if (q_time > max_q_time) {
                max_q_time = q_time;
            }
#endif // }
            qdepth += ath_txq_depth(sc, i);
        }
    }
    ath_wrr_schedule(sc);
#else // } else { VOW_TIDSCHED
    for (i = 0; i < HAL_NUM_TX_QUEUES; i++) {
        if (ATH_TXQ_SETUP(sc, i)) {
            if (((qcumask & (1 << i)) 
#if ATH_TX_POLL
                || (sc->sc_txq[i].axq_depth &&
                (q_time = ATH_DIFF(sc->sc_txq[i].axq_lastq_tick,ticks))  > MSEC_TO_TICKS(ATH_TX_POLL_TIMER))
#endif
                    )) {
                nacked += ath_tx_processq(sc, &sc->sc_txq[i]);
            }
#if ATH_TX_TO_RESET_ENABLE
            if (q_time > max_q_time) {
                max_q_time = q_time;
            }
#endif
            qdepth += ath_txq_depth(sc, i);
        }
    }
#endif // } VOW_TIDSCHED
    ath_vap_pause_txq_use_dec(sc);

#if ATH_TX_TO_RESET_ENABLE
    if (max_q_time > MSEC_TO_TICKS(ATH_TXQ_MAX_TIME)) {
        DPRINTF(sc, ATH_DEBUG_RESET, "%s: timed out on TXQ \n",__func__);
#ifdef AR_DEBUG		
		ath_dump_descriptors(sc);
#endif
        ath_internal_reset(sc);
    }
#endif

    ath_vap_pause_txq_use_inc(sc);
#ifdef ATH_SUPERG_XR
    if (sc->sc_xrtxq && txqactive(sc->sc_ah, sc->sc_xrtxq->axq_qnum))
#ifdef VOW_TIDSCHED
        nacked += ath_tx_processq(sc, sc->sc_xrtxq, 1);
#else
        nacked += ath_tx_processq(sc, sc->sc_xrtxq);
#endif
#endif
    ath_vap_pause_txq_use_dec(sc);


    if (sc->sc_ieee_ops->notify_txq_status)
        sc->sc_ieee_ops->notify_txq_status(sc->sc_ieee, qdepth);
#if ATH_SUPPORT_FLOWMAC_MODULE
    if (sc->sc_osnetif_flowcntrl) {
        ath_netif_wake_queue(sc);
    }
#endif
}

void
ath_tx_draintxq(struct ath_softc *sc, struct ath_txq *txq, HAL_BOOL retry_tx)
{
    struct ath_buf *bf, *lastbf;
    ath_bufhead bf_head;

    /*
     * NB: this assumes output has been stopped and
     *     we do not need to block ath_tx_tasklet
     */
    for (;;) {
        ATH_TXQ_LOCK(txq);
        if (sc->sc_enhanceddmasupport) {
            bf = TAILQ_FIRST(&txq->axq_fifo[txq->axq_tailindex]);
            if (bf == NULL) {
                if (txq->axq_headindex != txq->axq_tailindex)
                    printk("ath_tx_draintxq: ERR head %d tail %d\n",
                           txq->axq_headindex, txq->axq_tailindex);
                txq->axq_headindex = 0;
                txq->axq_tailindex = 0;
                ATH_TXQ_UNLOCK(txq);
                break;
            }
        } else {
            bf = TAILQ_FIRST(&txq->axq_q);
            if (bf == NULL) {
                txq->axq_link = NULL;
                txq->axq_linkbuf = NULL;
                ATH_TXQ_UNLOCK(txq);
                break;
            }

#if ATH_SUPPORT_CFEND
            if ((txq == sc->sc_cfendq) && (bf) && (bf->bf_state.bfs_iscfend)) {
                /* process CF End packet */
                    TAILQ_INIT(&bf_head);
                    ATH_TXQ_MOVE_HEAD_UNTIL(txq, &bf_head, bf, bf_list);
                    ath_tx_cfend_complete (sc, bf, &bf_head);
                    ATH_TXQ_UNLOCK(txq); /*unlock before return */
                    /* there is only one frame in queue always, do not
                     * cycle again
                     */
                    return;
            }
#endif
            if (bf->bf_status & ATH_BUFSTATUS_STALE) {
                ATH_TXQ_REMOVE_STALE_HEAD(txq, bf, bf_list);
                ATH_TXQ_UNLOCK(txq);

                ATH_TXBUF_LOCK(sc);
#if ATH_TX_BUF_FLOW_CNTL
                if(bf) {
                    txq->axq_num_buf_used--;
                    sc->sc_txbuf_free++;
                }
#endif
                TAILQ_INSERT_TAIL(&sc->sc_txbuf, bf, bf_list);
                ATH_TXBUF_UNLOCK(sc);
#if ATH_SUPPORT_FLOWMAC_MODULE
                if (sc->sc_osnetif_flowcntrl) {
                    ath_netif_wake_queue(sc);
                }
#endif
                continue;
            }
        }

        lastbf = bf->bf_lastbf;
        if (!retry_tx)
            lastbf->bf_isswaborted = 1;

        /* remove ath_buf's of the same mpdu from txq */
        if (sc->sc_enhanceddmasupport) {
            if ((txq == sc->sc_cabq) || (txq == sc->sc_uapsdq)) {
                    ATH_EDMA_MCASTQ_MOVE_HEAD_UNTIL(txq, &bf_head, lastbf, bf_list);
            }
            else {
                    ATH_EDMA_TXQ_MOVE_HEAD_UNTIL(txq, &bf_head, lastbf, bf_list);
            }
        } else {
            ATH_TXQ_MOVE_HEAD_UNTIL(txq, &bf_head, lastbf, bf_list);
        }

        /*
         * Fail this transmit unit
         */
        if (bf->bf_isaggr) {
            txq->axq_aggr_depth--;
           __11nstats(sc,tx_comperror);
        }

        ATH_TXQ_UNLOCK(txq);

#ifdef AR_DEBUG
        if (!sc->sc_enhanceddmasupport && (sc->sc_debug & ATH_DEBUG_RESET))
            /* Legacy only as the enhanced DMA txprocdesc() 
             * will move the tx status ring tail pointer.
             */
            ath_printtxbuf(bf, ath_hal_txprocdesc(sc->sc_ah, bf->bf_desc) == HAL_OK);
#endif /* AR_DEBUG */

        if (bf->bf_isampdu) {
            if (!bf->bf_isaggr) {
                __11nstats(sc,tx_unaggr_comperror);
            }
            ath_tx_complete_aggr_rifs(sc, txq, bf, &bf_head,
                                      &((struct ath_desc *)(lastbf->bf_desc))->ds_txstat, 0);
        } else {
            if (bf->bf_isbar) {
                ath_tx_complete_bar(sc, bf, &bf_head, 0);
            }
            else
#ifdef ATH_SUPPORT_TxBF
                ath_tx_complete_buf(sc, bf, &bf_head, 0, 0, 0);
#else
                ath_tx_complete_buf(sc, bf, &bf_head, 0);
#endif
        }
    }

    /* flush any pending frames if aggregation is enabled */
    if (!retry_tx) {
        ATH_TXQ_LOCK(txq);
        ath_txq_drain_pending_buffers(sc, txq);
        ATH_TXQ_UNLOCK(txq);
        sc->sc_ieee_ops->drain_amsdu(sc->sc_ieee); /* drain amsdu buffers */
    }

#if defined(ATH_SWRETRY) && defined(ATH_SWRETRY_MODIFY_DSTMASK)
	ATH_TXQ_LOCK(txq);
    if (sc->sc_swRetryEnabled)
        txq->axq_destmask = AH_TRUE;
	ATH_TXQ_UNLOCK(txq);
#endif
}

static void
ath_tx_stopdma(struct ath_softc *sc, struct ath_txq *txq, int timeout)
{
    struct ath_hal *ah = sc->sc_ah;

    (void) ath_hal_stoptxdma(ah, txq->axq_qnum, timeout);
    DPRINTF(sc, ATH_DEBUG_RESET, "%s: tx queue [%u] %x, link %p\n",
            __func__, txq->axq_qnum,
            ath_hal_gettxbuf(ah, txq->axq_qnum), txq->axq_link);
}

static void
ath_tx_abortalldma(struct ath_softc *sc)
{
    struct ath_hal *ah = sc->sc_ah;

    (void) ath_hal_aborttxdma(ah);
    DPRINTF(sc, ATH_DEBUG_RESET, "%s: all tx queues\n", __func__);
}

/*
 * Drain only the data queues, called by reset_start().
 */
static void
ath_reset_drain_txdataq(struct ath_softc *sc, HAL_BOOL retry_tx, int timeout)
{
    struct ath_hal *ah = sc->sc_ah;
    int i;
    int npend = 0;
    HAL_HT_MACMODE ht_macmode = sc->sc_ieee_ops->cwm_macmode(sc->sc_ieee);

    /* XXX return value */
    if (!sc->sc_invalid) {
#ifndef ATH_SUPPORT_HTC
        for (i = 0; i < HAL_NUM_TX_QUEUES; i++) {
            if (ATH_TXQ_SETUP(sc, i)) {
                if ( !(sc->sc_fastabortenabled && (sc->sc_reset_type == ATH_RESET_NOLOSS)) ){
                    ath_tx_stopdma(sc, &sc->sc_txq[i], timeout); 
                }

                /* The TxDMA may not really be stopped.
                 * Double check the hal tx pending count */
                npend += ath_hal_numtxpending(ah, sc->sc_txq[i].axq_qnum);
            }
        }
#else
        ath_wmi_drain_txq(sc);
#endif
    }

    if (npend) {
        HAL_STATUS status;

#ifdef AR_DEBUG		
		ath_dump_descriptors(sc);
#endif
        /* TxDMA not stopped, reset the hal */
        DPRINTF(sc, ATH_DEBUG_RESET, "%s: Unable to stop TxDMA. Reset HAL!\n", __func__);

        ATH_USB_TX_STOP(sc->sc_osdev);
#if ATH_C3_WAR
        STOP_C3_WAR_TIMER(sc);
#endif
#if !ATH_RESET_SERIAL
		ATH_LOCK_PCI_IRQ(sc);
#endif
        if (!ath_hal_reset(ah, sc->sc_opmode,
                           &sc->sc_curchan, ht_macmode,
                           sc->sc_tx_chainmask, sc->sc_rx_chainmask,
                           sc->sc_ht_extprotspacing, AH_FALSE, &status,
                           sc->sc_scanning)) {
            printk("%s: unable to reset hardware; hal status %u\n", __func__, status);
        }
        ATH_HTC_ABORTTXQ(sc);
#if !ATH_RESET_SERIAL
        ATH_UNLOCK_PCI_IRQ(sc);
#endif
        ATH_USB_TX_START(sc->sc_osdev);
    }

    //sc->sc_dev->trans_start = jiffies;
    // RNWF Need this?? netif_start_queue(sc->sc_dev);
    for (i = 0; i < HAL_NUM_TX_QUEUES; i++) {
        if (ATH_TXQ_SETUP(sc, i)) {
#ifdef ATH_SUPPORT_UAPSD
            if (&sc->sc_txq[i] == sc->sc_uapsdq) {
                ath_tx_uapsd_draintxq(sc);
                continue;
            }
#endif
            ath_tx_draintxq(sc, &sc->sc_txq[i], retry_tx);
        }
    }
}

/*
 * Drain the transmit queues and reclaim resources, called in reset_start().
 */
void
ath_reset_draintxq(struct ath_softc *sc, HAL_BOOL retry_tx, int timeout)
{
    /* stop beacon queue. The beacon will be freed when we go to INIT state */
    ath_vap_pause_txq_use_inc(sc);
    if (!sc->sc_invalid) {
        if ( !(sc->sc_fastabortenabled && (sc->sc_reset_type == ATH_RESET_NOLOSS)) ){
            (void) ath_hal_stoptxdma(sc->sc_ah, sc->sc_bhalq, timeout);
        }
        if (sc->sc_fastabortenabled) {
            /* fast tx abort */
            ath_tx_abortalldma(sc);
        }
        DPRINTF(sc, ATH_DEBUG_RESET, "%s: beacon queue %x\n", __func__,
                ath_hal_gettxbuf(sc->sc_ah, sc->sc_bhalq));

    }

    ath_reset_drain_txdataq(sc, retry_tx, timeout);
    ath_vap_pause_txq_use_dec(sc);
}


/*
 * Drain only the data queues.
 */
static void
ath_drain_txdataq(struct ath_softc *sc, HAL_BOOL retry_tx, int timeout)
{
    struct ath_hal *ah = sc->sc_ah;
    int i;
    int npend = 0;
    HAL_HT_MACMODE ht_macmode = sc->sc_ieee_ops->cwm_macmode(sc->sc_ieee);

#if ATH_SUPPORT_FLOWMAC_MODULE
    if (sc->sc_osnetif_flowcntrl) {
        ath_netif_stop_queue(sc);
    }
#endif
    /* XXX return value */
    if (!sc->sc_invalid) {
#ifndef ATH_SUPPORT_HTC
        for (i = 0; i < HAL_NUM_TX_QUEUES; i++) {
            if (ATH_TXQ_SETUP(sc, i)) {
                if ( !(sc->sc_fastabortenabled && (sc->sc_reset_type == ATH_RESET_NOLOSS)) ){
                    ath_tx_stopdma(sc, &sc->sc_txq[i], timeout); 
                }

                /* The TxDMA may not really be stopped.
                 * Double check the hal tx pending count */
                npend += ath_hal_numtxpending(ah, sc->sc_txq[i].axq_qnum);
            }
        }
#else
        ath_wmi_drain_txq(sc);
#endif
    }

    if (npend) {
        HAL_STATUS status;

#ifdef AR_DEBUG		
		ath_dump_descriptors(sc);
#endif
        /* TxDMA not stopped, reset the hal */
        DPRINTF(sc, ATH_DEBUG_RESET, "%s: Unable to stop TxDMA. Reset HAL!\n", __func__);

        ATH_USB_TX_STOP(sc->sc_osdev);
#ifdef ATH_C3_WAR
        STOP_C3_WAR_TIMER(sc);
#endif
#if ATH_RESET_SERIAL
        ATH_RESET_ACQUIRE_MUTEX(sc);
        ATH_RESET_LOCK(sc);
        atomic_inc(&sc->sc_hold_reset);
        ath_reset_wait_tx_rx_finished(sc);
        ATH_RESET_UNLOCK(sc);
#else
		ATH_LOCK_PCI_IRQ(sc);
#endif
        if (!ath_hal_reset(ah, sc->sc_opmode,
                           &sc->sc_curchan, ht_macmode,
                           sc->sc_tx_chainmask, sc->sc_rx_chainmask,
                           sc->sc_ht_extprotspacing, AH_FALSE, &status,
                           sc->sc_scanning)) {
            printk("%s: unable to reset hardware; hal status %u\n", __func__, status);
        }
        ATH_HTC_ABORTTXQ(sc);
#if ATH_RESET_SERIAL
        atomic_dec(&sc->sc_hold_reset);
        ATH_RESET_RELEASE_MUTEX(sc);
#else
        ATH_UNLOCK_PCI_IRQ(sc);
#endif
        ATH_USB_TX_START(sc->sc_osdev);
    }

    //sc->sc_dev->trans_start = jiffies;
    // RNWF Need this?? netif_start_queue(sc->sc_dev);
    for (i = 0; i < HAL_NUM_TX_QUEUES; i++) {
        if (ATH_TXQ_SETUP(sc, i)) {
#ifdef ATH_SUPPORT_UAPSD
            if (&sc->sc_txq[i] == sc->sc_uapsdq) {
                ath_tx_uapsd_draintxq(sc);
                continue;
            }
#endif
            ath_tx_draintxq(sc, &sc->sc_txq[i], retry_tx);
        }
    }
#if ATH_SUPPORT_FLOWMAC_MODULE
    if (sc->sc_osnetif_flowcntrl) {
        ath_netif_wake_queue(sc);
        ath_netif_update_trans(sc);
    }
#endif
}

/*
 * Drain the transmit queues and reclaim resources.
 */
void
ath_draintxq(struct ath_softc *sc, HAL_BOOL retry_tx, int timeout)
{
    /* stop beacon queue. The beacon will be freed when we go to INIT state */
    ath_vap_pause_txq_use_inc(sc);
    if (!sc->sc_invalid) {
        if ( !(sc->sc_fastabortenabled && (sc->sc_reset_type == ATH_RESET_NOLOSS)) ){
            (void) ath_hal_stoptxdma(sc->sc_ah, sc->sc_bhalq, timeout);
        }
        if (sc->sc_fastabortenabled) {
            /* fast tx abort */
            ath_tx_abortalldma(sc);
        }
        DPRINTF(sc, ATH_DEBUG_RESET, "%s: beacon queue %x\n", __func__,
                ath_hal_gettxbuf(sc->sc_ah, sc->sc_bhalq));

    }

    ath_drain_txdataq(sc, retry_tx, timeout);
    ath_vap_pause_txq_use_dec(sc);
}

/*
 * Flush the pending traffic in tx queues. Beacon queue is not stopped.
 */
void
ath_tx_flush(ath_dev_t dev)
{
    struct ath_softc *sc = ATH_DEV_TO_SC(dev);

    ATH_PS_WAKEUP(sc);
    ath_vap_pause_txq_use_inc(sc);
    ath_drain_txdataq(sc, AH_FALSE, 0);
    ath_vap_pause_txq_use_dec(sc);
    ATH_PS_SLEEP(sc);
}

u_int32_t
ath_txq_depth(ath_dev_t dev, int qnum)
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

u_int32_t
ath_txq_aggr_depth(ath_dev_t dev, int qnum)
{
    struct ath_softc *sc = ATH_DEV_TO_SC(dev);

    return sc->sc_txq[qnum].axq_aggr_depth;
}

#ifdef ATH_TX_BUF_FLOW_CNTL
/* API provided to dynamically adjust the minfree value for each queue */
void
ath_txq_set_minfree(ath_dev_t dev, int qtype, int haltype, u_int minfree)
{
#define    N(a)    (sizeof(a)/sizeof(a[0]))
    struct ath_softc *sc = ATH_DEV_TO_SC(dev);
    struct ath_txq *txq;
    int qnum;
    
    if (qtype == HAL_TX_QUEUE_BEACON || qtype == HAL_TX_QUEUE_CAB)
        return;

    qnum = ath_tx_get_qnum(dev, qtype, haltype);
    if (qnum == -1)
        return;

    if (!ATH_TXQ_SETUP(sc, qnum))
        return;

    if (minfree > ATH_TXBUF)
        return;

    txq = &sc->sc_txq[qnum];
    txq->axq_minfree = minfree;
    
    return;
}
#endif

/*
 * ath_pkt_dur - compute packet duration (NB: not NAV)
 * ref: depot/chips/owl/2.0/rtl/mac/doc/rate_to_duration_ht.xls
 *
 * rix - rate index
 * pktlen - total bytes (delims + data + fcs + pads + pad delims)
 * width  - 0 for 20 MHz, 1 for 40 MHz
 * half_gi - to use 4us v/s 3.6 us for symbol time
 */
u_int32_t
ath_pkt_duration(struct ath_softc *sc, u_int8_t rix, struct ath_buf *bf,
                 int width, int half_gi, HAL_BOOL shortPreamble)
{
    const HAL_RATE_TABLE    *rt = sc->sc_currates;
    u_int32_t               nbits, nsymbits, duration, nsymbols;
    u_int8_t                rc;
    int                     streams;
    int                     pktlen;
    
    pktlen = bf->bf_isaggr ? bf->bf_al : bf->bf_frmlen;
    rc = rt->info[rix].rateCode;

    /*
     * for legacy rates, use old function to compute packet duration
     */
    if (!IS_HT_RATE(rc))
        return ath_hal_computetxtime(sc->sc_ah, rt, pktlen, rix,
                                     shortPreamble);

    /*
     * find number of symbols: PLCP + data
     */
    nbits = (pktlen << 3) + OFDM_PLCP_BITS;
    nsymbits = bits_per_symbol[HT_RC_2_MCS(rc)][width];
    nsymbols = (nbits + nsymbits - 1) / nsymbits;

    if (!half_gi)
        duration = SYMBOL_TIME(nsymbols);
    else
        duration = SYMBOL_TIME_HALFGI(nsymbols);

    /*
     * addup duration for legacy/ht training and signal fields
     */
    streams = HT_RC_2_STREAMS(rc);
    duration += L_STF + L_LTF + L_SIG + HT_SIG + HT_STF + HT_LTF(streams);
    return duration;
}

/*
 * Adaptive Power Management:
 * Some 3 stream chips exceed the PCIe power requirements.
 * This workaround will reduce power consumption by using 2 tx chains
 * for 1 and 2 stream rates (5 GHz only)
 *
 */
uint8_t ath_txchainmask_reduction(struct ath_softc *sc,
                                  uint8_t chainmask, uint8_t rate)
{
    if (sc->sc_enableapm && (sc->sc_curchan.channelFlags & CHANNEL_5GHZ) &&
        (chainmask == 0x7) && (rate < 0x90)) {
        return 0x3;
    } else {
        return chainmask;
    }
}

/*
 * Rate module function to set rate related fields in tx descriptor.
 */
void
ath_buf_set_rate(struct ath_softc *sc, struct ath_buf *bf)
{
    struct ath_hal       *ah = sc->sc_ah;
    const HAL_RATE_TABLE *rt;
    void                 *ds = bf->bf_desc;
    void                 *lastds = bf->bf_lastbf->bf_desc;
    HAL_11N_RATE_SERIES  series[4];
    int                  i, flags, rtsctsena = 0, dynamic_mimops = 0;
    u_int8_t             rix = 0, cix, ctsrate = 0;
    u_int                ctsduration = 0;
    u_int32_t            aggr_limit_with_rts = sc->sc_rtsaggrlimit;
    struct ath_node      *an = (struct ath_node *) bf->bf_node;
    u_int                txpower;
#ifdef ATH_SUPPORT_TxBF
    u_int32_t           tmpflags;
#endif

#if ATH_SUPPORT_IQUE && ATH_SUPPORT_IQUE_EXT
    u_int               retry_duration = 0;
#endif
#if UMAC_SUPPORT_INTERNALANTENNA
    u_int32_t ant = 0;
    u_int8_t rtset_index = 0;
#endif    
    u_int32_t smartAntenna = 0;
    /*
     * get the cix for the lowest valid rix.
     */
    rt = sc->sc_currates;
    for (i = 4; i--;) {
        if (bf->bf_rcs[i].tries) {
            rix = bf->bf_rcs[i].rix;
            break;
        }
    }
    flags = (bf->bf_flags & (HAL_TXDESC_RTSENA | HAL_TXDESC_CTSENA));
#ifdef ATH_SUPPORT_TxBF
    tmpflags= bf->bf_flags & HAL_TXDESC_TXBF;
#endif
    cix = rt->info[rix].controlRate;

    if (flags & (HAL_TXDESC_RTSENA | HAL_TXDESC_CTSENA)) {
        rtsctsena = 1;
    }
    /*
     * If 802.11g protection is enabled, determine whether
     * to use RTS/CTS or just CTS.  Note that this is only
     * done for OFDM/HT unicast frames.
     */
    else if (sc->sc_protmode != PROT_M_NONE &&
        (rt->info[rix].phy == IEEE80211_T_OFDM ||
         rt->info[rix].phy == IEEE80211_T_HT) &&
        (bf->bf_flags & HAL_TXDESC_NOACK) == 0)
    {
        if (sc->sc_protmode == PROT_M_RTSCTS)
            flags = HAL_TXDESC_RTSENA;
        else if (sc->sc_protmode == PROT_M_CTSONLY)
            flags = HAL_TXDESC_CTSENA;

        cix = rt->info[sc->sc_protrix].controlRate;
        sc->sc_stats.ast_tx_protect++;
        rtsctsena = 1;
    }

    /* For 11n, the default behavior is to enable RTS for
     * hw retried frames. We enable the global flag here and
     * let rate series flags determine which rates will actually
     * use RTS.
     */
    if (sc->sc_hashtsupport && bf->bf_isdata) {
        KASSERT(an != NULL, ("an == null"));
        /*
         * 802.11g protection not needed, use our default behavior
         */
        if (!rtsctsena)
            flags = HAL_TXDESC_RTSENA;
        /*
         * For dynamic MIMO PS, RTS needs to precede the first aggregate
         * and the second aggregate should have any protection at all.
         */
        if (an->an_smmode == ATH_SM_PWRSAV_DYNAMIC) {
            if (!bf->bf_aggrburst) {
                flags = HAL_TXDESC_RTSENA;
                dynamic_mimops = 1;
            } else {
                flags = 0;
            }
        }
    }

    /*
     * Set protection if aggregate protection on
     */
    if (sc->sc_config.ath_aggr_prot && (!bf->bf_isaggr ||
        (bf->bf_isaggr && bf->bf_al < sc->sc_config.ath_aggr_prot_max)))
    {
        flags = HAL_TXDESC_RTSENA;
        cix = rt->info[sc->sc_protrix].controlRate;
        rtsctsena = 1;
    }

    /*
     * OWL 2.0 WAR, RTS cannot be followed by a frame larger than 8K.
     */
    if (bf->bf_isaggr && (bf->bf_al > aggr_limit_with_rts)) {
        /*
         * Ensure that in the case of SM Dynamic power save
         * while we are bursting the second aggregate the
         * RTS is cleared.
         */
        flags &= ~(HAL_TXDESC_RTSENA);
    }

    /*
     * CTS transmit rate is derived from the transmit rate
     * by looking in the h/w rate table.  We must also factor
     * in whether or not a short preamble is to be used.
     */
    /* NB: cix is set above where RTS/CTS is enabled */
    KASSERT(cix != 0xff, ("cix not setup"));
    ctsrate = rt->info[cix].rateCode |
        (bf->bf_shpreamble ? rt->info[cix].shortPreamble : 0);

    /*
     * Setup HAL rate series
     */
    OS_MEMZERO(series, sizeof(HAL_11N_RATE_SERIES) * 4);
    txpower = IS_CHAN_2GHZ(&sc->sc_curchan) ?
        sc->sc_config.txpowlimit2G :
        sc->sc_config.txpowlimit5G;

    /*
     * Check bf->bf_reftxpower for short range mode.
     * If bf->bf_reftxpower is SRM txpower (0), save it as a txpower cap.
     */
    if (bf->bf_reftxpower == ATH_TX_POWER_SRM) txpower = ATH_TX_POWER_SRM;

    for (i = 0; i < 4; i++) {
        int max_tx_numchains;

        if (!bf->bf_rcs[i].tries)
            continue;

        rix = bf->bf_rcs[i].rix;

        series[i].RateIndex = rix;
        series[i].TxPowerCap = txpower;

        series[i].Rate = rt->info[rix].rateCode |
                         (bf->bf_shpreamble ? rt->info[rix].shortPreamble : 0);

        series[i].Tries = bf->bf_rcs[i].tries;

        series[i].RateFlags = (
            (bf->bf_rcs[i].flags & ATH_RC_RTSCTS_FLAG) ? HAL_RATESERIES_RTS_CTS : 0) |
            ((bf->bf_rcs[i].flags & ATH_RC_CW40_FLAG) ? HAL_RATESERIES_2040 : 0)     |
            ((bf->bf_rcs[i].flags & ATH_RC_SGI_FLAG) ? HAL_RATESERIES_HALFGI : 0)    |
#ifdef ATH_SUPPORT_TxBF
            ((bf->bf_rcs[i].flags & ATH_RC_TXBF_FLAG) ? HAL_RATESERIES_TXBF : 0)     | 
#endif
            ((bf->bf_rcs[i].flags & ATH_RC_TX_STBC_FLAG) ? HAL_RATESERIES_STBC: 0);

        series[i].PktDuration = ath_pkt_duration(
            sc, rix, bf,
            (bf->bf_rcs[i].flags & ATH_RC_CW40_FLAG) != 0,
            (bf->bf_rcs[i].flags & ATH_RC_SGI_FLAG),
            bf->bf_shpreamble);

#if ATH_SUPPORT_IQUE && ATH_SUPPORT_IQUE_EXT
        retry_duration += series[i].PktDuration * series[i].Tries;
#endif

        /*
         * Check whether fewer than the full number of chains should
         * be used, e.g. due to the dynamic tx chainmask feature's
         * limitation that the number of chains should not exceed the
         * number of 64-QAM MIMO streams, or the CDD feature's limitation
         * to a single chain on legacy channels.
         */
        max_tx_numchains = ath_rate_max_tx_chains(sc, rix, bf->bf_rcs[i].flags);
        if (sc->sc_tx_numchains > max_tx_numchains) {
           static uint8_t tx_reduced_chainmasks[] = {
                0x0 /* n/a  never have 0 chains */,
                0x1 /* single chain (0) for single stream */,
                0x5 /* 2 chains (0+2) for 2 streams */,
           };
           ASSERT(max_tx_numchains > 0);
           ASSERT(max_tx_numchains < ARRAY_LEN(tx_reduced_chainmasks));
           series[i].ChSel = ath_txchainmask_reduction(sc,
                   tx_reduced_chainmasks[max_tx_numchains], series[i].Rate);
        } else {
            if ((an->an_smmode == ATH_SM_PWRSAV_STATIC) &&
                ((bf->bf_rcs[i].flags & (ATH_RC_DS_FLAG | ATH_RC_TS_FLAG)) == 0))
            {
                /*
                 * When sending to an HT node that has enabled static
                 * SM/MIMO power save, send at single stream rates but use
                 * maximum allowed transmit chains per user, hardware,
                 * regulatory, or country limits for better range.
                 */
                series[i].ChSel = ath_txchainmask_reduction(sc, sc->sc_tx_chainmask,
                                                            series[i].Rate);
            } else {
#ifdef ATH_CHAINMASK_SELECT
                if (bf->bf_ht)
                    series[i].ChSel = ath_txchainmask_reduction(sc,
                            ath_chainmask_sel_logic(sc, an), series[i].Rate);
                else
                    series[i].ChSel = ath_txchainmask_reduction(sc,
                            sc->sc_tx_chainmask, series[i].Rate);
#else
                series[i].ChSel = ath_txchainmask_reduction(sc, sc->sc_tx_chainmask,
                                                            series[i].Rate);
#endif
            }
        }

        if (rtsctsena)
            series[i].RateFlags |= HAL_RATESERIES_RTS_CTS;

        /*
         * Set RTS for all rates if node is in dynamic powersave
         * mode and we are using dual stream rates.
         */
        if (dynamic_mimops && 
            (bf->bf_rcs[i].flags & (ATH_RC_DS_FLAG | ATH_RC_TS_FLAG))) 
        {
            series[i].RateFlags |= HAL_RATESERIES_RTS_CTS;
        }
    }

#if ATH_SUPPORT_IQUE && ATH_SUPPORT_IQUE_EXT
    /*
     * Calculate and update the latency due to retries for VO/VI data queues only
     */
    if (bf->bf_isdata && sc->sc_txaggr && sc->sc_retry_duration > 0) {
        int ac = TID_TO_WME_AC(bf->bf_tidno);
        if (ac == WME_AC_VI || ac == WME_AC_VO)
        {
            ath_rateseries_update_retry(sc, bf, series, retry_duration);
        }
    }
#endif

    /*
     * For non-HT devices, calculate RTS/CTS duration in software
     * and disable multi-rate retry.
     */
    if (flags && !sc->sc_hashtsupport) {
        /*
         * Compute the transmit duration based on the frame
         * size and the size of an ACK frame.  We call into the
         * HAL to do the computation since it depends on the
         * characteristics of the actual PHY being used.
         *
         * NB: CTS is assumed the same size as an ACK so we can
         *     use the precalculated ACK durations.
         */
        if (flags & HAL_TXDESC_RTSENA) {    /* SIFS + CTS */
            ctsduration += bf->bf_shpreamble ?
                rt->info[cix].spAckDuration : rt->info[cix].lpAckDuration;
        }

        ctsduration += series[0].PktDuration;

        if ((bf->bf_flags & HAL_TXDESC_NOACK) == 0) {  /* SIFS + ACK */
            ctsduration += bf->bf_shpreamble ?
                rt->info[rix].spAckDuration : rt->info[rix].lpAckDuration;
        }

        /*
         * Disable multi-rate retry when using RTS/CTS by clearing
         * series 1, 2 and 3.
         */
        OS_MEMZERO(&series[1], sizeof(HAL_11N_RATE_SERIES) * 3);
    }
#ifdef ATH_SUPPORT_TxBF
    flags=flags | tmpflags; //for txbf
    if (flags &HAL_TXDESC_TXBF_SOUND)
    {
        //DPRINTF(sc, ATH_DEBUG_ANY,"==>%s:this is a sounding frame, rate code %x",__func__,series[0].Rate);
        if (!(flags & HAL_TXDESC_CAL))
        {
            struct ieee80211_qosframe_htc *wh1;

            wh1 = (struct ieee80211_qosframe_htc *)wbuf_header(bf->bf_mpdu);
            if (wh1->i_fc[1]&IEEE80211_FC1_ORDER)  // contain +HTC.  
            {
                if (series[0].Rate<0x80)    // legacy rate, cancell sounding.
                {
                    wh1->i_htc[0]=wh1->i_htc[1]=wh1->i_htc[2]=0;
                    flags&= ~(HAL_TXDESC_TXBF_SOUND);
                    //DPRINTF(sc, ATH_DEBUG_ANY,"==>%s:cancel sounding\n",__func__);
                }
            }    
           // DPRINTF(sc, ATH_DEBUG_ANY,"==>%s:HTC filed %x %x %x ",__func__,wh1->i_htc[0],wh1->i_htc[1],wh1->i_htc[2]);
        } 
    }
#endif

/* Programming TX desc with antenna combinations choosen by smart antenna module
 *   a) if it training packet use antena from wbuf
 *   b) if it is non training packet and not HT rates use default antenna for management and control packtes
 *   c) if it is HT rate then choose antenna stored in node structure, which contains default antenna if
 *      train is not done and selected antennas after training has completed by smart antenna module
 */

#if UMAC_SUPPORT_INTERNALANTENNA
   
#if  UMAC_SUPPORT_SMARTANTENNA 
if(wbuf_is_sa_train_packet(bf->bf_mpdu))
{
    /* this is training packet so take antenna values from skb->cb */
    ant = wbuf_sa_get_antenna(bf->bf_mpdu);
    smartAntenna = ant | (ant << 8)|(ant << 16) | (ant << 24);
} 
else
{   /* non training packet get values from node rate set*/
#endif

     if(sc->sc_smartant_enable) {
         /* form smartAntenna by adding antennas for each rate series */
         if (IS_HT_RATE(series[0].Rate)) {
             for (i =0 ; i<4 ;i++)
             {
                 rtset_index = series[i].Rate & 0x7f;
                 rtset_index = (rtset_index & 070)>>3;  /* rate set based on number of stream rates*/
                 ant = an->selected_smart_antenna[rtset_index] ;//ni->rtset[rtset_index].selected_antenna;
                 smartAntenna |= (ant << (8*i)); /* antenna for all rate series */
             }
         } else {
             /* default antenna for non HT rates*/
             smartAntenna = (sc->sc_defant) |(sc->sc_defant << 8)| (sc->sc_defant << 16) | (sc->sc_defant << 24);
         }

     } else {
         smartAntenna = SMARTANT_INVALID; /* if smart antenna is not enabled */
     }
    
#if  UMAC_SUPPORT_SMARTANTENNA 
}
#endif
#else    
    smartAntenna = SMARTANT_INVALID; /* if smart antenna is not enabled */
#endif    
    /*
     * set dur_update_en for l-sig computation except for PS-Poll frames
     */
    ath_hal_set11n_ratescenario(ah, ds, lastds,
                                !bf->bf_ispspoll,
                                ctsrate,
                                ctsduration,
                                series, 4, flags, smartAntenna);
    
    if (sc->sc_config.ath_aggr_prot && flags)
        ath_hal_set11n_burstduration(ah, ds, sc->sc_config.ath_aggr_prot_duration);
}

#if ATH_SUPPORT_IQUE && ATH_SUPPORT_IQUE_EXT
/*
 * Calculate the accumulative latency of the frame due to sw retries which is updated per sw retry
 * and the hw retries from the series for the worst cast, and compare the latency with the threshold.
 * If the accumulative latency is larger than the threshold, decrease the hw retries from the
 * lowest rate to the highest rate step by step.
 */
void
ath_rateseries_update_retry(struct ath_softc *sc, struct ath_buf *bf, HAL_11N_RATE_SERIES *series, int retry_duration)
{
    u_int       duration, delay_gap;
    u_int32_t   adjust[4];
    int8_t      i, total_tries=0;

    /*
     * If this is a sw retry frame and the accumulative tx duration is larger than the
     * threshold of tx duration, this frame will be dropped.
     */
    if (bf->bf_txduration > sc->sc_retry_duration) {
        series[0].Tries = 1;
        series[1].Tries = 0;
        series[2].Tries = 0;
        series[3].Tries = 0;
        return ;
    }

    /*
     * Here 'duration' is the accumulative latency due to
     * sw retries (bf->bf_txduration in us) and the hw
     * retries (retry_duration in us).
     */
    duration = retry_duration + bf->bf_txduration;
    if (duration > sc->sc_retry_duration) {
        delay_gap = duration - sc->sc_retry_duration;
        for (i = 3; i >= 0; i --) {
            adjust[i] = (u_int32_t)delay_gap/series[i].PktDuration;

            /*
             * Check for some boundary conditions
             */
            if (delay_gap < adjust[i] * series[i].PktDuration) {
                adjust[i] --;
            }
            if (adjust[i] < 0) {
                adjust[i] = 0;
            }
            if (adjust[i] > series[i].Tries) {
                adjust[i] = series[i].Tries;
            }

            /*
             * Adjust the hw retries and update the delay_gap
             */
            series[i].Tries -= adjust[i];
            delay_gap -= adjust[i] * series[i].PktDuration;
            total_tries += series[i].Tries;
        }
    }
    if (total_tries == 0) {
        series[0].Tries = 1;
    }
    return;
}
#endif

#ifdef ATH_SUPERG_FF
int
ath_ff_check(ath_dev_t dev, 
             ath_node_t node, 
             int qnum,
             int check_qdepth, 
             u_int32_t txoplimit,
             u_int32_t framelen)
{
    struct ath_softc *sc = ATH_DEV_TO_SC(dev);
    struct ath_node *an = ATH_NODE(node);
    struct ath_txq *txq = &sc->sc_txq[qnum];
    u_int32_t txtime;

    if (sc->sc_curchan.channelFlags & (CHANNEL_HALF | CHANNEL_QUARTER)) {
        return AH_FALSE;
    }

    if (check_qdepth) {
        ATH_TXQ_LOCK(txq);
        if (txq->axq_depth < sc->sc_fftxqmin) {
            ATH_TXQ_UNLOCK(txq);
            return AH_FALSE;
        }
        ATH_TXQ_UNLOCK(txq);
    }

    /* if the 4msec limit is set on the channel, take it in to account */
    if(sc->sc_curchan.privFlags & CHANNEL_4MS_LIMIT)
        txoplimit = MIN(txoplimit, US_PER_4MS); 

    txtime = ath_hal_computetxtime(sc->sc_ah, sc->sc_currates, framelen,
                       an->an_prevdatarix, AH_FALSE);
    if (txoplimit != 0 && txtime > txoplimit) {
        DPRINTF(sc, ATH_DEBUG_XMIT | ATH_DEBUG_FF,
            "%s: FF TxOp violation\n", __func__);
        return AH_FALSE;
    }

    return AH_TRUE;

}
#endif

void
ath_txto_tasklet(struct ath_softc *sc)
{
    DPRINTF(sc, ATH_DEBUG_XMIT, "%s\n", __func__);

#if 0
    /* Notify CWM */
    ath_cwm_txtimeout(sc);
#endif
}

#if ATH_SUPPORT_VOWEXT
int
ath_deq_single_buf(struct ath_softc *sc, struct ath_txq *txq)
{
    ath_bufhead         bf_head;
    struct ath_buf      *bf = NULL;
    struct ath_atx_tid *tid;
    struct ath_atx_ac  *ac;
    int done = 0;

    /* Dequeue buf only if the no of bufs used by txq is > min resev bufs */
#if ATH_TX_BUF_FLOW_CNTL
    if(txq->axq_num_buf_used <= MIN_BUF_RESV) {
        return -ENOMEM;
    }
#endif

    TAILQ_FOREACH(ac, &txq->axq_acq, ac_qelem) {
          TAILQ_FOREACH(tid, &ac->tid_q, tid_qelem) {
            bf = TAILQ_LAST(&tid->buf_q, ath_bufhead_s);
            if (!bf || bf->bf_isretried) {
               break;   // can't remove from this tid
            } 
            else {
                done = 1;  // found the packet to remove
                break;
            }
          }
          if ( done )
              break;
    }

    if (done) {
        
	    TAILQ_REMOVE(&tid->buf_q, bf, bf_list);
	    if (tid->seq_next == 0 ) 
            tid->seq_next = IEEE80211_SEQ_MAX - 1;
	    else
            tid->seq_next--;

        if (TAILQ_EMPTY(&tid->buf_q))
        {
            /* No buffers associated to this tid.
               So remove this tid from tid_q and mark sched as FALSE */
            tid->sched = AH_FALSE;

            TAILQ_REMOVE (&ac->tid_q, tid, tid_qelem);
            if (TAILQ_EMPTY (&ac->tid_q))
            {
                /* No tds from this ac for scheduling.
                   So, remove this ac from axq_acq and mark sched as FALSE */
                ac->sched = AH_FALSE;
                TAILQ_REMOVE (&txq->axq_acq, ac, ac_qelem);
            }
        }

	    TAILQ_INIT(&bf_head);
	    TAILQ_INSERT_TAIL(&bf_head, bf, bf_list);
        
	    ATH_TXBUF_UNLOCK(sc);
        /* do not try to stop the queues here */
        sc->sc_stats.ast_tx_nobuf++;
        sc->sc_stats.ast_txq_nobuf[TID_TO_WME_AC(bf->bf_tidno)]++;

#ifdef ATH_SUPPORT_TxBF
	    ath_tx_complete_buf(sc, bf, &bf_head, 0, 0, 0);
#else
	    ath_tx_complete_buf(sc, bf, &bf_head, 0);
#endif
        /* update statistics */
	    ATH_TXBUF_LOCK(sc);
    
	    return 0;
    }
    else return -ENOMEM;
}
#endif
#ifdef AR_DEBUG
void ath_printtxbuf(struct ath_buf *bf, u_int8_t desc_has_status) 
{

}



#endif

#ifdef ATH_C3_WAR
int ath_c3_war_handler(void *arg)
{
    struct ath_softc *sc = (struct ath_softc *)arg;

    return 0;
}
#endif

int
ath_tx_get_rts_retrylimit(struct ath_softc *sc, struct ath_txq *txq)
{
    HAL_TXQ_INFO qi;

    ath_hal_gettxqueueprops(sc->sc_ah, txq->axq_qnum, &qi);

    return qi.tqi_shretry;
}
