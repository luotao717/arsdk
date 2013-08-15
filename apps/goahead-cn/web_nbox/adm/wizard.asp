<html>
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Expires" content="-1">
<meta http-equiv="Content-Type" content="text/html; charset=<% getCharset(); %>">
<link rel="stylesheet" href="../style/normal_ws.css" type="text/css">
<script language="javascript" src="../js/language_<% getCfgZero(1, "LanguageType"); %>.js"></script>
<script language="javascript" src="../js/common.js"></script>
<script language="javascript">
function formCheck()
{
	if (document.wizard_form.connectionType.options.selectedIndex == 0)//STATIC
	{
		if (!ipCheck(document.wizard_form.staticIp, MM_ipaddr, 0)) 
			return false;
		
		if (!maskCheck(document.wizard_form.staticNetmask, MM_submask)) 
			return false;
		
		if (document.wizard_form.staticGateway.value != "") 
		{
			if (!ipCheck(document.wizard_form.staticGateway, MM_default_gateway, 0)) 
				return false;
				
			if (!subnetCheck(document.wizard_form.staticGateway.value, document.wizard_form.staticNetmask.value, document.wizard_form.staticIp.value)) 
			{
				alert(JS_msg13);
				document.wizard_form.staticGateway.focus();
				return false;
			}
		}
		
		if (document.wizard_form.staticPriDns.value != "") 
		{ 
			if (!ipCheck(document.wizard_form.staticPriDns, MM_pridns, 0)) 
				return false; 
		}
		
		if (document.wizard_form.staticSecDns.value != "") 
		{
			if (!ipCheck(document.wizard_form.staticSecDns, MM_secdns, 0)) 
				return false; 
		}
	}
	else if (document.wizard_form.connectionType.options.selectedIndex == 2)//PPPOE
	{ 
		if (!blankCheck(document.wizard_form.pppoeUser, MM_username)) 
			return false;
		
		if (!blankCheck(document.wizard_form.pppoePass, MM_password)) 
			return false;
		
		if (document.wizard_form.pppoePass.value != document.wizard_form.pppoePass2.value) 
		{
			alert(JS_msg14);
			document.wizard_form.pppoePass.focus();
			return false;
		}		
	}

	if (!ssidCheck(document.wizard_form.ssid, MM_ssid)) 
		return false;

	if (document.wizard_form.security_mode.selectedIndex == 1)
	{
		if (!blankCheck(document.wizard_form.security_key, MM_wep_key)) 
			return false;
			
		var keyvalue = document.wizard_form.security_key.value;
		if (keyvalue.length != 5 && keyvalue.length != 13 && keyvalue.length != 10 && keyvalue.length != 26)
		{
			alert(JS_msg15);
			document.wizard_form.security_key.focus();
			return false;
		}
		
		if (!stringCheck(document.wizard_form.security_key, MM_wep_key))
			return false;
	}
	else if (document.wizard_form.security_mode.selectedIndex >= 2)
	{
		var keyvalue = document.wizard_form.security_key.value;
		if (keyvalue.length == 0)
		{
			alert(JS_msg16);
			document.wizard_form.security_key.focus();
			return false;
		}
		
		if (keyvalue.length < 8)
		{
			alert(JS_msg17);
			document.wizard_form.security_key.focus();
			return false;
		}
		
		if (!stringCheck(document.wizard_form.security_key, MM_wpa_key))
			return false;
	}
	
	return true;
}

function updateConnectionType()
{
	document.getElementById("wan_static_ip").style.display = "none";
	document.getElementById("wan_static_mask").style.display = "none";
	document.getElementById("wan_static_gateway").style.display = "none";
	document.getElementById("wan_static_dns1").style.display = "none";
	document.getElementById("wan_static_dns2").style.display = "none";
	document.getElementById("wan_pppoe_user").style.display = "none";
	document.getElementById("wan_pppoe_pwd").style.display = "none";
	document.getElementById("wan_pppoe_pwd2").style.display = "none";
	
	if (document.wizard_form.connectionType.options.selectedIndex == 0)//STATIC
	{
		document.getElementById("wan_static_ip").style.display = "";
		document.getElementById("wan_static_mask").style.display = "";
		document.getElementById("wan_static_gateway").style.display = "";
		document.getElementById("wan_static_dns1").style.display = "";
		document.getElementById("wan_static_dns2").style.display = "";
	}
	else if (document.wizard_form.connectionType.options.selectedIndex == 2)//PPPOE
	{
		document.getElementById("wan_pppoe_user").style.display = "";
		document.getElementById("wan_pppoe_pwd").style.display = "";
		document.getElementById("wan_pppoe_pwd2").style.display = "";
	}
}

function updateSecurityMode()
{
	if (document.wizard_form.security_mode.selectedIndex == 0)
	{
		document.getElementById("wlan_key").style.display = "none";
	}
	else if (document.wizard_form.security_mode.selectedIndex == 1)	//WEP
	{
		document.getElementById("wlan_key").style.display = "";
		document.wizard_form.security_key.value = "<% getCfgGeneral(1, "Key1Str1"); %>";
	}
	else	//WPA
	{
		document.getElementById("wlan_key").style.display = "";
		document.wizard_form.security_key.value = "<% getCfgGeneral(1, "WPAPSK1"); %>";
	}
}

function updateWlanMode()
{
	if (document.wizard_form.wlan_disabled.checked == true)
	{
		document.getElementById("wlan_ssid").style.display = "none";
		document.getElementById("wlan_security").style.display = "none";
		document.getElementById("wlan_key").style.display = "none";
	}
	else
	{
		document.getElementById("wlan_ssid").style.display = "";
		document.getElementById("wlan_security").style.display = "";
		//document.getElementById("wlan_key").style.display = "";
		updateSecurityMode();
	}
}

function Load_Setting()
{
	var wan_connect_mode = '<% getCfgGeneral(1, "wanConnectionMode"); %>';
	var wifi_off = '<% getCfgZero(1, "WiFiOff"); %>';
	
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
		
	if (1*wifi_off == 1)
		document.wizard_form.wlan_disabled.checked = true;
	else
		document.wizard_form.wlan_disabled.checked = false;
		
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
		document.wizard_form.security_mode.selectedIndex = 0;
	else if (ieee8021xArray[0]==0 && encryptionTypeArray[0]=="WEP")
		document.wizard_form.security_mode.selectedIndex = 1;
	else if (ieee8021xArray[0]==0 && authenticationModeArray[0]=="WPAPSK")
		document.wizard_form.security_mode.selectedIndex = 2;
	else if (ieee8021xArray[0]==0 && authenticationModeArray[0]=="WPA2PSK")
		document.wizard_form.security_mode.selectedIndex = 3;
	else if (ieee8021xArray[0]==0 && authenticationModeArray[0]=="WPAPSKWPA2PSK")
		document.wizard_form.security_mode.selectedIndex = 4;
	else
		document.wizard_form.security_mode.selectedIndex = 0;
		
	updateConnectionType();
	updateWlanMode();
	//updateSecurityMode();
}

function resetForm()
{
	location=location; 
}
</script>
</head>
 
<body onLoad="Load_Setting()">
<table width=700><tr><td>
<table width=100% border=0 cellpadding=3 cellspacing=1> 
<tr><td class="title"><script>dw(MM_easywizard)</script></td></tr>
<tr><td><script>dw(JS_msg158)</script></td></tr>
<tr><td><hr></td></tr>
</table>

<form method=post name="wizard_form" action="/goform/setWizard">
<table width=100% border=0 cellpadding=3 cellspacing=1>
<tr>
  <td class="title2" colspan="2"><script>dw(MM_cur_status)</script><hr></td>
</tr>
<tr>
   <td class="thead"><script>dw(MM_cur_status)</script>:</td>
   <td><font color="#ff0000"><span id="wan_connect_mode"> </span></font></td>
</tr>
<tr>
  <td colspan="2">&nbsp;</td>
</tr>
<tr>
  <td class="title2" colspan="2"><script>dw(JS_msg159)</script><hr></td>
</tr>
<tr>
  <td class="thead"><script>dw(MM_connection)</script>:</td>
  <td><select name="connectionType" onChange="updateConnectionType()">
  <option value="STATIC"><script>dw(MM_staticip)</script></option>
  <option value="DHCP" selected><script>dw(MM_dhcp)</script></option>
  <option value="PPPOE"><script>dw(MM_pppoe)</script></option>             
  </select></td>
</tr>
<tr id="wan_static_ip" style="display:none">
  <td class="thead">WAN <script>dw(MM_ipaddr)</script>:</td>
  <td><input name="staticIp" maxlength=15 value="<% getWanIp(); %>"></td>
</tr>
<tr id="wan_static_mask" style="display:none">
  <td class="thead"><script>dw(MM_submask)</script>:</td>
  <td><input name="staticNetmask" maxlength=15 value="<% getWanNetmask(); %>">
</tr>
<tr id="wan_static_gateway" style="display:none">
  <td class="thead"><script>dw(MM_default_gateway)</script>:</td>
  <td><input name="staticGateway" maxlength=15 value="<% getWanGateway(); %>"></td>
</tr>
<tr id="wan_static_dns1" style="display:none">
  <td class="thead"><script>dw(MM_pridns)</script>:</td>
  <td><input name="staticPriDns" maxlength=15 value="<% getDns(1); %>"></td>
</tr>
<tr id="wan_static_dns2" style="display:none">
  <td class="thead"><script>dw(MM_secdns)</script>:</td>
  <td><input name="staticSecDns" maxlength=15 value="<% getDns(2); %>"> (<script>dw(MM_optional)</script>)</td>
</tr>

<tr id="wan_pppoe_user" style="display:none">
  <td class="thead"><script>dw(MM_username)</script>:</td>
  <td><input name="pppoeUser" maxlength=32 value="<% getCfgGeneral(1, "wan_pppoe_user"); %>"></td>
</tr>
<tr id="wan_pppoe_pwd" style="display:none">
  <td class="thead"><script>dw(MM_password)</script>:</td>
  <td><input type="password" name="pppoePass" maxlength=32 value="<% getCfgGeneral(1, "wan_pppoe_pass"); %>"></td>
</tr>
<tr id="wan_pppoe_pwd2" style="display:none">
  <td class="thead"><script>dw(MM_conpassword)</script>:</td>
  <td><input type="password" name="pppoePass2" maxlength=32 value="<% getCfgGeneral(1, "wan_pppoe_pass"); %>"></td>
</tr>
<tr style="display:none">
  <td class="thead"><script>dw(MM_connect_mode)</script>:</td>
  <td><select name="pppoeOPMode" onChange="pppoeOPModeSwitch()">
      <option value="KeepAlive" selected><script>dw(MM_keep_alive)</script></option>
      <option value="OnDemand"><script>dw(MM_ondemand)</script></option>
      <option value="Manual"><script>dw(MM_manual)</script></option>
    </select></td>
</tr>
<tr style="display:none">
  <td class="thead"><script>dw(MM_redial_period)</script>:</td>
  <td><input type="text" name="pppoeRedialPeriod" maxlength="5" size="3" value="60"><script>dw(MM_seconds)</script></td>
</tr>
<tr>
  <td colspan="2">&nbsp;</td>
</tr>
<tr>
  <td class="title2" colspan="2"><script>dw(JS_msg160)</script><hr></td>
</tr>
<tr>
  <td class="thead"><script>dw(MM_disable_wlan)</script>:</td>
  <td><input type="checkbox" name="wlan_disabled" value="ON" onClick="updateWlanMode()"></td>
</tr>
<tr id="wlan_ssid">
  <td class="thead"><script>dw(MM_ssid)</script>:</td>
  <td><input type="text" name="ssid" size="32" maxlength="32" value="<% getCfgToHTML(1, "SSID1"); %>"></td>
</tr>      
<tr id="wlan_security">
  <td class="thead"><script>dw(MM_security_mode)</script>:</td>					 
  <td><select name="security_mode" onChange="updateSecurityMode();">
      <option value="Disable" selected><script>dw(MM_disable)</script></option>
      <option value="OPEN">WEP</option>
      <option value="WPAPSK">WPA-PSK</option>
      <option value="WPA2PSK">WPA2-PSK</option>
      <option value="WPAPSKWPA2PSK">WPA/WPA2-PSK</option>
    </select></td>
</tr>
<tr id="wlan_key">
  <td class="thead"><script>dw(MM_key)</script>:</td>
  <td><input type="text" name="security_key" size="28" maxlength="64"></td>
</tr>
</table>

<br>
<table width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr>
    <td>
      <script>dw('<input type=submit class=button value="'+BT_apply+'" onClick="return formCheck()"> &nbsp; &nbsp;\
      <input type=button class=button value="'+BT_reset+'" onClick="resetForm();">')</script>
    </td>
  </tr>
</table>
</form>

</td></tr></table>
</body></html>