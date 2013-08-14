/*
 *      Header file of AP mib
 *      Authors: Kony Luo	<yucosta@sohu.com>
 *
 *      $Id: apmib.h,v 1.00 2013/01/11 01:12:24 bradhuang Exp $
 *
 */


#ifndef INCLUDE_APMIB_H
#define INCLUDE_APMIB_H

#define HW_SETTING_SECTOR_LEN           (0x8000-0x6000)
#define DEFAULT_SETTING_SECTOR_LEN      (0xc000-0x8000)
#define CURRENT_SETTING_SECTOR_LEN      (0x10000-0xc000)

#ifndef WIN32
#define __PACK__		__attribute__ ((packed))
#else
#define __PACK__
#endif


/* Config/fw image file header */

typedef enum { HW_SETTING=1, DEFAULT_SETTING=2, CURRENT_SETTING=4 } CONFIG_DATA_T;
#define HW_SETTING_HEADER_TAG		((char *)"h6")
#define DEFAULT_SETTING_HEADER_TAG	((char *)"6V")
#define CURRENT_SETTING_HEADER_TAG	((char *)"6g")

// force tag
#define HW_SETTING_HEADER_FORCE_TAG	((char *)"Hf")
#define DEFAULT_SETTING_HEADER_FORCE_TAG ((char *)"Df")
#define CURRENT_SETTING_HEADER_FORCE_TAG ((char *)"Cf")
// upgrade
#define HW_SETTING_HEADER_UPGRADE_TAG	((char *)"Hu")
#define DEFAULT_SETTING_HEADER_UPGRADE_TAG ((char *)"Du")
#define CURRENT_SETTING_HEADER_UPGRADE_TAG ((char *)"Cu")

#define TAG_LEN				2
#define HW_SETTING_VER			1	// hw setting version
#define DEFAULT_SETTING_VER		1	// default setting version
#define CURRENT_SETTING_VER		DEFAULT_SETTING_VER // current setting version

#define FW_HEADER_WITH_ROOT	((char *)"cr6c")
#define FW_HEADER			((char *)"kf01")
#define WEB_HEADER			((char *)"w6cg")
#define ROOT_HEADER			((char *)"rf01")

#define CERT_HEADER			((char *)"cert")
#define BOOT_HEADER			((char *)"boot")
#define ALL_HEADER			((char *)"allp")
#define SIGNATURE_LEN			4

#define DWORD_SWAP(v) ( (((v&0xff)<<24)&0xff000000) | ((((v>>8)&0xff)<<16)&0xff0000) | \
				((((v>>16)&0xff)<<8)&0xff00) | (((v>>24)&0xff)&0xff) )
#define WORD_SWAP(v) ((unsigned short)(((v>>8)&0xff) | ((v<<8)&0xff00)))

/* scramble saved configuration data */
#define ENCODE_DATA(data,len) { \
	int i; \
	for (i=0; i<len; i++) \
		data[i] = ~ ( data[i] + 0x38); \
}

#define DECODE_DATA(data,len) { \
	int i; \
	for (i=0; i<len; i++) \
		data[i] = ~data[i] - 0x38;	\
}

/* Do checksum and verification for configuration data */
#ifndef WIN32
static inline unsigned char CHECKSUM(unsigned char *data, int len)
#else
__inline unsigned char CHECKSUM(unsigned char *data, int len)
#endif
{
	int i;
	unsigned char sum=0;

	for (i=0; i<len; i++)
		sum += data[i];

	sum = ~sum + 1;
	return sum;
}
#ifndef WIN32
static inline int CHECKSUM_OK(unsigned char *data, int len)
#else
__inline int CHECKSUM_OK(unsigned char *data, int len)
#endif
{
	int i;
	unsigned char sum=0;

	for (i=0; i<len; i++)
		sum += data[i];

	if (sum == 0)
		return 1;
	else
		return 0;
}

/* Config file header */
typedef struct param_header {
	unsigned char signature[SIGNATURE_LEN];  // Tag + version
	unsigned short len;
}__PACK__ PARAM_HEADER_T, *PARAM_HEADER_Tp;

/* Firmware image file header */
typedef struct img_header {
	unsigned char signature[SIGNATURE_LEN];
	unsigned int startAddr;
	unsigned int burnAddr;
	unsigned int len;
}__PACK__ IMG_HEADER_T, *IMG_HEADER_Tp;

/* Web page file header */
typedef IMG_HEADER_T WEB_HEADER_T;
typedef IMG_HEADER_Tp WEB_HEADER_Tp;

typedef struct _file_entry {
	char name[128];
	unsigned int size;
}__PACK__ FILE_ENTRY_T, *FILE_ENTRY_Tp;

#ifdef COMPRESS_MIB_SETTING
/* Compress config setting file header */
#if 0
#define	COMP_TRACE	fprintf
#else
#define COMP_TRACE
#endif

#define COMP_SIGNATURE_LEN			6
#define COMP_HS_SIGNATURE			"COMPHS"
#define COMP_CS_SIGNATURE			"COMPCS"
#define COMP_DS_SIGNATURE			"COMPDS"
typedef struct compress_mib_header {
	unsigned char signature[COMP_SIGNATURE_LEN];
	unsigned short compRate;
	unsigned int compLen;
}__PACK__ COMPRESS_MIB_HEADER_T, *COMPRESS_MIB_HEADER_Tp;

#endif // #ifdef COMPRESS_MIB_SETTING

#ifdef WIN32
#pragma pack()
#endif


#endif // INCLUDE_APMIB_H