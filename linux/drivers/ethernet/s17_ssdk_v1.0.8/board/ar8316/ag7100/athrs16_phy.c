/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright © 2007 Atheros Communications, Inc.,  All Rights Reserved.
 */

/*
 * Manage the atheros ethernet PHY.
 *
 * All definitions in this file are operating system independent!
 */

#include <linux/config.h>
#include <linux/types.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/delay.h>
#include "ag7100_phy.h"
#include "ag7100.h"

#include "build_mode.h"
#include "ssdk_init.h"
#include "garuda_reg_access.h"

/* PHY selections and access functions */

extern void* ssdk_mdio_set;
extern void* ssdk_mdio_get;
extern void* ssdk_hdr_reg_set;
extern void* ssdk_hdr_reg_get;
static struct semaphore	hdr_sem;

typedef enum {
    PHY_SRCPORT_INFO, 
    PHY_PORTINFO_SIZE,
} PHY_CAP_TYPE;

typedef enum {
    PHY_SRCPORT_NONE,
    PHY_SRCPORT_VLANTAG, 
    PHY_SRCPORT_TRAILER,
} PHY_SRCPORT_TYPE;

#define DRV_LOG(DBG_SW, X0, X1, X2, X3, X4, X5, X6)
#define DRV_MSG(x,a,b,c,d,e,f)
#define DRV_PRINT(DBG_SW,X)

#define ATHR_LAN_PORT_VLAN          1
#define ATHR_WAN_PORT_VLAN          2


/*depend on connection between cpu mac and s16 mac*/
#if defined (CONFIG_PORT0_AS_SWITCH)
#define ENET_UNIT_LAN 0  
#define ENET_UNIT_WAN 1
#define CFG_BOARD_AP96 1
#else
#define ENET_UNIT_LAN 1  
#define ENET_UNIT_WAN 0
#define CFG_BOARD_PB45 1
#endif

#define TRUE    1
#define FALSE   0

#define ATHR_PHY0_ADDR   0x0
#define ATHR_PHY1_ADDR   0x1
#define ATHR_PHY2_ADDR   0x2
#define ATHR_PHY3_ADDR   0x3
#define ATHR_PHY4_ADDR   0x4
#define ATHR_IND_PHY 4

#define MODULE_NAME "ATHRS16"

/*
 * Track per-PHY port information.
 */
typedef struct {
    BOOL   isEnetPort;       /* normal enet port */
    BOOL   isPhyAlive;       /* last known state of link */
    int    ethUnit;          /* MAC associated with this phy port */
    uint32_t phyBase;
    uint32_t phyAddr;          /* PHY registers associated with this phy port */
    uint32_t VLANTableSetting; /* Value to be written to VLAN table */
} athrPhyInfo_t;

/*
 * Per-PHY information, indexed by PHY unit number.
 */
static athrPhyInfo_t athrPhyInfo[] = {
    {TRUE,   /* phy port 0 -- LAN port 0 */
     FALSE,
     ENET_UNIT_LAN,
     0,
     ATHR_PHY0_ADDR,
     ATHR_LAN_PORT_VLAN
    },

    {TRUE,   /* phy port 1 -- LAN port 1 */
     FALSE,
     ENET_UNIT_LAN,
     0,
     ATHR_PHY1_ADDR,
     ATHR_LAN_PORT_VLAN
    },

    {TRUE,   /* phy port 2 -- LAN port 2 */
     FALSE,
     ENET_UNIT_LAN,
     0,
     ATHR_PHY2_ADDR, 
     ATHR_LAN_PORT_VLAN
    },

    {TRUE,   /* phy port 3 -- LAN port 3 */
     FALSE,
     ENET_UNIT_LAN,
     0,
     ATHR_PHY3_ADDR, 
     ATHR_LAN_PORT_VLAN
    },

    {TRUE,   /* phy port 4 -- WAN port or LAN port 4 */
     FALSE,
     ENET_UNIT_WAN,
     0,
     ATHR_PHY4_ADDR, 
     ATHR_LAN_PORT_VLAN   /* Send to all ports */
    },
    
    {FALSE,  /* phy port 5 -- CPU port (no RJ45 connector) */
     TRUE,
     ENET_UNIT_LAN,
     0,
     0x00, 
     ATHR_LAN_PORT_VLAN    /* Send to all ports */
    },
};

static uint8_t athr16_init_flag = 0;
static cmd_resp_t cmd_resp;
static DECLARE_WAIT_QUEUE_HEAD (hd_conf_wait);
static int wait_flag = 0;
static ag7100_mac_t *ag7100_macs[2];
static atomic_t seqcnt = ATOMIC_INIT(0);

//#define ATHR_PHY_MAX (sizeof(ipPhyInfo) / sizeof(ipPhyInfo[0]))
#define ATHR_PHY_MAX 5

/* Range of valid PHY IDs is [MIN..MAX] */
#define ATHR_ID_MIN 0
#define ATHR_ID_MAX (ATHR_PHY_MAX-1)

/* Convenience macros to access myPhyInfo */
#define ATHR_IS_ENET_PORT(phyUnit) (athrPhyInfo[phyUnit].isEnetPort)
#define ATHR_IS_PHY_ALIVE(phyUnit) (athrPhyInfo[phyUnit].isPhyAlive)
#define ATHR_ETHUNIT(phyUnit) (athrPhyInfo[phyUnit].ethUnit)
#define ATHR_PHYBASE(phyUnit) (athrPhyInfo[phyUnit].phyBase)
#define ATHR_PHYADDR(phyUnit) (athrPhyInfo[phyUnit].phyAddr)
#define ATHR_VLAN_TABLE_SETTING(phyUnit) (athrPhyInfo[phyUnit].VLANTableSetting)


#define ATHR_IS_ETHUNIT(phyUnit, ethUnit) \
            (ATHR_IS_ENET_PORT(phyUnit) &&        \
            ATHR_ETHUNIT(phyUnit) == (ethUnit))

#define ATHR_IS_WAN_PORT(phyUnit) (!(ATHR_ETHUNIT(phyUnit)==ENET_UNIT_LAN))
            

sw_error_t athrs16_header_write(uint32_t dev_id, uint32_t reg_addr, uint8_t *reg_data, uint32_t len)
{
    athrs16_reg_write( reg_addr, *(uint32_t *)reg_data);
    return 0;
}

sw_error_t athrs16_header_read(uint32_t dev_id, uint32_t reg_addr, uint8_t *reg_data, uint32_t len)
{    
	  uint32_t data;
    
    data = athrs16_reg_read(reg_addr);
    *(uint32_t*)reg_data = data;
    return 0;
}

sw_error_t athrs16_phy_write(a_uint32_t dev_id, a_uint32_t phy_addr, a_uint32_t reg, a_uint16_t data)
{
    phy_reg_write(0, phy_addr, reg, data);

    return 0;
}

sw_error_t athrs16_phy_read(a_uint32_t dev_id, a_uint32_t phy_addr, a_uint32_t reg, a_uint16_t *data)
{    
    *data = phy_reg_read(0, phy_addr, reg);

    return 0;
}

void phy_mode_setup(void) 
{
#ifdef S16_VER10
    printk("phy_mode_setup\n");

    /*work around for phy4 rgmii mode*/
    phy_reg_write(ATHR_PHYBASE(ATHR_IND_PHY), ATHR_PHYADDR(ATHR_IND_PHY), 29, 18);     
    phy_reg_write(ATHR_PHYBASE(ATHR_IND_PHY), ATHR_PHYADDR(ATHR_IND_PHY), 30, 0x480c);    

    /*rx delay*/ 
    phy_reg_write(ATHR_PHYBASE(ATHR_IND_PHY), ATHR_PHYADDR(ATHR_IND_PHY), 29, 0);     
    phy_reg_write(ATHR_PHYBASE(ATHR_IND_PHY), ATHR_PHYADDR(ATHR_IND_PHY), 30, 0x824e);  

    /*tx delay*/ 
    phy_reg_write(ATHR_PHYBASE(ATHR_IND_PHY), ATHR_PHYADDR(ATHR_IND_PHY), 29, 5);     
    phy_reg_write(ATHR_PHYBASE(ATHR_IND_PHY), ATHR_PHYADDR(ATHR_IND_PHY), 30, 0x3d47);    
#endif
}

/*test : if no header support in boot loader*/
void
athrs16_mdio_write(uint32_t reg_addr, uint32_t reg_val)
{
    uint32_t reg_word_addr;
    uint32_t phy_addr;
    uint16_t phy_val;
    uint8_t phy_reg;

    /* change reg_addr to 16-bit word address, 32-bit aligned */
    reg_word_addr = (reg_addr & 0xfffffffc) >> 1;

    /* configure register high address */
    phy_addr = 0x18;
    phy_reg = 0x0;
    phy_val = (uint16_t) ((reg_word_addr >> 8) & 0x1ff);  /* bit16-8 of reg address */
    phy_reg_write(0, phy_addr, phy_reg, phy_val);

    /* For some registers such as ARL and VLAN, since they include BUSY bit */
    /* in lower address, we should write the higher 16-bit register then the */
    /* lower one */

    /* read register in higher address */
    reg_word_addr++;
    phy_addr = 0x10 | ((reg_word_addr >> 5) & 0x7); /* bit7-5 of reg address */
    phy_reg = (uint8_t) (reg_word_addr & 0x1f);   /* bit4-0 of reg address */
    phy_val = (uint16_t) ((reg_val >> 16) & 0xffff);
    phy_reg_write(0, phy_addr, phy_reg, phy_val);

    /* write register in lower address */
    reg_word_addr--;
    phy_addr = 0x10 | ((reg_word_addr >> 5) & 0x7); /* bit7-5 of reg address */
    phy_reg = (uint8_t) (reg_word_addr & 0x1f);   /* bit4-0 of reg address */
    phy_val = (uint16_t) (reg_val & 0xffff);
    phy_reg_write(0, phy_addr, phy_reg, phy_val); 
}

uint32_t
athrs16_mdio_read(uint32_t reg_addr) 
{
    uint32_t reg_word_addr;
    uint32_t phy_addr, tmp_val, reg_val;
    uint16_t phy_val;
    uint8_t phy_reg;

    /* change reg_addr to 16-bit word address, 32-bit aligned */
    reg_word_addr = (reg_addr & 0xfffffffc) >> 1;

    /* configure register high address */
    phy_addr = 0x18;
    phy_reg = 0x0;
    phy_val = (uint16_t) ((reg_word_addr >> 8) & 0x1ff);  /* bit16-8 of reg address */
    phy_reg_write(0, phy_addr, phy_reg, phy_val);

    /* For some registers such as MIBs, since it is read/clear, we should */
    /* read the lower 16-bit register then the higher one */

    /* read register in lower address */
    phy_addr = 0x10 | ((reg_word_addr >> 5) & 0x7); /* bit7-5 of reg address */
    phy_reg = (uint8_t) (reg_word_addr & 0x1f);   /* bit4-0 of reg address */
    reg_val = (uint32_t) phy_reg_read(0, phy_addr, phy_reg);

    /* read register in higher address */
    reg_word_addr++;
    phy_addr = 0x10 | ((reg_word_addr >> 5) & 0x7); /* bit7-5 of reg address */
    phy_reg = (uint8_t) (reg_word_addr & 0x1f);   /* bit4-0 of reg address */
    tmp_val = (uint32_t) phy_reg_read(0, phy_addr, phy_reg);
    reg_val |= (tmp_val << 16);

    return reg_val;   
  
}

void header_config_prepare(void)
{
#ifdef HEADER_EN
    printk("%s: set port0 header enable\n", __func__);
    athrs16_mdio_write(0x104, athrs16_mdio_read(0x104)|0x800);
#endif

#ifdef HEADER_REG_CONF
    printk("%s: set cpuport enable\n", __func__);
    athrs16_mdio_write(0x78, athrs16_mdio_read(0x78)|0x100);
#endif   
    printk("%s: set port0 enable \n", __func__);
    athrs16_mdio_write(0x100, 0x7e);
    
    printk("%s: set power on strip:0x012E1bea \n", __func__);
    athrs16_mdio_write(0x8, 0x012E1bea);
}

void athrs16_reg_init()
{
    /* if using header for register configuration, we have to     */
    /* configure s16 register after frame transmission is enabled */
    if (athr16_init_flag)
        return;
    init_MUTEX(&hdr_sem);
    header_config_prepare();

#ifdef FULL_FEATURE
{
    ssdk_init_cfg cfg;
    garuda_init_spec_cfg chip_spec_cfg;

    memset(&cfg, 0, sizeof(ssdk_init_cfg));
    memset(&chip_spec_cfg, 0, sizeof(garuda_init_spec_cfg));

    cfg.cpu_mode = HSL_CPU_2;
    cfg.reg_mode = HSL_MDIO;
    //cfg.reg_mode = HSL_HEADER;
    cfg.nl_prot  = 30;
    cfg.chip_type=CHIP_UNSPECIFIED;
    cfg.chip_spec_cfg = &chip_spec_cfg;
    cfg.reg_func.mdio_set = athrs16_phy_write;
    cfg.reg_func.mdio_get = athrs16_phy_read;
    cfg.reg_func.header_reg_set = athrs16_header_write;
    cfg.reg_func.header_reg_get = athrs16_header_read;

    int rv = ssdk_init(0, &cfg);
    if(rv == 0)
        printk("\n########ssdk_init complete success.##########\n");
    else
        printk("\n########ssdk_init complete fail! [%d]##########\n", rv);
}
#endif
    /*Power on strip mode setup*/
#if defined CFG_BOARD_PB45
    athrs16_reg_write(0x208, 0x2fd0001);  /*tx delay*/   
    athrs16_reg_write(0x108, 0x2be0001);  /*mac0 rgmii mode*/ 
#elif defined CFG_BOARD_AP96
    athrs16_reg_write(0x8, 0x012e1bea);
#endif
    
    athrs16_reg_write(0x100, 0x7e);
    athrs16_reg_write(0x200, 0x200);
    athrs16_reg_write(0x300, 0x200);
    athrs16_reg_write(0x400, 0x200);
    athrs16_reg_write(0x500, 0x200);
#if defined CFG_BOARD_PB45
    athrs16_reg_write(0x600, 0x200);
#elif defined CFG_BOARD_AP96
    athrs16_reg_write(0x600, 0x0);
#endif

    athrs16_reg_write(0x2c, 0x003f003f);

#if defined(CONFIG_VLAN_8021Q) || defined(CONFIG_VLAN_8021Q_MODULE)        
#ifdef HEADER_EN        
    athrs16_reg_write(0x104, 0x6804);
#else
    athrs16_reg_write(0x104, 0x6004);
#endif

    athrs16_reg_write(0x204, 0x6004);
    athrs16_reg_write(0x304, 0x6004);
    athrs16_reg_write(0x404, 0x6004);
    athrs16_reg_write(0x504, 0x6004);    
    athrs16_reg_write(0x604, 0x6004);    
#else
#ifdef HEADER_EN        
    athrs16_reg_write(0x104, 0x4804);
#else
    athrs16_reg_write(0x104, 0x4004);
#endif
#endif

    printk("athrs16_reg_init complete.\n");

    athr16_init_flag = 1;

    phy_mode_setup(); 
}

/******************************************************************************
*
* athrs16_phy_is_link_alive - test to see if the specified link is alive
*
* RETURNS:
*    TRUE  --> link is alive
*    FALSE --> link is down
*/
BOOL
athrs16_phy_is_link_alive(int phyUnit)
{
    uint16_t phyHwStatus;
    uint32_t phyBase;
    uint32_t phyAddr;

    phyBase = ATHR_PHYBASE(phyUnit);
    phyAddr = ATHR_PHYADDR(phyUnit);

    phyHwStatus = phy_reg_read(phyBase, phyAddr, ATHR_PHY_SPEC_STATUS);

    if (phyHwStatus & ATHR_STATUS_LINK_PASS)
        return TRUE;

    return FALSE;
}

/******************************************************************************
*
* athrs16_phy_setup - reset and setup the PHY associated with
* the specified MAC unit number.
*
* Resets the associated PHY port.
*
* RETURNS:
*    TRUE  --> associated PHY is alive
*    FALSE --> no LINKs on this ethernet unit
*/

BOOL
athrs16_phy_setup(int ethUnit)
{
    int       phyUnit;
    uint16_t  phyHwStatus;
    uint16_t  timeout;
    int       liveLinks = 0;
    uint32_t  phyBase = 0;
    BOOL      foundPhy = FALSE;
    uint32_t  phyAddr = 0;
    

    /* See if there's any configuration data for this enet */
    /* start auto negogiation on each phy */
    for (phyUnit=0; phyUnit < ATHR_PHY_MAX; phyUnit++) {
        if (!ATHR_IS_ETHUNIT(phyUnit, ethUnit)) {
            continue;
        }

        foundPhy = TRUE;
        phyBase = ATHR_PHYBASE(phyUnit);
        phyAddr = ATHR_PHYADDR(phyUnit);
        
        phy_reg_write(phyBase, phyAddr, ATHR_AUTONEG_ADVERT,
                      ATHR_ADVERTISE_ALL);

        phy_reg_write(phyBase, phyAddr, ATHR_1000BASET_CONTROL,
                      ATHR_ADVERTISE_1000FULL);

        /* Reset PHYs*/
        phy_reg_write(phyBase, phyAddr, ATHR_PHY_CONTROL,
                      ATHR_CTRL_AUTONEGOTIATION_ENABLE 
                      | ATHR_CTRL_SOFTWARE_RESET);

    }

    if (!foundPhy) {
        return FALSE; /* No PHY's configured for this ethUnit */
    }

    /*
     * After the phy is reset, it takes a little while before
     * it can respond properly.
     */
    mdelay(1000);


    /*
     * Wait up to 3 seconds for ALL associated PHYs to finish
     * autonegotiation.  The only way we get out of here sooner is
     * if ALL PHYs are connected AND finish autonegotiation.
     */
    for (phyUnit=0; (phyUnit < ATHR_PHY_MAX) /*&& (timeout > 0) */; phyUnit++) {
        if (!ATHR_IS_ETHUNIT(phyUnit, ethUnit)) {
            continue;
        }

        timeout=20;
        for (;;) {
            phyHwStatus = phy_reg_read(phyBase, phyAddr, ATHR_PHY_CONTROL);

            if (ATHR_RESET_DONE(phyHwStatus)) {
                DRV_PRINT(DRV_DEBUG_PHYSETUP,
                          ("Port %d, Neg Success\n", phyUnit));
                break;
            }
            if (timeout == 0) {
                DRV_PRINT(DRV_DEBUG_PHYSETUP,
                          ("Port %d, Negogiation timeout\n", phyUnit));
                break;
            }
            if (--timeout == 0) {
                DRV_PRINT(DRV_DEBUG_PHYSETUP,
                          ("Port %d, Negogiation timeout\n", phyUnit));
                break;
            }

            mdelay(150);
        }
    }

    /*
     * All PHYs have had adequate time to autonegotiate.
     * Now initialize software status.
     *
     * It's possible that some ports may take a bit longer
     * to autonegotiate; but we can't wait forever.  They'll
     * get noticed by mv_phyCheckStatusChange during regular
     * polling activities.
     */
    for (phyUnit=0; phyUnit < ATHR_PHY_MAX; phyUnit++) {
        if (!ATHR_IS_ETHUNIT(phyUnit, ethUnit)) {
            continue;
        }

        if (athrs16_phy_is_link_alive(phyUnit)) {
            liveLinks++;
            ATHR_IS_PHY_ALIVE(phyUnit) = TRUE;
        } else {
            ATHR_IS_PHY_ALIVE(phyUnit) = FALSE;
        }

        DRV_PRINT(DRV_DEBUG_PHYSETUP,
            ("eth%d: Phy Specific Status=%4.4x\n",
            ethUnit, 
            phy_reg_read(ATHR_PHYBASE(phyUnit),
                         ATHR_PHYADDR(phyUnit),
                         ATHR_PHY_SPEC_STATUS)));
    }
    //phy_mode_setup();    
    return (liveLinks > 0);
}

/******************************************************************************
*
* athrs16_phy_is_fdx - Determines whether the phy ports associated with the
* specified device are FULL or HALF duplex.
*
* RETURNS:
*    1 --> FULL
*    0 --> HALF
*/
int
athrs16_phy_is_fdx(int ethUnit)
{
    int       phyUnit;
    uint32_t  phyBase;
    uint32_t  phyAddr;
    uint16_t  phyHwStatus;
    int       ii = 200;

    if (ethUnit == ENET_UNIT_LAN)
        return TRUE;

    for (phyUnit=0; phyUnit < ATHR_PHY_MAX; phyUnit++) {
        if (!ATHR_IS_ETHUNIT(phyUnit, ethUnit)) {
            continue;
        }

        if (athrs16_phy_is_link_alive(phyUnit)) {

            phyBase = ATHR_PHYBASE(phyUnit);
            phyAddr = ATHR_PHYADDR(phyUnit);

            do {
                phyHwStatus = phy_reg_read (phyBase, phyAddr, 
                                               ATHR_PHY_SPEC_STATUS);
		if(phyHwStatus & ATHR_STATUS_RESOVLED)
			break;
                mdelay(10);
            } while(--ii);
            
            if (phyHwStatus & ATHER_STATUS_FULL_DEPLEX)
                return TRUE;
        }
    }

    return FALSE;
}

/******************************************************************************
*
* athrs16_phy_speed - Determines the speed of phy ports associated with the
* specified device.
*
* RETURNS:
*               AG7100_PHY_SPEED_10T, AG7100_PHY_SPEED_100TX;
*               AG7100_PHY_SPEED_1000T;
*/

int
athrs16_phy_speed(int ethUnit)
{
    int       phyUnit;
    uint16_t  phyHwStatus;
    uint32_t  phyBase;
    uint32_t  phyAddr;
    int       ii = 200;
    ag7100_phy_speed_t phySpeed = AG7100_PHY_SPEED_10T;

    for (phyUnit=0; phyUnit < ATHR_PHY_MAX; phyUnit++) {
        if (!ATHR_IS_ETHUNIT(phyUnit, ethUnit)) {
            continue;
        }

        phyBase = ATHR_PHYBASE(phyUnit);
        phyAddr = ATHR_PHYADDR(phyUnit);
        phySpeed = AG7100_PHY_SPEED_10T;

        if (athrs16_phy_is_link_alive(phyUnit)) {

            do {
                phyHwStatus = phy_reg_read(phyBase, phyAddr, 
                                              ATHR_PHY_SPEC_STATUS);
		        if(phyHwStatus & ATHR_STATUS_RESOVLED)
			        break;
                mdelay(10);
            }while(--ii);
            
            phyHwStatus = ((phyHwStatus & ATHER_STATUS_LINK_MASK) >>
                           ATHER_STATUS_LINK_SHIFT);

            switch(phyHwStatus) {
            case 0:
                phySpeed = AG7100_PHY_SPEED_10T;
                break;
            case 1:
                phySpeed = AG7100_PHY_SPEED_100TX;
                break;
            case 2:
                phySpeed = AG7100_PHY_SPEED_1000T;
                break;
            default:
                printk("Unkown speed read!\n");
            }
        } 

        //phy_reg_write(phyBase, phyAddr, ATHR_DEBUG_PORT_ADDRESS, 0x18);
        
        if(phySpeed == AG7100_PHY_SPEED_100TX) {
            //phy_reg_write(phyBase, phyAddr, ATHR_DEBUG_PORT_DATA, 0xba8);
        } else {            
            //phy_reg_write(phyBase, phyAddr, ATHR_DEBUG_PORT_DATA, 0x2ea);
        }
    }

    if (ethUnit == ENET_UNIT_LAN)
         phySpeed = AG7100_PHY_SPEED_1000T;

    return phySpeed;
}

/*****************************************************************************
*
* athr_phy_is_up -- checks for significant changes in PHY state.
*
* A "significant change" is:
*     dropped link (e.g. ethernet cable unplugged) OR
*     autonegotiation completed + link (e.g. ethernet cable plugged in)
*
* When a PHY is plugged in, phyLinkGained is called.
* When a PHY is unplugged, phyLinkLost is called.
*/

int
athrs16_phy_is_up(int ethUnit)
{
    int           phyUnit;
    uint16_t      phyHwStatus, phyHwControl;
    athrPhyInfo_t *lastStatus;
    int           linkCount   = 0;
    int           lostLinks   = 0;
    int           gainedLinks = 0;
    uint32_t      phyBase;
    uint32_t      phyAddr;
#ifdef HEADER_REG_CONF
    /* if using header to config s16, the link of MAC0 should always be up */
    if (ethUnit == ENET_UNIT_LAN)
        return 1;
#endif

    for (phyUnit=0; phyUnit < ATHR_PHY_MAX; phyUnit++) {
        if (!ATHR_IS_ETHUNIT(phyUnit, ethUnit)) {
            continue;
        }

        phyBase = ATHR_PHYBASE(phyUnit);
        phyAddr = ATHR_PHYADDR(phyUnit);

        lastStatus = &athrPhyInfo[phyUnit];

        if (lastStatus->isPhyAlive) { /* last known link status was ALIVE */
            phyHwStatus = phy_reg_read(phyBase, phyAddr, ATHR_PHY_SPEC_STATUS);

            /* See if we've lost link */
            if (phyHwStatus & ATHR_STATUS_LINK_PASS) {
                linkCount++;
            } else {
                lostLinks++;
                DRV_PRINT(DRV_DEBUG_PHYCHANGE,("\nenet%d port%d down\n",
                                               ethUnit, phyUnit));
                lastStatus->isPhyAlive = FALSE;
            }
        } else { /* last known link status was DEAD */
            /* Check for reset complete */
            phyHwStatus = phy_reg_read(phyBase, phyAddr, ATHR_PHY_STATUS);
            if (!ATHR_RESET_DONE(phyHwStatus))
                continue;

            phyHwControl = phy_reg_read(phyBase, phyAddr, ATHR_PHY_CONTROL);
            /* Check for AutoNegotiation complete */            
            if ((!(phyHwControl & ATHR_CTRL_AUTONEGOTIATION_ENABLE)) 
                 || ATHR_AUTONEG_DONE(phyHwStatus)) {
                phyHwStatus = phy_reg_read(phyBase, phyAddr, 
                                           ATHR_PHY_SPEC_STATUS);

                if (phyHwStatus & ATHR_STATUS_LINK_PASS) {
                gainedLinks++;
                linkCount++;
                DRV_PRINT(DRV_DEBUG_PHYCHANGE,("\nenet%d port%d up\n",
                                               ethUnit, phyUnit));
                lastStatus->isPhyAlive = TRUE;
                }
            }
        }
    }

    return (linkCount);

}

static int
athrs16_header_config_reg (struct net_device *dev, uint8_t wr_flag, 
                           uint32_t reg_addr, uint16_t cmd_len,
                           uint8_t *val, uint32_t seq_num) 
{
    struct sk_buff *skb;
    at_header_t at_header;
    reg_cmd_t reg_cmd;
    uint16_t head_offset = 12;
    
    /*allocate skb*/        
    skb = dev_alloc_skb(64);
    if (!skb) {
        printk("allocate skb fail\n");
        return -1;
    }
    skb_put(skb, 60);     
    
    /*fill at_header*/
    at_header.version0 =  2; 
    at_header.priority =  0;
    at_header.type =      5;/*READ_WRITE_REG*/
    at_header.broadcast = 0;  
    at_header.from_cpu =  1;
    at_header.version1 =  2;   
    at_header.port_num =  0;

    skb->data[head_offset+1] = at_header.port_num;
    skb->data[head_offset+1] |= at_header.version1 << 4;
    skb->data[head_offset+1] |= at_header.from_cpu << 6;
    skb->data[head_offset+1] |= at_header.broadcast << 7;
    
    skb->data[head_offset] = at_header.type;  
    skb->data[head_offset] |= at_header.priority << 4;    
    skb->data[head_offset] |= at_header.version0 << 6;     
     
    /*fill reg cmd*/
    if(cmd_len > 4) 
        cmd_len = 4;//only support 32bits register r/w
      
    reg_cmd.reg_addr = reg_addr&0x3fffc;
    reg_cmd.cmd_len = cmd_len;
    reg_cmd.cmd = wr_flag;
    reg_cmd.check_code = 5;
    reg_cmd.seq_num = seq_num;   

    skb->data[0] = reg_cmd.reg_addr & 0xff;
    skb->data[1] = (reg_cmd.reg_addr & 0xff00) >> 8;
    skb->data[2] = (reg_cmd.reg_addr & 0x30000) >> 16;
    skb->data[2] |= reg_cmd.cmd_len << 4;
    skb->data[3] = reg_cmd.cmd << 4;
    skb->data[3] |= reg_cmd.check_code << 5;
    skb->data[4] = (reg_cmd.seq_num & 0x7f) << 1;
    skb->data[5] = (reg_cmd.seq_num & 0x7f80) >> 7;
    skb->data[6] = (reg_cmd.seq_num & 0x7f8000) >> 15;
    skb->data[7] = (reg_cmd.seq_num & 0x7f800000) >> 23;
  
    if(!wr_flag)//write 
    {
         memcpy(skb->data + 8, val, 4);
         memcpy(skb->data + 14, val + 4, cmd_len - 4);
    }
        //memcpy(skb->data + 10, val, cmd_len);
    
    skb->dev = dev;

    /* add identify for header */
    skb->cb[0] = 0x7f;
    skb->cb[1] = 0x5d;

    /*start xmit*/
    header_xmit(skb, dev);

    return 0;
}


static int
__athrs16_header_write_reg(uint32_t reg_addr, uint16_t cmd_len, uint8_t *reg_data,
                           uint32_t seq_num)
{
    long timeout;
    int i = 2;
    uint8_t reg_tmp[4];
    int rtn = 0;
    
    down(&hdr_sem);
    
    /*fill reg data*/
    reg_tmp[0] = (uint8_t)(0x00ff & (*((uint32_t *)reg_data))); 
    reg_tmp[1] = (uint8_t)((0xff00 & (*((uint32_t *)reg_data))) >> 8);      
    reg_tmp[2] = (uint8_t)((0xff0000 & (*((uint32_t *)reg_data))) >> 16);
    reg_tmp[3] = (uint8_t)((0xff000000 & (*((uint32_t *)reg_data))) >> 24);

    do {
        wait_flag = 0;
        athrs16_header_config_reg(ag7100_macs[0]->mac_dev, 0, reg_addr, cmd_len, 
                                  reg_tmp, seq_num);
        timeout = HZ;     
        if (!in_interrupt()) {
            timeout = wait_event_interruptible_timeout (hd_conf_wait, 
                                                        wait_flag != 0, timeout);
        }
        if (timeout || ((reg_addr == 0)&&(reg_tmp[3]&0x80)))  //ignore reset write echo 
            break;
        else
            printk("write time out\n");
    } while (i--);

    if(i==0) {
        rtn = -1;
    }

    up(&hdr_sem);
  
    return rtn;   
}

static int
__athrs16_header_read_reg(uint32_t reg_addr, uint16_t cmd_len, uint8_t *reg_data, 
                          uint32_t seq_num)
{
    long timeout;
    int i = 2;
    int rtn = 0;
    
    if (in_interrupt())
        return -1;

    down(&hdr_sem);
        
    do {
        wait_flag = 0;          
        athrs16_header_config_reg(ag7100_macs[0]->mac_dev, 1, reg_addr,
                                  cmd_len, reg_data, seq_num);
        timeout = HZ;  
        timeout = wait_event_interruptible_timeout (hd_conf_wait, 
                                                    wait_flag != 0, timeout);
        if (timeout) 
            break;
        else 
            printk("read time out\n");
    } while(i--);   
    
    if ((i==0) || (seq_num != cmd_resp.seq) || (cmd_len != cmd_resp.len)) {
        for(i=0; i<1000; i++)
        {
            if(seq_num == cmd_resp.seq) {
                break;
            } else {
                udelay(10);
            }    
        }
        printk("##(%d) seq[%d:%d] len[%d:%d]\n", i, seq_num, cmd_resp.seq, cmd_len, cmd_resp.len);
        if(i==1000) {
            rtn = -1;
        }
    }
  
    (*((uint32_t *)reg_data)) = cmd_resp.data[0] | (cmd_resp.data[1] << 8)
                             | (cmd_resp.data[2] << 16)| (cmd_resp.data[3] << 24);

    up(&hdr_sem);
    
    return rtn;   
}

int
athrs16_header_write_reg(uint32_t reg_addr, uint16_t cmd_len, uint8_t *reg_data)
{
    if(atomic_read(&seqcnt) == 0x7fffffff){
        atomic_set(&seqcnt, 0);
    } else {
        atomic_inc(&seqcnt);
    }

    __athrs16_header_write_reg(reg_addr, 4, reg_data, atomic_read(&seqcnt));

    return 0;   
}

int
athrs16_header_read_reg(uint32_t reg_addr, uint16_t cmd_len, uint8_t *reg_data)
{
      if(atomic_read(&seqcnt) == 0x7fffffff){
      atomic_set(&seqcnt, 0);
    } else {
      atomic_inc(&seqcnt);
    }
    __athrs16_header_read_reg(reg_addr, 4, reg_data, atomic_read(&seqcnt));

    return 0;
}

void header_receive_skb(struct sk_buff *skb)
{
    wait_flag = 1;
    cmd_resp.len = skb->data[2] >> 4;
    if (cmd_resp.len > 10)
        goto out;

    cmd_resp.seq = 0;
    cmd_resp.seq = skb->data[4] >> 1;
    cmd_resp.seq |= skb->data[5] << 7;
    cmd_resp.seq |= skb->data[6] << 15;
    cmd_resp.seq |= skb->data[7] << 23;

    if (cmd_resp.seq < atomic_read(&seqcnt))
        goto out;

    memcpy ((cmd_resp.data), (skb->data + 8), 4);
    memcpy ((cmd_resp.data+4), (skb->data + 14), cmd_resp.len-4);
    
    wake_up_interruptible(&hd_conf_wait);
    
out:    
    kfree_skb(skb); 
}

void athrs16_reg_dev(ag7100_mac_t **mac)
{
    ag7100_macs[0] = mac[0];
    ag7100_macs[0]->mac_speed = 0xff;
    ag7100_macs[1] = mac[1];
    ag7100_macs[1]->mac_speed = 0xff;

}

uint32_t
athrs16_reg_read(uint32_t reg_addr)
{
#ifndef HEADER_REG_CONF 
    return athrs16_mdio_read(reg_addr);
#else
    uint8_t reg_data[4] = {0};
    athrs16_header_read_reg(reg_addr, 4, reg_data);
    return *((uint32_t *)reg_data);
#endif    
}

void
athrs16_reg_write(uint32_t reg_addr, uint32_t reg_val)
{
#ifndef HEADER_REG_CONF
    athrs16_mdio_write(reg_addr, reg_val);
#else
    athrs16_header_write_reg (reg_addr, 4, (uint8_t *)&reg_val);
#endif
}

int
athr_ioctl(uint32_t *args, int cmd)
{
    printk("EOPNOTSUPP\n");
    return -EOPNOTSUPP;

}


