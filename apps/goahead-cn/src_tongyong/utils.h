/*
 *	utils.h -- System Utilitie Header
 *
 *	Copyright (c) Ralink Technology Corporation All Rights Reserved.
 *
 *	$Id: //WIFI_SOC/release/SDK_4_1_0_0/source/user/goahead/src/utils.h#1 $
 */
#include 	"webs.h"
#include	"uemf.h"

#if defined (CONFIG_LKTOS_PRIVATE_OEM_NAME_NBOX)
#define Version				"v1.00"
#elif defined (CONFIG_LKTOS_PRIVATE_OEM_NAME_CDRKING)
#define Version				"CDRKING_WR-NET-022-ZI_V1.0.0"
#else
#define Version				"v4.0"
#endif

#define LED_ON				1
#define LED_OFF				0

#define WPS_LED_RESET			1
#define WPS_LED_PROGRESS		2
#define WPS_LED_ERROR			3
#define WPS_LED_SESSION_OVERLAP		4
#define WPS_LED_SUCCESS			5

#define SIG_MODEM_EXIST                       (SIGUSR1+61)	
#define SIG_MODEM_NOEXIST                  (SIGUSR1+62)	

int doKillPid(char_t *fmt, ...);
int doSystem(char_t *fmt, ...);
char *getNthValue(int index, char *values);
char *setNthValue(int index, char *old_values, char *new_value);
int deleteNthValueMulti(int index[], int count, char *value, char delimit);
int getValueCount(char *values);
void formDefineUtilities(void);
int checkSemicolon(char *str);
int isInLan(char *radius_ip_addr);
int getNthValueSafe(int index, char *value, char delimit, char *result, int len);
int setTimer(int mili, void ((*sigroutine)(int)));
void stopTimer(void);
int ledAlways(int gpio, int on);
int ledWps(int gpio, int mode);
unsigned int Sleep(unsigned int secs);
char *racat(char *s, int i);
void arplookup(char *ip, char *arp);
void websLongWrite(webs_t wp, char *longstr);
int netmask_aton(const char *ip);
inline int getOnePortOnly(void);
void ConverterStringToDisplay(char *str);
