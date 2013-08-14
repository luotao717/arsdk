
/*
  main function
  lktOS wirelss config moudle for all platform 
  
  v1.0 support rt3050,rt3052,mt7620  by luot  20121207
  
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "nvram.h"
#include "lktInitConfig.h"

#define FPRINT_NUM(x) fprintf(fp, #x"=%d\n", atoi(nvram_bufget(ralinkMode, #x)));
#define FPRINT_STR(x) fprintf(fp, #x"=%s\n", nvram_bufget(ralinkMode, #x));

static int getRalinkStdConfigFile(T_LKTOS_INITCONFIG_PLATFORM_TYPE_ platform,unsigned char* errbuf)
{
	FILE *fp;
	int  i, ssid_num = 1;
	char tx_rate[16], wmm_enable[16];
	int ralinkMode=RT2860_NVRAM;

	nvram_init(ralinkMode);

	
	if (platform== RALINK3050_STD) 
	{
		nvram_bufset(ralinkMode, "RFICType", "5");
		nvram_bufset(ralinkMode, "HT_TxStream", "1");
		nvram_bufset(ralinkMode, "TXPath", "1");
		nvram_bufset(ralinkMode, "HT_RxStream", "1");
		nvram_bufset(ralinkMode, "RXPath", "1");
		nvram_commit(ralinkMode);
	}
	else if (platform== MTK7620_STD) 
	{
//		nvram_bufset(ralinkMode, "RFICType", "5");
		nvram_bufset(ralinkMode, "HT_TxStream", "2");
		nvram_bufset(ralinkMode, "TXPath", "2");
		nvram_bufset(ralinkMode, "HT_RxStream", "2");
		nvram_bufset(ralinkMode, "RXPath", "2");
		nvram_commit(ralinkMode);
	}
	else 
	{
		nvram_bufset(ralinkMode, "RFICType", "5");
		nvram_bufset(ralinkMode, "HT_TxStream", "1");
		nvram_bufset(ralinkMode, "TXPath", "1");
		nvram_bufset(ralinkMode, "HT_RxStream", "1");
		nvram_bufset(ralinkMode, "RXPath", "1");
		nvram_commit(ralinkMode);
	}
	system("mkdir -p /etc/Wireless/RT2860");
	fp = fopen("/etc/Wireless/RT2860/RT2860.dat", "w+");
	if(fp == NULL)
	{
		strcpy(errbuf,"open wireless config file error");
		return 0;
	}
	fprintf(fp, "#The word of \"Default\" must not be removed\n");
	fprintf(fp, "Default\n");


	FPRINT_NUM(CountryRegion);
	FPRINT_NUM(CountryRegionABand);
	FPRINT_STR(CountryCode);
	FPRINT_NUM(BssidNum);
	ssid_num = atoi(nvram_get(ralinkMode, "BssidNum"));

	FPRINT_STR(SSID1);
	FPRINT_STR(SSID2);
	FPRINT_STR(SSID3);
	FPRINT_STR(SSID4);
	FPRINT_STR(SSID5);
	FPRINT_STR(SSID6);
	FPRINT_STR(SSID7);
	FPRINT_STR(SSID8);
#if defined (CONFIG_16MBSSID_MODE)
	{
		FPRINT_STR(SSID9);
		FPRINT_STR(SSID10);
		FPRINT_STR(SSID11);
		FPRINT_STR(SSID12);
		FPRINT_STR(SSID13);
		FPRINT_STR(SSID14);
		FPRINT_STR(SSID15);
		FPRINT_STR(SSID16);
	}
#endif


	FPRINT_NUM(WirelessMode);
	FPRINT_STR(FixedTxMode);

	//TxRate(FixedRate)
	bzero(tx_rate, sizeof(char)*16);
	for (i = 0; i < ssid_num; i++)
	{
		sprintf(tx_rate+strlen(tx_rate), "%d",
				atoi(nvram_bufget(ralinkMode, "TxRate")));
			sprintf(tx_rate+strlen(tx_rate), "%c", ';');
	}
	tx_rate[strlen(tx_rate) - 1] = '\0';
	fprintf(fp, "TxRate=%s\n", tx_rate);

	FPRINT_NUM(Channel);
	FPRINT_NUM(BasicRate);
	FPRINT_NUM(BeaconPeriod);
	FPRINT_NUM(DtimPeriod);
	FPRINT_NUM(TxPower);
	FPRINT_NUM(DisableOLBC);
	FPRINT_NUM(BGProtection);
	fprintf(fp, "TxAntenna=\n");
	fprintf(fp, "RxAntenna=\n");
	FPRINT_NUM(TxPreamble);
	FPRINT_NUM(RTSThreshold  );
	FPRINT_NUM(FragThreshold  );
	FPRINT_NUM(TxBurst);
	FPRINT_NUM(PktAggregate);
	FPRINT_NUM(AutoProvisionEn);
	FPRINT_NUM(FreqDelta);
	fprintf(fp, "TurboRate=0\n");


	//WmmCapable
	bzero(wmm_enable, sizeof(char)*16);
	for (i = 0; i < ssid_num; i++)
	{
		sprintf(wmm_enable+strlen(wmm_enable), "%d",
					atoi(nvram_bufget(ralinkMode, "WmmCapable")));
		sprintf(wmm_enable+strlen(wmm_enable), "%c", ';');
	}
	wmm_enable[strlen(wmm_enable) - 1] = '\0';
	fprintf(fp, "WmmCapable=%s\n", wmm_enable);

	FPRINT_STR(APAifsn);
	FPRINT_STR(APCwmin);
	FPRINT_STR(APCwmax);
	FPRINT_STR(APTxop);
	FPRINT_STR(APACM);
	FPRINT_STR(BSSAifsn);
	FPRINT_STR(BSSCwmin);
	FPRINT_STR(BSSCwmax);
	FPRINT_STR(BSSTxop);
	FPRINT_STR(BSSACM);
	FPRINT_STR(AckPolicy);
	FPRINT_STR(APSDCapable);
	FPRINT_STR(DLSCapable);
	FPRINT_STR(NoForwarding);
	FPRINT_NUM(NoForwardingBTNBSSID);
	FPRINT_STR(HideSSID);
	FPRINT_NUM(ShortSlot);
	FPRINT_NUM(AutoChannelSelect);

	FPRINT_STR(IEEE8021X);
	FPRINT_NUM(IEEE80211H);
	FPRINT_NUM(CarrierDetect);

	FPRINT_NUM(ITxBfEn);
	FPRINT_STR(PreAntSwitch);
	FPRINT_NUM(PhyRateLimit);
	FPRINT_NUM(DebugFlags);
	FPRINT_NUM(ETxBfEnCond);
	FPRINT_NUM(ITxBfTimeout);
	FPRINT_NUM(ETxBfTimeout);
	FPRINT_NUM(ETxBfNoncompress);
	FPRINT_NUM(ETxBfIncapable);
	FPRINT_NUM(FineAGC);
	FPRINT_NUM(StreamMode);
	FPRINT_STR(StreamModeMac0);
	FPRINT_STR(StreamModeMac1);
	FPRINT_STR(StreamModeMac2);
	FPRINT_STR(StreamModeMac3);

	FPRINT_NUM(CSPeriod);
	FPRINT_STR(RDRegion);
	FPRINT_NUM(StationKeepAlive);
	FPRINT_NUM(DfsLowerLimit);
	FPRINT_NUM(DfsUpperLimit);
	FPRINT_NUM(DfsOutdoor);
	FPRINT_NUM(SymRoundFromCfg);
      FPRINT_NUM(BusyIdleFromCfg);
      FPRINT_NUM(DfsRssiHighFromCfg);
      FPRINT_NUM(DfsRssiLowFromCfg);
	FPRINT_NUM(DFSParamFromConfig);
	FPRINT_STR(FCCParamCh0);
	FPRINT_STR(FCCParamCh1);
	FPRINT_STR(FCCParamCh2);
	FPRINT_STR(FCCParamCh3);
	FPRINT_STR(CEParamCh0);
	FPRINT_STR(CEParamCh1);
	FPRINT_STR(CEParamCh2);
	FPRINT_STR(CEParamCh3);
	FPRINT_STR(JAPParamCh0);
	FPRINT_STR(JAPParamCh1);
	FPRINT_STR(JAPParamCh2);
	FPRINT_STR(JAPParamCh3);
	FPRINT_STR(JAPW53ParamCh0);
	FPRINT_STR(JAPW53ParamCh1);
	FPRINT_STR(JAPW53ParamCh2);
	FPRINT_STR(JAPW53ParamCh3);
	FPRINT_NUM(FixDfsLimit);
	FPRINT_NUM(LongPulseRadarTh);
	FPRINT_NUM(AvgRssiReq);
	FPRINT_NUM(DFS_R66);
	FPRINT_STR(BlockCh);
	
	FPRINT_NUM(GreenAP);
	FPRINT_STR(PreAuth);
	FPRINT_STR(AuthMode);
	FPRINT_STR(EncrypType);
	/*kurtis: WAPI*/
	FPRINT_STR(WapiPsk1);
	FPRINT_STR(WapiPsk2);
	FPRINT_STR(WapiPsk3);
	FPRINT_STR(WapiPsk4);
	FPRINT_STR(WapiPsk5);
	FPRINT_STR(WapiPsk6);
	FPRINT_STR(WapiPsk7);
	FPRINT_STR(WapiPsk8);
#if defined (CONFIG_16MBSSID_MODE)
	{
		FPRINT_STR(WapiPsk9);
		FPRINT_STR(WapiPsk10);
		FPRINT_STR(WapiPsk11);
		FPRINT_STR(WapiPsk12);
		FPRINT_STR(WapiPsk13);
		FPRINT_STR(WapiPsk14);
		FPRINT_STR(WapiPsk15);
		FPRINT_STR(WapiPsk16);
	}
#endif
	FPRINT_STR(WapiPskType);
	FPRINT_STR(Wapiifname);
	FPRINT_STR(WapiAsCertPath);
	FPRINT_STR(WapiUserCertPath);
	FPRINT_STR(WapiAsIpAddr);
	FPRINT_STR(WapiAsPort);

	FPRINT_NUM(BssidNum);

	FPRINT_STR(RekeyMethod);
	FPRINT_NUM(RekeyInterval);
	FPRINT_STR(PMKCachePeriod);

	FPRINT_NUM(MeshAutoLink);
	FPRINT_STR(MeshAuthMode);
	FPRINT_STR(MeshEncrypType);
	FPRINT_NUM(MeshDefaultkey);
	FPRINT_STR(MeshWEPKEY);
	FPRINT_STR(MeshWPAKEY);
	FPRINT_STR(MeshId);

	//WPAPSK
	FPRINT_STR(WPAPSK1);
	FPRINT_STR(WPAPSK2);
	FPRINT_STR(WPAPSK3);
	FPRINT_STR(WPAPSK4);
	FPRINT_STR(WPAPSK5);
	FPRINT_STR(WPAPSK6);
	FPRINT_STR(WPAPSK7);
	FPRINT_STR(WPAPSK8);
#if defined (CONFIG_16MBSSID_MODE)
	{
		FPRINT_STR(WPAPSK9);
		FPRINT_STR(WPAPSK10);
		FPRINT_STR(WPAPSK11);
		FPRINT_STR(WPAPSK12);
		FPRINT_STR(WPAPSK13);
		FPRINT_STR(WPAPSK14);
		FPRINT_STR(WPAPSK15);
		FPRINT_STR(WPAPSK16);
	}
#endif

	FPRINT_STR(DefaultKeyID);
	FPRINT_STR(Key1Type);
	FPRINT_STR(Key1Str1);
	FPRINT_STR(Key1Str2);
	FPRINT_STR(Key1Str3);
	FPRINT_STR(Key1Str4);
	FPRINT_STR(Key1Str5);
	FPRINT_STR(Key1Str6);
	FPRINT_STR(Key1Str7);
	FPRINT_STR(Key1Str8);
#if defined (CONFIG_16MBSSID_MODE)
	{
		FPRINT_STR(Key1Str9);
		FPRINT_STR(Key1Str10);
		FPRINT_STR(Key1Str11);
		FPRINT_STR(Key1Str12);
		FPRINT_STR(Key1Str13);
		FPRINT_STR(Key1Str14);
		FPRINT_STR(Key1Str15);
		FPRINT_STR(Key1Str16);
	}
#endif
	FPRINT_STR(Key2Type);
	FPRINT_STR(Key2Str1);
	FPRINT_STR(Key2Str2);
	FPRINT_STR(Key2Str3);
	FPRINT_STR(Key2Str4);
	FPRINT_STR(Key2Str5);
	FPRINT_STR(Key2Str6);
	FPRINT_STR(Key2Str7);
	FPRINT_STR(Key2Str8);
#if defined (CONFIG_16MBSSID_MODE)
	{
		FPRINT_STR(Key2Str9);
		FPRINT_STR(Key2Str10);
		FPRINT_STR(Key2Str11);
		FPRINT_STR(Key2Str12);
		FPRINT_STR(Key2Str13);
		FPRINT_STR(Key2Str14);
		FPRINT_STR(Key2Str15);
		FPRINT_STR(Key2Str16);
	}
#endif
	FPRINT_STR(Key3Type);
	FPRINT_STR(Key3Str1);
	FPRINT_STR(Key3Str2);
	FPRINT_STR(Key3Str3);
	FPRINT_STR(Key3Str4);
	FPRINT_STR(Key3Str5);
	FPRINT_STR(Key3Str6);
	FPRINT_STR(Key3Str7);
	FPRINT_STR(Key3Str8);
#if defined (CONFIG_16MBSSID_MODE)
	{
		FPRINT_STR(Key3Str9);
		FPRINT_STR(Key3Str10);
		FPRINT_STR(Key3Str11);
		FPRINT_STR(Key3Str12);
		FPRINT_STR(Key3Str13);
		FPRINT_STR(Key3Str14);
		FPRINT_STR(Key3Str15);
		FPRINT_STR(Key3Str16);
	}
#endif
	FPRINT_STR(Key4Type);
	FPRINT_STR(Key4Str1);
	FPRINT_STR(Key4Str2);
	FPRINT_STR(Key4Str3);
	FPRINT_STR(Key4Str4);
	FPRINT_STR(Key4Str5);
	FPRINT_STR(Key4Str6);
	FPRINT_STR(Key4Str7);
	FPRINT_STR(Key4Str8);
#if defined (CONFIG_16MBSSID_MODE)
	{
		FPRINT_STR(Key4Str9);
		FPRINT_STR(Key4Str10);
		FPRINT_STR(Key4Str11);
		FPRINT_STR(Key4Str12);
		FPRINT_STR(Key4Str13);
		FPRINT_STR(Key4Str14);
		FPRINT_STR(Key4Str15);
		FPRINT_STR(Key4Str16);
	}
#endif
	FPRINT_NUM(HSCounter);

	FPRINT_NUM(HT_HTC);
	FPRINT_NUM(HT_RDG);
	FPRINT_NUM(HT_LinkAdapt);
	FPRINT_NUM(HT_OpMode);
	FPRINT_NUM(HT_MpduDensity);
	FPRINT_NUM(HT_EXTCHA);
	FPRINT_NUM(HT_BW);
	FPRINT_NUM(HT_AutoBA);
	FPRINT_NUM(HT_BADecline);
	FPRINT_NUM(HT_AMSDU);
	FPRINT_NUM(HT_BAWinSize);
	FPRINT_NUM(HT_GI);
	FPRINT_NUM(HT_STBC);
	FPRINT_STR(HT_MCS);
	FPRINT_NUM(HT_TxStream);
	FPRINT_NUM(HT_RxStream);
	FPRINT_NUM(HT_PROTECT);
	FPRINT_NUM(HT_DisallowTKIP);
	FPRINT_NUM(HT_BSSCoexistence);
	FPRINT_NUM(GreenAP);

	FPRINT_NUM(WscConfMode);

	//WscConfStatus
	if (atoi(nvram_bufget(ralinkMode, "WscConfigured")) == 0)
		fprintf(fp, "WscConfStatus=%d\n", 1);
	else
		fprintf(fp, "WscConfStatus=%d\n", 2);
	if (strcmp(nvram_bufget(ralinkMode, "WscVendorPinCode"), "") != 0)
		FPRINT_STR(WscVendorPinCode);
	FPRINT_NUM(WCNTest);
	//market by luot
	//#if defined CONFIG_UNIQUE_WPS
			//FPRINT_STR(WSC_UUID_Str1);
			//FPRINT_STR(WSC_UUID_E1);
	//#endif

	FPRINT_NUM(AccessPolicy0);
	FPRINT_STR(AccessControlList0);
	FPRINT_NUM(AccessPolicy1);
	FPRINT_STR(AccessControlList1);
	FPRINT_NUM(AccessPolicy2);
	FPRINT_STR(AccessControlList2);
	FPRINT_NUM(AccessPolicy3);
	FPRINT_STR(AccessControlList3);
	FPRINT_NUM(AccessPolicy4);
	FPRINT_STR(AccessControlList4);
	FPRINT_NUM(AccessPolicy5);
	FPRINT_STR(AccessControlList5);
	FPRINT_NUM(AccessPolicy6);
	FPRINT_STR(AccessControlList6);
	FPRINT_NUM(AccessPolicy7);
	FPRINT_STR(AccessControlList7);
#if defined (CONFIG_16MBSSID_MODE)
	{
		FPRINT_NUM(AccessPolicy8);
		FPRINT_STR(AccessControlList8);
		FPRINT_NUM(AccessPolicy9);
		FPRINT_STR(AccessControlList9);
		FPRINT_NUM(AccessPolicy10);
		FPRINT_STR(AccessControlList10);
		FPRINT_NUM(AccessPolicy11);
		FPRINT_STR(AccessControlList11);
		FPRINT_NUM(AccessPolicy12);
		FPRINT_STR(AccessControlList12);
		FPRINT_NUM(AccessPolicy13);
		FPRINT_STR(AccessControlList13);
		FPRINT_NUM(AccessPolicy14);
		FPRINT_STR(AccessControlList14);
		FPRINT_NUM(AccessPolicy15);
		FPRINT_STR(AccessControlList15);
	}
#endif
	FPRINT_NUM(WdsEnable);
	FPRINT_STR(WdsPhyMode);
	FPRINT_STR(WdsEncrypType);
	FPRINT_STR(WdsList);
	FPRINT_STR(Wds0Key);
	FPRINT_STR(Wds1Key);
	FPRINT_STR(Wds2Key);
	FPRINT_STR(Wds3Key);
	FPRINT_STR(RADIUS_Server);
	FPRINT_STR(RADIUS_Port);
	FPRINT_STR(RADIUS_Key1);
	FPRINT_STR(RADIUS_Key2);
	FPRINT_STR(RADIUS_Key3);
	FPRINT_STR(RADIUS_Key4);
	FPRINT_STR(RADIUS_Key5);
	FPRINT_STR(RADIUS_Key6);
	FPRINT_STR(RADIUS_Key7);
	FPRINT_STR(RADIUS_Key8);
#if defined (CONFIG_16MBSSID_MODE)
	{
		FPRINT_STR(RADIUS_Key9);
		FPRINT_STR(RADIUS_Key10);
		FPRINT_STR(RADIUS_Key11);
		FPRINT_STR(RADIUS_Key12);
		FPRINT_STR(RADIUS_Key13);
		FPRINT_STR(RADIUS_Key14);
		FPRINT_STR(RADIUS_Key15);
		FPRINT_STR(RADIUS_Key16);
	}
#endif
	FPRINT_STR(RADIUS_Acct_Server);
	FPRINT_NUM(RADIUS_Acct_Port);
	FPRINT_STR(RADIUS_Acct_Key);
	FPRINT_STR(own_ip_addr);
	FPRINT_STR(Ethifname);
	FPRINT_STR(EAPifname);
	FPRINT_STR(PreAuthifname);
	FPRINT_NUM(session_timeout_interval);
	FPRINT_NUM(idle_timeout_interval);
	FPRINT_NUM(WiFiTest);
	FPRINT_NUM(TGnWifiTest);

	//AP Client parameters
	FPRINT_NUM(ApCliEnable);
	FPRINT_STR(ApCliSsid);
	FPRINT_STR(ApCliBssid);
	FPRINT_STR(ApCliAuthMode);
	FPRINT_STR(ApCliEncrypType);
	FPRINT_STR(ApCliWPAPSK);
	FPRINT_NUM(ApCliDefaultKeyID);
	FPRINT_NUM(ApCliKey1Type);
	FPRINT_STR(ApCliKey1Str);
	FPRINT_NUM(ApCliKey2Type);
	FPRINT_STR(ApCliKey2Str);
	FPRINT_NUM(ApCliKey3Type);
	FPRINT_STR(ApCliKey3Str);
	FPRINT_NUM(ApCliKey4Type);
	FPRINT_STR(ApCliKey4Str);

	//Radio On/Off
	if (atoi(nvram_bufget(ralinkMode, "RadioOff")) == 1)
		fprintf(fp, "RadioOn=0\n");
	else
		fprintf(fp, "RadioOn=1\n");

		/*
		 * There are no SSID/WPAPSK/Key1Str/Key2Str/Key3Str/Key4Str anymore since driver1.5 , but 
		 * STA WPS still need these entries to show the WPS result(That is the only way i know to get WPAPSK key) and
		 * so we create empty entries here.   --YY
		 */
		fprintf(fp, "SSID=\nWPAPSK=\nKey1Str=\nKey2Str=\nKey3Str=\nKey4Str=\n");

	nvram_close(ralinkMode);
	fclose(fp);
	return 1;
}

static int initRalinkLanInterfaceByWifi(T_LKTOS_INITCONFIG_PLATFORM_TYPE_ platform) 
{
	FILE *fp;
	int intVal=0;
	int ralinkMode=RT2860_NVRAM;
	int staticDhcpEnable=1;
	char *tmpStr=NULL;
	char *headStr=NULL;
	

	nvram_init(ralinkMode);

	if(platform == MTK7620_STD || platform == RALINK5350_STD)
	{
		intVal = atoi(nvram_bufget(ralinkMode, "BssidNum"));
		if(2== intVal)
		{
			system("ifconfig ra1 up");
			system("brctl addif br0 ra1 1>/dev/null 2>&1");
		}
		if(3== intVal)
		{
			system("ifconfig ra1 up");
			system("ifconfig ra2 up");
			system("brctl addif br0 ra1 1>/dev/null 2>&1");
			system("brctl addif br0 ra2 1>/dev/null 2>&1");
		}
		intVal = atoi(nvram_bufget(ralinkMode, "WdsEnable"));
		if(intVal)
		{
			system("ifconfig wds0 up");
			system("ifconfig wds1 up");
			system("ifconfig wds2 up");
			system("ifconfig wds3 up");
			system("brctl addif br0 wds0 1>/dev/null 2>&1");
			system("brctl addif br0 wds1 1>/dev/null 2>&1");
			system("brctl addif br0 wds2 1>/dev/null 2>&1");
			system("brctl addif br0 wds3 1>/dev/null 2>&1");
		}
	}
	else
	{
		nvram_close(ralinkMode);
		return 0;
	}
	nvram_close(ralinkMode);
	return 1;
}

int lktos_wificonfig_initall_by_filemode(T_LKTOS_INITCONFIG_PLATFORM_TYPE_ platform,unsigned char* errormsg)
{
	int resultFlag=0;
	if(errormsg == NULL)
	{
		printf("\r\n the second papa error");
		return resultFlag;
	}
	if((platform <= UNDEFINED) || (platform >= UNKNOW) )
	{
		printf("\r\n the first para(platform type) error");
		return resultFlag;
	}
	switch(platform)
	{
		case RALINK3052_STD:
			system("ifconfig ra0 down");
			resultFlag=getRalinkStdConfigFile(platform,errormsg);
			system("ifconfig ra0 up");
			break;
		case RALINK3050_STD:
			system("ifconfig ra0 down");
			resultFlag=getRalinkStdConfigFile(platform,errormsg);
			system("ifconfig ra0 up");
			break;
		case RALINK5350_STD:
		case MTK7620_STD:
			system("ifconfig ra0 down");
			system("ifconfig ra1 down 1>/dev/null 2>&1");
			system("ifconfig ra2 down 1>/dev/null 2>&1");
			system("ifconfig wds0 down 1>/dev/null 2>&1");
			system("ifconfig wds1 down 1>/dev/null 2>&1");
			system("ifconfig wds2 down 1>/dev/null 2>&1");
			system("ifconfig wds3 down 1>/dev/null 2>&1");
			system("brctl delif br0 wds0 1>/dev/null 2>&1");
			system("brctl delif br0 wds1 1>/dev/null 2>&1");
			system("brctl delif br0 wds2 1>/dev/null 2>&1");
			system("brctl delif br0 wds3 1>/dev/null 2>&1");
			resultFlag=getRalinkStdConfigFile(platform,errormsg);
			system("ifconfig ra0 up");
			initRalinkLanInterfaceByWifi(platform);
			break;
		default:
			strcpy(errormsg,"wrong palt form!");
			return resultFlag;
			
	}
	return resultFlag;
}

