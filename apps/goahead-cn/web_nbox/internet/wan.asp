<html>
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Expires" content="-1">
<meta http-equiv="Content-Type" content="text/html; charset=<% getCharset(); %>">
<link rel="stylesheet" href="../style/normal_ws.css" type="text/css">
<script language="javascript" src="../js/language_<% getCfgZero(1, "LanguageType"); %>.js"></script>
<script language="javascript" src="../js/common.js"></script>
<script language="javascript">
var http_request = false;
function macCloneMacFillSubmit()
{
    http_request = false;
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
    http_request.onreadystatechange = doFillMyMAC;
    http_request.open('POST', '/goform/getMyMAC', true);
    http_request.send('n\a');
}

function doFillMyMAC()
{
    if (http_request.readyState == 4) {
		if (http_request.status == 200) {
			document.getElementById("macCloneMac").value = http_request.responseText;
		} else {
			alert(JS_msg48);
		}
	}
}

function macCloneSwitch()
{
	if (document.wanCfg.macCloneEnbl.options.selectedIndex == 1)
		document.getElementById("macCloneMacRow").style.display = "";
	else
		document.getElementById("macCloneMacRow").style.display = "none";
}

function connectionTypeSwitch()
{
	document.getElementById("static").style.display = "none";
	document.getElementById("dhcp").style.display = "none";
	document.getElementById("pppoe").style.display = "none";
	document.getElementById("l2tp").style.display = "none";
	document.getElementById("pptp").style.display = "none";
	document.getElementById("3G").style.display = "none";

	if (document.wanCfg.connectionType.options.selectedIndex == 0) 
	{
		document.getElementById("static").style.display = "";
	}
	else if (document.wanCfg.connectionType.options.selectedIndex == 1) 
	{
		document.getElementById("dhcp").style.display = "";
	}
	else if (document.wanCfg.connectionType.options.selectedIndex == 2) 
	{
		document.getElementById("pppoe").style.display = "";
		pppoeOPModeSwitch();
	}
	else if (document.wanCfg.connectionType.options.selectedIndex == 3) 
	{
		document.getElementById("l2tp").style.display = "";
		l2tpModeSwitch();
		l2tpOPModeSwitch();
	}
	else if (document.wanCfg.connectionType.options.selectedIndex == 4) 
	{
		document.getElementById("pptp").style.display = "";
		pptpModeSwitch();
		pptpOPModeSwitch();
	}
	else if (document.wanCfg.connectionType.options.selectedIndex == 5) 
	{
		document.getElementById("3G").style.display = "";
		config3gTypeSwitch("<% getCfgGeneral(1, "dial3gchoicetype"); %>");
	}
	else 
	{
		document.getElementById("static").style.display = "";
	}
}

function config3gTypeSwitch(value)
{
	if (value == "AUTO") 
	{
		document.wanCfg.Config3G.options.selectedIndex=0;
		document.wanCfg.Dial3G.disabled = true;
		document.wanCfg.User3G.disabled = true;
		document.wanCfg.Password3G.disabled = true;
		document.wanCfg.APN3G.disabled = true;
		
	}
	else if (value == "MANUAL") 
	{
		document.wanCfg.Config3G.options.selectedIndex=1;
		document.wanCfg.Dial3G.disabled = false;
		document.wanCfg.User3G.disabled = false;
		document.wanCfg.Password3G.disabled = false;
		document.wanCfg.APN3G.disabled = false;

	}

}

function l2tpModeSwitch()
{
	if (document.wanCfg.l2tpMode.selectedIndex == 0) 
	{
		document.getElementById("l2tpIp").style.display = "";
		document.getElementById("l2tpNetmask").style.display = "";
		document.getElementById("l2tpGateway").style.display = "";
	}
	else 
	{
		document.getElementById("l2tpIp").style.display = "none";
		document.getElementById("l2tpNetmask").style.display = "none";
		document.getElementById("l2tpGateway").style.display = "none";
	}
}

function pptpModeSwitch()
{
	if (document.wanCfg.pptpMode.selectedIndex == 0) 
	{
		document.getElementById("pptpIp").style.display = "";
		document.getElementById("pptpNetmask").style.display = "";
		document.getElementById("pptpGateway").style.display = "";
	}
	else 
	{
		document.getElementById("pptpIp").style.display = "none";
		document.getElementById("pptpNetmask").style.display = "none";
		document.getElementById("pptpGateway").style.display = "none";
	}
}

function pppoeOPModeSwitch()
{
	document.getElementById("pppoe_redial_period").style.display = "none";
	document.getElementById("pppoe_idle_time").style.display = "none";	
	document.wanCfg.pppoeRedialPeriod.disabled = true;
	document.wanCfg.pppoeIdleTime.disabled = true;
	
	if (document.wanCfg.pppoeOPMode.options.selectedIndex == 0) 
	{
		document.getElementById("pppoe_redial_period").style.display = "";
		document.wanCfg.pppoeRedialPeriod.disabled = false;
	}
	else if (document.wanCfg.pppoeOPMode.options.selectedIndex == 1)
	{
		document.getElementById("pppoe_idle_time").style.display = "";
		document.wanCfg.pppoeIdleTime.disabled = false;
	}
}

function l2tpOPModeSwitch()
{
	document.getElementById("l2tp_redial_period").style.display = "none";
	//document.getElementById("l2tp_idle_time").style.display = "none";	
	document.wanCfg.l2tpRedialPeriod.disabled = true;
	//document.wanCfg.l2tpIdleTime.disabled = true;
	
	if (document.wanCfg.l2tpOPMode.options.selectedIndex == 0) 
	{	
		document.getElementById("l2tp_redial_period").style.display = "";
		document.wanCfg.l2tpRedialPeriod.disabled = false;
	}
	/*
	else if (document.wanCfg.l2tpOPMode.options.selectedIndex == 1)
	{	
		document.getElementById("l2tp_idle_time").style.display = "";	
		document.wanCfg.l2tpIdleTime.disabled = false;
	}
	*/
}

function pptpOPModeSwitch()
{
	document.getElementById("pptp_redial_period").style.display = "none";
	//document.getElementById("pptp_idle_time").style.display = "none";	
	document.wanCfg.pptpRedialPeriod.disabled = true;
	//document.wanCfg.pptpIdleTime.disabled = true;
	
	if (document.wanCfg.pptpOPMode.options.selectedIndex == 0) 
	{
		document.getElementById("pptp_redial_period").style.display = "";
		document.wanCfg.pptpRedialPeriod.disabled = false;
	}
	/*
	else if (document.wanCfg.pptpOPMode.options.selectedIndex == 1)
	{
		document.getElementById("pptp_idle_time").style.display = "";	
		document.wanCfg.pptpIdleTime.disabled = false;
	}
	*/
}

function formCheck()
{
	if (document.wanCfg.connectionType.selectedIndex == 0) //STATIC
	{ 
		if (!ipCheck(document.wanCfg.staticIp, MM_ipaddr, 0)) 
			return false;
		
		if (!maskCheck(document.wanCfg.staticNetmask, MM_submask)) 
			return false;

		if (document.wanCfg.staticGateway.value != "") 
		{
			if (!ipCheck(document.wanCfg.staticGateway, MM_default_gateway, 0)) 
				return false;
				
			if (!subnetCheck(document.wanCfg.staticGateway.value, document.wanCfg.staticNetmask.value, document.wanCfg.staticIp.value)) 
			{
				alert(JS_msg13);
				document.wanCfg.staticGateway.focus();
				return false;
			}
		}
		
		if (document.wanCfg.staticPriDns.value != "") 
		{ 
			if (!ipCheck(document.wanCfg.staticPriDns, MM_pridns, 0)) 
				return false; 
		}
		
		if (document.wanCfg.staticSecDns.value != "") 
		{
			if (!ipCheck(document.wanCfg.staticSecDns, MM_secdns, 0)) 
				return false; 
		}

		//if (!rangeCheckNullMsg(document.wanCfg.staticMtu, 576, 1500, "MTU")) 
	//	{
		//	return false;
		//}
	}
	else if (document.wanCfg.connectionType.selectedIndex == 1) //DHCP
	{ 
		if (!hostnameCheck(document.wanCfg.hostname, MM_hostname))
			return false;
		
		//if (!rangeCheckNullMsg(document.wanCfg.dhcpMtu, 576, 1500, "MTU")) 
		//{
			//return false;
		//}
	}
	else if (document.wanCfg.connectionType.selectedIndex == 2) //PPPOE
	{ 
		if (!blankCheck(document.wanCfg.pppoeUser, MM_username)) 
			return false;
		
		if (!blankCheck(document.wanCfg.pppoePass, MM_password)) 
			return false;
		
		if (document.wanCfg.pppoePass.value != document.wanCfg.pppoePass2.value) 
		{
			alert(JS_msg14);
			document.wanCfg.pppoePass.focus();
			return false;
		}		
		
		if (document.wanCfg.pppoeOPMode.options.selectedIndex == 0)
		{
			if (!numCheckNullMsg(document.wanCfg.pppoeRedialPeriod, "PPPoE "+MM_redial_period)) 
				return false;
		}
		else if (document.wanCfg.pppoeOPMode.options.selectedIndex == 1)
		{
			if (!numCheckNullMsg(document.wanCfg.pppoeIdleTime, "PPPoE "+MM_idle_time)) 
				return false;
		}
		
		//if (!rangeCheckNullMsg(document.wanCfg.pppoeMtu, 1000, 1492, "MTU")) 
		//{
			//return false;
		//}
	}
	else if (document.wanCfg.connectionType.selectedIndex == 3) //L2TP
	{ 
		if (!ipCheck(document.wanCfg.l2tpServer, "L2TP "+MM_server_ipaddr, 0)) 
			return false;

		if (!blankCheck(document.wanCfg.l2tpUser, MM_username)) 
			return false;
		
		if (!blankCheck(document.wanCfg.l2tpPass, MM_password)) 
			return false;

		if (document.wanCfg.l2tpMode.selectedIndex == 0) 
		{
			if (!ipCheck(document.wanCfg.l2tpIp, MM_ipaddr, 0)) 
				return false;
			
			if (!maskCheck(document.wanCfg.l2tpNetmask, MM_submask)) 
				return false;
			
			if (!ipCheck(document.wanCfg.l2tpGateway, MM_default_gateway, 0)) 
				return false;
		}
		
		if (document.wanCfg.l2tpOPMode.selectedIndex == 0)
		{
			if (!numCheckNullMsg(document.wanCfg.l2tpRedialPeriod, "L2TP "+MM_redial_period)) 
				return false;
		}
		/*
		else if (document.wanCfg.l2tpOPMode.options.selectedIndex == 1)
		{
			if (!numCheckNullMsg(document.wanCfg.l2tpIdleTime, "L2TP "+MM_idle_time)) 
				return false;
		}
		*/
	}
	else if (document.wanCfg.connectionType.selectedIndex == 4) //PPTP
	{ 
		if (!ipCheck(document.wanCfg.pptpServer, "PPTP "+MM_server_ipaddr, 0)) 
			return false;

		if (!blankCheck(document.wanCfg.pptpUser, MM_username)) 
			return false;
		
		if (!blankCheck(document.wanCfg.pptpPass, MM_password)) 
			return false;

		if (document.wanCfg.pptpMode.selectedIndex == 0) 
		{
			if (!ipCheck(document.wanCfg.pptpIp, MM_ipaddr, 0)) 
				return false;
			
			if (!maskCheck(document.wanCfg.pptpNetmask, MM_submask)) 
				return false;
			
			if (!ipCheck(document.wanCfg.pptpGateway, MM_default_gateway, 0)) 
				return false;
		}
		
		if (document.wanCfg.pptpOPMode.options.selectedIndex == 0)
		{
			if (!numCheckNullMsg(document.wanCfg.pptpRedialPeriod, "PPTP "+MM_redial_period)) 
				return false;
		}
		/*
		else if(document.wanCfg.pptpOPMode.options.selectedIndex == 1)
		{
			if (!numCheckNullMsg(document.wanCfg.pptpIdleTime, "PPTP "+MM_idle_time)) 
				return false; 
		}
		*/
	}
	else if (document.wanCfg.connectionType.selectedIndex == 5) //3G
	{ 
	}
	else
		return false;

	if (document.wanCfg.macCloneEnbl.options.selectedIndex == 1) 
	{
		if (!blankCheck(document.wanCfg.macCloneMac, MM_macaddr)) 
			return false;
		
		if (!macCheck(document.wanCfg.macCloneMac, MM_macaddr))  
			return false;
	}

	return true;
}

function Load_Setting()
{
	var opmode = "<% getCfgZero(1, "OperationMode"); %>";
	var wan_mode = "<% getCfgGeneral(1, "wanConnectionMode"); %>";
	var l2tpMode = "<% getCfgZero(1, "wan_l2tp_mode"); %>";
	var pptpMode = "<% getCfgZero(1, "wan_pptp_mode"); %>";
	var clone_mode = "<% getCfgZero(1, "macCloneEnabled"); %>";
	var dongle3gb = "<% get3GBuilt(); %>";
	var pppoe_xkjs = "<% getCfgZero(1, "pppoe_xkjs"); %>";
	
	if (opmode == "3")
	{
		document.getElementById("div_macclone_br").style.display = "none";
		document.getElementById("div_macclone").style.display = "none";
	}	
	else
	{
		//document.getElementById("div_macclone_br").style.display = "";
		//document.getElementById("div_macclone").style.display = "";
	}
	
	if (pppoe_xkjs == "1")
		document.getElementById("pppoe_xkjs").style.display = "";
	else
		document.getElementById("pppoe_xkjs").style.display = "none";

	if (dongle3gb == "1")
		document.wanCfg.connectionType.options[5] = new Option("3G","3G",false, false);

	if (wan_mode == "STATIC") 
	{
		document.wanCfg.connectionType.options.selectedIndex = 0;
	}
	else if (wan_mode == "DHCP") 
	{
		document.wanCfg.connectionType.options.selectedIndex = 1;
		document.wanCfg.hostname.value = "<% getCfgGeneral(1, "wan_dhcp_hn"); %>";
	}
	else if (wan_mode == "PPPOE") 
	{
		var pppoe_opmode = "<% getCfgGeneral(1, "wan_pppoe_opmode"); %>";
		var pppoe_optime = "<% getCfgGeneral(1, "wan_pppoe_optime"); %>";
		var spec_type = "<% getCfgGeneral(1, "wan_pppoe_spectype"); %>";

		document.wanCfg.connectionType.options.selectedIndex = 2;
		if (pppoe_opmode == "Manual")
		{
			document.wanCfg.pppoeOPMode.options.selectedIndex = 2;
		}
		else if (pppoe_opmode == "OnDemand")
		{
			document.wanCfg.pppoeOPMode.options.selectedIndex = 1;
			if (pppoe_optime != "")
				document.wanCfg.pppoeIdleTime.value = pppoe_optime;
		}
		else if (pppoe_opmode == "KeepAlive")
		{
			document.wanCfg.pppoeOPMode.options.selectedIndex = 0;
			if (pppoe_optime != "")
				document.wanCfg.pppoeRedialPeriod.value = pppoe_optime;
		}

		pppoeOPModeSwitch();
		
		document.wanCfg.specType.options.selectedIndex = spec_type;
	}
	else if (wan_mode == "L2TP") 
	{
		var l2tp_opmode = "<% getCfgGeneral(1, "wan_l2tp_opmode"); %>";
		var l2tp_optime = "<% getCfgGeneral(1, "wan_l2tp_optime"); %>";
		
		document.wanCfg.connectionType.options.selectedIndex = 3;
		document.wanCfg.l2tpMode.options.selectedIndex = 1*l2tpMode;
		l2tpModeSwitch();
		if (l2tp_opmode == "Manual")
		{
			// document.wanCfg.l2tpOPMode.options.selectedIndex = 2;
			document.wanCfg.l2tpOPMode.options.selectedIndex = 1;
		}
		/*
		else if (l2tp_opmode == "OnDemand")
		{
			document.wanCfg.l2tpOPMode.options.selectedIndex = 1;
			if (l2tp_optime != "")
				document.wanCfg.l2tpIdleTime.value = l2tp_optime;
		}
		*/
		else if (l2tp_opmode == "KeepAlive")
		{
			document.wanCfg.l2tpOPMode.options.selectedIndex = 0;
			if (l2tp_optime != "")
				document.wanCfg.l2tpRedialPeriod.value = l2tp_optime;
		}
		l2tpOPModeSwitch();
	}
	else if (wan_mode == "PPTP") 
	{
		var pptp_opmode = "<% getCfgGeneral(1, "wan_pptp_opmode"); %>";
		var pptp_optime = "<% getCfgGeneral(1, "wan_pptp_optime"); %>";

		document.wanCfg.connectionType.options.selectedIndex = 4;
		document.wanCfg.pptpMode.options.selectedIndex = 1*pptpMode;
		pptpModeSwitch();
		if (pptp_opmode == "Manual")
		{
			// document.wanCfg.pptpOPMode.options.selectedIndex = 2;
			document.wanCfg.pptpOPMode.options.selectedIndex = 1;
			if (pptp_optime != "")
				document.wanCfg.pptpIdleTime.value = pptp_optime;
		}
		/*
		else if (pptp_opmode == "OnDemand")
		{
			document.wanCfg.pptpOPMode.options.selectedIndex = 1;
			if (pptp_optime != "")
				document.wanCfg.pptpIdleTime.value = pptp_optime;
		}
		*/
		else if (pptp_opmode == "KeepAlive")
		{
			document.wanCfg.pptpOPMode.options.selectedIndex = 0;
			if (pptp_optime != "")
				document.wanCfg.pptpRedialPeriod.value = pptp_optime;
		}
		pptpOPModeSwitch();
	}
	else if (wan_mode == "3G") 
	{
		var dev_3g = "<% getCfgGeneral(1, "wan_3g_dev"); %>";
/*
		var w3g_opmode = "<% getCfgGeneral(1, "wan_3g_opmode"); %>";

		if (w3g_opmode == "Manual")
			document.wanCfg.OPMode3G.options.selectedIndex = 1;
		else if (w3g_opmode == "Automatic")
			document.wanCfg.OPMode3G.options.selectedIndex = 0;
		else
			document.wanCfg.OPMode3G.options.selectedIndex = 0;
*/

		document.wanCfg.connectionType.options.selectedIndex = 5;


		//w3GOPModeSwitch();
	}
	else 
	{
		document.wanCfg.connectionType.options.selectedIndex = 0;
	}
	
	connectionTypeSwitch();

	if (clone_mode == 1) 
	{
		document.wanCfg.macCloneEnbl.options.selectedIndex = 1;
		document.wanCfg.macCloneMac.value = "<% getCfgGeneral(1, "macCloneMac"); %>";
	}
	else
	{
		document.wanCfg.macCloneEnbl.options.selectedIndex = 0;
		document.wanCfg.macCloneMac.value = "";
	}
	
	macCloneSwitch();
}

function resetForm()
{
	location=location; 
}

function syncWithMyPcTime()
{
	var currentTime = new Date();

	var seconds = currentTime.getSeconds();
	var minutes = currentTime.getMinutes();
	var hours = currentTime.getHours();
	var month = currentTime.getMonth() + 1;
	var day = currentTime.getDate();
	var year = currentTime.getFullYear();

	var seconds_str = " ";
	var minutes_str = " ";
	var hours_str = " ";
	var month_str = " ";
	var day_str = " ";
	var year_str = " ";

	if(seconds < 10)
		seconds_str = "0" + seconds;
	else
		seconds_str = ""+seconds;

	if(minutes < 10)
		minutes_str = "0" + minutes;
	else
		minutes_str = ""+minutes;

	if(hours < 10)
		hours_str = "0" + hours;
	else
		hours_str = ""+hours;

	if(month < 10)
		month_str = "0" + month;
	else
		month_str = ""+month;

	if(day < 10)
		day_str = "0" + day;
	else
		day_str = day;

	document.wanCfg.syncWithPcTime.value = month_str + day_str + hours_str + minutes_str + year + " ";
}
</script>
</head>
<body onLoad="Load_Setting()">
<table width=700><tr><td>
<table width=100% border=0 cellpadding=3 cellspacing=1> 
<tr><td class="title"><script>dw(MM_wan_settings)</script></td></tr>
<tr style="display:none"><td><script>dw(JS_msg150)</script></td></tr>
<tr><td><hr></td></tr>
</table>

<form method=post name="wanCfg" action="/goform/setWan">
<input type="hidden" name="submit-url" value="/internet/wan.asp">
<input type="hidden" name="syncWithPcTime" value="081510102012">
<table width=100% border=0 cellpadding=3 cellspacing=1> 
<tr>
  <td class="thead"><script>dw(MM_connection)</script>:</td>
  <td><select name="connectionType" onChange="connectionTypeSwitch();">
      <option value="STATIC"><script>dw(MM_staticip)</script></option>
      <option value="DHCP"><script>dw(MM_dhcp)</script></option>
      <option value="PPPOE"><script>dw(MM_pppoe)</script></option>
      <option value="L2TP">L2TP</option>
      <option value="PPTP">PPTP</option>
    </select></td>
</tr>
</table>

<table id="static" width=100% border=0 cellpadding=3 cellspacing=1> 
<tr>
  <td class="thead"><script>dw(MM_ipaddr)</script>:</td>
  <td><input name="staticIp" maxlength=15 value="<% getWanIp(); %>"></td>
</tr>
<tr>
  <td class="thead"><script>dw(MM_submask)</script>:</td>
  <td><input name="staticNetmask" maxlength=15 value="<% getWanNetmask(); %>"></td>
</tr>
<tr>
  <td class="thead"><script>dw(MM_default_gateway)</script></td>
  <td><input name="staticGateway" maxlength=15 value="<% getWanGateway(); %>"></td>
</tr>
<tr>
  <td class="thead"><script>dw(MM_pridns)</script>:</td>
  <td><input name="staticPriDns" maxlength=15 value="<% getDns(1); %>"></td>
</tr>
<tr>
  <td class="thead"><script>dw(MM_secdns)</script>:</td>
  <td><input name="staticSecDns" maxlength=15 value="<% getDns(2); %>"> (<script>dw(MM_optional)</script>)</td>
</tr>
<tr style="display:none">
  <td class="thead">MTU:</td>
  <td><input name="staticMtu" maxlength=4 size=5 value="<% getCfgGeneral(1, "wan_static_mtu"); %>"> (576-1500)</td>
</tr>
</table>

<table id="dhcp" width=100% border=0 cellpadding=3 cellspacing=1> 
<tr>
  <td class="thead"><script>dw(MM_hostname)</script>:</td>
  <td><input type=text name="hostname" maxlength=32 value=""> (<script>dw(MM_optional)</script>)</td>
</tr>
<tr style="display:none">
  <td class="thead">MTU</td>
  <td><input name="dhcpMtu" maxlength=4 size=5 value="<% getCfgGeneral(1, "wan_dhcp_mtu"); %>"> (576-1500)</td>
</tr>
</table>

<table id="pppoe" width=100% border=0 cellpadding=3 cellspacing=1> 
<tr>
  <td class="thead"><script>dw(MM_username)</script>:</td>
  <td><input name="pppoeUser" maxlength=32 value="<% getCfgGeneral(1, "wan_pppoe_user"); %>"></td>
</tr>
<tr>
  <td class="thead"><script>dw(MM_password)</script>:</td>
  <td><input type="password" name="pppoePass" maxlength=32 value="<% getCfgGeneral(1, "wan_pppoe_pass"); %>"></td>
</tr>
<tr>
  <td class="thead"><script>dw(MM_conpassword)</script>:</td>
  <td><input type="password" name="pppoePass2" maxlength=32 value="<% getCfgGeneral(1, "wan_pppoe_pass"); %>"></td>
</tr>
<tr style="display:none">
  <td class="thead">MTU:</td>
  <td><input name="pppoeMtu" maxlength=4 size=5 value="<% getCfgGeneral(1, "wan_pppoe_mtu"); %>"> (1000-1492)</td>
</tr>
<tr id="pppoe_xkjs" style="display:none">
  <td class="thead"><script>dw(MM_spec_connection)</script>:</td>
  <td><select name="specType">
      <option value="0"><script>dw(MM_normal)</script></option>
      <option value="1"><script>dw(MM_hunan_telecom)</script> 1</option>
      <option value="2"><script>dw(MM_hunan_telecom)</script> 2</option>
      <option value="3"><script>dw(MM_henan_netcom)</script></option>
      <!--<option value="4"><script>dw(MM_jiangxi_telecom)</script></option>-->
    </select></td>
</tr>
<tr>
  <td class="thead"><script>dw(MM_connect_mode)</script>:</td>
  <td><select name="pppoeOPMode" onChange="pppoeOPModeSwitch()">
      <option value="KeepAlive"><script>dw(MM_keep_alive)</script></option>
      <!--<option value="OnDemand"><script>dw(MM_ondemand)</script></option>
      <option value="Manual"><script>dw(MM_manual)</script></option>-->
    </select> 
    <span id="pppoe_redial_period" style="display:none">
	<script>dw(MM_redial_period)</script>
    <input type="text" name="pppoeRedialPeriod" maxlength="5" size="3" value="60">
    <script>dw(MM_seconds)</script>
    </span>
    
    <span id="pppoe_idle_time" style="display:none">
	<script>dw(MM_idle_time)</script>
    <input type="text" name="pppoeIdleTime" maxlength="3" size="2" value="5">
    <script>dw(MM_minutes)</script>
    </span></td>
</tr>
</table>

<table id="l2tp" width=100% border=0 cellpadding=3 cellspacing=1> 
<tr>
  <td class="thead">L2TP <script>dw(MM_server_ipaddr)</script>:</td>
  <td><input name="l2tpServer" maxlength="15" value="<% getCfgGeneral(1, "wan_l2tp_server"); %>"></td>
</tr>
<tr>
  <td class="thead"><script>dw(MM_username)</script>:</td>
  <td><input name="l2tpUser" maxlength="20" value="<% getCfgGeneral(1, "wan_l2tp_user"); %>"></td>
</tr>
<tr>
  <td class="thead"><script>dw(MM_password)</script>:</td>
  <td><input type="password" name="l2tpPass" maxlength="32" value="<% getCfgGeneral(1, "wan_l2tp_pass"); %>"></td>
</tr>
<tr>
  <td class="thead"><script>dw(MM_address_mode)</script>:</td>
  <td><select name="l2tpMode" onChange="l2tpModeSwitch()">
      <option value="0"><script>dw(MM_static)</script></option>
      <option value="1"><script>dw(MM_dynamic)</script></option>
    </select></td>
</tr>
<tr id="l2tpIp">
  <td class="thead"><script>dw(MM_ipaddr)</script>:</td>
  <td><input name="l2tpIp" maxlength=15 value="<% getCfgGeneral(1, "wan_l2tp_ip"); %>"></td>
</tr>
<tr id="l2tpNetmask">
  <td class="thead"><script>dw(MM_submask)</script>:</td>
  <td><input name="l2tpNetmask" maxlength=15 value="<% getCfgGeneral(1, "wan_l2tp_netmask"); %>"></td>
</tr>
<tr id="l2tpGateway">
  <td class="thead"><script>dw(MM_default_gateway)</script></td>
  <td><input name="l2tpGateway" maxlength=15 value="<% getCfgGeneral(1, "wan_l2tp_gateway"); %>"></td>
</tr>
<tr>
  <td class="thead"><script>dw(MM_connect_mode)</script>:</td>
  <td><select name="l2tpOPMode" onChange="l2tpOPModeSwitch()">
      <option value="KeepAlive"><script>dw(MM_keep_alive)</script></option>
      <!--<option value="OnDemand"><script>dw(MM_ondemand)</script></option>      
      <option value="Manual"><script>dw(MM_manual)</script></option>-->
    </select>
    <span id="l2tp_redial_period" style="display:none">
	<script>dw(MM_redial_period)</script>
    <input type="text" name="l2tpRedialPeriod" maxlength="5" size="3" value="60">
    <script>dw(MM_seconds)</script>
    </span>
    <!--
    <span id="l2tp_idle_time" style="display:none">
	<script>dw(MM_idle_time)</script>
    <input type="text" name="l2tpIdleTime" maxlength="3" size="2" value="5">
    <script>dw(MM_minutes)</script>
    </span>
    --></td>
</tr>
</table>

<table id="pptp" width=100% border=0 cellpadding=3 cellspacing=1> 
<tr>
  <td class="thead">PPTP <script>dw(MM_server_ipaddr)</script>:</td>
  <td><input name="pptpServer" maxlength="15" value="<% getCfgGeneral(1, "wan_pptp_server"); %>"></td>
</tr>
<tr>
  <td class="thead"><script>dw(MM_username)</script>:</td>
  <td><input name="pptpUser" maxlength="20" value="<% getCfgGeneral(1, "wan_pptp_user"); %>"></td>
</tr>
<tr>
  <td class="thead"><script>dw(MM_password)</script>:</td>
  <td><input type="password" name="pptpPass" maxlength="32" value="<% getCfgGeneral(1, "wan_pptp_pass"); %>"></td>
</tr>
<tr>
  <td class="thead"><script>dw(MM_address_mode)</script>:</td>
  <td><select name="pptpMode" onChange="pptpModeSwitch()">
      <option value="0"><script>dw(MM_static)</script></option>
      <option value="1"><script>dw(MM_dynamic)</script></option>
    </select></td>
</tr>
<tr id="pptpIp">
  <td class="thead"><script>dw(MM_ipaddr)</script>:</td>
  <td><input name="pptpIp" maxlength=15 value="<% getCfgGeneral(1, "wan_pptp_ip"); %>"></td>
</tr>
<tr id="pptpNetmask">
  <td class="thead"><script>dw(MM_submask)</script>:</td>
  <td><input name="pptpNetmask" maxlength=15 value="<% getCfgGeneral(1, "wan_pptp_netmask"); %>"></td>
</tr>
<tr id="pptpGateway">
  <td class="thead"><script>dw(MM_default_gateway)</script></td>
  <td><input name="pptpGateway" maxlength=15 value="<% getCfgGeneral(1, "wan_pptp_gateway"); %>"></td>
</tr>
<tr>
  <td class="thead"><script>dw(MM_connect_mode)</script>:</td>
  <td><select name="pptpOPMode" onChange="pptpOPModeSwitch()">
      <option value="KeepAlive"><script>dw(MM_keep_alive)</script></option>
      <!--<option value="OnDemand"><script>dw(MM_ondemand)</script></option>      
      <option value="Manual"><script>dw(MM_manual)</script></option>-->
    </select>
    <span id="pptp_redial_period" style="display:none">
	<script>dw(MM_redial_period)</script>
    <input type="text" name="pptpRedialPeriod" maxlength="5" size="3" value="60">
    <script>dw(MM_seconds)</script>
    </span>
    <!--
    <span id="pptp_idle_time" style="display:none">
	<script>dw(MM_idle_time)</script>
    <input type="text" name="pptpIdleTime" maxlength="3" size="2" value="5">
    <script>dw(MM_minutes)</script>
    </span>
    --></td>
</tr>
</table>

<table id="3G" width=100% border=0 cellpadding=3 cellspacing=1> 
<tr>
  <td class="thead"><script>dw(MM_3gnet_configmode)</script>:</td>
  <td><select name="Config3G" onChange="config3gTypeSwitch(this.value);">
      <option value="auto"><script>dw(MM_3gnet_autoconfig)</script></option>
      <option value="manual"><script>dw(MM_3gnet_manualconfig)</script></option>
    </select></td>
</tr>
<tr>
  <td class="thead">PIN:</td>
  <td><input name="PIN3G" maxlength=128 value="<% getCfgGeneral(1, "dial3gsimpin"); %>"></td>
</tr>
<tr>
  <td class="thead"><script>dw(MM_3gnet_dialnum)</script>:</td>
  <td><input name="Dial3G" maxlength=128 value="<% getCfgGeneral(1, "dial3gnum"); %>"></td>
</tr>
<tr>
  <td class="thead"><script>dw(MM_3gnet_username)</script>:</td>
  <td><input name="User3G" maxlength=128 value="<% getCfgGeneral(1, "dial3gusername"); %>"></td>
</tr>
<tr>
  <td class="thead"><script>dw(MM_3gnet_password)</script>:</td>
  <td><input name="Password3G" maxlength=128 value="<% getCfgGeneral(1, "dial3gpassword"); %>"></td>
</tr>
<tr>
  <td class="thead">APN:</td>
  <td><input name="APN3G" maxlength=128 value="<% getCfgGeneral(1, "dial3gapn"); %>"></td>
</tr>
<tr>
<!--
<tr>
  <td class="thead">Connection Mode:</td>
  <td colspan="2"><select name="OPMode3G" onChange="w3GOPModeSwitch()">
      <option value="Automatic">Auto</option>
      <option value="Manual">Manual</option>
    </select></td>
</tr>
-->
</table>

<br id="div_macclone_br" style="display:none">
<table id="div_macclone" style="display:none" width=100% border=0 cellpadding=3 cellspacing=1> 
<tr>
  <td class="title2" colspan="2"><script>dw(MM_clone_mac_settings)</script><hr></td>
</tr>
<tr>
  <td class="thead"><script>dw(MM_clone_mac)</script>:</td>
  <td><select name="macCloneEnbl" onChange="macCloneSwitch()">
      <option value="0"><script>dw(MM_disable)</script></option>
      <option value="1"><script>dw(MM_enable)</script></option>
    </select></td>
</tr>
<tr id="macCloneMacRow">
  <td class="thead"><script>dw(MM_macaddr)</script>:</td>
  <td><input name="macCloneMac" id="macCloneMac" maxlength=17 value=""> 
  <script>dw('<input type="button" class=button3 value="'+BT_clone_mac+'" onClick="macCloneMacFillSubmit();">')</script></td>
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
