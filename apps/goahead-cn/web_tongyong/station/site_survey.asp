<html>
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Expires" content="-1">
<meta http-equiv="Content-Type" content="text/html; charset=<% getCharset(); %>">
<link rel="stylesheet" href="../style/normal_ws.css" type="text/css">
<script language="javascript" src="../js/language_<% getCfgZero(1, "LanguageType"); %>.js"></script>
<script language="javascript" src="../js/common.js"></script>
<script language="javascript">
var g_ssid;
var g_networktype;
var g_channel;
var g_auth;
var g_encry;
var g_bssid;

function countTime()
{
	//var connectstatus = '<!--#include ssi=getStaConnectionStatus() -->';
	//if (connectstatus == 1)  // 0 is NdisMediaStateConnected, 1 is NdisMediaStateDisconnected
		setTimeout("window.location.reload();", 1000*4);
}

function selectedSSIDChange(ssid, bssid, networktype, channel, encry, auth)
{
	g_ssid = ssid;
	g_networktype = networktype;
	g_channel = channel;
	g_auth = auth;
	g_encry = encry;
	g_bssid = bssid

	document.sta_site_survey.connectionButton.disabled=false;
	document.sta_site_survey.addProfileButton.disabled=false;
	document.sta_site_survey.connectedssid.disabled=true;

	//alert("ssid="+ssid+" networktype="+networktype+" channel="+channel+" auth="+auth+" bssid="+bssid);
}

function open_connection_page()
{
	cwin = window.open("site_survey_connection.asp","sta_site_survey_connection","toolbar=no, location=yes, scrollbars=yes, resizable=no, width=660, height=600");
}

function open_profile_page()
{
	pcwin = window.open("add_profile_page.asp","add_profile_page","toolbar=no, location=yes, scrollbars=yes, resizable=no, width=660, height=600");
}

function showConnectionSsid()
{
	cwin.document.forms["sta_site_survey_connection"].Ssid.value = g_ssid;
	cwin.document.forms["sta_site_survey_connection"].bssid.value = g_bssid;

	if (g_networktype == 1){
		if (g_auth.indexOf("WPA2-PSK") >= 0)
			cwin.document.forms["sta_site_survey_connection"].security_infra_mode.value = 7;
		else if (g_auth.indexOf("WPA-PSK") >= 0)
			cwin.document.forms["sta_site_survey_connection"].security_infra_mode.value = 4;
		else if (g_auth.indexOf("WPA2") >= 0)
			cwin.document.forms["sta_site_survey_connection"].security_infra_mode.value = 6;
		else if (g_auth.indexOf("WPA") >= 0)
			cwin.document.forms["sta_site_survey_connection"].security_infra_mode.value = 3;
		else
			cwin.document.forms["sta_site_survey_connection"].security_infra_mode.value = 0;
	}
	else{
		if ( g_auth.indexOf("WPA-NONE") >= 0 || g_auth.indexOf("WPA2-NONE") >= 0)
			cwin.document.forms["sta_site_survey_connection"].security_adhoc_mode.value = 5;
		else
			cwin.document.forms["sta_site_survey_connection"].security_adhoc_mode.value = 0;
	}

	//encry
	if (g_encry.indexOf("WEP") >= 0)
		cwin.document.forms["sta_site_survey_connection"].security_encryp_mode.value = 1;
	else if (g_encry.indexOf("AES") >= 0)
		cwin.document.forms["sta_site_survey_connection"].cipher[1].checked = true;
	else if (g_encry.indexOf("TKIP") >= 0)
		cwin.document.forms["sta_site_survey_connection"].cipher[0].checked = true;
	else
		cwin.document.forms["sta_site_survey_connection"].security_encryp_mode.value = 0;

	cwin.document.forms["sta_site_survey_connection"].network_type.value = g_networktype;
}

function showProfileSsid()
{
	pcwin.document.forms["profile_page"].Ssid.value = g_ssid;
	//alert("site_survey: "+g_ssid);

	if(g_networktype == 1 ){
		if (g_auth.indexOf("WPA2-PSK") >= 0)
			pcwin.document.forms["profile_page"].security_infra_mode.value = 7;
		else if (g_auth.indexOf("WPA-PSK") >= 0)
			pcwin.document.forms["profile_page"].security_infra_mode.value = 4;
		else if (g_auth.indexOf("WPA2") >= 0)
			pcwin.document.forms["profile_page"].security_infra_mode.value = 6;
		else if (g_auth.indexOf("WPA") >= 0)
			pcwin.document.forms["profile_page"].security_infra_mode.value = 3;
		else		
			pcwin.document.forms["profile_page"].security_infra_mode.value = 0;
	}
	else{
		if ( g_auth.indexOf("WPA-NONE") >= 0 || g_auth.indexOf("WPA2-NONE") >= 0)
			pcwin.document.forms["profile_page"].security_adhoc_mode.value = 5;
		else
			pcwin.document.forms["profile_page"].security_adhoc_mode.value = 0;
	}

	//encry
	if (g_encry.indexOf("WEP") >= 0)
		pcwin.document.forms["profile_page"].security_encryp_mode.value = 1;
	else if (g_encry.indexOf("TKIP") >= 0)
		pcwin.document.forms["profile_page"].cipher[0].checked = true;
	else if (g_encry.indexOf("AES") >= 0)
		pcwin.document.forms["profile_page"].cipher[1].checked = true;
	else
		pcwin.document.forms["profile_page"].security_encryp_mode.value = 0;

	pcwin.document.forms["profile_page"].network_type.value = g_networktype;

	pcwin.document.forms["profile_page"].channel.value = g_channel;
}
</script>
</head>
<body>
<table><tr><td>
<h2>Station Site Survey</h2>
<p>Site survey page shows information of APs nearby. You may choose one of these APs connecting or adding it to profile.</p>
<hr />
<form method=post name="sta_site_survey">
<table width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr>
    <td class="title" colspan="8">Site Survey</td>
  </tr>
  <tr>
    <td bgcolor="#C0C0C0">&nbsp;</td>
    <td bgcolor="#C0C0C0">SSID</td>
    <td bgcolor="#C0C0C0">BSSID</td>
    <td bgcolor="#C0C0C0">RSSI</td>
    <td bgcolor="#C0C0C0">Channel</td>
    <td bgcolor="#C0C0C0">Encryption</td>
    <td bgcolor="#C0C0C0">Authentication</td>
    <td bgcolor="#C0C0C0">Network Type</td>
  </tr>
  <% getStaBSSIDList(); %>
</table>

<table width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr>
    <td>
	<input type=text name="connectedssid" size=32 value="<% getStaConnectionSSID(); %>" disabled>&nbsp;&nbsp;&nbsp;
	<input type=button class="button" name="connectionButton" value="Connect" disabled onClick="open_connection_page()">&nbsp;&nbsp;
	<input type=button class="button" value="Rescan" onClick="location.href=location.href">&nbsp;&nbsp;
	<!--
	<input type=button class="button" value="Rescan" onClick="location.href=location.href">&nbsp; 
	<input type=button class="button" value="Rescan" onClick="resetForm();">&nbsp;
	-->
	<input type=button class="button" name="addProfileButton" value="Add Profile" disabled onClick="open_profile_page()">
    </td>
  </tr>
</table>
</form>

</td></tr></table>
</body></html>
