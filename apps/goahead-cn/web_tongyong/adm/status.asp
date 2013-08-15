<html>
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Expires" content="-1">
<meta http-equiv="Content-Type" content="text/html; charset=<% getCharset(); %>">
<link rel="stylesheet" href="../style/normal_ws.css" type="text/css">
<script language="javascript" src="../js/language_<% getCfgZero(1, "LanguageType"); %>.js"></script>
<script language="javascript" src="../js/common.js"></script>
<META HTTP-EQUIV="refresh" CONTENT="5; URL=./status.asp">
<script language="javascript">
function showOpMode()
{
	var opmode = 1* <% getCfgZero(1, "OperationMode"); %>;
	if (opmode == 0)
		document.write(MM_bridge_mode);
	else if (opmode == 1)
		document.write(MM_gateway_mode);
	else if (opmode == 2)
		document.write(MM_wisp_mode);
	else if (opmode == 3)
		document.write(MM_apclient_mode);
}

function checkDate(str)
{
	var month = ["Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"];
	var week = [MM_sun, MM_mon, MM_tue, MM_wed, MM_thu, MM_fri, MM_sat];
	
	if ((str.substring(4,5)) == " ")
		str = str.replace(" ","");
	else
		str = str;
	
	var t = str.split(" ");	
	for (var j=0; j<12; j++)
	{
		if (t[0] == month[j]) 
			t[0] = j + 1;
	}
	
	return t[2] + "-" + t[0] + "-" + t[1];
}

function Load_Setting()
{
	var opmode = 1* <% getCfgZero(1, "OperationMode"); %>;	
	var wan_connect_mode = '<% getCfgGeneral(1, "wanConnectionMode"); %>';
	var wan_connect_status = <% getIndex(1, "wanConnectStatus"); %>;
	var dhcpEnb = <% getCfgZero(1, "dhcpEnabled"); %>;

	var customer_url = <% getCustomerUrlBuilt(); %>;

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

	if (wan_connect_mode == "STATIC")
		document.getElementById("wan_connect_mode").innerHTML = MM_staticip;
	else if (wan_connect_mode == "DHCP")
		document.getElementById("wan_connect_mode").innerHTML = MM_dhcp;
	else if (wan_connect_mode == "PPPOE")
		document.getElementById("wan_connect_mode").innerHTML = MM_pppoe;
	else if (wan_connect_mode == "PPTP")
		document.getElementById("wan_connect_mode").innerHTML = "PPTP";
	else if (wan_connect_mode == "L2TP")
		document.getElementById("wan_connect_mode").innerHTML = "L2TP";
	else if (wan_connect_mode == "3G")
		document.getElementById("wan_connect_mode").innerHTML = "3G";
		
	if (opmode == 0)
	{
		document.getElementById("div_wan_br").style.display = "none";
		document.getElementById("div_wan").style.display = "none";
	}
	else
	{
		document.getElementById("div_wan_br").style.display = "";
		document.getElementById("div_wan").style.display = "";
	}
	
	if (channel == 0)
		document.getElementById("channel").innerHTML = MM_auto_select;
	else
		document.getElementById("channel").innerHTML = channel;
		
	if (wan_connect_mode == "STATIC" || wan_connect_mode == "3G")
	{
		document.getElementById("wan_ip").innerHTML = "<% getWanIp(); %>";
		document.getElementById("wan_netmask").innerHTML = "<% getWanNetmask(); %>";
		document.getElementById("wan_gateway").innerHTML = "<% getWanGateway(); %>";
		document.getElementById("wan_dns1").innerHTML = "<% getDns(1); %>";
		document.getElementById("wan_dns2").innerHTML = "<% getDns(2); %>";
	}
	else
	{
		if (wan_connect_status == MM_connect_no)
		{
			document.getElementById("wan_ip").innerHTML = "";
			document.getElementById("wan_netmask").innerHTML = "";
			document.getElementById("wan_gateway").innerHTML = "";
			document.getElementById("wan_dns1").innerHTML = "";
			document.getElementById("wan_dns2").innerHTML = "";
		}
		else
		{
			document.getElementById("wan_ip").innerHTML = "<% getWanIp(); %>";
			document.getElementById("wan_netmask").innerHTML = "<% getWanNetmask(); %>";
			document.getElementById("wan_gateway").innerHTML = "<% getWanGateway(); %>";
			document.getElementById("wan_dns1").innerHTML = "<% getDns(1); %>";
			document.getElementById("wan_dns2").innerHTML = "<% getDns(2); %>";
		}
	}
	
	
	if (dhcpEnb == 1)
		document.getElementById("dhcp_server").innerHTML = MM_enable;
	else
		document.getElementById("dhcp_server").innerHTML = MM_disable;
	
	if (opmode == 1 && wan_connect_mode=="3G")
	{
		document.getElementById("div_3g_info_br").style.display = "";
		document.getElementById("div_3g_info").style.display = "";
		
		var dialstatus = "<% getDial3gStatus(); %>";
		var dial3gsimcard = "<% getDial3gSimCard(); %>";
		var pppdflag = "<% getDial3gPPPDFlag(); %>";
		
		if (dialstatus == "UP")
		{
			document.getElementById("3Gdialstatus").innerHTML = MM_dialStatus1;
		}
		else
		{
			if (dial3gsimcard == "")
				document.getElementById("3Gdialstatus").innerHTML = "";
			else if (dial3gsimcard=="failure")
				document.getElementById("3Gdialstatus").innerHTML = MM_dialStatus2;
			else if (dial3gsimcard == "SIMPINFail")
				document.getElementById("3Gdialstatus").innerHTML = MM_dialStatus3;
			else if (dial3gsimcard == "successful")
				document.getElementById("3Gdialstatus").innerHTML = MM_dialStatus4;
			
			if (pppdflag == "1")
				document.getElementById("3Gdialstatus").innerHTML = MM_dialStatus5;
			else
				document.getElementById("3Gdialstatus").innerHTML = MM_dialStatus6;
		}
		
		var info = "<% getDial3gUsbModemInfo(); %>";
		var usbflag = "<% getDial3gUsbModemFlag(); %>";
		var cimivalue="<% getDial3gCimiValue(); %>";

		if (usbflag == "yes")
		{
			if (info != "")
				document.getElementById("USBmodemstatus").innerHTML = MM_modemStatus1 + "&nbsp;&nbsp;<% getDial3gUsbModemInfo(); %>";
			else
				document.getElementById("USBmodemstatus").innerHTML = MM_modemStatus1;
		}
		else
		{
			document.getElementById("USBmodemstatus").innerHTML = MM_modemStatus2;
		}

		if (cimivalue == "")
			document.getElementById("USBsimstatus").innerHTML = MM_NoSim;
		else
			document.getElementById("USBsimstatus").innerHTML = MM_HaveSim + "&nbsp;&nbsp;<% getDial3gCimiValue(); %>";
	}
	else
	{
		document.getElementById("div_3g_info_br").style.display = "none";
		document.getElementById("div_3g_info").style.display = "none";
	}	

	if (customer_url == "1")
		document.getElementById("customer_url").style.display = "";
	else
		document.getElementById("customer_url").style.display = "none";
}
</script>
</head>
<body onLoad="Load_Setting()" class="mainbody">
<blockquote>
<table width=700><tr><td>
<table width=100% border=0 cellpadding=3 cellspacing=1> 
<tr><td class="title"><script>dw(MM_sysstatus)</script></td></tr>
<tr><td><script>dw(JS_msg_status)</script></td></tr>
<tr><td><hr></td></tr>
</table>
<br>
<table class="list">
<tr><td class="tcenter"><script>dw(MM_system)</script></td></tr>
<tr><td><table class="list1">
<tr id="customer_url" style="display:none">
  <td class="thead">Customer URL:</td>
  <td><% getCustomerUrl(); %></td>
</tr>
<tr>
  <td class="thead"><script>dw(MM_cur_firmware)</script>:</td>
  <td><% getSdkVersion(); %></td>
</tr>
<tr>
  <td class="thead"><script>dw(MM_firmware_date)</script>:</td>
  <td><script>dw(checkDate("<% getSysBuildTime(); %>"));</script></td>
</tr>
<tr>
  <td class="thead"><script>dw(MM_uptime)</script>:</td>
  <td><% getSysUptime(); %></td>
</tr>
<tr style="display:none">
  <td class="thead"><script>dw(MM_sys_platform)</script>:</td>
  <td><% getPlatform(); %></td>
</tr>
<tr>
  <td class="thead"><script>dw(MM_opmode)</script>:</td>
  <td><script language="javascript">showOpMode();</script></td>
</tr>
</table></td></tr>

<!--3G detail info begin-->
<tr id="div_3g_info_br"><td class="tcenter"><script>dw(MM_3g_detail_info)</script></td></tr>
<tr id="div_3g_info"><td><table class="list1">
<tr>
  <td class="thead"><script>dw(MM_3g_dialStatus)</script>:</td>
  <td><span id="3Gdialstatus"> </span></td>
</tr>
<tr>
  <td class="thead"><script>dw(MM_3g_signal)</script>:</td>
  <td><% getDial3gSignalLength(); %>%</td>
</tr>
<tr>
  <td class="thead"><script>dw(MM_3g_isp)</script>:</td>
  <td><% getDial3gISPProvider(); %></td>
</tr>
<tr>
  <td class="thead"><script>dw(MM_3g_modemStatus)</script>:</td>
  <td><span id="USBmodemstatus"> </span></td>
</tr>
<tr>
  <td class="thead"><script>dw(MM_3g_SIM)</script>:</td>
  <td><span id="USBsimstatus"> </span></td>
</tr>
</table></td></tr>
<!--end-->

<tr id="div_wan_br"><td class="tcenter"><script>dw(MM_wan_iface)</script></td></tr>
<tr id="div_wan"><td><table class="list1">
<tr>
  <td class="thead"><script>dw(MM_connect_status)</script>:</td>
  <td><script>dw(<% getIndex(1, "wanConnectStatus"); %>)</script></td>
</tr>
<tr>
  <td class="thead"><script>dw(MM_connection)</script>:</td>
  <td><span id="wan_connect_mode"> </span> </td>
</tr>
<tr>
  <td class="thead"><script>dw(MM_ipaddr)</script>:</td>
  <td><span id="wan_ip"> </span> </td>
</tr>
<tr>
  <td class="thead"><script>dw(MM_submask)</script>:</td>
  <td><span id="wan_netmask"> </span> </td>
</tr>
<tr>
  <td class="thead"><script>dw(MM_default_gateway)</script></td>
  <td><span id="wan_gateway"> </span> </td>
</tr>
<tr>
  <td class="thead"><script>dw(MM_pridns)</script>:</td>
  <td><span id="wan_dns1"> </span> </td>
</tr>
<tr>
  <td class="thead"><script>dw(MM_secdns)</script>:</td>
  <td><span id="wan_dns2"> </span> </td>
</tr>
<tr>
  <td class="thead"><script>dw(MM_macaddr)</script>:</td>
  <td><% getWanMac(); %></td>
</tr>
</table></td></tr>

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

<tr id="div_apclient_br"><td class="tcenter"><script>dw(MM_wireless_repeater_iface)</script></td></tr>
<tr id="div_apclient"><td><table class="list1">
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

<tr><td class="tcenter"><script>dw(MM_lan_iface)</script></td></tr>
<tr><td><table class="list1">
<tr>
  <td class="thead"><script>dw(MM_ipaddr)</script>:</td>
  <td><% getLanIp(); %></td>
</tr>
<tr>
  <td class="thead"><script>dw(MM_submask)</script>:</td>
  <td><% getLanNetmask(); %></td>
</tr>
<tr>
  <td class="thead"><script>dw(MM_dhcp_server)</script>:</td>
  <td><span id="dhcp_server"> </span> </td>
</tr>
<tr>
  <td class="thead"><script>dw(MM_macaddr)</script>:</td>
  <td><% getLanMac(); %></td>
</tr>
</table></td></tr>
</table>
<br><br>
</td></tr></table>
</blockquote>
</body></html>
