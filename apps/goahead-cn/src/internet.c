/* vi: set sw=4 ts=4 sts=4 fdm=marker: */
/*
 *	internet.c -- Internet Settings
 *
 *	Copyright (c) Ralink Technology Corporation All Rights Reserved.
 *
 *	$Id: //WIFI_SOC/release/SDK_4_1_0_0/source/user/goahead/src/internet.c#1 $
 */

#include	<stdlib.h>
#include	<sys/ioctl.h>
#include	<net/if.h>
#include	<net/route.h>
#include    <string.h>
#include    <dirent.h>
#include	"internet.h"
#include	"nvram.h"
#include	"webs.h"
#include	"utils.h"
#include 	"firewall.h"
#include	"management.h"
#include	"station.h"
#include	"wireless.h"

#include	"linux/autoconf.h"  //kernel config
#include	"config/autoconf.h" //user config
#include	"user/busybox/include/autoconf.h" //busybox config

#ifdef CONFIG_RALINKAPP_SWQOS
#include      "qos.h"
#endif

static int getUSBBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int getIPv6Built(int eid, webs_t wp, int argc, char_t **argv);
static int getIPv66rdBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int getIPv6DSBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int getStorageBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int getFtpBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int getSmbBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int getMediaBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int getWebCamBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int getPrinterSrvBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int getiTunesBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int getIgmpProxyBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int getVPNBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int getDnsmasqBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int getGWBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int getLltdBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int getPppoeRelayBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int getUpnpBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int getRadvdBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int getDynamicRoutingBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int getSWQoSBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int getDATEBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int getDDNSBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int getSysLogBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int getETHTOOLBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int get3GBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int getPktFilterBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int get3GBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int getPPTPBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int getL2TPBuilt(int eid, webs_t wp, int argc, char_t **argv);



static int getDhcpCliList(int eid, webs_t wp, int argc, char_t **argv);
static int needRebootWaring(int eid, webs_t wp, int argc, char_t **argv);
static int checkSuperUser(int eid, webs_t wp, int argc, char_t **argv);
static int getDns(int eid, webs_t wp, int argc, char_t **argv);
static int getHostSupp(int eid, webs_t wp, int argc, char_t **argv);
static int getIfLiveWeb(int eid, webs_t wp, int argc, char_t **argv);
static int getIfIsUpWeb(int eid, webs_t wp, int argc, char_t **argv);
static int getLanIp(int eid, webs_t wp, int argc, char_t **argv);
static int getLanMac(int eid, webs_t wp, int argc, char_t **argv);
static int getLanIfNameWeb(int eid, webs_t wp, int argc, char_t **argv);
static int getLanNetmask(int eid, webs_t wp, int argc, char_t **argv);
static int getWanIp(int eid, webs_t wp, int argc, char_t **argv);
static int getWanMac(int eid, webs_t wp, int argc, char_t **argv);
static int getWiFiMac(int eid, webs_t wp, int argc, char_t **argv);
static int getWanIfNameWeb(int eid, webs_t wp, int argc, char_t **argv);
static int getWanNetmask(int eid, webs_t wp, int argc, char_t **argv);
static int getWanGateway(int eid, webs_t wp, int argc, char_t **argv);
static int getRoutingTable(int eid, webs_t wp, int argc, char_t **argv);
static void setLan(webs_t wp, char_t *path, char_t *query);
static void setStaticDhcp(webs_t wp, char_t *path, char_t *query);
static void setUpnp(webs_t wp, char_t *path, char_t *query);
static void setIgmp(webs_t wp, char_t *path, char_t *query);
#if defined (CONFIG_IPV6)
static void setIPv6(webs_t wp, char_t *path, char_t *query);
#endif
#if 1
static int getDial3gStatus(int eid, webs_t wp, int argc, char_t **argv);
static int getDial3gSimCard(int eid, webs_t wp, int argc, char_t **argv);
static int getDial3gPPPDFlag(int eid, webs_t wp, int argc, char_t **argv);
static int getDial3gUsbModemInfo(int eid, webs_t wp, int argc, char_t **argv);
static int getDial3gUsbModemFlag(int eid, webs_t wp, int argc, char_t **argv);
static int getDial3gCimiValue(int eid, webs_t wp, int argc, char_t **argv);
static int getDial3gUsbModemFirmware(int eid, webs_t wp, int argc, char_t **argv);
static int getDial3gSignalLength(int eid, webs_t wp, int argc, char_t **argv);
static int getDial3gISPProvider(int eid, webs_t wp, int argc, char_t **argv); 
#endif
static void setVpnPaThru(webs_t wp, char_t *path, char_t *query);
static void setWan(webs_t wp, char_t *path, char_t *query);
static void setWan2(webs_t wp, char_t *path, char_t *query);
static void getMyMAC(webs_t wp, char_t *path, char_t *query);
static void addRouting(webs_t wp, char_t *path, char_t *query);
static void delRouting(webs_t wp, char_t *path, char_t *query);
static void dynamicRouting(webs_t wp, char_t *path, char_t *query);
static void UpdateCert(webs_t wp, char_t *path, char_t *query);
static void formConnectAutoCheck(webs_t wp,char_t*path,char_t *query); //by chenfei for wan auto detect


static int getStaticDhcpEnableASP(int eid, webs_t wp, int argc, char_t **argv);
static int getStaticDhcpRuleNumsASP(int eid, webs_t wp, int argc, char_t **argv);
static int showStaticDhcpRulesASP(int eid, webs_t wp, int argc, char_t **argv);
static void staticDhcpBasicSettings(webs_t wp, char_t *path, char_t *query);
static void staticDhcp(webs_t wp, char_t *path, char_t *query);
static void staticDhcpDelete(webs_t wp, char_t *path, char_t *query);


inline void zebraRestart(void);
void ripdRestart(void);

void formDefineInternet(void) {
	websAspDefine(T("getDhcpCliList"), getDhcpCliList);
	websAspDefine(T("needRebootWaring"), needRebootWaring);
	websAspDefine(T("checkSuperUser"), checkSuperUser);
	websAspDefine(T("getDns"), getDns);
	websAspDefine(T("getHostSupp"), getHostSupp);
	websAspDefine(T("getIfLiveWeb"), getIfLiveWeb);
	websAspDefine(T("getIfIsUpWeb"), getIfIsUpWeb);
	websAspDefine(T("getIgmpProxyBuilt"), getIgmpProxyBuilt);
	websAspDefine(T("getVPNBuilt"), getVPNBuilt);
	websAspDefine(T("getLanIp"), getLanIp);
	websAspDefine(T("getLanMac"), getLanMac);
	websAspDefine(T("getWiFiMac"), getWiFiMac);
	websAspDefine(T("getLanIfNameWeb"), getLanIfNameWeb);
	websAspDefine(T("getLanNetmask"), getLanNetmask);
	websAspDefine(T("getDnsmasqBuilt"), getDnsmasqBuilt);
	websAspDefine(T("getGWBuilt"), getGWBuilt);
	websAspDefine(T("getLltdBuilt"), getLltdBuilt);
	websAspDefine(T("getPppoeRelayBuilt"), getPppoeRelayBuilt);
	websAspDefine(T("getUpnpBuilt"), getUpnpBuilt);
	websAspDefine(T("getRadvdBuilt"), getRadvdBuilt);
	websAspDefine(T("getWanIp"), getWanIp);
	websAspDefine(T("getWanMac"), getWanMac);
	websAspDefine(T("getWanIfNameWeb"), getWanIfNameWeb);
	websAspDefine(T("getWanNetmask"), getWanNetmask);
	websAspDefine(T("getWanGateway"), getWanGateway);
	websAspDefine(T("getRoutingTable"), getRoutingTable);
	websAspDefine(T("getUSBBuilt"), getUSBBuilt);
	websAspDefine(T("getIPv6Built"), getIPv6Built);
	websAspDefine(T("getIPv66rdBuilt"), getIPv66rdBuilt);
	websAspDefine(T("getIPv6DSBuilt"), getIPv6DSBuilt);
	websAspDefine(T("getStorageBuilt"), getStorageBuilt);
	websAspDefine(T("getFtpBuilt"), getFtpBuilt);
	websAspDefine(T("getSmbBuilt"), getSmbBuilt);
	websAspDefine(T("getMediaBuilt"), getMediaBuilt);
	websAspDefine(T("getWebCamBuilt"), getWebCamBuilt);
	websAspDefine(T("getPrinterSrvBuilt"), getPrinterSrvBuilt);
	websAspDefine(T("getiTunesBuilt"), getiTunesBuilt);
	websFormDefine(T("setLan"), setLan);
	websFormDefine(T("setStaticDhcp"), setStaticDhcp);
	websFormDefine(T("setUpnp"), setUpnp);
	websFormDefine(T("setIgmp"), setIgmp);
	websFormDefine(T("formConnectAutoCheck"), formConnectAutoCheck); //added by chenfei for wan auto detect
#if defined (CONFIG_IPV6)
	websFormDefine(T("setIPv6"), setIPv6);
#endif
#if 1
	websAspDefine(T("getDial3gStatus"), getDial3gStatus);
	websAspDefine(T("getDial3gSimCard"), getDial3gSimCard);
	websAspDefine(T("getDial3gPPPDFlag"), getDial3gPPPDFlag);
	websAspDefine(T("getDial3gUsbModemInfo"), getDial3gUsbModemInfo);
	websAspDefine(T("getDial3gUsbModemFlag"), getDial3gUsbModemFlag);
	websAspDefine(T("getDial3gCimiValue"), getDial3gCimiValue);
	websAspDefine(T("getDial3gUsbModemFirmware"), getDial3gUsbModemFirmware); 
	websAspDefine(T("getDial3gSignalLength"), getDial3gSignalLength);
	websAspDefine(T("getDial3gISPProvider"), getDial3gISPProvider);
#endif
	websFormDefine(T("setVpnPaThru"), setVpnPaThru);
	websFormDefine(T("setWan"), setWan);
	websFormDefine(T("setWan2"), setWan2);
	websFormDefine(T("getMyMAC"), getMyMAC);
	websFormDefine(T("addRouting"), addRouting);
	websFormDefine(T("delRouting"), delRouting);
	websFormDefine(T("dynamicRouting"), dynamicRouting);
	websFormDefine(T("UpdateCert"), UpdateCert);
	websAspDefine(T("getDynamicRoutingBuilt"), getDynamicRoutingBuilt);
	websAspDefine(T("getSWQoSBuilt"), getSWQoSBuilt);
	websAspDefine(T("getDATEBuilt"), getDATEBuilt);
	websAspDefine(T("getDDNSBuilt"), getDDNSBuilt);
	websAspDefine(T("getSysLogBuilt"), getSysLogBuilt);
	websAspDefine(T("getETHTOOLBuilt"), getETHTOOLBuilt);
	websAspDefine(T("get3GBuilt"), get3GBuilt);
	websAspDefine(T("getPPTPBuilt"), getPPTPBuilt);
	websAspDefine(T("getL2TPBuilt"), getL2TPBuilt);
	websAspDefine(T("getPktFilterBuilt"), getPktFilterBuilt);

	websAspDefine(T("getStaticDhcpEnableASP"), getStaticDhcpEnableASP);
	websAspDefine(T("getStaticDhcpRuleNumsASP"), getStaticDhcpRuleNumsASP);
	websAspDefine(T("showStaticDhcpRulesASP"), showStaticDhcpRulesASP);
	websFormDefine(T("staticDhcpBasicSettings"), staticDhcpBasicSettings);//add by chenfei for static dhcp
	websFormDefine(T("staticDhcp"), staticDhcp);
	websFormDefine(T("staticDhcpDelete"), staticDhcpDelete);
}

/*
 * arguments: ifname  - interface name
 * description: test the existence of interface through /proc/net/dev
 * return: -1 = fopen error, 1 = not found, 0 = found
 */
int getIfLive(char *ifname)
{
	FILE *fp;
	char buf[256], *p;
	int i;

	if (NULL == (fp = fopen("/proc/net/dev", "r"))) {
		error(E_L, E_LOG, T("getIfLive: open /proc/net/dev error"));
		return -1;
	}

	fgets(buf, 256, fp);
	fgets(buf, 256, fp);
	while (NULL != fgets(buf, 256, fp)) {
		i = 0;
		while (isspace(buf[i++]))
			;
		p = buf + i - 1;
		while (':' != buf[i++])
			;
		buf[i-1] = '\0';
		if (!strcmp(p, ifname)) {
			fclose(fp);
			return 0;
		}
	}
	fclose(fp);
	error(E_L, E_LOG, T("getIfLive: device %s not found"), ifname);
	return 1;
}

/*
 * arguments: ifname  - interface name
 *            if_addr - a 18-byte buffer to store mac address
 * description: fetch mac address according to given interface name
 */
int getIfMac(char *ifname, char *if_hw)
{
	struct ifreq ifr;
	char *ptr;
	int skfd;

	if((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		error(E_L, E_LOG, T("getIfMac: open socket error"));
		return -1;
	}

	strncpy(ifr.ifr_name, ifname, IF_NAMESIZE);
	if(ioctl(skfd, SIOCGIFHWADDR, &ifr) < 0) {
		close(skfd);
		//error(E_L, E_LOG, T("getIfMac: ioctl SIOCGIFHWADDR error for %s"), ifname);
		return -1;
	}

	ptr = (char *)&ifr.ifr_addr.sa_data;
	sprintf(if_hw, "%02X:%02X:%02X:%02X:%02X:%02X",
			(ptr[0] & 0377), (ptr[1] & 0377), (ptr[2] & 0377),
			(ptr[3] & 0377), (ptr[4] & 0377), (ptr[5] & 0377));

	close(skfd);
	return 0;
}

/*
 * arguments: ifname  - interface name
 *            if_addr - a 16-byte buffer to store ip address
 * description: fetch ip address, netmask associated to given interface name
 */
int getIfIp(char *ifname, char *if_addr)
{
	struct ifreq ifr;
	int skfd = 0;

	if((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		error(E_L, E_LOG, T("getIfIp: open socket error"));
		return -1;
	}

	strncpy(ifr.ifr_name, ifname, IF_NAMESIZE);
	if (ioctl(skfd, SIOCGIFADDR, &ifr) < 0) {
		close(skfd);
		//error(E_L, E_LOG, T("getIfIp: ioctl SIOCGIFADDR error for %s"), ifname);
		return -1;
	}
	strcpy(if_addr, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));

	close(skfd);
	return 0;
}

/*
 * arguments: ifname - interface name
 * description: return 1 if interface is up
 *              return 0 if interface is down
 */
int getIfIsUp(char *ifname)
{
	struct ifreq ifr;
	int skfd;

	skfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (skfd == -1) {
		perror("socket");
		return -1;
	}
	strncpy(ifr.ifr_name, ifname, sizeof(ifr.ifr_name));
	if (ioctl(skfd, SIOCGIFFLAGS, &ifr) < 0) {
		perror("ioctl");
		close(skfd);
		return -1;
	}
	close(skfd);
	if (ifr.ifr_flags & IFF_UP)
		return 1;
	else
		return 0;
}

/*
 * arguments: ifname - interface name
 *            if_net - a 16-byte buffer to store subnet mask
 * description: fetch subnet mask associated to given interface name
 *              0 = bridge, 1 = gateway, 2 = wirelss isp
 */
int getIfNetmask(char *ifname, char *if_net)
{
	struct ifreq ifr;
	int skfd = 0;

	if((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		error(E_L, E_LOG, T("getIfNetmask: open socket error"));
		return -1;
	}

	strncpy(ifr.ifr_name, ifname, IF_NAMESIZE);
	if (ioctl(skfd, SIOCGIFNETMASK, &ifr) < 0) {
		close(skfd);
		//error(E_L, E_LOG, T("getIfNetmask: ioctl SIOCGIFNETMASK error for %s\n"), ifname);
		return -1;
	}
	strcpy(if_net, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
	close(skfd);
	return 0;
}

/*
 * description: return WAN interface name
 *              0 = bridge, 1 = gateway, 2 = wirelss isp
 */
char* getWanIfName(void)
{
	const char *mode = nvram_bufget(RT2860_NVRAM, "OperationMode");
	static char *if_name = "br0";

	if (NULL == mode)
		return if_name;
	if (!strncmp(mode, "0", 2))
		if_name = "br0";
	else if (!strncmp(mode, "1", 2)) {
#if defined CONFIG_RAETH_ROUTER || defined CONFIG_MAC_TO_MAC_MODE || defined CONFIG_RT_3052_ESW
#if defined (CONFIG_RAETH_SPECIAL_TAG)
#if defined (CONFIG_WAN_AT_P0)
		if_name = "eth2.1";
#else
		if_name = "eth2.5";
#endif
#else
		//if_name = "eth2.2";
		if_name = "br1";
#endif
#elif defined (CONFIG_GE1_RGMII_AN) && defined (CONFIG_GE2_RGMII_AN)
		if_name = "eth3";
#else /* MARVELL & CONFIG_ICPLUS_PHY */
		if_name = "eth2";
#endif
	}
	else if (!strncmp(mode, "2", 2))
		if_name = "ra0";
	else if (!strncmp(mode, "3", 2))
		if_name = "apcli0";
	return if_name;
}

char* getWanIfNamePPP(void)
{
    const char *cm = nvram_bufget(RT2860_NVRAM, "wanConnectionMode");
    if (!strncmp(cm, "PPPOE", 6) || !strncmp(cm, "L2TP", 5) || !strncmp(cm, "PPTP", 5) 
#ifdef CONFIG_USER_3G
		|| !strncmp(cm, "3G", 3)
#endif
	){
        return "ppp0";
	}

    return getWanIfName();
}


/*
 * description: return LAN interface name
 */
char* getLanIfName(void)
{
	const char *mode = nvram_bufget(RT2860_NVRAM, "OperationMode");
	static char *if_name = "br0";

	if (NULL == mode)
		return if_name;
	if (!strncmp(mode, "0", 2))
		if_name = "br0";
	else if (!strncmp(mode, "1", 2)) {
#if defined CONFIG_RAETH_ROUTER || defined CONFIG_MAC_TO_MAC_MODE || defined CONFIG_RT_3052_ESW
		if_name = "br0";
#elif defined  CONFIG_ICPLUS_PHY && CONFIG_RT2860V2_AP_MBSS
		char *num_s = nvram_bufget(RT2860_NVRAM, "BssidNum");
		if(atoi(num_s) > 1)	// multiple ssid
			if_name = "br0";
		else
			if_name = "ra0";
#elif defined (CONFIG_GE1_RGMII_AN) && defined (CONFIG_GE2_RGMII_AN)
		if_name = "br0";
#else
		if_name = "ra0";
#endif
	}
	else if (!strncmp(mode, "2", 2)) {
		if_name = "br0";
	}
	else if (!strncmp(mode, "3", 2)) {
		if_name = "br0";
	}
	return if_name;
}

/*
 * description: get the value "WAN" or "LAN" the interface is belong to.
 */
char *getLanWanNamebyIf(char *ifname)
{
	const char *mode = nvram_bufget(RT2860_NVRAM, "OperationMode");

	if (NULL == mode)
		return "Unknown";

	if (!strcmp(mode, "0")){	// bridge mode
		if(!strcmp(ifname, "br0"))
			return "LAN";
		return ifname;
	}

	if (!strcmp(mode, "1")) {	// gateway mode
#if defined CONFIG_RAETH_ROUTER || defined CONFIG_MAC_TO_MAC_MODE || defined CONFIG_RT_3052_ESW
		if(!strcmp(ifname, "br0"))
			return "LAN";
#if defined (CONFIG_RAETH_SPECIAL_TAG)
#if defined (CONFIG_WAN_AT_P0)
		if(!strcmp(ifname, "eth2.1") || !strcmp(ifname, "ppp0"))
#else
		if(!strcmp(ifname, "eth2.5") || !strcmp(ifname, "ppp0"))
#endif
#else
		//if(!strcmp(ifname, "eth2.2") || !strcmp(ifname, "ppp0"))
		if(!strcmp(ifname, "br1") || !strcmp(ifname, "ppp0"))
#endif
			return "WAN";
		return ifname;
#elif defined  CONFIG_ICPLUS_PHY && CONFIG_RT2860V2_AP_MBSS
		char *num_s = nvram_bufget(RT2860_NVRAM, "BssidNum");
		if(atoi(num_s) > 1 && !strcmp(ifname, "br0") )	// multiple ssid
			return "LAN";
		if(atoi(num_s) == 1 && !strcmp(ifname, "ra0"))
			return "LAN";
		if (!strcmp(ifname, "eth2") || !strcmp(ifname, "ppp0"))
			return "WAN";
		return ifname;
#elif defined (CONFIG_GE1_RGMII_AN) && defined (CONFIG_GE2_RGMII_AN)
		if(!strcmp(ifname, "br0"))
			return "LAN";
		if(!strcmp(ifname, "eth3"))
			return "WAN";
#else
		if(!strcmp(ifname, "ra0"))
			return "LAN";
		return ifname;
#endif
	}
	else if (!strncmp(mode, "2", 2)) {	// ethernet convertor
		if(!strcmp("eth2", ifname))
			return "LAN";
		if(!strcmp("ra0", ifname))
			return "WAN";
		return ifname;
	}
	else if (!strncmp(mode, "3", 2)) {	// apcli mode
		if(!strcmp("br0", ifname))
			return "LAN";
		if(!strcmp("apcli0", ifname))
			return "WAN";
		return ifname;
	}
	return ifname;
}

/*
 * description: write DHCP client list
 */
static int getDhcpCliList(int eid, webs_t wp, int argc, char_t **argv)
{
	FILE *fp;
	struct dhcpOfferedAddr {
		//unsigned char hostname[16];
		unsigned char mac[16];
		unsigned long ip;
		unsigned long expires;
	} lease;
	int i;
	struct in_addr addr;
	unsigned long expires;
	unsigned d, h, m;

	doSystem("killall -q -USR1 udhcpd");

	fp = fopen("/var/udhcpd.leases", "r");
	if (NULL == fp)
	{
		printf("\r\nfsdfsd\r\n");
		return websWrite(wp, T(""));
	}
	while (fread(&lease, 1, sizeof(lease), fp) == sizeof(lease)) {
		
		if (lease.mac[1]==0 && lease.mac[2]==0 && lease.mac[3]==0 && lease.mac[4]==0 && lease.mac[5]==0)
			continue;

		//if (strlen(lease.hostname) > 0)
			//websWrite(wp, T("<tr align=center><td>%-16s</td>\n"), lease.hostname);
		//else
			//websWrite(wp, T("<tr align=center><td>&nbsp;</td>"));
		websWrite(wp, T("<td>%02X"), lease.mac[0]);
		for (i = 1; i < 6; i++)
			websWrite(wp, T(":%02X"), lease.mac[i]);
		addr.s_addr = lease.ip;
		expires = ntohl(lease.expires);
		websWrite(wp, T("</td>\n<td>%s</td>\n<td>"), inet_ntoa(addr));
		d = expires / (24*60*60); expires %= (24*60*60);
		h = expires / (60*60); expires %= (60*60);
		m = expires / 60; expires %= 60;
		if (d>1)
			websWrite(wp, T("%u <script>dw(MM_days)</script> "), d);
		else	
			websWrite(wp, T("%u <script>dw(MM_day)</script> "), d);
		websWrite(wp, T("%02u:%02u:%02u</td></tr>\n"), h, m, (unsigned)expires);
	}
	fclose(fp);
	return 0;
}

static int needRebootWaring(int eid, webs_t wp, int argc, char_t **argv)
{
	const char *needreboot = nvram_bufget(RT2860_NVRAM, "needreboot");
	if(!strncmp(needreboot,"yes",3))
	{
		websWrite(wp, T("<font color=\"#FF0000\"><script>dw(OTHER_rebootwar)</script></font> "));
	}
	return 0;
}
static int checkSuperUser(int eid, webs_t wp, int argc, char_t **argv)
{
	if(strcmp("sup",umGetUserGroup(wp->userName)))
	{
		websRedirect(wp, "/rule.htm");
	}
	/*
	websWrite(wp, T("<script language=\"javascript\">\n\
		function refresh_all(){	\n\
		  top.location.href = \"http://%s/home.asp\"; \n\
		} \n\
		function update(){ \n\
		  self.setTimeout(\"refresh_all()\", 40000);\n\
		}\n</script>\n"), lan_ipaddr);	
		*/
	return 0;
}

/*
 * arguments: type - 1 = write Primary DNS
 *                   2 = write Secondary DNS
 * description: write DNS ip address accordingly
 */
static int getDns(int eid, webs_t wp, int argc, char_t **argv)
{
	FILE *fp;
	char buf[80] = {0}, ns_str[11], dns[16] = {0};
	int type, idx = 0, req = 0;

	if (ejArgs(argc, argv, T("%d"), &type) == 1) {
		if (1 == type)
			req = 1;
		else if (2 == type)
			req = 2;
		else
			return websWrite(wp, T(""));
	}

	fp = fopen("/etc/resolv.conf", "r");
	if (NULL == fp)
		return websWrite(wp, T(""));
	while (fgets(buf, sizeof(buf), fp) != NULL) {
		if (strncmp(buf, "nameserver", 10) != 0)
			continue;
		sscanf(buf, "%s%s", ns_str, dns);
		idx++;
		if (idx == req)
			break;
	}
	fclose(fp);

	return websWrite(wp, T("%s"), dns);
}

/*
 * arguments: 
 * description: return 1 if hostname is supported
 */
static int getHostSupp(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef GA_HOSTNAME_SUPPORT
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

/*
 * arguments: name - interface name (ex. eth0, rax ..etc)
 * description: write the existence of given interface,
 *              0 = ifc dosen't exist, 1 = ifc exists
 */
static int getIfLiveWeb(int eid, webs_t wp, int argc, char_t **argv)
{
	char_t *name;
	char exist[2] = "0";

	if (ejArgs(argc, argv, T("%s"), &name) < 1) {
		websError(wp, 400, T("Insufficient args\n"));
		return -1;
	}
	exist[0] = (getIfLive(name) == 0)? '1' : '0';
	return websWrite(wp, T("%s"), exist);
}

/*
 * arguments: name - interface name (ex. eth0, rax ..etc)
 * description: write the existence of given interface,
 *              0 = ifc is down, 1 = ifc is up
 */
static int getIfIsUpWeb(int eid, webs_t wp, int argc, char_t **argv)
{
	char_t *name;
	char up[2] = "1";

	if (ejArgs(argc, argv, T("%s"), &name) < 1) {
		websError(wp, 400, T("Insufficient args\n"));
		return -1;
	}
	up[0] = (getIfIsUp(name) == 1)? '1' : '0';
	return websWrite(wp, T("%s"), up);
}

static int getIgmpProxyBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_USER_IGMP_PROXY
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getVPNBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined CONFIG_NF_CONNTRACK_PPTP || defined CONFIG_NF_CONNTRACK_PPTP_MODULE || \
    defined CONFIG_IP_NF_PPTP        || defined CONFIG_IP_NF_PPTP_MODULE
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getUSBBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if (defined CONFIG_USB) || (defined CONFIG_MMC)
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getIPv6Built(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined (CONFIG_IPV6)
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getIPv66rdBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined (CONFIG_IPV6_SIT_6RD)
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getIPv6DSBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined (CONFIG_IPV6_TUNNEL)
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getStorageBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined CONFIG_USER_STORAGE && (defined CONFIG_USB || defined CONFIG_MMC)
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getFtpBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined CONFIG_USER_PROFTPD
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getSmbBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined CONFIG_USER_SAMBA
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getMediaBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined CONFIG_USB && defined CONFIG_USER_USHARE
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getWebCamBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined CONFIG_USB && defined CONFIG_USER_UVC_STREAM
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getPrinterSrvBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined CONFIG_USB && defined CONFIG_USER_P910ND
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getiTunesBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined CONFIG_USER_MTDAAPD 
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getDynamicRoutingBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined CONFIG_USER_ZEBRA
    return websWrite(wp, T("1"));
#else
    return websWrite(wp, T("0"));
#endif
}

static int getSWQoSBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined CONFIG_RALINKAPP_SWQOS
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getDATEBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined CONFIG_DATE
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getDDNSBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined CONFIG_USER_DDNS
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getSysLogBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined CONFIG_LOGREAD && defined CONFIG_KLOGD
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getETHTOOLBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined CONFIG_USER_ETHTOOL
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int get3GBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined CONFIG_USER_3G
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getPPTPBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined CONFIG_USER_ACCEL_PPTP
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getL2TPBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined CONFIG_USER_OPENL2TP
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}



#if 1

typedef struct proc_s {
	char cmd[16];					/* basename of executable file in call to exec(2) */
	int ruid;						/* real only (sorry) */
	int pid;						/* process id */
	int ppid;						/* pid of parent process */
	char state;						/* single-char code for process state (S=sleeping) */
	unsigned int vmsize;			/* size of process as far as the vm is concerned */
} proc_t;

static int file2str_ex(char *filename, char *ret, int cap)
{
	int fd, num_read;

	if ((fd = open(filename, O_RDONLY, 0)) == -1)
		return -1;
	if ((num_read = read(fd, ret, cap - 1)) <= 0)
		return -1;
	ret[num_read] = 0;
	close(fd);
	return num_read;
}

static int parse_proc_status(char *S, proc_t * P)
{
	char *tmp;

	memset(P->cmd, 0, sizeof P->cmd);
	sscanf(S, "Name:\t%15c", P->cmd);
	tmp = strchr(P->cmd, '\n');
	if (tmp)
		*tmp = '\0';
	tmp = strstr(S, "State");
	sscanf(tmp, "State:\t%c", &P->state);

	P->vmsize = 0;
	tmp = strstr(S, "Pid:");
	if (tmp)
		sscanf(tmp, "Pid:\t%d\n" "PPid:\t%d\n", &P->pid, &P->ppid);
	else
		return 0;

	/* For busybox, ignoring effective, saved, etc. */
	tmp = strstr(S, "Uid:");
	if (tmp)
		sscanf(tmp, "Uid:\t%d", &P->ruid);
	else
		return 0;

	tmp = strstr(S, "VmSize:");
	if (tmp)
		sscanf(tmp, "VmSize:\t%d", &P->vmsize);

	return 1;
}

int DBgetPid(char *progname)
{
	proc_t p;
	DIR *dir;
	FILE *file;
	struct dirent *entry;
	char path[32], sbuf[512];
	char uidName[9], stmp[200];
	int len, i, c, pid=0, ret, shcmd=0;

	dir = opendir("/proc");
	if (!dir) {
		return 0;
	}

	while ((entry = readdir(dir)) != NULL) {
		if (!isdigit(*entry->d_name))
			continue;
		sprintf(path, "/proc/%s/status", entry->d_name);
		if ((file2str_ex(path, sbuf, sizeof(sbuf))) != -1) {
			ret = parse_proc_status(sbuf, &p);
			if ( ret == 0 ) break;
		}

		sprintf(path, "/proc/%s/cmdline", entry->d_name);
		file = fopen(path, "r");
		if (file == NULL)
			continue;
		i = 0;

		memset( stmp, 0, sizeof(stmp) );
		shcmd = 0;
		while (((c = getc(file)) != EOF) && (i < 200)) {
			stmp[i] = c;
			if (c == '\0' && i)	{
				if ( strcmp(stmp,"sh") != 0 ){
				  	if ( strcmp(stmp,"-c") != 0 )
				  		break;
				  	else 
				  		shcmd=2;
				}
				else
				    shcmd=1;
				i=0;
			}
			else
				i++;
		}

		fclose(file);
		
		if (i == 0){
			if ( !strcmp(p.cmd, progname) )	{
				pid = p.pid;
				break;
			}
		}
		else{
			if ( !strcmp(stmp, progname) ){
				pid = p.pid;
				break;
			}
			else if ( shcmd && strstr(stmp, progname) )	{
				pid = p.pid;
				break;
			}
		}
	}
	closedir(dir);
	return pid;
}
#endif

static int getPktFilterBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined CONFIG_IP_NF_FILTER
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

/*
 * description: write LAN ip address accordingly
 */
static int getLanIp(int eid, webs_t wp, int argc, char_t **argv)
{
	char if_addr[16];

	if (-1 == getIfIp(getLanIfName(), if_addr)) {
		//websError(wp, 500, T("getLanIp: calling getIfIp error\n"));
		return websWrite(wp, T(""));
	}
	return websWrite(wp, T("%s"), if_addr);
}

/*
 * description: write LAN MAC address accordingly
 */
static int getLanMac(int eid, webs_t wp, int argc, char_t **argv)
{
	char if_mac[18];

	if (-1 == getIfMac(getLanIfName(), if_mac)) {
		//websError(wp, 500, T("getLanIp: calling getIfMac error\n"));
		return websWrite(wp, T(""));
	}
	return websWrite(wp, T("%s"), if_mac);
}

/*
 * description: write WiFi MAC address accordingly
 */
static int getWiFiMac(int eid, webs_t wp, int argc, char_t **argv)
{
	char if_mac[18];
	char *ifname;

	if (ejArgs(argc, argv, T("%s"), &ifname) < 1) {
		return 0;
	}
	if (-1 == getIfMac(ifname, if_mac)) {
		//websError(wp, 500, T("getLanIp: calling getIfMac error\n"));
		return websWrite(wp, T(""));
	}
	return websWrite(wp, T("%s"), if_mac);
}

/*
 * arguments: type - 0 = return LAN interface name (default)
 *                   1 = write LAN interface name
 * description: return or write LAN interface name accordingly
 */
static int getLanIfNameWeb(int eid, webs_t wp, int argc, char_t **argv)
{
	int type;
	char *name = getLanIfName();

	if (ejArgs(argc, argv, T("%d"), &type) == 1) {
		if (1 == type) {
			return websWrite(wp, T("%s"), name);
		}
	}
	ejSetResult(eid, name);
	return 0;
}

/*
 * description: write LAN subnet mask accordingly
 */
static int getLanNetmask(int eid, webs_t wp, int argc, char_t **argv)
{
	char if_net[16];

	if (-1 == getIfNetmask(getLanIfName(), if_net)) {
		//websError(wp, 500, T("getLanNetmask: calling getIfNetmask error\n"));
		return websWrite(wp, T(""));
	}
	return websWrite(wp, T("%s"), if_net);
}

static int getGWBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined CONFIG_LAN_WAN_SUPPORT || defined CONFIG_MAC_TO_MAC_MODE || \
	(defined CONFIG_GE1_RGMII_AN && defined CONFIG_GE2_RGMII_AN)
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getDnsmasqBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_USER_DNSMASQ
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getLltdBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined CONFIG_USER_LLTD
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getPppoeRelayBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_USER_RPPPPOE_RELAY
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getUpnpBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if (defined CONFIG_USER_UPNP_IGD) || (defined CONFIG_USER_MINIUPNPD)
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getRadvdBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_USER_RADVD
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

int getInAddrLktos( char *interface, void *pAddr )
{
    struct ifreq ifr;
    int skfd, found=0;
	struct sockaddr_in *addr;
    skfd = socket(AF_INET, SOCK_DGRAM, 0);

    strcpy(ifr.ifr_name, interface);
    if (ioctl(skfd, SIOCGIFFLAGS, &ifr) < 0){
    		close( skfd );
		return (0);
	}
  
    if (ioctl(skfd, SIOCGIFDSTADDR, &ifr) >= 0) {
		addr = ((struct sockaddr_in *)&ifr.ifr_addr);
		*((struct in_addr *)pAddr) = *((struct in_addr *)&addr->sin_addr);
		found = 1;
    }
    close( skfd );
    return found;

}


/*
 * description: write WAN ip address accordingly
 */
static int getWanIp(int eid, webs_t wp, int argc, char_t **argv)
{
	char if_addr[16];

	if (-1 == getIfIp(getWanIfNamePPP(), if_addr)) {
		//websError(wp, 500, T("getWanIp: calling getIfIp error\n"));
		return websWrite(wp, T(""));
	}
	return websWrite(wp, T("%s"), if_addr);
}

/*
 * description: write WAN MAC address accordingly
 */
static int getWanMac(int eid, webs_t wp, int argc, char_t **argv)
{
	char if_mac[18];

	if (-1 == getIfMac(getWanIfName(), if_mac)) {
		//websError(wp, 500, T("getLanIp: calling getIfMac error\n"));
		return websWrite(wp, T(""));
	}
	return websWrite(wp, T("%s"), if_mac);
}

/*
 * arguments: type - 0 = return WAN interface name (default)
 *                   1 = write WAN interface name
 * description: return or write WAN interface name accordingly
 */
static int getWanIfNameWeb(int eid, webs_t wp, int argc, char_t **argv)
{
	int type;
	char *name = getWanIfName();

	if (ejArgs(argc, argv, T("%d"), &type) == 1) {
		if (1 == type) {
			return websWrite(wp, T("%s"), name);
		}
	}
	ejSetResult(eid, name);
	return 0;
}

/*
 * description: write WAN subnet mask accordingly
 */
static int getWanNetmask(int eid, webs_t wp, int argc, char_t **argv)
{
	char if_net[16];
	const char *cm = nvram_bufget(RT2860_NVRAM, "wanConnectionMode");

	if (!strncmp(cm, "PPPOE", 6) || !strncmp(cm, "L2TP", 5) || !strncmp(cm, "PPTP", 5) 
#ifdef CONFIG_USER_3G
			|| !strncmp(cm, "3G", 3)
#endif
	){ //fetch ip from ppp0
		if (-1 == getIfNetmask("ppp0", if_net)) {
			return websWrite(wp, T(""));
		}
	}
	else if (-1 == getIfNetmask(getWanIfName(), if_net)) {
		//websError(wp, 500, T("getWanNetmask: calling getIfNetmask error\n"));
		return websWrite(wp, T(""));
	}
	return websWrite(wp, T("%s"), if_net);
}

/*
 * description: write WAN default gateway accordingly
 */
static int getWanGateway(int eid, webs_t wp, int argc, char_t **argv)
{
	char   buff[256];
	int    nl = 0 ;
	struct in_addr dest;
	struct in_addr gw;
	struct in_addr	intaddr;
	int    flgs, ref, use, metric;
	unsigned long int d,g,m;
	int    find_default_flag = 0;

	char sgw[16]={0};
	FILE *fp=NULL;
	const char *cm = nvram_bufget(RT2860_NVRAM, "wanConnectionMode");

	if (!strncmp(cm, "PPPOE", 6))
	{
		if(getInAddrLktos("ppp0", (void *)&intaddr ))
		{
			sprintf(sgw,"%s",inet_ntoa(intaddr));
			find_default_flag=1;
		}
		
	}
	else
	{
		fp = fopen("/proc/net/route", "r");
		while (fgets(buff, sizeof(buff), fp) != NULL) {
			if (nl) {
				int ifl = 0;
				while (buff[ifl]!=' ' && buff[ifl]!='\t' && buff[ifl]!='\0')
					ifl++;
				buff[ifl]=0;    /* interface */
				if (sscanf(buff+ifl+1, "%lx%lx%X%d%d%d%lx",
							&d, &g, &flgs, &ref, &use, &metric, &m)!=7) {
					fclose(fp);
					return websWrite(wp, T("format error"));
				}

				if (flgs&RTF_UP) {
					dest.s_addr = d;
					gw.s_addr   = g;
					strcpy(sgw, (gw.s_addr==0 ? "" : inet_ntoa(gw)));

					if (dest.s_addr == 0) {
						find_default_flag = 1;
						break;
					}
				}
			}
			nl++;
		}
		fclose(fp);
	}
	

	if (find_default_flag == 1)
		return websWrite(wp, T("%s"), sgw);
	else
		return websWrite(wp, T(""));
}


#define DD printf("%d\n", __LINE__);fflush(stdout);

/*
 *
 */
int getIndexOfRoutingRule(char *dest, char *netmask, char *interface)
{
	int index=0;
	char *rrs, one_rule[256];
	char dest_f[32], netmask_f[32], interface_f[32];

	rrs = (char *) nvram_bufget(RT2860_NVRAM, "RoutingRules");
	if(!rrs || !strlen(rrs))
		return -1;

	while( getNthValueSafe(index, rrs, ';', one_rule, 256) != -1 ){
		if((getNthValueSafe(0, one_rule, ',', dest_f, sizeof(dest_f)) == -1)){
			index++;
			continue;
		}
		if((getNthValueSafe(1, one_rule, ',', netmask_f, sizeof(netmask_f)) == -1)){
			index++;
			continue;
		}
		if((getNthValueSafe(4, one_rule, ',', interface_f, sizeof(interface_f)) == -1)){
			index++;
			continue;
		}
		//printf("@@@@@ %s %s %s\n", dest_f, netmask_f, interface_f);
		//printf("----- %s %s %s\n", dest, netmask, interface);
		if((!strcmp(dest, dest_f)) && (!strcmp(netmask, netmask_f)) && (!strcmp(interface, interface_f))){
			return index;
		}
		index++;
	}

	return -1;
}

static void removeRoutingRule(char *dest, char *netmask, char *ifname)
{
	char cmd[1024];
	strcpy(cmd, "route del ");
	
	// host or net?
	if(!strcmp(netmask, "255.255.255.255") )
		strcat(cmd, "-host ");
	else
		strcat(cmd, "-net ");

	// destination
	strcat(cmd, dest);
	strcat(cmd, " ");

	// netmask
	if(strcmp(netmask, "255.255.255.255"))
		sprintf(cmd, "%s netmask %s", cmd, netmask);

	//interface
	sprintf(cmd, "%s dev %s ", cmd, ifname);
	doSystem(cmd);
}

void staticRoutingInit(void)
{
	int index=0;
	char one_rule[256];
	char *rrs;
	struct in_addr dest_s, gw_s, netmask_s;
	char dest[32], netmask[32], gw[32], interface[32], true_interface[32], custom_interface[32], comment[32];
	int	flgs, ref, use, metric, nl=0;
	unsigned long int d,g,m;
	int isGatewayMode = (!strcmp("1", nvram_bufget(RT2860_NVRAM, "OperationMode"))) ? 1 : 0 ;

	// delete old user rules
	FILE *fp = fopen("/proc/net/route", "r");
	if(!fp)
		return;

	while (fgets(one_rule, sizeof(one_rule), fp) != NULL) {
		if (nl) {
			if (sscanf(one_rule, "%s%lx%lx%X%d%d%d%lx",
					interface, &d, &g, &flgs, &ref, &use, &metric, &m) != 8) {
				printf("format error\n");
				fclose(fp);
				return;
			}
			dest_s.s_addr = d;
			gw_s.s_addr = g;
			netmask_s.s_addr = m;

			strncpy(dest, inet_ntoa(dest_s), sizeof(dest));
			strncpy(gw, inet_ntoa(gw_s), sizeof(gw));
			strncpy(netmask, inet_ntoa(netmask_s), sizeof(netmask));

			// check if internal routing rules
			if( (index=getIndexOfRoutingRule(dest, netmask, interface)) != -1){
				removeRoutingRule(dest, netmask, interface);
			}
		}
		nl++;
	}
	fclose(fp);

	index = 0;
	rrs = (char *) nvram_bufget(RT2860_NVRAM, "RoutingRules");
	if(!rrs|| !strlen(rrs))
		return;

	while( getNthValueSafe(index, rrs, ';', one_rule, 256) != -1 ){
		char cmd[1024];

		if((getNthValueSafe(0, one_rule, ',', dest, sizeof(dest)) == -1)){
			index++;
			continue;
		}
		if((getNthValueSafe(1, one_rule, ',', netmask, sizeof(netmask)) == -1)){
			index++;
			continue;
		}
		if((getNthValueSafe(2, one_rule, ',', gw, sizeof(gw)) == -1)){
			index++;
			continue;
		}
		if((getNthValueSafe(3, one_rule, ',', interface, sizeof(interface)) == -1)){
			index++;
			continue;
		}
		if((getNthValueSafe(4, one_rule, ',', true_interface, sizeof(true_interface)) == -1)){
			index++;
			continue;
		}
		if((getNthValueSafe(5, one_rule, ',', custom_interface, sizeof(custom_interface)) == -1)){
			index++;
			continue;
		}
		if((getNthValueSafe(6, one_rule, ',', comment, sizeof(comment)) == -1)){
			index++;
			continue;
		}

		strcpy(cmd, "route add ");
		
		// host or net?
		if(!strcmp(netmask, "255.255.255.255") )
			strcat(cmd, "-host ");
		else
			strcat(cmd, "-net ");

		// destination
		strcat(cmd, dest);
		strcat(cmd, " ");

		// netmask
		if(strcmp(netmask, "255.255.255.255") )
			sprintf(cmd, "%s netmask %s", cmd, netmask);

		// gateway
		if(strlen(gw) && strcmp(gw, "0.0.0.0"))
			sprintf(cmd, "%s gw %s", cmd, gw);

		//interface
//		if (!strcmp(interface, "WAN")){
//			true_interface = getWanIfName();
//		}else if (!gstrcmp(interface, "Custom")){
//			true_interface = custom_interface;
//		}else	// LAN & unknown
//			true_interface = getLanIfName();

		sprintf(cmd, "%s dev %s ", cmd, true_interface);

		strcat(cmd, "2>&1 ");

		if(strcmp(interface, "WAN") || (!strcmp(interface, "WAN") && isGatewayMode)  ){
			doSystem(cmd);
		}
		else{
			printf("Skip WAN routing rule in the non-Gateway mode: %s\n", cmd);
		}

		index++;
	}
	return;
}

void dynamicRoutingInit(void)
{
	zebraRestart();
	ripdRestart();
}

void RoutingInit(void)
{
	staticRoutingInit();
	dynamicRoutingInit();
}

static inline int getNums(char *value, char delimit)
{
    char *pos = value;
    int count=1;

    if(!pos || !strlen(pos))
        return 0;
    while( (pos = strchr(pos, delimit))){
        pos = pos+1;
        count++;
    }
    return count;
}
/*
 *
 */
inline int getRuleNums2(char *rules)
{
	return getNums(rules, ';');
}

/*
 * description: get routing table
 */
static int getRoutingTable(int eid, webs_t wp, int argc, char_t **argv)
{
	char   result[4096] = {0};
	char   buff[512];
	int    nl = 0, index;
	char   ifname[32], interface[128];
	struct in_addr dest, gw, netmask;
	char   dest_str[32], gw_str[32], netmask_str[32], comment[32];
	int    flgs, ref, use, metric;
	int	   *running_rules = NULL;
	unsigned long int d,g,m;
	char *rrs = (char *) nvram_bufget(RT2860_NVRAM, "RoutingRules");
	int  rule_count;
	FILE *fp = fopen("/proc/net/route", "r");
	if(!fp)
		return -1;

	rule_count = getNums(rrs, ';');

	if(rule_count){
		running_rules = calloc(1, sizeof(int) * rule_count);
		if(!running_rules) {
			fclose(fp);
			return -1;
		}
	}
		
	strncat(result, "\"", sizeof(result));
	while (fgets(buff, sizeof(buff), fp) != NULL) {
		if (nl) {
			if (sscanf(buff, "%s%lx%lx%X%d%d%d%lx",
					ifname, &d, &g, &flgs, &ref, &use, &metric, &m) != 8) {
				printf("format error\n");
				free(running_rules);
				fclose(fp);
				return websWrite(wp, T(""));
			}
			dest.s_addr = d;
			gw.s_addr = g;
			netmask.s_addr = m;

			if(! (flgs & 0x1) )	// skip not usable
				continue;

			strncpy(dest_str, inet_ntoa(dest), sizeof(dest_str));
			strncpy(gw_str, inet_ntoa(gw), sizeof(gw_str));
			strncpy(netmask_str, inet_ntoa(netmask), sizeof(netmask_str));

			if(nl > 1)
				strncat(result, ";", sizeof(result));
			strncat(result, ifname, sizeof(result));		strncat(result, ",", sizeof(result));
			strncat(result, dest_str, sizeof(result));		strncat(result, ",", sizeof(result));
			strncat(result, gw_str, sizeof(result));			strncat(result, ",", sizeof(result));
			strncat(result, netmask_str, sizeof(result) );	strncat(result, ",", sizeof(result));
			snprintf(result, sizeof(result), "%s%d,%d,%d,%d,", result, flgs, ref, use, metric);

			// check if internal routing rules
			strcpy(comment, " ");
			if( (index=getIndexOfRoutingRule(dest_str, netmask_str, ifname)) != -1){
				char one_rule[256];

				if(index < rule_count)
					running_rules[index] = 1;
				else
					printf("fatal error in %s\n", __FUNCTION__);

				snprintf(result, sizeof(result), "%s%d,", result, index);
				if(rrs && strlen(rrs)){
					if( getNthValueSafe(index, rrs, ';', one_rule, sizeof(one_rule)) != -1){

						if( getNthValueSafe(3, one_rule, ',', interface, sizeof(interface)) != -1){
							strncat(result, interface, sizeof(result));
							strncat(result, ",", sizeof(result));
						}
						if( getNthValueSafe(6, one_rule, ',', comment, sizeof(comment)) != -1){
							// do nothing;
						}
					}
				}
			}else{
				strncat(result, "-1,", sizeof(result));
				strncat(result, getLanWanNamebyIf(ifname), sizeof(result));
				strncat(result, ",", sizeof(result));
			}
			strncat(result, "0,", sizeof(result));	// used rule
			strncat(result, comment, sizeof(result));
		}
		nl++;
	}

	for(index=0; index < rule_count; index++){
		char one_rule[256];

		if(running_rules[index])
			continue;

		if(getNthValueSafe(index, rrs, ';', one_rule, sizeof(one_rule)) == -1)
			continue;

		if(getNthValueSafe(0, one_rule, ',', dest_str, sizeof(dest_str)) == -1)
			continue;

		if(getNthValueSafe(1, one_rule, ',', netmask_str, sizeof(netmask_str)) == -1)
			continue;

		if(getNthValueSafe(2, one_rule, ',', gw_str, sizeof(gw_str)) == -1)
			continue;

		if(getNthValueSafe(3, one_rule, ',', interface, sizeof(interface)) == -1)
			continue;

		if(getNthValueSafe(4, one_rule, ',', ifname, sizeof(ifname)) == -1)
			continue;

		if(getNthValueSafe(6, one_rule, ',', comment, sizeof(comment)) == -1)
			continue;

		if(strlen(result))
			strncat(result, ";", sizeof(result));

		snprintf(result, sizeof(result), "%s%s,%s,%s,%s,0,0,0,0,%d,%s,1,%s", result, ifname, dest_str, gw_str, netmask_str, index, interface, comment);
	}

	strcat(result, "\"");
	websLongWrite(wp, result);
	fclose(fp);
	if(running_rules)
		free(running_rules);
	//printf("%s\n", result);
	return 0;
}

static void addRouting(webs_t wp, char_t *path, char_t *query)
{
	char_t  *submitUrl = websGetVar(wp, T("submit-url"), T(""));

	char_t *dest, *hostnet, *netmask, *gateway, *iface, *true_interface, *custom_interface, *comment;
	char cmd[256] = {0};
	char result[256] = {0};
	FILE *fp;

	dest = websGetVar(wp, T("dest"), T(""));
	hostnet = websGetVar(wp, T("hostnet"), T(""));
	netmask = websGetVar(wp, T("netmask"), T(""));	
	gateway = websGetVar(wp, T("gateway"), T(""));
	iface = websGetVar(wp, T("interface"), T(""));
	custom_interface = websGetVar(wp, T("custom_interface"), T(""));
	comment = websGetVar(wp, T("comment"), T(""));

	if( !dest)
		return;

	strcat(cmd, "route add ");
	
	// host or net?
	if(!gstrcmp(hostnet, "net"))
		strcat(cmd, "-net ");
	else
		strcat(cmd, "-host ");

	// destination
	strcat(cmd, dest);
	strcat(cmd, " ");

	// netmask
	if(gstrlen(netmask))
		sprintf(cmd, "%s netmask %s", cmd, netmask);
	else
		netmask = "255.255.255.255";

	//gateway
	if(gstrlen(gateway))
		sprintf(cmd, "%s gw %s", cmd, gateway);
	else
		gateway = "0.0.0.0";

	//interface
	if(gstrlen(iface)){
		if (!gstrcmp(iface, "WAN")){
			true_interface = getWanIfName();
		}else if (!gstrcmp(iface, "Custom")){
			if(!gstrlen(custom_interface))
				return;
			true_interface = custom_interface;
		}else	// LAN & unknown
			true_interface = getLanIfName();
	}else{
		iface = "LAN";
		true_interface = getLanIfName();
	}
	sprintf(cmd, "%s dev %s ", cmd, true_interface);

	strcat(cmd, "2>&1 ");

	printf("%s\n", cmd);
	fp = popen(cmd, "r");
	fgets(result, sizeof(result), fp);
	pclose(fp);


	if(!strlen(result)){
		// success, write down to the flash
		char tmp[1024];
		const char *rrs = nvram_bufget(RT2860_NVRAM, "RoutingRules");
		if(!rrs || !strlen(rrs)){
			memset(tmp, 0, sizeof(tmp));
		}
		else{
			strncpy(tmp, rrs, sizeof(tmp));
		}
		
		if(strlen(tmp))
			strcat(tmp, ";");

		sprintf(tmp, "%s%s,%s,%s,%s,%s,%s,%s", tmp, dest, netmask, gateway, iface, true_interface, custom_interface, comment);
		nvram_bufset(RT2860_NVRAM, "RoutingRules", tmp);
		nvram_commit(RT2860_NVRAM);
	}
	else{
		websHeader(wp);		
		websWrite(wp, T("<h1>Add routing failed:<br> %s<h1>"), result);
		websFooter(wp);
		websDone(wp, 200);
		return;
	}

//#if defined(CONFIG_RALINK_MT7620)
	//doSystem("lktos_networkconfig mtk7620 initlan");
//#else
	//doSystem("lktos_networkconfig ra5350 initlan");
	//initInternet();
//#endif

	websRedirect(wp, submitUrl);
}

static void delRouting(webs_t wp, char_t *path, char_t *query)
{
	char_t  *submitUrl = websGetVar(wp, T("submit-url"), T(""));

	int index, rule_count;
	char_t *value, dest[256], netmask[256], true_interface[256];
	char name_buf[16] = {0};
	char *rrs;
	char *new_rrs;
	int *deleArray, j=0;
	
	rrs = (char *) nvram_bufget(RT2860_NVRAM, "RoutingRules");
	if(!rrs || !strlen(rrs))
		return;

	rule_count = getNums(rrs, ';');
	if(!rule_count)
		return;

	if(!(deleArray = malloc(sizeof(int) * rule_count) ) )
		return;

	if(! (new_rrs = strdup(rrs))){
		free(deleArray);
		return;
	}

	//websHeader(wp);

	for(index=0; index< rule_count; index++){
		snprintf(name_buf, sizeof(name_buf), "DR%d", index);
		value = websGetVar(wp, name_buf, NULL);
		if(value){
			deleArray[j++] = index;
			if(strlen(value) > 256)
				continue;
			sscanf(value, "%s%s%s", dest, netmask, true_interface);
			removeRoutingRule(dest, netmask, true_interface);
			//websWrite(wp, T("Delete entry: %s,%s,%s<br>\n"), dest, netmask, true_interface);
		}
	}

	if(j>0){
		deleteNthValueMulti(deleArray, j, new_rrs, ';');
		nvram_bufset(RT2860_NVRAM, "RoutingRules", new_rrs);
		nvram_commit(RT2860_NVRAM);
	}

	//websFooter(wp);
	//websDone(wp, 200);

//#if defined(CONFIG_RALINK_MT7620)
	//doSystem("lktos_networkconfig mtk7620 initlan");
//#else
	//doSystem("lktos_networkconfig ra5350 initlan");
	//initInternet();
//#endif

	websRedirect(wp, submitUrl);

	free(deleArray);
	free(new_rrs);
}

void ripdRestart(void)
{
	char lan_ip[16], wan_ip[16], lan_mask[16], wan_mask[16];

	const char *opmode = nvram_bufget(RT2860_NVRAM, "OperationMode");
	const char *password = nvram_bufget(RT2860_NVRAM, "Password");
	const char *RIPEnable = nvram_bufget(RT2860_NVRAM, "RIPEnable");

	doSystem("killall -q ripd");

	if(!opmode||!strlen(opmode))
		return;
	if(!strcmp(opmode, "0"))	// bridge
		return;

	if(!RIPEnable || !strlen(RIPEnable) || !strcmp(RIPEnable,"0"))
        return;

	if(!password || !strlen(password))
		password = "rt2880";

	doSystem("echo \"hostname linux.router1\" > /etc/ripd.conf ");
	doSystem("echo \"password %s\" >> /etc/ripd.conf ", password);
	doSystem("echo \"router rip\" >> /etc/ripd.conf ");

	// deal with WAN
	if(getIfIp(getWanIfName(), wan_ip) != -1){
		if(getIfNetmask(getWanIfName(), wan_mask) != -1){
			doSystem("echo \"network %s/%d\" >> /etc/ripd.conf", wan_ip, netmask_aton(wan_mask));
			doSystem("echo \"network %s\" >> /etc/ripd.conf", getWanIfName());
		}else
			printf("ripdRestart(): The WAN IP is still undeterminated...\n");
	}else
		printf("ripdRestart(): The WAN IP is still undeterminated...\n");

	// deal with LAN
	if(getIfIp(getLanIfName(), lan_ip) != -1){
		if(getIfNetmask(getLanIfName(), lan_mask) != -1){
			doSystem("echo \"network %s/%d\" >> /etc/ripd.conf", lan_ip, netmask_aton(lan_mask));
			doSystem("echo \"network %s\" >> /etc/ripd.conf", getLanIfName());
		}
	}
	doSystem("echo \"version 2\" >> /etc/ripd.conf");
	doSystem("echo \"log syslog\" >> /etc/ripd.conf");
	doSystem("ripd -f /etc/ripd.conf -d");
}

inline void zebraRestart(void)
{
	const char *opmode = nvram_bufget(RT2860_NVRAM, "OperationMode");
	const char *password = nvram_bufget(RT2860_NVRAM, "Password");
	const char *RIPEnable = nvram_bufget(RT2860_NVRAM, "RIPEnable");

	doSystem("killall -q zebra");

	if(!opmode||!strlen(opmode))
		return;
	if(!strcmp(opmode, "0"))	// bridge
		return;

	if(!RIPEnable || !strlen(RIPEnable) || !strcmp(RIPEnable,"0"))
		return;

	if(!password || !strlen(password))
		password = "rt2880";

	doSystem("echo \"hostname linux.router1\" > /etc/zebra.conf ");
	doSystem("echo \"password %s\" >> /etc/zebra.conf ", password);
	doSystem("echo \"enable password rt2880\" >> /etc/zebra.conf ");
	doSystem("echo \"log syslog\" >> /etc/zebra.conf ");
	doSystem("zebra -d -f /etc/zebra.conf");
}

static void dynamicRouting(webs_t wp, char_t *path, char_t *query)
{
	char_t  *submitUrl = websGetVar(wp, T("submit-url"), T(""));

	char_t *rip;
	const char *RIPEnable = nvram_bufget(RT2860_NVRAM, "RIPEnable");
	rip = websGetVar(wp, T("RIPSelect"), T(""));
	if(!rip || !strlen(rip))
		return;

	if(!RIPEnable || !strlen(RIPEnable))
		RIPEnable = "0";

	if(!gstrcmp(rip, "0") && !strcmp(RIPEnable, "0")){
		// nothing changed
	}else if(!gstrcmp(rip, "1") && !strcmp(RIPEnable, "1")){
		// nothing changed
	}else if(!gstrcmp(rip, "0") && !strcmp(RIPEnable, "1")){
		nvram_bufset(RT2860_NVRAM, "RIPEnable", rip);
		nvram_commit(RT2860_NVRAM);
		doSystem("killall -q ripd");
		doSystem("killall -q zebra");
	}else if(!gstrcmp(rip, "1") && !strcmp(RIPEnable, "0")){
		nvram_bufset(RT2860_NVRAM, "RIPEnable", rip);
		nvram_commit(RT2860_NVRAM);
		zebraRestart();
		ripdRestart();
	}else{
		return;
	}

	websRedirect(wp, submitUrl);
}

/**********/
int getAP6Mac(char *ifname, char *if_hw)
{
    struct ifreq ifr;
	char *ptr;
	int skfd;

	if((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		error(E_L, E_LOG, T("getIfMac: open socket error"));
		return -1;
	}

	strncpy(ifr.ifr_name, ifname, IF_NAMESIZE);
	if(ioctl(skfd, SIOCGIFHWADDR, &ifr) < 0) {
		close(skfd);
		return -1;
	}

	ptr = (char *)&ifr.ifr_addr.sa_data;
    sprintf(if_hw, "%02X%02X%02X", (ptr[3] & 0377), (ptr[4] & 0377), (ptr[5] & 0377));
	
    close(skfd);
    return 0;
}
/**********/

/*
 * description: setup internet according to nvram configurations
 *              (assume that nvram_init has already been called)
 *              return value: 0 = successful, -1 = failed
 */
int initInternet(void)
{
#ifndef CONFIG_RALINK_RT2880
	const char *auth_mode = nvram_bufget(RT2860_NVRAM, "AuthMode");
#endif
#if defined CONFIG_RT2860V2_STA || defined CONFIG_RT2860V2_STA_MODULE
	const char *opmode;
#endif

	//doSystem("internet.sh");

	//automatically connect to AP according to the active profile
#if defined CONFIG_RT2860V2_STA || defined CONFIG_RT2860V2_STA_MODULE
	opmode = nvram_bufget(RT2860_NVRAM, "OperationMode");
	if (!strcmp(opmode, "2") || (!strcmp(opmode, "0") &&
				!strcmp("1", nvram_bufget(RT2860_NVRAM, "ethConvert")))) {
		if (-1 != initStaProfile())
			initStaConnection();
	}
#endif

#if !defined(CONFIG_RALINK_RT2880) && !defined(CONFIG_RALINK_MT7620)
	if (!strcmp(auth_mode, "Disable") || !strcmp(auth_mode, "OPEN"))
		ledAlways(13, LED_OFF); //turn off security LED (gpio 13)
	else
		ledAlways(13, LED_ON); //turn on security LED (gpio 13)
#endif

#if defined (RT2860_WAPI_SUPPORT) || defined (RTDEV_WAPI_SUPPORT)
	restartWAPIDaemon(RT2860_NVRAM);	// in wireless.c
#endif
#if ! defined (CONFIG_FIRST_IF_NONE) 
	restart8021XDaemon(RT2860_NVRAM);	// in wireless.c
#endif
#if ! defined (CONFIG_SECOND_IF_NONE)
	restart8021XDaemon(RTDEV_NVRAM);	// in wireless.c
#endif

#ifdef CONFIG_RT2860V2_RT3XXX_ANTENNA_DIVERSITY
	AntennaDiversityInit();
#endif

	firewall_init();
	management_init();
	RoutingInit();
	nvram_bufset(RT2860_NVRAM, "needreboot", "no");
	//sleep(20);
	//doSystem("apuplktos");
#if defined CONFIG_LKTOS_PRIVATE_OEM_NAME_CDRKING
	system("gpio c 13 1");
#else
	system("gpio u 0"); //by luot for close the usb led init
#endif
#ifdef CONFIG_RALINKAPP_SWQOS
	QoSInit();
#endif
#if defined (CONFIG_IPV6)
	ipv6Config(strtol(nvram_bufget(RT2860_NVRAM, "IPv6OpMode"), NULL, 10));
#endif

	return 0;
}

static void getMyMAC(webs_t wp, char_t *path, char_t *query)
{
	char myMAC[32];

	arplookup(wp->ipaddr, myMAC);
	websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nCache-Control: no-cache\n\n"));
	websWrite(wp, T("%s"), myMAC);
	websDone(wp, 200);
}

/* goform/setLan */
static void setLan(webs_t wp, char_t *path, char_t *query)
{
	char_t  *submitUrl = websGetVar(wp, T("submit-url"), T(""));

	char_t	*ip, *nm, *dhcp_tp, *stp_en, *lltd_en, *igmp_en,
			*radvd_en, *pppoer_en, *dnsp_en;
	char_t	*gw = NULL, *pd = NULL, *sd = NULL;
	char_t *lan2enabled, *lan2_ip, *lan2_nm;
#ifdef GA_HOSTNAME_SUPPORT
	char_t	*host;
#endif
	char_t  *dhcp_s, *dhcp_e, *dhcp_m, *dhcp_pd, *dhcp_sd, *dhcp_g, *dhcp_l;
	const char	*opmode = nvram_bufget(RT2860_NVRAM, "OperationMode");
	const char	*wan_ip = nvram_bufget(RT2860_NVRAM, "wan_ipaddr");
	const char	*ctype = nvram_bufget(RT2860_NVRAM, "connectionType");

	ip = websGetVar(wp, T("lanIp"), T(""));
	nm = websGetVar(wp, T("lanNetmask"), T(""));
	lan2enabled = websGetVar(wp, T("lan2enabled"), T(""));
	lan2_ip = websGetVar(wp, T("lan2Ip"), T(""));
	lan2_nm = websGetVar(wp, T("lan2Netmask"), T(""));
#ifdef GA_HOSTNAME_SUPPORT
	host = websGetVar(wp, T("hostname"), T("0"));
#endif
	dhcp_tp = websGetVar(wp, T("lanDhcpType"), T("DISABLE"));
	stp_en = websGetVar(wp, T("stpEnbl"), T("0"));
	lltd_en = websGetVar(wp, T("lltdEnbl"), T("0"));
	igmp_en = websGetVar(wp, T("igmpEnbl"), T("0"));
	radvd_en = websGetVar(wp, T("radvdEnbl"), T("0"));
	pppoer_en = websGetVar(wp, T("pppoeREnbl"), T("0"));
	dnsp_en = websGetVar(wp, T("dnspEnbl"), T("0"));
	dhcp_s = websGetVar(wp, T("dhcpStart"), T(""));
	dhcp_e = websGetVar(wp, T("dhcpEnd"), T(""));
	dhcp_m = websGetVar(wp, T("dhcpMask"), T(""));
	dhcp_pd = websGetVar(wp, T("dhcpPriDns"), T(""));
	dhcp_sd = websGetVar(wp, T("dhcpSecDns"), T(""));
	dhcp_g = websGetVar(wp, T("dhcpGateway"), T(""));
	dhcp_l = websGetVar(wp, T("dhcpLease"), T("86400"));

	/*
	 * check static ip address:
	 * lan and wan ip should not be the same except in bridge mode
	 */
	if (strncmp(ctype, "STATIC", 7)) {
		if (strcmp(opmode, "0") && !strncmp(ip, wan_ip, 15)) {
			websError(wp, 200, "IP address is identical to WAN");
			return;
		}
		if (!strcmp(lan2enabled, "1"))
		{
			if (strcmp(opmode, "0") && !strncmp(lan2_ip, wan_ip, 15)) {
				websError(wp, 200, "LAN2 IP address is identical to WAN");
				return;
			}
			else if (strcmp(opmode, "0") && !strncmp(lan2_ip, ip, 15)) {
				websError(wp, 200, "LAN2 IP address is identical to LAN1");
				return;
			}
		}
	}
	// configure gateway and dns (WAN) at bridge mode
	if (!strncmp(opmode, "0", 2)) {
		gw = websGetVar(wp, T("lanGateway"), T(""));
		pd = websGetVar(wp, T("lanPriDns"), T(""));
		sd = websGetVar(wp, T("lanSecDns"), T(""));
		nvram_bufset(RT2860_NVRAM, "wan_gateway", gw);
		nvram_bufset(RT2860_NVRAM, "wan_primary_dns", pd);
		nvram_bufset(RT2860_NVRAM, "wan_secondary_dns", sd);
	}
	nvram_bufset(RT2860_NVRAM, "lan_ipaddr", ip);
	nvram_bufset(RT2860_NVRAM, "lan_netmask", nm);
	nvram_bufset(RT2860_NVRAM, "Lan2Enabled", lan2enabled);
	nvram_bufset(RT2860_NVRAM, "lan2_ipaddr", lan2_ip);
	nvram_bufset(RT2860_NVRAM, "lan2_netmask", lan2_nm);
#ifdef GA_HOSTNAME_SUPPORT
	nvram_bufset(RT2860_NVRAM, "HostName", host);
#endif
	if (!strncmp(dhcp_tp, "DISABLE", 8))
		nvram_bufset(RT2860_NVRAM, "dhcpEnabled", "0");
	else if (!strncmp(dhcp_tp, "SERVER", 7)) {
		if (-1 == inet_addr(dhcp_s)) {
			nvram_commit(RT2860_NVRAM);
			websError(wp, 200, "invalid DHCP Start IP");
			return;
		}
		nvram_bufset(RT2860_NVRAM, "dhcpStart", dhcp_s);
		if (-1 == inet_addr(dhcp_e)) {
			nvram_commit(RT2860_NVRAM);
			websError(wp, 200, "invalid DHCP End IP");
			return;
		}
		nvram_bufset(RT2860_NVRAM, "dhcpEnd", dhcp_e);
		if (-1 == inet_addr(dhcp_m)) {
			nvram_commit(RT2860_NVRAM);
			websError(wp, 200, "invalid DHCP Subnet Mask");
			return;
		}
		nvram_bufset(RT2860_NVRAM, "dhcpMask", dhcp_m);
		nvram_bufset(RT2860_NVRAM, "dhcpEnabled", "1");
		nvram_bufset(RT2860_NVRAM, "dhcpPriDns", dhcp_pd);
		nvram_bufset(RT2860_NVRAM, "dhcpSecDns", dhcp_sd);
		nvram_bufset(RT2860_NVRAM, "dhcpGateway", dhcp_g);
		nvram_bufset(RT2860_NVRAM, "dhcpLease", dhcp_l);
	}
	nvram_bufset(RT2860_NVRAM, "stpEnabled", stp_en);
	nvram_bufset(RT2860_NVRAM, "lltdEnabled", lltd_en);
	nvram_bufset(RT2860_NVRAM, "igmpEnabled", igmp_en);
	nvram_bufset(RT2860_NVRAM, "radvdEnabled", radvd_en);
	nvram_bufset(RT2860_NVRAM, "pppoeREnabled", pppoer_en);
	nvram_bufset(RT2860_NVRAM, "dnsPEnabled", dnsp_en);
	nvram_commit(RT2860_NVRAM);

//#if defined(CONFIG_RALINK_MT7620)
	//doSystem("lktos_networkconfig mtk7620 initlan");
//#else
	//doSystem("lktos_networkconfig ra5350 initlan");
	//initInternet();
//#endif
	doSystem("lktos_initconfig ar9331hc lan");
	websRedirect(wp, submitUrl);
}

/* goform/setStaticDhcp */
static void setStaticDhcp(webs_t wp, char_t *path, char_t *query)
{
	char_t  *submitUrl = websGetVar(wp, T("submit-url"), T(""));

	char_t	*dhcp_sl1, *dhcp_sl2, *dhcp_sl3;
	char_t	*dhcp_sl4, *dhcp_sl5, *dhcp_sl6, *dhcp_sl7, *dhcp_sl8, *dhcp_sl9, *dhcp_sl10;

	dhcp_sl1 = websGetVar(wp, T("dhcpStatic1"), T(""));
	dhcp_sl2 = websGetVar(wp, T("dhcpStatic2"), T(""));
	dhcp_sl3 = websGetVar(wp, T("dhcpStatic3"), T(""));
	dhcp_sl4 = websGetVar(wp, T("dhcpStatic4"), T(""));
	dhcp_sl5 = websGetVar(wp, T("dhcpStatic5"), T(""));
	dhcp_sl6 = websGetVar(wp, T("dhcpStatic6"), T(""));
	dhcp_sl7 = websGetVar(wp, T("dhcpStatic7"), T(""));
	dhcp_sl8 = websGetVar(wp, T("dhcpStatic8"), T(""));
	dhcp_sl9 = websGetVar(wp, T("dhcpStatic9"), T(""));
	dhcp_sl10 = websGetVar(wp, T("dhcpStatic10"), T(""));


	nvram_bufset(RT2860_NVRAM, "dhcpStatic1", dhcp_sl1);
	nvram_bufset(RT2860_NVRAM, "dhcpStatic2", dhcp_sl2);
	nvram_bufset(RT2860_NVRAM, "dhcpStatic3", dhcp_sl3);
	nvram_bufset(RT2860_NVRAM, "dhcpStatic4", dhcp_sl4);
	nvram_bufset(RT2860_NVRAM, "dhcpStatic5", dhcp_sl5);
	nvram_bufset(RT2860_NVRAM, "dhcpStatic6", dhcp_sl6);
	nvram_bufset(RT2860_NVRAM, "dhcpStatic7", dhcp_sl7);
	nvram_bufset(RT2860_NVRAM, "dhcpStatic8", dhcp_sl8);
	nvram_bufset(RT2860_NVRAM, "dhcpStatic9", dhcp_sl9);
	nvram_bufset(RT2860_NVRAM, "dhcpStatic10", dhcp_sl10);
	nvram_commit(RT2860_NVRAM);

//#if defined(CONFIG_RALINK_MT7620)
	//doSystem("lktos_networkconfig mtk7620 initlan");
//#else
	//doSystem("lktos_networkconfig ra5350 initlan");
	//initInternet();
//#endif
	doSystem("lktos_initconfig ar9331hc lan");
	websRedirect(wp, submitUrl);
}

/* goform/setUpnp */
static void setUpnp(webs_t wp, char_t *path, char_t *query)
{
	char_t  *submitUrl = websGetVar(wp, T("submit-url"), T(""));
	char_t	*upnp_en = websGetVar(wp, T("upnpEnbl"), T("0"));

	nvram_bufset(RT2860_NVRAM, "upnpEnabled", upnp_en);
	nvram_commit(RT2860_NVRAM);

//#if defined(CONFIG_RALINK_MT7620)
	//doSystem("lktos_networkconfig mtk7620 initlan");
//#else
	//doSystem("lktos_networkconfig ra5350 initlan");
	//initInternet();
//#endif

	websRedirect(wp, submitUrl);
}

/* goform/setIgmp */
static void setIgmp(webs_t wp, char_t *path, char_t *query)
{
	char_t  *submitUrl = websGetVar(wp, T("submit-url"), T(""));
 	char_t	*igmp_en = websGetVar(wp, T("igmpEnbl"), T("0"));

	nvram_bufset(RT2860_NVRAM, "igmpEnabled", igmp_en);
	nvram_commit(RT2860_NVRAM);

//#if defined(CONFIG_RALINK_MT7620)
	//doSystem("lktos_networkconfig mtk7620 initlan");
//#else
	//doSystem("lktos_networkconfig ra5350 initlan");
	//initInternet();
//#endif
	websRedirect(wp, submitUrl);
}

#if defined (CONFIG_IPV6)
void ipv6Config(int mode)
{
	const char *wan_v6addr = nvram_bufget(RT2860_NVRAM, "IPv6WANIPAddr");
#if defined (CONFIG_IPV6_SIT_6RD) || defined (CONFIG_IPV6_TUNNEL)
	const char *srv_v6addr = nvram_bufget(RT2860_NVRAM, "IPv6SrvAddr");
#endif
	int prefix_len = strtol(nvram_bufget(RT2860_NVRAM, "IPv6PrefixLen"), NULL, 10);
	int wan_prefix_len = strtol(nvram_bufget(RT2860_NVRAM, "IPv6WANPrefixLen"), NULL, 10);
	const char *gw_v6addr = nvram_bufget(RT2860_NVRAM, "IPv6GWAddr");
	char *wan_if = getWanIfName();
	char *lan_if = getLanIfName();
	char v6addr[40]; 
#if defined (CONFIG_IPV6_SIT_6RD)
	char wan_addr[16]; 
	char ipv6_ip_addr[20];
	unsigned short temp[8];
	int i, used, shift;
	char *tok = NULL;
#endif

	strcpy(v6addr, nvram_bufget(RT2860_NVRAM, "IPv6IPAddr"));
#if defined (CONFIG_IPV6_SIT_6RD)
	doSystem("ip link set 6rdtun down");
#endif
	doSystem("echo 0 > /proc/sys/net/ipv6/conf/all/forwarding");

	switch (mode) {
	case 1:
		doSystem("ifconfig %s add  %s/%d", lan_if, v6addr, prefix_len);
		doSystem("ifconfig %s add  %s/%d", wan_if, wan_v6addr, wan_prefix_len);
		doSystem("route -A inet6 add default gw %s dev %s", gw_v6addr, wan_if);
		doSystem("echo 1 > /proc/sys/net/ipv6/conf/all/forwarding");
		// doSystem("ecmh");
		break;
#if defined (CONFIG_IPV6_SIT_6RD)
	case 2:
		if (getIfIp(getWanIfNamePPP(), wan_addr) < 0) {
			fprintf(stderr, "Can't Query WAN IPv4 Address!\n");
			return;
		}
		memset(temp, 0, sizeof(temp));
		doSystem("ip tunnel add 6rdtun mode sit local %s ttl 64", wan_addr);
		for (i=0, tok = strtok(v6addr, ":"); tok; i++, tok = strtok(NULL, ":"))
			temp[i] = strtol(tok, NULL, 16);
		if ((shift = 16 - (prefix_len % 16)) < 16) {
			temp[i-1] = (temp[i-1] >> shift) << shift;
		}
		sprintf(v6addr, "%x", temp[0]);
		for (used=1; used<i; used++)
			sprintf(v6addr, "%s:%x", v6addr, temp[used]);
		for (tok = strtok(wan_addr, "."); tok; i++, tok = strtok(NULL, ".")) {
			temp[i] = strtol(tok, NULL, 10)<<8;
			tok = strtok(NULL, ".");
			temp[i] += strtol(tok, NULL, 10);
		}
		if (shift < 16) {
			used = prefix_len / 16;
			while (used < i) {
				temp[used] = (temp[used] >> shift) << shift; 
				temp[used] += temp[used+1]>>(16-shift);
				temp[used+1] <<= shift;
				used++;
			}
		} else {
			used = i;
		}
		sprintf(ipv6_ip_addr, "%x", temp[0]);
		for (i=1; i<used; i++) {
			sprintf(ipv6_ip_addr, "%s:%x", ipv6_ip_addr, temp[i]);
		}
		doSystem("ip tunnel 6rd dev 6rdtun 6rd-prefix %s::/%d", v6addr, prefix_len);
		doSystem("ip addr add %s::1/%d dev 6rdtun", ipv6_ip_addr, prefix_len);
		doSystem("ip link set 6rdtun up");
		doSystem("ip route add ::/0 via ::%s dev 6rdtun", srv_v6addr);
		doSystem("ip addr add %s::1/64 dev %s", ipv6_ip_addr, lan_if);
		doSystem("echo 1 > /proc/sys/net/ipv6/conf/all/forwarding");
		doSystem("radvd.sh %s", ipv6_ip_addr);
		break;
#endif
#if defined (CONFIG_IPV6_TUNNEL)
	case 3:
		doSystem("config-dslite.sh %s %s %s", srv_v6addr, v6addr, gw_v6addr);
		break;
#endif
	default:
		break;
	}
	return;
}

/* goform/setIPv6 */
static void setIPv6(webs_t wp, char_t *path, char_t *query)
{
	char_t  *submitUrl = websGetVar(wp, T("submit-url"), T(""));

	char_t	*opmode;
	char_t  *ipaddr, *prefix_len, *wan_ipaddr, *wan_prefix_len, *srv_ipaddr;
#if defined (CONFIG_IPV6_TUNNEL)
	char_t  *gw_ipaddr = NULL;
#endif

	ipaddr = prefix_len = wan_ipaddr = wan_prefix_len = srv_ipaddr = NULL;
	opmode = websGetVar(wp, T("ipv6_opmode"), T("0"));
	if (!strcmp(opmode, "1")) {
		ipaddr = websGetVar(wp, T("ipv6_lan_ipaddr"), T(""));
		prefix_len = websGetVar(wp, T("ipv6_lan_prefix_len"), T(""));
		wan_ipaddr = websGetVar(wp, T("ipv6_wan_ipaddr"), T(""));
		wan_prefix_len = websGetVar(wp, T("ipv6_wan_prefix_len"), T(""));
		srv_ipaddr = websGetVar(wp, T("ipv6_static_gw"), T(""));
		nvram_bufset(RT2860_NVRAM, "IPv6IPAddr", ipaddr);
		nvram_bufset(RT2860_NVRAM, "IPv6PrefixLen", prefix_len);
		nvram_bufset(RT2860_NVRAM, "IPv6WANIPAddr", wan_ipaddr);
		nvram_bufset(RT2860_NVRAM, "IPv6WANPrefixLen", wan_prefix_len);
		nvram_bufset(RT2860_NVRAM, "IPv6GWAddr", srv_ipaddr);
#if defined (CONFIG_IPV6_SIT_6RD)
	} else if (!strcmp(opmode, "2")) {
		ipaddr = websGetVar(wp, T("ipv6_6rd_prefix"), T(""));
		prefix_len = websGetVar(wp, T("ipv6_6rd_prefix_len"), T(""));
		srv_ipaddr = websGetVar(wp, T("ipv6_6rd_border_ipaddr"), T(""));
		nvram_bufset(RT2860_NVRAM, "IPv6IPAddr", ipaddr);
		nvram_bufset(RT2860_NVRAM, "IPv6PrefixLen", prefix_len);
		nvram_bufset(RT2860_NVRAM, "IPv6SrvAddr", srv_ipaddr);
		nvram_bufset(RT2860_NVRAM, "radvdEnabled", "1");
#endif
#if defined (CONFIG_IPV6_TUNNEL)
	} else if (!strcmp(opmode, "3")) {
		ipaddr = websGetVar(wp, T("ipv6_ds_wan_ipaddr"), T(""));
		srv_ipaddr = websGetVar(wp, T("ipv6_ds_aftr_ipaddr"), T(""));
		gw_ipaddr = websGetVar(wp, T("ipv6_ds_gw_ipaddr"), T(""));
		nvram_bufset(RT2860_NVRAM, "IPv6IPAddr", ipaddr);
		nvram_bufset(RT2860_NVRAM, "IPv6SrvAddr", srv_ipaddr);
		nvram_bufset(RT2860_NVRAM, "IPv6GWAddr", gw_ipaddr);
#endif
	}
	nvram_bufset(RT2860_NVRAM, "IPv6OpMode", opmode);
	nvram_commit(RT2860_NVRAM);
	initInternet();

	websRedirect(wp, submitUrl);
}
#endif

#if 1
int File_Get_Modem_Info(char *name,char *value, int len)
{
	FILE *fp=NULL;
	char path[64];
	int	num=0;

	memset(path,'\0',64);
	sprintf(path,"%s%s","/var/usbmodem/",name);

	fp = fopen(path,"r");
	if(fp == NULL)
	{
		return -1;
	}
	else
	{		
		num=fgets(value,len,fp);
		fclose(fp);
		fp = NULL;
		return num;
	}
	
}

//zhubo add 2010.04.19 for 3G dial
static int getDial3gStatus(int eid, webs_t wp, int argc, char_t **argv)
{
	char if_addr[16];

	if (getIfIp(getWanIfNamePPP(), if_addr) != -1) 
		websWrite(wp, "UP");
	else
		websWrite(wp, "");	
}

static int getDial3gSimCard(int eid, webs_t wp, int argc, char_t **argv)
{
	char sim_card[16];

	memset(sim_card, '\0', 16);
	File_Get_Modem_Info("dial3gsimcard", sim_card, 16);

	websWrite(wp, sim_card);
}

static int getDial3gPPPDFlag(int eid, webs_t wp, int argc, char_t **argv)
{
	if(DBgetPid("pppd"))
		websWrite(wp, "1");
	else
		websWrite(wp, "");
}

static int getDial3gUsbModemInfo(int eid, webs_t wp, int argc, char_t **argv)
{
	char modem_info[32];//pid/vid
	char p_vid[16];
	int i=0;

	memset(modem_info, '\0', 32);
	memset(p_vid, '\0', 16);
	File_Get_Modem_Info("modeminfo", modem_info, 32);

	for(i=0; i<13; i++)
		p_vid[i] = modem_info[i];
	websWrite(wp, p_vid);
}

static int getDial3gUsbModemFlag(int eid, webs_t wp, int argc, char_t **argv)
{
	char modem_flag[4]; //yes or no

	memset(modem_flag, '\0', 4);
	File_Get_Modem_Info("usbmodemflag", modem_flag, 4);

	websWrite(wp, modem_flag);
}

static int getDial3gCimiValue(int eid, webs_t wp, int argc, char_t **argv)
{
	char cimivalue[32];

	memset(cimivalue, '\0', 32);
	File_Get_Modem_Info("cimivalue",cimivalue, 32);

	websWrite(wp, cimivalue);
}

static int getDial3gUsbModemFirmware(int eid, webs_t wp, int argc, char_t **argv)
{
	char modem_version[256];

	memset(modem_version, '\0', 256);
	File_Get_Modem_Info("modemversion",modem_version, 256);

	websWrite(wp, modem_version);
}

static int getDial3gSignalLength(int eid, webs_t wp, int argc, char_t **argv)
{
	char signal[4], value[16];
	int	  	number=0;
	float  	percent;

	memset(signal, '\0', 4);
	memset(value, '\0', 16);

	File_Get_Modem_Info("dial3gsignal", signal, 4);
	if(signal[0]=='\0'||strcmp(signal,"0")==0||strcmp(signal,"-1")==0) {
		websWrite(wp, "0.0");
	}
	else {
		number = atoi(signal);
		if(number>=99) {
			if(number==99||number==100||number==199)
				percent = 0.0;
			else
				percent = (float)((number-99)/99.0)*100;
		}			
		else
			percent = (float)(number/31.0)*100;

		sprintf(value,"%.1f",percent);

		websWrite(wp, value);
	}		
	
}

static int getDial3gISPProvider(int eid, webs_t wp, int argc, char_t **argv)
{
	char ispname[32];

	memset(ispname, '\0', 32);
	File_Get_Modem_Info("dial3gprovider", ispname, 32);
	websWrite(wp, ispname);
}
#endif

/* goform/setVpnPaThru */
static void setVpnPaThru(webs_t wp, char_t *path, char_t *query)
{
	char_t  *submitUrl = websGetVar(wp, T("submit-url"), T(""));

	char_t	*l2tp_pt, *ipsec_pt, *pptp_pt;

	l2tp_pt = websGetVar(wp, T("l2tpPT"), T("0"));
	ipsec_pt = websGetVar(wp, T("ipsecPT"), T("0"));
	pptp_pt = websGetVar(wp, T("pptpPT"), T("0"));
	
	nvram_bufset(RT2860_NVRAM, "l2tpPassThru", l2tp_pt);
	nvram_bufset(RT2860_NVRAM, "ipsecPassThru", ipsec_pt);
	nvram_bufset(RT2860_NVRAM, "pptpPassThru", pptp_pt);
	nvram_commit(RT2860_NVRAM);

	doSystem("vpn-passthru.sh");

	websRedirect(wp, submitUrl);
}

#if 0
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
#endif

typedef enum { 
	IP_ADDR, 
	SUBNET_MASK, 
	DEFAULT_GATEWAY, 
	HW_ADDR 
} ADDR_T;

int getIfAddr( char *interface, ADDR_T type, void *pAddr )
{
    struct ifreq ifr;
    int skfd, found=0;
    struct sockaddr_in *addr;

    skfd = socket(AF_INET, SOCK_DGRAM, 0);

    strcpy(ifr.ifr_name, interface);
    if (ioctl(skfd, SIOCGIFFLAGS, &ifr) < 0)
		return (0);

    if (type == HW_ADDR) {
    	if (ioctl(skfd, SIOCGIFHWADDR, &ifr) >= 0) {
			memcpy(pAddr, &ifr.ifr_hwaddr, sizeof(struct sockaddr));
			found = 1;
		}
    }
    else if (type == IP_ADDR) {
		if (ioctl(skfd, SIOCGIFADDR, &ifr) == 0) {
			addr = ((struct sockaddr_in *)&ifr.ifr_addr);
			*((struct in_addr *)pAddr) = *((struct in_addr *)&addr->sin_addr);
			found = 1;
		}
    }
    else if (type == SUBNET_MASK) {
		if (ioctl(skfd, SIOCGIFNETMASK, &ifr) >= 0) {
			addr = ((struct sockaddr_in *)&ifr.ifr_addr);
			*((struct in_addr *)pAddr) = *((struct in_addr *)&addr->sin_addr);
			found = 1;
		}
   	}
    close( skfd );
    return found;
}

static void formConnectAutoCheck(webs_t wp, char_t *path, char_t *query)
{
	char myMAC[32];
	char test[30]="test auto";
	struct stat st;
	int intVal,i;
	struct in_addr	intaddr;
	int link=0;
	char WAN_IF[20]="br1";	char *iface;

	FILE *fp;
	char *p;
	char long_buf[4096];
	int  ret = 1;

	//get WAN Link status
#if defined CONFIG_WAN_AT_P4
	system("mii_mgr -g -p 4 -r 1 > /tmp/wanConnectStatus");
#else
	system("mii_mgr -g -p 0 -r 1 > /tmp/wanConnectStatus");
#endif
	fp = fopen("/tmp/wanConnectStatus", "r");
	if (!fp) return NULL;
	while(fgets(long_buf, 512, fp)) {
		p = strstr(long_buf, "=");	
		*(p+2+3)='\0';
		if(((atoi(p+2+2)) & 0x0002))  link=1;	//Is connected
	}
	fclose(fp);
	//link down
	if(link!=1) {
		//printf("+++link down\n");
		sprintf(test, "%s", "linkN");
		websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\n\n"));
		websWrite(wp, T("%s"), test);
		websDone(wp, 200);
		goto check_OK; 
	}
	//pppoe
	arplookup(wp->ipaddr, myMAC);
	system("killall -9 syslogd");
	system("killall -9 udhcpc");
	system("killall -9 l2tpd");	
	system("killall -q openl2tpd");
	system("killall -q ppp_daemon");
	system("killall -9 pppd");	
	system("rm -f /tmp/pppco");
	system("rm -f /tmp/isppp");
	system("config-pppoe.sh user testpass br1 KeepAlive 60");
	for(i=0;i<10;i++) {
		//printf("+++check pppoe1\n");
		intVal=stat("/tmp/isppp",&st);
		if(!intVal) {
			sprintf(test, "%s", "pppoe") ;
			websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\n\n"));
			websWrite(wp, T("%s"), test);
			websDone(wp, 200);
			goto check_OK; 
		}
		sleep(3);		
	}
	
	intVal=stat("/tmp/pppco",&st);
	if (intVal) {
		//printf("+++check pppoe2\n");
		sprintf(test, "%s", "connect type is pppoe") ;
		websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\n\n"));
		websWrite(wp, T("%s"), test);
		websDone(wp, 200);
		goto check_OK;
	}
	//dhcp
	system("killall -9 pppd");
	system("killall -9 udhcpc");
	system("udhcpc -i eth0-s /sbin/udhcpc.sh -p /var/run/udhcpc.pid &");
	for(i=0;i<10;i++) {
		iface = WAN_IF;
		if (iface && getIfAddr(iface, IP_ADDR, (void *)&intaddr)) {
			//printf("+++check dhcp\n");
			sprintf(test, "%s", "dhcpc") ;
			websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\n\n"));
			websWrite(wp, T("%s"), test);
			websDone(wp, 200);
			goto check_OK; 
		}
		sleep(3);
	}
	//static ip
	sprintf(test, "%s", "may be static ip") ;
	websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\n\n"));
	websWrite(wp, T("%s"), test);
	websDone(wp, 200);
	
check_OK:
	return;
}


/* goform/setWan */
static void setWan(webs_t wp, char_t *path, char_t *query)
{
	char_t  *submitUrl = websGetVar(wp, T("submit-url"), T(""));

	char_t	*ctype;
	char_t	*ip, *nm, *gw, *pd, *sd;
	char_t	*eth, *user, *pass, *hostname;
	char_t	*clone_en, *clone_mac;
	char_t  *pptp_srv, *pptp_mode;
	char_t  *l2tp_srv, *l2tp_mode;
	char_t   *tmpstr;
	char_t	vlansetBuf[32]={0};
	int pid,tmpflag=0;
#ifdef CONFIG_USER_3G
	char_t  *simpin, *choicetype, *dialnum, *usrname, *passwd, *apn;
	int		pid_pppd=0;
#endif
	const char	*opmode = nvram_bufget(RT2860_NVRAM, "OperationMode");
	const char	*lan_ip = nvram_bufget(RT2860_NVRAM, "lan_ipaddr");
	const char	*lan2enabled = nvram_bufget(RT2860_NVRAM, "Lan2Enabled");

	ctype = ip = nm = gw = pd = sd = eth = user = pass = hostname =
		clone_en = clone_mac = pptp_srv = pptp_mode = l2tp_srv = l2tp_mode =
		NULL;

	ctype = websGetVar(wp, T("connectionType"), T("0")); 
	if (!strncmp(ctype, "STATIC", 7) || !strcmp(opmode, "0")) {
		//always treat bridge mode having static wan connection
		ip = websGetVar(wp, T("staticIp"), T(""));
		nm = websGetVar(wp, T("staticNetmask"), T("0"));
		gw = websGetVar(wp, T("staticGateway"), T(""));
		pd = websGetVar(wp, T("staticPriDns"), T(""));
		sd = websGetVar(wp, T("staticSecDns"), T(""));

		nvram_bufset(RT2860_NVRAM, "wanConnectionMode", ctype);
		if (-1 == inet_addr(ip)) {
			nvram_commit(RT2860_NVRAM);
			websError(wp, 200, "invalid IP Address");
			return;
		}
		/*
		 * lan and wan ip should not be the same except in bridge mode
		 */
		 printf("\r\nip=%s",ip);
		if (NULL != opmode && strcmp(opmode, "0") && !strncmp(ip, lan_ip, 15)) {
			nvram_commit(RT2860_NVRAM);
			websError(wp, 200, "IP address is identical to LAN");
			return;
		}
		if (!strcmp(lan2enabled, "1"))
		{
			const char	*lan2_ip = nvram_bufget(RT2860_NVRAM, "lan2_ipaddr");
			if (NULL != opmode && strcmp(opmode, "0") && !strncmp(ip, lan2_ip, 15)) {
				nvram_commit(RT2860_NVRAM);
				websError(wp, 200, "IP address is identical to LAN2");
				return;
			}
		}
		nvram_bufset(RT2860_NVRAM, "wan_ipaddr", ip);
		if (-1 == inet_addr(nm)) {
			nvram_commit(RT2860_NVRAM);
			websError(wp, 200, "invalid Subnet Mask");
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
			hostname = websGetVar(wp, T("hostname"), T(""));

			nvram_bufset(RT2860_NVRAM, "wanConnectionMode", ctype);
			nvram_bufset(RT2860_NVRAM, "wan_dhcp_hn", hostname);
		}
		else if (!strncmp(ctype, "PPPOE", 6)) {
			char_t *pppoe_opmode, *pppoe_optime, *pppoe_spectype;

			user = websGetVar(wp, T("pppoeUser"), T(""));
			pass = websGetVar(wp, T("pppoePass"), T(""));
			pppoe_spectype = websGetVar(wp, T("specType"), T("0"));
			
			pppoe_opmode = websGetVar(wp, T("pppoeOPMode"), T(""));			
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
		#if 0
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
		while(1)
		{
			pid_pppd = DBgetPid("pppd");
			if(pid_pppd>0)
			{
				doSystem("killall -15 pppd");
				sleep(2);
				continue;
			}			
			doSystem("comgt -d /var/ttyACM0 -s /etc_ro/ppp/3g/Generic_disconn.scr");
			break;
		}	
	
		choicetype = websGetVar(wp, T("Config3G"), T(""));
		dialnum = websGetVar(wp, T("Dial3G"), T(""));
		usrname = websGetVar(wp, T("User3G"), T(""));
		passwd  = websGetVar(wp, T("Password3G"), T(""));
		apn		= websGetVar(wp, T("APN3G"), T(""));
		simpin = websGetVar(wp, T("PIN3G"), T(""));
		
		nvram_bufset(RT2860_NVRAM, "dial3gchoicetype", choicetype);
		nvram_bufset(RT2860_NVRAM, "dial3gnum", dialnum);
		nvram_bufset(RT2860_NVRAM, "dial3gusername", usrname);
		nvram_bufset(RT2860_NVRAM, "dial3gpassword", passwd);
		nvram_bufset(RT2860_NVRAM, "dial3gapn", apn);
		nvram_bufset(RT2860_NVRAM, "dial3gsimpin", simpin);
		nvram_bufset(RT2860_NVRAM, "wanConnectionMode", ctype);	
	}
#endif
		else {
			websHeader(wp);
			websWrite(wp, T("<h2>Unknown Connection Type: %s</h2><br>\n"), ctype);
			websFooter(wp);
			websDone(wp, 200);
			return;
		}
	}

	// mac clone
	clone_en = websGetVar(wp, T("macCloneEnbl"), T("0"));
	clone_mac = websGetVar(wp, T("macCloneMac"), T(""));
	nvram_bufset(RT2860_NVRAM, "macCloneEnabled", clone_en);
	if (!strncmp(clone_en, "1", 2))
		nvram_bufset(RT2860_NVRAM, "macCloneMac", clone_mac);
	
	tmpstr = websGetVar(wp, T("workconnectmode"), T("0"));
	nvram_bufset(RT2860_NVRAM, "Connect1OperationMode", tmpstr);
	tmpstr = websGetVar(wp, T("ethport3"), T("OFF"));
	if (!strncmp(tmpstr, "ON", 3))
		nvram_bufset(RT2860_NVRAM, "Connect1ethports", "3");
	else
		nvram_bufset(RT2860_NVRAM, "Connect1ethports", "");
	
	tmpstr = websGetVar(wp, T("wifiport0"), T("OFF"));
	if (!strncmp(tmpstr, "ON", 3))
	{
		sprintf(vlansetBuf,"%s","0");
		tmpflag=1;
	}
	tmpstr = websGetVar(wp, T("wifiport1"), T("OFF"));
	if (!strncmp(tmpstr, "ON", 3))
	{
		if(tmpflag)
			sprintf(vlansetBuf,"%s;%s",vlansetBuf,"1");
		else
		{
			sprintf(vlansetBuf,"%s","1");
			tmpflag=1;
		}
	}
	tmpstr = websGetVar(wp, T("wifiport2"), T("OFF"));
	if (!strncmp(tmpstr, "ON", 3))
	{
		if(tmpflag)
			sprintf(vlansetBuf,"%s;%s",vlansetBuf,"2");
		else
		{
			sprintf(vlansetBuf,"%s","2");
			tmpflag=1;
		}
	}
	tmpstr = websGetVar(wp, T("wifiport3"), T("OFF"));
	if (!strncmp(tmpstr, "ON", 3))
	{
		if(tmpflag)
			sprintf(vlansetBuf,"%s;%s",vlansetBuf,"3");
		else
		{
			sprintf(vlansetBuf,"%s","3");
			tmpflag=1;
		}
	}
	if(tmpflag)
		nvram_bufset(RT2860_NVRAM, "Connect1wifiports", vlansetBuf);
	else
		nvram_bufset(RT2860_NVRAM, "Connect1wifiports", "");

	tmpstr = websGetVar(wp, T("vlanSetEnbl"), T("0"));
	nvram_bufset(RT2860_NVRAM, "Connect1vlanEn", tmpstr);
	nvram_bufset(RT2860_NVRAM, "Connect2vlanEn", tmpstr);//force connect1 and connect2 vlan enable

	tmpstr = websGetVar(wp, T("vlan1id"), T("0"));
	nvram_bufset(RT2860_NVRAM, "Connect1vlanid", tmpstr);

	tmpstr = websGetVar(wp, T("vlan1prio"), T("0"));
	nvram_bufset(RT2860_NVRAM, "Connect1vlanprio", tmpstr);
	
	nvram_commit(RT2860_NVRAM);

//#if defined(CONFIG_RALINK_MT7620)
	//doSystem("lktos_networkconfig mtk7620 initwan");
//#else
	//printf("\r\n goto init wan");
	//sleep(5);
	//system("echo 111 >> /tmp/tttt");
	//doSystem("lktos_networkconfig ra5350 initwan");
	//initInternet();
//#endif
	printf("\r\nstart----start\r\n");
	//system("lktos_initconfig ar9331hc wan &");
	//pid=fork();
	//if(pid==0)
	//{
		//system("lktos_initconfig ar9331hc wan &");
		//exit(0);
	//}
	//OK_MSG_luo(submitUrl);
	doSystem("lktos_initconfig ar9331hc wan");
	websRedirect(wp, submitUrl);
	return;
}




/* goform/setWan */
static void setWan2(webs_t wp, char_t *path, char_t *query)
{
	char_t  *submitUrl = websGetVar(wp, T("submit-url"), T(""));

	char_t	*ctype;
	char_t	*ip, *nm, *gw, *pd, *sd;
	char_t	*eth, *user, *pass, *hostname;
	char_t	*clone_en, *clone_mac;
	char_t  *pptp_srv, *pptp_mode;
	char_t  *l2tp_srv, *l2tp_mode;
	char_t   *tmpstr;
	char_t	vlansetBuf[32]={0};
	int tmpflag=0;
#ifdef CONFIG_USER_3G
	char_t  *simpin, *choicetype, *dialnum, *usrname, *passwd, *apn;
	int		pid_pppd=0;
#endif
	const char	*opmode = nvram_bufget(RT2860_NVRAM, "OperationMode");
	const char	*lan_ip = nvram_bufget(RT2860_NVRAM, "lan_ipaddr");
	const char	*lan2enabled = nvram_bufget(RT2860_NVRAM, "Lan2Enabled");

	ctype = ip = nm = gw = pd = sd = eth = user = pass = hostname =
		clone_en = clone_mac = pptp_srv = pptp_mode = l2tp_srv = l2tp_mode =
		NULL;

	ctype = websGetVar(wp, T("connectionType"), T("0")); 
	if (!strncmp(ctype, "STATIC", 7) || !strcmp(opmode, "0")) {
		//always treat bridge mode having static wan connection
		ip = websGetVar(wp, T("staticIp"), T(""));
		nm = websGetVar(wp, T("staticNetmask"), T("0"));
		gw = websGetVar(wp, T("staticGateway"), T(""));
		pd = websGetVar(wp, T("staticPriDns"), T(""));
		sd = websGetVar(wp, T("staticSecDns"), T(""));

		nvram_bufset(RT2860_NVRAM, "wanConnectionMode", ctype);
		if (-1 == inet_addr(ip)) {
			nvram_commit(RT2860_NVRAM);
			websError(wp, 200, "invalid IP Address");
			return;
		}
		/*
		 * lan and wan ip should not be the same except in bridge mode
		 */
		if (NULL != opmode && strcmp(opmode, "0") && !strncmp(ip, lan_ip, 15)) {
			nvram_commit(RT2860_NVRAM);
			websError(wp, 200, "IP address is identical to LAN");
			return;
		}
		if (!strcmp(lan2enabled, "1"))
		{
			const char	*lan2_ip = nvram_bufget(RT2860_NVRAM, "lan2_ipaddr");
			if (NULL != opmode && strcmp(opmode, "0") && !strncmp(ip, lan2_ip, 15)) {
				nvram_commit(RT2860_NVRAM);
				websError(wp, 200, "IP address is identical to LAN2");
				return;
			}
		}
		nvram_bufset(RT2860_NVRAM, "wan_ipaddr", ip);
		if (-1 == inet_addr(nm)) {
			nvram_commit(RT2860_NVRAM);
			websError(wp, 200, "invalid Subnet Mask");
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
			hostname = websGetVar(wp, T("hostname"), T(""));

			nvram_bufset(RT2860_NVRAM, "wanConnectionMode", ctype);
			nvram_bufset(RT2860_NVRAM, "wan_dhcp_hn", hostname);
		}
		else if (!strncmp(ctype, "PPPOE", 6)) {
			char_t *pppoe_opmode, *pppoe_optime, *pppoe_spectype;

			user = websGetVar(wp, T("pppoeUser"), T(""));
			pass = websGetVar(wp, T("pppoePass"), T(""));
			pppoe_spectype = websGetVar(wp, T("specType"), T("0"));
			
			pppoe_opmode = websGetVar(wp, T("pppoeOPMode"), T(""));			
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
		while(1)
		{
			pid_pppd = DBgetPid("pppd");
			if(pid_pppd>0)
			{
				doSystem("killall -15 pppd");
				sleep(2);
				continue;
			}			
			doSystem("comgt -d /var/ttyACM0 -s /etc_ro/ppp/3g/Generic_disconn.scr");
			break;
		}	
	
		choicetype = websGetVar(wp, T("Config3G"), T(""));
		dialnum = websGetVar(wp, T("Dial3G"), T(""));
		usrname = websGetVar(wp, T("User3G"), T(""));
		passwd  = websGetVar(wp, T("Password3G"), T(""));
		apn		= websGetVar(wp, T("APN3G"), T(""));
		simpin = websGetVar(wp, T("PIN3G"), T(""));
		
		nvram_bufset(RT2860_NVRAM, "dial3gchoicetype", choicetype);
		nvram_bufset(RT2860_NVRAM, "dial3gnum", dialnum);
		nvram_bufset(RT2860_NVRAM, "dial3gusername", usrname);
		nvram_bufset(RT2860_NVRAM, "dial3gpassword", passwd);
		nvram_bufset(RT2860_NVRAM, "dial3gapn", apn);
		nvram_bufset(RT2860_NVRAM, "dial3gsimpin", simpin);
		nvram_bufset(RT2860_NVRAM, "wanConnectionMode", ctype);	
	}
#endif
		else {
			websHeader(wp);
			websWrite(wp, T("<h2>Unknown Connection Type: %s</h2><br>\n"), ctype);
			websFooter(wp);
			websDone(wp, 200);
			return;
		}
	}

	// mac clone
	clone_en = websGetVar(wp, T("macCloneEnbl"), T("0"));
	clone_mac = websGetVar(wp, T("macCloneMac"), T(""));
	nvram_bufset(RT2860_NVRAM, "macCloneEnabled", clone_en);
	if (!strncmp(clone_en, "1", 2))
		nvram_bufset(RT2860_NVRAM, "macCloneMac", clone_mac);
	
	tmpstr = websGetVar(wp, T("workconnectmode"), T("1"));
	nvram_bufset(RT2860_NVRAM, "Connect2OperationMode", tmpstr);
	tmpstr = websGetVar(wp, T("ethport3"), T("OFF"));
	if (!strncmp(tmpstr, "ON", 3))
		nvram_bufset(RT2860_NVRAM, "Connect2ethports", "3");
	else
		nvram_bufset(RT2860_NVRAM, "Connect2ethports", "");
	
	tmpstr = websGetVar(wp, T("wifiport0"), T("OFF"));
	if (!strncmp(tmpstr, "ON", 3))
	{
		sprintf(vlansetBuf,"%s","0");
		tmpflag=1;
	}
	tmpstr = websGetVar(wp, T("wifiport1"), T("OFF"));
	if (!strncmp(tmpstr, "ON", 3))
	{
		if(tmpflag)
			sprintf(vlansetBuf,"%s;%s",vlansetBuf,"1");
		else
		{
			sprintf(vlansetBuf,"%s","1");
			tmpflag=1;
		}
	}
	tmpstr = websGetVar(wp, T("wifiport2"), T("OFF"));
	if (!strncmp(tmpstr, "ON", 3))
	{
		if(tmpflag)
			sprintf(vlansetBuf,"%s;%s",vlansetBuf,"2");
		else
		{
			sprintf(vlansetBuf,"%s","2");
			tmpflag=1;
		}
	}
	tmpstr = websGetVar(wp, T("wifiport3"), T("OFF"));
	if (!strncmp(tmpstr, "ON", 3))
	{
		if(tmpflag)
			sprintf(vlansetBuf,"%s;%s",vlansetBuf,"3");
		else
		{
			sprintf(vlansetBuf,"%s","3");
			tmpflag=1;
		}
	}
	if(tmpflag)
		nvram_bufset(RT2860_NVRAM, "Connect2wifiports", vlansetBuf);
	else
		nvram_bufset(RT2860_NVRAM, "Connect2wifiports", "");

	tmpstr = websGetVar(wp, T("vlanSetEnbl"), T("0"));
	nvram_bufset(RT2860_NVRAM, "Connect2vlanEn", tmpstr);
	nvram_bufset(RT2860_NVRAM, "Connect1vlanEn", tmpstr);//force connect1 and connect2 vlan enable

	tmpstr = websGetVar(wp, T("vlan1id"), T("0"));
	nvram_bufset(RT2860_NVRAM, "Connect2vlanid", tmpstr);

	tmpstr = websGetVar(wp, T("vlan1prio"), T("0"));
	nvram_bufset(RT2860_NVRAM, "Connect2vlanprio", tmpstr);
	
	nvram_commit(RT2860_NVRAM);

//#if defined(CONFIG_RALINK_MT7620)
	//doSystem("lktos_networkconfig mtk7620 initwan");
//#else
	//printf("\r\n goto init wan");
	//sleep(5);
	//system("echo 111 >> /tmp/tttt");
	//doSystem("lktos_networkconfig ra5350 initwan");
	//initInternet();
//#endif
	doSystem("lktos_initconfig ar9331hc wan");
	websRedirect(wp, submitUrl);
}


static void UpdateCert( webs_t wp, char_t *path, char_t *query)
{
	char file[128];
	FILE *pp;
	
	sleep (2);
	if (!strcmp(query, "key"))
		pp = popen("nvram_get cert KeyCertFile", "r");
	else if (!strcmp(query, "client"))
		pp = popen("nvram_get cert CACLCertFile", "r");
	else if (!strcmp(query, "as"))
		pp = popen("nvram_get wapi ASCertFile", "r");
	else if (!strcmp(query, "user"))
		pp = popen("nvram_get wapi UserCertFile", "r");
	else
		return;

	if (!pp) {
		error(E_L, E_LOG, T("Certificate update error"));
		return;
	}
	memset(file, 0, 128);
	fscanf(pp, "%s", file);
	pclose(pp);

	websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nCache-Control: no-cache\n\n"));

	if (strlen(file) > 0)
		websWrite(wp, T("%s"), file);
	else
		websWrite(wp, T(""));

    websDone(wp, 200);	
	return;
}

////-------------------------------------------------------
static int isIpValid(char *str)
{
	struct in_addr addr;	// for examination
	if( (! strcmp(T("any"), str)) || (! strcmp(T("any/0"), str)))
		return 1;

	if(! (inet_aton(str, &addr))){
		printf("isIpValid(): %s is not a valid IP address.\n", str);
		return 0;
	}
	return 1;
}

static int getStaticDhcpEnableASP(int eid, webs_t wp, int argc, char_t **argv)
{
	int type, value;
	char *pfe = (char *)nvram_bufget(RT2860_NVRAM, "staticDhcpEnabled");

	if(pfe)
		value = atoi(pfe);
	else
		value = 0;

	if( ejArgs(argc, argv, T("%d"), &type) == 1){
		if(type == value)
			websWrite(wp, T("selected"));
		else
			websWrite(wp, T(" "));
		return 0;
	}
	return -1;
}

static int getStaticDhcpRuleNumsASP(int eid, webs_t wp, int argc, char_t **argv)
{
    char *rules = (char *)nvram_bufget(RT2860_NVRAM, "staticDhcpRules");
	if(!rules || !strlen(rules) ){
		websWrite(wp, T("0"));
		return 0;
	}
	websWrite(wp, T("%d"), getRuleNums2(rules));
	return 0;
}

static void staticDhcpBasicSettings(webs_t wp, char_t *path, char_t *query)
{
	char_t  *submitUrl = websGetVar(wp, T("submit-url"), T(""));

	char *staticdhcp_enable = websGetVar(wp, T("staticDhcpEnabled"), T(""));//add by chenfei

	if(!staticdhcp_enable && !strlen(staticdhcp_enable))
		return;

	if(atoi(staticdhcp_enable))	// user choose nothing but press "apply" only
		nvram_set(RT2860_NVRAM, "staticDhcpEnabled", "1");
	else
		nvram_set(RT2860_NVRAM, "staticDhcpEnabled", "0");

	nvram_commit(RT2860_NVRAM);
	
//#if defined(CONFIG_RALINK_MT7620)
	//doSystem("lktos_networkconfig mtk7620 initlan");
//#else
	//doSystem("lktos_networkconfig ra5350 initlan");
	//initInternet();
//#endif

    websRedirect(wp, submitUrl);
}


static void staticDhcp(webs_t wp, char_t *path, char_t *query)
{
	char_t  *submitUrl = websGetVar(wp, T("submit-url"), T(""));

	char rule[8192];
	char *ip_address, *mac_address;
	char *staticDhcpRules;

	mac_address = websGetVar(wp, T("mac_address"), T(""));
	ip_address = websGetVar(wp, T("ip_address"), T(""));

	// we dont trust user input.....
	if(strlen(mac_address)){
		if(!isMacValid(mac_address))
			return;
	}

	if(!ip_address && !strlen(ip_address))
		return;
	if(!isIpValid(ip_address))
		return;
	
	if(( staticDhcpRules = (char *)nvram_bufget(RT2860_NVRAM, "staticDhcpRules")) && strlen( staticDhcpRules) )
		snprintf(rule, sizeof(rule), "%s;%s,%s",  staticDhcpRules, mac_address, ip_address);
	else
		snprintf(rule, sizeof(rule), "%s,%s", mac_address, ip_address);

	nvram_set(RT2860_NVRAM, "staticDhcpRules", rule);
	nvram_commit(RT2860_NVRAM);

//#if defined(CONFIG_RALINK_MT7620)
	//doSystem("lktos_networkconfig mtk7620 initlan");
//#else
	//doSystem("lktos_networkconfig ra5350 initlan");
	//initInternet();
//#endif
	websRedirect(wp, submitUrl);       
}

static void staticDhcpDelete(webs_t wp, char_t *path, char_t *query)
{
	char_t  *submitUrl = websGetVar(wp, T("submit-url"), T(""));

	int i, j, rule_count;
	char_t name_buf[16];
	char_t *value;
	int *deleArray;

	char *new_rules;
    const char *rules = (char *)nvram_bufget(RT2860_NVRAM, "staticDhcpRules");
    if(!rules || !strlen(rules) )
        return;

	rule_count = getRuleNums2((char *)rules);
	
	if(!rule_count)
		return;

	deleArray = (int *)malloc(rule_count * sizeof(int));
	if(!deleArray)
		return;

	new_rules = strdup(rules);
	if(!new_rules){
		free(deleArray);
		return;
	}

	for(i=0, j=0; i< rule_count; i++){
		snprintf(name_buf, 16, "delRule%d", i);
		value = websGetVar(wp, T(name_buf), NULL);
		if(value){
			deleArray[j++] = i;
		}
	}

    if(!j){
		free(deleArray);
		free(new_rules);
        websHeader(wp);
        websWrite(wp, T("You didn't select any rules to delete.<br>\n"));
        websFooter(wp);
        websDone(wp, 200);
        return;
    }

	deleteNthValueMulti(deleArray, rule_count, new_rules, ';');
	free(deleArray);

	nvram_set(RT2860_NVRAM, "staticDhcpRules", new_rules);
	nvram_commit(RT2860_NVRAM);
	free(new_rules);

//#if defined(CONFIG_RALINK_MT7620)
	//doSystem("lktos_networkconfig mtk7620 initlan");
//#else
	//doSystem("lktos_networkconfig ra5350 initlan");
	//initInternet();
//#endif
    websRedirect(wp, submitUrl);
	return;
}

/*
 * ASP function
 */
static int showStaticDhcpRulesASP(int eid, webs_t wp, int argc, char_t **argv)
{
	int i=0;
	char ip_address[16], mac_address[18];
	char rec[128];
	char *rules = (char *)nvram_bufget(RT2860_NVRAM, "staticDhcpRules");
	if(!rules)
		return 0;
	if(!strlen(rules))
		return 0;

	/* format is :
	 * [ip],[mac];
	 */
	while(getNthValueSafe(i++, rules, ';', rec, sizeof(rec)) != -1 ){
		//printf("i=%d : \n",i);
		// get mac address
		if((getNthValueSafe(0, rec, ',', mac_address, sizeof(mac_address)) == -1)){
			//printf("prf fail!!\n");
			continue;
		}

		// get ip address
		if((getNthValueSafe(1, rec, ',', ip_address, sizeof(ip_address)) == -1)){
			//printf("ip fail!!\n");
			continue;
		}
		if(!isIpValid(ip_address)){
			continue;
		}		
		
		websWrite(wp, T("<tr>\n"));
		// output No.
		websWrite(wp, T("<td> %d&nbsp; <input type=\"checkbox\" name=\"delRule%d\"> </td>"), i, i-1 );

		// output MAC address
		websWrite(wp, T("<td align=center> %s </td>"), mac_address);

		// output IP address
		websWrite(wp, T("<td align=center> %s </td>"), ip_address);

		websWrite(wp, T("</tr>\n"));
	}	  
	return 0;	
}

