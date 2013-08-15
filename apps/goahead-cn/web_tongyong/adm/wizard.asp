<html>
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Expires" content="-1">
<meta http-equiv="Content-Type" content="text/html; charset=<% getCharset(); %>">
<link rel="stylesheet" href="../style/normal_ws.css" type="text/css">
<script language="javascript" src="../js/language_<% getCfgZero(1, "LanguageType"); %>.js"></script>
<script language="javascript" src="../js/common.js"></script>
<script language="javascript">
var Times=0; 
var flag=true;
var statinfo;
var http_request=false;

function leavePage() 
{
	if (flag){
		Times+=1;
		document.getElementById("msg").innerHTML=Times+" "+MM_seconds;
		setTimeout("leavePage()", 1000);
	}
	else{
		document.getElementById("msg").style.display="none";
	}
}

function connectAutoCheckSubmit()
{
    http_request = false;	
	document.getElementById("autoCheckInfo").innerHTML=JS_msg50;
	//document.wizard_form.connectAutoCheck.disabled=true;
	Times=0;
	flag=true;	
	
	document.getElementById("msg").style.display="";
	setTimeout('leavePage()', 1000);
	
	if (window.XMLHttpRequest) { // Mozilla, Safari,...
        http_request = new XMLHttpRequest();
        if (http_request.overrideMimeType) {
            http_request.overrideMimeType('text/xml');
        }
    } else if (window.ActiveXObject) { // IE
        try {
            http_request = new ActiveXObject("Msxml2.XMLHTTP");
        } catch (e) {
            try {
            http_request = new ActiveXObject("Microsoft.XMLHTTP");
            } catch (e) {}
        }
    }
    if (!http_request) {
        alert(JS_msg5);
        return false;
    }
    http_request.onreadystatechange = doFillCheckInfo;
    http_request.open('POST', '/goform/formConnectAutoCheck', true);
    http_request.send('n\a');	
}

function doFillCheckInfo()
{
    if (http_request.readyState==4) {
		if (http_request.status==200) {
			document.getElementById("autoCheckInfo").innerHTML=http_request.responseText;
			document.getElementById("autoCheckInfo").style.display="none";
			statinfo=document.getElementById("autoCheckInfo").innerHTML;
			if(statinfo=="linkN") {
				alert(JS_msg91);
				flag=false;
			}
			else if(statinfo=="pppoe") {
				alert(JS_msg92);
				flag=false;
				document.wizard_form.connectionType.options.selectedIndex=2;
				updateConnectionType();
			}			
			else if(statinfo=="dhcpc") {
				alert(JS_msg93);
				flag=false;
				document.wizard_form.connectionType.options.selectedIndex=1;
				updateConnectionType();
			}
			else {
				alert(JS_msg94);
				flag=false;
				document.wizard_form.connectionType.options.selectedIndex=0;
				updateConnectionType();
			}
		}
		else {
			alert(JS_msg89);
			flag=false;
		}
		//document.wizard_form.connectAutoCheck.disabled=false;
	}
}

function formCheck()
{
	if (document.wizard_form.connectionType.options.selectedIndex == 0) {
		if (!isIpAddrMsg(document.wizard_form.staticIp.value, MM_ipaddr)) 
			return false;
		
		if (!isMaskAddrMsg(document.wizard_form.staticNetmask.value, MM_submask)) 
			return false;
		
		if (document.wizard_form.staticGateway.value != "") {
			if (!isIpAddrMsg(document.wizard_form.staticGateway.value, MM_default_gateway)) 
				return false;
				
			if (!isIpSubnet(document.wizard_form.staticGateway.value, document.wizard_form.staticNetmask.value, document.wizard_form.staticIp.value)) {
				alert(JS_msg13);
				return false;
			}
		}
		
		if (document.wizard_form.staticPriDns.value != "")
			if (!isIpAddrMsg(document.wizard_form.staticPriDns.value, MM_pridns)) 
				return false; 
		
		if (document.wizard_form.staticSecDns.value != "")
			if (!isIpAddrMsg(document.wizard_form.staticSecDns.value, MM_secdns)) 
				return false; 
	}
	else if (document.wizard_form.connectionType.options.selectedIndex == 2){ 
		if (!isBlankMsg(document.wizard_form.pppoeUser.value, MM_username))
			return false;
		
		if (!isBlankMsg(document.wizard_form.pppoePass.value, MM_password))
			return false;
		
		if (document.wizard_form.pppoePass.value != document.wizard_form.pppoePass2.value) {
			alert(JS_msg14);
			return false;
		}		
	}

	if (!isEmptyMsg(document.wizard_form.ssid.value, MM_ssid))
		return false;

	if (document.wizard_form.security_mode.selectedIndex == 1){
		if (!isBlankMsg(document.wizard_form.security_key.value, MM_wep_key))
			return false;
		
		var keyvalue = document.wizard_form.security_key.value;
		if (keyvalue.length != 5 && keyvalue.length != 13 && keyvalue.length != 10 && keyvalue.length != 26){
			alert(JS_msg15);
			return false;
		}

		if (keyvalue.length == 5 || keyvalue.length == 10){		
			if (!isAscciMsg(document.wizard_form.security_key.value, MM_wep_key))
				return false;
		}
		else{
			if (!isHexMsg(document.wizard_form.security_key.value, MM_wep_key))
				return false;
		}
	}
	else if (document.wizard_form.security_mode.selectedIndex >= 2){
		var keyvalue = document.wizard_form.security_key.value;
		if (keyvalue.length < 8 || keyvalue.length > 63){
			alert(JS_msg17);
			return false;
		}
		
		if (!isAscciMsg(keyvalue, MM_wpa_key))
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
	
	if (document.wizard_form.connectionType.options.selectedIndex == 0){
		document.getElementById("wan_static_ip").style.display = "";
		document.getElementById("wan_static_mask").style.display = "";
		document.getElementById("wan_static_gateway").style.display = "";
		document.getElementById("wan_static_dns1").style.display = "";
		document.getElementById("wan_static_dns2").style.display = "";
	}
	else if (document.wizard_form.connectionType.options.selectedIndex == 2){
		document.getElementById("wan_pppoe_user").style.display = "";
		document.getElementById("wan_pppoe_pwd").style.display = "";
		document.getElementById("wan_pppoe_pwd2").style.display = "";
	}
}

function updateSecurityMode()
{
	if (document.wizard_form.security_mode.selectedIndex == 0){
		document.getElementById("wlan_key").style.display = "none";
	}
	else if (document.wizard_form.security_mode.selectedIndex == 1){
		document.getElementById("wlan_key").style.display = "";
		document.wizard_form.security_key.value = "<% getCfgGeneral(1, "Key1Str1"); %>";
	}
	else{
		document.getElementById("wlan_key").style.display = "";
		document.wizard_form.security_key.value = "<% getCfgGeneral(1, "WPAPSK1"); %>";
	}
}

function updateWlanMode()
{
	if (document.wizard_form.wlan_disabled.checked == true){
		document.getElementById("wlan_ssid").style.display = "none";
		document.getElementById("wlan_bgn").style.display = "none";
		document.getElementById("wlan_security").style.display = "none";
		document.getElementById("wlan_key").style.display = "none";
	}
	else{
		document.getElementById("wlan_ssid").style.display = "";
		document.getElementById("wlan_bgn").style.display = "none";
		document.getElementById("wlan_security").style.display = "";
		//document.getElementById("wlan_key").style.display = "";
		updateSecurityMode();
	}
}

function Load_Setting()
{
	var wan_connect_mode = '<% getCfgGeneral(1, "wanConnectionMode"); %>';
	var wifi_off = <% getCfgZero(1, "WiFiOff"); %>;
	var PhyMode = <% getCfgZero(1, "WirelessMode"); %>;
	
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
	else 
		document.getElementById("wan_connect_mode").innerHTML = MM_dhcp;
	document.wizard_form.connectionType.value = wan_connect_mode;
	updateConnectionType();
	if (wifi_off == 1)
		document.wizard_form.wlan_disabled.checked = true;
	else
		document.wizard_form.wlan_disabled.checked = false;
		
	if (PhyMode == 0)
		document.wizard_form.wirelessmode.options.selectedIndex = 0;
	else if (PhyMode == 1)
		document.wizard_form.wirelessmode.options.selectedIndex = 1;
	else if (PhyMode == 4)
		document.wizard_form.wirelessmode.options.selectedIndex = 2;
	else if (PhyMode == 9)
		document.wizard_form.wirelessmode.options.selectedIndex = 3;
	else if (PhyMode == 6)
		document.wizard_form.wirelessmode.options.selectedIndex = 4;
	
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
		
	updateWlanMode();
}

function resetForm()
{
	location=location; 
}
</script>
</head>
 
<body onLoad="Load_Setting()" class="mainbody">
<blockquote>
<table width=700><tr><td>
<table width=100% border=0 cellpadding=3 cellspacing=1> 
<tr><td class="title"><script>dw(MM_easywizard)</script></td></tr>
<tr><td><script>dw(JS_msg_wizard)</script></td></tr>
<tr><td><hr></td></tr>
</table>

<form method=post name="wizard_form" action="/goform/setWizard">
<fieldset>
<legend><script>dw(MM_cur_status)</script></legend>
<table width=100% border=0 cellpadding=3 cellspacing=1>
<tr style="display:none">
  <td class="title2" colspan="2"><script>dw(JS_msg95)</script><hr></td>
</tr>
<tr style="display:none">
   <td class="thead"><script>dw('<input type="button" class="button4" name="connectAutoCheck" value="'+BT_auto_detects+'" onClick="connectAutoCheckSubmit();">')</script></td>
   <td><span id="autoCheckInfo"> </span>&nbsp;<span style="color:#FF0000" id="msg"> </span></td>
</tr>
<tr>
   <td class="thead"><script>dw(MM_cur_status)</script>:</td>
   <td><font color="#ff0000"><span id="wan_connect_mode"> </span></font></td>
</tr>
</table>
</fieldset>
<br>

<fieldset>
<legend><script>dw(JS_msg_wizard_wan)</script></legend>
<table width=100% border=0 cellpadding=3 cellspacing=1>
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
  <td><input name="staticNetmask" maxlength=15 value="<% getWanNetmask(); %>"></td>
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
</table>
</fieldset>

<br>
<fieldset>
<legend><script>dw(JS_msg_wizard_wireless)</script></legend>
<table width=100% border=0 cellpadding=3 cellspacing=1>
<tr>
  <td class="thead"><script>dw(MM_disable_wlan)</script>:</td>
  <td><input type="checkbox" name="wlan_disabled" value="ON" onClick="updateWlanMode()"></td>
</tr>
<tr id="wlan_ssid">
  <td class="thead"><script>dw(MM_ssid)</script>:</td>
  <td><input type="text" name="ssid" size="32" maxlength="32" value="<% getCfgToHTML(1, "SSID1"); %>"></td>
</tr>
<tr id="wlan_bgn"> 
  <td class="thead"><script>dw(MM_network_mode)</script>:</td>
  <td><select name="wirelessmode">
    <option value=0>11b/g</option>
    <option value=1>11b</option>
    <option value=4>11g</option>
    <option value=9>11b/g/n</option>
    <option value=6>11n(2.4G)</option>
  </select></td>
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
<tr id="wlan_security_info">
  <td class="thead"><script>dw(MM_notes)</script>:</td>
  <td>WPA-PSK</td>
</tr>
</table>
</fieldset>

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
</blockquote>
</body></html>