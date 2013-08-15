#include <stdlib.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <asm/types.h>
#include <linux/if.h>
#include <linux/wireless.h>
#include <ctype.h>
#include <errno.h>
#include "linux/autoconf.h"
#include "config/autoconf.h" //user config
#include "user/busybox/include/autoconf.h" //busybox config

#include "nvram.h"
#include "utils.h"
#include "webs.h"
#include "internet.h"
#include "appinterface.h"

static void mult(long op1, long op2, long *prod_hi, long *prod_lo)
{
    long op1_hi = (op1 >> 16) & 0xffff;
    long op1_lo = op1 & 0xffff;
    long op2_hi = (op2 >> 16) & 0xffff;
    long op2_lo = op2 & 0xffff;
    long cross_prod = op1_lo * op2_hi + op1_hi * op2_lo;
    *prod_hi = op1_hi * op2_hi + ((cross_prod >> 16) & 0xffff);
    *prod_lo = op1_lo * op2_lo + ((cross_prod << 16) & 0xffff0000);
}

static void AppReturnHeader(webs_t wp)
{
	websWrite(wp, T("HTTP/1.0 200 OK\n"));
	websWrite(wp, T("Server: %s\r\n"), WEBS_NAME);
	websWrite(wp, T("Pragma: no-cache\n"));
	websWrite(wp, T("Cache-control: no-cache\n"));
	websWrite(wp, T("Content-Type: text/xml\n"));
	websWrite(wp, T("\n"));
	
}

static void AppReturnOK(webs_t wp)		
{ 
	AppReturnHeader(wp);
	websWrite(wp, T("<?xml version=\"1.0\" encoding=\"UTF-8\"?>"));		
	websWrite(wp, T("<response>"));	
	websWrite(wp, T("<retcode>"));	
	websWrite(wp, T("0"));
	websWrite(wp, T("</retcode>"));	
	websWrite(wp, T("<retdesc>"));	
	websWrite(wp, T("success"));
	websWrite(wp, T("</retdesc>"));	
	websWrite(wp, T("</response>"));	
	websDone(wp, 200);	
}

static void AppReturnNOK(webs_t wp, char *err_buf)	
{	
	AppReturnHeader(wp);
	websWrite(wp, T("<?xml version=\"1.0\" encoding=\"UTF-8\"?>"));		
	websWrite(wp, T("<response>"));	
	websWrite(wp, T("<retcode>"));	
	websWrite(wp, T("1"));
	websWrite(wp, T("</retcode>"));	
	websWrite(wp, T("<retdesc>"));	
	websWrite(wp, T("%s"), err_buf);
	websWrite(wp, T("</retdesc>"));	
	websWrite(wp, T("</response>"));	
	websDone(wp, 200);	
}

static void AppLogin(webs_t wp, char_t *path, char_t *query)
{	
	char *us = websGetVar(wp, T("username"), T("0"));
	char *pd = websGetVar(wp, T("password"), T("0"));
	char *pf = websGetVar(wp, T("platform"), T("0"));
	char *cv = websGetVar(wp, T("version"), T("0"));

	APP_TRACE("us is %s, pd is %s, pf is %s, cv is %s\n", us, pd, pf, cv);

	const char *username = nvram_bufget(RT2860_NVRAM, "Login"); 
	const char *password= nvram_bufget(RT2860_NVRAM, "Password"); 

	if (!strcmp(us, "0") || !strcmp(pd, "0") || !strcmp(pf, "0") || !strcmp(cv, "0"))
	{
		AppReturnNOK(wp, T("parameter error"));
		return;	
	}
	
	if (!strcmp(us, username) && !strcmp(pd, password))
		printf("check username and password ok\n");
	else
	{
		AppReturnNOK(wp, T("check username or password error"));
		return;
	}

	if (!strcmp(pf, "ios") && !strcmp(cv, "1.00"))
		printf("check platform and version ok\n");
	else
	{
		AppReturnNOK(wp, T("check platform or version error"));
		return;
	}
	
	AppReturnOK(wp);	

}

static void AppLogout(webs_t wp, char_t *path, char_t *query)
{
	AppReturnOK(wp);
}

static void AppSetWifi(webs_t wp, char_t *path, char_t *query)
{
	char *ssid = websGetVar(wp, T("ssidname"), T(""));
	char *pd = websGetVar(wp, T("password"), T(""));

	APP_TRACE("ssid is %s, pd is %s\n", ssid, pd);

	if(strlen(ssid) == 0 || strlen(pd) == 0)
	{
		AppReturnNOK(wp, T("wifi ssid or  password empty"));
		return;
	}

	if(strlen(pd) < 8)
	{
		AppReturnNOK(wp, T("wifi password length must ge 8"));
		return;
	}

	APP_TRACE("set ssid and set default wifi encrypType: wpa2psk\n");
	nvram_bufset(RT2860_NVRAM, "SSID1", ssid);
	nvram_bufset(RT2860_NVRAM, "AuthMode", "WPA2PSK");
	
	nvram_bufset(RT2860_NVRAM, "EncrypType", "AES");
	nvram_bufset(RT2860_NVRAM, "DefaultKeyID", "1"); //old value=2, modify by chenfei
	nvram_bufset(RT2860_NVRAM, "RekeyInterval", "3600");
	nvram_bufset(RT2860_NVRAM, "RekeyMethod", "TIME");
	nvram_bufset(RT2860_NVRAM, "IEEE8021X", "0");
	nvram_bufset(RT2860_NVRAM, "WPAPSK1", pd);

	nvram_commit(RT2860_NVRAM);      
	
	AppReturnOK(wp);

	doSystem("killall goahead;goahead &");

}

static void AppSetWan(webs_t wp, char_t *path, char_t *query)
{
	char_t	*ctype;
	char_t	*ip, *nm, *gw, *pd, *sd;
	char_t	*eth, *user, *pass, *hostname;
	char_t  *pptp_srv, *pptp_mode;
	char_t  *l2tp_srv, *l2tp_mode;
#ifdef CONFIG_USER_3G
	char_t	*usb3g_dev=NULL, *usb3g_pin=NULL, *usb3g_apn=NULL, *usb3g_dial=NULL, *usb3g_user=NULL, *usb3g_pass=NULL;
	//char_t	*usb3g_mode=NULL;
#endif
	const char	*opmode = nvram_bufget(RT2860_NVRAM, "OperationMode");
	const char	*lan_ip = nvram_bufget(RT2860_NVRAM, "lan_ipaddr");
	const char	*lan2enabled = nvram_bufget(RT2860_NVRAM, "Lan2Enabled");

	ctype = ip = nm = gw = pd = sd = eth = user = pass = hostname =
		pptp_srv = pptp_mode = l2tp_srv = l2tp_mode =
		NULL;

	ctype = websGetVar(wp, T("wantype"), T("0")); 
	APP_TRACE("mode ctype  is %s\n", ctype);
	
	if (!strncmp(ctype, "STATIC", 7) || !strcmp(opmode, "0")) {
		APP_TRACE("wan---static mode\n");
		
		//always treat bridge mode having static wan connection
		ip = websGetVar(wp, T("ip"), T(""));
		nm = websGetVar(wp, T("mask"), T("0"));
		gw = websGetVar(wp, T("gateway"), T(""));
		pd = websGetVar(wp, T("dns1"), T(""));
		sd = websGetVar(wp, T("dns2"), T(""));

		nvram_bufset(RT2860_NVRAM, "wanConnectionMode", ctype);
		if (-1 == inet_addr(ip)) {
			nvram_commit(RT2860_NVRAM);
			AppReturnNOK(wp, "invalid IP Address");
			return;
		}
		/*
		 * lan and wan ip should not be the same except in bridge mode
		 */
		if (NULL != opmode && strcmp(opmode, "0") && !strncmp(ip, lan_ip, 15)) {
			nvram_commit(RT2860_NVRAM);
			AppReturnNOK(wp, "IP address is identical to LAN");
			return;
		}
		if (!strcmp(lan2enabled, "1"))
		{
			const char	*lan2_ip = nvram_bufget(RT2860_NVRAM, "lan2_ipaddr");
			if (NULL != opmode && strcmp(opmode, "0") && !strncmp(ip, lan2_ip, 15)) {
				nvram_commit(RT2860_NVRAM);
				AppReturnNOK(wp, "IP address is identical to LAN2");
				return;
			}
		}
		nvram_bufset(RT2860_NVRAM, "wan_ipaddr", ip);
		if (-1 == inet_addr(nm)) {
			nvram_commit(RT2860_NVRAM);
			AppReturnNOK(wp, "invalid Subnet Mask");
			return;
		}
		nvram_bufset(RT2860_NVRAM, "wan_netmask", nm);
		/*
		 * in Bridge Mode, lan and wan are bridged together and associated with
		 * the same ip address
		 */
		if (NULL != opmode && !strcmp(opmode, "0")) {
			nvram_bufset(RT2860_NVRAM, "lan_ipaddr", ip);
			nvram_bufset(RT2860_NVRAM, "lan_netmask", nm);
		}
		nvram_bufset(RT2860_NVRAM, "wan_gateway", gw);
		nvram_bufset(RT2860_NVRAM, "wan_primary_dns", pd);
		nvram_bufset(RT2860_NVRAM, "wan_secondary_dns", sd);
	}
	else {
		nvram_bufset(RT2860_NVRAM, "wan_ipaddr", "");
		nvram_bufset(RT2860_NVRAM, "wan_netmask", "");
		nvram_bufset(RT2860_NVRAM, "wan_gateway", "");
		
		if (!strncmp(ctype, "DHCP", 5)) {
			APP_TRACE("wan---dhcp mode\n");
			
			hostname = websGetVar(wp, T("hostname"), T(""));

			nvram_bufset(RT2860_NVRAM, "wanConnectionMode", ctype);
			nvram_bufset(RT2860_NVRAM, "wan_dhcp_hn", hostname);
		}
		else if (!strncmp(ctype, "PPPOE", 6)) {
			APP_TRACE("wan---pppoe mode\n");
			
			char_t *pppoe_opmode, *pppoe_optime, *pppoe_spectype;

			user = websGetVar(wp, T("name"), T(""));
			pass = websGetVar(wp, T("password"), T(""));
			pppoe_spectype = websGetVar(wp, T("specType"), T("0"));
			pppoe_opmode = websGetVar(wp, T("pppoeOPMode"), T("KeepAlive"));

			APP_TRACE("pppoe_spectype is %s\n", pppoe_spectype);
			APP_TRACE("pppoe_opmode is %s\n", pppoe_opmode);
			
			if (0 == strcmp(pppoe_opmode, "OnDemand"))
				pppoe_optime = websGetVar(wp, T("pppoeIdleTime"), T(""));
			else 
				pppoe_optime = websGetVar(wp, T("pppoeRedialPeriod"), T(""));

			nvram_bufset(RT2860_NVRAM, "wan_pppoe_user", user);
			nvram_bufset(RT2860_NVRAM, "wan_pppoe_pass", pass);
			nvram_bufset(RT2860_NVRAM, "wanConnectionMode", ctype);
			nvram_bufset(RT2860_NVRAM, "wan_pppoe_opmode", pppoe_opmode);
			nvram_bufset(RT2860_NVRAM, "wan_pppoe_optime", pppoe_optime);
			nvram_bufset(RT2860_NVRAM, "wan_pppoe_spectype", pppoe_spectype);

#if 1	//for xkjs, support hunan telcom, henan netcom
			char pppoe_user_tmp[40];		
			if (atoi(pppoe_spectype)==0)
				nvram_bufset(RT2860_NVRAM, "wan_pppoe_user_mm", user);
			else if (atoi(pppoe_spectype)==1) {
				sprintf(pppoe_user_tmp, "\n\r%s", user);
				nvram_bufset(RT2860_NVRAM, "wan_pppoe_user_mm", pppoe_user_tmp);
			}
			else if (atoi(pppoe_spectype)==2) {
				sprintf(pppoe_user_tmp, "^^%s", user);
				nvram_bufset(RT2860_NVRAM, "wan_pppoe_user_mm", pppoe_user_tmp);
			}
			else if (atoi(pppoe_spectype)==3) {	
				char cas_str[]="9012345678abcdeABCDEFGHIJKLMNfghijklmnUVWXYZxyzuvwopqrstOPQRST";
				int cas_str_buffer[16]={17,52,201,35,117,24,215,226,18,53,41,43,236,182,35,25};
				int cas_esi = 37;
				char src_str[128],dec_str[128]="";
				int k=0;
				int cas_eax,cas_edx;
				div_t x;
				int i,j;
					
				sprintf(src_str,"%s", user);
							
				for ( i=0;i<strlen(src_str);i++) {
					for(j=0;j<strlen(cas_str);j++)
						if (src_str[i]==cas_str[j])	{
							if(i<16) {
								cas_eax=cas_str_buffer[i];
							}
							else {
								x=div(i,16);
								cas_eax=cas_str_buffer[x.rem] ;
							}
													
						cas_edx=cas_esi+cas_esi*2 ;
						cas_eax=cas_eax^cas_edx;
						cas_eax=cas_eax^k;
						cas_eax=cas_eax+j;
						x=div(cas_eax,62);
						dec_str[i]=cas_str[x.rem];
						cas_edx=x.rem;
						cas_esi=cas_esi^(cas_edx+9433);
			
						break;
					}
			
					if( dec_str[i]==NULL)
						dec_str[i]=src_str[i];
			
					k=k+5;
				}
			
				sprintf(pppoe_user_tmp,"2:%s", dec_str);
				nvram_bufset(RT2860_NVRAM, "wan_pppoe_user_mm", pppoe_user_tmp);
			}
			else if (atoi(pppoe_spectype)==4) {
				unsigned char shiftKey[16]={
					0x07,0x0C,0x11,0x16,0x05,0x09,0x0e,0x14,
					0x04,0x0b,0x10,0x17,0x06,0x0a,0x0f,0x15
				};
				unsigned char valueKey[48]={
					0x01,0x06,0x0b,0x00,0x05,0x0a,0x0f,0x04,
					0x09,0x0e,0x03,0x08,0x0d,0x02,0x07,0x0c,
					0x05,0x08,0x0b,0x0e,0x01,0x04,0x07,0x0a,
					0x0d,0x00,0x03,0x06,0x09,0x0c,0x0f,0x02,
					0x00,0x07,0x0e,0x05,0x0c,0x03,0x0a,0x01,
					0x08,0x0f,0x06,0x0d,0x04,0x0b,0x02,0x09
				};
				unsigned int globalKey[64]={
					0xD76AA478,0xE8C7B756,0x242070DB,0xC1BDCEEE,
					0xF57C0FAF,0x4787C62A,0xA8304613,0xFD469501,
					0x698098D8,0x8B44F7AF,0xFFFF5BB1,0x895CD7BE,
					0x6B901122,0xFD987193,0xA679438E,0x49B40821,
					0xF61E2562,0xC040B340,0x265E5A51,0xE9B6C7AA,
					0xD62F105D,0x02441453,0xD8A1E681,0xE7D3FBC8,
					0x21E1CDE6,0xC33707D6,0xF4D50D87,0x455A14ED,
					0xA9E3E905,0xFCEFA3F8,0x676F02D9,0x8D2A4C8A,
					0xFFFA3942,0x8771F681,0x6D9D6122,0xFDE5380C,
					0xA4BEEA44,0x4BDECFA9,0xF6BB4B60,0xBEBFBC70,
					0x289B7EC6,0xEAA127FA,0xD4EF3085,0x04881D05,
					0xD9D4D039,0xE6DB99E5,0x1FA27CF8,0xC4AC5665,
					0xF4292244,0x432AFF97,0xAB9423A7,0xFC93A039,
					0x655B59C3,0x8F0CCC92,0xFFEFF47D,0x85845DD1,
					0x6FA87E4F,0xFE2CE6E0,0xA3014314,0x4E0811A1,
					0xF7537E82,0xBD3AF235,0x2AD7D2BB,0xEB86D391
				};
				unsigned long countKey[4]={0x67452301,0xEFCDAB89,0x98BADCFE,0x10325476};
				unsigned long countKeyTemp[4]={0x67452301,0xEFCDAB89,0x98BADCFE,0x10325476};
	
				unsigned long totalInt[16]={0};
				char bufShare[40];
				char bufRealKey2[8];
				char ourEncryptKey[60];			
				//char username[20]={0};
				//char username[20]="lan0199543";
				//char globalName[40]="lan0199543";
				//char *string = user;
				//char username[20]="sanmao";
				//char globalName[40]="sanmao";
				char username[20];
				char globalName[40];
				strcpy(username,user);
				strcpy(globalName,user);
				//char *username=user;
				//char *globalName=user;
				char sharename[7]="radius";
				int usernamelen=strlen(username);//10
				int totallen=usernamelen+6;//16
				unsigned long timeMark;
				unsigned long timeMarkLow;
				unsigned long timeMarkHigh;
				unsigned long timeMarkHighShiftRight1;
				unsigned long timeMarkUse;
				unsigned long timeMarkUse1;
				uint64_t timeMark64;
				unsigned long timeMarkUseLow;
				unsigned long timeMarkUseHigh;
				int tempi1;
				int tempi2;
				int tempk;
				int tempi;
				int tempj;
				int k;
				int m;
				//int i;
				unsigned char my2_al,my2_dl,my2_cl;
				unsigned char my2_char_arr[7];
				unsigned long tempUse1=0;
				unsigned char frontKey[30];
				int countKey_sign;
				unsigned char shiftCharValue;
				unsigned long my_esi;
				unsigned long my_eax,temp_eax,my_ebx,my_ecx,my_edx,my_ecx2,my_ebp,my_edi;
	
				unsigned long my2_esi,zhi_eax,zhi_ecx;
				unsigned long my2_eax,temp_eax2,my2_ebx,my2_ecx,my2_edx,my2_ecx2,my2_ebp,my2_edi;
				strcat(globalName,sharename);
			#if XKJSDEBUG
				printf("\r\n%d---%d\r\n",sizeof(unsigned long),sizeof(unsigned long long));
			#endif
				//timeMark=getSysTime();
				//timeMark=0x49C98A06;
				//timeMark=0x49D08e24;
				time(&timeMark);//get current time
				printf("\r\ntime=%0lx",timeMark);
				mult(timeMark,0x66666667,&timeMarkHigh,&timeMarkLow);
				//timeMarkHigh=1D83D0CF,timeMarkLow=5F78EC6A
			#if XKJSDEBUG
				printf("\r\ntimeH=%0lx,timeL=%0lx,shift=%0lx\r\n",timeMarkHigh,timeMarkLow,timeMarkHigh>>1);
			#endif
				if((timeMarkHigh>>31)==1)
					timeMarkHighShiftRight1=timeMarkHigh>>1+0x10000000;
				else
					timeMarkHighShiftRight1=timeMarkHigh>>1;
				//timeMarkHighShiftRight1=0EC1E867
				//printf("\r\nshift=%0lx\r\n",timeMarkHighShiftRight1);
				timeMarkHighShiftRight1=timeMarkHighShiftRight1+(timeMarkHighShiftRight1>>31);
			#if XKJSDEBUG
				printf("\r\nshift=%0lx\r\n",timeMarkHighShiftRight1);
			#endif
				//timeMarkHighShiftRight1=0EC1E867
	
				timeMarkUse=ntohl(timeMarkHighShiftRight1);//in little ed
			#if XKJSDEBUG
				printf("eeee\r\n--%0lx",timeMarkUse);
				//timeMarkUse=htonl(timeMarkHighShiftRight1);//in big ed
				printf("\r\nkkk --%0lx--%0lx--%0lx--%0lx--",timeMarkHighShiftRight1<<24,timeMarkHighShiftRight1<<16,timeMarkHighShiftRight1<<8,timeMarkHighShiftRight1);
			#endif
				timeMarkUse=((timeMarkHighShiftRight1<<24)&0xff000000)+((timeMarkHighShiftRight1<<8)&0x00ff0000)+((timeMarkHighShiftRight1>>8)&0x0000ff00)+((timeMarkHighShiftRight1>>24)&0x000000ff);
				timeMarkUse1=timeMarkUse;
				//timeMarkUSE=0x67E8C10E
				timeMarkUseLow=timeMarkUse&0x0000ffff;//=0000C10E
				timeMarkUseHigh=timeMarkUse&0xffff0000;//=67E80000
			#if XKJSDEBUG
				printf("\r\nuse=%0lx,uselow=%0x,usehi=%0x\r\n",timeMarkUse,timeMarkUseLow,timeMarkUseHigh);
			#endif	
	
				my2_edx=timeMarkUseLow;
				my2_eax=3;
				my2_ecx=0;
				my2_esi=my2_esi^my2_esi;
				for(k=0;k<0x20;k++)
				{
					zhi_eax=my2_eax;
					zhi_ecx=my2_ecx;
			#if XKJSDEBUG
					printf("--zhi=%0lx,%0lx",zhi_eax,zhi_ecx);
			#endif
					my2_eax=(my2_eax&0xffffff00)+((timeMarkUse1>>8*my2_eax)&0x000000ff);
					my2_ecx=(my2_ecx&0xffffff00)+((tempUse1>>8*my2_ecx)&0x000000ff);
			#if XKJSDEBUG
					printf("\r\ntest 1 -- %0lx---%0lx",my2_eax,my2_ecx);
			#endif
					my2_edx=(my2_edx&0xffffff00)+ (my2_eax&0x000000ff);
			#if XKJSDEBUG
					printf("---%0lx",my2_edx);
			#endif
					my2_ecx=(my2_ecx&0xffffff00)+(((my2_ecx<<24)<<1)>>24);
			#if XKJSDEBUG
					printf("-%0lx",my2_ecx);
			#endif
					my2_edx=(my2_edx&0xffffff00)+((my2_edx&0x000000ff)&0x00000001);
			#if XKJSDEBUG
					printf("-%0lx",my2_edx);
			#endif
					my2_ecx=(my2_ecx&0xffffff00)+((my2_ecx&0x000000ff)|(my2_edx&0x000000ff));
			#if XKJSDEBUG
					printf("-%0lx",my2_ecx);
			#endif
					my2_eax=(my2_eax&0xffffff00)+((my2_eax&0x000000ff)>>1);
					my2_esi++;
			#if XKJSDEBUG
					printf("-%0lx-%0lx",my2_eax,my2_esi);
			#endif
					if(zhi_eax==0)
						timeMarkUse1=(timeMarkUse1&0xffffff00)+(my2_eax&0x000000ff);
					else if(zhi_eax==1)
						timeMarkUse1=(timeMarkUse1&0xffff00ff)+((my2_eax<<8)&0x0000ff00);
					else if(zhi_eax==2)
						timeMarkUse1=(timeMarkUse1&0xff00ffff)+((my2_eax<<16)&0x00ff0000);
					else
						timeMarkUse1=(timeMarkUse1&0x00ffffff)+((my2_eax<<24)&0xff000000);
			#if XKJSDEBUG
					printf("-%0lx",timeMarkUse1);
			#endif
					if(zhi_ecx==0)
						tempUse1=(tempUse1&0xffffff00)+(my2_ecx&0x000000ff);
					else if(zhi_ecx==1)
						tempUse1=(tempUse1&0xffff00ff)+((my2_ecx<<8)&0x0000ff00);
					else if(zhi_ecx==2)
						tempUse1=(tempUse1&0xff00ffff)+((my2_ecx<<16)&0x00ff0000);
					else
						tempUse1=(tempUse1&0x00ffffff)+((my2_ecx<<24)&0xff000000);
			
			#if XKJSDEBUG
					printf("-%0lx",tempUse1);
			#endif
					my2_eax=my2_esi;
			#if XKJSDEBUG
					printf("-&&%0lx",my2_eax);
			#endif
			
					//my2_edx=my2_edx+((my2_eax>>31)<<31);  //cdq call
					my2_edx=0;
					my2_edx=my2_edx&0x00000007;
					my2_ecx=3;
			#if XKJSDEBUG
					printf("-%0lx-%0lx",my2_edx,my2_ecx);
			#endif
					my2_eax=my2_eax+my2_edx;
			#if XKJSDEBUG
					printf("-RR%0lx",my2_eax);
			#endif
					my2_edx=my2_esi;
			#if XKJSDEBUG
					printf("-%0lx",my2_edx);
			#endif
					my2_eax=my2_eax/8;
					//if((my2_eax>>31)==1)
						 //my2_eax=(my2_eax>>3)+0x80000000;
					//else
				 		//my2_eax=(my2_eax>>3);
			#if XKJSDEBUG
					printf("zhongyao--%0lx-%0lx",my2_ecx,my2_eax);
			#endif
			
					my2_ecx=my2_ecx-my2_eax;
			#if XKJSDEBUG
					printf("-%0lx",my2_ecx);
			#endif
					my2_edx=my2_edx&0x80000003;
					my2_eax=my2_ecx;
					if((my2_eax>>31)==1)
					{
			#if XKJSDEBUG
						printf("-jinqu??()**&^%-----");
			#endif
						my2_edx--;
						my2_edx=my2_edx|0xfffffffc;
						my2_edx++;
					}
			#if XKJSDEBUG
					printf("-%0lx-%0lx",my2_edx,my2_eax);
			#endif
					my2_ecx=my2_edx;
			#if XKJSDEBUG
					printf("-%0lx",my2_ecx);
					printf("\r\n nmy2---sign1=%0lx,sign2=%0lx\r\n",timeMarkUse1,tempUse1);
			#endif
				}
			#if XKJSDEBUG
				printf("\r\nguanjian timeUse=%0lx",tempUse1);
			#endif
	
				my2_cl=tempUse1&0x000000ff;
				my2_cl=my2_cl>>2;
				my2_char_arr[0]=my2_cl;
				my2_cl=tempUse1&0x000000ff;
				my2_cl=my2_cl&0x03;
				my2_cl=my2_cl<<4;
				my2_char_arr[1]=my2_cl;
				my2_dl=(tempUse1>>8)&0x000000ff;
				my2_dl=my2_dl>>4;
				my2_dl=my2_dl|my2_cl;
				my2_char_arr[1]=my2_dl;
				my2_cl=(tempUse1>>8)&0x000000ff;
				my2_cl=my2_cl&0x0f;
				my2_cl=my2_cl<<2;
				my2_char_arr[2]=my2_cl;
				my2_dl=(tempUse1>>16)&0x000000ff;
				my2_dl=my2_dl>>6;
				my2_dl=my2_dl|my2_cl;
				my2_char_arr[2]=my2_dl;
				my2_cl=(tempUse1>>16)&0x000000ff;
				my2_cl=my2_cl&0x3f;
				my2_char_arr[3]=my2_cl;
				my2_dl=(tempUse1>>24)&0x000000ff;
				my2_dl=my2_dl>>2;
				my2_char_arr[4]=my2_dl;
				my2_cl=(tempUse1>>24)&0x000000ff;
				my2_cl=my2_cl&0x03;
				my2_cl=my2_cl<<4;
				my2_char_arr[5]=my2_cl;
	
				for(m=0;m<6;m++)
				{
					my2_dl=my2_char_arr[m];
					my2_dl=my2_dl+0x20;
					my2_char_arr[m]=my2_dl;
					if(my2_dl>=0x40){
						my2_dl++;
						my2_char_arr[m]=my2_dl;
					}
		
				}
			#if XKJSDEBUG
				printf("\r\nfront key:");
			#endif
			#if XKJSDEBUG
				for(m=0;m<6;m++)
				{
					printf("\r\n%c--%0x",my2_char_arr[m],my2_char_arr[m]);
				}

				printf("\r\n");
				printf("\r\nlen=%d,%s\r\n",totallen,globalName);
			#endif
				totalInt[0]=timeMarkUse;
				tempi1=totallen/4;//4
				tempi2=totallen%4;//0
				int i;
				int j=-1;
				for(i=1;i<=tempi1;i++)
				{
					j+=4;
					totalInt[i]=totalInt[i]+globalName[j];
					totalInt[i]=(totalInt[i]<<8)+globalName[j-1];
					totalInt[i]=(totalInt[i]<<8)+globalName[j-2];
					totalInt[i]=(totalInt[i]<<8)+globalName[j-3];
	
				}
				if(tempi2==0)
					totalInt[i]=0x80;
				else if(tempi2==1)
				{
					totalInt[i]=0x80;
					totalInt[i]=(totalInt[i]<<8)+globalName[totallen-1];
				}
				else if(tempi2==2)
				{
					totalInt[i]=0x80;
					totalInt[i]=(totalInt[i]<<8)+globalName[totallen-1];
					totalInt[i]=(totalInt[i]<<8)+globalName[totallen-2];
				}
				else
				{
					totalInt[i]=0x80;
					totalInt[i]=(totalInt[i]<<8)+globalName[totallen-1];
					totalInt[i]=(totalInt[i]<<8)+globalName[totallen-2];
					totalInt[i]=(totalInt[i]<<8)+globalName[totallen-3];
				}
				//totalInt[14]=totalInt[14]+0xA0;
				totalInt[14]=totalInt[14]+(usernamelen+0x0a)*0x08;

			#if XKJSDEBUG
				for(tempk=0;tempk<16;tempk++)
				{
					printf("\r\n%08x",totalInt[tempk]);
				}
			#endif
				countKey_sign=0;
				for(tempi=0;tempi<4;tempi++)
				{
					tempj=0;
					//countKey_sign=countKey_sign+tempi*16;
					countKey_sign=tempi*16;
					for(tempj=0;tempj<0x10;tempj++)
					{
						my_eax=tempj;
			#if XKJSDEBUG
						printf("--count=%d--",tempj);
			#endif
						temp_eax=my_eax%4;
			#if XKJSDEBUG
						printf("--temp_eax=%d--",temp_eax);
			#endif
						if(temp_eax==1)
							my_edx=temp_eax+2;
						else if(temp_eax==3)
							my_edx=temp_eax-2;
						else
							my_edx=temp_eax;
			
						if(temp_eax==0)
						{
							my_eax=2;
							my_ecx=1;
						}
						else if(temp_eax==1)
						{
							my_eax=1;
							my_ecx=0;
						}
						else if(temp_eax==2)
						{
							my_eax=0;
							my_ecx=3;
						}
						else
						{
							my_eax=3;
							my_ecx=2;
						}
			
						if(temp_eax==0)
							my_ecx2=3;
						else if(temp_eax==1)
							my_ecx2=2;
						else if(temp_eax==2)
							my_ecx2=1;
						else
							my_ecx2=0;
			
				#if XKJSDEBUG
						printf("--eax=%d,ecx=%d,ecx2=%d",my_eax,my_ecx,my_ecx2);
				#endif
						shiftCharValue=shiftKey[temp_eax+tempi*4];
				#if XKJSDEBUG
						printf("\r\n%0x",shiftCharValue);
				#endif
						my_ebp=countKey[my_ecx];
						my_edi=countKey[my_eax];
						my_ecx2=countKey[my_ecx2];
						switch(tempi)
						{
						case 0:
							my_eax=my_ebp;
							my_edi=my_edi&my_ebp;
							my_eax=~my_eax;
							my_eax=my_eax&my_ecx2;
							my_eax=my_eax|my_edi;
							my_edi=totalInt[tempj];
					
							my_eax=my_eax+my_edi;
							my_edi=countKey[my_edx];
							my_ecx2=globalKey[countKey_sign];
							my_ecx2=my_ecx2+my_edi;
							my_eax=my_eax+my_ecx2;
							my_ebx=my_eax;
							my_ebx=my_ebx>>(0x20-shiftCharValue);
							my_eax=my_eax<<(0x0+shiftCharValue);
							my_ebx=my_ebx|my_eax;
							my_ebx=my_ebx+my_ebp;
							my_eax=my_ebx;
							countKey[my_edx]=my_eax;
						#if XKJSDEBUG
							printf("\r\nmy_edx=%0x,value=%0lx",my_edx,countKey[my_edx]);
						#endif
							countKey_sign++;
							break;
						case 1:
							my_eax=my_ecx2;
							my_ecx2=my_ecx2&my_ebp;
							my_eax=~my_eax;
							my_eax=my_eax&my_edi;
							my_eax=my_eax|my_ecx2;
							my_ecx2=my_ecx2^my_ecx2;
							my_ecx2=valueKey[tempj+(tempi-1)*16];
							my_edi=totalInt[my_ecx2];
					
							my_eax=my_eax+my_edi;
							my_edi=countKey[my_edx];
							my_ecx2=globalKey[countKey_sign];
							my_ecx2=my_ecx2+my_edi;
							my_eax=my_eax+my_ecx2;
							my_ebx=my_eax;
							my_ebx=my_ebx>>(0x20-shiftCharValue);
							my_eax=my_eax<<(0x0+shiftCharValue);
							my_ebx=my_ebx|my_eax;
							my_ebx=my_ebx+my_ebp;
							my_eax=my_ebx;
							countKey[my_edx]=my_eax;
						#if XKJSDEBUG
							printf("\r\nmy_edx=%0x,value1=%0lx",my_edx,countKey[my_edx]);
						#endif
							countKey_sign++;
							break;
						case 2:
							my_eax=my_eax^my_eax;
							my_ecx2=my_ecx2^my_edi;
							my_eax=valueKey[tempj+(tempi-1)*16];
						#if XKJSDEBUG
							printf("vk=%0x",my_eax);
						#endif
							my_ecx2=my_ecx2^my_ebp;
							my_eax=totalInt[my_eax];
							my_eax=my_eax+my_ecx2;
					
					
							//my_eax=my_eax+my_edi;
							my_edi=countKey[my_edx];
							my_ecx2=globalKey[countKey_sign];
							my_ecx2=my_ecx2+my_edi;
							my_eax=my_eax+my_ecx2;
							my_ebx=my_eax;
							my_ebx=my_ebx>>(0x20-shiftCharValue);
							my_eax=my_eax<<(0x0+shiftCharValue);
							my_ebx=my_ebx|my_eax;
							my_ebx=my_ebx+my_ebp;
							my_eax=my_ebx;
							countKey[my_edx]=my_eax;
						#if XKJSDEBUG
							printf("\r\nmy_edx=%0x,value2=%0lx",my_edx,countKey[my_edx]);
						#endif
							countKey_sign++;
							break;
						default:
							my_eax=my_eax^my_eax;
							my_eax=valueKey[tempj+(tempi-1)*16];
							my_ecx2=~my_ecx2;
							my_eax=totalInt[my_eax];
							my_ecx2=my_ecx2|my_ebp;
							my_ecx2=my_ecx2^my_edi;
							my_eax=my_eax+my_ecx2;
					
					
							my_edi=countKey[my_edx];
							my_ecx2=globalKey[countKey_sign];
							my_ecx2=my_ecx2+my_edi;
							my_eax=my_eax+my_ecx2;
							my_ebx=my_eax;
							my_ebx=my_ebx>>(0x20-shiftCharValue);
							my_eax=my_eax<<(0x0+shiftCharValue);
							my_ebx=my_ebx|my_eax;
							my_ebx=my_ebx+my_ebp;
							my_eax=my_ebx;
							countKey[my_edx]=my_eax;
						#if XKJSDEBUG
							printf("\r\nmy_edx=%0x,value3=%0lx",my_edx,countKey[my_edx]);
						#endif
							countKey_sign++;
							break;	
						}
					}
				}
			#if XKJSDEBUG
				printf("\r\nkey1 value");
				for(i=0;i<4;i++)
				{
					printf("\r\n%08lx",countKey[i]);
				} 
			#endif
				for(i=0;i<4;i++)
				{
					my_esi=countKeyTemp[i];
					my_ebp=countKey[i];
					my_ebp=my_ebp+my_esi;
					countKey[i]=my_ebp;
				}
			#if XKJSDEBUG
				printf("\r\nkey2 value");
				for(i=0;i<4;i++)
				{
					printf("\r\n%08lx",countKey[i]);
				} 
				printf("\r\n");
			#endif
				sprintf(bufShare,"%02x%02x%02x%02x",countKey[0]&0x000000ff,(countKey[0]>>8)&0x000000ff,(countKey[0]>>16)&0x000000ff,(countKey[0]>>24)&0x000000ff);
				for(i=1;i<4;i++)
				{
					sprintf(bufShare,"%s%02x%02x%02x%02x",bufShare,
						countKey[i]&0x000000ff,
						(countKey[i]>>8)&0x000000ff,
						(countKey[i]>>16)&0x000000ff,
						(countKey[i]>>24)&0x000000ff
					);
				}
			#if XKJSDEBUG
				printf("\r\nshareKey=%s",bufShare);
			#endif
				strncpy(bufRealKey2,bufShare,4);
			#if XKJSDEBUG
				printf("\r\nbufRealKey2=%s",bufRealKey2);
			#endif
  				ourEncryptKey[0]=0x0d;
 				ourEncryptKey[1]=0x0a;
  				for(m=0;m<6;m++)
				{
					ourEncryptKey[m+2]=my2_char_arr[m];;
				}
				ourEncryptKey[8]=bufRealKey2[0];
				ourEncryptKey[9]=bufRealKey2[1];
				for(m=0;m<usernamelen;m++)
				{
					ourEncryptKey[10+m]=username[m];
				}
				ourEncryptKey[10+usernamelen]='\0';
			#if XKJSDEBUG
				printf("\r\nkey is \r\n");
				for(m=0;m<10+usernamelen;m++)
				{
					printf("%c---%0x\r\n",ourEncryptKey[m], ourEncryptKey[m]);
				}
			#endif
				nvram_bufset(RT2860_NVRAM, "wan_pppoe_user_mm", ourEncryptKey);
			}
#endif
		}
		else if (!strncmp(ctype, "L2TP", 5)) {
			char_t *l2tp_opmode, *l2tp_optime;

			l2tp_srv = websGetVar(wp, T("l2tpServer"), T(""));
			user = websGetVar(wp, T("l2tpUser"), T(""));
			pass = websGetVar(wp, T("l2tpPass"), T(""));
			l2tp_mode = websGetVar(wp, T("l2tpMode"), T("0"));
			ip = websGetVar(wp, T("l2tpIp"), T(""));
			nm = websGetVar(wp, T("l2tpNetmask"), T(""));
			gw = websGetVar(wp, T("l2tpGateway"), T(""));
			l2tp_opmode = websGetVar(wp, T("l2tpOPMode"), T(""));
			if (0 == strcmp(l2tp_opmode, "OnDemand"))
				l2tp_optime = websGetVar(wp, T("l2tpIdleTime"), T(""));
			else
				l2tp_optime = websGetVar(wp, T("l2tpRedialPeriod"), T(""));
			nvram_bufset(RT2860_NVRAM, "wanConnectionMode", ctype);
			nvram_bufset(RT2860_NVRAM, "wan_l2tp_server", l2tp_srv);
			nvram_bufset(RT2860_NVRAM, "wan_l2tp_user", user);
			nvram_bufset(RT2860_NVRAM, "wan_l2tp_pass", pass);
			nvram_bufset(RT2860_NVRAM, "wan_l2tp_mode", l2tp_mode);
			nvram_bufset(RT2860_NVRAM, "wan_l2tp_opmode", l2tp_opmode);
			nvram_bufset(RT2860_NVRAM, "wan_l2tp_optime", l2tp_optime);
			if (!strncmp(l2tp_mode, "0", 2)) {
				nvram_bufset(RT2860_NVRAM, "wan_l2tp_ip", ip);
				nvram_bufset(RT2860_NVRAM, "wan_l2tp_netmask", nm);
				nvram_bufset(RT2860_NVRAM, "wan_l2tp_gateway", gw);
			}
		}
		else if (!strncmp(ctype, "PPTP", 5)) {
			char_t *pptp_opmode, *pptp_optime;

			pptp_srv = websGetVar(wp, T("pptpServer"), T(""));
			user = websGetVar(wp, T("pptpUser"), T(""));
			pass = websGetVar(wp, T("pptpPass"), T(""));
			pptp_mode = websGetVar(wp, T("pptpMode"), T("0"));
			ip = websGetVar(wp, T("pptpIp"), T(""));
			nm = websGetVar(wp, T("pptpNetmask"), T(""));
			gw = websGetVar(wp, T("pptpGateway"), T(""));
			pptp_opmode = websGetVar(wp, T("pptpOPMode"), T(""));
			if (0 == strcmp(pptp_opmode, "OnDemand"))
				pptp_optime = websGetVar(wp, T("pptpIdleTime"), T(""));
			else
				pptp_optime = websGetVar(wp, T("pptpRedialPeriod"), T(""));

			nvram_bufset(RT2860_NVRAM, "wanConnectionMode", ctype);
			nvram_bufset(RT2860_NVRAM, "wan_pptp_server", pptp_srv);
			nvram_bufset(RT2860_NVRAM, "wan_pptp_user", user);
			nvram_bufset(RT2860_NVRAM, "wan_pptp_pass", pass);
			nvram_bufset(RT2860_NVRAM, "wan_pptp_mode", pptp_mode);
			nvram_bufset(RT2860_NVRAM, "wan_pptp_opmode", pptp_opmode);
			nvram_bufset(RT2860_NVRAM, "wan_pptp_optime", pptp_optime);
			if (!strncmp(pptp_mode, "0", 2)) {
				nvram_bufset(RT2860_NVRAM, "wan_pptp_ip", ip);
				nvram_bufset(RT2860_NVRAM, "wan_pptp_netmask", nm);
				nvram_bufset(RT2860_NVRAM, "wan_pptp_gateway", gw);
			}
		}
#ifdef CONFIG_USER_3G
		else if (!strncmp(ctype, "3G", 3)) {
			//usb3g_mode = websGetVar(wp, T("OPMode3G"), T(""));
			usb3g_dev = websGetVar(wp, T("Dev3G"), T(""));
			usb3g_apn = websGetVar(wp, T("APN3G"), T(""));
			usb3g_pin = websGetVar(wp, T("PIN3G"), T(""));
			usb3g_dial = websGetVar(wp, T("Dial3G"), T(""));
			usb3g_user = websGetVar(wp, T("User3G"), T(""));
			usb3g_pass = websGetVar(wp, T("Password3G"), T(""));

			//nvram_bufset(RT2860_NVRAM, "wan_3g_opmode", usb3g_mode);
			nvram_bufset(RT2860_NVRAM, "wan_3g_dev", usb3g_dev);
			nvram_bufset(RT2860_NVRAM, "wan_3g_apn", usb3g_apn);
			nvram_bufset(RT2860_NVRAM, "wan_3g_pin", usb3g_pin);
			nvram_bufset(RT2860_NVRAM, "wan_3g_dial", usb3g_dial);
			nvram_bufset(RT2860_NVRAM, "wan_3g_user", usb3g_user);
			nvram_bufset(RT2860_NVRAM, "wan_3g_pass", usb3g_pass);
			nvram_bufset(RT2860_NVRAM, "wanConnectionMode", ctype);
		}
#endif
		else {
			AppReturnNOK(wp, "Unknown Connection Type");
			return;
		}
	}

	nvram_commit(RT2860_NVRAM);

	sleep(2);
	AppReturnOK(wp);

#if defined(CONFIG_RALINK_MT7620)
	doSystem("lktos_networkconfig mtk7620 initwan");
#else
	doSystem("lktos_networkconfig ra5350 initwan");
	//initInternet();
#endif
}


static int AppGetNetworkStatus(webs_t wp, char_t *path, char_t *query)
{
	char *opmode = (char *)nvram_bufget(RT2860_NVRAM, "OperationMode");
	char *wanmode = (char *)nvram_bufget(RT2860_NVRAM, "wanConnectionMode");
	char wan_ip[16];
	char long_buf[4096];
	char tmpBuf[100];
	FILE *fp;
 	char *p = NULL;

	if (atoi(opmode)==1) {		//GW mode
#if defined (CONFIG_USER_CDRKING)
		system("mii_mgr -g -p 0 -r 1 > /tmp/wanConnectStatus");
					
		fp = fopen("/tmp/wanConnectStatus", "r");
		if (!fp) return NULL;
		while(fgets(long_buf, 512, fp)) {
			p = strstr(long_buf, "=");	
			*(p+2+3)='\0';
			if(((atoi(p+2+2)) & 0x0002))  p=NULL;	
		}
					
		fclose(fp); 	
		if (p==NULL) {//Is connected
			if (!strcmp(wanmode, "DHCP")) {
				if (getIfIp("eth2.2", wan_ip) == -1)
					sprintf(tmpBuf, "%s", "MM_connect_no");//Can not get ip address
				else
					sprintf(tmpBuf, "%s", "MM_connect_yes");
			}
			else if (!strcmp(wanmode, "STATIC")) {
				sprintf(tmpBuf, "%s", "MM_connect_yes");
			}
			else {
				if (getIfIp("ppp0", wan_ip) == -1)
					sprintf(tmpBuf, "%s", "MM_connect_no");//Can not get ip address
				else
					sprintf(tmpBuf, "%s", "MM_connect_yes");
			}
		}
		else
			sprintf(tmpBuf, "%s", "MM_connect_no");
#else
		system("phyval 0 > /tmp/wanConnectStatus");
			
		fp = fopen("/tmp/wanConnectStatus", "r");
		if (!fp) 
			return -1;
		
		while(fgets(long_buf, 512, fp)){
			p = strstr(long_buf, "Link is down");
			if (p != NULL)
				break;
		}
			
		fclose(fp);		

		if (p==NULL) {//Is connected
			if (!strcmp(wanmode, "DHCP")) {
					if (getIfIp("eth2.2", wan_ip) == -1)
						sprintf(tmpBuf, "%s", "unreachable");//Can not get ip address
					else
						sprintf(tmpBuf, "%s", "reachable");
			}
			else if (!strcmp(wanmode, "STATIC")) {
					sprintf(tmpBuf, "%s", "reachable");
			}
			else {
					if (getIfIp("ppp0", wan_ip) == -1)
						sprintf(tmpBuf, "%s", "unreachable");//Can not get ip address
					else
						sprintf(tmpBuf, "%s", "reachable");
				}
		}
		else
			sprintf(tmpBuf, "%s", "unreachable");
#endif
	}
	else if (atoi(opmode)==3) {			
			//First check the state of apclient
			if (!strcmp(wanmode, "DHCP")) {				
				if (getIfIp("apcli0", wan_ip) == -1)
					sprintf(tmpBuf, "%s", "unreachable");//Can not get ip address
				else
					sprintf(tmpBuf, "%s", "reachable");
			}
			else if (!strcmp(wanmode, "STATIC")) {
				sprintf(tmpBuf, "%s", "reachable");
			}
			else {				
				if (getIfIp("ppp0", wan_ip) == -1) //Can not get ip address
					sprintf(tmpBuf, "%s", "unreachable");
				else
					sprintf(tmpBuf, "%s", "reachable");
			}
	}
	else{
		sprintf(tmpBuf, "%s", "unknown");
	}

	APP_TRACE("networkstatus is %s\n", tmpBuf);
	
	AppReturnHeader(wp);
	websWrite(wp, T("<?xml version=\"1.0\" encoding=\"UTF-8\"?>"));		
	websWrite(wp, T("<response>"));	
	websWrite(wp, T("<retcode>"));	
	websWrite(wp, T("0"));		
	websWrite(wp, T("</retcode>"));	
	websWrite(wp, T("<retdesc>"));	
	websWrite(wp, T("success"));	
	websWrite(wp, T("</retdesc>"));
	websWrite(wp, T("<data>"));
	websWrite(wp, T("<networkstatus>%s</networkstatus>\n"), tmpBuf);
	websWrite(wp, T("</data>"));	
	websWrite(wp, T("</response>"));	
	websDone(wp, 200);

	return 0;

}

static int AppGetDevInfo(webs_t wp, char_t *path, char_t *query)
{
	const char *modelname = nvram_bufget(RT2860_NVRAM, "HostName");
	const char *modelnumber = nvram_bufget(RT2860_NVRAM, "Platform");	
	const char *firmwareversion = "V1.0";
	const char *connectionTypewan = nvram_bufget(RT2860_NVRAM, "wanConnectionMode");
	const char *ipaddresslan = nvram_bufget(RT2860_NVRAM, "lan_ipaddr");
	char serialnumber[18];
	char ipaddresswan[16];
	
	//lan mac
	if ( -1 == getIfMac(getLanIfName(), serialnumber))
		strcpy(serialnumber, "00:00:00:00:00:00"); 
	APP_TRACE("serialnumber(lan mac) is %s\n", serialnumber);
	
	//wan ip
	if (-1 == getIfIp(getWanIfNamePPP(), ipaddresswan)) 
		strcpy(ipaddresswan, "0.0.0.0");
	APP_TRACE("ipaddresswan(wan ip) is %s\n", ipaddresswan);

	AppReturnHeader(wp);
	websWrite(wp, T("<?xml version=\"1.0\" encoding=\"UTF-8\"?>"));		
	websWrite(wp, T("<response>"));	
	websWrite(wp, T("<retcode>"));	
	websWrite(wp, T("0"));		
	websWrite(wp, T("</retcode>"));	
	websWrite(wp, T("<retdesc>"));	
	websWrite(wp, T("success"));	
	websWrite(wp, T("</retdesc>"));
	websWrite(wp, T("<data>"));
	websWrite(wp, T("<modelname>%s</modelname>\n"), modelname);
	websWrite(wp, T("<modelnumber>%s</modelnumber>\n"), modelnumber);
	websWrite(wp, T("<serialnumber>%s</serialnumber>\n"), serialnumber);
	websWrite(wp, T("<Firmwareversion>%s</Firmwareversion>\n"), firmwareversion);
	websWrite(wp, T("<connectionTypewan>%s</connectionTypewan>\n"), connectionTypewan);
	websWrite(wp, T("<ipaddresslan>%s</ipaddresslan>\n"), ipaddresslan);
	websWrite(wp, T("<ipaddresswan>%s</ipaddresswan>\n"), ipaddresswan);
	websWrite(wp, T("<deviceipaddress>%s</deviceipaddress>\n"), wp->ipaddr);
	websWrite(wp, T("</data>"));	
	websWrite(wp, T("</response>"));	
	websDone(wp, 200);	

}

static int AppGetLanListInfo(webs_t wp, char_t *path, char_t *query)
{
	FILE *fp;
	struct dhcpOfferedAddr {
		unsigned char hostname[16];
		unsigned char mac[16];
		unsigned long ip;
		unsigned long expires;
	} lease;
	int index = 1;

	doSystem("killall -q -USR1 udhcpd");

	fp = fopen("/var/udhcpd.leases", "r");
	if (NULL == fp)
	{
		AppReturnNOK(wp, T("open leases file error"));
		return -1;
	}
	
	AppReturnHeader(wp);
	websWrite(wp, T("<?xml version=\"1.0\" encoding=\"UTF-8\"?>"));		
	websWrite(wp, T("<response>"));	
	websWrite(wp, T("<retcode>"));	
	websWrite(wp, T("0"));		
	websWrite(wp, T("</retcode>"));	
	websWrite(wp, T("<retdesc>"));	
	websWrite(wp, T("success"));	
	websWrite(wp, T("</retdesc>"));
	websWrite(wp, T("<data>"));
	
	while (fread(&lease, 1, sizeof(lease), fp) == sizeof(lease)) {
		
		if (lease.mac[1]==0 && lease.mac[2]==0 && lease.mac[3]==0 && lease.mac[4]==0 && lease.mac[5]==0)
			continue;
				
		if (strlen(lease.hostname) > 0)
			websWrite(wp, T("<device><devicename>%-16s</devicename><deviceid>%d</deviceid></device>\n"), lease.hostname, index);
		else
			websWrite(wp, T("<device><devicename>Unknown</devicename><deviceid>%d</deviceid></device>\n"), index);
		
		index++;
	}
	websWrite(wp, T("</data>"));	
	websWrite(wp, T("</response>"));	
	websDone(wp, 200);


	fclose(fp);
	return 0;
}


static void AppLimitStatus(webs_t wp, char *mac)
{
	char *Curlimit = (char *)nvram_bufget(RT2860_NVRAM, "LimitMac");

	APP_TRACE("curmac is %s\n", mac);
	
	if (strstr(Curlimit, mac))
		websWrite(wp, T("<internetenable>Limit</internetenable>"));	
	else
		websWrite(wp, T("<internetenable>Allow</internetenable>"));	

	return;
	
}
static int AppGetLanDetailInfo(webs_t wp, char_t *path, char_t *query)
{
	char *num = websGetVar(wp, T("num"), T(""));
	FILE *fp;
	struct dhcpOfferedAddr {
		unsigned char hostname[16];
		unsigned char mac[16];
		unsigned long ip;
		unsigned long expires;
	} lease;
	int index = 1;
	struct in_addr addr;
	char curMac[50];

	doSystem("killall -q -USR1 udhcpd");

	fp = fopen("/var/udhcpd.leases", "r");
	if (NULL == fp)
	{
		AppReturnNOK(wp, T("open leases file error"));
		return -1;
	}
	
	AppReturnHeader(wp);
	websWrite(wp, T("<?xml version=\"1.0\" encoding=\"UTF-8\"?>"));		
	websWrite(wp, T("<response>"));	
	websWrite(wp, T("<retcode>"));	
	websWrite(wp, T("0"));		
	websWrite(wp, T("</retcode>"));	
	websWrite(wp, T("<retdesc>"));	
	websWrite(wp, T("success"));	
	websWrite(wp, T("</retdesc>"));
	websWrite(wp, T("<data>"));

	APP_TRACE("num is %s\n", num);
	
	while (fread(&lease, 1, sizeof(lease), fp) == sizeof(lease)) {

		if (lease.mac[1]==0 && lease.mac[2]==0 && lease.mac[3]==0 && lease.mac[4]==0 && lease.mac[5]==0)
			continue;
		
		if (index == atoi(num))
		{
				if (strlen(lease.hostname) > 0)
				{
					addr.s_addr = lease.ip;
					websWrite(wp, T("<devicename>%-16s</devicename><deviceid>%d</deviceid><firmwareversion>V1.0</firmwareversion><ipaddress>%s</ipaddress><status>connected</status><macaddress>%02X:%02X:%02X:%02X:%02X:%02X</macaddress>\n"), 
								lease.hostname, index, inet_ntoa(addr), lease.mac[0],lease.mac[1],lease.mac[2],lease.mac[3],lease.mac[4],lease.mac[5]);
					sprintf(curMac, "%02X:%02X:%02X:%02X:%02X:%02X", lease.mac[0],lease.mac[1],lease.mac[2],lease.mac[3],lease.mac[4],lease.mac[5]);
					AppLimitStatus(wp, curMac);
				}
				else
				{
					addr.s_addr = lease.ip;
					websWrite(wp, T("<devicename>Unknown</devicename><deviceid>%d</deviceid><firmwareversion>V1.0</firmwareversion><ipaddress>%s</ipaddress><status>connected</status><macaddress>%02X:%02X:%02X:%02X:%02X:%02X</macaddress>\n"), 
								index, inet_ntoa(addr), lease.mac[0],lease.mac[1],lease.mac[2],lease.mac[3],lease.mac[4],lease.mac[5]);
					sprintf(curMac, "%02X:%02X:%02X:%02X:%02X:%02X", lease.mac[0],lease.mac[1],lease.mac[2],lease.mac[3],lease.mac[4],lease.mac[5]);
					AppLimitStatus(wp, curMac);
				}
		}
		
		index++;
	}
	
	websWrite(wp, T("</data>"));	
	websWrite(wp, T("</response>"));	
	websDone(wp, 200);


	fclose(fp);
	return 0;
}

static int AppLimitLanPc(webs_t wp, char_t *path, char_t *query)
{
	FILE *fp;
	struct dhcpOfferedAddr {
		unsigned char hostname[16];
		unsigned char mac[16];
		unsigned long ip;
		unsigned long expires;
	} lease;
	
	int index = 1;
	char CMD[256];
	char limitmac[32] = {0};
	char maclist[1024];
	char *Curlimit;
	int i=0;
	int j=0;
	int count=0;
	int rule_count=0;
	int *deleArray;
	char *tmp;

	char *id = websGetVar(wp, T("id"), T(""));
	char *action = websGetVar(wp, T("internetenable"), T(""));
	APP_TRACE("limit pc%s, action is %s\n", id, action);
	
	doSystem("killall -q -USR1 udhcpd");

	fp = fopen("/var/udhcpd.leases", "r");
	if (NULL == fp)
	{
		AppReturnNOK(wp, T("open leases file error"));
		return -1;
	}

	
	while (fread(&lease, 1, sizeof(lease), fp) == sizeof(lease)) {

		if (lease.mac[1]==0 && lease.mac[2]==0 && lease.mac[3]==0 && lease.mac[4]==0 && lease.mac[5]==0)
			continue;
		
		if (index == atoi(id))
		{
			if (!strcmp(action, "no"))
			{
				sprintf(CMD, "iptables -I FORWARD -m mac --mac-source %02X:%02X:%02X:%02X:%02X:%02X -j DROP", lease.mac[0],lease.mac[1],lease.mac[2],lease.mac[3],lease.mac[4],lease.mac[5]);
				system(CMD);
				APP_TRACE("limit, %s\n", CMD);
				
				sprintf(limitmac, "%02X:%02X:%02X:%02X:%02X:%02X", lease.mac[0],lease.mac[1],lease.mac[2],lease.mac[3],lease.mac[4],lease.mac[5]);
				APP_TRACE("limitmac, %s\n", limitmac);

				if(( Curlimit = (char *)nvram_bufget(RT2860_NVRAM, "LimitMac")) && strlen( Curlimit))
					snprintf(maclist, sizeof(maclist), "%s;%s", Curlimit, limitmac);
				else
					snprintf(maclist, sizeof(maclist), "%s", limitmac);
				
				APP_TRACE("maclist, %s\n", maclist);
				
				nvram_set(RT2860_NVRAM, "LimitMac", maclist);
				nvram_commit(RT2860_NVRAM);
				
			}
			else if (!strcmp(action, "yes"))
			{
				sprintf(CMD, "iptables -D FORWARD -m mac --mac-source %02X:%02X:%02X:%02X:%02X:%02X -j DROP", lease.mac[0],lease.mac[1],lease.mac[2],lease.mac[3],lease.mac[4],lease.mac[5]);
				system(CMD);
				APP_TRACE("allow, %s\n", CMD);

				Curlimit = (char *)nvram_bufget(RT2860_NVRAM, "LimitMac");
				APP_TRACE("Curlimit %s, strlen(Curlimit) %d\n", Curlimit, strlen(Curlimit));

				tmp = Curlimit;
				while(*tmp++)
				{
					if (*tmp== ';')
						rule_count++;
				}
				APP_TRACE("rule_count %d\n", rule_count);
				
				strcpy(maclist, Curlimit);
	
				if(rule_count == 0)
				{
					APP_TRACE("only only\n");
					nvram_set(RT2860_NVRAM, "LimitMac", "");
					nvram_commit(RT2860_NVRAM);
				}
				else
				{
					sprintf(limitmac, "%02X:%02X:%02X:%02X:%02X:%02X", lease.mac[0],lease.mac[1],lease.mac[2],lease.mac[3],lease.mac[4],lease.mac[5]);
					
					while(i++ <= (strstr(Curlimit, limitmac) - Curlimit))
					{
						if (Curlimit[i] == ';')
							count++;	
					}
					APP_TRACE("count %d\n", count);
					
					deleArray = (int *)malloc(rule_count * sizeof(int));
					if(!deleArray)
						return;
					deleArray[j++] = count;
							
					deleteNthValueMulti(deleArray, rule_count, maclist, ';');
					free(deleArray);
					
					APP_TRACE("new maclist is %s\n", maclist);		
					nvram_set(RT2860_NVRAM, "LimitMac", maclist);
					nvram_commit(RT2860_NVRAM);
					
				}
				
			}
			else
				APP_TRACE("unknow action, do nothing\n");		
			
		}
		
		index++;
	}
	
	fclose(fp);

	AppReturnOK(wp);
	
	return 0;
}
static void AppDevReboot(webs_t wp, char_t *path, char_t *query)
{
	AppReturnOK(wp);
		
	sleep(2);
	system("reboot");
}

static void AppDevReset(webs_t wp, char_t *path, char_t *query)
{
	AppReturnOK(wp);
		
	system("ralink_init clear 2860");
	system("ralink_init renew 2860 /etc_ro/Wireless/RT2860AP/RT2860_default_vlan");
	
	sleep(2);
	system("reboot");

}

void formDefineAppInterface(void)
{
	websFormDefine(T("AppLogin"), AppLogin);
	websFormDefine(T("AppLogout"), AppLogout);
	websFormDefine(T("AppSetWifi"), AppSetWifi);
	websFormDefine(T("AppSetWan"), AppSetWan);
	websFormDefine(T("AppGetDevInfo"), AppGetDevInfo);
	websFormDefine(T("AppGetLanListInfo"), AppGetLanListInfo);
	websFormDefine(T("AppGetLanDetailInfo"), AppGetLanDetailInfo);
	websFormDefine(T("AppLimitLanPc"), AppLimitLanPc);
	websFormDefine(T("AppGetNetworkStatus"), AppGetNetworkStatus);
	websFormDefine(T("AppDevReboot"), AppDevReboot);
	websFormDefine(T("AppDevReset"), AppDevReset);
}



