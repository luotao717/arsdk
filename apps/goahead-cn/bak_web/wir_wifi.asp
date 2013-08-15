<html>
<head>
<title>WR922</title>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312" />
<link href="images/kingnet.css" type="text/css" rel="stylesheet">
<script language="JavaScript" type="text/javascript">
var basicRate = '<% getCfgZero(1, "BasicRate"); %>';
var bgProtection = '<% getCfgZero(1, "BGProtection"); %>';
//var dataRate = '<!--#include ssi=getWlanDataRate()-->';
var beaconInterval = '<% getCfgZero(1, "BeaconPeriod"); %>';
var dtimValue = '<% getCfgZero(1, "DtimPeriod"); %>';
var fragmentThreshold = '<% getCfgZero(1, "FragThreshold"); %>';
var rtsThreshold = '<% getCfgZero(1, "RTSThreshold"); %>';
var shortPreamble = '<% getCfgZero(1, "TxPreamble"); %>';
var shortSlot = '<% getCfgZero(1, "ShortSlot"); %>';
var txBurst = '<% getCfgZero(1, "TxBurst"); %>';
var pktAggregate = '<% getCfgZero(1, "PktAggregate"); %>';
var wmmCapable = '<% getCfgZero(1, "WmmCapable"); %>';
var APSDCapable = '<% getCfgZero(1, "APSDCapable"); %>';
var DLSCapable = '<% getCfgZero(1, "DLSCapable"); %>';
var wirelessMode  = '<% getCfgZero(1, "WirelessMode"); %>';
var ieee80211h  = '<% getCfgZero(1, "IEEE80211H"); %>';
var countrycode = '<% getCfgGeneral(1, "CountryCode"); %>';
var txPower = '<% getCfgZero(1, "TxPower"); %>';
var DLSBuilt = '<% getDLSBuilt(); %>';
var m2uBuilt = '<% getWlanM2UBuilt(); %>';
var m2uEnabled = '<% getCfgZero(1, "M2UEnabled"); %>';

function style_display_on()
{
	if (window.ActiveXObject)
	{ // IE
		return "block";
	}
	else if (window.XMLHttpRequest)
	{ // Mozilla, Safari,...
		return "table-row";
	}
}

function initValue()
{
	var datarateArray;
	wmm_capable_enable_switch();
	bgProtection = 1*bgProtection;
	document.wireless_advanced.bg_protection.options.selectedIndex = bgProtection;
	basicRate = 1*basicRate;

	if (basicRate == 3)
		document.wireless_advanced.basic_rate.options.selectedIndex = 0;
	else if (basicRate == 15)
		document.wireless_advanced.basic_rate.options.selectedIndex = 1;
	else if (basicRate == 351)
		document.wireless_advanced.basic_rate.options.selectedIndex = 2;

	wirelessMode = 1*wirelessMode;

	if ((wirelessMode == 2) || (wirelessMode == 8))
		document.wireless_advanced.basic_rate.disabled = true;
	else
		document.wireless_advanced.ieee_80211h.disabled = true;

	beaconInterval = 1*beaconInterval;
	document.wireless_advanced.beacon.value = beaconInterval;
	dtimValue = 1*dtimValue;
	document.wireless_advanced.dtim.value = dtimValue;
	fragmentThreshold = 1*fragmentThreshold;
	document.wireless_advanced.fragment.value = fragmentThreshold;
	rtsThreshold = 1*rtsThreshold;
	document.wireless_advanced.rts.value = rtsThreshold;

	shortPreamble = 1*shortPreamble;
	if (shortPreamble == 1)
	{
		document.wireless_advanced.short_preamble[0].checked = true;
		document.wireless_advanced.short_preamble[1].checked = false;
	}
	else
	{
		document.wireless_advanced.short_preamble[0].checked = false;
		document.wireless_advanced.short_preamble[1].checked = true;
	}

	shortSlot = 1*shortSlot;
	if (shortSlot == 1)
	{
		document.wireless_advanced.short_slot[0].checked = true;
	}
	else
	{
		document.wireless_advanced.short_slot[1].checked = true;
	}

	txBurst = 1*txBurst;
	if (txBurst == 1)
	{
		document.wireless_advanced.tx_burst[0].checked = true;
		document.wireless_advanced.tx_burst[1].checked = false;
	}
	else
	{
		document.wireless_advanced.tx_burst[0].checked = false;
		document.wireless_advanced.tx_burst[1].checked = true;
	}

	pktAggregate = 1*pktAggregate;
	if (pktAggregate == 1)
	{
		document.wireless_advanced.pkt_aggregate[0].checked = true;
		document.wireless_advanced.pkt_aggregate[1].checked = false;
	}
	else
	{
		document.wireless_advanced.pkt_aggregate[0].checked = false;
		document.wireless_advanced.pkt_aggregate[1].checked = true;
	}

	ieee80211h = 1*ieee80211h;
	if (ieee80211h == 1)
		document.wireless_advanced.ieee_80211h[0].checked = true;
	else
		document.wireless_advanced.ieee_80211h[1].checked = true;

	wmmCapable = 1*wmmCapable;
	if (wmmCapable == 1)
	{
		document.wireless_advanced.wmm_capable[0].checked = true;
		document.wireless_advanced.wmm_capable[1].checked = false;
	}
	else
	{
		document.wireless_advanced.wmm_capable[0].checked = false;
		document.wireless_advanced.wmm_capable[1].checked = true;
	}

	document.getElementById("div_apsd_capable").style.visibility = "hidden";
	document.getElementById("div_apsd_capable").style.display = "none";
	document.wireless_advanced.apsd_capable.disabled = true;
	document.getElementById("div_dls_capable").style.visibility = "hidden";
	document.getElementById("div_dls_capable").style.display = "none";
	document.wireless_advanced.dls_capable.disabled = true;
	//add by chenfei 2008-10-28
	document.getElementById("div_wifi_2").style.visibility = "hidden";
	document.getElementById("div_wifi_2").style.display = "none";
	document.getElementById("div_wifi_3").style.visibility = "hidden";
	document.getElementById("div_wifi_3").style.display = "none";
	document.getElementById("div_wifi_5").style.visibility = "hidden";
	document.getElementById("div_wifi_5").style.display = "none";
	document.getElementById("div_wifi_6").style.visibility = "hidden";
	document.getElementById("div_wifi_6").style.display = "none";

	DLSBuilt = 1*DLSBuilt;
	if (wmmCapable == 1)
	{
		document.getElementById("div_apsd_capable").style.visibility = "visible";
		document.getElementById("div_apsd_capable").style.display = style_display_on();
		document.wireless_advanced.apsd_capable.disabled = false;
		//add by chenfei 2008-10-28
		document.getElementById("div_wifi_2").style.visibility = "visible";
		document.getElementById("div_wifi_2").style.display = style_display_on();
		document.getElementById("div_wifi_3").style.visibility = "visible";
		document.getElementById("div_wifi_3").style.display = style_display_on();
		document.getElementById("div_wifi_5").style.visibility = "visible";
		document.getElementById("div_wifi_5").style.display = style_display_on();
		document.getElementById("div_wifi_6").style.visibility = "visible";
		document.getElementById("div_wifi_6").style.display = style_display_on();
		if (DLSBuilt == 1)
		{
			document.getElementById("div_dls_capable").style.visibility = "visible";
			document.getElementById("div_dls_capable").style.display = style_display_on();
			document.wireless_advanced.dls_capable.disabled = false;
		}
	}

	APSDCapable = 1*APSDCapable;
	if (APSDCapable == 1)
	{
		document.wireless_advanced.apsd_capable[0].checked = true;
		document.wireless_advanced.apsd_capable[1].checked = false;
	}
	else
	{
		document.wireless_advanced.apsd_capable[0].checked = false;
		document.wireless_advanced.apsd_capable[1].checked = true;
	}

	if (DLSBuilt == 1)
	{
		DLSCapable = 1*DLSCapable;
		if (DLSCapable == 1)
		{
			document.wireless_advanced.dls_capable[0].checked = true;
			document.wireless_advanced.dls_capable[1].checked = false;
		}
		else
		{
			document.wireless_advanced.dls_capable[0].checked = false;
			document.wireless_advanced.dls_capable[1].checked = true;
		}
	}
	document.wireless_advanced.tx_power.value = txPower;

	if (countrycode == "US")
		document.wireless_advanced.country_code.options.selectedIndex = 0;
	else if (countrycode == "JP")
		document.wireless_advanced.country_code.options.selectedIndex = 1;
	else if (countrycode == "FR")
		document.wireless_advanced.country_code.options.selectedIndex = 2;
	else if (countrycode == "TW")
		document.wireless_advanced.country_code.options.selectedIndex = 3;
	else if (countrycode == "IE")
		document.wireless_advanced.country_code.options.selectedIndex = 4;
	else if (countrycode == "HK")
		document.wireless_advanced.country_code.options.selectedIndex = 5;
	else if (countrycode == "NONE")
		document.wireless_advanced.country_code.options.selectedIndex = 6;
	else
		document.wireless_advanced.country_code.options.selectedIndex = 6;

	//multicase to unicast converter
	m2uBuilt = 1*m2uBuilt;
	document.getElementById("div_m2u").style.display = "none";
	if (m2uBuilt == 1) {
		if (window.ActiveXObject) // IE
			document.getElementById("div_m2u").style.display = "block";
		else
			document.getElementById("div_m2u").style.display = "table";

		m2uEnabled = 1*m2uEnabled;
		if (m2uEnabled == 1)
		{
			document.wireless_advanced.m2u_enable[0].checked = true;
			document.wireless_advanced.m2u_enable[1].checked = false;
		}
		else
		{
			document.wireless_advanced.m2u_enable[0].checked = false;
			document.wireless_advanced.m2u_enable[1].checked = true;
		}
	}
	var APAifsn    = '<% getCfgGeneral(1, "APAifsn"); %>';
   var APCwmin    = '<% getCfgGeneral(1, "APCwmin"); %>';
   var APCwmax    = '<% getCfgGeneral(1, "APCwmax"); %>';
   var APTxop     = '<% getCfgGeneral(1, "APTxop"); %>';
   var APACM      = '<% getCfgGeneral(1, "APACM"); %>';
   var BSSAifsn   = '<% getCfgGeneral(1, "BSSAifsn"); %>';
   var BSSCwmin   = '<% getCfgGeneral(1, "BSSCwmin"); %>';
   var BSSCwmax   = '<% getCfgGeneral(1, "BSSCwmax"); %>';
   var BSSTxop    = '<% getCfgGeneral(1, "BSSTxop"); %>';
   var BSSACM     = '<% getCfgGeneral(1, "BSSACM"); %>';
   var AckPolicy  = '<% getCfgGeneral(1, "AckPolicy"); %>';

   if (APAifsn   == "") APAifsn = "7;3;1;1";
   if (APCwmin   == "") APCwmin = "4;4;3;2";
   if (APCwmax   == "") APCwmax = "6;10;4;3";
   if (APTxop    == "") APTxop = "0;0;94;47";
   if (APACM     == "") APACM = "0;0;0;0";
   if (BSSAifsn  == "") BSSAifsn = "7;3;2;2";
   if (BSSCwmin  == "") BSSCwmin = "4;4;3;2";
   if (BSSCwmax  == "") BSSCwmax = "10;10;4;3";
   if (BSSTxop   == "") BSSTxop = "0;0;94;47";
   if (BSSACM    == "") BSSACM = "0;0;0;0";
   if (AckPolicy == "") AckPolicy = "0;0;0;0";

   var APAifsnArray   = APAifsn.split(";");
   var APCwminArray   = APCwmin.split(";");
   var APCwmaxArray   = APCwmax.split(";");
   var APTxopArray    = APTxop.split(";");
   var APACMArray     = APACM.split(";");
   var BSSAifsnArray  = BSSAifsn.split(";");
   var BSSCwminArray  = BSSCwmin.split(";");
   var BSSCwmaxArray  = BSSCwmax.split(";");
   var BSSTxopArray   = BSSTxop.split(";");
   var BSSACMArray    = BSSACM.split(";");
   var AckPolicyArray = AckPolicy.split(";");

   document.wireless_advanced.ap_aifsn_acbe.value = APAifsnArray[0];
   document.wireless_advanced.ap_aifsn_acbk.value = APAifsnArray[1];
   document.wireless_advanced.ap_aifsn_acvi.value = APAifsnArray[2];
   document.wireless_advanced.ap_aifsn_acvo.value = APAifsnArray[3];

   document.wireless_advanced.ap_cwmin_acbe.options.selectedIndex = 1*APCwminArray[0] - 1;
   document.wireless_advanced.ap_cwmin_acbk.options.selectedIndex = 1*APCwminArray[1] - 1;
   document.wireless_advanced.ap_cwmin_acvi.options.selectedIndex = 1*APCwminArray[2] - 1;
   document.wireless_advanced.ap_cwmin_acvo.options.selectedIndex = 1*APCwminArray[3] - 1;

   document.wireless_advanced.ap_cwmax_acbe.options.selectedIndex = 1*APCwmaxArray[0] - 1;
   document.wireless_advanced.ap_cwmax_acbk.options.selectedIndex = 1*APCwmaxArray[1] - 1;
   document.wireless_advanced.ap_cwmax_acvi.options.selectedIndex = 1*APCwmaxArray[2] - 1;
   document.wireless_advanced.ap_cwmax_acvo.options.selectedIndex = 1*APCwmaxArray[3] - 1;

   document.wireless_advanced.ap_txop_acbe.value = APTxopArray[0];
   document.wireless_advanced.ap_txop_acbk.value = APTxopArray[1];
   document.wireless_advanced.ap_txop_acvi.value = APTxopArray[2];
   document.wireless_advanced.ap_txop_acvo.value = APTxopArray[3];

   if (1*APACMArray[0] == 1)
      document.wireless_advanced.ap_acm_acbe.checked = true;
   else
      document.wireless_advanced.ap_acm_acbe.checked = false;

   if (1*APACMArray[1] == 1)
      document.wireless_advanced.ap_acm_acbk.checked = true;
   else
      document.wireless_advanced.ap_acm_acbk.checked = false;

   if (1*APACMArray[2] == 1)
      document.wireless_advanced.ap_acm_acvi.checked = true;
   else
      document.wireless_advanced.ap_acm_acvi.checked = false;

   if (1*APACMArray[3] == 1)
      document.wireless_advanced.ap_acm_acvo.checked = true;
   else
      document.wireless_advanced.ap_acm_acvo.checked = false;

   if (1*AckPolicyArray[0] == 1)
      document.wireless_advanced.ap_ackpolicy_acbe.checked = true;
   else
      document.wireless_advanced.ap_ackpolicy_acbe.checked = false;

   if (1*AckPolicyArray[1] == 1)
      document.wireless_advanced.ap_ackpolicy_acbk.checked = true;
   else
      document.wireless_advanced.ap_ackpolicy_acbk.checked = false;

   if (1*AckPolicyArray[2] == 1)
      document.wireless_advanced.ap_ackpolicy_acvi.checked = true;
   else
      document.wireless_advanced.ap_ackpolicy_acvi.checked = false;

   if (1*AckPolicyArray[3] == 1)
      document.wireless_advanced.ap_ackpolicy_acvo.checked = true;
   else
      document.wireless_advanced.ap_ackpolicy_acvo.checked = false;

   document.wireless_advanced.sta_aifsn_acbe.value = BSSAifsnArray[0];
   document.wireless_advanced.sta_aifsn_acbk.value = BSSAifsnArray[1];
   document.wireless_advanced.sta_aifsn_acvi.value = BSSAifsnArray[2];
   document.wireless_advanced.sta_aifsn_acvo.value = BSSAifsnArray[3];

   document.wireless_advanced.sta_cwmin_acbe.options.selectedIndex = 1*BSSCwminArray[0] - 1;
   document.wireless_advanced.sta_cwmin_acbk.options.selectedIndex = 1*BSSCwminArray[1] - 1;
   document.wireless_advanced.sta_cwmin_acvi.options.selectedIndex = 1*BSSCwminArray[2] - 1;
   document.wireless_advanced.sta_cwmin_acvo.options.selectedIndex = 1*BSSCwminArray[3] - 1;

   document.wireless_advanced.sta_cwmax_acbe.options.selectedIndex = 1*BSSCwmaxArray[0] - 1;
   document.wireless_advanced.sta_cwmax_acbk.options.selectedIndex = 1*BSSCwmaxArray[1] - 1;
   document.wireless_advanced.sta_cwmax_acvi.options.selectedIndex = 1*BSSCwmaxArray[2] - 1;
   document.wireless_advanced.sta_cwmax_acvo.options.selectedIndex = 1*BSSCwmaxArray[3] - 1;

   document.wireless_advanced.sta_txop_acbe.value = BSSTxopArray[0];
   document.wireless_advanced.sta_txop_acbk.value = BSSTxopArray[1];
   document.wireless_advanced.sta_txop_acvi.value = BSSTxopArray[2];
   document.wireless_advanced.sta_txop_acvo.value = BSSTxopArray[3];

   if (1*BSSACMArray[0] == 1)
      document.wireless_advanced.sta_acm_acbe.checked = true;
   else
      document.wireless_advanced.sta_acm_acbe.checked = false;

   if (1*BSSACMArray[1] == 1)
      document.wireless_advanced.sta_acm_acbk.checked = true;
   else
      document.wireless_advanced.sta_acm_acbk.checked = false;

   if (1*BSSACMArray[2] == 1)
      document.wireless_advanced.sta_acm_acvi.checked = true;
   else
      document.wireless_advanced.sta_acm_acvi.checked = false;

   if (1*BSSACMArray[3] == 1)
      document.wireless_advanced.sta_acm_acvo.checked = true;
   else
      document.wireless_advanced.sta_acm_acvo.checked = false;
}

function CheckValue()
{

	var APAifsn_s;
   var APCwmin_s;
   var APCwmax_s;
   var APTxop_s;
   var APACM_s;
   var BSSAifsn_s;
   var BSSCwmin_s;
   var BSSCwmax_s;
   var BSSTxop_s;
   var BSSACM_s;
   var AckPolicy_s;

   if (1*document.wireless_advanced.ap_aifsn_acbe.value < 1 || document.wireless_advanced.ap_aifsn_acbe.value > 15) {
     alert('AP AC_BE AIFSN out of range (1~15)!');
     return false;
   }
   if (1*document.wireless_advanced.ap_aifsn_acbk.value < 1 || document.wireless_advanced.ap_aifsn_acbk.value > 15) {
     alert('AP AC_BK AIFSN out of range (1~15)!');
     return false;
   }
   if (1*document.wireless_advanced.ap_aifsn_acvi.value < 1 || document.wireless_advanced.ap_aifsn_acvi.value > 15) {
     alert('AP AC_VI AIFSN out of range (1~15)!');
     return false;
   }
   if (1*document.wireless_advanced.ap_aifsn_acvo.value < 1 || document.wireless_advanced.ap_aifsn_acvo.value > 15) {
     alert('AP AC_VO AIFSN out of range (1~15)!');
     return false;
   }
   APAifsn_s = '';
   APAifsn_s = APAifsn_s + document.wireless_advanced.ap_aifsn_acbe.value;
   APAifsn_s = APAifsn_s + ';';
   APAifsn_s = APAifsn_s + document.wireless_advanced.ap_aifsn_acbk.value;
   APAifsn_s = APAifsn_s + ';';
   APAifsn_s = APAifsn_s + document.wireless_advanced.ap_aifsn_acvi.value;
   APAifsn_s = APAifsn_s + ';';
   APAifsn_s = APAifsn_s + document.wireless_advanced.ap_aifsn_acvo.value;
   document.wireless_advanced.ap_aifsn_all.value = APAifsn_s;
   document.wireless_advanced.ap_aifsn_acbe.disabled = true;
   document.wireless_advanced.ap_aifsn_acbk.disabled = true;
   document.wireless_advanced.ap_aifsn_acvi.disabled = true;
   document.wireless_advanced.ap_aifsn_acvo.disabled = true;

   APCwmin_s = '';
   APCwmin_s = APCwmin_s + document.wireless_advanced.ap_cwmin_acbe.value;
   APCwmin_s = APCwmin_s + ';';
   APCwmin_s = APCwmin_s + document.wireless_advanced.ap_cwmin_acbk.value;
   APCwmin_s = APCwmin_s + ';';
   APCwmin_s = APCwmin_s + document.wireless_advanced.ap_cwmin_acvi.value;
   APCwmin_s = APCwmin_s + ';';
   APCwmin_s = APCwmin_s + document.wireless_advanced.ap_cwmin_acvo.value;
   document.wireless_advanced.ap_cwmin_all.value = APCwmin_s;
   document.wireless_advanced.ap_cwmin_acbe.disabled = true;
   document.wireless_advanced.ap_cwmin_acbk.disabled = true;
   document.wireless_advanced.ap_cwmin_acvi.disabled = true;
   document.wireless_advanced.ap_cwmin_acvo.disabled = true;

   APCwmax_s = '';
   APCwmax_s = APCwmax_s + document.wireless_advanced.ap_cwmax_acbe.value;
   APCwmax_s = APCwmax_s + ';';
   APCwmax_s = APCwmax_s + document.wireless_advanced.ap_cwmax_acbk.value;
   APCwmax_s = APCwmax_s + ';';
   APCwmax_s = APCwmax_s + document.wireless_advanced.ap_cwmax_acvi.value;
   APCwmax_s = APCwmax_s + ';';
   APCwmax_s = APCwmax_s + document.wireless_advanced.ap_cwmax_acvo.value;
   document.wireless_advanced.ap_cwmax_all.value = APCwmax_s;
   document.wireless_advanced.ap_cwmax_acbe.disabled = true;
   document.wireless_advanced.ap_cwmax_acbk.disabled = true;
   document.wireless_advanced.ap_cwmax_acvi.disabled = true;
   document.wireless_advanced.ap_cwmax_acvo.disabled = true;

   APTxop_s = '';
   APTxop_s = APTxop_s + document.wireless_advanced.ap_txop_acbe.value;
   APTxop_s = APTxop_s + ';';
   APTxop_s = APTxop_s + document.wireless_advanced.ap_txop_acbk.value;
   APTxop_s = APTxop_s + ';';
   APTxop_s = APTxop_s + document.wireless_advanced.ap_txop_acvi.value;
   APTxop_s = APTxop_s + ';';
   APTxop_s = APTxop_s + document.wireless_advanced.ap_txop_acvo.value;
   document.wireless_advanced.ap_txop_all.value = APTxop_s;
   document.wireless_advanced.ap_txop_acbe.disabled = true;
   document.wireless_advanced.ap_txop_acbk.disabled = true;
   document.wireless_advanced.ap_txop_acvi.disabled = true;
   document.wireless_advanced.ap_txop_acvo.disabled = true;

   APACM_s = '';
   if (document.wireless_advanced.ap_acm_acbe.checked)
      APACM_s = APACM_s + '1';
   else
      APACM_s = APACM_s + '0';
   APACM_s = APACM_s + ';';
   if (document.wireless_advanced.ap_acm_acbk.checked)
      APACM_s = APACM_s + '1';
   else
      APACM_s = APACM_s + '0';
   APACM_s = APACM_s + ';';
   if (document.wireless_advanced.ap_acm_acvi.checked)
      APACM_s = APACM_s + '1';
   else
      APACM_s = APACM_s + '0';
   APACM_s = APACM_s + ';';
   if (document.wireless_advanced.ap_acm_acvo.checked)
      APACM_s = APACM_s + '1';
   else
      APACM_s = APACM_s + '0';
   document.wireless_advanced.ap_acm_acbe.disabled = true;
   document.wireless_advanced.ap_acm_acbk.disabled = true;
   document.wireless_advanced.ap_acm_acvi.disabled = true;
   document.wireless_advanced.ap_acm_acvo.disabled = true;
   document.wireless_advanced.ap_acm_all.value = APACM_s;

   AckPolicy_s = '';
   if (document.wireless_advanced.ap_ackpolicy_acbe.checked)
      AckPolicy_s = AckPolicy_s + '1';
   else
      AckPolicy_s = AckPolicy_s + '0';
   AckPolicy_s = AckPolicy_s + ';';
   if (document.wireless_advanced.ap_ackpolicy_acbk.checked)
      AckPolicy_s = AckPolicy_s + '1';
   else
      AckPolicy_s = AckPolicy_s + '0';
   AckPolicy_s = AckPolicy_s + ';';
   if (document.wireless_advanced.ap_ackpolicy_acvi.checked)
      AckPolicy_s = AckPolicy_s + '1';
   else
      AckPolicy_s = AckPolicy_s + '0';
   AckPolicy_s = AckPolicy_s + ';';
   if (document.wireless_advanced.ap_ackpolicy_acvo.checked)
      AckPolicy_s = AckPolicy_s + '1';
   else
      AckPolicy_s = AckPolicy_s + '0';
   document.wireless_advanced.ap_ackpolicy_acbe.disabled = true;
   document.wireless_advanced.ap_ackpolicy_acbk.disabled = true;
   document.wireless_advanced.ap_ackpolicy_acvi.disabled = true;
   document.wireless_advanced.ap_ackpolicy_acvo.disabled = true;
   document.wireless_advanced.ap_ackpolicy_all.value = AckPolicy_s;

   if (1*document.wireless_advanced.sta_aifsn_acbe.value < 1 || document.wireless_advanced.sta_aifsn_acbe.value > 15) {
     alert('Station AC_BE AIFSN out of range (1~15)!');
     return false;
   }
   if (1*document.wireless_advanced.sta_aifsn_acbk.value < 1 || document.wireless_advanced.sta_aifsn_acbk.value > 15) {
     alert('Station AC_BK AIFSN out of range (1~15)!');
     return false;
   }
   if (1*document.wireless_advanced.sta_aifsn_acvi.value < 1 || document.wireless_advanced.sta_aifsn_acvi.value > 15) {
     alert('Station AC_VI AIFSN out of range (1~15)!');
     return false;
   }
   if (1*document.wireless_advanced.sta_aifsn_acvo.value < 1 || document.wireless_advanced.sta_aifsn_acvo.value > 15) {
     alert('Station AC_VO AIFSN out of range (1~15)!');
     return false;
   }
   BSSAifsn_s = '';
   BSSAifsn_s = BSSAifsn_s + document.wireless_advanced.sta_aifsn_acbe.value;
   BSSAifsn_s = BSSAifsn_s + ';';
   BSSAifsn_s = BSSAifsn_s + document.wireless_advanced.sta_aifsn_acbk.value;
   BSSAifsn_s = BSSAifsn_s + ';';
   BSSAifsn_s = BSSAifsn_s + document.wireless_advanced.sta_aifsn_acvi.value;
   BSSAifsn_s = BSSAifsn_s + ';';
   BSSAifsn_s = BSSAifsn_s + document.wireless_advanced.sta_aifsn_acvo.value;
   document.wireless_advanced.sta_aifsn_all.value = BSSAifsn_s;
   document.wireless_advanced.sta_aifsn_acbe.disabled = true;
   document.wireless_advanced.sta_aifsn_acbk.disabled = true;
   document.wireless_advanced.sta_aifsn_acvi.disabled = true;
   document.wireless_advanced.sta_aifsn_acvo.disabled = true;

   BSSCwmin_s = '';
   BSSCwmin_s = BSSCwmin_s + document.wireless_advanced.sta_cwmin_acbe.value;
   BSSCwmin_s = BSSCwmin_s + ';';
   BSSCwmin_s = BSSCwmin_s + document.wireless_advanced.sta_cwmin_acbk.value;
   BSSCwmin_s = BSSCwmin_s + ';';
   BSSCwmin_s = BSSCwmin_s + document.wireless_advanced.sta_cwmin_acvi.value;
   BSSCwmin_s = BSSCwmin_s + ';';
   BSSCwmin_s = BSSCwmin_s + document.wireless_advanced.sta_cwmin_acvo.value;
   document.wireless_advanced.sta_cwmin_all.value = BSSCwmin_s;
   document.wireless_advanced.sta_cwmin_acbe.disabled = true;
   document.wireless_advanced.sta_cwmin_acbk.disabled = true;
   document.wireless_advanced.sta_cwmin_acvi.disabled = true;
   document.wireless_advanced.sta_cwmin_acvo.disabled = true;

   BSSCwmax_s = '';
   BSSCwmax_s = BSSCwmax_s + document.wireless_advanced.sta_cwmax_acbe.value;
   BSSCwmax_s = BSSCwmax_s + ';';
   BSSCwmax_s = BSSCwmax_s + document.wireless_advanced.sta_cwmax_acbk.value;
   BSSCwmax_s = BSSCwmax_s + ';';
   BSSCwmax_s = BSSCwmax_s + document.wireless_advanced.sta_cwmax_acvi.value;
   BSSCwmax_s = BSSCwmax_s + ';';
   BSSCwmax_s = BSSCwmax_s + document.wireless_advanced.sta_cwmax_acvo.value;
   document.wireless_advanced.sta_cwmax_all.value = BSSCwmax_s;
   document.wireless_advanced.sta_cwmax_acbe.disabled = true;
   document.wireless_advanced.sta_cwmax_acbk.disabled = true;
   document.wireless_advanced.sta_cwmax_acvi.disabled = true;
   document.wireless_advanced.sta_cwmax_acvo.disabled = true;

   BSSTxop_s = '';
   BSSTxop_s = BSSTxop_s + document.wireless_advanced.sta_txop_acbe.value;
   BSSTxop_s = BSSTxop_s + ';';
   BSSTxop_s = BSSTxop_s + document.wireless_advanced.sta_txop_acbk.value;
   BSSTxop_s = BSSTxop_s + ';';
   BSSTxop_s = BSSTxop_s + document.wireless_advanced.sta_txop_acvi.value;
   BSSTxop_s = BSSTxop_s + ';';
   BSSTxop_s = BSSTxop_s + document.wireless_advanced.sta_txop_acvo.value;
   document.wireless_advanced.sta_txop_all.value = BSSTxop_s;
   document.wireless_advanced.sta_txop_acbe.disabled = true;
   document.wireless_advanced.sta_txop_acbk.disabled = true;
   document.wireless_advanced.sta_txop_acvi.disabled = true;
   document.wireless_advanced.sta_txop_acvo.disabled = true;

   BSSACM_s = '';
   if (document.wireless_advanced.sta_acm_acbe.checked)
      BSSACM_s = BSSACM_s + '1';
   else
      BSSACM_s = BSSACM_s + '0';
   BSSACM_s = BSSACM_s + ';';
   if (document.wireless_advanced.sta_acm_acbk.checked)
      BSSACM_s = BSSACM_s + '1';
   else
      BSSACM_s = BSSACM_s + '0';
   BSSACM_s = BSSACM_s + ';';
   if (document.wireless_advanced.sta_acm_acvi.checked)
      BSSACM_s = BSSACM_s + '1';
   else
      BSSACM_s = BSSACM_s + '0';
   BSSACM_s = BSSACM_s + ';';
   if (document.wireless_advanced.sta_acm_acvo.checked)
      BSSACM_s = BSSACM_s + '1';
   else
      BSSACM_s = BSSACM_s + '0';
   document.wireless_advanced.sta_acm_acbe.disabled = true;
   document.wireless_advanced.sta_acm_acbk.disabled = true;
   document.wireless_advanced.sta_acm_acvi.disabled = true;
   document.wireless_advanced.sta_acm_acvo.disabled = true;
   document.wireless_advanced.sta_acm_all.value = BSSACM_s;

	if (document.wireless_advanced.beacon.value == "" )
	{
		alert('Beacon Interval 为空');
		document.wireless_advanced.beacon.focus();
		document.wireless_advanced.beacon.select();
		return false;
	}

	if (isNaN(document.wireless_advanced.beacon.value) || document.wireless_advanced.beacon.value < 20 || document.wireless_advanced.beacon.value > 999)
	{
		alert('无效的 Beacon Interval');
		document.wireless_advanced.beacon.focus();
		document.wireless_advanced.beacon.select();
		return false;
	}

	if (document.wireless_advanced.dtim.value == "" )
	{
		alert('DTIM Interval 不能为空');
		document.wireless_advanced.dtim.focus();
		document.wireless_advanced.dtim.select();
		return false;
	}

	if (isNaN(document.wireless_advanced.dtim.value) || document.wireless_advanced.dtim.value < 1 || document.wireless_advanced.dtim.value > 255)
	{
		alert('无效的 DTIM Interval');
		document.wireless_advanced.dtim.focus();
		document.wireless_advanced.dtim.select();
		return false;
	}

	if (document.wireless_advanced.fragment.value == "" )
	{
		alert('分片长度不能为空');
		document.wireless_advanced.fragment.focus();
		document.wireless_advanced.fragment.select();
		return false;
	}

	if (isNaN(document.wireless_advanced.fragment.value) || document.wireless_advanced.fragment.value < 1 || document.wireless_advanced.fragment.value > 2346)
	{
		alert('无效的分片长度');
		document.wireless_advanced.fragment.focus();
		document.wireless_advanced.fragment.select();
		return false;
	}

	if (document.wireless_advanced.rts.value == "" )
	{
		alert('RTS Threshold 不能为空');
		document.wireless_advanced.rts.focus();
		document.wireless_advanced.rts.select();
		return false;
	}

	if (isNaN(document.wireless_advanced.rts.value) || document.wireless_advanced.rts.value < 1 || document.wireless_advanced.rts.value > 2347)
	{
		alert('无效的 RTS Threshold');
		document.wireless_advanced.rts.focus();
		document.wireless_advanced.rts.select();
		return false;
	}

	if (document.wireless_advanced.ieee_80211h[0].checked == true)
	{
		if (1*ieee80211h == 0)
			document.wireless_advanced.rebootAP.value = 1;
	}
	else
	{
		if (1*ieee80211h == 1)
			document.wireless_advanced.rebootAP.value = 1;
	}

	DLSBuilt = 1*DLSBuilt;
	if (document.wireless_advanced.wmm_capable[0].checked == true)
	{
		if (1*wmmCapable == 0)
			document.wireless_advanced.rebootAP.value = 1;
	}
	else
	{
		document.wireless_advanced.apsd_capable[1].checked = true;
		if (DLSBuilt == 1)
		{
			document.wireless_advanced.dls_capable[1].checked = true;
		}

		if (1*wmmCapable == 1)
			document.wireless_advanced.rebootAP.value = 1;
	}

	if (document.wireless_advanced.apsd_capable[0].checked == true)
	{
		if (1*APSDCapable == 0)
			document.wireless_advanced.rebootAP.value = 1;
	}
	else
	{
		if (1*APSDCapable == 1)
			document.wireless_advanced.rebootAP.value = 1;
	}

	if (DLSBuilt == 1)
	{
		if (document.wireless_advanced.dls_capable[0].checked == true)
		{
			if (1*DLSCapable == 0)
				document.wireless_advanced.rebootAP.value = 1;
		}
		else
		{
			if (1*DLSCapable == 1)
				document.wireless_advanced.rebootAP.value = 1;
		}
	}

	return true;
}

function open_wmm_window()
{
	window.open("wir_wireless_advanced.asp","WMM_Parameters_LIst","toolbar=no, location=yes, scrollbars=yes, resizable=no, width=640, height=480")
}

function wmm_capable_enable_switch()
{
	document.getElementById("div_apsd_capable").style.visibility = "hidden";
	document.getElementById("div_apsd_capable").style.display = "none";
	document.getElementById("div_dls_capable").style.visibility = "hidden";
	document.getElementById("div_dls_capable").style.display = "none";
	document.wireless_advanced.apsd_capable.disabled = true;
	//add by chenfei 2008-10-28
	document.getElementById("div_wifi_2").style.visibility = "hidden";
	document.getElementById("div_wifi_2").style.display = "none";
	document.getElementById("div_wifi_3").style.visibility = "hidden";
	document.getElementById("div_wifi_3").style.display = "none";
	document.getElementById("div_wifi_5").style.visibility = "hidden";
	document.getElementById("div_wifi_5").style.display = "none";
	document.getElementById("div_wifi_6").style.visibility = "hidden";
	document.getElementById("div_wifi_6").style.display = "none";

	DLSBuilt = 1*DLSBuilt;
	if (DLSBuilt == 1)
	{
		document.wireless_advanced.dls_capable.disabled = true;
	}

	if (document.wireless_advanced.wmm_capable[0].checked == true)
	{
		document.getElementById("div_apsd_capable").style.visibility = "visible";
		document.getElementById("div_apsd_capable").style.display = style_display_on();
		document.wireless_advanced.apsd_capable.disabled = false;
		//add by chenfei 2008-10-28
		document.getElementById("div_wifi_2").style.visibility = "visible";
		document.getElementById("div_wifi_2").style.display = style_display_on();
		document.getElementById("div_wifi_3").style.visibility = "visible";
		document.getElementById("div_wifi_3").style.display = style_display_on();		
		document.getElementById("div_wifi_5").style.visibility = "visible";
		document.getElementById("div_wifi_5").style.display = style_display_on();
		document.getElementById("div_wifi_6").style.visibility = "visible";
		document.getElementById("div_wifi_6").style.display = style_display_on();
		if (DLSBuilt == 1)
		{
			document.getElementById("div_dls_capable").style.visibility = "visible";
			document.getElementById("div_dls_capable").style.display = style_display_on();
			document.wireless_advanced.dls_capable.disabled = false;
		}
	}
}
</script>
</head>
<body onLoad="initValue()">
<form method=post name=wireless_advanced action="/goform/wirelessWifi" onSubmit="return CheckValue()">
<table width="90%" border="0" cellpadding="0" cellspacing="0" align="center">
	<tr>
    	<td style="padding-top:30px;">
	   		<table width="100%" border="0" cellpadding="0" cellspacing="0">
          		<tr>
            		<td class="nav">当前路径:KN-WR922 &gt;&gt;无线设置 &gt;&gt;WIFI 多媒体</td>
          		</tr>
				<tr>
				 	<td>&nbsp;</td>
				</tr>
				<tr>
          <td class="space" style="padding-left:10px; padding-right:10px;">&nbsp;&nbsp;&nbsp;本页面提供了对无线多媒体参数的设置,wmm 允许无线通信根据数据类型定义一个优先级范围。时间敏感的数据，如视频/音频数据将比普通的数据有更高的优先级。为了使wmm 功能工作，无线客户端必须也支持wmm。客户可以根据需求选择是或否。</td>
       </tr>
	   	<tr>
        	<td>&nbsp;</td>
      	</tr>
          		<tr style="display:none; visibility:hidden">
            		<td class="titlebg" id="advWireless">无线高级设置</td>
          		</tr>
          		<tr style="display:none; visibility:hidden">
            		<td>
			  			<table class="space" width="100%" border="0" cellspacing="0" cellpadding="0">
                			<tr>
                  				<td width="20%" class="contentpadding" id="advBGProtect">BG 保护模式:</td>
                  				<td width="80%"><select name="bg_protection" size="1">
										<option value=0 selected id="advBGProAuto">自动</option>
										<option value=1 id="advBGProOn">开</option>
										<option value=2 id="advBGProOff">关</option></select></td>
							</tr>
							<tr>
							  <td class="contentpadding" id="advBasicDtRt">基本数据传输速率:</td>
							  <td><select name="basic_rate" size="1">
				  						<option value="3" >1-2 Mbps</option>
										<option value="15" id="advBasicDtRtDefault">默认(1-2-5.5-11 Mbps)</option>
										<option value="351" id="advBasicDtRtAll">完整(1-2-5.5-6-11-12-24 Mbps)</option></select></td>
							</tr>
							<tr>
							  <td class="contentpadding" id="advBeaconInterval">信标间隔:</td>
							  <td><input type=text name=beacon size=5 maxlength=3 value="100"> ms <font color="#808080">(范围 20 - 999, 默认 100)</font></td>
							</tr>
							<tr>
							  <td class="contentpadding" id="advDTIM">数据信标比例(DTIM):</td>
							  <td><input type=text name=dtim size=5 maxlength=3 value="1"> ms <font color="#808080">(范围 1 - 255, 默认 1)</font> </td>
							</tr>
							<tr>
							  <td class="contentpadding" id="advFrag">分片域值:</td>
							  <td><input type=text name=fragment size=5 maxlength=4 value=""> <font color="#808080">(范围 256 - 2346, 默认 2346)</font></td>
							</tr>
							<tr>
							  <td class="contentpadding">RTS 域值:</td>
							  <td><input type=text name=rts size=5 maxlength=4 value=""> <font color="#808080">(范围 1 - 2347, 默认 2347)</font></td>
							</tr>
							<tr>
							  <td class="contentpadding">发射功率 (SSID):</td>
							  <td><input type=text name=tx_power size=5 maxlength=3 value="100"> <font color="#808080">(范围 1 - 100, 默认 100)</font></td>
							</tr>
							<tr>
							  <td class="contentpadding">短前导帧:</td>
							  <td><input type=radio name=short_preamble value="1" checked>启用&nbsp;<input type=radio name=short_preamble value="0">禁用</td>
							</tr>
							<tr> 
							  <td class="contentpadding">短碰撞槽:</td>
							  <td><input type=radio name=short_slot value="1" checked>启用&nbsp;<input type=radio name=short_slot value="0">禁用</td>
							</tr>
							<tr> 
							  <td class="contentpadding">传输突发:</td>
							  <td><input type=radio name=tx_burst value="1" checked>启用&nbsp;<input type=radio name=tx_burst value="0">禁用</td>
							</tr>
							<tr> 
							  <td class="contentpadding">数据包聚合:</td>
							  <td><input type=radio name=pkt_aggregate value="1">启用&nbsp;<input type=radio name=pkt_aggregate value="0" checked>禁用</td>
							</tr>
							<tr> 
							  <td class="contentpadding">IEEE 802.11H:</td>
							  <td><input type=radio name=ieee_80211h value="1">启用&nbsp;<input type=radio name=ieee_80211h value="0" checked>禁用 <font color="#808080">(只用于A 模式)</font>				  </td>
							</tr>
							<tr> 
							  <td class="contentpadding">国家代码:</td>
							  <td><select name="country_code">
                          <option value="US">US (United States)</option>
                          <option value="JP">JP (Japan)</option>
                          <option value="FR">FR (France)</option>
                          <option value="TW">TW (Taiwan)</option>
                          <option value="IE">IE (Ireland)</option>
                          <option value="HK">HK (Hong Kong)</option>
                          <option value="NONE" selected>NONE</option>
		  			  </select></td>
							</tr>
              			</table>
				  </td>
          		</tr>
		  		<tr>
            		<td class="titlebg">WIFI 多媒体设置</td>
          		</tr>
		 		<tr>
            		<td>
					  <table class="space" width="100%" border="0" cellspacing="0" cellpadding="0">
						<tr>
						  <td width="20%" class="contentpadding">WIFI多媒体WMM:</td>
						  <td width="80%"><input type=radio name=wmm_capable value="1" onClick="wmm_capable_enable_switch()" checked>启用&nbsp;<input type=radio name=wmm_capable value="0" onClick="wmm_capable_enable_switch()">禁用</td>
						</tr>
						<tr id="div_apsd_capable" name="div_apsd_capable">
						  <td class="contentpadding">自动省电模式:</td>
						  <td><input type=radio name=apsd_capable value="1">启用&nbsp;<input type=radio name=apsd_capable value="0" checked>禁用</td>
						</tr>
						  <tr id="div_dls_capable" name="div_dls_capable">
							<td class="contentpadding" id="advDLS">DLS Capable:</td>
							<td><input type=radio name=dls_capable value="1">启用&nbsp;<input type=radio name=dls_capable value="0" checked>禁用</td>
						</tr>
						<tr style="display:none; visibility:hidden">
						  <td class="contentpadding">WMM 参数:</td>
						  <td><input type=button class="button" name="wmm_list" value=" 配置WMM " onClick="open_wmm_window()"></td>
						</tr>
					  </table>
					</td>
          		</tr><input type="hidden" name="rebootAP" value="0">
		  		<tr style="display:none; visibility:hidden">
            		<td class="titlebg">多播对单播</td>
          		</tr>
		  		<tr style="display:none; visibility:hidden">
            		<td>
			  			<table class="space" width="100%" border="0" cellspacing="0" cellpadding="0">
						  	<tr id="div_m2u">
							  <td width="30%" class="contentpadding">多播对单播:</td>
							  <td width="70%"><input type=radio name="m2u_enable" value="1">启用&nbsp;<input type=radio name="m2u_enable" value="0">禁用</td>
							</tr>
			  			</table>
					</td>
				</tr>
				  <tr id="div_wifi_2">
					<td class="titlebg">接入点WMM参数</td>
				  </tr>
				  <tr id="div_wifi_3">
					<td>
					  <table class="space" width="100%" border="1" cellspacing="0" cellpadding="0" align="center" bordercolor="#bfe8fe" >
						<tr>
						  <td>&nbsp;</td>
						  <td>Aifsn</td>
						  <td>CWMin</td>
						  <td>CWMax</td>
						  <td>Txop</td>
						  <td>ACM</td>
						  <td>AckPolicy</td>
						</tr>
						<tr>
						  <td class="contentpadding">AC_BE</td>
						  <td><input type=text name=ap_aifsn_acbe size=4 maxlength=4 value=""></td>
						  <td><select name="ap_cwmin_acbe" size="1">
			  <option value=1>1</option>
			  <option value=2>3</option>
			  <option value=3>7</option>
			  <option value=4 selected>15</option>
			</select></td>
						  <td><select name="ap_cwmax_acbe" size="1">
			  <option value=1>1</option>
			  <option value=2>3</option>
			  <option value=3>7</option>
			  <option value=4>15</option>
			  <option value=5>31</option>
			  <option value=6 selected>63</option>
			</select></td>
						  <td><input type=text name=ap_txop_acbe size=4 maxlength=4 value=""></td>
						  <td><input type=checkbox name=ap_acm_acbe value="1"></td>
						  <td><input type=checkbox name=ap_ackpolicy_acbe value="1"></td>
						</tr>
						<input type=hidden name=ap_aifsn_all>
			<input type=hidden name=ap_cwmin_all>
			<input type=hidden name=ap_cwmax_all>
			<input type=hidden name=ap_txop_all>
			<input type=hidden name=ap_acm_all>
			<input type=hidden name=ap_ackpolicy_all>
						<tr>
						  <td class="contentpadding">AC_BK</td>
						  <td><input type=text name=ap_aifsn_acbk size=4 maxlength=4 value=""></td>
						  <td><select name="ap_cwmin_acbk" size="1">
			  <option value=1>1</option>
			  <option value=2>3</option>
			  <option value=3>7</option>
			  <option value=4 selected>15</option>
			</select></td>
						  <td><select name="ap_cwmax_acbk" size="1">
			  <option value=1>1</option>
			  <option value=2>3</option>
			  <option value=3>7</option>
			  <option value=4>15</option>
			  <option value=5>31</option>
			  <option value=6>63</option>
			  <option value=7>127</option>
			  <option value=8>255</option>
			  <option value=9>511</option>
			  <option value=10 selected>1023</option>
			</select></td>
						  <td><input type=text name=ap_txop_acbk size=4 maxlength=4 value=""></td>
						  <td><input type=checkbox name=ap_acm_acbk value="1"></td>
						  <td><input type=checkbox name=ap_ackpolicy_acbk value="1"></td>
						</tr>
						<tr>
						  <td class="contentpadding">AC_VI</td>
						  <td><input type=text name=ap_aifsn_acvi size=4 maxlength=4 value=""></td>
						  <td><select name="ap_cwmin_acvi" size="1">
			  <option value=1>1</option>
			  <option value=2>3</option>
			  <option value=3 selected>7</option>
			</select></td>
						  <td><select name="ap_cwmax_acvi" size="1">
			  <option value=1>1</option>
			  <option value=2>3</option>
			  <option value=3>7</option>
			  <option value=4 selected>15</option>
			</select></td>
						  <td><input type=text name=ap_txop_acvi size=4 maxlength=4 value=""></td>
						  <td><input type=checkbox name=ap_acm_acvi value="1"></td>
						  <td><input type=checkbox name=ap_ackpolicy_acvi value="1"></td>
						</tr>
						<tr>
						  <td class="contentpadding">AC_VO</td>
						  <td><input type=text name=ap_aifsn_acvo size=4 maxlength=4 value=""></td>
						  <td><select name="ap_cwmin_acvo" size="1">
			  <option value=1>1</option>
			  <option value=2 selected>3</option>
			</select></td>
						  <td><select name="ap_cwmax_acvo" size="1">
			  <option value=1>1</option>
			  <option value=2>3</option>
			  <option value=3 selected>7</option>
			</select></td>
						  <td><input type=text name=ap_txop_acvo size=4 maxlength=4 value=""></td>
						  <td><input type=checkbox name=ap_acm_acvo value="1"></td>
						  <td><input type=checkbox name=ap_ackpolicy_acvo value="1"></td>
						</tr>
					  </table>
					</td>
				  </tr>
				  <tr id="div_wifi_5">
					<td class="titlebg">站点的WMM参数</td>
				  </tr>
				  <tr id="div_wifi_6">
					<td>
					  <table class="space" width="100%" border="1" cellspacing="0" cellpadding="0" align="center" bordercolor="#bfe8fe" >
						<tr>
						  <td>&nbsp;</td>
						  <td>Aifsn</td>
						  <td>CWMin</td>
						  <td>CWMax</td>
						  <td>Txop</td>
						  <td>ACM</td>
						</tr>
						<tr>
						  <td class="contentpadding">AC_BE</td>
						  <td><input type=text name=sta_aifsn_acbe size=4 maxlength=4 value=""></td>
						  <td><select name="sta_cwmin_acbe" size="1">
			  <option value=1>1</option>
			  <option value=2>3</option>
			  <option value=3>7</option>
			  <option value=4 selected>15</option>
			</select></td>
						  <td><select name="sta_cwmax_acbe" size="1">
			  <option value=1>1</option>
			  <option value=2>3</option>
			  <option value=3>7</option>
			  <option value=4>15</option>
			  <option value=5>31</option>
			  <option value=6>63</option>
			  <option value=7>127</option>
			  <option value=8>255</option>
			  <option value=9>511</option>
			  <option value=10 selected>1023</option>
			</select></td>
						  <td><input type=text name=sta_txop_acbe size=4 maxlength=4 value=""></td>
						  <td><input type=checkbox name=sta_acm_acbe value="1"></td>
						</tr>
						 <input type=hidden name=sta_aifsn_all>
			<input type=hidden name=sta_cwmin_all>
			<input type=hidden name=sta_cwmax_all>
			<input type=hidden name=sta_txop_all>
			<input type=hidden name=sta_acm_all>
						<tr>
						  <td class="contentpadding">AC_BK</td>
						  <td><input type=text name=sta_aifsn_acbk size=4 maxlength=4 value=""></td>
						  <td><select name="sta_cwmin_acbk" size="1">
			  <option value=1>1</option>
			  <option value=2>3</option>
			  <option value=3>7</option>
			  <option value=4 selected>15</option>
			</select></td>
						  <td><select name="sta_cwmax_acbk" size="1">
			  <option value=1>1</option>
			  <option value=2>3</option>
			  <option value=3>7</option>
			  <option value=4>15</option>
			  <option value=5>31</option>
			  <option value=6>63</option>
			  <option value=7>127</option>
			  <option value=8>255</option>
			  <option value=9>511</option>
			  <option value=10 selected>1023</option>
			</select></td>
						  <td><input type=text name=sta_txop_acbk size=4 maxlength=4 value=""></td>
						  <td><input type=checkbox name=sta_acm_acbk value="1"></td>
						</tr>
						
						<tr>
						  <td class="contentpadding">AC_VI</td>
						  <td><input type=text name=sta_aifsn_acvi size=4 maxlength=4 value=""></td>
						  <td><select name="sta_cwmin_acvi" size="1">
			  <option value=1>1</option>
			  <option value=2>3</option>
			  <option value=3 selected>7</option>
			</select></td>
						  <td><select name="sta_cwmax_acvi" size="1">
			  <option value=1>1</option>
			  <option value=2>3</option>
			  <option value=3>7</option>
			  <option value=4 selected>15</option>
			</select></td>
						  <td><input type=text name=sta_txop_acvi size=4 maxlength=4 value=""></td>
						  <td><input type=checkbox name=sta_acm_acvi value="1"></td>
						</tr>
						<tr>
						  <td class="contentpadding">AC_VO</td>
						  <td><input type=text name=sta_aifsn_acvo size=4 maxlength=4 value=""></td>
						  <td><select name="sta_cwmin_acvo" size="1">
			  <option value=1>1</option>
			  <option value=2 selected>3</option>
			</select></td>
						  <td><select name="sta_cwmax_acvo" size="1">
			  <option value=1>1</option>
			  <option value=2>3</option>
			  <option value=3 selected>7</option>
			</select></td>
						  <td><input type=text name=sta_txop_acvo size=4 maxlength=4 value=""></td>
						  <td><input type=checkbox name=sta_acm_acvo value="1"></td>
						</tr>
					  </table>
					</td>
				  </tr>
				<tr>
            		<td>&nbsp;</td>
          		</tr>
          		<tr>
            		<td class="contentpadding"><input type="hidden" value="/wir_wifi.asp" name="submit-url"><input type="submit" class="button" value="提 交" />&nbsp;&nbsp;<input type="reset" class="button" value="取 消" onClick="window.location.reload()"></td>
          		</tr>
				<tr>
            		<td>&nbsp;</td>
          		</tr>
	  		</table>
	 	</td>
  	</tr>
  	<script language="javascript"> wmm_capable_enable_switch();
  	</script>
</table>
</form>
</body>
</html>