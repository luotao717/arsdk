#include <wasp_api.h>
#include <dt_defs.h>
#include <vbuf_api.h>
#include <dma_lib.h>
#include <dma_engine_api.h>
#include <asm/addrspace.h>

/***********************Constants***************************/

/**
 * @brief Descriptor specific bitmaps
 */
enum __dma_desc_status{
    DMA_STATUS_OWN_DRV = 0x0,
    DMA_STATUS_OWN_DMA = 0x1,
    DMA_STATUS_OWN_MSK = 0x3
};

enum __dma_bit_op{
    DMA_BIT_CLEAR  = 0x0,
    DMA_BIT_SET    = 0x1
};

enum __dma_burst_size{
    DMA_BURST_4W   = 0x00,
    DMA_BURTS_8W   = 0x01,
    DMA_BURST_16W  = 0x10
};
enum __dma_byte_swap{
    DMA_BYTE_SWAP_OFF = 0x00,
    DMA_BYTE_SWAP_ON  = 0x01
};
/**
*  @brief Interrupt status bits
 */
typedef enum __dma_intr_bits{
    DMA_INTR_TX1_END   = (1 << 25),/*TX1 reached the end or Under run*/
    DMA_INTR_TX0_END   = (1 << 24),/*TX0 reached the end or Under run*/
    DMA_INTR_TX1_DONE  = (1 << 17),/*TX1 has transmitted a packet*/
    DMA_INTR_TX0_DONE  = (1 << 16),/*TX1 has transmitted a packet*/
    DMA_INTR_RX3_END   = (1 << 11),/*RX3 reached the end or Under run*/
    DMA_INTR_RX2_END   = (1 << 10),/*RX2 reached the end or Under run*/
    DMA_INTR_RX1_END   = (1 << 9), /*RX1 reached the end or Under run*/
    DMA_INTR_RX0_END   = (1 << 8), /*RX0 reached the end or Under run*/
    DMA_INTR_RX3_DONE  = (1 << 3), /*RX3 received a packet*/
    DMA_INTR_RX2_DONE  = (1 << 2), /*RX2 received a packet*/
    DMA_INTR_RX1_DONE  = (1 << 1), /*RX1 received a packet*/
    DMA_INTR_RX0_DONE  = 1,        /*RX0 received a packet*/
}__dma_intr_bits_t;
/**
 * @brief Base addresses for various HIF
 */
typedef enum __dma_base_off{
    DMA_BASE_OFF_HST  = 0xb8127000,
    //DMA_BASE_OFF_GMAC = 0x00054000,
    DMA_BASE_OFF_PCI  = DMA_BASE_OFF_HST,
    DMA_BASE_OFF_PCIE = DMA_BASE_OFF_HST
}__dma_base_off_t;
/**
 * @brief Engine offset to add for per engine register reads or
 *        writes
 */
typedef enum __dma_eng_off{
    DMA_ENG_OFF_RX0 = 0x800,
    DMA_ENG_OFF_RX1 = 0x900,
    DMA_ENG_OFF_RX2 = 0xa00,
    DMA_ENG_OFF_RX3 = 0xb00,
    DMA_ENG_OFF_TX0 = 0xc00,
    DMA_ENG_OFF_TX1 = 0xd00
}__dma_eng_off_t;
/**
 *@brief DMA registers
 */
typedef enum __dma_reg_off{
    /**
     * Common or Non Engine specific
     */
    DMA_REG_IFTYPE   = 0x00,/*XXX*/
    DMA_REG_ISR      = 0x00,/*Interrupt Status Register*/
    DMA_REG_IMR      = 0x04,/*Interrupt Mask Register*/
    /**
     * Transmit
     */
    DMA_REG_TXDESC   = 0x00,/*TX DP*/
    DMA_REG_TXSTART  = 0x04,/*TX start*/
    DMA_REG_INTRLIM  = 0x08,/*TX Interrupt limit*/
    DMA_REG_TXBURST  = 0x0c,/*TX Burst Size*/
    DMA_REG_TXSWAP   = 0x18,
    /**
     * Receive
     */
    DMA_REG_RXDESC   = 0x00,/*RX DP*/
    DMA_REG_RXSTART  = 0x04,/*RX Start*/
    DMA_REG_RXBURST  = 0x08,/*RX Burst Size*/
    DMA_REG_RXPKTOFF = 0x0c,/*RX Packet Offset*/
    DMA_REG_RXSWAP   = 0x1c
}__dma_reg_off_t;

/*******************************Data types******************************/

typedef struct zsDmaDesc    __dma_desc_t;

typedef struct zsDmaQueue   __dma_rxq_t;

typedef struct zsTxDmaQueue  __dma_txq_t;

/**
 * @brief Register Address
 */
typedef struct __dma_reg_addr{
    __dma_base_off_t     base;/*Base address, Fixed*/
    __dma_eng_off_t      eng;/*Engine offset, Fixed*/
    __dma_reg_off_t      reg;/*Register offset*/
}__dma_reg_addr_t;

typedef struct __dma_intr{
    __dma_intr_bits_t  end;/*Under run status,Fixed*/
    __dma_intr_bits_t  done;/*Packet done status, Fixed*/
}__dma_intr_t;
/**
 * @brief DMA engine's Queue
 */
typedef struct __dma_eng_q{
   __dma_reg_addr_t     addr;
   __dma_intr_t         intr;/*Interrupt status/mask for this engine*/
   __dma_rxq_t          rxq;
   __dma_txq_t          txq;
}__dma_eng_q_t;

/***********************Defines*****************************/

#define DMA_ADDR_INIT(_eng)     {   \
    .base = DMA_BASE_OFF_HST,       \
    .eng  = DMA_ENG_OFF_##_eng      \
}
#define DMA_INTR_INIT(_eng){      \
    .end  = DMA_INTR_##_eng##_END, \
    .done = DMA_INTR_##_eng##_DONE \
}
/**
 * @brief check if the val doesn't lie between the low & high of
 *        the engine numbers
 */
#define DMA_ENG_CHECK(_val, _low, _high)    \
    ((_val) < DMA_ENGINE_##_low || (_val) > DMA_ENGINE_##_high)


/********************************Globals*************************************/

__dma_eng_q_t    eng_q[DMA_ENGINE_MAX] = {
    {.addr = DMA_ADDR_INIT(RX0), .intr = DMA_INTR_INIT(RX0)},
    {.addr = DMA_ADDR_INIT(RX1), .intr = DMA_INTR_INIT(RX1)},
    {.addr = DMA_ADDR_INIT(RX2), .intr = DMA_INTR_INIT(RX2)},
    {.addr = DMA_ADDR_INIT(RX3), .intr = DMA_INTR_INIT(RX3)},
    {.addr = DMA_ADDR_INIT(TX0), .intr = DMA_INTR_INIT(TX0)},
    {.addr = DMA_ADDR_INIT(TX1), .intr = DMA_INTR_INIT(TX1)},
};

/**********************************API's*************************************/

/**
 * @brief Read the register
 *
 * @param addr
 *
 * @return a_uint32_t
 */
a_uint32_t
__dma_reg_read(a_uint32_t addr)
{
    return *((volatile a_uint32_t *)addr);
}
/**
 * @brief Write into the register
 *
 * @param addr
 * @param val
 */
void
__dma_reg_write(a_uint32_t addr, a_uint32_t val)
{
    *((volatile a_uint32_t *)addr) = val;
}
/**
 * @brief Set the base address
 *
 * @param eng_no
 * @param if_type
 */
void
__dma_set_base(dma_engine_t  eng_no, dma_iftype_t if_type)
{
    switch (if_type) {
#if 0
    case DMA_IF_GMAC:
        eng_q[eng_no].addr.base = DMA_BASE_OFF_GMAC;
        break;
#endif
    case DMA_IF_PCI:
        eng_q[eng_no].addr.base = DMA_BASE_OFF_PCI;
        break;
    case DMA_IF_PCIE:
        eng_q[eng_no].addr.base = DMA_BASE_OFF_PCIE;
        break;
    default:
        return;
    }
}
/**
 * @brief setup the interrupts for the specified engine number
 *
 * @param eng_no
 */
void
__dma_set_intr(dma_engine_t  eng_no)
{
    a_uint32_t       addr, intr_mask = 0;

    addr  = eng_q[eng_no].addr.base;

    intr_mask    = __dma_reg_read(addr + DMA_REG_IMR);
    intr_mask   |= eng_q[eng_no].intr.done;

    __dma_reg_write(addr + DMA_REG_IMR, intr_mask);

}
/**
 *
 * @param if_type
 *
 * @return a_uint32_t interrupt status mask
 */
a_uint32_t
__dma_intr_status(dma_iftype_t  if_type)
{
    __dma_base_off_t  base;

    switch (if_type) {
#if 0
    case DMA_IF_GMAC:
        base = DMA_BASE_OFF_GMAC;
        break;
#endif
    case DMA_IF_PCI:
        base = DMA_BASE_OFF_PCI;
        break;
    case DMA_IF_PCIE:
        base = DMA_BASE_OFF_PCIE;
        break;
    default:
        return A_STATUS_EINVAL;
    }

    return __dma_reg_read(base + DMA_REG_ISR);
}

/**
 * @brief init the Transmit queue
 *
 * @param eng_no
 * @param if_type
 *
 * @return a_status_t
 */
a_status_t
__dma_lib_tx_init(dma_engine_t  eng_no, dma_iftype_t  if_type)
{
    __dma_desc_t  *head = NULL;
    a_uint32_t     addr, *p;

    if(DMA_ENG_CHECK(eng_no, TX0, TX1))
        return A_STATUS_EINVAL;

    DMA_Engine_init_tx_queue(&eng_q[eng_no].txq);

    __dma_set_base(eng_no, if_type);

    addr  = eng_q[eng_no].addr.base + eng_q[eng_no].addr.eng;

    head = eng_q[eng_no].txq.head;

    p = (a_uint32_t *)head;
//	A_PRINTF("Tx[0x%08x]: %p 0x%08x\n", addr + DMA_REG_TXDESC, head, PHYSADDR((a_uint32_t)head));
    __dma_reg_write(addr + DMA_REG_TXDESC, PHYSADDR((a_uint32_t)head));
    __dma_reg_write(addr + DMA_REG_TXBURST, DMA_BURST_4W);
    __dma_reg_write(addr + DMA_REG_TXSWAP, DMA_BYTE_SWAP_ON);
//	A_PRINTF("Tx[%p]: 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n", p, p[0], p[1], p[2], p[3], p[4]);

    return A_STATUS_OK;
}

void
__dma_lib_rx_config(dma_engine_t   eng_no, a_uint16_t   num_desc,
                    a_uint16_t     gran)
{
    __dma_desc_t     *desc = NULL;
    a_uint32_t       addr = 0;
    a_uint32_t       *p;

    /**
     * Allocate the Receive Queue
     */
    DMA_Engine_config_rx_queue(&eng_q[eng_no].rxq, num_desc, gran);

    desc  = eng_q[eng_no].rxq.head;
    addr  = eng_q[eng_no].addr.base + eng_q[eng_no].addr.eng;
    p = (a_uint32_t *)addr;
    /**
     * Update RX queue head in the H/W, set the burst & say go
     */
//	A_PRINTF("Rx[0x%08x]: %p 0x%08x\n", addr + DMA_REG_RXDESC, desc, PHYSADDR((a_uint32_t)desc));
    __dma_reg_write(addr + DMA_REG_RXDESC, PHYSADDR((a_uint32_t)desc));
    __dma_reg_write(addr + DMA_REG_RXBURST, DMA_BURST_4W);
    __dma_reg_write(addr + DMA_REG_RXSWAP,  DMA_BYTE_SWAP_ON);
    __dma_reg_write(addr + DMA_REG_RXSTART, DMA_BIT_SET);
//	A_PRINTF("Rx[%p]: 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n", p, p[0], p[1], p[2], p[3], p[4]);

}

/**
 * @brief Initialize the DMA engine
 *
 * @param rx_desc
 *
 * @return a_status_t
 */
a_status_t
__dma_lib_rx_init(dma_engine_t   eng_no, dma_iftype_t     if_type)
{
    if(DMA_ENG_CHECK(eng_no, RX0, RX3))
        return A_STATUS_EINVAL;

    /**
     * XXX:The init can be called multiple times to setup different
     * geometries of descriptors
     */
    DMA_Engine_init_rx_queue(&eng_q[eng_no].rxq);

    __dma_set_base(eng_no, if_type);
//    __dma_set_intr(eng_no);
    return A_STATUS_OK;
}
/**
 * @brief Transmit VBUF for the specified engine number
 *
 * @param VBUF
 *
 * @return a_status_t
 */
a_status_t
__dma_hard_xmit(dma_engine_t eng_no, VBUF *vbuf)
{
    a_uint32_t addr;

    addr = eng_q[eng_no].addr.base + eng_q[eng_no].addr.eng;

    DMA_Engine_xmit_buf(&eng_q[eng_no].txq, vbuf);
    /**
     * Say go
     */
//	A_PRINTF("Tx: Setting start at 0x%x 0x%x 0x%x\n", addr + DMA_REG_TXSTART, *((unsigned *)(addr + DMA_REG_TXSTART)), *(unsigned *)0xb8127c00);
    __dma_reg_write(addr + DMA_REG_TXSTART, DMA_BIT_SET);
    return A_STATUS_OK;
}
/**
 * @brief return a VBUF for the specified engine number
 *
 * @param eng_no
 *
 * @return VBUF*
 */
VBUF *
__dma_reap_xmitted(dma_engine_t eng_no)
{
    return DMA_Engine_reap_xmited_buf(&eng_q[eng_no].txq);
}
/**
 * @brief flush all xmitted & to be xmitted (if you have the
 *        window) dudes from H/W
 *
 * @param eng_no
 */
void
__dma_flush_xmit(dma_engine_t  eng_no)
{
    a_uint32_t addr;
    __dma_desc_t  *desc, *term;

    addr = eng_q[eng_no].addr.base + eng_q[eng_no].addr.eng;

    desc = eng_q[eng_no].txq.head;
    term = eng_q[eng_no].txq.terminator;

//	A_PRINTF("%s: called\n");

    /**
     * XXX: I don't know how to kick the all dudes out, Ideally
     * there should be a DMA reset button (the red one)
     */
    __dma_reg_write(addr + DMA_REG_TXSTART, DMA_BIT_CLEAR);
    __dma_reg_write(addr + DMA_REG_TXDESC, PHYSADDR((a_uint32_t)term));

    /**
     * Make the H/W queue ready for TX reap
     */
    for(;desc != term; desc = get_next_addr(desc))
        desc->status = DMA_STATUS_OWN_DRV;

//    DMA_Engine_flush_xmit(&eng_q[eng_no].txq);
}
/**
 * @brief check if there are xmitted vbufs (dudes) hanging
 *        around
 *
 * @param eng_no
 *
 * @return a_bool_t
 */
a_status_t
__dma_xmit_done(dma_engine_t  eng_no)
{
    if(DMA_ENG_CHECK(eng_no, TX0, TX1))
        return 0;

    return DMA_Engine_has_compl_packets((struct zsDmaQueue *)
		    			&(eng_q[eng_no].txq));
}
/**
 * @brief Reap VBUF's from the specified engine number
 *
 * @param eng
 *
 * @return VBUF*
 */
VBUF *
__dma_reap_recv(dma_engine_t  eng)
{
    return DMA_Engine_reap_recv_buf(&eng_q[eng].rxq);
}
/**
 * @brief return to source, put the vbuf back into the queue, In
 *        case the Engine is stopped so start it again
 *
 * @param eng_no
 * @param vbuf
 */
void
__dma_return_recv(dma_engine_t  eng_no, VBUF *vbuf)
{
    a_uint32_t addr;

    addr = eng_q[eng_no].addr.base + eng_q[eng_no].addr.eng;

    DMA_Engine_return_recv_buf(&eng_q[eng_no].rxq, vbuf);

	//A_PRINTF("%s: 0x%x\n", __func__, addr + DMA_REG_RXSTART);
    __dma_reg_write(addr + DMA_REG_RXSTART, DMA_BIT_SET);
}
/**
 * @brief check if there are freshly arrived vbufs (dudes)
 *
 * @param eng_no
 *
 * @return a_bool_t
 */
a_status_t
__dma_recv_pkt(dma_engine_t  eng_no)
{
    if(DMA_ENG_CHECK(eng_no, RX0, RX3))
        return 0;

    return DMA_Engine_has_compl_packets(&eng_q[eng_no].rxq);
}

void
dma_lib_module_install(struct dma_lib_api *apis)
{
    apis->intr_status  = __dma_intr_status;
    apis->tx_init      = __dma_lib_tx_init;
    apis->rx_init      = __dma_lib_rx_init;
    apis->rx_config    = __dma_lib_rx_config;
    apis->hard_xmit    = __dma_hard_xmit;
    apis->flush_xmit   = __dma_flush_xmit;
    apis->xmit_done    = __dma_xmit_done;
    apis->reap_recv    = __dma_reap_recv;
    apis->reap_xmitted = __dma_reap_xmitted;
    apis->return_recv  = __dma_return_recv;
    apis->recv_pkt     = __dma_recv_pkt;
}
