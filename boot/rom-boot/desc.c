/************************************************************************/
/*  Copyright (c) 2000-2005  ZyDAS Technology Corporation               */
/*                                                                      */
/*  Module Name : desc.c                                                */
/*                                                                      */
/*  Abstract                                                            */
/*      This module contains DMA descriptors handle functions.          */
/*                                                                      */
/*  ROUTINES                                                            */
/*                                                                      */
/*      zfDmaInitDescriptor                                             */
/*      zfDmaGetPacket                                                  */
/*      zfDmaReclaimPacket                                              */
/*      zfDmaPutPacket                                                  */
/*                                                                      */
/*  NOTES                                                               */
/*      None                                                            */
/*                                                                      */
/************************************************************************/
#include "dt_defs.h"
//#include "string.h"
//#include "gv_extr.h"
#include "reg_defs.h"
//#include "uart_extr.h"
#include <osapi.h>
//#include <HIF_api.h>
//#include "HIF_usb.h"
#include <wasp_api.h>
#include <vdesc_api.h>
#include "desc.h"
#include <asm/addrspace.h>

/* Function prototypes */
//void zfDmaInitDescriptor(void);
struct zsDmaDesc* zfDmaGetPacket(struct zsDmaQueue* q);
void zfDmaReclaimPacket(struct zsDmaQueue* q, struct zsDmaDesc* desc);
void zfDmaPutPacket(struct zsDmaQueue* q, struct zsDmaDesc* desc);

/************************************************************************/
/*                                                                      */
/*    FUNCTION DESCRIPTION                  zfDmaGetPacket              */
/*      Get a completed packet with # descriptors. Return the first     */
/*      descriptor and pointer the head directly by lastAddr->nextAddr  */
/*                                                                      */
/*    ROUTINES CALLED                                                   */
/*                                                                      */
/*    INPUTS                                                            */
/*      struct zsDmaQueue* q                                            */
/*                                                                      */
/*    OUTPUTS                                                           */
/*      struct zsDmaDesc* desc                                          */
/*                                                                      */
/*    AUTHOR                                                            */
/*      Stephen Chen   ZyDAS Communication Corporation        2005.10   */
/*                                                                      */
/*    NOTES                                                             */
/*                                                                      */
/************************************************************************/
struct zsDmaDesc* zfDmaGetPacket(struct zsDmaQueue* q)
{
    struct zsDmaDesc* desc = NULL;

    if(q->head == q->terminator)
       return NULL;

    if (((q->head->status & ZM_OWN_BITS_MASK) == ZM_OWN_BITS_SW)
            || ((q->head->status & ZM_OWN_BITS_MASK) == ZM_OWN_BITS_SE))

    //if ( (((q->head->status & ZM_OWN_BITS_MASK) == ZM_OWN_BITS_SW) && ((u32_t)q != (u32_t)&zgDnQ))
    //        || (((q->head->status & ZM_OWN_BITS_MASK) == ZM_OWN_BITS_SE) && ((u32_t)q == (u32_t)&zgDnQ)) )

    {
        desc = q->head;

        q->head = get_next_addr(get_last_addr(desc));
    }
    return desc;
}

/************************************************************************/
/*                                                                      */
/*    FUNCTION DESCRIPTION                  zfDmaReclaimPacket         	*/
/*      Free descriptor.                                                */
/*	Exchange the terminator and the first descriptor of the packet  */
/*	for hardware ascy... .                                          */
/*                                                                      */
/*    ROUTINES CALLED                                                   */
/*                                                                      */
/*    INPUTS                                                            */
/*      struct zsDmaQueue* q                                            */
/*	struct zsDmaDesc* desc                                          */
/*                                                                      */
/*    OUTPUTS                                                           */
/*                                                                      */
/*    AUTHOR                                                            */
/*      Stephen Chen   ZyDAS Communication Corporation        2005.10   */
/*                                                                      */
/*    NOTES                                                             */
/*                                                                      */
/************************************************************************/
void zfDmaReclaimPacket(struct zsDmaQueue* q, struct zsDmaDesc* desc)
{
    struct zsDmaDesc* tmpDesc;
    struct zsDmaDesc tdesc;
    VDESC *vdesc;
    VDESC *vtermdesc;
    //int tmp;
    //u8_t *tmpAddr;

    /* 1. Set OWN bit to 1 for all TDs to be added, clear ctrl and size */
    tmpDesc = KSEG1ADDR(desc);
    while (1)
    {
        tmpDesc->status = ZM_OWN_BITS_HW;
        tmpDesc->ctrl = 0;
        tmpDesc->totalLen = 0;

#if ZM_FM_LOOPBACK == 1
        vdesc = VDESC_HW_TO_VDESC(tmpDesc);
        tmpDesc->dataSize = vdesc->buf_size;
#endif

        //A_PRINTF("tmpDesc->dataSize = %d\n", (u32_t)tmpDesc->dataSize);

        /* TODO : Exception handle */
        if (get_last_addr(desc) == tmpDesc)
        {
            break;
        }
        tmpDesc = get_next_addr(tmpDesc);
    }

    /* 3. Next address of Last TD to be added = first TD */
    set_next_addr(get_last_addr(desc), desc);

    /* 2. Copy first TD to be added to TTD */
    //zfMemoryCopyInWord(&tdesc, desc, sizeof(struct zsDmaDesc));
    A_MEMCPY(&tdesc, desc, sizeof(struct zsDmaDesc));

    /* 4. set first TD OWN bit to 0 */
    desc->status &= (~ZM_OWN_BITS_MASK);

    /* 5. Copy TTD to last TD */
    tdesc.status &= (~ZM_OWN_BITS_MASK);

    vdesc = VDESC_HW_TO_VDESC(desc);
    vtermdesc = VDESC_HW_TO_VDESC(q->terminator);

    VDESC_swap_vdesc(vtermdesc, vdesc);

    //zfMemoryCopyInWord((void*)q->terminator, (void*)&tdesc, sizeof(struct zsDmaDesc));
    A_MEMCPY((void*)q->terminator, (void*)&tdesc, sizeof(struct zsDmaDesc));

    //desc->dataSize = 0;
    //desc->dataAddr = 0;
    q->terminator->status |= ZM_OWN_BITS_HW;

    /* Update terminator pointer */
    q->terminator = desc;
}

/************************************************************************/
/*                                                                      */
/*    FUNCTION DESCRIPTION                  zfDmaPutPacket              */
/*      Put a complete packet into the tail of the Queue q.             */
/*      Exchange the terminator and the first descriptor of the packet  */
/*      for hardware ascy... .                                          */
/*                                                                      */
/*    ROUTINES CALLED                                                   */
/*                                                                      */
/*    INPUTS                                                            */
/*      struct zsDmaQueue* q                                            */
/*      struct zsDmaDesc* desc                                          */
/*                                                                      */
/*    OUTPUTS                                                           */
/*                                                                      */
/*    AUTHOR                                                            */
/*      Stephen Chen   ZyDAS Communication Corporation        2005.10   */
/*                                                                      */
/*    NOTES                                                             */
/*                                                                      */
/************************************************************************/
void zfDmaPutPacket(struct zsDmaQueue* q, struct zsDmaDesc* desc)
{
    struct zsDmaDesc* tmpDesc;
    struct zsDmaDesc tdesc;
    VDESC *vdesc;
    VDESC *vtermdesc;
    //u32_t tmp;
    //u8_t *tmpAddr;


    /* 1. Set OWN bit to 1 for all TDs to be added */
    tmpDesc = KSEG1ADDR(desc);
    while (1)
    {
        tmpDesc->status =
                ((tmpDesc->status & (~ZM_OWN_BITS_MASK)) | ZM_OWN_BITS_HW);
	//A_PRINTF("%s: setting ownership %p\n", __func__, tmpDesc);
        /* TODO : Exception handle */

        if (get_last_addr(desc) == tmpDesc)
        {
            break;
        }
        tmpDesc = get_next_addr(tmpDesc);
    }

    /* 3. Next address of Last TD to be added = first TD */
    set_next_addr(get_last_addr(desc), desc);

    /* If there is only one descriptor, update pointer of last descriptor */
    if (get_last_addr(desc) == desc)
    {
        set_last_addr(desc, q->terminator);
    }

    /* 2. Copy first TD to be added to TTD */
    A_MEMCPY(&tdesc, desc, sizeof(struct zsDmaDesc));
    //tdesc.dataSize = 0;
    //tdesc.dataAddr = 0;

    /* 4. set first TD OWN bit to 0 */
    desc->status &= (~ZM_OWN_BITS_MASK);

    /* 5. Copy TTD to last TD */
    tdesc.status &= (~ZM_OWN_BITS_MASK);

    vdesc = VDESC_HW_TO_VDESC(desc);
    vtermdesc = VDESC_HW_TO_VDESC(q->terminator);

    VDESC_swap_vdesc(vtermdesc, vdesc);

    A_MEMCPY((void*)q->terminator, (void*)&tdesc, sizeof(struct zsDmaDesc));
    q->terminator->status |= ZM_OWN_BITS_HW;
    /* Update terminator pointer */
    q->terminator = desc;
}

