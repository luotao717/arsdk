<html>
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Expires" content="-1">
<meta http-equiv="Content-Type" content="text/html; charset=<% getCharset(); %>">
<link rel="stylesheet" href="../style/normal_ws.css" type="text/css">
<script language="javascript" src="../js/language_<% getCfgZero(1, "LanguageType"); %>.js"></script>
<script language="javascript" src="../js/common.js"></script>
<script language="javascript" src="wps_timer.js"></script>
<script language="javascript">
var wps_status;
var wps_result;

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
		alert('Giving up :( Cannot create an XMLHTTP instance');
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
			WPSUpdateHTML(http_request.responseText);
		} else {
			alert('There was a problem with the request.');
		}
	}
}

function WPSUpdateHTML(str)
{
	var all_str = new Array();
	all_str = str.split("\n");

	if (all_str[0] == "1" || all_str[0] == "0")
		document.getElementById("WPSConfigured").innerHTML = "No";
	else if (all_str[0] == "2")
		document.getElementById("WPSConfigured").innerHTML = "Yes";
	else
		document.getElementById("WPSConfigured").innerHTML = "Unknown";
	
	document.getElementById("WPSSSID").innerHTML = all_str[1];
	document.getElementById("WPSAuthMode").innerHTML = all_str[2];
	document.getElementById("WPSEncryptype").innerHTML = all_str[3];
	document.getElementById("WPSDefaultKeyIndex").innerHTML = all_str[4];

	if (all_str[3] == "WEP")
		document.getElementById("WPSKeyType").innerHTML = "WPS Key(Hex value)";
	else
		document.getElementById("WPSKeyType").innerHTML = "WPS Key(ASCII)";

	document.getElementById("WPSWPAKey").innerHTML = all_str[5];
	document.getElementById("WPSCurrentStatus").innerHTML = all_str[6];

	if (all_str[7] == "-1")
		document.getElementById("WPSInfo").value = "WSC failed";
	else if (all_str[7] == "0")
		document.getElementById("WPSInfo").value = "WSC:" + all_str[6];
	else if (all_str[7] == "1")
		document.getElementById("WPSInfo").value = "WSC Success";
	
	wps_result = all_str[7];
	wps_status = all_str[8];
}

function updateWPS()
{
	clear_progress_bar();
	makeRequest("/goform/updateWPS", "something");
	document.WPSConfig.WPSEnable.disabled = false;
	document.SubmitGenPIN.GenPIN.disabled = false;
	document.SubmitOOB.submitResetOOB.disabled = false;
	document.WPS.PINPBCRadio[0].disabled = false;
	document.WPS.PINPBCRadio[1].disabled = false;
	document.WPS.PIN.disabled = false;
	document.WPS.submitWPS.disabled = false;
	document.wps_cancel.wpsCancel.disabled = true;
	if (wps_result == "1")
		refresh_progress_bar(34);
	else
		refresh_progress_bar(wps_status);
}

function ValidateChecksum(PIN)
{
    var accum = 0;

    accum += 3 * (parseInt(PIN / 10000000) % 10);
    accum += 1 * (parseInt(PIN / 1000000) % 10);
    accum += 3 * (parseInt(PIN / 100000) % 10);
    accum += 1 * (parseInt(PIN / 10000) % 10);
    accum += 3 * (parseInt(PIN / 1000) % 10);
    accum += 1 * (parseInt(PIN / 100) % 10);
    accum += 3 * (parseInt(PIN / 10) % 10);
    accum += 1 * (parseInt(PIN / 1) % 10);

    return ((accum % 10) == 0);
}

function PINPBCFormCheck()
{
	if (document.WPS.PINPBCRadio[0].checked) {	// PIN
		if (document.WPS.PIN.value != "") {// Driver 1.9 supports 4 digit PIN code.
			if (document.WPS.PIN.value.length != 4 && !ValidateChecksum(document.WPS.PIN.value)) {
				alert("PIN number validation failed\n");
				return false;
			}
		}
	}
	return true;
}

function checkSecurity()
{
	var authmode = '<% getCfgGeneral(1, "AuthMode"); %>';
	var ieee8021x = '<% getCfgGeneral(1, "IEEE8021X"); %>';
	var security = new Array(); 

	security = authmode.split(";");
	if (security[0] == "SHARED" || security[0] == "WEPAUTO" || security[0] == "WPA" || 
		security[0] == "WPA2" || security[0] == "WPA1WPA2" || ieee8021x == "1")
		alert("WPS does not support this security mode, please reset to OOB first!");
}

function Load_Setting()
{
	// hide tables first
	document.getElementById("div_wps_status").style.display = "none";
	document.getElementById("div_wps").style.display = "none";
	document.getElementById("div_wps_info").style.display = "none";

	var wpsenable = "<% getCfgZero(1, "WscModeOption"); %>";
	if (wpsenable == "0"){// disable WPS
		document.getElementById("WPSEnable").options.selectedIndex = 0;
	}
	else{// enable WPS
		document.getElementById("WPSEnable").options.selectedIndex = 1;
		checkSecurity();

		document.getElementById("div_wps_status").style.display = "";
		document.getElementById("div_wps").style.display = "";
		document.getElementById("div_wps_info").style.display = "";

		updateWPS();
		InitializeTimer(3);
	}
}

function onPINPBCRadioClick(value)
{
	if (value == 1)	// PIN selected
		document.getElementById("PINRow").style.display = "";
	else	// PBC selected
		document.getElementById("PINRow").style.display = "none";
}

function refresh_progress_bar(index)
{
	var bar_color = "blue";
	var clear = "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"
	var wps_progress;

	switch (1*index) {
		case 3:		// Start WSC Process
			wps_progress = 0;
			break;
		case 4:		// Received EAPOL-Start
			wps_progress = 1;
			break;
		case 5:		// Sending EAP-Req(ID)
		case 6:		// Receive EAP-Rsp(ID)
			wps_progress = 2;
			break;
		case 7:		// Receive EAP-Req with wrong WSC SMI Vendor Id
		case 8:		// Receive EAPReq with wrong WSC Vendor Type
		case 9:		// Sending EAP-Req(WSC_START)
			wps_progress = 3;
			break;
		case 10:	// Send M1
		case 11:	// Received M1
			wps_progress = 4;
			break;
		case 12:	// Send M2
		case 13:	// Received M2
			wps_progress = 5;
			break;
		case 15: 	// Send M3
		case 16:	// Received M3
			wps_progress = 6;
			break;
		case 17:	// Send M4
		case 18:	// Received M4
			wps_progress = 7;
			break;
		case 19:	// Send M5
		case 20:	// Received M5
			wps_progress = 8;
			break;
		case 21:	// Send M6
		case 22:	// Received M6
			wps_progress = 9;
			break;
		case 23:	// Send M7
		case 24:	// Received M7
			wps_progress = 10;
			break;
		case 25:	// Send M8
		case 26:	// Received M8
			wps_progress = 11;
			break;
		case 27:	// Processing EAP Response (ACK)
		case 28:	// Processing EAP Request (Done)
		case 29:	// Processing EAP Response (Done)
			wps_progress = 12;
			break;
		case 30:	// Sending EAP-Fail
		case 34:	// Configured
			wps_progress = 13;
			break;
	}
	for (i = 0; i <= wps_progress; i++) {
		var block = document.getElementById("block" + i);
		block.innerHTML = clear;
		block.style.backgroundColor = bar_color;
	}
}

function clear_progress_bar()
{
	index = 13;
	var bar_color = "white";
	var clear = "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"
		
	for (i = 0; i <= index; i++) {
		var block = document.getElementById("block" + i);
		block.innerHTML = clear;
		block.style.backgroundColor = bar_color;
	}
}

function WPSCancel()
{
	document.WPS.PINPBCRadio[0].disabled = false;
	document.WPS.PINPBCRadio[1].disabled = false;
	document.WPS.PIN.disabled = false;
	document.WPS.submitWPS.disabled = false;
	document.SubmitGenPIN.GenPIN.disabled = false;
	document.SubmitOOB.submitResetOOB.disabled = false;
	document.wps_cancel.submit();
	window.location.reload();
}
</script>
</head>
<body onLoad="Load_Setting()" class="mainbody">
<blockquote>
<table width=700><tr><td>
<table width=100% border=0 cellpadding=3 cellspacing=1> 
<tr><td class="title">WPS Setup</td></tr>
<tr><td>You could setup security easily by choosing PIN or PBC method to do WPS Setting.</td></tr>
<tr><td><hr></td></tr>
</table>

<form method="post" name ="WPSConfig" action="/goform/WPSSetup">
<table width=100% border=0 cellpadding=3 cellspacing=1> 
<tr>
  <td class="thead">WPS Mode</td>
  <td><select id="WPSEnable" name="WPSEnable">
	<option value=0>Disable</option>
	<option value=1>Enable</option>
	</select></td>
</tr>
</table>

<br>
<table width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr>
    <td>
      <input type=submit class=button value="Apply"> &nbsp; &nbsp;
      <input type=button class=button value="Reset" onClick="resetForm();">
    </td>
  </tr>
</table>
</form>

<br />
<table id="div_wps_status" width=100% border=0 cellpadding=3 cellspacing=1> 
<tr>
  <td class="thead">WPS Current Status:</td>
  <td> <span id="WPSCurrentStatus"> </span> </td>
</tr>
<tr>
  <td class="thead">WPS Configured:</td>
  <td> <span id="WPSConfigured"> </span> </td>
</tr>
<tr>
  <td class="thead">WPS SSID:</td>
  <td> <span id="WPSSSID"> </span> </td>
</tr>
<tr>
  <td class="thead">WPS Auth Mode:</td>
  <td> <span id="WPSAuthMode"> </span> </td>
</tr>
<tr>
  <td class="thead">WPS Encryp Type:</td>
  <td> <span id="WPSEncryptype"> </span> </td> 
</tr>
<tr>
  <td class="thead">WPS Default Key Index:</td>
  <td> <span id="WPSDefaultKeyIndex"> </span> </td>
</tr>
<tr>
  <td class="thead" > <span id="WPSKeyType"> </span>:</td>
  <td> <span id="WPSWPAKey"> </span> </td>
</tr>
<form method="post" name="SubmitGenPIN" action="/goform/GenPIN">
<tr>
  <td class="thead">AP PIN:</td>
  <td> <% getPINASP(); %> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<input type=submit class="button" value="Generate" name="GenPIN"></td>
</tr>
</form>
<tr>
  <td colspan="2">
	<form method="post" name ="SubmitOOB" action="/goform/OOB">
		<input type="submit" class="button" value="Reset OOB" name="submitResetOOB">
	</form>
  </td>
</tr>
</table>

<br />
<form method="post" name ="WPS" action="/goform/WPS">
<table id="div_wps" width=100% border=0 cellpadding=3 cellspacing=1> 
<tr>
	<td class="thead">WPS Progress Mode:</td>
	<td><input name="PINPBCRadio" value="1" type="radio" checked onClick="onPINPBCRadioClick(1)">PIN <input name="PINPBCRadio" value="2" type="radio" onClick="onPINPBCRadioClick(2)">PBC</td>
</tr>
<tr id="PINRow">
	<td class="thead">PIN:</td>
	<td><input value="" name="PIN" size="8" maxlength="16" type="text"></td>
</tr>
<tr>
	<td colspan="2"><input type="submit" value="Apply" name="submitWPS" class="button" onClick="return PINPBCFormCheck();"></td>
</tr>
</table>
</form>

<br />
<table id="div_wps_info" style="display:none" width=100% border=0 cellpadding=3 cellspacing=1> 
<tr>
  <td class="title2">WPS Status</td>
</tr>
<tr>
  <td><textarea name="WPSInfo" id="WPSInfo" cols="55" rows="2" wrap="off" readonly="1"></textarea></td>
</tr>
<tr>
  <td>
    <div id="wps_progress_bar">
      <span id="block0">&nbsp;</span>
      <span id="block1">&nbsp;</span>
      <span id="block2">&nbsp;</span>
      <span id="block3">&nbsp;</span>
      <span id="block4">&nbsp;</span>
      <span id="block5">&nbsp;</span>
      <span id="block6">&nbsp;</span>
      <span id="block7">&nbsp;</span>
      <span id="block8">&nbsp;</span>
      <span id="block9">&nbsp;</span>
      <span id="block10">&nbsp;</span>
      <span id="block11">&nbsp;</span>
      <span id="block12">&nbsp;</span>
      <span id="block13">&nbsp;</span>
    </div>
  <form method="post" name="wps_cancel" action="/goform/WPSCancel">
    &nbsp;&nbsp;&nbsp;&nbsp;<input type="button" class="button" value="Reset" name="wpsCancel" onClick="WPSCancel()">
  </form>
  </td>
</tr>
</table>

</td></tr></table>
</blockquote>
</body></html>
