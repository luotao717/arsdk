#ifndef __mtd_h
#define __mtd_h

#ifdef __KERNEL__

#include <linux/config.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/mtd/compatmac.h>
#include <linux/notifier.h>
#include <linux/module.h>
#include <linux/uio.h>

#endif /* __KERNEL__ */

struct erase_info_user {
	u_int32_t start;
	u_int32_t length;
};

struct mtd_oob_buf {
	u_int32_t start;
	u_int32_t length;
	unsigned char *ptr;
};


#define MTD_CHAR_MAJOR 90
#define MTD_BLOCK_MAJOR 31
#define MAX_MTD_DEVICES 16



#define MTD_ABSENT		0
#define MTD_RAM			1
#define MTD_ROM			2
#define MTD_NORFLASH		3
#define MTD_NANDFLASH		4
#define MTD_PEROM		5
#define MTD_OTHER		14
#define MTD_UNKNOWN		15



#define MTD_CLEAR_BITS		1       // Bits can be cleared (flash)
#define MTD_SET_BITS		2       // Bits can be set
#define MTD_ERASEABLE		4       // Has an erase function
#define MTD_WRITEB_WRITEABLE	8       // Direct IO is possible
#define MTD_VOLATILE		16      // Set for RAMs
#define MTD_XIP			32	// eXecute-In-Place possible
#define MTD_OOB			64	// Out-of-band data (NAND flash)
#define MTD_ECC			128	// Device capable of automatic ECC

// Some common devices / combinations of capabilities
#define MTD_CAP_ROM		0
#define MTD_CAP_RAM		(MTD_CLEAR_BITS|MTD_SET_BITS|MTD_WRITEB_WRITEABLE)
#define MTD_CAP_NORFLASH        (MTD_CLEAR_BITS|MTD_ERASEABLE)
#define MTD_CAP_NANDFLASH       (MTD_CLEAR_BITS|MTD_ERASEABLE|MTD_OOB)
#define MTD_WRITEABLE		(MTD_CLEAR_BITS|MTD_SET_BITS)


// Types of automatic ECC/Checksum available
#define MTD_ECC_NONE		0 	// No automatic ECC available
#define MTD_ECC_RS_DiskOnChip	1	// Automatic ECC on DiskOnChip
#define MTD_ECC_SW		2	// SW ECC for Toshiba & Samsung devices

struct mtd_info_user {
	u_char type;
	u_int32_t flags;
	u_int32_t size;	 // Total size of the MTD
	u_int32_t erasesize;
	u_int32_t oobblock;  // Size of OOB blocks (e.g. 512)
	u_int32_t oobsize;   // Amount of OOB data per block (e.g. 16)
	u_int32_t ecctype;
	u_int32_t eccsize;
};

struct region_info_user {
	u_int32_t offset;		/* At which this region starts, 
					 * from the beginning of the MTD */
	u_int32_t erasesize;		/* For this region */
	u_int32_t numblocks;		/* Number of blocks in this region */
	u_int32_t regionindex;
};

#define MEMGETINFO              _IOR('M', 1, struct mtd_info_user)
#define MEMERASE                _IOW('M', 2, struct erase_info_user)
#define MEMWRITEOOB             _IOWR('M', 3, struct mtd_oob_buf)
#define MEMREADOOB              _IOWR('M', 4, struct mtd_oob_buf)
#define MEMLOCK                 _IOW('M', 5, struct erase_info_user)
#define MEMUNLOCK               _IOW('M', 6, struct erase_info_user)
#define MEMGETREGIONCOUNT	_IOR('M', 7, int)
#define MEMGETREGIONINFO	_IOWR('M', 8, struct region_info_user)
#define	MEMREADDATA             _IOWR('M', 9, struct mtd_oob_buf)
#define	MEMWRITEDATA            _IOWR('M', 10, struct mtd_oob_buf)
#include <stdbool.h>

#ifdef target_brcm47xx
#define target_brcm 1
#endif

#define JFFS2_EOF "\xde\xad\xc0\xde"

extern int quiet;
extern int mtdsize;
extern int erasesize;

extern int mtd_open(const char *mtd, bool block);
extern int mtd_check_open(const char *mtd);
extern int mtd_erase_block(int fd, int offset);
extern int mtd_write_buffer(int fd, const char *buf, int offset, int length);
extern int mtd_write_jffs2(const char *mtd, const char *filename, const char *dir);
extern int mtd_replace_jffs2(const char *mtd, int fd, int ofs, const char *filename);
extern void mtd_parse_jffs2data(const char *buf, const char *dir);

/* target specific functions */
extern int trx_fixup(int fd, const char *name)  __attribute__ ((weak));
extern int trx_check(int imagefd, const char *mtd, char *buf, int *len) __attribute__ ((weak));
extern int mtd_fixtrx(const char *mtd, size_t offset) __attribute__ ((weak));
extern int mtd_fixseama(const char *mtd, size_t offset) __attribute__ ((weak));
#endif /* __mtd_h */
