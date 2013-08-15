<html>
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Expires" content="-1">
<meta http-equiv="Content-Type" content="text/html; charset=<% getCharset(); %>">
<link rel="stylesheet" href="../style/normal_ws.css" type="text/css">
<script language="javascript" src="../js/language_<% getCfgZero(1, "LanguageType"); %>.js"></script>
<script language="javascript" src="../js/common.js"></script>
<script language="javascript">
var selectedIF;
var BssidNum = "<% getCfgZero(1, "BssidNum"); %>";
var PhyMode  = "<% getCfgZero(1, "WirelessMode"); %>";
var AUTH = new Array();
var WPS = new Array();
var HS = new Array();
var INTERNET = new Array();
var HESSID = new Array();
var ROAMING = new Array();
var NAI = new Array();
var REALM1 = new Array();
var REALM2 = new Array();
var REALM3 = new Array();
var REALM4 = new Array();
var EAP1 = new Array();
var EAP2 = new Array();
var EAP3 = new Array();
var EAP4 = new Array();
var LINKSTATUS = new Array();
var CAPACITY = new Array();
var DLSPEED = new Array();
var ULSPEED = new Array();
var DLLOAD = new Array();
var ULLOAD = new Array();
var LMD = new Array();
var PLMN = new Array();
var MCC1 = new Array();
var MCC2 = new Array();
var MCC3 = new Array();
var MCC4 = new Array();
var MCC5 = new Array();
var MCC6 = new Array();
var MNC1 = new Array();
var MNC2 = new Array();
var MNC3 = new Array();
var MNC4 = new Array();
var MNC5 = new Array();
var MNC6 = new Array();
var PROXYARP = new Array();
var DGAF = new Array();
var L2F = new Array();
var ICMP = new Array();

function QuerryValue() 
{
	var str;
		
	str = "<% getCfgGeneral(1, "HS_enabled"); %>";
	HS = str.split(";");
	str = "<% getCfgGeneral(1, "AuthMode"); %>";
	AUTH = str.split(";");
	str = "<% getCfgGeneral(1, "WscModeOption"); %>";
	WPS = str.split(";");
	str = "<% getCfgGeneral(1, "HS_internet"); %>";	
	INTERNET = str.split(";");
	str = "<% getCfgGeneral(1, "HS_hessid"); %>";	
	HESSID = str.split(";");
	str = "<% getCfgGeneral(1, "HS_roaming_consortium_oi"); %>";	
	ROAMING = str.split(";");
	str = "<% getCfgGeneral(1, "HS_nai"); %>";	
	NAI = str.split(";");
	str = "<% getCfgGeneral(1, "HS_nai1_realm"); %>";	
	REALM1 = str.split(";");
	str = "<% getCfgGeneral(1, "HS_nai2_realm"); %>";	
	REALM2 = str.split(";");
	str = "<% getCfgGeneral(1, "HS_nai3_realm"); %>";	
	REALM3 = str.split(";");
	str = "<% getCfgGeneral(1, "HS_nai4_realm"); %>";	
	REALM4 = str.split(";");
	str = "<% getCfgGeneral(1, "HS_nai1_eap_method"); %>";	
	EAP1 = str.split(";");
	str = "<% getCfgGeneral(1, "HS_nai2_eap_method"); %>";	
	EAP2 = str.split(";");
	str = "<% getCfgGeneral(1, "HS_nai3_eap_method"); %>";	
	EAP3 = str.split(";");
	str = "<% getCfgGeneral(1, "HS_nai4_eap_method"); %>";	
	EAP4 = str.split(";");
	str = "<% getCfgGeneral(1, "HS_link_status"); %>";	
	LINKSTATUS = str.split(";");
	str = "<% getCfgGeneral(1, "HS_capacity"); %>";	
	CAPACITY = str.split(";");
	str = "<% getCfgGeneral(1, "HS_dl_speed"); %>";	
	DLSPEED = str.split(";");
	str = "<% getCfgGeneral(1, "HS_ul_speed"); %>";	
	ULSPEED = str.split(";");
	str = "<% getCfgGeneral(1, "HS_dl_load"); %>";	
	DLLOAD = str.split(";");
	str = "<% getCfgGeneral(1, "HS_ul_load"); %>";	
	ULLOAD = str.split(";");
	str = "<% getCfgGeneral(1, "HS_lmd"); %>";	
	LMD = str.split(";");
	str = "<% getCfgGeneral(1, "HS_plmn"); %>";	
	PLMN = str.split(";");
	str = "<% getCfgGeneral(1, "HS_plmn1_mcc"); %>";	
	MCC1 = str.split(";");
	str = "<% getCfgGeneral(1, "HS_plmn2_mcc"); %>";	
	MCC2 = str.split(";");
	str = "<% getCfgGeneral(1, "HS_plmn3_mcc"); %>";	
	MCC3 = str.split(";");
	str = "<% getCfgGeneral(1, "HS_plmn4_mcc"); %>";	
	MCC4 = str.split(";");
	str = "<% getCfgGeneral(1, "HS_plmn5_mcc"); %>";	
	MCC5 = str.split(";");
	str = "<% getCfgGeneral(1, "HS_plmn6_mcc"); %>";	
	MCC6 = str.split(";");
	str = "<% getCfgGeneral(1, "HS_plmn1_mnc"); %>";	
	MNC1 = str.split(";");
	str = "<% getCfgGeneral(1, "HS_plmn2_mnc"); %>";	
	MNC2 = str.split(";");
	str = "<% getCfgGeneral(1, "HS_plmn3_mnc"); %>";	
	MNC3 = str.split(";");
	str = "<% getCfgGeneral(1, "HS_plmn4_mnc"); %>";	
	MNC4 = str.split(";");
	str = "<% getCfgGeneral(1, "HS_plmn5_mnc"); %>";	
	MNC5 = str.split(";");
	str = "<% getCfgGeneral(1, "HS_plmn6_mnc"); %>";	
	MNC6 = str.split(";");
	str = "<% getCfgGeneral(1, "HS_proxy_arp"); %>";	
	PROXYARP = str.split(";");
	str = "<% getCfgGeneral(1, "HS_dgaf_disabled"); %>";	
	DGAF = str.split(";");
	str = "<% getCfgGeneral(1, "HS_l2_filter"); %>";	
	L2F = str.split(";");
	str = "<% getCfgGeneral(1, "HS_icmpv4_deny"); %>";	
	ICMP = str.split(";");
}

function checkMac(str){
	var len = str.length;
	if(len!=17)
		return false;

	for (var i=0; i<str.length; i++) {
		if((i%3) == 2){
			if(str.charAt(i) == ':')
				continue;
		}else{
			if (    (str.charAt(i) >= '0' && str.charAt(i) <= '9') ||
					(str.charAt(i) >= 'a' && str.charAt(i) <= 'f') ||
					(str.charAt(i) >= 'A' && str.charAt(i) <= 'F') )
			continue;
		}
		return false;
	}
	return true;
}

function checkInjection(str)
{
	var len = str.length;
	for (var i=0; i<str.length; i++) {
		if (str.charAt(i) == ';') {
			return false;
		} else {
			continue;
		}
	}
    return true;
}

function ""
{
	if (window.ActiveXObject) { // IE
		return "block";
	}
	else if (window.XMLHttpRequest) { // Mozilla, Safari,...
		return "table";
	}
}

function checkData()
{
	var idx;
	var realm_data;
	var mcc_data;
	var mnc_data;
	var input;

	if (document.hotspot_form.hessid.value != "") 
	{
		if (document.hotspot_form.hessid.value != "bssid" && checkMac(document.hotspot_form.hessid.value) == false) 
		{
			alert("please input correct HESSID");
			document.hotspot_form.hessid.select();
			return false;
		} 
		else if (checkInjection(document.hotspot_form.hessid.value) == false) 
		{
			alert("Don't include \";\" character");
			return false;
		}
	}
	
	if (checkInjection(document.hotspot_form.roaming_consortium_oi.value) == false) 
	{
		alert("Don't include \";\" character");
		return false;
	}
	
	if (document.hotspot_form.nai_enabled[0].checked == true) 
	{
		input = 0;
		for (idx=1; idx<4; idx++) 
		{
			realm_data = eval("document.hotspot_form.realm"+idx);
			if (realm_data.value != "") 
			{
				if (checkInjection(realm_data.value) == false) 
				{
					alert("Don't include \";\" character");
					return false;
				}
				input = 1;
			}
		}
		if (input == 0) 
		{
			alert("please input at least one set of NAI configuration!");
			document.hotspot_form.realm1.select();
			return false;
		}
	}
	
	if (document.hotspot_form.plmn_enabled[0].checked == true) 
	{
		input = 0;
		for (idx=1; idx<7; idx++) 
		{
			mcc_data = eval("document.hotspot_form.mcc"+idx);
			mnc_data = eval("document.hotspot_form.mnc"+idx);
			if (mcc_data.value != "" || mnc_data.value != "") 
			{
				if (mcc_data.value == "" || checkInjection(mcc_data.value) == false) 
				{
					alert("please input correct MCC and don't include \";\" character");
					mcc_data.select();
					return false;
				}
				if (mnc_data.value == "" || checkInjection(mnc_data.value) == false) 
				{
					alert("please input correct MNC and don't include \";\" character");
					mnc_data.select();
					return false;
				}
				input = 1;
			}
		}
		
		if (input == 0) 
		{
			alert("please input at least one set of 3GPP configuration!");
			document.hotspot_form.mcc1.select();
			return false;
		}
	}

	return true;
}

function submit_apply()
{
	if (1*WPS[selectedIF] > 0) 
	{
		alert("please turn off ra"+selectedIF+"\'s WPS!"); 
		return false;
	}

	if (AUTH[selectedIF] != "WPA2") 
	{
		alert("please change ra"+selectedIF+"\'s security to WPA2-Enterprise!"); 
		return false;
	}

	if (checkData() == true) 
	{
		document.hotspot_form.submit();
	}
}

function Load_Setting()
{
	var idx;

	alert("1.Please make sure security mode at WPA2 Enterprise before Hotspot configuration.\n2.Hotspot doesn't support WPS");
	QuerryValue();
	for (idx=0; idx<BssidNum; idx++) 
	{
		document.hotspot_form.wifiIndex.options[idx] = new Option("ra"+idx, idx);
	}
	document.hotspot_form.wifiIndex.selectedIndex = 0;
	switch_if();
}

function switch_l2f(on)
{
	if (on == 1) 
	{
		document.hotspot_form.l2_filter[0].checked = true;
		document.hotspot_form.icmpv4_deny[0].disabled = false;
		document.hotspot_form.icmpv4_deny[1].disabled = false;
		if (ICMP[selectedIF] == "1")
			document.hotspot_form.icmpv4_deny[0].checked = true;
		else
			document.hotspot_form.icmpv4_deny[1].checked = true;
	} 
	else 
	{
		document.hotspot_form.l2_filter[1].checked = true;
		document.hotspot_form.icmpv4_deny[0].disabled = true;
		document.hotspot_form.icmpv4_deny[1].disabled = true;
	}
}

function UpdateBasic()
{
	if (INTERNET[selectedIF] == "1")
		document.hotspot_form.internet[0].checked = true;
	else
		document.hotspot_form.internet[1].checked = true;
	
	if (HESSID[selectedIF] != "" && HESSID[selectedIF])
		document.hotspot_form.hessid.value = HESSID[selectedIF];
	else
		document.hotspot_form.hessid.value = "";
	
	if (ROAMING[selectedIF] != "" && ROAMING[selectedIF])
		document.hotspot_form.roaming_consortium_oi.value = ROAMING[selectedIF];
	else
		document.hotspot_form.roaming_consortium_oi.value = "";
	
	if (PROXYARP[selectedIF] == "1")
		document.hotspot_form.proxy_arp[0].checked = true;
	else
		document.hotspot_form.proxy_arp[1].checked = true;
	
	if (DGAF[selectedIF] == "1")
		document.hotspot_form.dgaf_disabled[0].checked = true;
	else
		document.hotspot_form.dgaf_disabled[1].checked = true;
	
	if (L2F[selectedIF] == "1")
		switch_l2f(1);
	else
		switch_l2f(0);
}

function swtich_nai(on)
{
	var idx;
	var realm_value;
	var eap_value;
	var realm;
	var eap;

	if (on == 1) 
	{
		document.hotspot_form.nai_enabled[0].checked = true;
		document.getElementById("div_nai_table").style.display = "";
	} 
	else 
	{
		document.hotspot_form.nai_enabled[1].checked = true;
		document.getElementById("div_nai_table").style.display = "none";
		for (idx=1; idx<5; idx++) 
		{
			realm = eval("document.hotspot_form.realm"+idx);
			eap = eval("document.hotspot_form.eap_method"+idx);
			realm.disabled = true;
			eap.disabled = true;
		}
		return;
	}

	for (idx=1; idx<5; idx++) 
	{
		realm_value = eval("REALM"+idx);
		eap_value = eval("EAP"+idx);
		realm = eval("document.hotspot_form.realm"+idx);
		eap = eval("document.hotspot_form.eap_method"+idx);
		realm.disabled = false;
		eap.disabled = false;

		if (realm_value[selectedIF] == "" || !realm_value[selectedIF]) 
		{
			realm.value = "";
			continue;
		}
		
		realm.value = realm_value[selectedIF];
		if (eap_value[selectedIF] == "eap-ttls/eap-tls")
			eap.selectedIndex = 2;
		else if (eap_value[selectedIF] == "eap-tls")
			eap.selectedIndex = 1;
		else
			eap.selectedIndex = 0;
	}
}

function swtich_plmn(on)
{
	var idx;
	var mcc_value;
	var mnc_value;
	var mcc;
	var mnc;

	if (on == 1) 
	{
		document.hotspot_form.plmn_enabled[0].checked = true;
		document.getElementById("div_plmn_table").style.display = "";
	} 
	else 
	{
		document.hotspot_form.plmn_enabled[1].checked = true;
		document.getElementById("div_plmn_table").style.display = "none";
		for (idx=1; idx<7; idx++) 
		{
			mcc = eval("document.hotspot_form.mcc"+idx);
			mnc = eval("document.hotspot_form.mnc"+idx);
			mcc.disabled = true;
			mnc.disabled = true;
		}
		return;
	}

	for (idx=1; idx<7; idx++) 
	{
		mcc_value = eval("MCC"+idx);
		mnc_value = eval("MNC"+idx);
		mcc = eval("document.hotspot_form.mcc"+idx);
		mnc = eval("document.hotspot_form.mnc"+idx);
		mcc.disabled = false;
		mnc.disabled = false;

		if (mcc_value[selectedIF] == "" || !mcc_value[selectedIF]) 
		{
			mcc.value = "";
			mnc.value = "";
			continue;
		}
		mcc.value = mcc_value[selectedIF];
		mnc.value = mnc_value[selectedIF];
	}
}

function UpdateWANMetrics()
{
	if (LINKSTATUS[selectedIF] != "" && LINKSTATUS[selectedIF])
		document.getElementById("link_status").innerHTML = LINKSTATUS[selectedIF];
	else
		document.getElementById("link_status").innerHTML = "0";
	
	if (CAPACITY[selectedIF] != "" && CAPACITY[selectedIF])
		document.getElementById("at_capacity").innerHTML = CAPACITY[selectedIF];
	else
		document.getElementById("at_capacity").innerHTML = "0";
	
	if (DLSPEED[selectedIF] != "" && DLSPEED[selectedIF])
		document.getElementById("dl_speed").innerHTML = DLSPEED[selectedIF];
	else
		document.getElementById("dl_speed").innerHTML = "0";
	
	if (ULSPEED[selectedIF] != "" && ULSPEED[selectedIF])
		document.getElementById("ul_speed").innerHTML = ULSPEED[selectedIF];
	else
		document.getElementById("ul_speed").innerHTML = "0";
	
	if (DLLOAD[selectedIF] != "" && DLLOAD[selectedIF])
		document.getElementById("dl_load").innerHTML = DLLOAD[selectedIF];
	else
		document.getElementById("dl_load").innerHTML = "0";
	
	if (ULLOAD[selectedIF] != "" && ULLOAD[selectedIF])
		document.getElementById("up_load").innerHTML = ULLOAD[selectedIF];
	else
		document.getElementById("up_load").innerHTML = "0";
	
	if (LMD[selectedIF] != "" && LMD[selectedIF])
		document.getElementById("lmd").innerHTML = LMD[selectedIF];
	else
		document.getElementById("lmd").innerHTML = "0";
}

function switch_hs(on)
{
	var idx;
	var mcc;
	var mnc;
	if (on == 1) 
	{
		document.hotspot_form.hs_enabled[0].checked = true;
		document.hotspot_form.internet[0].disabled = false;
		document.hotspot_form.internet[1].disabled = false;
		document.hotspot_form.hessid.disabled = false;
		document.hotspot_form.roaming_consortium_oi.disabled = false;
		document.hotspot_form.proxy_arp[0].disabled = false;
		document.hotspot_form.proxy_arp[1].disabled = false;
		document.hotspot_form.dgaf_disabled[0].disabled = false;
		document.hotspot_form.dgaf_disabled[1].disabled = false;
		document.hotspot_form.l2_filter[0].disabled = false;
		document.hotspot_form.l2_filter[1].disabled = false;
		document.hotspot_form.icmpv4_deny[0].disabled = false;
		document.hotspot_form.icmpv4_deny[1].disabled = false;
		document.hotspot_form.nai_enabled[0].disabled = false;
		document.hotspot_form.nai_enabled[1].disabled = false;
		document.hotspot_form.plmn_enabled[0].disabled = false;
		document.hotspot_form.plmn_enabled[1].disabled = false;
		UpdateBasic();
		
		if (NAI[selectedIF] == "1")
			swtich_nai(1);
		else
			swtich_nai(0);

		if (PLMN[selectedIF] == "1")
			swtich_plmn(1);
		else
			swtich_plmn(0);
	} 
	else 
	{
		document.hotspot_form.hs_enabled[1].checked = true;
		document.hotspot_form.internet[0].disabled = true;
		document.hotspot_form.internet[1].disabled = true;
		document.hotspot_form.hessid.disabled = true;
		document.hotspot_form.roaming_consortium_oi.disabled = true;
		document.hotspot_form.proxy_arp[0].disabled = true;
		document.hotspot_form.proxy_arp[1].disabled = true;
		document.hotspot_form.dgaf_disabled[0].disabled = true;
		document.hotspot_form.dgaf_disabled[1].disabled = true;
		document.hotspot_form.l2_filter[0].disabled = true;
		document.hotspot_form.l2_filter[1].disabled = true;
		document.hotspot_form.icmpv4_deny[0].disabled = true;
		document.hotspot_form.icmpv4_deny[1].disabled = true;
		document.hotspot_form.nai_enabled[0].disabled = true;
		document.hotspot_form.nai_enabled[1].disabled = true;
		document.hotspot_form.plmn_enabled[0].disabled = true;
		document.hotspot_form.plmn_enabled[1].disabled = true;
		swtich_nai(0);
		swtich_plmn(0);
	}
}

function switch_if()
{
	selectedIF = document.hotspot_form.wifiIndex.selectedIndex;
	if (HS[selectedIF] == "1") 
		switch_hs(1);
	else 
		switch_hs(0);
	
	UpdateWANMetrics();
}
</script>
</head>
<body onLoad="Load_Setting()" class="mainbody">
<blockquote>
<table width=700><tr><td>
<table width=100% border=0 cellpadding=3 cellspacing=1> 
<tr><td class="title">Hotspot Settings</td></tr>
<tr><td> </td></tr>
<tr><td><hr></td></tr>
</table>

<form method="post" name="hotspot_form" action="/goform/setHotspot">
<input type="hidden" name="selectedif">
<table width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr>
    <td class="thead">WiFi Interface choice</td>
    <td><select name="wifiIndex" size="1" onChange="switch_if()"></select></td>
  </tr>
  <tr>
    <td class="thead">Hotspot: </td>
    <td><input type="radio" name="hs_enabled" value="1" onClick="switch_hs(1)">Enable
    <input type="radio" name="hs_enabled" value="0" onClick="switch_hs(0)">Disable</td>
  </tr>
  <tr>
    <td class="thead">Internet Available:</td>
    <td><input type="radio" name="internet" value="1">Enable
    <input type="radio" name="internet" value="0">Disable</td>
  </tr>
  <tr>
    <td class="thead">HESSID:</td>
    <td><input type="text" name="hessid" maxlength="20">
    <br /><font color="#808080">ex:00:0C:43:38:83:00 or bssid<br />(bssid=AP own mac address)</font></td>
  </tr>
  <tr>
    <td class="thead">Roaming Consortium:</td>
    <td><input type="text" name="roaming_consortium_oi" maxlength="30">
    <br /><font color="#808080">ex:50-6F-9A,00-1B-C5-04-BD</font></td>
  </tr>
  <tr>
    <td class="thead">Proxy ARP serive:</td>
    <td><input type="radio" name="proxy_arp" value="1">Enable
      <input type="radio" name="proxy_arp" value="0">Disable</td>
  </tr>
  <tr>
    <td class="thead">Deactivation of Broadcast/Multicast Functionality:</td>
    <td><input type="radio" name="dgaf_disabled" value="1">Enable
      <input type="radio" name="dgaf_disabled" value="0">Disable</td>
  </tr>
  <tr>
    <td class="thead">L2 Traffic Inspection and Filtering:</td>
    <td><input type="radio" name="l2_filter" value="1" onClick="switch_l2f(1)">Enable
      <input type="radio" name="l2_filter" value="0" onClick="switch_l2f(0)">Disable
      <br />ICMPv4 Deny:
      <input type="radio" name="icmpv4_deny" value="1">Enable
      <input type="radio" name="icmpv4_deny" value="0">Disable</td>
  </tr>
</table>
<br />

<!-- NAI Realm List -->
<table width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr>
    <td class="thead">NAI Realm List: 
      <input type="radio" value="1" name="nai_enabled" onClick="swtich_nai(1)">Enable
      <input type="radio" value="0" name="nai_enabled" onClick="swtich_nai(0)">Disable
    </td>
  </tr>
</table>
<script language="javascript">
	var nai_idx;
	document.write("table width=100% border=0 cellpadding=3 cellspacing=1 id=div_nai_table>");
	for (nai_idx=1; nai_idx<5; nai_idx++) {
		document.write("<tr><td class=title2 colspan=2 id=\"naiSet"+nai_idx+"\">NAI Settings</td></tr>");
    	document.write("<tr><td class=thead id=\"naiRealm\">Realm:</td>");
    	document.write("<td><input type=text name=\"realm"+nai_idx+"\" id=\"realm"+nai_idx+"\" maxlength=\"64\"></td></tr>");
		document.write("<tr><td class=thead id=\"naiEAP\">EAP Method:</td>");
		document.write("<td><select name=\"eap_method"+nai_idx+"\" size=\"1\">");
		document.write("<option value=\"eap-ttls\">TTLS</option>");
		document.write("<option value=\"eap-tls\">TLS</option>");
		document.write("<option value=\"eap-ttls/eap-tls\">TTLS/TLS</option>");
		document.write("</select></td></tr>");
	}
	document.write("</table>");
</script>
<br />

<!-- 3GPP Cellular Network information -->
<table width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr>
    <td class="thead">3GPP Cellular Network information:
  	<input type="radio" value="1" name="plmn_enabled" onClick="swtich_plmn(1)">Enable
	<input type="radio" value="0" name="plmn_enabled" onClick="swtich_plmn(0)">Disabled
    </td>
  </tr>
</table>
<script language="javascript" type="text/javascript">
	var plmn_idx;
	document.write("<table width=100% border=0 cellpadding=3 cellspacing=1 id=div_plmn_table>");
	for (plmn_idx=1; plmn_idx<7; plmn_idx++) {
		document.write("<tr><td class=title2 colspan=2 id=\"plmnSet"+plmn_idx+"\">3GPP Settings</td></tr>");
    	document.write("<tr><td class=thead id=\"plmnMcc\">MCC:</td>");
    	document.write("<td><input type=text name=\"mcc"+plmn_idx+"\" id=\"mcc"+plmn_idx+"\" size=\"5\" maxlength=\"5\"></td></tr>");
		document.write("<tr><td class=thead id=\"plmnMnc\">MNC:</td>");
    	document.write("<td><input type=text name=\"mnc"+plmn_idx+"\" id=\"mnc"+plmn_idx+"\" size=\"5\" maxlength=\"5\"></td></tr>");
	}
	document.write("</table>");
</script>
<br />

<!-- WAN Metrics -->
<table width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr>
    <td class="title2" colspan="2">WAN Metrics<hr></td>
  </tr>
  <tr>
    <td class="thead">Link Status:</td>
    <td><span id="link_status"> </span></td>
  </tr>
  <tr>
    <td class="thead">Capacity:</td>
    <td><span id="at_capacity"> </span></td>
  </tr>
  <tr>
    <td class="thead">Downlink WAN Speed (kbps):</td>
    <td><span id="dl_speed"> </span></td>
  </tr>
  <tr>
    <td class="thead">Uplink WAN Speed (kbps):</td>
    <td><span id="ul_speed"> </span></td>
  </tr>
  <tr>
    <td class="thead">Downlink WAN Loading (%):</td>
    <td><span id="dl_load"> </span></td>
  </tr>
  <tr>
    <td class="thead">Uplink WAN Loading (%):</td>
    <td><span id="up_load"> </span></td>
  </tr>
  <tr>
    <td class="thead">Load Measure:</td>
    <td><span id="lmd"> </span></td>
  </tr>
</table>

<br />
<table width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr>
    <td>
      <input type="button" class="button" value="Apply" onClick="submit_apply()"> &nbsp; &nbsp;
      <input type="reset" class="button" value="Reset" onClick="window.location.reload()" >
    </td>
  </tr>
</table>
</form>

</td></tr></table>
</blockquote>
</body></html>
 
