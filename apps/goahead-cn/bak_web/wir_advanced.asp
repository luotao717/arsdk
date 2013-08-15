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
var PhyMode  = '<% getCfgZero(1, "WirelessMode"); %>';
var ht_mode = '<% getCfgZero(1, "HT_OpMode"); %>';
var ht_bw = '<% getCfgZero(1, "HT_BW"); %>';
var ht_gi = '<% getCfgZero(1, "HT_GI"); %>';
var ht_stbc = '<% getCfgZero(1, "HT_STBC"); %>';
var ht_mcs = '<% getCfgZero(1, "HT_MCS"); %>';
var ht_htc = '<% getCfgZero(1, "HT_HTC"); %>';
var ht_rdg = '<% getCfgZero(1, "HT_RDG"); %>';
//var ht_linkadapt = '<% getCfgZero(1, "HT_LinkAdapt"); %>';
var ht_extcha = '<% getCfgZero(1, "HT_EXTCHA"); %>';
var ht_amsdu = '<% getCfgZero(1, "HT_AMSDU"); %>';
var ht_autoba = '<% getCfgZero(1, "HT_AutoBA"); %>';
var ht_badecline = '<% getCfgZero(1, "HT_BADecline"); %>';
var ht_f_40mhz = '<% getCfgZero(1, "HT_40MHZ_INTOLERANT"); %>';
//var wifi_optimum = '<!--#include ssi=getWlanWiFiTest()-->';
//var apcli_include = '<!--#include ssi=getWlanApCliInclude()-->';
//var wifi_optimum = '0';
var apcli_include = '<% getWlanApcliBuilt(); %>';
var tx_stream_idx = '<% getCfgZero(1, "HT_TxStream"); %>';
var rx_stream_idx = '<% getCfgZero(1, "HT_RxStream"); %>';

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

	bgProtection = 1*bgProtection;
	document.wireless_advanced.bg_protection.options.selectedIndex = bgProtection;
	basicRate = 1*basicRate;

	if (1*ht_bw == 0)
	{
		document.wireless_advanced.n_bandwidth[0].checked = true;
		document.getElementById("extension_channel").style.visibility = "hidden";
		document.getElementById("extension_channel").style.display = "none";
		document.wireless_advanced.n_extcha.disabled = true;
	}
	else
	{
		document.wireless_advanced.n_bandwidth[1].checked = true;
		document.getElementById("extension_channel").style.visibility = "visible";
		document.getElementById("extension_channel").style.display = style_display_on();
		document.wireless_advanced.n_extcha.disabled = false;
	}
	//end
	
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

	DLSBuilt = 1*DLSBuilt;
	if (wmmCapable == 1)
	{
		document.getElementById("div_apsd_capable").style.visibility = "visible";
		document.getElementById("div_apsd_capable").style.display = style_display_on();
		document.wireless_advanced.apsd_capable.disabled = false;
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

	if (1*ht_mode == 0)
		document.wireless_advanced.n_mode[0].checked = true;
	else
		document.wireless_advanced.n_mode[1].checked = true;


	if (1*ht_gi == 0)
		document.wireless_advanced.n_gi[0].checked = true;
	else
		document.wireless_advanced.n_gi[1].checked = true;

	if (1*ht_mcs <= 15)
		document.wireless_advanced.n_mcs.options.selectedIndex = ht_mcs;
	else if (1*ht_mcs == 32)
		document.wireless_advanced.n_mcs.options.selectedIndex = 16;
	else if (1*ht_mcs == 33)
		document.wireless_advanced.n_mcs.options.selectedIndex = 17;

	if (1*ht_rdg == 0)
		document.wireless_advanced.n_rdg[0].checked = true;
	else
		document.wireless_advanced.n_rdg[1].checked = true;

	var option_length = document.wireless_advanced.n_extcha.options.length;

	if (1*ht_extcha == 0)
	{
		if (option_length > 1)
			document.wireless_advanced.n_extcha.options.selectedIndex = 0;
	}
	else if (1*ht_extcha == 1)
	{
		if (option_length > 1)
			document.wireless_advanced.n_extcha.options.selectedIndex = 1;
	}
	else
	{
		document.wireless_advanced.n_extcha.options.selectedIndex = 0;
	}

	if (1*ht_amsdu == 0)
		document.wireless_advanced.n_amsdu[0].checked = true;
	else
		document.wireless_advanced.n_amsdu[1].checked = true;

	if (1*ht_autoba == 0)
		document.wireless_advanced.n_autoba[0].checked = true;
	else
		document.wireless_advanced.n_autoba[1].checked = true;

	if (1*ht_badecline == 0)
		document.wireless_advanced.n_badecline[0].checked = true;
	else
		document.wireless_advanced.n_badecline[1].checked = true;

	//if (1*ht_f_40mhz == 0)
		//document.wireless_advanced.f_40mhz[0].checked = true;
	//else
		//document.wireless_advanced.f_40mhz[1].checked = true;

	/*
	if (1*wifi_optimum == 0)
		document.wireless_advanced.wifi_opt[0].checked = true;
	else
		document.wireless_advanced.wifi_opt[1].checked = true;
	*/


	document.wireless_advanced.rx_stream.options.selectedIndex = rx_stream_idx - 1;
	document.wireless_advanced.tx_stream.options.selectedIndex = tx_stream_idx - 1;
	if (1*PhyMode >= 8)
	{	
		document.getElementById("div_11n").style.visibility = "visible";
		document.getElementById("div_11n").style.display = style_display_on();
		document.wireless_advanced.n_mode.disabled = false;
		document.wireless_advanced.n_bandwidth.disabled = false;
		document.wireless_advanced.n_rdg.disabled = false;
		document.wireless_advanced.n_gi.disabled = false;
		document.wireless_advanced.n_mcs.disabled = false;
	}
	else
	{
		document.getElementById("div_11n").style.visibility = "hidden";
		document.getElementById("div_11n").style.display = "none";
		document.wireless_advanced.n_mode.disabled = true;
		document.wireless_advanced.n_bandwidth.disabled = true;
		document.wireless_advanced.n_rdg.disabled = true;
		document.wireless_advanced.n_gi.disabled = true;
		document.wireless_advanced.n_mcs.disabled = true;
	}
}

function CheckValue()
{

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
function Channel_BandWidth_onClick()
{

	if (document.wireless_advanced.n_bandwidth[0].checked == true)
	{
		document.getElementById("extension_channel").style.visibility = "hidden";
		document.getElementById("extension_channel").style.display = "none";
		document.wireless_advanced.n_extcha.disabled = true;
	}
	else
	{
		document.getElementById("extension_channel").style.visibility = "visible";
		document.getElementById("extension_channel").style.display = style_display_on();
		document.wireless_advanced.n_extcha.disabled = false;
	}
}

function open_wmm_window()
{
	window.open("wir_wmm.asp","WMM_Parameters_LIst","toolbar=no, location=yes, scrollbars=yes, resizable=no, width=640, height=480")
}

function wmm_capable_enable_switch()
{
	document.getElementById("div_apsd_capable").style.visibility = "hidden";
	document.getElementById("div_apsd_capable").style.display = "none";
	document.getElementById("div_dls_capable").style.visibility = "hidden";
	document.getElementById("div_dls_capable").style.display = "none";
	document.wireless_advanced.apsd_capable.disabled = true;

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
<form method=post name=wireless_advanced action="/goform/wirelessAdvanced" onSubmit="return CheckValue()">
<table width="92%" border="0" cellpadding="0" cellspacing="0" align="center">
  <tr>
    <td style="padding-top:30px;">
	   <table width="100%" border="0" cellpadding="0" cellspacing="0">
          <tr>
            <td class="nav">当前路径:KN-WR922 &gt;&gt;无线设置 &gt;&gt;高级设置</td>
          </tr>
		  <tr>
		    <td>&nbsp;</td>
		  </tr>
          <tr>
            <td class="space" style="padding-left:10px; padding-right:10px;">&nbsp;&nbsp;&nbsp;本页面提供了对无线高级参数的设置,一般情况下并不需要对这些作修改,保持系统默认的就可以了.如果您对无线参数非常了解,可以按您的需求进行设置。</td>
          </tr>
		  <tr>
        	<td>&nbsp;</td>
      	  </tr>
          <tr>
            <td class="titlebg" id="advWireless">无线高级设置</td>
          </tr>
          <tr>
            <td>
			  <table class="space" width="100%" border="0" cellspacing="0" cellpadding="0">
                <tr>
                  <td width="25%" class="contentpadding" id="advBGProtect">BG 保护模式:</td>
                  <td width="75%"><select name="bg_protection" size="1">
	<option value=0 selected id="advBGProAuto">自动</option>
	<option value=1 id="advBGProOn">开</option>
	<option value=2 id="advBGProOff">关</option></select></td>
                </tr>
                <tr>
                  <td class="contentpadding" id="advBasicDtRt">基本数据传输速率:</td>
                  <td><select name="basic_rate" size="1">
      <option value="3" >1-2 Mbps</option>
	<option value="15" id="advBasicDtRtDefault">默认(1-2-5.5-11 Mbps)</option>
	<option value="351" id="advBasicDtRtAll">完整(1-2-5.5-6-11-12-24 Mbps)</option>
    </select></td>
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
                  <td><input type=radio name=tx_burst value="1" checked>启用&nbsp;<input type=radio name=tx_burst value="0">禁用 </td>
                </tr>
				<tr> 
                  <td class="contentpadding">数据包聚合:</td>
                  <td><input type=radio name=pkt_aggregate value="1">启用&nbsp;<input type=radio name=pkt_aggregate value="0" checked>禁用</td>
                </tr>
				
				<tr style="display:none; visibility:hidden"> 
                  <td class="contentpadding">IEEE 802.11H:</td>
                  <td><input type=radio name=ieee_80211h value="1">启用&nbsp;<input type=radio name=ieee_80211h value="0" checked>禁用 <font color="#808080">(只用于A 模式)</font></td>
                </tr>
				<tr style="display:none; visibility:hidden">
                  <td class="contentpadding">国家(地区)代码:</td>
                  <td><select name="country_code">
        <option value="US">US (美国)</option>
        <option value="JP">JP (日本)</option>
        <option value="FR">FR (法国)</option>
        <option value="TW">TW (中国台湾)</option>
        <option value="IE">IE (爱尔兰)</option>
        <option value="HK">HK (中国香港)</option>
        <option value="NONE" selected>NONE</option></select></td>
                </tr>
              </table>
			</td>
          </tr>
		  <tr style="display:none; visibility:hidden">
            <td class="titlebg">WIFI 设置</td>
          </tr>
		  <tr style="display:none; visibility:hidden">
            <td>
			  <table class="space" width="100%" border="0" cellspacing="0" cellpadding="0">
                <tr>
                  <td width="25%" class="contentpadding">WIFI 多媒体WMM: </td>
                  <td width="75%"><input type=radio name=wmm_capable value="1" onClick="wmm_capable_enable_switch()" checked>启用&nbsp;<input type=radio name=wmm_capable value="0" onClick="wmm_capable_enable_switch()">禁用</td>
                </tr>
                <tr id="div_apsd_capable" name="div_apsd_capable">
                  <td class="contentpadding">自动省电模式: </td>
                  <td><input type=radio name=apsd_capable value="1">启用&nbsp;<input type=radio name=apsd_capable value="0" checked>禁用</td>
                </tr>
			    <tr id="div_dls_capable" name="div_dls_capable">
				  <td class="contentpadding" id="advDLS">DLS Capable:</td>
				  <td><input type=radio name=dls_capable value="1"><font id="advDLSEnable">启用</font>&nbsp;<input type=radio name=dls_capable value="0" checked><font id="advDLSDisable">禁用</font></td>
			    </tr>
				<tr>
                  <td class="contentpadding">WMM 参数:</td>
                  <td><input type=button class="button2" name="wmm_list" value="配置WMM" onClick="open_wmm_window()"></td>
                </tr>
              </table>
			</td>
          </tr>
		  <input type="hidden" name="rebootAP" value="0">
		  <tr id="div_11n_space" name="div_11n_space">
            <td class="titlebg" id="basicHTPhyMode">HT 实体模式</td>
          </tr>
		  <tr id="div_11n" name="div_11n" >
            <td><!--<table width="98%" id="div_11n" name="div_11n" border="0" cellspacing="0" cellpadding="0" style="display:none">-->
			  <table class="space" width="100%"border="0" cellspacing="0" cellpadding="0">
                <tr>
                  <td width="25%" class="contentpadding" id="basicHTOPMode">运作模式:</td>
                  <td width="75%"><input type=radio name=n_mode value="0" checked><font id="basicHTMixed">混合模式</font>&nbsp;<input type=radio name=n_mode value="1">Green Field</td>
                </tr>
                <tr>
                  <td class="contentpadding" id="basicHTChannelBW">频段带宽:</td>
                  <td><input type=radio name=n_bandwidth value="0" onClick="Channel_BandWidth_onClick()" checked>20&nbsp;<input type=radio name=n_bandwidth value="1" onClick="Channel_BandWidth_onClick()">20/40</td>
                </tr>
                <tr>
                  <td class="contentpadding" id="basicHTGI">保护间隔:</td>
                  <td><input type=radio name=n_gi value="0" checked><font id="basicHTLongGI">长</font>&nbsp;<input type=radio name=n_gi value="1"><font id="basicHTAutoGI">自动</font></td>
                </tr>
				<tr>
                  <td class="contentpadding">MCS:</td>
                  <td><select name="n_mcs" size="1">
	                                 <option value = 0>0</option>
	                                 <option value = 1>1</option>
                                     <option value = 2>2</option>
                                	 <option value = 3>3</option>
	                                 <option value = 4>4</option>
	                                 <option value = 5>5</option>
	                                 <option value = 6>6</option>
	                                 <option value = 7>7</option>
	                                 <option value = 8>8</option>
	                                 <option value = 9>9</option>
	                                 <option value = 10>10</option>
	                                 <option value = 11>11</option>
	                                 <option value = 12>12</option>
	                                 <option value = 13>13</option>
	                                 <option value = 14>14</option>
	                                 <option value = 15>15</option>
	                                 <option value = 32>32</option>
	                                 <option value = 33 selected id="basicHTAutoMCS">自动</option></select></td>
                </tr>
				<tr>
                  <td class="contentpadding" id="basicHTRDG">反转方向权限(RDG):</td>
                  <td><input type=radio name=n_rdg value="0" checked><font id="basicHTRDGDisable">禁用</font>&nbsp;<input type=radio name=n_rdg value="1"><font id="basicHTRDGEnable">启用</font></td>
                </tr>
  				<tr name="extension_channel" id="extension_channel">
                  <td class="contentpadding" id="basicHTExtChannel">扩展频段:</td>
                  <td><select id="n_extcha" name="n_extcha" size="1"><option value=1 selected>2412MHz (Channel 1)</option></select></td>
                </tr>
				<tr>
                  <td class="contentpadding" id="basicHTAMSDU">聚合MAC业务数据单元(A-MSDU):</td>
                  <td><input type=radio name=n_amsdu value="0" checked><font id="basicHTAMSDUDisable">禁用</font>&nbsp;<input type=radio name=n_amsdu value="1"><font id="basicHTAMSDUEnable">启用</font></td>
                </tr>
				<tr>
                  <td class="contentpadding" id="basicHTAddBA">自动单一区块确认:</td>
                  <td><input type=radio name=n_autoba value="0" checked>禁用&nbsp;<input type=radio name=n_autoba value="1">启用</td>
                </tr>
				<tr>
                  <td class="contentpadding" id="basicHTDelBA">拒绝单一区块确认要求:</td>
                  <td><input type=radio name=n_badecline value="0" checked><font id="basicHTAddBADisable">禁用</font>&nbsp;<input type=radio name=n_badecline value="1"><font id="basicHTDelBAEnable">启用</font></td>
                </tr>
			  </table>
			</td>
          </tr>
		  <tr style="display:none; visibility:hidden">
            <td class="titlebg" id="basicOther">其他设置</td>
          </tr>
          <tr id="div_11n_plugfest" name="div_11n_plugfest" style="display:none; visibility:hidden">
            <td>
			  <table class="space" width="100%" border="0" cellspacing="0" cellpadding="0">
				  <!--
				  <tr>
					<td width="45%" bgcolor="#E8F8FF" nowrap>40 Mhz Intolerant</td>
					<td><font color="#003366" face=arial><b>
					  <input type=radio name=f_40mhz value="0" checked>Diable&nbsp;
					  <input type=radio name=f_40mhz value="1">Enable
					</b></font></td>
				  </tr>
				  <tr>
					<td class="head">WiFi Optimum</td>
					<td>
					  <input type=radio name=wifi_opt value="0" checked>Diable&nbsp;
					  <input type=radio name=wifi_opt value="1">Enable
					</td>
				  </tr>
				  -->
                <tr>
                  <td width="25%" class="contentpadding" id="basicHTTxStream">高吞吐量传送数据流:</td>
                  <td width="75%"><select name="tx_stream" size="1">
                            	<option value = 1>1</option>
	                            <option value = 2>2</option></select></td>
                </tr>
                <tr>
                  <td class="contentpadding" id="basicHTRxStream">高吞吐量接收数据流:</td>
                  <td><select name="rx_stream" size="1">
	                                <option value = 1>1</option>
	                                <option value = 2>2</option></select></td>
                </tr>
              </table>
			</td>
          </tr>
		  <tr>
            <td class="titlebg">多播对单播</td>
          </tr>
		  <tr>
            <td>
			  <table class="space" width="100%" border="0" cellspacing="0" cellpadding="0">
			    <tr id="div_m2u">
                  <td width="25%" class="contentpadding">多播对单播:</td>
                  <td width="75%">&nbsp;<input type=radio name="m2u_enable" value="1">启用&nbsp;<input type=radio name="m2u_enable" value="0">禁用</td>
                </tr>
			  </table>
			</td>
		  </tr>
		  <tr>
		    <td>&nbsp;</td>
		  </tr>
          <tr>
            <td class="contentpadding"><input type="hidden" value="/wir_advanced.asp" name="submit-url"><input type="submit" class="button"  value="提 交" />&nbsp;&nbsp;<input type="reset" class="button"  value="取 消" onClick="window.location.reload()"></td>
          </tr>
		  <tr>
		    <td>&nbsp;</td>
		  </tr>
	  </table>
	 </td>
  </tr>
  <tr>
     <td>&nbsp;</td>
  </tr>
</table>
</form>
</body>
</html>