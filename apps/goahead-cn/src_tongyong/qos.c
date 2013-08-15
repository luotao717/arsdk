#include    <stdlib.h>
#include    <sys/ioctl.h>
#include    <net/if.h>
#include    <net/route.h>
#include    <string.h>
#include    <dirent.h>
#include    "internet.h"
#include    "nvram.h"
#include    "webs.h"
#include    "utils.h"
#include    "firewall.h"
#include    "qos.h"
#include    "config/autoconf.h" //user config
#include    "linux/autoconf.h"  //kernel config

#if 0
extern char l7name[];							// in firewall.c
inline int getRuleNums(const char *);

struct entry_s QOS_PROFILE[QOS_PROFILE_ENTRYS_MAX] = {
	/* QoS model, 1:DRR, 2:SPQ, 3:SPQ+DRR 4: RemarkerOnly */
	{"QoSModel",	"1"},

	/* Egress(upload) groups */
	{"QoSAF5ULName","High(Upload)"},
	{"QoSAF5ULRate","30"},
	{"QoSAF5ULCeil","100"},
	{"QoSAF2ULName","Middle(Upload)"},
	{"QoSAF2ULRate","20"},
	{"QoSAF2ULCeil","100"},
	{"QoSAF6ULName","Default(Upload)"},
	{"QoSAF6ULRate","5"},
	{"QoSAF6ULCeil","100"},
	{"QoSAF1ULName","Low(Upload)"},
	{"QoSAF1ULRate","10"},
	{"QoSAF1ULCeil","100"},

	/* Ingress(download) groups */
	{"QoSAF5DLName","High(Download)"},
	{"QoSAF5DLRate","30"},
	{"QoSAF5DLCeil","100"},
	{"QoSAF2DLName","Middle(Download)"},
	{"QoSAF2DLRate","20"},
	{"QoSAF2DLCeil","100"},
	{"QoSAF6DLName","Default(Download)"},
	{"QoSAF6DLRate","5"},
	{"QoSAF6DLCeil","100"},
	{"QoSAF1DLName","Low(Download)"},
	{"QoSAF1DLRate","10"},
	{"QoSAF1DLCeil","100"},

	/* QoS Rules */
	{"QoSULRules",
"ICMP_HIGH,5,1,,ICMP,,,,,,,,,,,,EF;\
Small_Packet_HIGH,5,1,,,,,0,128,,,,,,,,EF;\
VoIP_H323_HIGH,5,1,,Application,,,,,,,,,h323,,,EF;\
VoIP_SIP_HIGH,5,1,,Application,,,,,,,,,sip,,,EF;\
VoIP_Skype1_HIGH,5,1,,Application,,,,,,,,,skypeout,,,EF;\
VoIP_Skype2_HIGH,5,1,,Application,,,,,,,,,skypetoskype,,,EF;\
RTP_HIGH,5,1,,Application,,,,,,,,,rtp,,,EF;\
SSH_HIGH,5,1,,Application,,,,,,,,,ssh,,,EF;\
MSN_Messenger_MIDDLE,2,1,,Application,,,,,,,,,msnmessenger,,,AF21;\
Yahoo_MIDDLE,2,1,,Application,,,,,,,,,yahoo,,,AF21;\
PoP3_LOW,1,1,,Application,,,,,,,,,msnmessenger,,,AF11;\
SMTP_LOW,1,1,,Application,,,,,,,,,smtp,,,AF11;\
P2P_eMule_LOW,1,1,,Application,,,,,,,,,edonkey,,,AF11;\
P2P_BT_LOW,1,1,,Application,,,,,,,,,bittorrent,,,AF11"
	},

	{ NULL, NULL}
};

/*
static void QoSAFAttribute(webs_t wp, char_t *path, char_t *query)
{
	char_t  *submitUrl = websGetVar(wp, T("submit-url"), T(""));

	char tmp[512];
	char_t *dir, *index, *name, *rate, *ceil;
	int index_i;
	dir = websGetVar(wp, T("af_dir"), T(""));
	index = websGetVar(wp, T("af_index"), T(""));
	name = websGetVar(wp, T("af_name"), T(""));
	rate = websGetVar(wp, T("af_rate"), T(""));
	ceil = websGetVar(wp, T("af_ceil"), T(""));
	if(!dir || !index|| !name || !rate || !ceil)
		return;
	if(!strlen(dir) || !strlen(index) || !strlen(name) || !strlen(rate) || !strlen(ceil))
		return;
	if( strcmp("Download", dir) && strcmp("Upload", dir) )
		return;

	index_i = atoi(index);
	if(index_i > 6 || index_i <= 0)
		return;
	if(atoi(rate) > 100 || atoi(ceil) > 100)		//percentage
		return;

	sprintf(tmp, "QoSAF%d%sName", index_i, (!strcmp("Download", dir)) ? "DL" : "UL");
	nvram_bufset(RT2860_NVRAM, tmp, name);
	sprintf(tmp, "QoSAF%d%sRate", index_i, (!strcmp("Download", dir)) ? "DL" : "UL");
	nvram_bufset(RT2860_NVRAM, tmp, rate);
	sprintf(tmp, "QoSAF%d%sCeil", index_i, (!strcmp("Download", dir)) ? "DL" : "UL");
	nvram_bufset(RT2860_NVRAM, tmp, ceil);

	nvram_commit(RT2860_NVRAM);

	QoSRestart();

    	websRedirect(wp, submitUrl);
}
*/

static void qosClassifier(webs_t wp, char_t *path, char_t *query)
{
	char_t  *submitUrl = websGetVar(wp, T("submit-url"), T(""));

	char rule[8192];
	const char *old_rule;
	char tmp[8];

	char_t *dir, *af_index, *dp_index, *comment, *mac_address, *dip_address, *sip_address, *pktlenfrom, *pktlento,
			*protocol, *dprf, *dprt, *sprf, *sprt, *layer7, *dscp, *ingress_if, *remark ;
	int af_index_i, dp_index_i, sprf_int, dprf_int, sprt_int, dprt_int;

	dir = websGetVar(wp, T("dir"), T(""));
	af_index = websGetVar(wp, T("af_index"), T(""));
	dp_index = websGetVar(wp, T("dp_index"), T(""));
	comment = websGetVar(wp, T("comment"), T(""));
	mac_address = websGetVar(wp, T("mac_address"), T(""));
	dip_address = websGetVar(wp, T("dip_address"), T(""));
	sip_address = websGetVar(wp, T("sip_address"), T(""));
	pktlenfrom = websGetVar(wp, T("pktlenfrom"), T(""));
	pktlento = websGetVar(wp, T("pktlento"), T(""));
	protocol = websGetVar(wp, T("protocol"), T(""));
	dprf = websGetVar(wp, T("dFromPort"), T(""));
	dprt = websGetVar(wp, T("dToPort"), T(""));
	sprf = websGetVar(wp, T("sFromPort"), T(""));
	sprt = websGetVar(wp, T("sToPort"), T(""));
	layer7 = websGetVar(wp, T("layer7"), T(""));
	dscp = websGetVar(wp, T("dscp"), T(""));
	ingress_if = "";
	remark =  websGetVar(wp, T("remark_dscp"), T(""));

	if(!dir || !af_index || !dp_index || !comment || !remark)
		return;

	if(!strlen(dir) || !strlen(af_index) || !strlen(dp_index) || strlen(comment) > 32)
		return;

	// i know you will try to break our box... ;)
	if(strchr(comment, ';') || strchr(comment, ','))
		return;

	af_index_i = atoi(af_index);
	dp_index_i = atoi(dp_index);
/*
	if(qosGetIndexByName(af_index_i, dp_index_i, comment) != -1){
		// This name is not unique.
		// return;
	}
*/
	// pkt len from/to must be co-exist.
	if( (!strlen(pktlenfrom) && strlen(pktlento)) || 
		(strlen(pktlenfrom) && !strlen(pktlento)) )
		return;

	if(!strlen(protocol)){
		if(	!strlen(mac_address) && !strlen(sip_address) && !strlen(dip_address) &&
			!strlen(dscp) /*&& !strlen(ingress_if)*/ && !strlen(pktlenfrom) )
			return;
		layer7 = "";
	}else if(!strcmp(protocol, "TCP") || !strcmp(protocol, "UDP")){
		if(!strlen(dprf) && !strlen(sprf))
			return;
		layer7 = "";
	}else if(!strcmp(protocol, "Application")){
		if(!strlen(layer7))
			return;
	}else if(!strcmp(protocol, "ICMP")){
		layer7 = "";	// do nothing.
	}else
		return;

	// we dont trust user input.....
	if(strlen(mac_address)){
		if(!isMacValid(mac_address))
			return;
	}

	if(strlen(sip_address)){
		if(!isIpNetmaskValid(sip_address))
			return;
	}

    if(strlen(dip_address)){
        if(!isIpNetmaskValid(dip_address))
            return;
    }


	if(!strlen(sprf)){
		sprf_int = 0;
	}else{
		sprf_int = atoi(sprf);
		if(sprf_int == 0 || sprf_int > 65535)
			return;
	}
	if(!strlen(sprt)){
		sprt_int = 0;
	}else{
		sprt_int = atoi(sprt);
		if(sprt_int ==0 || sprt_int > 65535)
			return;
	}
	if(!strlen(dprf)){
		dprf_int = 0;
	}else{
		dprf_int = atoi(dprf);
		if(dprf_int == 0 || dprf_int > 65535)
			return;
	}
	if(!strlen(dprt)){
		dprt_int = 0;
	}else{
		dprt_int = atoi(dprt);
		if(dprt_int == 0 || dprt_int > 65535)
			return;
	}

	if(!strcmp(remark, "Auto")){
		if(af_index_i == 5 && dp_index_i == 1){				/* EF class */
			remark = "EF";
		}else if(af_index_i == 6 && dp_index_i == 1){		/* BE class */
			remark = "BE";
		}else{                                  /* AF classes */
            snprintf(tmp, sizeof(tmp), "AF%d%d", af_index_i, dp_index_i);
            remark = tmp;
        }
	}

	if(!strcmp(dir, "Download"))
		old_rule = nvram_bufget(RT2860_NVRAM, "QoSDLRules");
	else
		old_rule = nvram_bufget(RT2860_NVRAM, "QoSULRules");

	if(!old_rule || !strlen(old_rule))
		snprintf(rule, sizeof(rule), "%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s", comment, af_index, dp_index, mac_address, protocol, dip_address, sip_address, pktlenfrom, pktlento, dprf, dprt, sprf, sprt, layer7, dscp, ingress_if, remark);
	else
		snprintf(rule, sizeof(rule), "%s;%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s", old_rule, comment, af_index, dp_index, mac_address, protocol, dip_address, sip_address, pktlenfrom, pktlento, dprf, dprt, sprf, sprt, layer7, dscp, ingress_if, remark);

	if(!strcmp(dir, "Download"))
		nvram_bufset(RT2860_NVRAM, "QoSDLRules", rule);
	else	
		nvram_bufset(RT2860_NVRAM, "QoSULRules", rule);
	nvram_commit(RT2860_NVRAM);

	QoSRestart();

	websRedirect(wp, submitUrl);
}


/*
static int qosGetIndexByName(int af_index, int dp_index, char *match)
{
	int i;
	char rec[256];
	char asdp_str[512];
	char *rule, name[32];

	sprintf(asdp_str, "QoSAF%dDP%d", af_index, dp_index);
	rule = nvram_bufget(RT2860_NVRAM, asdp_str);
	if(!rule || !strlen(rule))
		return -1;

	i=0;
	while( (getNthValueSafe(i, rule, ';', rec, sizeof(rec)) != -1) ){
		if( (getNthValueSafe(0, rec, ',', name, sizeof(name)) == -1) )
			continue;
		if(!strcmp(name, match))
			return i;
		i++;
	}
	return -1;
}
*/

/*
static void QoSDelete(webs_t wp, char_t *path, char_t *query)
{
	char tmp[32];
	char *orig_rule, *new_rule;
	int del_index, af_i, dp_i;

	char_t af[2], dp[2], name[64];
    printf("query = %s\n", query);
    sscanf(query, "%2s %2s %64s", af, dp, name);
    printf("af = %s\n dp = %s\n name = %s\n", af, dp, name);

	af_i = atoi(af);
	dp_i = atoi(dp);

	snprintf(tmp, sizeof(tmp), "QoSAF%dDP%d", af_i, dp_i);
	orig_rule = nvram_bufget(RT2860_NVRAM, tmp);
	if(!orig_rule || !strlen(orig_rule))
		goto err;

	del_index = qosGetIndexByName(af_i, dp_i, name);
	printf("del_index = %d\n", del_index);
	if(del_index == -1)
		goto err;

	new_rule = strdup(orig_rule);
	deleteNthValueMulti(&del_index, 1, new_rule, ';');

	nvram_bufset(RT2860_NVRAM, tmp, new_rule);
    nvram_commit(RT2860_NVRAM);

	free(new_rule);

	doSystem("qos_run c");

err:

    websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nCache-Control: no-cache\n\n"));
    websWrite(wp, T("success\n"));
    websDone(wp, 200);
}
*/

static void QoSDeleteRules(webs_t wp, char_t *path, char_t *query, char *which_rule)
{
    int i, j, rule_count;
    char_t name_buf[16];
    char_t *value;
    int *deleArray;

    char *new_rules;
	const char *rules = nvram_bufget(RT2860_NVRAM, which_rule);
    if(!rules || !strlen(rules) )
        return;

    rule_count = getRuleNums(rules);
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
        snprintf(name_buf, 16, "del_qos_%d", i);
        value = websGetVar(wp, name_buf, NULL);
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

    deleteNthValueMulti(deleArray, j, new_rules, ';');

    nvram_set(RT2860_NVRAM, which_rule, new_rules);
    nvram_commit(RT2860_NVRAM);

	QoSRestart();

    websHeader(wp);
	websWrite(wp, T("Delete Rules:\n") );
	for(i=0; i<j; i++)
		websWrite(wp, "%d ", (deleArray[i] + 1) );

    websFooter(wp);
    websDone(wp, 200);
    free(deleArray);
    free(new_rules);
}

static void QoSDeleteULRules(webs_t wp, char_t *path, char_t *query)
{
	QoSDeleteRules(wp, path, query, "QoSULRules");
}

static void QoSDeleteDLRules(webs_t wp, char_t *path, char_t *query)
{
	QoSDeleteRules(wp, path, query, "QoSDLRules");
}

static void QoSLoadDefaultProfile(webs_t wp, char_t *path, char_t *query)
{
    int i;
	for(i=0; i<QOS_PROFILE_ENTRYS_MAX ; i++){
		if(QOS_PROFILE[i].name){
			nvram_bufset(RT2860_NVRAM, QOS_PROFILE[i].name, QOS_PROFILE[i].value);
		}else{
			break;
		}
	}
	nvram_commit(RT2860_NVRAM);
	QoSRestart(); 

    websHeader(wp);
    websWrite(wp, T("Load default profile successfully.\n"));
    websFooter(wp);
    websDone(wp, 200);	
}

static void QoSPortSetup(webs_t wp, char_t *path, char_t *query)
{
	int i;
	char_t *portX[32];
	char_t *portX_remark[32];	
	char tmp_str[512];

	memset(portX, 0, sizeof(char *) * 32);
	memset(portX_remark, 0, sizeof(char *) * 32);

	for(i=0; i<5; i++){
		sprintf(tmp_str, "port%d_group", i);
		portX[i] = websGetVar(wp, T(tmp_str), T(""));

		sprintf(tmp_str, "port%d_remarker", i);
		portX_remark[i] = websGetVar(wp, T(tmp_str), T(""));
	}

	for(i=1; i<9; i++){
		sprintf(tmp_str, "ssid%d_group", i);
		portX[i+4] = websGetVar(wp, T(tmp_str), T(""));

		sprintf(tmp_str, "ssid%d_remarker", i);
		portX_remark[i+4] = websGetVar(wp, T(tmp_str), T(""));
	}

	tmp_str[0] = '\0';
	for(i=0; i<13; i++){
		if(portX[i])
			strcat(tmp_str, portX[i]);
		strcat(tmp_str, ",");
		if(portX_remark[i])
			strcat(tmp_str, portX_remark[i]);
		strcat(tmp_str, ",");
	}

	nvram_bufset(RT2860_NVRAM, "QoSPortBasedRules", tmp_str);
	nvram_commit(RT2860_NVRAM);

	QoSRestart(); 

	websHeader(wp);
	for(i=0; i<13; i++){
		sprintf(tmp_str, "port%d=%s,%s<br>", i, portX[i], portX_remark[i]);
		websWrite(wp, T(tmp_str));
	}
	websFooter(wp);
	websDone(wp, 200);	
}

static void QoSSetup(webs_t wp, char_t *path, char_t *query)
{
	char_t  *submitUrl = websGetVar(wp, T("submit-url"), T(""));

	char_t *qos_enable, *qos_model, *upload_bandwidth, *download_bandwidth, *upload_bandwidth_custom, *download_bandwidth_custom;
	char_t *highest_upload_queue_rate, *highest_upload_queue_ceil, *high_upload_queue_rate, *high_upload_queue_ceil, *default_upload_queue_rate, *default_upload_queue_ceil, *low_upload_queue_rate, *low_upload_queue_ceil;
	char_t *highest_download_queue_rate, *highest_download_queue_ceil, *high_download_queue_rate, *high_download_queue_ceil, *default_download_queue_rate, *default_download_queue_ceil, *low_download_queue_rate, *low_download_queue_ceil;
	char_t *reserve_bandwidth;
	qos_enable = websGetVar(wp, T("QoSSelect"), T(""));
	qos_model = websGetVar(wp, T("Model"), T(""));
	upload_bandwidth = websGetVar(wp, T("UploadBandwidth"), T(""));
	download_bandwidth = websGetVar(wp, T("DownloadBandwidth"), T(""));
	upload_bandwidth_custom = websGetVar(wp, T("UploadBandwidth_Custom"), T(""));
	download_bandwidth_custom = websGetVar(wp, T("DownloadBandwidth_Custom"), T(""));

	reserve_bandwidth = websGetVar(wp, T("ReserveBandwidth"), T(""));

	highest_upload_queue_rate  = websGetVar(wp, T("highest_upload_queue_rate"), T(""));
	high_upload_queue_rate  = websGetVar(wp, T("high_upload_queue_rate"), T(""));
	default_upload_queue_rate  = websGetVar(wp, T("default_upload_queue_rate"), T(""));
	low_upload_queue_rate  = websGetVar(wp, T("low_upload_queue_rate"), T(""));

	highest_upload_queue_ceil  = websGetVar(wp, T("highest_upload_queue_ceil"), T(""));
	high_upload_queue_ceil  = websGetVar(wp, T("high_upload_queue_ceil"), T(""));
	default_upload_queue_ceil  = websGetVar(wp, T("default_upload_queue_ceil"), T(""));
	low_upload_queue_ceil  = websGetVar(wp, T("low_upload_queue_ceil"), T(""));

	highest_download_queue_rate  = websGetVar(wp, T("highest_download_queue_rate"), T(""));
	high_download_queue_rate  = websGetVar(wp, T("high_download_queue_rate"), T(""));
	default_download_queue_rate  = websGetVar(wp, T("default_download_queue_rate"), T(""));
	low_download_queue_rate  = websGetVar(wp, T("low_download_queue_rate"), T(""));

	highest_download_queue_ceil  = websGetVar(wp, T("highest_download_queue_ceil"), T(""));
	high_download_queue_ceil  = websGetVar(wp, T("high_download_queue_ceil"), T(""));
	default_download_queue_ceil  = websGetVar(wp, T("default_download_queue_ceil"), T(""));
	low_download_queue_ceil  = websGetVar(wp, T("low_download_queue_ceil"), T(""));

	if(!qos_enable || !strlen(qos_enable))
		return;

	if(!strcmp(upload_bandwidth, "custom"))
		if(!upload_bandwidth_custom)
			return;

	if(!strcmp(download_bandwidth, "custom"))
		if(!download_bandwidth_custom)
			return;

	if(!strcmp(qos_enable, "1") /* bi-dir */ || !strcmp(qos_enable, "2") /* upload */ || !strcmp(qos_enable, "3") /* download */ || !strcmp(qos_enable, "4") /* port based */){
		if(!strlen(upload_bandwidth))
			return;
		if(!strlen(download_bandwidth))
			return;
	}
	nvram_bufset(RT2860_NVRAM, "QoSEnable", qos_enable);

	if(!strcmp(qos_enable, "1") /* bi-dir */ || !strcmp(qos_enable, "2")/* upload */ || !strcmp(qos_enable, "3") /* download */ || !strcmp(qos_enable, "4") /* port based */){
		char postfix[16];
		strncpy(postfix, upload_bandwidth_custom, sizeof(postfix));
		if(!strchr(postfix, 'k') && !strchr(postfix, 'K')  && !strchr(postfix, 'm') && !strchr(postfix, 'M') )
			strncat(postfix, "k", sizeof(postfix));
		nvram_bufset(RT2860_NVRAM, "QoSUploadBandwidth_custom", postfix);
		strncpy(postfix, download_bandwidth_custom, sizeof(postfix));
		if(!strchr(postfix, 'k') && !strchr(postfix, 'K')  && !strchr(postfix, 'm') && !strchr(postfix, 'M') )
			strncat(postfix, "k", sizeof(postfix));

		nvram_bufset(RT2860_NVRAM, "QoSDownloadBandwidth_custom", postfix);
		nvram_bufset(RT2860_NVRAM, "QoSUploadBandwidth", upload_bandwidth);
		nvram_bufset(RT2860_NVRAM, "QoSDownloadBandwidth", download_bandwidth);

		nvram_bufset(RT2860_NVRAM, "QoSReserveBandwidth", reserve_bandwidth);

		nvram_bufset(RT2860_NVRAM, "QoSModel", qos_model);

		if(highest_upload_queue_rate && strlen(highest_upload_queue_rate))
			nvram_bufset(RT2860_NVRAM, "QoSAF5ULRate", highest_upload_queue_rate);
		if(high_upload_queue_rate && strlen(high_upload_queue_rate))
			nvram_bufset(RT2860_NVRAM, "QoSAF2ULRate", high_upload_queue_rate);
		if(default_upload_queue_rate && strlen(default_upload_queue_rate))
			nvram_bufset(RT2860_NVRAM, "QoSAF6ULRate", default_upload_queue_rate);
		if(low_upload_queue_rate && strlen(low_upload_queue_rate))
			nvram_bufset(RT2860_NVRAM, "QoSAF1ULRate", low_upload_queue_rate);		

		if(highest_upload_queue_ceil && strlen(highest_upload_queue_ceil))
			nvram_bufset(RT2860_NVRAM, "QoSAF5ULCeil", highest_upload_queue_ceil);
		if(high_upload_queue_ceil && strlen(high_upload_queue_ceil))
			nvram_bufset(RT2860_NVRAM, "QoSAF2ULCeil", high_upload_queue_ceil);
		if(default_upload_queue_ceil && strlen(default_upload_queue_ceil))
			nvram_bufset(RT2860_NVRAM, "QoSAF6ULCeil", default_upload_queue_ceil);
		if(low_upload_queue_ceil && strlen(low_upload_queue_ceil))
			nvram_bufset(RT2860_NVRAM, "QoSAF1ULCeil", low_upload_queue_ceil);	

		if(highest_download_queue_rate && strlen(highest_download_queue_rate))
			nvram_bufset(RT2860_NVRAM, "QoSAF5DLRate", highest_download_queue_rate);
		if(high_download_queue_rate && strlen(high_download_queue_rate))
			nvram_bufset(RT2860_NVRAM, "QoSAF2DLRate", high_download_queue_rate);
		if(default_download_queue_rate && strlen(default_download_queue_rate))
			nvram_bufset(RT2860_NVRAM, "QoSAF6DLRate", default_download_queue_rate);
		if(low_download_queue_rate && strlen(low_download_queue_rate))
			nvram_bufset(RT2860_NVRAM, "QoSAF1DLRate", low_download_queue_rate);	

		if(highest_download_queue_ceil && strlen(highest_download_queue_ceil))
			nvram_bufset(RT2860_NVRAM, "QoSAF5DLCeil", highest_download_queue_ceil);
		if(high_download_queue_ceil && strlen(high_download_queue_ceil))
			nvram_bufset(RT2860_NVRAM, "QoSAF2DLCeil", high_download_queue_ceil);
		if(default_download_queue_ceil && strlen(default_download_queue_ceil))
			nvram_bufset(RT2860_NVRAM, "QoSAF6DLCeil", default_download_queue_ceil);
		if(low_download_queue_ceil && strlen(low_download_queue_ceil))
			nvram_bufset(RT2860_NVRAM, "QoSAF1DLCeil", low_download_queue_ceil);

	}

	// 
	nvram_commit(RT2860_NVRAM);

	QoSRestart();

	websRedirect(wp, submitUrl);	
}

static int QoSisPortBasedQoSSupport(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined (CONFIG_RALINK_RT3052) || defined (CONFIG_RALINK_RT3352) || defined (CONFIG_RALINK_RT5350) || defined (CONFIG_RALINK_RT6855) || defined (CONFIG_RALINK_MT7620) || defined (CONFIG_RALINK_MT7621)
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}
#endif

/*modify by hs		2012-12-14 */
inline void QoSRestart(void)
{
	char *CurQosE = (char *)nvram_bufget(RT2860_NVRAM, "QosEnable");

	if (atoi(CurQosE))
		doSystem("ip_qos.sh");
}

inline void QoSInit(void)
{
	const char *cm;
	cm = nvram_bufget(RT2860_NVRAM, "wanConnectionMode");
	if (!strncmp(cm, "PPPOE", 6) || !strncmp(cm, "L2TP", 5) || !strncmp(cm, "PPTP", 5) 
#ifdef CONFIG_USER_3G
		|| !strncmp(cm, "3G", 3)
#endif
	){
		// Just return.
		// The PPP daemon would trigger "qos_run" after dial up successfully, so do nothing here.
		return;
	}

	QoSRestart();
}

static int getIpQosEnableASP(int eid, webs_t wp, int argc, char_t **argv)
{
	int type, value;
	char *pfe = (char *)nvram_bufget(RT2860_NVRAM, "QosEnable");

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

static int getIpQosRuleNumsASP(int eid, webs_t wp, int argc, char_t **argv)
{
    char *rules = (char *)nvram_bufget(RT2860_NVRAM, "QosRules");
	if(!rules || !strlen(rules) ){
		websWrite(wp, T("0"));
		return 0;
	}
	websWrite(wp, T("%d"), getRuleNums(rules));
	return 0;
}

//add by chenfei for qos
static void ipQosBasicSettings(webs_t wp, char_t *path, char_t *query)
{
	char_t  *submitUrl = websGetVar(wp, T("submit-url"), T(""));

	char *qos_enable = websGetVar(wp, T("qosEnabled"), T(""));//add by chenfei

	if(!qos_enable && !strlen(qos_enable))
		return;

	if(atoi(qos_enable))	// user choose nothing but press "apply" only
		nvram_set(RT2860_NVRAM, "QosEnable", "1");
	else
		nvram_set(RT2860_NVRAM, "QosEnable", "0");

	nvram_commit(RT2860_NVRAM);

	doSystem("ip_qos.sh");
	
    websRedirect(wp, submitUrl);
}

static int showIpQosRulesASP(int eid, webs_t wp, int argc, char_t **argv)
{
	int	nBytesSent=0;
	int i=0;
	char rec[128];
	char *rules = (char *)nvram_bufget(RT2860_NVRAM, "QosRules");
	char ip[200];
	//char mode[100];
	//char mac[20];
	char ipstart[40], ipend[40], uplink[10], downlink[10], comment[40];

	while(getNthValueSafe(i++, rules, ';', rec, sizeof(rec)) != -1 )
	{
		if((getNthValueSafe(2, rec, ',', ipstart, sizeof(ipstart)) == -1)){
			continue;
		}

		if((getNthValueSafe(3, rec, ',', ipend, sizeof(ipend)) == -1)){
			continue;
		}
	
		sprintf(ip, "%s - %s", ipstart, ipend);

		//strcpy(mac, "----");

		//strcpy(mode, "Restricted maximum bandwidth");
		
		if((getNthValueSafe(5, rec, ',', uplink, sizeof(uplink)) == -1)){
			continue;
		}

		if((getNthValueSafe(6, rec, ',', downlink, sizeof(downlink)) == -1)){
			continue;
		}

		if((getNthValueSafe(8, rec, ',', comment, sizeof(comment)) == -1)){
			continue;
		}

		nBytesSent += websWrite(wp, ("<tr>"
			"<td>%d<input type=\"checkbox\" name=\"delRule%d\" value=\"ON\"></td>\n"
			"<td align=center>%s</td>\n"
      		//"<td align=center>%s</td>\n"
      		//"<td align=center><script>dw(%s)</script></td>\n"
      		"<td align=center>%s</td>\n"
      		"<td align=center>%s</td>\n"
     		"<td align=center>%s</td></tr>\n"),
			i, i, ip, /*"MM_maximum_bw",*/ uplink, downlink, comment);
	}
	return nBytesSent;
}

static int checkQosRuleBw( int curUplinkBw, int curDownlinkBw, int totalUplinkBw, int totalDownlinkBw, char *mode)
{
	int i=0, ret;
	unsigned long tmpTotolUplinkBw, tmpTotalDownlinkBw;
	tmpTotolUplinkBw=curUplinkBw;
	tmpTotalDownlinkBw=curDownlinkBw;

	int up, down;
	char up_tmp[8], down_tmp[8];
	char *rules = (char *)nvram_bufget(RT2860_NVRAM, "QosRules");
	char rec[128];

	printf("curUplinkBw is %d\n", curUplinkBw);
	printf("curDownlinkBw is %d\n", curDownlinkBw);
	printf("totalUplinkBw is %d\n", totalUplinkBw);
	printf("totalDownlinkBw is %d\n", totalDownlinkBw);
	
	while(getNthValueSafe(i++, rules, ';', rec, sizeof(rec)) != -1 ){
		if((getNthValueSafe(6, rec, ',', up_tmp, sizeof(up_tmp)) == -1)){
			continue;
		}
		up = atoi(up_tmp);
		
		if((getNthValueSafe(7, rec, ',', down_tmp, sizeof(down_tmp)) == -1)){
			continue;
		}
		down = atoi(down_tmp);
		
		if ( !atoi(mode) )	{
			tmpTotolUplinkBw += up;
			tmpTotalDownlinkBw += down;
		}
		else
			continue;
	}

	ret=0;
	
	if(tmpTotolUplinkBw > totalUplinkBw)
		ret += 1;

	if(tmpTotalDownlinkBw > totalDownlinkBw)
		ret += 2;

	return ret;
}

static void ipQosDelete(webs_t wp, char_t *path, char_t *query)
{
    char_t  *submitUrl = websGetVar(wp, T("submit-url"), T(""));

	int i, j, rule_count;
    char_t name_buf[16];
    char_t *value;
    int *deleArray;

    char *new_rules;
    const char *rules = nvram_bufget(RT2860_NVRAM, "QosRules");
    if(!rules || !strlen(rules) )
        return;

    char *CurQosNum = (char *)nvram_bufget(RT2860_NVRAM, "QosRulesNum");
    char numbuf[5];
	
    rule_count = atoi(CurQosNum);

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
        snprintf(name_buf, 16, "delRule%d", i+1);
        value = websGetVar(wp, name_buf, NULL);
		
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

    deleteNthValueMulti(deleArray, j, new_rules, ';');

    nvram_set(RT2860_NVRAM, "QosRules", new_rules);	
    stritoa(rule_count-j, numbuf, sizeof(numbuf));
    nvram_set(RT2860_NVRAM, "QosRulesNum", numbuf);
	
    nvram_commit(RT2860_NVRAM);

	QoSRestart();

    //websHeader(wp);
	//websWrite(wp, T("Delete Rule:\n") );
	//for(i=0; i<j; i++)
	//	websWrite(wp, "%d ", (deleArray[i] + 1) );

    //websFooter(wp);
    //websDone(wp, 200);
    free(deleArray);
    free(new_rules);

	websRedirect(wp, submitUrl);

	return;
}

static void ipQos(webs_t wp, char_t *path, char_t *query)
{
	char_t  *submitUrl = websGetVar(wp, T("submit-url"), T(""));

	char rule[1024];
	char *CurQosRules, *CurQosNum;
	char_t *auto_up, *manual_up, *auto_down, *manual_down;
	char_t *ip_start, *ip_end, *mac, *up_bandwidth, *down_bandwidth, *comment;
	char_t *address_type, *qos_mode;
	int totalUplinkBw, totalDownlinkBw;
	int iRet;
	char tmpbuf[200];
	int num = 0;
	char numbuf[5];

	auto_up = websGetVar(wp, T("automaticUplinkSpeed"), T(""));
	manual_up = websGetVar(wp, T("manualUplinkSpeed"), T(""));
	auto_down = websGetVar(wp, T("automaticDownlinkSpeed"), T(""));
	manual_down = websGetVar(wp, T("manualDownlinkSpeed"), T(""));
	ip_start = websGetVar(wp, T("ipStart"), T(""));
	ip_end = websGetVar(wp, T("ipEnd"), T(""));
	mac = websGetVar(wp, T("mac"), T(""));
	up_bandwidth = websGetVar(wp, T("bandwidth"), T(""));
	down_bandwidth = websGetVar(wp, T("bandwidth_downlink"), T(""));
	comment = websGetVar(wp, T("comment"), T(""));
	address_type = websGetVar(wp, T("addressType"), T(""));
	qos_mode = websGetVar(wp, T("mode"), T(""));
	
	if (atoi(auto_up)==1){
		totalUplinkBw=102400;			// 100Mbps
		nvram_bufset(RT2860_NVRAM, "AutoUplinkSpeed", "1");
	}
	else{
		nvram_bufset(RT2860_NVRAM, "AutoUplinkSpeed", "0");
		nvram_bufset(RT2860_NVRAM, "ManualUplinkSpeed", manual_up);
		totalUplinkBw = atoi(manual_up);		
	}

	if (atoi(auto_down)==1){
		nvram_bufset(RT2860_NVRAM, "AutoDownlinkSpeed", "1");
		totalDownlinkBw=102400;			// 100Mbps
	}
	else{
		nvram_bufset(RT2860_NVRAM, "AutoDownlinkSpeed", "0");
		nvram_bufset(RT2860_NVRAM, "ManualDownlinkSpeed", manual_down);
		totalDownlinkBw = atoi(manual_down);
	}

	if (!atoi(qos_mode)){
		iRet = checkQosRuleBw(atoi(up_bandwidth), atoi(down_bandwidth), totalUplinkBw, totalDownlinkBw, qos_mode);

		if(iRet==3){
			websError(wp, 200, "both link oversize");
			return;
		}
		else if(iRet==2){
			websError(wp, 200, "downlink oversize");
			return;
		}
		else if(iRet==1){
			websError(wp, 200, "uplink oversize");
			return;
		}
	}

	//set rule
	if( strcmp(up_bandwidth, "") && strcmp(down_bandwidth, "") )
	{
			if(( CurQosRules = (char *)nvram_bufget(RT2860_NVRAM, "QosRules")) && strlen( CurQosRules) ) {
				snprintf(rule, sizeof(rule), "%s;%d,%d,%s,%s,%s,%d,%d,%d,%s",  
				CurQosRules,  totalUplinkBw, totalDownlinkBw, ip_start, ip_end, mac, atoi(up_bandwidth), atoi(down_bandwidth), atoi(address_type) ? 10 : 6, comment);
			}
			else{
				snprintf(rule, sizeof(rule), "%d,%d,%s,%s,%s,%d,%d,%d,%s",  
				totalUplinkBw, totalDownlinkBw, ip_start, ip_end, mac, atoi(up_bandwidth), atoi(down_bandwidth), atoi(address_type) ? 10 : 6, comment);
			}
			
			nvram_set(RT2860_NVRAM, "QosRules", rule);

			CurQosNum = (char *)nvram_bufget(RT2860_NVRAM, "QosRulesNum");
			if(atoi(CurQosNum) != 0) {
				num = atoi(CurQosNum) + 1;
				stritoa(num, numbuf, sizeof(numbuf));
			}
			else{
				num = 1;
				stritoa(num, numbuf, sizeof(numbuf));
			}

			nvram_set(RT2860_NVRAM, "QosRulesNum", numbuf);	
			nvram_commit(RT2860_NVRAM);
	
	}
	
	char *qos_enable = (char *)nvram_bufget(RT2860_NVRAM, "QosEnable");
	if (atoi(qos_enable)==1)
		doSystem("ip_qos.sh");

    websRedirect(wp, submitUrl);
}

void formDefineQoS(void)
{
#if 0
//	websFormDefine(T("QoSAFAttribute"), QoSAFAttribute);
	websFormDefine(T("QoSPortSetup"), QoSPortSetup);
	websFormDefine(T("qosClassifier"), qosClassifier);
	websFormDefine(T("QoSSetup"), QoSSetup);
//	websFormDefine(T("QoSDelete"), QoSDelete);
	websFormDefine(T("QoSDeleteULRules"), QoSDeleteULRules);
	websFormDefine(T("QoSDeleteDLRules"), QoSDeleteDLRules);
	websFormDefine(T("QoSLoadDefaultProfile"), QoSLoadDefaultProfile);
	websAspDefine(T("QoSisPortBasedQoSSupport"), QoSisPortBasedQoSSupport);
#endif
	websFormDefine(T("ipQos"), ipQos);
	websFormDefine(T("ipQosDelete"), ipQosDelete);
	websFormDefine(T("ipQosBasicSettings"), ipQosBasicSettings);
	websAspDefine(T("getIpQosEnableASP"), getIpQosEnableASP);
	websAspDefine(T("getIpQosRuleNumsASP"), getIpQosRuleNumsASP);
	websAspDefine(T("showIpQosRulesASP"), showIpQosRulesASP);	
}

