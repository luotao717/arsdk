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

#ifdef USER_MANAGEMENT_SUPPORT
#include "um.h"
#endif
#include "nvram.h"
#include "utils.h"
#include "webs.h"
#include "internet.h"
#include "wireless.h"

#include "management.h"
#include "wps.h"

void WPSRestart(void);
void formDefineWPS(void);
#if defined (RTDEV_SUPPORT)
#ifndef CONFIG_UNIQUE_WPS
void RaixWPSRestart();
#endif
#endif

static int getWizardBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int getPPPOECDBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int getWatchDogBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int getSnortBuilt(int eid, webs_t wp, int argc, char_t **argv);

#define COMMAND_MAX	1024
static char system_command[COMMAND_MAX];

#ifdef CONFIG_USER_WIZARD
#define LFW(x, y)	do { 	x = websGetVar(wp, T(#y), T("")); } while(0)
#define DHCPCONF(x, y, z)	do { 	strcpy(x, y);		\
					char *p = strrchr(x, '.');	\
					strcpy(p+1, #z);		\
				} while(0)
static void setWizard(webs_t wp, char_t *path, char_t *query)
{
	char_t *connectionType, *security_mode;
	char_t *staticIp, *staticNetmask, *staticGateway, *staticPriDns, *staticSecDns;
	char_t *pppoeUser, *pppoePass, *pppoeOPMode, *pppoeRedialPeriod;
	char_t *wlan_disabled, *ssid, *wireless_mode, *security_key;
	char lan_ipaddr[16];
	
	strcpy(lan_ipaddr, nvram_bufget(RT2860_NVRAM, "lan_ipaddr"));

	LFW(connectionType, connectionType);
	LFW(staticIp, staticIp);
	LFW(staticNetmask, staticNetmask);
	LFW(staticGateway, staticGateway);
	LFW(staticPriDns, staticPriDns);
	LFW(staticSecDns, staticSecDns);
	LFW(pppoeUser, pppoeUser);
	LFW(pppoePass, pppoePass);
#ifndef CONFIG_USER_PPPOECD
	LFW(pppoeOPMode, pppoeOPMode);
	LFW(pppoeRedialPeriod, pppoeRedialPeriod);
#endif
	LFW(wlan_disabled, wlan_disabled);
	LFW(ssid, ssid);
	LFW(wireless_mode, wireless_mode);
	LFW(security_mode, security_mode);
	LFW(security_key, security_key);

	nvram_bufset(RT2860_NVRAM, "wanConnectionMode", connectionType);
	if (!strncmp(connectionType, "STATIC", 7)) 
	{
		/*
		 * lan and wan ip should not be the same
		 */
		if (!strncmp(staticIp, lan_ipaddr, 15)) 
		{
			websError(wp, 200, "IP address is identical to LAN");
			return;
		}
		nvram_bufset(RT2860_NVRAM, "wan_ipaddr", staticIp);
		nvram_bufset(RT2860_NVRAM, "wan_netmask", staticNetmask);
		nvram_bufset(RT2860_NVRAM, "wan_gateway", staticGateway);
		nvram_bufset(RT2860_NVRAM, "wan_primary_dns", staticPriDns);
		nvram_bufset(RT2860_NVRAM, "wan_secondary_dns", staticSecDns);
	}
	else if (!strncmp(connectionType, "PPPOE", 6)) 
	{
		nvram_bufset(RT2860_NVRAM, "wan_pppoe_user", pppoeUser);
		nvram_bufset(RT2860_NVRAM, "wan_pppoe_user_mm", pppoeUser);
		nvram_bufset(RT2860_NVRAM, "wan_pppoe_pass", pppoePass);
		nvram_bufset(RT2860_NVRAM, "wan_pppoe_opmode", pppoeOPMode);//Keep Alive
		nvram_bufset(RT2860_NVRAM, "wan_pppoe_optime", pppoeRedialPeriod);//Redial Period time
	}

	if (!strcmp(wlan_disabled, "ON")) 
	{
		//doSystem("ifconfig ra0 down");
		nvram_set(RT2860_NVRAM, "WiFiOff", "1");
	}
	else
	{
		//doSystem("ifconfig ra0 up");
		nvram_set(RT2860_NVRAM, "WiFiOff", "0");
	}	

	if (0 == strlen(ssid)) 
	{
		websError(wp, 403, T("'SSID' should not be empty!"));
		return;
	}
	nvram_bufset(RT2860_NVRAM, "SSID1", ssid);
	nvram_bufset(RT2860_NVRAM, "WirelessMode", wireless_mode);
	nvram_bufset(RT2860_NVRAM, "AuthMode", security_mode);
	if (!strcmp(security_mode, "OPEN"))
	{ 
		nvram_bufset(RT2860_NVRAM, "EncrypType", "WEP");
		nvram_bufset(RT2860_NVRAM, "DefaultKeyID", "1");
		if (10 == strlen(security_key) || 26 == strlen(security_key))
			nvram_bufset(RT2860_NVRAM, "Key1Type", "0"); 	// HEX
		else if (5 == strlen(security_key) || 13 == strlen(security_key))
			nvram_bufset(RT2860_NVRAM, "Key1Type", "1");	// ASCII
		nvram_bufset(RT2860_NVRAM, "Key1Str1", security_key);
	}
	else if (!strcmp(security_mode, "WPAPSK"))
	{
		nvram_bufset(RT2860_NVRAM, "EncrypType", "AES");
		nvram_bufset(RT2860_NVRAM, "DefaultKeyID", "1"); //old value=2, modify by chenfei
		nvram_bufset(RT2860_NVRAM, "RekeyInterval", "3600");
		nvram_bufset(RT2860_NVRAM, "RekeyMethod", "TIME");
		nvram_bufset(RT2860_NVRAM, "IEEE8021X", "0");
		nvram_bufset(RT2860_NVRAM, "WPAPSK1", security_key);
	}
	else if (!strcmp(security_mode, "WPA2PSK"))
	{
		nvram_bufset(RT2860_NVRAM, "EncrypType", "AES");
		nvram_bufset(RT2860_NVRAM, "DefaultKeyID", "1"); //old value=2, modify by chenfei
		nvram_bufset(RT2860_NVRAM, "RekeyInterval", "3600");
		nvram_bufset(RT2860_NVRAM, "RekeyMethod", "TIME");
		nvram_bufset(RT2860_NVRAM, "IEEE8021X", "0");
		nvram_bufset(RT2860_NVRAM, "WPAPSK1", security_key);
	}
	else if (!strcmp(security_mode, "WPAPSKWPA2PSK"))
	{
		nvram_bufset(RT2860_NVRAM, "EncrypType", "AES");
		nvram_bufset(RT2860_NVRAM, "DefaultKeyID", "1"); //old value=2, modify by chenfei
		nvram_bufset(RT2860_NVRAM, "RekeyInterval", "3600");
		nvram_bufset(RT2860_NVRAM, "RekeyMethod", "TIME");
		nvram_bufset(RT2860_NVRAM, "IEEE8021X", "0");
		nvram_bufset(RT2860_NVRAM, "WPAPSK1", security_key);
	}
	else
	{
		nvram_bufset(RT2860_NVRAM, "EncrypType", "NONE");
	}

	nvram_commit(RT2860_NVRAM);      

	const char	*langType = nvram_bufget(RT2860_NVRAM, "LanguageType");

	a_assert(websValid(wp));

	websWrite(wp, T("HTTP/1.0 200 OK\n"));

/*
 *	By license terms the following line of code must not be modified
 */
	websWrite(wp, T("Server: %s\r\n"), WEBS_NAME);

	websWrite(wp, T("Pragma: no-cache\n"));
	websWrite(wp, T("Cache-control: no-cache\n"));
	websWrite(wp, T("Content-Type: text/html\n"));
	websWrite(wp, T("\n"));
	websWrite(wp, T("<html>\n<head>\n"));
	websWrite(wp, T("<title>My Title</title>\n"));
	websWrite(wp, T("<link rel=\"stylesheet\" href=\"/style/normal_ws.css\" type=\"text/css\">\n"));
	websWrite(wp, T("<meta http-equiv=\"content-type\" content=\"text/html;charset=utf-8\">\n"));
	websWrite(wp, T("<script language=\"javascript\" src=\"../js/language_%s.js\"></script>\n"),langType);

	websWrite(wp, T("<script language=\"javascript\">\n\
		function refresh_all(){	\n\
		  top.location.href = \"http://%s/home.asp\"; \n\
		} \n\
		function update(){ \n\
		  self.setTimeout(\"refresh_all()\", 40000);\n\
		}\n</script>\n"), lan_ipaddr);	
	websWrite(wp, T("</head>\n<body onload=\"update()\">\n"));
	websWrite(wp, T("<blockquote>\n"));
	websWrite(wp, T("<br><b><script>dw(MM_set_wizard_ok)</script></b>\n"));
	websWrite(wp, T("</blockquote>\n"));
	websWrite(wp, T("</body>\n</html>\n"));

	sleep(3);
	doSystem("reboot");
}
#endif

#if (defined CONFIG_RALINK_WATCHDOG || defined CONFIG_RALINK_WATCHDOG_MODULE) && defined CONFIG_USER_WATCHDOG
static void WatchDogRestart(void)
{
	doSystem("killall -9 watchdog 1>/dev/null 2>&1");
	doSystem("rmmod ralink_wdt.o");
	if (strcmp(nvram_get(RT2860_NVRAM, "WatchDogEnable"), "1") == 0) {
		doSystem("insmod -q ralink_wdt.o");
		doSystem("wdg.sh");
		doSystem("watchdog");
	}
}
#endif

/*
 * goform/setSysAdm
 */
static void setSysAdm(webs_t wp, char_t *path, char_t *query)
{
	char_t  *submitUrl = websGetVar(wp, T("submit-url"), T(""));

	char_t *admuser, *admpass;
	char *old_user;

	old_user = (char *) nvram_bufget(RT2860_NVRAM, "Login");
	admuser = websGetVar(wp, T("admuser"), T(""));
	admpass = websGetVar(wp, T("admpass"), T(""));

	if (!strlen(admuser)) {
		error(E_L, E_LOG, T("setSysAdm: account empty, leave it unchanged"));
		return;
	}
	if (!strlen(admpass)) {
		error(E_L, E_LOG, T("setSysAdm: password empty, leave it unchanged"));
		return;
	}
#if (defined CONFIG_RALINK_WATCHDOG || defined CONFIG_RALINK_WATCHDOG_MODULE) && defined CONFIG_USER_WATCHDOG
	char_t *watchdogcap;
	watchdogcap = websGetVar(wp, T("admwatchdog"), T(""));
	nvram_bufset(RT2860_NVRAM, "WatchDogEnable", watchdogcap);
#endif

	/* modify /etc/passwd to new user name and passwd */
	doSystem("sed -e 's/^%s:/%s:/' /etc/passwd > /etc/newpw", old_user, admuser);
	doSystem("cp /etc/newpw /etc/passwd");
	doSystem("rm -f /etc/newpw");
	doSystem("chpasswd.sh %s %s", admuser, admpass);

#ifdef USER_MANAGEMENT_SUPPORT
	if (umGroupExists(T("adm")) == FALSE)
		umAddGroup(T("adm"), 0x07, AM_DIGEST, FALSE, FALSE);
	if (old_user != NULL && umUserExists(old_user))
		umDeleteUser(old_user);
	if (umUserExists(admuser))
		umDeleteUser(admuser);
	umAddUser(admuser, admpass, T("adm"), FALSE, FALSE);
#endif
	nvram_bufset(RT2860_NVRAM, "Login", admuser);
	nvram_bufset(RT2860_NVRAM, "Password", admpass);
	nvram_commit(RT2860_NVRAM);
	doSystem("storage.sh samba");
#if (defined CONFIG_RALINK_WATCHDOG || defined CONFIG_RALINK_WATCHDOG_MODULE) && defined CONFIG_USER_WATCHDOG
	WatchDogRestart();
#endif

	websRedirect(wp, submitUrl);     
}

/*
 * goform/setSysLang
 */
static void setSysLang(webs_t wp, char_t *path, char_t *query)
{
	char_t *lang;

	lang = websGetVar(wp, T("langSelection"), T(""));
	nvram_bufset(RT2860_NVRAM, "Language", lang);
	nvram_commit(RT2860_NVRAM);

	websHeader(wp);
	websWrite(wp, T("<h2>Language Selection</h2><br>\n"));
	websWrite(wp, T("language: %s<br>\n"), lang);
	websFooter(wp);
	websDone(wp, 200);        
}

/*
 * goform/NTP
 */
static void NTP(webs_t wp, char_t *path, char_t *query)
{
	char_t  *submitUrl = websGetVar(wp, T("submit-url"), T(""));

	char *tz, *ntpServer, *ntpSync;

	tz = websGetVar(wp, T("time_zone"), T(""));
	ntpServer = websGetVar(wp, T("NTPServerIP"), T(""));
	ntpSync = websGetVar(wp, T("NTPSync"), T(""));

	if(!tz || !ntpServer || !ntpSync)
		return;

	if(!strlen(tz))
		return;

	if(checkSemicolon(tz))
		return;

	if(!strlen(ntpServer)){
		// user choose to make  NTP server disable
		nvram_bufset(RT2860_NVRAM, "NTPServerIP", "");
		nvram_bufset(RT2860_NVRAM, "NTPSync", "");
	}else{
		if(checkSemicolon(ntpServer))
			return;
		if(!strlen(ntpSync))
			return;
		if(atoi(ntpSync) > 300)
			return;
		nvram_bufset(RT2860_NVRAM, "NTPServerIP", ntpServer);
		nvram_bufset(RT2860_NVRAM, "NTPSync", ntpSync);
	}
	nvram_bufset(RT2860_NVRAM, "TZ", tz);
	nvram_commit(RT2860_NVRAM);

	doSystem("ntp.sh");

	websRedirect(wp, submitUrl);     
}

#ifdef CONFIG_DATE
/*
 * goform/NTPSyncWithHost
 */
static void NTPSyncWithHost(webs_t wp, char_t *path, char_t *query)
{
	if(!query || (!strlen(query)))
		return;
	if(strchr(query, ';'))
		return;

	doSystem("date -s %s", query);


	websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nCache-Control: no-cache\n\n"));
	websWrite(wp, T("n/a"));
	websDone(wp, 200);
}
#endif

#ifdef CONFIG_USER_GOAHEAD_GreenAP
/*
 * goform/GreenAP
 */
static void GreenAP(webs_t wp, char_t *path, char_t *query)
{
	char_t *shour1, *sminute1, *ehour1, *eminute1, *action1;
	char_t *shour2, *sminute2, *ehour2, *eminute2, *action2;
	char_t *shour3, *sminute3, *ehour3, *eminute3, *action3;
	char_t *shour4, *sminute4, *ehour4, *eminute4, *action4;
	char start[6], end[6];
	shour1 = websGetVar(wp, T("GAPSHour1"), T(""));
	sminute1 = websGetVar(wp, T("GAPSMinute1"), T(""));
	ehour1 = websGetVar(wp, T("GAPEHour1"), T(""));
	eminute1 = websGetVar(wp, T("GAPEMinute1"), T(""));
	action1 = websGetVar(wp, T("GAPAction1"), T(""));
	sprintf(start, "%s %s", sminute1, shour1);
	sprintf(end, "%s %s", eminute1, ehour1);
	nvram_bufset(RT2860_NVRAM, "GreenAPStart1", start);
	nvram_bufset(RT2860_NVRAM, "GreenAPEnd1", end);
	nvram_bufset(RT2860_NVRAM, "GreenAPAction1", action1);
	shour2 = websGetVar(wp, T("GAPSHour2"), T(""));
	sminute2 = websGetVar(wp, T("GAPSMinute2"), T(""));
	ehour2 = websGetVar(wp, T("GAPEHour2"), T(""));
	eminute2 = websGetVar(wp, T("GAPEMinute2"), T(""));
	action2 = websGetVar(wp, T("GAPAction2"), T(""));
	sprintf(start, "%s %s", sminute2, shour2);
	sprintf(end, "%s %s", eminute2, ehour2);
	nvram_bufset(RT2860_NVRAM, "GreenAPStart2", start);
	nvram_bufset(RT2860_NVRAM, "GreenAPEnd2", end);
	nvram_bufset(RT2860_NVRAM, "GreenAPAction2", action2);
	shour3 = websGetVar(wp, T("GAPSHour3"), T(""));
	sminute3 = websGetVar(wp, T("GAPSMinute3"), T(""));
	ehour3 = websGetVar(wp, T("GAPEHour3"), T(""));
	eminute3 = websGetVar(wp, T("GAPEMinute3"), T(""));
	action3 = websGetVar(wp, T("GAPAction3"), T(""));
	sprintf(start, "%s %s", sminute3, shour3);
	sprintf(end, "%s %s", eminute3, ehour3);
	nvram_bufset(RT2860_NVRAM, "GreenAPStart3", start);
	nvram_bufset(RT2860_NVRAM, "GreenAPEnd3", end);
	nvram_bufset(RT2860_NVRAM, "GreenAPAction3", action3);
	shour4 = websGetVar(wp, T("GAPSHour4"), T(""));
	sminute4 = websGetVar(wp, T("GAPSMinute4"), T(""));
	ehour4 = websGetVar(wp, T("GAPEHour4"), T(""));
	eminute4 = websGetVar(wp, T("GAPEMinute4"), T(""));
	action4 = websGetVar(wp, T("GAPAction4"), T(""));
	sprintf(start, "%s %s", sminute4, shour4);
	sprintf(end, "%s %s", eminute4, ehour4);
	nvram_bufset(RT2860_NVRAM, "GreenAPStart4", start);
	nvram_bufset(RT2860_NVRAM, "GreenAPEnd4", end);
	nvram_bufset(RT2860_NVRAM, "GreenAPAction4", action4);
	nvram_commit(RT2860_NVRAM);

	doSystem("greenap.sh init");

	websHeader(wp);
	websWrite(wp, T("GreenAPStart1: %s %s<br>\n"), sminute1, shour1);
	websWrite(wp, T("GreenAPEnd1: %s %s<br>\n"), eminute1, ehour1);
	websWrite(wp, T("GreenAPAction1: %s<br>\n"), action1);
	websWrite(wp, T("GreenAPStart2: %s %s<br>\n"), sminute2, shour2);
	websWrite(wp, T("GreenAPEnd2: %s %s<br>\n"), eminute2, ehour2);
	websWrite(wp, T("GreenAPAction2: %s<br>\n"), action2);
	websWrite(wp, T("GreenAPStart3: %s %s<br>\n"), sminute3, shour3);
	websWrite(wp, T("GreenAPEnd3: %s %s<br>\n"), eminute3, ehour3);
	websWrite(wp, T("GreenAPAction3: %s<br>\n"), action3);
	websWrite(wp, T("GreenAPStart4: %s %s<br>\n"), sminute4, shour4);
	websWrite(wp, T("GreenAPEnd4: %s %s<br>\n"), eminute4, ehour4);
	websWrite(wp, T("GreenAPAction4: %s<br>\n"), action4);
	websFooter(wp);
	websDone(wp, 200);        
}
#endif

#if defined CONFIG_USER_DDNS
/*
 * description: write ddns type in <select> tag
 */
static int getDdnsCombox(int eid, webs_t wp, int argc, char_t **argv)
{
	websWrite(wp, T("<option value=\"dyndns.org\"> www.dyndns.org </option>\n"));
	websWrite(wp, T("<option value=\"no-ip.com\"> www.no-ip.com </option>\n"));
#if defined (CONFIG_USER_LANG_ALL) || defined (CONFIG_USER_LANG_CN)
	websWrite(wp, T("<option value=\"3322.org\"> www.3322.org </option>\n"));
#endif	
	return 0;
}

/*
 * goform/DDNS
 */
static void DDNS(webs_t wp, char_t *path, char_t *query)
{
	char_t  *submitUrl = websGetVar(wp, T("submit-url"), T(""));

	char *ddns_enable, *ddns_provider;
	char *ddns_domain, *ddns_acc, *ddns_pass;
	int enabled = 0;

	ddns_enable = websGetVar(wp, T("DDNSEnabled"), T(""));
	ddns_provider = websGetVar(wp, T("DDNSProvider"), T("none"));
	ddns_domain = websGetVar(wp, T("DDNS"), T(""));
	ddns_acc = websGetVar(wp, T("Account"), T(""));
	ddns_pass = websGetVar(wp, T("Password"), T(""));

	if(!strncmp(ddns_enable, "1", 2)) {	//enable ddns
	  	enabled=1;
        nvram_bufset(RT2860_NVRAM, "DDNSEnabled", "1");
    }
	else {
	  	nvram_bufset(RT2860_NVRAM, "DDNSEnabled", "0");
	  	nvram_commit(RT2860_NVRAM);
	}

	if(enabled) {
			if(!ddns_domain || !ddns_acc || !ddns_pass)
				return;

			if(!strlen(ddns_domain) || !strlen(ddns_acc) || !strlen(ddns_pass))
				return;

			if(checkSemicolon(ddns_domain) || checkSemicolon(ddns_acc) || checkSemicolon(ddns_pass))
				return;


		nvram_bufset(RT2860_NVRAM, "DDNSProvider", ddns_provider);
		nvram_bufset(RT2860_NVRAM, "DDNS", ddns_domain);
		nvram_bufset(RT2860_NVRAM, "DDNSAccount", ddns_acc);
		nvram_bufset(RT2860_NVRAM, "DDNSPassword", ddns_pass);
		nvram_commit(RT2860_NVRAM);

		doSystem("ddns.sh &");
	}
	else {
	  	doSystem("killall -q yddns");
	}

	initInternet();
	
	websRedirect(wp, submitUrl);          
}
#endif

#ifdef CONFIG_USER_SNORT
/*
 * goform/Snort
 */
static void Snort(webs_t wp, char_t *path, char_t *query)
{
	char *snort_enable_str;
	int snort_enable;

	snort_enable_str = websGetVar(wp, T("SnortSelect"), T("0"));
	if(!snort_enable_str)
		return;

	snort_enable = atoi(snort_enable_str);
	if(snort_enable == 1)
		nvram_bufset(RT2860_NVRAM, "SnortEnable", "1");
	else
		nvram_bufset(RT2860_NVRAM, "SnortEnable", "0");
	nvram_commit(RT2860_NVRAM);

	doSystem("snort.sh");

	websHeader(wp);
	websWrite(wp, T("<h2>Snort Settings</h2><br>\n"));
	websWrite(wp, T("Snort: %s<br>\n"), snort_enable ? "Enable" : "Disable");
	websFooter(wp);
	websDone(wp, 200);        
}
#endif

static void SystemCommand(webs_t wp, char_t *path, char_t *query)
{
	char_t  *submitUrl = websGetVar(wp, T("submit-url"), T(""));

	char *command;

	command = websGetVar(wp, T("command"), T(""));

	if(!command)
		return;

	if(!strlen(command))
		snprintf(system_command, COMMAND_MAX, "cat /dev/null > %s", SYSTEM_COMMAND_LOG);
	else
		snprintf(system_command, COMMAND_MAX, "%s 1>%s 2>&1", command, SYSTEM_COMMAND_LOG);
	
	if(strlen(system_command))
		doSystem(system_command);

	websRedirect(wp, submitUrl);     

	return;
}

static void repeatLastSystemCommand(webs_t wp, char_t *path, char_t *query)
{
	char_t  *submitUrl = websGetVar(wp, T("submit-url"), T(""));

	if(strlen(system_command))
		doSystem(system_command);

	websRedirect(wp, submitUrl);     

	return;
}


int showSystemCommandASP(int eid, webs_t wp, int argc, char_t **argv)
{
	FILE *fp;
	char buf[1024];
	
	fp = fopen(SYSTEM_COMMAND_LOG, "r");
	if(!fp){
		websWrite(wp, T(""));
		return 0;
	}

	while(fgets(buf, 1024, fp)){
		websWrite(wp, T("%s"), buf);
	}
	fclose(fp);
	
	return 0;
}

static inline char *strip_space(char *str)
{
	while( *str == ' ')
		str++;
	return str;
}


char* getField(char *a_line, char *delim, int count)
{
	int i=0;
	char *tok;
	tok = strtok(a_line, delim);
	while(tok){
		if(i == count)
			break;
        i++;
		tok = strtok(NULL, delim);
    }
    if(tok && isdigit(*tok))
		return tok;

	return NULL;
}

/*
 *   C version. (ASP version is below)
 */
static long long getIfStatistic(char *interface, int type)
{
	int found_flag = 0;
	int skip_line = 2;
	char buf[1024], *field, *semiColon = NULL;
	FILE *fp = fopen(PROC_IF_STATISTIC, "r");
	if(!fp){
		printf("no proc?\n");
		return -1;
	}

	while(fgets(buf, 1024, fp)){
		char *ifname;
		if(skip_line != 0){
			skip_line--;
			continue;
		}
		if(! (semiColon = strchr(buf, ':'))  )
			continue;
		*semiColon = '\0';
		ifname = buf;
		ifname = strip_space(ifname);

		if(!strcmp(ifname, interface)){
			found_flag = 1;
			break;
		}
	}
	fclose(fp);

	semiColon++;

	switch(type){
	case TXBYTE:
		if(  (field = getField(semiColon, " ", 8))  ){
			return strtoll(field, NULL, 10);
		}
		break;
	case TXPACKET:
		if(  (field = getField(semiColon, " ", 9))  ){
			return strtoll(field, NULL, 10);
		}
		break;
	case RXBYTE:
		if(  (field = getField(semiColon, " ", 0))  ){
			return strtoll(field, NULL, 10);
		}
		break;
	case RXPACKET:
		if(  (field = getField(semiColon, " ", 1))  ){
			return strtoll(field, NULL, 10);
		}
		break;
	}
	return -1;
}

/*
 *     getIfStatistic()   ASP version
 */
int getIfStatisticASP(int eid, webs_t wp, int argc, char_t **argv)
{
	int found_flag = 0;
	int skip_line = 2;
	char *interface, *type, *field, *semiColon = NULL;
	char buf[1024], result[32];
	FILE *fp = fopen(PROC_IF_STATISTIC, "r");
	if(!fp){
		websWrite(wp, T("no proc?\n"));
		return -1;
	}

    if(ejArgs(argc, argv, T("%s %s"), &interface, &type) != 2){
	    fclose(fp);
	    websWrite(wp, T("Wrong argument.\n"));
	    return -1;
    }

	while(fgets(buf, 1024, fp)){
		char *ifname;
		if(skip_line != 0){
			skip_line--;
			continue;
		}
		if(! (semiColon = strchr(buf, ':'))  )
			continue;
		*semiColon = '\0';
		ifname = buf;
		ifname = strip_space(ifname);

		if(!strcmp(ifname, interface)){
			found_flag = 1;
			break;
		}
	}
	fclose(fp);

	semiColon++;

	if(!strcmp(type, T("TXBYTE")  )){
		if(  (field = getField(semiColon, " ", 8))  ){
			snprintf(result, 32,"%lld",   strtoll(field, NULL, 10));
			ejSetResult(eid, result);
		}
	}else if(!strcmp(type, T("TXPACKET")  )){
		if(  (field = getField(semiColon, " ", 9))  ){
			snprintf(result, 32,"%lld",   strtoll(field, NULL, 10));
			ejSetResult(eid, result);
		}
    }else if(!strcmp(type, T("RXBYTE")  )){
		if(  (field = getField(semiColon, " ", 0))  ){
			snprintf(result, 32,"%lld",   strtoll(field, NULL, 10));
			ejSetResult(eid, result);
		}
    }else if(!strcmp(type, T("RXPACKET")  )){
		if(  (field = getField(semiColon, " ", 1))  ){
			snprintf(result, 32,"%lld",   strtoll(field, NULL, 10));
			ejSetResult(eid, result);
		}
    }else{
		websWrite(wp, T("unknown type.") );
		return -1;
	}
	return -1;
}

int getWANRxByteASP(int eid, webs_t wp, int argc, char_t **argv)
{
	char_t buf[32];
	long long data = getIfStatistic( getWanIfName(), RXBYTE);
	snprintf(buf, 32, "%lld", data);	
	websWrite(wp, T("%s"), buf);
	return 0;
}

int getWANRxPacketASP(int eid, webs_t wp, int argc, char_t **argv)
{
	char_t buf[32];
	long long data = getIfStatistic( getWanIfName(), RXPACKET);
	snprintf(buf, 32, "%lld", data);	
	websWrite(wp, T("%s"), buf);
	return 0;
}

int getWANTxByteASP(int eid, webs_t wp, int argc, char_t **argv)
{
	char_t buf[32];
	long long data = getIfStatistic( getWanIfName(), TXBYTE);
	snprintf(buf, 32, "%lld", data);	
	websWrite(wp, T("%s"), buf);
	return 0;
}

int getWANTxPacketASP(int eid, webs_t wp, int argc, char_t **argv)
{
	char_t buf[32];
	long long data = getIfStatistic( getWanIfName(), TXPACKET);
	snprintf(buf, 32, "%lld", data);	
	websWrite(wp, T("%s"), buf);
	return 0;
}

int getLANRxByteASP(int eid, webs_t wp, int argc, char_t **argv)
{
	char_t buf[32];
	long long data = getIfStatistic( getLanIfName(), RXBYTE);
	snprintf(buf, 32, "%lld", data);	
	websWrite(wp, T("%s"), buf);
	return 0;
}

int getLANRxPacketASP(int eid, webs_t wp, int argc, char_t **argv)
{
	char_t buf[32];
	long long data = getIfStatistic( getLanIfName(), RXPACKET);
	snprintf(buf, 32, "%lld", data);	
	websWrite(wp, T("%s"), buf);
	return 0;
}

int getLANTxByteASP(int eid, webs_t wp, int argc, char_t **argv)
{
	char_t buf[32];
	long long data = getIfStatistic( getLanIfName(), TXBYTE);
	snprintf(buf, 32, "%lld", data);	
	websWrite(wp, T("%s"), buf);
	return 0;
}

int getLANTxPacketASP(int eid, webs_t wp, int argc, char_t **argv)
{
	char_t buf[32];
	long long data = getIfStatistic( getLanIfName(), TXPACKET);
	snprintf(buf, 32, "%lld", data);	
	websWrite(wp, T("%s"),buf);
	return 0;
}

/*
 * This ASP function is for javascript usage, ex:
 *
 * <script type="text/javascript">
 *   var a = new Array();
 *   a = [<% getAllNICStatisticASP(); %>];         //ex: a = ["lo","10","1000", "20", "2000","eth2"];
 *   document.write(a)
 * </script>
 *
 * Javascript could get info with  getAllNICStatisticASP().
 *
 * We dont produce table-related tag in this ASP function .It's
 * more extensive since ASP just handle data and Javascript present them,
 * although the data form is only for Javascript now.
 *
 * TODO: a lot, there are many ASP functions binding with table-relted tag...
 */
int getAllNICStatisticASP(int eid, webs_t wp, int argc, char_t **argv)
{
	char result[1024];
	char buf[1024];
	int rc = 0, pos = 0, skip_line = 2;
	int first_time_flag = 1;
	FILE *fp = fopen(PROC_IF_STATISTIC, "r");
	if(!fp){
		printf("no proc?\n");
		return -1;
	}

	while(fgets(buf, 1024, fp)){
		char *ifname, *semiColon;
		long long rxpkt, rxbyte, txpkt, txbyte;
		if(skip_line != 0){
			skip_line--;
			continue;
		}
		if(! (semiColon = strchr(buf, ':'))  )
			continue;
		*semiColon = '\0';

		ifname = buf;
		ifname = strip_space(ifname);

		rxpkt = getIfStatistic(ifname, RXPACKET);
		rxbyte = getIfStatistic(ifname, RXBYTE);
		txpkt = getIfStatistic(ifname, TXPACKET);
		txbyte = getIfStatistic(ifname, TXBYTE);
		if (rxpkt <= 0 && rxbyte <= 0 && txpkt <= 0 && txbyte <= 0) 
			continue;
		/* try to get statistics data */
		if(getIfStatistic(ifname, RXPACKET) >= 0){
			/* a success try */
			if(first_time_flag){
				pos = snprintf(result+rc, 1024-rc, "\"%s\"", ifname);
				rc += pos;
				first_time_flag = 0;
			}else{
				pos = snprintf(result+rc, 1024-rc, ",\"%s\"", ifname);
				rc += pos;
			}

		}else	/* failed and just skip */
			continue;

		pos = snprintf(result+rc, 1024-rc, ",\"%lld\"", rxpkt);
		rc += pos;
		pos = snprintf(result+rc, 1024-rc, ",\"%lld\"", rxbyte);
		rc += pos;
		pos = snprintf(result+rc, 1024-rc, ",\"%lld\"", txpkt);
		rc += pos;
		pos = snprintf(result+rc, 1024-rc, ",\"%lld\"", txbyte);
		rc += pos;
	}
	fclose(fp);

	websWrite(wp, T("%s"), result);
    return 0;
}


int getMemTotalASP(int eid, webs_t wp, int argc, char_t **argv)
{
	char buf[1024], *semiColon, *key, *value;
	FILE *fp = fopen(PROC_MEM_STATISTIC, "r");
	if(!fp){
		websWrite(wp, T("no proc?\n"));
		return -1;
	}

	while(fgets(buf, 1024, fp)){
		if(! (semiColon = strchr(buf, ':'))  )
			continue;
		*semiColon = '\0';
		key = buf;
		value = semiColon + 1;
		if(!strcmp(key, "MemTotal")){
			value = strip_space(value);
			websWrite(wp, T("%s"), value);
			fclose(fp);
			return 0;
		}
	}
	websWrite(wp, T(""));
	fclose(fp);
	
	return -1;
}

int getCurrentTimeASP(int eid, webs_t wp, int argc, char_t **argv)
{
	char_t buf[64];
		
	FILE *fp = popen("date", "r");
	if(!fp){
		websWrite(wp, T("none"));
		return 0;
	}
	fgets(buf, 64, fp);
	pclose(fp);

	if(buf[strlen(buf)-1] == '\n')
	{
		buf[strlen(buf)-1] = '\0'; 
		websWrite(wp, "%s", buf);
	}
	else
		websWrite(wp, "%s", buf);
	return 0;
}

int getMemLeftASP(int eid, webs_t wp, int argc, char_t **argv)
{
	char buf[1024], *semiColon, *key, *value;
	FILE *fp = fopen(PROC_MEM_STATISTIC, "r");
	if(!fp){
		websWrite(wp, T("no proc?\n"));
		return -1;
	}

	while(fgets(buf, 1024, fp)){
		if(! (semiColon = strchr(buf, ':'))  )
			continue;
		*semiColon = '\0';
		key = buf;
		value = semiColon + 1;
		if(!strcmp(key, "MemFree")){
			value = strip_space(value);
			websWrite(wp, T("%s"), value);
			fclose(fp);
			return 0;
		}
	}
	websWrite(wp, T(""));
	fclose(fp);
	return -1;
}

static int FirmwareUpgradePostASP(int eid, webs_t wp, int argc, char_t **argv)
{
#if 0
	FILE *fp;
	char ver[128], week[32], mon[32] , date[32], time[32], *pos;
	
	char *expect = nvram_bufget(RT2860_NVRAM, "Expect_Firmware");
	if(!expect || !strlen(expect) )
		return 0;

	fp = fopen("/proc/version", "r");
	if(!fp)
		return 0;

	fgets(ver, 128, fp);
	fclose(fp);	

	if(!(pos = strchr(ver, '#')) )
		return 0;

	if(!(pos = strchr(pos+1, ' ')) )
		return 0;

	pos++;
	sscanf(pos, "%s %s %s %s", week, mon, date, time);
	sprintf(ver, "Linux Kernel Image %s%s%s", mon, date, time); 

	if(!strcmp(expect, ver)){
		websWrite(wp, T("alert(\"Firmware Upgrade Success.\");"));
		nvram_bufset(RT2860_NVRAM, "Expect_Firmware", "");
		nvram_commit(RT2860_NVRAM);
	}else{
		websWrite(wp, T("alert(\"Firmware Upgrade may be failed:\\nexpect new image : %s\\ncurrent : %s\");"), expect, ver);
	}	
	return 0;
#endif
	FILE *fp;
	char buf[512];
	const char *old_firmware = nvram_bufget(RT2860_NVRAM, "old_firmware");
	if(!old_firmware || !strlen(old_firmware) )
		return 0;
	fp = fopen("/proc/version", "r");
	if(!fp)
		return 0;

	fgets(buf, sizeof(buf), fp);
	fclose(fp);	
	if(!strcmp(buf, old_firmware)){
		websWrite(wp, T("alert(\"Warning!The firmware didn't change.\");"));
	}else{
		websWrite(wp, T("alert(\"Firmware Upgrade success\");"));
	}	
	nvram_bufset(RT2860_NVRAM, "old_firmware", "");
	nvram_commit(RT2860_NVRAM);

	return 0;
}

static void LoadDefaultSettings(webs_t wp, char_t *path, char_t *query)
{	
	const char	*langType = nvram_bufget(RT2860_NVRAM, "LanguageType");

	FILE *pp = popen("cat /proc/mounts | grep media", "r");
	char dir[13];
	while(EOF != fscanf(pp, "%*s %s %*s %*s %*s %*s\n", dir))
	{
		if (!memcmp(dir, "/media/sda", 10))
		{
			fprintf(stderr, "get the first partition: %s\n", dir);
			break;
		}

	}
	pclose(pp);
	doSystem("rm -rf %s/.smb_config",dir);

	system("ralink_init clear 2860");
#if defined CONFIG_LAN_WAN_SUPPORT || defined CONFIG_MAC_TO_MAC_MODE
    system("ralink_init renew 2860 /etc_ro/Wireless/RT2860AP/RT2860_default_vlan");
	system("rm -f /tmp/apcliStatus");//modify by chenfei, restore factory settings, rm -f /tmp/apcliStatus
#elif defined(CONFIG_ICPLUS_PHY)
    system("ralink_init renew 2860 /etc_ro/Wireless/RT2860AP/RT2860_default_oneport");
#else
    system("ralink_init renew 2860 /etc_ro/Wireless/RT2860AP/RT2860_default_novlan");
#endif
#if defined (RTDEV_SUPPORT)
	system("ralink_init clear rtdev");
    system("ralink_init renew rtdev /etc_ro/Wireless/iNIC/RT2860AP.dat");
#elif defined (CONFIG_RT2561_AP) || defined (CONFIG_RT2561_AP_MODULE)
	system("ralink_init clear rtdev");
    system("ralink_init renew rtdev /etc_ro/Wireless/RT61AP/RT2561_default");
#elif defined (CONFIG_RTDEV_PLC)
	system("ralink_init clear rtdev");
    system("ralink_init renew rtdev /etc_ro/PLC/plc_default.dat");
#endif

	const char	*lan_ip = nvram_bufget(RT2860_NVRAM, "lan_ipaddr");

	a_assert(websValid(wp));

	websWrite(wp, T("HTTP/1.0 200 OK\n"));

/*
 *	By license terms the following line of code must not be modified
 */
	websWrite(wp, T("Server: %s\r\n"), WEBS_NAME);

	websWrite(wp, T("Pragma: no-cache\n"));
	websWrite(wp, T("Cache-control: no-cache\n"));
	websWrite(wp, T("Content-Type: text/html\n"));
	websWrite(wp, T("\n"));
	websWrite(wp, T("<html>\n<head>\n"));
	websWrite(wp, T("<title>My Title</title>\n"));
	websWrite(wp, T("<link rel=\"stylesheet\" href=\"/style/normal_ws.css\" type=\"text/css\">\n"));
	websWrite(wp, T("<meta http-equiv=\"content-type\" content=\"text/html;charset=utf-8\">\n"));
	websWrite(wp, T("<script language=\"javascript\" src=\"../js/language_%s.js\"></script>\n"),langType);
	websWrite(wp, T("<script language=\"javascript\">\n\
		function refresh_all(){	\n\
		  top.location.href = \"http://%s/home.asp\"; \n\
		} \n\
		function update(){ \n\
		  self.setTimeout(\"refresh_all()\", 40000);\n\
		}\n</script>\n"), lan_ip);	
	websWrite(wp, T("</head>\n<body onload=\"update()\">\n"));
	websWrite(wp, T("<blockquote>\n"));

	websWrite(wp, T("<br><b><script>dw(MM_load_factory)</script></b>\n"));
	websWrite(wp, T("</blockquote>\n"));
	websWrite(wp, T("</body>\n</html>\n"));
	
	sleep(1);
	system("reboot");
}

static void RebootSystem(webs_t wp, char_t *path, char_t *query)
{
	const char	*lan_ip = nvram_bufget(RT2860_NVRAM, "lan_ipaddr");
	const char	*langType = nvram_bufget(RT2860_NVRAM, "LanguageType");

	a_assert(websValid(wp));

	websWrite(wp, T("HTTP/1.0 200 OK\n"));

/*
 *	By license terms the following line of code must not be modified
 */
	websWrite(wp, T("Server: %s\r\n"), WEBS_NAME);

	websWrite(wp, T("Pragma: no-cache\n"));
	websWrite(wp, T("Cache-control: no-cache\n"));
	websWrite(wp, T("Content-Type: text/html\n"));
	websWrite(wp, T("\n"));
	websWrite(wp, T("<html>\n<head>\n"));
	websWrite(wp, T("<title>My Title</title>\n"));
	websWrite(wp, T("<link rel=\"stylesheet\" href=\"/style/normal_ws.css\" type=\"text/css\">\n"));
	websWrite(wp, T("<meta http-equiv=\"content-type\" content=\"text/html;charset=utf-8\">\n"));
	websWrite(wp, T("<script language=\"javascript\" src=\"../js/language_%s.js\"></script>\n"),langType);
	websWrite(wp, T("<script language=\"javascript\">\n\
		function refresh_all(){	\n\
		  top.location.href = \"http://%s/home.asp\"; \n\
		} \n\
		function update(){ \n\
		  self.setTimeout(\"refresh_all()\", 40000);\n\
		}\n</script>\n"), lan_ip);	
	websWrite(wp, T("</head>\n<body onload=\"update()\">\n"));
	websWrite(wp, T("<blockquote>\n"));
	websWrite(wp, T("<br><b><script>dw(MM_reboot_system)</script></b>\n"));
	websWrite(wp, T("</blockquote>\n"));
	websWrite(wp, T("</body>\n</html>\n"));
	
	sleep(1);
	system("reboot");
}

/*
 * callee must free memory.
 */
/*
static char *getLog(char *filename)
{
	FILE *fp;
	struct stat filestat;
	char *log;

	if(stat(filename, &filestat) == -1)
		return NULL;

//	printf("%d\n", filestat.st_size);
	log = (char *)malloc(sizeof(char) * (filestat.st_size + 1) );
	if(!log)
		return NULL;

	if(!(fp = fopen(filename, "r"))){
		return NULL;
	}

	if( fread(log, 1, filestat.st_size, fp) != filestat.st_size){
		printf("read not enough\n");
		free(log);
		return NULL;
	}

	log[filestat.st_size] = '\0';

	fclose(fp);
	return log;
}
*/

#if defined CONFIG_LOGREAD && defined CONFIG_KLOGD
static void clearlog(webs_t wp, char_t *path, char_t *query)
{
	char_t  *submitUrl = websGetVar(wp, T("submit-url"), T(""));

	doSystem("killall -q klogd");
	doSystem("killall -q syslogd");
	sleep(1);
	doSystem("syslogd -C8 1>/dev/null 2>&1");
	doSystem("klogd 1>/dev/null 2>&1");
	
	websRedirect(wp, submitUrl);     
}
#endif

#define LOG_MAX (16384)
static void syslog(webs_t wp, char_t *path, char_t *query)
{
	FILE *fp = NULL;
	char *log;

	websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nCache-Control: no-cache\n\n"));

	fp = popen("logread", "r");
	if(!fp){
		websWrite(wp, "-1");
		goto error;
	}

	log = malloc(LOG_MAX * sizeof(char));
	if(!log){
		websWrite(wp, "-1");
		goto error;
	}
	memset(log, 0, LOG_MAX);
	fread(log, 1, LOG_MAX, fp);
	websLongWrite(wp, log);

	free(log);
error:
	if(fp)
		pclose(fp);
	websDone(wp, 200);
}

void management_init(void)
{
	doSystem("killall -q ntp");
	doSystem("killall -q ddns.sh");
	doSystem("ntp.sh &");
#ifdef CONFIG_USER_DDNS
	doSystem("ddns.sh &");
#endif
#ifdef CONFIG_USER_GOAHEAD_GreenAP
    doSystem("greenap.sh init");
#endif	
	WPSRestart();
#if defined (RTDEV_SUPPORT)
#ifndef CONFIG_UNIQUE_WPS
	RaixWPSRestart();
#endif
#endif
#if defined CONFIG_RALINK_WATCHDOG|CONFIG_RALINK_WATCHDOG_MODULE && defined CONFIG_USER_WATCHDOG
	WatchDogRestart();
#endif

	doSystem("killall -q klogd");
	doSystem("killall -q syslogd");
	doSystem("syslogd -C8 1>/dev/null 2>&1");
	doSystem("klogd 1>/dev/null 2>&1");
}

void management_fini(void)
{
	doSystem("killall -q klogd");
	doSystem("killall -q syslogd");
}

static int getGAPBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_USER_GOAHEAD_GreenAP
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getSnortBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_USER_SNORT
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

void formDefineManagement(void)
{
	websAspDefine(T("getPPPOECDBuilt"), getPPPOECDBuilt);
	websAspDefine(T("getWizardBuilt"), getWizardBuilt);
	websAspDefine(T("getWatchDogBuilt"), getWatchDogBuilt);
#ifdef CONFIG_USER_WIZARD
	websFormDefine(T("setWizard"), setWizard);
#endif
	websFormDefine(T("setSysAdm"), setSysAdm);
	websFormDefine(T("setSysLang"), setSysLang);
	websFormDefine(T("NTP"), NTP);
#ifdef CONFIG_DATE
	websFormDefine(T("NTPSyncWithHost"), NTPSyncWithHost);
#endif
	websAspDefine(T("getCurrentTimeASP"), getCurrentTimeASP);
	websAspDefine(T("getGAPBuilt"), getGAPBuilt);
	websAspDefine(T("getSnortBuilt"), getSnortBuilt);
#ifdef CONFIG_USER_GOAHEAD_GreenAP
	websFormDefine(T("GreenAP"), GreenAP);
#endif
#if defined CONFIG_USER_DDNS
	websFormDefine(T("DDNS"), DDNS);
	websAspDefine(T("getDdnsCombox"), getDdnsCombox);
#endif
#ifdef CONFIG_USER_SNORT
	websFormDefine(T("Snort"), Snort);
#endif

	websAspDefine(T("getMemLeftASP"), getMemLeftASP);
	websAspDefine(T("getMemTotalASP"), getMemTotalASP);

	websAspDefine(T("getWANRxByteASP"), getWANRxByteASP);
	websAspDefine(T("getWANTxByteASP"), getWANTxByteASP);
	websAspDefine(T("getLANRxByteASP"), getLANRxByteASP);
	websAspDefine(T("getLANTxByteASP"), getLANTxByteASP);
	websAspDefine(T("getWANRxPacketASP"), getWANRxPacketASP);
	websAspDefine(T("getWANTxPacketASP"), getWANTxPacketASP);
	websAspDefine(T("getLANRxPacketASP"), getLANRxPacketASP);
	websAspDefine(T("getLANTxPacketASP"), getLANTxPacketASP);

	websAspDefine(T("getAllNICStatisticASP"), getAllNICStatisticASP);

	websAspDefine(T("showSystemCommandASP"), showSystemCommandASP);
	websFormDefine(T("SystemCommand"), SystemCommand);
	websFormDefine(T("repeatLastSystemCommand"), repeatLastSystemCommand);

	websFormDefine(T("LoadDefaultSettings"), LoadDefaultSettings);
	websFormDefine(T("RebootSystem"), RebootSystem);

	websFormDefine(T("syslog"), syslog);
#if defined CONFIG_LOGREAD && defined CONFIG_KLOGD
	websFormDefine(T("clearlog"), clearlog);
#endif

	websAspDefine(T("FirmwareUpgradePostASP"), FirmwareUpgradePostASP);

	formDefineWPS();
#if defined (RTDEV_SUPPORT)
#ifndef CONFIG_UNIQUE_WPS
	formDefineRaixWPS();
#endif
#endif
}

static int getWizardBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined CONFIG_USER_WIZARD
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getPPPOECDBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined CONFIG_USER_PPPOECD
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getWatchDogBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined CONFIG_RALINK_WATCHDOG|CONFIG_RALINK_WATCHDOG_MODULE && defined CONFIG_USER_WATCHDOG
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

