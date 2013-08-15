<html>
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Expires" content="-1">
<meta http-equiv="Content-Type" content="text/html; charset=<% getCharset(); %>">
<link rel="stylesheet" href="../style/normal_ws.css" type="text/css">
<script language="javascript" src="../js/language_<% getCfgZero(1, "LanguageType"); %>.js"></script>
<script language="javascript" src="../js/common.js"></script>
<script language="javascript">
var secs;
var timerID = null;
var timerRunning = false;
var timeout = 3;
var delay = 1000;
var rules_num = <% getIPPortRuleNumsASP(); %>;
var lanIP = "<% getLanIp(); %>";
var lanMask = "<% getLanNetmask(); %>";

function InitializeTimer()
{
	// Set the length of the timer, in seconds
	secs = timeout;
	StopTheClock();
	StartTheTimer();
}

function StopTheClock()
{
	if (timerRunning)
		clearTimeout(timerID);
	timerRunning = false;
}

function StartTheTimer()
{
	if (secs==0){
		StopTheClock();
		timerHandler();
		secs = timeout;
		StartTheTimer();
    }
	else{
		self.status = secs;
		secs = secs - 1;
		timerRunning = true;
		timerID = self.setTimeout("StartTheTimer()", delay);
	}
}

var http_request = false;
function makeRequest(url, content) 
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
    http_request.onreadystatechange = alertContents;
    http_request.open('POST', url, true);
    http_request.send(content);
}

function alertContents() 
{
	if (http_request.readyState == 4) {
		if (http_request.status == 200) {
			updatePacketCount( http_request.responseText);
		} else {
			//alert(JS_msg6);
		}
	}
}

function updatePacketCount(str)
{
	var pc = new Array();
	pc = str.split(" ");
	for(i=0; i < pc.length; i++)
		document.getElementById("pktCnt" + i).innerHTML = pc[i];
}

function timerHandler()
{
	makeRequest("/goform/getRulesPacketCount", "n/a");
}

function deleteClick()
{
    for(i=0; i< rules_num; i++){
		var tmp = eval("document.ipportFilterDelete.delRule"+i);
		if(tmp.checked == true)
			return true;
	}
	alert(JS_msg18);
	return false;
	//return true;
}

function formCheck()
{
	if (rules_num >= 20){
		alert(JS_msg23);
		return false;
	}
	
	if ((document.BasicSettings.portFilterEnabled.options.selectedIndex == 0) && 
		(document.ipportFilter.sip_address.value == "" && document.ipportFilter.dFromPort.value == "" && document.ipportFilter.mac_address.value == ""))
		return true;
	
	if (document.ipportFilter.sip_address.value == "" && document.ipportFilter.dFromPort.value == "" && document.ipportFilter.mac_address.value == ""){
		alert(JS_msg21);
		return false;
	}
		
	// exam dest Port
	if (document.ipportFilter.dFromPort.value != ""){
		if (!isPortMsg(document.ipportFilter.dFromPort.value)) 
			return false;
		
		if (document.ipportFilter.dToPort.value != ""){
			if (!isPortMsg(document.ipportFilter.dToPort.value)) 
				return false;
			
			if (!isPortRange(document.ipportFilter.dFromPort.value,document.ipportFilter.dToPort.value)) {
				alert(JS_msg27);
				return false;
			}
		}
	}	
	
	// exam MAC address
	if (document.ipportFilter.mac_address.value != ""){ 
		if (!isMacMsg(document.ipportFilter.mac_address.value, MM_macaddr))  
			return false; 
	}

	// exam source IP address
	if (document.ipportFilter.sip_address.value != ""){
		if (!isIpAddrMsg(document.ipportFilter.sip_address.value, MM_sourceip)) 
			return false;

		if (!isIpSubnet(document.ipportFilter.sip_address.value, lanMask, lanIP)) {
			alert(JS_msg20);
			return false;
		}
	}	
	
	return true;
}

function defaultPolicyChanged()
{
	if (document.BasicSettings.defaultFirewallPolicy.options.selectedIndex == 0)
		document.ipportFilter.action.options.selectedIndex = 0;
	else
		document.ipportFilter.action.options.selectedIndex = 1;
}
	
function updateState()
{
    if (! rules_num ){
 		disableButton(document.ipportFilterDelete.deleteSelFilterPort);
 		disableButton(document.ipportFilterDelete.reset);
	}
	else{
        enableButton(document.ipportFilterDelete.deleteSelFilterPort);
        enableButton(document.ipportFilterDelete.reset);
	}

	if (document.BasicSettings.defaultFirewallPolicy.options.selectedIndex == 0)
		document.ipportFilter.action.options.selectedIndex = 0;
	else
		document.ipportFilter.action.options.selectedIndex = 1;

	protocolChange();

	if (document.BasicSettings.portFilterEnabled.options.selectedIndex == 1){
		InitializeTimer();	// update packet count

		enableTextField(document.ipportFilter.mac_address);
		enableTextField(document.ipportFilter.dip_address);
		enableTextField(document.ipportFilter.sip_address);
		enableTextField(document.ipportFilter.protocol);
		enableTextField(document.ipportFilter.sFromPort);
		enableTextField(document.ipportFilter.sToPort);
		enableTextField(document.ipportFilter.action);
		enableTextField(document.ipportFilter.comment);
		enableButton(document.ipportFilter.apply);
		enableButton(document.ipportFilter.reset);
	}
	else{
		disableTextField(document.ipportFilter.mac_address);
		disableTextField(document.ipportFilter.dip_address);
		disableTextField(document.ipportFilter.sip_address);
		disableTextField(document.ipportFilter.protocol);
		disableTextField(document.ipportFilter.action);
		disableTextField(document.ipportFilter.comment);
		disableButton(document.ipportFilter.apply);
		disableButton(document.ipportFilter.reset);
	}
}

function actionChanged()
{
	if (document.BasicSettings.defaultFirewallPolicy.options.selectedIndex != document.ipportFilter.action.options.selectedIndex)
		alert(JS_msg25);
}

function protocolChange()
{
	if (document.ipportFilter.protocol.options.selectedIndex == 1 || document.ipportFilter.protocol.options.selectedIndex == 2){
		enableTextField(document.ipportFilter.dFromPort);
		enableTextField(document.ipportFilter.dToPort);
		enableTextField(document.ipportFilter.sFromPort);
		enableTextField(document.ipportFilter.sToPort);
	}
	else{
		disableTextField(document.ipportFilter.dFromPort);
		disableTextField(document.ipportFilter.dToPort);
		disableTextField(document.ipportFilter.sFromPort);
		disableTextField(document.ipportFilter.sToPort);
		document.ipportFilter.dFromPort.value = "";
		document.ipportFilter.dToPort.value = "";
		document.ipportFilter.sFromPort.value = "";
		document.ipportFilter.sToPort.value = "";
	}
}

var xml = false;
function portFilterSubmit(value)
{
	xml = false;
    if (window.XMLHttpRequest) { // Mozilla, Safari,...
        xml = new XMLHttpRequest();
        if (xml.overrideMimeType) {
            xml.overrideMimeType('text/xml');
        }
    } else if (window.ActiveXObject) { // IE
        try {
            xml = new ActiveXObject("Msxml2.XMLHTTP");
        } catch (e) {
            try {
            xml = new ActiveXObject("Microsoft.XMLHTTP");
            } catch (e) {}
        }
    }
    if (!xml) {
        alert(JS_msg5);
        return false;
    }
    xml.onreadystatechange = doChange;
	xml.open('POST', '/goform/ipportBasicSettings', true);
	xml.send('firewallEnabled='+value);
}

function doChange()
{
    if (xml.readyState == 4){
		if (xml.status == 200){
			window.location.reload();
		} else {
			//alert(JS_msg6);
		}
	}
}

function resetForm()
{
	location=location; 
}
</script>
</head>
<body onLoad="updateState()" class="mainbody">
<blockquote>
<table width=700><tr><td>
<table width=100% border=0 cellpadding=3 cellspacing=1> 
<tr><td class="title"><script>dw(MM_ipportf_settings)</script></td></tr>
<tr><td><script>dw(JS_msg_ipportf)</script></td></tr>
<tr><td><hr></td></tr>
</table>

<form method=post name="BasicSettings" action=/goform/ipportBasicSettings>
<input type="hidden" name="submit-url" value="/firewall/port_filtering.asp">
<input type="hidden" name="firewallEnabled">
<table width=100% border=0 cellpadding=3 cellspacing=1> 
<tr>
	<td class="thead"><script>dw(MM_ipportf)</script>:</td>
	<td><select onChange="updateState();portFilterSubmit(this.value)" name="portFilterEnabled">
	<option value=0 <% getIPPortFilterEnableASP(0); %>><script>dw(MM_disable)</script></option>
    <option value=1 <% getIPPortFilterEnableASP(1); %>><script>dw(MM_enable)</script></option>
    </select></td>
</tr>
<tr style="display:none">
	<td class="thead"><script>dw(MM_default_policy)</script>:</td>
	<td><select onChange="defaultPolicyChanged()" name="defaultFirewallPolicy">
	<option value=0 selected><script>dw(MM_accept)</script></option>
	<option value=1><script>dw(MM_drop)</script></option>
	</select></td>
</tr>
</table>
</form>

<form method=post name="ipportFilter" action=/goform/ipportFilter>
<input type="hidden" name="submit-url" value="/firewall/port_filtering.asp">
<table width=100% border=0 cellpadding=3 cellspacing=1> 
<tr>
    <td class="title2" colspan="2"><script>dw(MM_add)</script><hr></td>
</tr>
<tr>
	<td class="thead"><script>dw(MM_macaddr)</script>:</td>
	<td><input type="text" name="mac_address" maxlength="17"></td>
</tr>
<tr style="display:none">
	<td class="thead"><script>dw(MM_destip)</script>:</td>
	<td><input type="text" name="dip_address" maxlength="15"></td>
</tr>
<tr>
	<td class="thead"><script>dw(MM_ipaddr)</script>:</td>
	<td><input type="text" name="sip_address" maxlength="15"></td>
</tr>
<tr>
	<td class="thead"><script>dw(MM_protocol)</script>:</td>
	<td><select onChange="protocolChange()" name="protocol">
	<option value="None"><script>dw(MM_none)</script></option>
	<option value="TCP">TCP</option>
	<option value="UDP">UDP</option>
	<option value="ICMP">ICMP</option>
	</select></td>
</tr>
<tr>
	<td class="thead"><script>dw(MM_portrange)</script>:</td>
	<td><input type="text" size="5" name="dFromPort" maxlength="5"> - <input type="text" size="5" name="dToPort" maxlength="5"> (1-65535)</td>
</tr>
<tr style="display:none">
	<td class="thead"><script>dw(MM_sport_range)</script>:</td>
	<td><input type="text" size="5" name="sFromPort" maxlength="5"> - <input type="text" size="5" name="sToPort" maxlength="5"> (1-65535)</td>
</tr>
<tr style="display:none">
	<td class="thead"><script>dw(MM_action)</script>:</td>
	<td><select onChange="actionChanged()" name="action">
   	<option value="Drop"><script>dw(MM_drop)</script></option>
	<option value="Accept"><script>dw(MM_accept)</script></option>
   	</select></td>
</tr>
<tr>
	<td class="thead"><script>dw(MM_comment)</script>:</td>
	<td><input type="text" name="comment" maxlength="10"></td>
</tr>
</table>

<br>
<table width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr>
    <td>
      <script>dw('<input type=submit class=button value="'+BT_apply+'" name=apply onClick="return formCheck()"> &nbsp; &nbsp;\
      <input type=button class=button value="'+BT_reset+'" name=reset onClick="resetForm();">')</script>
    </td>
  </tr>
</table>
</form>

<br>
<form action=/goform/ipportFilterDelete method=POST name="ipportFilterDelete">
<input type="hidden" name="submit-url" value="/firewall/port_filtering.asp">
<p><b><script>dw(MM_ipportf_list)</script>:<script>document.write(JS_msg68);</script></b></p>
<table width=100% border=1 cellpadding=3 style="border-collapse: collapse" bordercolor="#C8C8C8">
	<tr bgcolor=#f2f2f2>
		<td><b>No.</b></td>
		<td align=center><b><script>dw(MM_macaddr)</script></b></td>
		<!--<td align=center><b><script>dw(MM_destip)</script></b></td>-->
		<td align=center><b><script>dw(MM_ipaddr)</script></b></td>
		<td align=center><b><script>dw(MM_protocol)</script></b></td>
		<td align=center><b><script>dw(MM_portrange)</script></b></td>
		<!--<td align=center><b><script>dw(MM_sport_range)</script></b></td>-->
		<!--<td align=center><b><script>dw(MM_action)</script></b></td>-->
		<td align=center><b><script>dw(MM_comment)</script></b></td>
		<td align=center><b><script>dw(MM_pktcnt)</script></b></td>
	</tr>
	<% showIPPortFilterRulesASP(); %>
</table>

<br>
<table width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr>
    <td>
      <script>dw('<input type=submit class=button value="'+BT_delete+'" name="deleteSelFilterPort" onClick="return deleteClick()"> &nbsp; &nbsp;\
      <input type=button class=button value="'+BT_reset+'" name="reset" onClick="resetForm();">')</script>
    </td>
  </tr>
</table>
</form>

</td></tr></table>
</blockquote>
</body></html>
