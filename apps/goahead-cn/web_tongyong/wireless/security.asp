<html>
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Expires" content="-1">
<meta http-equiv="Content-Type" content="text/html; charset=<% getCharset(); %>">
<link rel="stylesheet" href="../style/normal_ws.css" type="text/css">
<script language="javascript" src="../js/language_<% getCfgZero(1, "LanguageType"); %>.js"></script>
<script language="javascript" src="../js/common.js"></script>
<script language="javascript">
var MBSSID_MAX 	= 8;
var ACCESSPOLICYLIST_MAX = 64;
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
			LoadFields(defaultShownMBSSID);

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
	var securitymode = document.security_form.security_mode.value;
	if (securitymode == "Disable")
	{
		if (wpsenable != "0") 
			alert(JS_msg69);
	}
	else if (securitymode == "OPEN" || securitymode == "SHARED" ||securitymode == "WEPAUTO")
	{
		if (!check_wep(securitymode))  
			return false;
		
		if (wpsenable != "0") 
			alert(JS_msg70);
	} 
	else if (securitymode == "WPAPSK" || securitymode == "WPA2PSK" || securitymode == "WPAPSKWPA2PSK" /* || security_mode == 5 */) 
	{
		var wpakey = document.security_form.passphrase.value;
		if (wpakey.length == 0)
		{
			alert(JS_msg16);
			document.security_form.passphrase.focus();
			return false;
		}
		
		if (wpakey.length < 8)
		{
			alert(JS_msg71);
			document.security_form.passphrase.focus();
			return false;
		}
		
		if (!stringCheck(document.security_form.passphrase, MM_wpa_key))
			return false;
		
		if (wpakey.length == 64 && !hexCheck(document.security_form.passphrase, MM_wpa_key)) 
		{
			alert(JS_msg72);
			document.security_form.passphrase.focus();
			return false;
		}
		
		if (check_wpa() == false)  
			return false;
	}
	else if (securitymode == "IEEE8021X")//802.1x
	{
		if (document.security_form.ieee8021x_wep[0].checked == false && document.security_form.ieee8021x_wep[1].checked == false)
		{
			alert(JS_msg73);
			return false;
		}
		
		if (check_radius() == false)  
			return false;
	}
	else if (securitymode == "WPA" || securitymode == "WPA1WPA2") //WPA or WPA1WP2 mixed mode
	{
		if (check_wpa() == false)  
			return false;
		
		if (check_radius() == false)  
			return false;
	}
	else if (securitymode == "WPA2")// WPA2
	{ 
		if (check_wpa() == false)  
			return false;
		
		if (document.security_form.PreAuthentication[0].checked == false && document.security_form.PreAuthentication[1].checked == false)
		{
			alert(JS_msg74);
			return false;
		}
		
		if (!numCheckNullMsg(document.security_form.PMKCachePeriod, MM_pmk_cache_period)) 
			return false;
		
		if (check_radius() == false)  
			return false;
	}
	else if (securitymode == "WAICERT")
	{
		if (check_as() == false)  
			return false;
	}
	else if (securitymode == "WAIPSK")
	{
		var pskey = document.security_form.wapipsk_prekey.value;
		if (pskey.length == 0 || pskey.length < 8)
		{
			alert(JS_msg75);
			document.security_form.wapipsk_prekey.focus();
			return false;
		}
		
		if (!stringCheck(document.security_form.wapipsk_prekey, "WAPI PSK pre-Shared Key"))
			return false;

		if (document.security_form.wapipsk_keytype.selectedIndex == 0 && (pskey.length%2) != 0)
		{
			alert(JS_msg76);
			return false;
		}
	}

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

function check_wpa()
{
	if (document.security_form.cipher[0].checked != true && 
	   	document.security_form.cipher[1].checked != true &&
	   	document.security_form.cipher[2].checked != true)
	{
	   	alert(JS_msg78);
	   	return false;
	}

	// there is no tkip-aes mixed mode in WPA-PSK.
	if ((document.security_form.security_mode.value == "WPA" || 
		document.security_form.security_mode.value == "WPAPSK") && 
		document.security_form.cipher[2].checked == true)
	{
		document.security_form.cipher[0].checked = true;
		document.security_form.cipher[1].checked = false;
		document.security_form.cipher[2].checked = false;
	}
	
	if (!numCheckMsg(document.security_form.keyRenewalInterval, MM_key_renewal_interval)) 
		return false;
					
	if (document.security_form.keyRenewalInterval.value == 0)
		alert(JS_msg79);

	if (ht_disallow_tkip == "1" && document.security_form.cipher[0].checked)
		alert(JS_msg80);
	
	return true;
}

function check_as()
{
	if (!ipCheck(document.security_form.wapicert_asipaddr, "Authenticator Server IP address", 0)) 
		return false;
		
	if (!portCheckNullMsg(document.security_form.wapicert_asport, "Authenticator Server port")) 
		return false;
}

function check_radius()
{
	if (!ipCheck(document.security_form.RadiusServerIP, "Radius"+MM_ipaddr, 0)) 
		return false;
		
	if (!portCheckNullMsg(document.security_form.RadiusServerPort, "Radius"+MM_port)) 
		return false;
		
	if (!stringCheck(document.security_form.RadiusServerSecret, "Radius"+MM_shared_secret))
		return false;	
			
	if (document.security_form.RadiusServerSessionTimeout.value.length)
	{
		if (!numCheckMsg(document.security_form.RadiusServerSessionTimeout, "Radius"+MM_session_time_out))
			return false;
	}
	
	return true;
}

function check_wep(securitymode)
{
/*
	var defaultid = document.security_form.wep_default_key.value;
	if (defaultid == 1)
		var keyvalue = document.security_form.wep_key_1.value;
	else if (defaultid == 2)
		var keyvalue = document.security_form.wep_key_2.value;
	else if (defaultid == 3)
		var keyvalue = document.security_form.wep_key_3.value;
	else if (defaultid == 4)
		var keyvalue = document.security_form.wep_key_4.value;

	if (keyvalue.length == 0 && (securitymode == "OPEN" || securitymode == "SHARED" || securitymode == "WEPAUTO"))// shared wep  || md5
	{ 
		alert(JS_msg52+'!');
		return false;
	}
*/

	var keylength = document.security_form.wep_key_1.value.length;
	
	if (keylength == 0)
	{ 
		alert(JS_msg52+'!');
		return false;
	}
	
	if (keylength != 0)
	{
		if (document.security_form.WEP1Select.options.selectedIndex == 0){
			if (keylength != 5 && keylength != 13) 
			{
				//alert(JS_msg53+"1!");
				alert(JS_msg53+"!");
				return false;
			}
			
			//if (!stringCheck(document.security_form.wep_key_1, MM_wep_key+1))
			if (!stringCheck(document.security_form.wep_key_1, MM_wep_key))
				return false;
		}
		if (document.security_form.WEP1Select.options.selectedIndex == 1)
		{
			if (keylength != 10 && keylength != 26) 
			{
				//alert(JS_msg54+"1!");
				alert(JS_msg54+"!");
				return false;
			}
			
			//if (!hexCheck(document.security_form.wep_key_1, MM_wep_key+1))
			if (!hexCheck(document.security_form.wep_key_1, MM_wep_key))
				return false;
		}
	}

	keylength = document.security_form.wep_key_2.value.length;
	if (keylength != 0)
	{
		if (document.security_form.WEP2Select.options.selectedIndex == 0)
		{
			if (keylength != 5 && keylength != 13) 
			{
				alert(JS_msg53+"2!");
				return false;
			}
			
			if (!stringCheck(document.security_form.wep_key_2, MM_wep_key+2))
				return false;		
		}
		if (document.security_form.WEP2Select.options.selectedIndex == 1)
		{
			if (keylength != 10 && keylength != 26) 
			{
				alert(JS_msg54+"2!");
				return false;
			}
			
			if (!hexCheck(document.security_form.wep_key_2, MM_wep_key+2))
				return false;
		}
	}

	keylength = document.security_form.wep_key_3.value.length;
	if (keylength != 0)
	{
		if (document.security_form.WEP3Select.options.selectedIndex == 0){
			if (keylength != 5 && keylength != 13) 
			{
				alert(JS_msg53+"3!");
				return false;
			}
			
			if (!stringCheck(document.security_form.wep_key_3, MM_wep_key+3))
				return false;
		}
		if (document.security_form.WEP3Select.options.selectedIndex == 1)
		{
			if (keylength != 10 && keylength != 26) 
			{
				alert(JS_msg54+"3!");
				return false;
			}
			
			if (!hexCheck(document.security_form.wep_key_3, MM_wep_key+3))
				return false;			
		}
	}

	keylength = document.security_form.wep_key_4.value.length;
	if (keylength != 0)
	{
		if (document.security_form.WEP4Select.options.selectedIndex == 0)
		{
			if (keylength != 5 && keylength != 13) 
			{
				alert(JS_msg53+"4!");
				return false;
			}
			
			if (!stringCheck(document.security_form.wep_key_4, MM_wep_key+4))
				return false;		
		}
		if (document.security_form.WEP4Select.options.selectedIndex == 1)
		{
			if(keylength != 10 && keylength != 26) 
			{
				alert(JS_msg54+"4!");
				return false;
			}
			
			if (!hexCheck(document.security_form.wep_key_4, MM_wep_key+4))
				return false;		
		}
	}
	
	if (ht_disallow_tkip == "1")
		alert(JS_msg80);
	
	return true;
}

function securityMode(c_f)
{
	changed = c_f;
	document.getElementById("div_wep").style.display = "none";
	document.getElementById("div_wpa").style.display = "none";
	document.getElementById("div_wpa_algorithms").style.display = "none";
	document.getElementById("wpa_passphrase").style.display = "none";
	document.getElementById("wpa_key_renewal_interval").style.display = "none";
	document.getElementById("wpa_PMK_Cache_Period").style.display = "none";
	document.getElementById("wpa_preAuthentication").style.display = "none";
	document.security_form.cipher[0].disabled = true;
	document.security_form.cipher[1].disabled = true;
	document.security_form.cipher[2].disabled = true;
	document.security_form.passphrase.disabled = true;
	document.security_form.keyRenewalInterval.disabled = true;
	document.security_form.PMKCachePeriod.disabled = true;
	document.security_form.PreAuthentication.disabled = true;

	// 802.1x
	document.getElementById("div_radius_server").style.display = "none";
	document.getElementById("div_8021x_wep").style.display = "none";
	document.security_form.ieee8021x_wep.disabled = true;
	document.security_form.RadiusServerIP.disabled = true;
	document.security_form.RadiusServerPort.disabled = true;
	document.security_form.RadiusServerSecret.disabled = true;	
	document.security_form.RadiusServerSessionTimeout.disabled = true;
	document.security_form.RadiusServerIdleTimeout.disabled = true;	

	// wapi
	document.getElementById("div_wapi_psk").style.display = "none";
	document.security_form.wapipsk_prekey.disabled = true;
	document.security_form.wapipsk_keytype.disabled = true;
	document.getElementById("div_wapi_cert").style.display = "none";
	document.security_form.wapicert_asipaddr.disabled = true;
	document.security_form.wapicert_asport.disabled = true;
	document.security_form.wapicert_ascert.disabled = true;
	document.security_form.wapicert_usercert.disabled = true;

	var security_mode = document.security_form.security_mode.value;
	if (security_mode == "OPEN" || security_mode == "SHARED" ||security_mode == "WEPAUTO")
	{
		document.getElementById("div_wep").style.display = "";
	}
	else if (security_mode == "WPAPSK" || security_mode == "WPA2PSK" || security_mode == "WPAPSKWPA2PSK")
	{
		<!-- WPA -->
		document.getElementById("div_wpa").style.display = "";
		document.getElementById("div_wpa_algorithms").style.display = "";
		document.security_form.cipher[0].disabled = false;
		document.security_form.cipher[1].disabled = false;

		// deal with TKIP-AES mixed mode
		if (security_mode == "WPA2PSK" || security_mode == "WPAPSKWPA2PSK")
			document.security_form.cipher[2].disabled = false;

		document.getElementById("wpa_passphrase").style.display = "";
		document.security_form.passphrase.disabled = false;
		document.getElementById("wpa_key_renewal_interval").style.display = "";
		document.security_form.keyRenewalInterval.disabled = false;
	}
	else if (security_mode == "WPA" || security_mode == "WPA2" || security_mode == "WPA1WPA2")//wpa enterprise
	{ 
		document.getElementById("div_wpa").style.display = "";
		document.getElementById("div_wpa_algorithms").style.display = "";
		document.security_form.cipher[0].disabled = false;
		document.security_form.cipher[1].disabled = false;
		document.getElementById("wpa_key_renewal_interval").style.display = "";
		document.security_form.keyRenewalInterval.disabled = false;
	
		<!-- 802.1x -->
		document.getElementById("div_radius_server").style.display = "";
		document.security_form.RadiusServerIP.disabled = false;
		document.security_form.RadiusServerPort.disabled = false;
		document.security_form.RadiusServerSecret.disabled = false;	
		document.security_form.RadiusServerSessionTimeout.disabled = false;
		document.security_form.RadiusServerIdleTimeout.disabled = false;	

		if (security_mode == "WPA2" || security_mode == "WPA1WPA2")
			document.security_form.cipher[2].disabled = false;

		if (security_mode == "WPA2")
		{
			document.getElementById("wpa_preAuthentication").style.display = "";
			document.security_form.PreAuthentication.disabled = false;
			document.getElementById("wpa_PMK_Cache_Period").style.display = "";
			document.security_form.PMKCachePeriod.disabled = false;
		}
	}
	else if (security_mode == "IEEE8021X")// 802.1X-WEP
	{ 
		document.getElementById("div_8021x_wep").style.display = "";
		document.getElementById("div_radius_server").style.display = "";
		document.security_form.ieee8021x_wep.disabled = false;
		document.security_form.RadiusServerIP.disabled = false;
		document.security_form.RadiusServerPort.disabled = false;
		document.security_form.RadiusServerSecret.disabled = false;	
		document.security_form.RadiusServerSessionTimeout.disabled = false;
		//document.security_form.RadiusServerIdleTimeout.disabled = false;
	}
	else if (security_mode == "WAIPSK") 
	{
		document.getElementById("div_wapi_psk").style.display = "";
		document.security_form.wapipsk_prekey.disabled = false;
		document.security_form.wapipsk_keytype.disabled = false;
	} 
	else if (security_mode == "WAICERT") 
	{
		document.getElementById("div_wapi_cert").style.display = "";
		document.security_form.wapicert_asipaddr.disabled = false;
		document.security_form.wapicert_asport.disabled = false;
		document.security_form.wapicert_ascert.disabled = false;
		document.security_form.wapicert_usercert.disabled = false;
	}
}

function submit_apply()
{
	if (formCheck() == true)
	{
		changed = 0;
		document.security_form.submit();
		//opener.location.reload();
	}
}

function LoadFields(MBSSID)
{
	// Security Policy
	var sp_select = document.security_form.security_mode;
	sp_select.options.length = 0;

	sp_select.options[sp_select.length] = new Option(MM_disable, "Disable",	false, AuthMode[MBSSID] == "Disable");
	sp_select.options[sp_select.length] = new Option("WEP", "OPEN",	false, AuthMode[MBSSID] == "OPEN");
	//sp_select.options[sp_select.length] = new Option(MM_wep_open_system, "OPEN",	false, AuthMode[MBSSID] == "OPEN");

	/*
	if (wpsenable == "0") 
	{
		sp_select.options[sp_select.length] = new Option(MM_wep_shared_key,"SHARED", false, AuthMode[MBSSID] == "SHARED");
		sp_select.options[sp_select.length] = new Option(MM_wep_auto, "WEPAUTO",  false, AuthMode[MBSSID] == "WEPAUTO");
	}
	*/
	
	sp_select.options[sp_select.length] = new Option("WPA-PSK", "WPAPSK",	false, AuthMode[MBSSID] == "WPAPSK");
	sp_select.options[sp_select.length] = new Option("WPA2-PSK", "WPA2PSK",	 false, AuthMode[MBSSID] == "WPA2PSK");
	sp_select.options[sp_select.length] = new Option("WPA/WPA2-PSK", "WPAPSKWPA2PSK",	false, AuthMode[MBSSID] == "WPAPSKWPA2PSK");
	
	/*
	if (wpsenable == "0") 
	{
		sp_select.options[sp_select.length] = new Option("WPA", "WPA",	false, AuthMode[MBSSID] == "WPA");
		sp_select.options[sp_select.length] = new Option("WPA2", "WPA2", false, AuthMode[MBSSID] == "WPA2");
		sp_select.options[sp_select.length] = new Option("WPA/WPA2","WPA1WPA2",	 false, AuthMode[MBSSID] == "WPA1WPA2");
		sp_select.options[sp_select.length] = new Option("802.1X",	"IEEE8021X", false, AuthMode[MBSSID] == "IEEE8021X");
	}
	*/
	
	if (wapib == "1")
	{
		sp_select.options[sp_select.length] = new Option("WAPI-PSK", "WAIPSK",	false, AuthMode[MBSSID] == "WAIPSK");
		sp_select.options[sp_select.length] = new Option("WAPI-Certificate", "WAICERT",	false, AuthMode[MBSSID] == "WAICERT");
	}
	// WEP
	document.security_form.wep_key_1.value = Key1Str[MBSSID];
	document.security_form.wep_key_2.value = Key2Str[MBSSID];
	document.security_form.wep_key_3.value = Key3Str[MBSSID];
	document.security_form.wep_key_4.value = Key4Str[MBSSID];

	document.security_form.WEP1Select.selectedIndex = (Key1Type[MBSSID] == "0" ? 1 : 0);
	document.security_form.WEP2Select.selectedIndex = (Key2Type[MBSSID] == "0" ? 1 : 0);
	document.security_form.WEP3Select.selectedIndex = (Key3Type[MBSSID] == "0" ? 1 : 0);
	document.security_form.WEP4Select.selectedIndex = (Key4Type[MBSSID] == "0" ? 1 : 0);

	document.security_form.wep_default_key.selectedIndex = parseInt(DefaultKeyID[MBSSID]) - 1 ;

	// WPA
	if(EncrypType[MBSSID] == "TKIP")
		document.security_form.cipher[0].checked = true;
	else if(EncrypType[MBSSID] == "AES")
		document.security_form.cipher[1].checked = true;
	else if(EncrypType[MBSSID] == "TKIPAES")
		document.security_form.cipher[2].checked = true;

	document.security_form.passphrase.value = WPAPSK[MBSSID];
	document.security_form.keyRenewalInterval.value = RekeyInterval[MBSSID];
	document.security_form.PMKCachePeriod.value = PMKCachePeriod[MBSSID];
	//document.security_form.PreAuthentication.value = PreAuth[MBSSID];
	if (PreAuth[MBSSID] == "0")
		document.security_form.PreAuthentication[0].checked = true;
	else
		document.security_form.PreAuthentication[1].checked = true;

	//802.1x wep
	if (IEEE8021X[MBSSID] == "1")
	{
		if(EncrypType[MBSSID] == "WEP")
			document.security_form.ieee8021x_wep[1].checked = true;
		else
			document.security_form.ieee8021x_wep[0].checked = true;
	}

	document.security_form.RadiusServerIP.value = RADIUS_Server[MBSSID];
	document.security_form.RadiusServerPort.value = RADIUS_Port[MBSSID];
	document.security_form.RadiusServerSecret.value = RADIUS_Key[MBSSID];			
	document.security_form.RadiusServerSessionTimeout.value = session_timeout_interval[MBSSID];

	// wapi
	document.security_form.wapicert_asipaddr.value = WapiAsIpAddr[MBSSID];
	for (var i=0;i<document.security_form.wapicert_ascert.length;i++)
	{
		var temp = document.security_form.wapicert_ascert.options[i].value;
		if (temp == WapiAsCertPath[MBSSID])
			document.security_form.wapicert_ascert.selectedIndex = i;
	}
	for (i=0;i<document.security_form.wapicert_usercert.length;i++)
	{
		var temp = document.security_form.wapicert_usercert.options[i].value;
		if (temp == WapiUserCertPath[MBSSID])
			document.security_form.wapicert_usercert.selectedIndex = i;
	}
	document.security_form.wapipsk_prekey.value = WapiPsk[MBSSID];
	if (WapiPskType[MBSSID] == "1")
		document.security_form.wapipsk_keytype.selectedIndex = 1;
	else
		document.security_form.wapipsk_keytype.selectedIndex = 0;

	securityMode(0);
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

	flag = eval(location.href.split("#")[1]);//get opener flag value
	if (flag == 0)
	{
		var selected = document.security_form.ssidIndex.options.selectedIndex;
	}
	else
	{
		var selected = document.security_form.ssidIndex.options.selectedIndex + 1;
	}
	// backup for user cancel action
	old_MBSSID = selected;

	MBSSIDChange(selected);
}

/*
 * When user select the different SSID, this function would be called.
 */ 
function MBSSIDChange(selected)
{
	// load wep/wpa/802.1x table for MBSSID[selected]
	LoadFields(selected);

	// update Access Policy for MBSSID[selected]
	ShowAP(selected);

	// radio button special case
	WPAAlgorithms = EncrypType[selected];
	IEEE8021XWEP = IEEE8021X[selected];
	PreAuthentication = PreAuth[selected];

	changeSecurityPolicyTableTitle(SSID[selected]);

	// clear all new access policy list field
	for(i=0; i<MBSSID_MAX; i++)
		document.getElementById("newap_text_"+i).value = "";

	return true;
}

function changeSecurityPolicyTableTitle(t)
{
	document.getElementById("sp_title").innerHTML = "\"" + t + "\"";
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
	flag = eval(location.href.split("#")[1]);//get opener flag value
	
	document.security_form.ssidIndex.length = 0;

	if (flag == 0)
	{
		document.security_form.ssidIndex.options[0] = new Option(SSID[0], 0, false, false);
		document.getElementById("submitUrl").value = "/wireless/basic.asp";
		document.security_form.ssidIndex.options.selectedIndex = 0;
		defaultShownMBSSID = 0;
		old_MBSSID = defaultShownMBSSID;
		changeSecurityPolicyTableTitle(SSID[defaultShownMBSSID]);
	}
	else
	{
		for(var i=1; i<SSID.length; i++)
		{
			var j = document.security_form.ssidIndex.options.length;
			document.security_form.ssidIndex.options[j] = new Option(SSID[i], i, false, false);
		}
		
		document.getElementById("submitUrl").value = "/wireless/mulipleap.asp";
		document.security_form.ssidIndex.options.selectedIndex = 0;
		defaultShownMBSSID = 1;
		old_MBSSID = defaultShownMBSSID;
		changeSecurityPolicyTableTitle(SSID[defaultShownMBSSID]);
	}
}

function setChange(c)
{
	changed = c;
}

var WPAAlgorithms;
function onWPAAlgorithmsClick(type)
{
	if(type == 0 && WPAAlgorithms == "TKIP") return;
	if(type == 1 && WPAAlgorithms == "AES") return;
	if(type == 2 && WPAAlgorithms == "TKIPAES") return;
	setChange(1);
}

var IEEE8021XWEP;
function onIEEE8021XWEPClick(type)
{
	if(type == 0 && IEEE8021XWEP == false) return;
	if(type == 1 && IEEE8021XWEP == true) return;
	setChange(1);
}

var PreAuthentication;
function onPreAuthenticationClick(type)
{
	if(type == 0 && PreAuthentication == false) return;
	if(type == 1 && PreAuthentication == true) return;
	setChange(1);
}

function open_wapi_cert_upload(target)
{
	if (target == "user")
		window.open("wapi_cert_user_upload.asp","wapi_cert_user_upload","toolbar=no, location=yes, scrollbars=yes, resizable=no, width=500, height=500");
	else if (target == "as")
		window.open("wapi_cert_as_upload.asp","wapi_cert_as_upload","toolbar=no, location=yes, scrollbars=yes, resizable=no, width=500, height=500");
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
<tr><td class="title"><script>dw(MM_security_settings)</script></td></tr>
<tr><td><script>dw(JS_msg_wlsecurity)</script></td></tr>
<tr><td><hr></td></tr>
</table>

<form method="post" name="security_form" action="/goform/APSecurity">
<input type="hidden" id="submitUrl" name="submit-url" value="/wireless/security.asp">
<table width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr>
    <td class="thead"><script>dw(MM_select_ssid)</script>:</td>
    <td><select name="ssidIndex" onChange="selectMBSSIDChanged()"></select></td>
  </tr>
  <tr style="display:none">
    <td class="title2" colspan="2"> <span id="sp_title">Security Policy</span></td>
  </tr>
  <tr id="div_security_infra_mode"> 
    <td class="thead"><script>dw(MM_security_mode)</script>:</td>
    <td><select name="security_mode" onChange="securityMode(1)"></select></td>
  </tr>
</table>
<table id="div_wep" style="display:none" width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr style="display:none"> 
    <td class="thead">WEP <script>dw(MM_default_key)</script>:</td>
    <td><select name="wep_default_key" onChange="setChange(1)">
	<option value="1"><script>dw(MM_key)</script> 1</option>
	<option value="2"><script>dw(MM_key)</script> 2</option>
	<option value="3"><script>dw(MM_key)</script> 3</option>
	<option value="4"><script>dw(MM_key)</script> 4</option>
      </select></td>
  </tr>
  <tr> 
    <td class="thead"><script>dw(MM_wep_key)</script>:</td>
    <td><input name="wep_key_1" maxlength="26" onKeyUp="setChange(1)"> <select name="WEP1Select" onChange="setChange(1)"> 
		<option value="1"><script>dw(MM_ascii)</script></option>
		<option value="0"><script>dw(MM_hex)</script></option>
		</select></td>
  </tr>
  <tr style="display:none"> 
    <td class="thead"><script>dw(MM_wep_key)</script> 2:</td>
    <td><input name="wep_key_2" maxlength="26" onKeyUp="setChange(1)"> <select name="WEP2Select" onChange="setChange(1)">
		<option value="1"><script>dw(MM_ascii)</script></option>
		<option value="0"><script>dw(MM_hex)</script></option>
		</select></td>
  </tr>
  <tr style="display:none"> 
    <td class="thead"><script>dw(MM_wep_key)</script> 3:</td>
    <td><input name="wep_key_3" maxlength="26" onKeyUp="setChange(1)"> <select name="WEP3Select" onChange="setChange(1)">
		<option value="1"><script>dw(MM_ascii)</script></option>
		<option value="0"><script>dw(MM_hex)</script></option>
		</select></td>
  </tr>
  <tr style="display:none"> 
    <td class="thead"><script>dw(MM_wep_key)</script> 4:</td>
    <td><input name="wep_key_4" maxlength="26" onKeyUp="setChange(1)"> <select name="WEP4Select" onChange="setChange(1)">
		<option value="1"><script>dw(MM_ascii)</script></option>
		<option value="0"><script>dw(MM_hex)</script></option>
		</select></td>
  </tr>
</table>

<table id="div_wpa" style="display:none" width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr id="div_wpa_algorithms" style="display:none">
    <td class="thead"><script>dw(MM_wpa_alg)</script>:</td>
    <td><input name="cipher" value="0" type="radio" onClick="onWPAAlgorithmsClick(0)">TKIP
    <input name="cipher" value="1" type="radio" onClick="onWPAAlgorithmsClick(1)">AES
    <input name="cipher" value="2" type="radio" onClick="onWPAAlgorithmsClick(2)">TKIP+AES</td>
  </tr>
  <tr id="wpa_passphrase" style="display:none">
    <td class="thead"><script>dw(MM_passphrase)</script>:</td>
    <td><input name="passphrase" size="28" maxlength="64" onKeyUp="setChange(1)"></td>
  </tr>
  <tr id="wpa_key_renewal_interval" style="display:none">
    <td class="thead"><script>dw(MM_key_renewal_interval)</script>:</td>
    <td><input name="keyRenewalInterval" size="6" maxlength="7" value="3600" onKeyUp="setChange(1)"> <script>dw(MM_seconds)</script></td>
  </tr>
  <tr id="wpa_PMK_Cache_Period" style="display:none">
    <td class="thead"><script>dw(MM_pmk_cache_period)</script>:</td>
    <td><input name="PMKCachePeriod" size="4" maxlength="4" onKeyUp="setChange(1)"> <script>dw(MM_minutes)</script></td>
  </tr>
  <tr id="wpa_preAuthentication" style="display:none">
    <td class="thead"><script>dw(MM_pre_auth)</script>:</td>
    <td><input name="PreAuthentication" value="0" type="radio" onClick="onPreAuthenticationClick(0)"><script>dw(MM_disable)</script>
    <input name="PreAuthentication" value="1" type="radio" onClick="onPreAuthenticationClick(1)"><script>dw(MM_enable)</script></td>
  </tr>
</table>

<table id="div_8021x_wep" style="display:none" width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr>
    <td class="thead">802.1x WEP:</td>
    <td><input name="ieee8021x_wep" value="0" type="radio" onClick="onIEEE8021XWEPClick(0)"><script>dw(MM_disable)</script>
    <input name="ieee8021x_wep" value="1" type="radio" onClick="onIEEE8021XWEPClick(1)"><script>dw(MM_enable)</script></td>
  </tr>
</table>

<!--<table id="div_radius_server" style="display:none" width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr> 
	<td class="thead"><script>dw(MM_radius_server)</script> <script>dw(MM_ipaddr)</script>:</td>
	<td><input name="RadiusServerIP" maxlength="15" onKeyUp="setChange(1)"> </td>
  </tr>
  <tr> 
	<td class="thead"><script>dw(MM_radius_server)</script> <script>dw(MM_port)</script>:</td>
	<td><input name="RadiusServerPort" size="5" maxlength="5" onKeyUp="setChange(1)"> (1-65535)</td>
  </tr>
  <tr> 
	<td class="thead"><script>dw(MM_radius_server)</script> <script>dw(MM_shared_secret)</script>:</td>
	<td><input name="RadiusServerSecret" maxlength="64" onKeyUp="setChange(1)"> </td>
  </tr>
  <tr> 
	<td class="thead"><script>dw(MM_radius_server)</script> <script>dw(MM_session_time_out)</script>:</td>
	<td><input name="RadiusServerSessionTimeout" size="3" maxlength="4" value="0" onKeyUp="setChange(1)"> </td>
  </tr>
  <tr> 
	<td class="thead"><script>dw(MM_radius_server)</script> <script>dw(MM_idle_time_out)</script>:</td>
	<td><input name="RadiusServerIdleTimeout" size="3" maxlength="4" onKeyUp="setChange(1)" readonly> </td>
  </tr>
</table>-->

<table id="div_radius_server" style="display:none" width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr> 
	<td colspan="5">&nbsp;</td>
  </tr>
  <tr> 
	<td class="title2" colspan="5"><script>dw(MM_radius_server)</script></td>
  </tr>
  <tr> 
	<td class="thead"><script>dw(MM_ipaddr)</script></td>
    <td><script>dw(MM_port)</script></td>
    <td><script>dw(MM_shared_secret)</script></td>
    <td><script>dw(MM_session_time_out)</script></td>
    <td><script>dw(MM_idle_time_out)</script></td>
  </tr>
  <tr>
	<td><input name="RadiusServerIP" maxlength="15" onKeyUp="setChange(1)"> </td>
	<td><input name="RadiusServerPort" size="5" maxlength="5" onKeyUp="setChange(1)"> (1-65535)</td>
	<td><input name="RadiusServerSecret" maxlength="64" onKeyUp="setChange(1)"> </td>
	<td><input name="RadiusServerSessionTimeout" size="3" maxlength="4" value="0" onKeyUp="setChange(1)"> </td>
	<td><input name="RadiusServerIdleTimeout" size="3" maxlength="4" onKeyUp="setChange(1)" readonly> </td>
  </tr>
</table>

<table id="div_wapi_psk" style="display:none" width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr>
    <td class="thead">WAPI-PSK Key:</td>
    <td><input type="text" name="wapipsk_prekey" size="28" maxlength="64" value=""></td>
    <td><select name="wapipsk_keytype">
        <option value="0" selected>HEX</option>
		<option value="1"><script>dw(MM_ascii)</script></option>
      </select></td>
  </tr>
</table>

<table id="div_wapi_cert" style="display:none" width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr> 
    <td class="thead">WAPI AS IP Address:</td>
    <td><input type="text" name="wapicert_asipaddr" maxlength="32" value=""></td>
  </tr>
  <tr> 
    <td class="thead">WAPI AS Port:</td>
    <td><input type="text" name="wapicert_asport" size="5" maxlength="5" value="3810" readOnly> (1-65535)</td>
  </tr>
  <tr>
    <td class="thead">WAPI AS Certificate:</td>
    <td><select name="wapicert_ascert">
        <% getWAPIASCertList(); %>
      </select>
      <input type="button" value="Install" name="wapi_as_cert_upload" onClick="open_wapi_cert_upload('as')"></td>
  </tr>
  <tr>
    <td class="thead">WAPI User Certificate:</td>
    <td><select name="wapicert_usercert">
        <% getWAPIUserCertList(); %>
      </select>
      <input type="button" value="Install" name="wapi_apuser_cert_upload" onClick="open_wapi_cert_upload('user')"></td>
  </tr>
</table>

<span style="display:none">
<script language="javascript">
var aptable;

for(aptable = 0; aptable < MBSSID_MAX; aptable++){
	document.write("<table id=AccessPolicy_"+ aptable +" width=100% border=0 cellpadding=3 cellspacing=1>");
	document.write("<tr><td class=thead>"+MM_auth_mode+":</td>");
	document.write("<td><select name=apselect_"+ aptable + " id=apselect_"+aptable+" onchange=\"setChange(1)\">");
	document.write("<option value=0 >"+MM_disable+"</option><option value=1 >"+MM_allow+"</option><option value=2 >"+MM_deny+"</option></select></td></tr>");
	document.write("<tr><td class=thead>"+MM_macaddr+":</td>");
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
</span>

<br>
<table width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr>
    <td>
      <script>dw('<input type="button" class="button" value="'+BT_apply+'" onClick="submit_apply()"> &nbsp; &nbsp;\
      <input type="button" class="button" value="'+BT_reset+'" onClick="resetForm();" >')</script>
    </td>
  </tr>
</table>
</form>

</td></tr></table>
</blockquote>
</body></html>
 