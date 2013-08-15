<html>
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Expires" content="-1">
<meta http-equiv="Content-Type" content="text/html; charset=<% getCharset(); %>">
<link rel="stylesheet" href="../style/normal_ws.css" type="text/css">
<script language="javascript" src="../js/language_<% getCfgZero(1, "LanguageType"); %>.js"></script>
<script language="javascript" src="../js/common.js"></script>
<script language="javascript">
var MBSSID_MAX 	= 4;
var ACCESSPOLICYLIST_MAX = 32;
var changed = 0;
var old_MBSSID;
var defaultShownMBSSID = 0;
var PhyMode  = '<% getCfgZero(1, "WirelessMode"); %>';
var SSID = new Array();
var PreAuth = new Array();
var AuthMode = new Array();
var EncrypType = new Array();
var DefaultKeyID = new Array();
var Key1Type = new Array();
var Key1Str = new Array();
var Key2Type = new Array();
var Key2Str = new Array();
var Key3Type = new Array();
var Key3Str = new Array();
var Key4Type = new Array();
var Key4Str = new Array();
var WPAPSK = new Array();
var RekeyMethod = new Array();
var RekeyInterval = new Array();
var PMKCachePeriod = new Array();
var IEEE8021X = new Array();
var RADIUS_Server = new Array();
var RADIUS_Port = new Array();
var RADIUS_Key = new Array();
var session_timeout_interval = new Array();
var AccessPolicy = new Array();
var AccessControlList = new Array();
var WapiPskType = new Array();
var WapiPsk = new Array();
var WapiAsIpAddr = new Array();
var WapiAsPort = new Array();
var WapiAsCertPath = new Array();
var WapiUserCertPath = new Array();
var wapib = "<% getRaxWAPIBuilt(); %>";
var wpsenable  = '<% getCfgZero(1, "WscModeOption"); %>';
var ht_disallow_tkip = "<% getCfgZero(1, "HT_DisallowTKIP"); %>";

function ASCertUpdate()
{
	makeRequest("/goform/UpdateCert", "as");
}

function UserCertUpdate()
{
	makeRequest("/goform/UpdateCert", "user");
}

function ASContents() 
{
	if (http_request.readyState == 4) {
		if (http_request.status == 200) {
			ASCertUpdateHTML(http_request.responseText);
		} else {
			alert(JS_msg6);
		}
	}
}

function UserContents() 
{
	if (http_request.readyState == 4) {
		if (http_request.status == 200) {
			UserCertUpdateHTML(http_request.responseText);
		} else {
			alert(JS_msg6);
		}
	}
}

function ASCertUpdateHTML(str)
{
	if (str.length > 0){
		var cert = document.security_form.wapicert_ascert;
		cert.options[cert.length] = new Option(str, str);
	}
}

function UserCertUpdateHTML(str)
{
	if (str.length > 0){
		var cert = document.security_form.wapicert_usercert;
		cert.options[cert.length] = new Option(str, str);
	}
}

var http_request = false;
function makeRequest(url, content, handler) 
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
	if (content == "as")
		http_request.onreadystatechange = ASContents;
	else if (content == "user")
		http_request.onreadystatechange = UserContents;
	else
	http_request.onreadystatechange = handler;
	http_request.open('POST', url, true);
	http_request.send(content);
}

function securityHandler() 
{
	if (http_request.readyState == 4) {
		if (http_request.status == 200) {
			parseAllData(http_request.responseText);
			UpdateMBSSIDList();

			// load Access Policy for MBSSID[selected]
			LoadAP();
			ShowAP(defaultShownMBSSID);
		} else {
			//alert(JS_msg6);
		}
	}
}

function deleteAccessPolicyListHandler()
{
	window.location.reload(false);
}

function parseAllData(str)
{
	var all_str = new Array();
	all_str = str.split("\n");

	defaultShownMBSSID = parseInt(all_str[0]);

	for (var i=0; i<all_str.length-1; i++) 
	{
		var fields_str = new Array();
		fields_str = all_str[i+1].split("\r");

		SSID[i] = fields_str[0];
		PreAuth[i] = fields_str[1];
		AuthMode[i] = fields_str[2];
		EncrypType[i] = fields_str[3];
		DefaultKeyID[i] = fields_str[4];
		Key1Type[i] = fields_str[5];
		Key1Str[i] = fields_str[6];
		Key2Type[i] = fields_str[7];
		Key2Str[i] = fields_str[8];
		Key3Type[i] = fields_str[9];
		Key3Str[i] = fields_str[10];
		Key4Type[i] = fields_str[11];
		Key4Str[i] = fields_str[12];
		WPAPSK[i] = fields_str[13];
		RekeyMethod[i] = fields_str[14];
		RekeyInterval[i] = fields_str[15];
		PMKCachePeriod[i] = fields_str[16];
		IEEE8021X[i] = fields_str[17];
		RADIUS_Server[i] = fields_str[18];
		RADIUS_Port[i] = fields_str[19];
		RADIUS_Key[i] = fields_str[20];
		session_timeout_interval[i] = fields_str[21];
		AccessPolicy[i] = fields_str[22];
		AccessControlList[i] = fields_str[23];

		WapiPskType[i] = fields_str[24];
		WapiPsk[i] = fields_str[25];
		WapiAsIpAddr[i] = fields_str[26];
		WapiAsPort[i] = fields_str[27];
		WapiAsCertPath[i] = fields_str[28];
		WapiUserCertPath[i] = fields_str[29];

		/* !!!! IMPORTANT !!!!*/
		if(IEEE8021X[i] == "1")
			AuthMode[i] = "IEEE8021X";

		if(AuthMode[i] == "OPEN" && EncrypType[i] == "NONE" && IEEE8021X[i] == "0")
			AuthMode[i] = "Disable";
	}
}

function formCheck()
{
	// check Access Policy
	for (i=0; i<MBSSID_MAX; i++)
	{
		if (document.getElementById("newap_text_" + i).value != "")
		{
			if (!macCheck(document.getElementById("newap_text_" + i), MM_macaddr))
				return false;

			if ((document.getElementById("newap_text_" + i).value == "<% getWiFiMac("ra0"); %>") || 
					(document.getElementById("newap_text_" + i).value == "<% getLanMac(); %>")) 
			{
				alert(JS_msg77);
				return false;
			}
		}
	}
	
	return true;
}

function submit_apply()
{
	if (formCheck() == true)
	{
		changed = 0;
		//document.security_form.submit();
		//opener.location.reload();
	}
	return true;
}

function ShowAP(MBSSID)
{
	var i;
	for(i=0; i<MBSSID_MAX; i++)
	{
		document.getElementById("apselect_"+i).selectedIndex	= AccessPolicy[i];
		document.getElementById("AccessPolicy_"+i).style.display = "none";
	}

	document.getElementById("AccessPolicy_"+MBSSID).style.display = "";
}

function LoadAP()
{
	for(var i=0; i<SSID.length; i++)
	{
		var j=0;
		var aplist = new Array;

		if(AccessControlList[i].length != 0)
		{
			aplist = AccessControlList[i].split(";");
			for(j=0; j<aplist.length; j++)
			{
				document.getElementById("newap_"+i+"_"+j).value = aplist[j];
			}

			// hide the lastest <td>
			if(j%2)
			{
				document.getElementById("newap_td_"+i+"_"+j).style.display = "none";
				j++;
			}
		}

		// hide <tr> left
		for(; j<ACCESSPOLICYLIST_MAX; j+=2)
		{
			document.getElementById("id_"+i+"_"+j).style.display = "none";
		}
	}
}

function selectMBSSIDChanged()
{
	// check if any security settings changed
	if (changed)
	{
		if (!confirm(JS_msg56))
		{
			document.security_form.ssidIndex.options.selectedIndex = old_MBSSID;
			return false;
		}
		else
			changed = 0;
	}

	var selected = document.security_form.ssidIndex.options.selectedIndex;

	// backup for user cancel action
	old_MBSSID = selected;

	MBSSIDChange(selected);
}

/*
 * When user select the different SSID, this function would be called.
 */ 
function MBSSIDChange(selected)
{
	// update Access Policy for MBSSID[selected]
	ShowAP(selected);

	// clear all new access policy list field
	for(i=0; i<MBSSID_MAX; i++)
		document.getElementById("newap_text_"+i).value = "";

	return true;
}

function delap(mbssid, num)
{
	makeRequest("/goform/APDeleteAccessPolicyList", mbssid+ "," +num, deleteAccessPolicyListHandler);
}

function Load_Setting()
{
	makeRequest("/goform/wirelessGetSecurity", "n/a", securityHandler);
}

function UpdateMBSSIDList()
{
	document.security_form.ssidIndex.length = 0;

	for(var i=0; i<SSID.length; i++)
	{
		var j = document.security_form.ssidIndex.options.length;
		document.security_form.ssidIndex.options[j] = new Option(SSID[i], i, false, false);
	}
	
	document.security_form.ssidIndex.options.selectedIndex = defaultShownMBSSID;
	old_MBSSID = defaultShownMBSSID;
}

function setChange(c)
{
	changed = c;
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
<tr><td class="title"><script>dw(MM_wireless_mac_list)</script></td></tr>
<tr><td><script>dw(JS_msg_wlmacf)</script></td></tr>
<tr><td><hr></td></tr>
</table>

<form method="post" name="security_form" action="/goform/APSecurity">
<input type="hidden" name="submit-url" value="/wireless/macfilter.asp">
<table width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr>
    <td class="thead2"><script>dw(MM_select_ssid)</script>:</td>
    <td><select name="ssidIndex" onChange="selectMBSSIDChanged()"></select></td>
  </tr>
</table>

<script language="javascript">
var aptable;

for(aptable = 0; aptable < MBSSID_MAX; aptable++){
	document.write("<table id=AccessPolicy_"+ aptable +" width=100% border=0 cellpadding=3 cellspacing=1>");
	document.write("<tr><td class=thead2>"+MM_auth_mode+":</td>");
	document.write("<td><select name=apselect_"+ aptable + " id=apselect_"+aptable+" onchange=\"setChange(1)\">");
	document.write("<option value=0 >"+MM_disable+"</option><option value=1 >"+MM_allow+"</option><option value=2 >"+MM_deny+"</option></select></td></tr>");

	document.write("<tr><td class=thead2>"+MM_macaddr+":</td>");
	document.write("<td><input name=newap_text_"+aptable+" id=newap_text_"+aptable+" size=17 maxlength=17></td></tr>");

	for(i=0; i< ACCESSPOLICYLIST_MAX/2; i++){
		input_name = "newap_"+ aptable +"_" + (2*i);
		td_name = "newap_td_"+ aptable +"_" + (2*i);

		document.write(" <tr id=id_"+aptable+"_");
		document.write(i*2);
		document.write("> <td id=");
		document.write(td_name);
		document.write("> <input id=");
		document.write(input_name);
		document.write(" size=17 maxlength=17 readonly> <input value="+BT_delete+" onclick=\"delap("+aptable+", ");
		document.write(2*i);
		document.write(")\" type=button > </td>");

		input_name = "newap_" + aptable + "_" + (2*i+1);
		td_name = "newap_td_" + aptable + "_" + (2*i+1);
		document.write("      <td id=");
		document.write(td_name);
		document.write("> <input id=");
		document.write(input_name);
		document.write(" size=17 maxlength=17 readonly> <input value="+BT_delete+" onclick=\"delap("+aptable+", ");
		document.write(2*i+1);
		document.write(")\" type=button></td> </tr>");
	}

	document.write("</table>");
}
</script>

<br>
<table width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr>
    <td>
      <script>dw('<input type="submit" class="button" value="'+BT_apply+'" onClick="return submit_apply()"> &nbsp; &nbsp;\
      <input type="button" class="button" value="'+BT_reset+'" onClick="resetForm();" >')</script>
    </td>
  </tr>
</table>
</form>

</td></tr></table>
</blockquote>
</body></html>
 