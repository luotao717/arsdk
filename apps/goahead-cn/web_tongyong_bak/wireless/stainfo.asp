<html>
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Expires" content="-1">
<meta http-equiv="Content-Type" content="text/html; charset=<% getCharset(); %>">
<link rel="stylesheet" href="../style/normal_ws.css" type="text/css">
<script language="javascript" src="../js/language_<% getCfgZero(1, "LanguageType"); %>.js"></script>
<script language="javascript" src="../js/common.js"></script>
<script language="javascript">
function Load_Setting()
{
	var opmode = 1* <% getCfgZero(1, "OperationMode"); %>;
	var wifi_off = '<% getCfgZero(1, "WiFiOff"); %>';
	var PhyMode  = '<% getCfgZero(1, "WirelessMode"); %>';
	var wpsenable = '<% getCfgZero(1, "WscModeOption"); %>';
	var wdsMode = '<% getCfgZero(1, "WdsEnable"); %>';	
	var channel = "<% getCfgGeneral(1, "Channel"); %>";	
	
	var apclib = "<% getWlanApcliBuilt(); %>";
	var apcli_en = '<% getCfgZero(1, "apClient"); %>';
	var apcli_mode = '<% getCfgGeneral(1, "ApCliAuthMode"); %>';
	var apcli_enc = '<% getCfgGeneral(1, "ApCliEncrypType"); %>';
	
	var authenticationMode = '<% getCfgZero(1, "AuthMode"); %>';
	var encryptionType = '<% getCfgZero(1, "EncrypType"); %>';
	var ieee8021x = '<% getCfgZero(1, "IEEE8021X"); %>';
	
	var ieee8021xArray;
	var authenticationModeArray;
	var encryptionTypeArray;
	
	ieee8021xArray = ieee8021x.split(";");
	authenticationModeArray = authenticationMode.split(";");
	encryptionTypeArray = encryptionType.split(";");
	
	if (ieee8021xArray[0]==0 && authenticationModeArray[0]=="OPEN" && encryptionTypeArray[0]=="NONE")
		document.getElementById("security_mode").innerHTML = MM_disable;
	else if (ieee8021xArray[0]==0 && encryptionTypeArray[0]=="WEP")
		document.getElementById("security_mode").innerHTML = "WEP";
	else if (ieee8021xArray[0]==1 && authenticationModeArray[0]=="OPEN")
		document.getElementById("security_mode").innerHTML = "IEEE 802.1X";
	else if (ieee8021xArray[0]==0 && authenticationModeArray[0]=="WPAPSK")
		document.getElementById("security_mode").innerHTML = "WPA-PSK";
	else if (ieee8021xArray[0]==0 && authenticationModeArray[0]=="WPA2PSK")
		document.getElementById("security_mode").innerHTML = "WPA2-PSK";
	else if (ieee8021xArray[0]==0 && authenticationModeArray[0]=="WPAPSKWPA2PSK")
		document.getElementById("security_mode").innerHTML = "WPA/WPA2-PSK";
	else if (ieee8021xArray[0]==0 && authenticationModeArray[0]=="WPA")
		document.getElementById("security_mode").innerHTML = "WPA";
	else if (ieee8021xArray[0]==0 && authenticationModeArray[0]=="WPA2")
		document.getElementById("security_mode").innerHTML = "WPA2";
	else if (ieee8021xArray[0]==0 && authenticationModeArray[0]=="WPA1WPA2")
		document.getElementById("security_mode").innerHTML = "WPA/WPA2";
	else
		document.getElementById("security_mode").innerHTML = MM_disable;
			
	if (1*wifi_off == 1)
		document.getElementById("wireless_status").innerHTML = MM_offed;
	else
		document.getElementById("wireless_status").innerHTML = MM_oned;
		
	if (1*PhyMode == 0)
		document.getElementById("network_mode").innerHTML = "11b/g";
	else if (1*PhyMode == 1) 
		document.getElementById("network_mode").innerHTML = "11b";	
	else if (1*PhyMode == 4)
		document.getElementById("network_mode").innerHTML = "11g"
	else if (1*PhyMode == 9)
		document.getElementById("network_mode").innerHTML = "11b/g/n";
	else if (1*PhyMode == 6)
		document.getElementById("network_mode").innerHTML = "11n(2.4G)";
		
	if (1*wpsenable == 0)
		document.getElementById("wps_mode").innerHTML = MM_disable;
	else
		document.getElementById("wps_mode").innerHTML = MM_enable;
		
	if (1*wdsMode == 0)
		document.getElementById("wds_mode").innerHTML = MM_disable;
	else if (1*wdsMode == 4)
		document.getElementById("wds_mode").innerHTML = MM_lazy_mode;	
	else if (1*wdsMode == 2)
		document.getElementById("wds_mode").innerHTML = MM_bridge_mode;	
	else if (1*wdsMode == 3)
		document.getElementById("wds_mode").innerHTML = MM_repeater_mode;	

	if (opmode == '3' || (opmode == '0' && apclib == "1" && apcli_en == "1"))
	{
		document.getElementById("div_apclient_br").style.display = "";
		document.getElementById("div_apclient").style.display = "";
		document.getElementById("apcli_status").innerHTML = <% getIndex(1, "apcliStatus"); %>;
		
		if (apcli_mode == "OPEN" && apcli_enc == "NONE")
			document.getElementById("apcli_security_mode").innerHTML = MM_disable;//Disable
		else if (apcli_mode == "OPEN" && apcli_enc == "WEP")
			document.getElementById("apcli_security_mode").innerHTML = "WEP";//WEP
		else if (apcli_mode == "WPAPSK")
			document.getElementById("apcli_security_mode").innerHTML = "WPAPSK";//WPAPSK
		else if (apcli_mode == "WPA2PSK")
			document.getElementById("apcli_security_mode").innerHTML = "WPA2PSK";//WPA2PSK
		else
			document.getElementById("apcli_security_mode").innerHTML = MM_disable;//Disable
	}
	else
	{
		document.getElementById("div_apclient_br").style.display = "none";
		document.getElementById("div_apclient").style.display = "none";
		document.getElementById("apcli_status").innerHTML = MM_unknown;
	}
	
	if (channel == 0)
		document.getElementById("channel").innerHTML = MM_auto_select;
	else
		document.getElementById("channel").innerHTML = channel;
	
	var txbf = "<% getTxBfBuilt(); %>";
	if (txbf == "1")
		document.getElementById("div_info_normal").style.display = "none";
	else
		document.getElementById("div_info_txbf").style.display = "none";
}
</script>
</head>
<body onLoad="Load_Setting()" class="mainbody">
<blockquote>
<table width=700><tr><td>
<table width=100% border=0 cellpadding=3 cellspacing=1> 
<tr><td class="title"><script>dw(MM_wireless_status)</script></td></tr>
<tr><td><script>dw(JS_msg_stainfo)</script></td></tr>
<tr><td><hr></td></tr>
</table>

<br>
<table class="list">
<tr><td class="tcenter"><script>dw(MM_wireless_iface)</script></td></tr>
<tr><td><table class="list1">
<tr>
  <td class="thead"><script>dw(MM_wireless_status)</script>:</td>
  <td><span id="wireless_status"> </span> </td>
</tr>
<tr>
  <td class="thead"><script>dw(MM_ssid)</script>:</td>
  <td><% getCfgToHTML(1, "SSID1"); %></td>
</tr>
<tr>
  <td class="thead">BSSID:</td>
  <td><% getWlanCurrentMac(); %></td>
</tr>
<tr>
  <td class="thead"><script>dw(MM_channel)</script>:</td>
  <td><span id="channel"> </span> </td>
</tr>
<tr>
  <td class="thead"><script>dw(MM_network_mode)</script>:</td>
  <td><span id="network_mode"> </span> </td>
</tr>
<tr>
  <td class="thead"><script>dw(MM_security_mode)</script>:</td>
  <td><span id="security_mode"> </span> </td>
</tr>
<tr>
  <td class="thead"><script>dw(MM_wps_mode)</script>:</td>
  <td><span id="wps_mode"> </span> </td>
</tr>
<tr>
  <td class="thead"><script>dw(MM_wds_mode)</script>:</td>
  <td><span id="wds_mode"> </span> </td>
</tr>
</table></td></tr>
</table>

<br id="div_apclient_br">
<table id="div_apclient" class="list">
<tr><td class="tcenter"><script>dw(MM_wireless_repeater_iface)</script></td></tr>
<tr><td><table class="list1">
<tr>
  <td class="thead"><script>dw(MM_apcli_status)</script>:</td>
  <td><span id="apcli_status"> </span> </td>
</tr>
<tr>
  <td class="thead"><script>dw(MM_ssid)</script>:</td>
  <td><% getCfgToHTML(1, "ApCliSsid"); %></td>
</tr>
<tr>
  <td class="thead"><script>dw(MM_channel)</script>:</td>
  <td><% getCfgGeneral(1, "Channel"); %></td>
</tr>
<tr>
  <td class="thead"><script>dw(MM_security_mode)</script>:</td>
  <td><span id="apcli_security_mode"> </span> </td>
</tr>
</table></td></tr>
</table>

<br>
<table id="div_info_normal" width=100% border=1 cellpadding=3 style="border-collapse: collapse" bordercolor="#C8C8C8">
  <tr bgcolor=#f2f2f2>
    <td align=center><b><script>dw(MM_macaddr)</script></b></td>
    <td align=center><b>Aid</b></td>
    <td align=center><b>PSM</b></td>
    <td align=center><b>Mimo<br>PS</b></td>
    <td align=center><b>MCS</b></td>
    <td align=center><b><script>dw(MM_bandwidth)</script></b></td>
    <td align=center><b>SGI</b></td>
    <td align=center><b>STBC</b></td>
  </tr>
  <% getWlanStaInfo("ra0"); %>
</table>

<table id="div_info_txbf" width=100% border=1 cellpadding=3 style="border-collapse: collapse" bordercolor="#C8C8C8">
  <tr bgcolor=#f2f2f2>
    <td align=center><b><script>dw(MM_macaddr)</script></b></td>
    <td align=center><b>Aid</b></td>
    <td align=center><b>PSM</b></td>
    <td align=center><b>MIMO<br>PS</b></td>
    <td align=center><b><script>dw(MM_tx_rate)</script></b></td>
    <td align=center><b>TxBF</b></td>
    <td align=center><b>RSSI</b></td>
    <td align=center><b>Stream<br>SNR</b></td>
    <td align=center><b>Snd Rsp<br>SNR</b></td>
    <td align=center><b>Last<br>RX Rate</b></td>
    <td align=center><b>Connect<br>Time</b></td>
  </tr>
  <% getWlanStaInfo("ra0"); %>
</table>
<br><br>
</td></tr></table>
</blockquote>
</body></html>
