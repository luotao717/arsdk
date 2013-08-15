<html>
<head>
<title>WR922</title>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312" />
<link href="images/kingnet.css" type="text/css" rel="stylesheet">
<SCRIPT src="helpScript.js"></SCRIPT>
<script language="JavaScript" type="text/javascript">
var MBSSID_MAX 				= 8;
var ACCESSPOLICYLIST_MAX	= 64;
var changed = 0;
var old_MBSSID;
var defaultShownMBSSID = 0;
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

function checkMac(str){
	var len = str.length;
	if(len!=17)
		return false;

	for (var i=0; i<str.length; i++) {
		if((i%3) == 2){
			if(str.charAt(i) == ':')
				continue;
		}else{
			if (    (str.charAt(i) >= '0' && str.charAt(i) <= '9') ||
					(str.charAt(i) >= 'a' && str.charAt(i) <= 'f') ||
					(str.charAt(i) >= 'A' && str.charAt(i) <= 'F') )
			continue;
		}
		return false;
	}
	return true;
}

function checkRange(str, num, min, max)
{
    d = atoi(str,num);
    if(d > max || d < min)
        return false;
    return true;
}

function checkIpAddr(field)
{
    if(field.value == "")
        return false;

    if ( checkAllNum(field.value) == 0)
        return false;

    if( (!checkRange(field.value,1,0,255)) ||
        (!checkRange(field.value,2,0,255)) ||
        (!checkRange(field.value,3,0,255)) ||
        (!checkRange(field.value,4,1,254)) ){
        return false;
    }
   return true;
}

function atoi(str, num)
{
    i=1;
    if(num != 1 ){
        while (i != num && str.length != 0){
            if(str.charAt(0) == '.'){
                i++;
            }
            str = str.substring(1);
        }
        if(i != num )
            return -1;
    }

    for(i=0; i<str.length; i++){
        if(str.charAt(i) == '.'){
            str = str.substring(0, i);
            break;
        }
    }
    if(str.length == 0)
        return -1;
    return parseInt(str, 10);
}

function checkHex(str){
	var len = str.length;

	for (var i=0; i<str.length; i++) {
		if ((str.charAt(i) >= '0' && str.charAt(i) <= '9') ||
			(str.charAt(i) >= 'a' && str.charAt(i) <= 'f') ||
			(str.charAt(i) >= 'A' && str.charAt(i) <= 'F') ){
				continue;
		}else
	        return false;
	}
    return true;
}

function checkInjection(str)
{
	var len = str.length;
	for (var i=0; i<str.length; i++) {
		if ( str.charAt(i) == '\r' || str.charAt(i) == '\n'){
				return false;
		}else
	        continue;
	}
    return true;
}

function checkStrictInjection(str)
{
	var len = str.length;
	for (var i=0; i<str.length; i++) {
		if ( str.charAt(i) == ';' || str.charAt(i) == ',' ||
			 str.charAt(i) == '\r' || str.charAt(i) == '\n'){
				return false;
		}else
	        continue;
	}
    return true;
}

function checkAllNum(str)
{
    for (var i=0; i<str.length; i++){
        if((str.charAt(i) >= '0' && str.charAt(i) <= '9') || (str.charAt(i) == '.' ))
            continue;
        return false;
    }
    return true;
}

function style_display_on()
{
	if (window.ActiveXObject) { // IE
		return "block";
	}
	else if (window.XMLHttpRequest) { // Mozilla, Safari,...
		return "table-row";
	}
}

var http_request = false;
function makeRequest(url, content, handler) {
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
	http_request.onreadystatechange = handler;
	http_request.open('POST', url, true);
	http_request.send(content);
}

function securityHandler() {
	if (http_request.readyState == 4) {
		if (http_request.status == 200) {
			parseAllData(http_request.responseText);
			UpdateMBSSIDList();
			LoadFields(defaultShownMBSSID);

			// load Access Policy for MBSSID[selected]
			//LoadAP();
			ShowAP(defaultShownMBSSID);

//			if(<% getWPSModeASP(); %> && <% isWPSConfiguredASP(); %>){
//				alert("Info: The security settings has been assigned under active WPS functions.\nYou still could change security setting manually but the existed WPS settings would be overwritted.");
//			}

		}else {
			alert('There was a problem with the request.');
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

	for (var i=0; i<all_str.length-2; i++) {
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

		/* !!!! IMPORTANT !!!!*/
		if(IEEE8021X[i] == "1")
			AuthMode[i] = "IEEE8021X";

		if(AuthMode[i] == "OPEN" && EncrypType[i] == "NONE" && IEEE8021X[i] == "0")
			AuthMode[i] = "Disable";
	}
}

function checkData()
{
	var securitymode;
//	var ssid = document.security_form.Ssid.value;
	
	securitymode = document.security_form.security_mode.value;
	if (securitymode == "OPEN" || securitymode == "SHARED" ||securitymode == "WEPAUTO")
	{
		if(! check_Wep(securitymode) )
			return false;
	}else if (securitymode == "WPAPSK" || securitymode == "WPA2PSK" || securitymode == "WPAPSKWPA2PSK" /* || security_mode == 5 */){
		var keyvalue = document.security_form.passphrase.value;

		if (keyvalue.length == 0){
			alert('������ wpapsk ��Կ!');
			return false;
		}

		if (keyvalue.length < 8){
			alert('����������8�� wpapsk key �ַ�!');
			return false;
		}
		
		if(checkInjection(document.security_form.passphrase.value) == false){
			alert('������Ч���ַ�.');
			return false;
		}

		if(document.security_form.cipher[0].checked != true && 
		   document.security_form.cipher[1].checked != true &&
   		   document.security_form.cipher[2].checked != true){
   		   alert('��ѡ��һ�� WPA �㷨.');
   		   return false;
		}

		if(checkAllNum(document.security_form.keyRenewalInterval.value) == false){
			alert('������һ������Ч�ļ��������Կ');
			return false;
		}
		if(document.security_form.keyRenewalInterval.value < 60){
			alert('����: ̫�̵ĸ��¼��.');
			// return false;
		}
		if(check_wpa() == false)
			return false;
	}
	//802.1x
	else if (securitymode == "IEEE8021X") // 802.1x
	{
		if( document.security_form.ieee8021x_wep[0].checked == false &&
			document.security_form.ieee8021x_wep[1].checked == false){
			alert('��ѡ��һ�� 802.1x WEP ��ʽ.');
			return false;
		}
		if(check_radius() == false)
			return false;
	}else if (securitymode == "WPA" || securitymode == "WPA1WPA2") //     WPA or WPA1WP2 mixed mode
	{
		if(check_wpa() == false)
			return false;
		if(check_radius() == false)
			return false;
	}else if (securitymode == "WPA2") //         WPA2
	{
		if(check_wpa() == false)
			return false;
		if( document.security_form.PreAuthentication[0].checked == false &&
			document.security_form.PreAuthentication[1].checked == false){
			alert('��ѡ��һ�� Pre-Authentication ��ʽ.');
			return false;
		}

		if(!document.security_form.PMKCachePeriod.value.length){
			alert('������ PMK ��������.');
			return false;
		}
		if(check_radius() == false)
			return false;
	}

	// check Access Policy
	for(i=0; i<MBSSID_MAX; i++){


		if( document.getElementById("newap_text_" + i).value != ""){
			if(!checkMac(document.getElementById("newap_text_" + i).value)){
				alert("�����б����MAC��ַ��Ч.\n");
				return false;
			}
		}
	}

	return true;
}

function check_wpa()
{
		if(document.security_form.cipher[0].checked != true && 
		   document.security_form.cipher[1].checked != true &&
   		   document.security_form.cipher[2].checked != true){
   		   alert('��ѡ��һ�� WPA �㷨.');
   		   return false;
		}

		if(checkAllNum(document.security_form.keyRenewalInterval.value) == false){
			alert('��������Ч����Կ��������');
			return false;
		}
		if(document.security_form.keyRenewalInterval.value < 60){
			alert('����: ��Կ�������ڴ���.');
			// return false;
		}
		return true;
}

function check_radius()
{
	if(document.security_form.ieee8021x_wep[1].checked == true)
	{
		if( ! check_Wep("OPEN") )
			return false;
	}
	if(!document.security_form.RadiusServerIP.value.length){
		alert('������radius ������ IP ��ַ.');
		return false;		
	}
	if(!document.security_form.RadiusServerPort.value.length){
		alert('������ radius �������˿ں�.');
		return false;		
	}
	if(!document.security_form.RadiusServerSecret.value.length){
		alert('������ radius ������������Կ.');
		return false;		
	}

	if(checkIpAddr(document.security_form.RadiusServerIP) == false){
		alert('������һ����Ч�� radius ������IP��ַ.');
		return false;		
	}
	if( (checkRange(document.security_form.RadiusServerPort.value, 1, 1, 65535)==false) ||
		(checkAllNum(document.security_form.RadiusServerPort.value)==false)){
		alert('������һ����Ч��radius�������˿�.');
		return false;		
	}
	if(checkStrictInjection(document.security_form.RadiusServerSecret.value)==false){
		alert('������Կ���溬�зǷ��ַ�.');
		return false;		
	}

	if(document.security_form.RadiusServerSessionTimeout.value.length){
		if(checkAllNum(document.security_form.RadiusServerSessionTimeout.value)==false){
			alert('������һ����Ч�ĻỰ����.');
			return false;	
		}	
	}

	return true;
}

function securityMode(c_f)
{
	var security_mode;


	changed = c_f;

	hideWep();
	//wpa
	document.getElementById("div_security_shared_mode").style.visibility = "hidden";
	document.getElementById("div_security_shared_mode").style.display = "none";
	document.getElementById("div_wpa").style.visibility = "hidden";
	document.getElementById("div_wpa").style.display = "none";
	document.getElementById("div_wpa_algorithms").style.visibility = "hidden";
	document.getElementById("div_wpa_algorithms").style.display = "none";
	document.getElementById("wpa_passphrase").style.visibility = "hidden";
	document.getElementById("wpa_passphrase").style.display = "none";
	document.getElementById("wpa_key_renewal_interval").style.visibility = "hidden";
	document.getElementById("wpa_key_renewal_interval").style.display = "none";
	document.getElementById("wpa_PMK_Cache_Period").style.visibility = "hidden";
	document.getElementById("wpa_PMK_Cache_Period").style.display = "none";
	document.getElementById("wpa_preAuthentication").style.visibility = "hidden";
	document.getElementById("wpa_preAuthentication").style.display = "none";
	document.security_form.cipher[0].disabled = true;
	document.security_form.cipher[1].disabled = true;
	document.security_form.cipher[2].disabled = true;
	document.security_form.passphrase.disabled = true;
	document.security_form.keyRenewalInterval.disabled = true;
	document.security_form.PMKCachePeriod.disabled = true;
	document.security_form.PreAuthentication.disabled = true;

	// 802.1x
	document.getElementById("div_radius_server").style.visibility = "hidden";
	document.getElementById("div_radius_server").style.display = "none";
	document.getElementById("div_8021x_wep").style.visibility = "hidden";
	document.getElementById("div_8021x_wep").style.display = "none";
//	document.getElementById("div_wep_8021x").style.visibility = "hidden";
//	document.getElementById("div_wep_8021x").style.display = "none";
	document.security_form.ieee8021x_wep.disable = true;
	document.security_form.RadiusServerIP.disable = true;
	document.security_form.RadiusServerPort.disable = true;
	document.security_form.RadiusServerSecret.disable = true;	
	document.security_form.RadiusServerSessionTimeout.disable = true;
	document.security_form.RadiusServerIdleTimeout.disable = true;	

	security_mode = document.security_form.security_mode.value;

	if (security_mode == "OPEN" || security_mode == "SHARED" ||security_mode == "WEPAUTO"){
		showWep(security_mode);
	}else if (security_mode == "WPAPSK" || security_mode == "WPA2PSK" || security_mode == "WPAPSKWPA2PSK"){
		<!-- WPA ��ʾ-->
		document.getElementById("div_wpa").style.visibility = "visible";
		
		if (window.ActiveXObject) { // IE
			document.getElementById("div_wpa").style.display = "block";
		}
		else if (window.XMLHttpRequest) { // Mozilla, Safari,...
			document.getElementById("div_wpa").style.display = "table";
		}

		document.getElementById("div_wpa_algorithms").style.visibility = "visible";
		document.getElementById("div_wpa_algorithms").style.display = style_display_on();
		document.security_form.cipher[0].disabled = false;
		document.security_form.cipher[1].disabled = false;

		// deal with TKIP-AES mixed mode
		if(security_mode == "WPAPSK" && document.security_form.cipher[2].checked)
			document.security_form.cipher[2].checked = false;
		// deal with TKIP-AES mixed mode
		if(security_mode == "WPA2PSK" || security_mode == "WPAPSKWPA2PSK")
			document.security_form.cipher[2].disabled = false;

		document.getElementById("wpa_passphrase").style.visibility = "visible";
		document.getElementById("wpa_passphrase").style.display = style_display_on();
		document.security_form.passphrase.disabled = false;

		document.getElementById("wpa_key_renewal_interval").style.visibility = "visible";
		document.getElementById("wpa_key_renewal_interval").style.display = style_display_on();
		
		document.security_form.keyRenewalInterval.disabled = false;
	}else if (security_mode == "WPA" || security_mode == "WPA2" || security_mode == "WPA1WPA2") //wpa enterprise
	{
		document.getElementById("div_wpa").style.visibility = "visible";
		if (window.ActiveXObject) { // IE
			document.getElementById("div_wpa").style.display = "block";
		}else if (window.XMLHttpRequest) { // Mozilla, Safari,...
			document.getElementById("div_wpa").style.display = "table";
		}

		document.getElementById("div_wpa_algorithms").style.visibility = "visible";
		document.getElementById("div_wpa_algorithms").style.display = style_display_on();
		document.security_form.cipher[0].disabled = false;
		document.security_form.cipher[1].disabled = false;
		document.getElementById("wpa_key_renewal_interval").style.visibility = "visible";
		document.getElementById("wpa_key_renewal_interval").style.display = style_display_on();
		document.security_form.keyRenewalInterval.disabled = false;
	
		<!-- 802.1x -->
		document.getElementById("div_radius_server").style.visibility = "visible";
		document.getElementById("div_radius_server").style.display = style_display_on();
		document.security_form.RadiusServerIP.disable = false;
		document.security_form.RadiusServerPort.disable = false;
		document.security_form.RadiusServerSecret.disable = false;	
		document.security_form.RadiusServerSessionTimeout.disable = false;
		document.security_form.RadiusServerIdleTimeout.disable = false;	

		// deal with TKIP-AES mixed mode
		if(security_mode == "WPA" && document.security_form.cipher[2].checked)
			document.security_form.cipher[2].checked = false;
		// deal with TKIP-AES mixed mode
		if(security_mode == "WPA2"){
			document.security_form.cipher[2].disabled = false;
			document.getElementById("wpa_preAuthentication").style.visibility = "visible";
			document.getElementById("wpa_preAuthentication").style.display = style_display_on();
			document.security_form.PreAuthentication.disabled = false;
			document.getElementById("wpa_PMK_Cache_Period").style.visibility = "visible";
			document.getElementById("wpa_PMK_Cache_Period").style.display = style_display_on();
			document.security_form.PMKCachePeriod.disabled = false;
		}

		// deal with WPA1WPA2 mixed mode
		if(security_mode == "WPA1WPA2"){
			document.security_form.cipher[2].disabled = false;
		}

	}else if (security_mode == "IEEE8021X"){ // 802.1X-WEP
		document.getElementById("div_8021x_wep").style.visibility = "visible";
		document.getElementById("div_8021x_wep").style.display = style_display_on();
		document.getElementById("div_radius_server").style.visibility = "visible";
		document.getElementById("div_radius_server").style.display = style_display_on();
		
		if(document.security_form.ieee8021x_wep[1].checked == true)
		{
			document.getElementById("div_wep").style.visibility = "visible";
			document.getElementById("div_wep").style.display = style_display_on();
		}
		document.getElementById("secureWEP").style.visibility = "hidden";
		document.getElementById("secureWEP").style.display = "none";
//		document.getElementById("div_wep_8021x").style.visibility = "visible";
//		document.getElementById("div_wep_8021x").style.display = style_display_on();
		
		document.security_form.ieee8021x_wep.disable = false;
		document.security_form.RadiusServerIP.disable = false;
		document.security_form.RadiusServerPort.disable = false;
		document.security_form.RadiusServerSecret.disable = false;	
		document.security_form.RadiusServerSessionTimeout.disable = false;
		document.security_form.RadiusServerIdleTimeout.disable = false;
	}
}


function hideWep()
{
	document.getElementById("div_wep").style.visibility = "hidden";
	document.getElementById("div_wep").style.display = "none";
}
function showWep(mode)
{
	<!-- WEP -->
	document.getElementById("secureWEP").style.visibility = "visible";
	document.getElementById("secureWEP").style.display = style_display_on();
	
	document.getElementById("div_wep").style.visibility = "visible";
	
	if (window.ActiveXObject) { // IE 
		document.getElementById("div_wep").style.display = "block";
	}
	else if (window.XMLHttpRequest) { // Mozilla, Safari...
		document.getElementById("div_wep").style.display = "table";
	}

	if(mode == "SHARED"){
		document.getElementById("div_security_shared_mode").style.visibility = "visible";
		document.getElementById("div_security_shared_mode").style.display = style_display_on();
	}
	//document.security_form.wep_auth_type.disabled = false;
}


function check_Wep(securitymode)
{
	var defaultid = document.security_form.wep_default_key.value;
	var key_input;

	if ( defaultid == 1 )
		var keyvalue = document.security_form.wep_key_1.value;
	else if (defaultid == 2)
		var keyvalue = document.security_form.wep_key_2.value;
	else if (defaultid == 3)
		var keyvalue = document.security_form.wep_key_3.value;
	else if (defaultid == 4)
		var keyvalue = document.security_form.wep_key_4.value;

	if (keyvalue.length == 0 &&  (securitymode == "SHARED" || securitymode == "OPEN")){ // shared wep  || md5
		alert('������ wep ��Կ'+defaultid+' !');
		return false;
	}

	var keylength = document.security_form.wep_key_1.value.length;
	if (keylength != 0){
		if (document.security_form.WEP1Select.options.selectedIndex == 0){
			if(keylength != 5 && keylength != 13) {
				alert('������ 5 ���� 13 �� wep ��Կ1 �ַ� !');
				return false;
			}
			if(checkInjection(document.security_form.wep_key_1.value)== false){
				alert('Wep��Կ1���зǷ��ַ�.');
				return false;
			}
		}
		if (document.security_form.WEP1Select.options.selectedIndex == 1){
			if(keylength != 10 && keylength != 26) {
				alert('������ 10 ���� 26 �� wep ��Կ1 16������ ! !');
				return false;
			}
			if(checkHex(document.security_form.wep_key_1.value) == false){
				alert('��Ч�� Wep��Կ1 ��ʽ!');
				return false;
			}
		}
	}

	keylength = document.security_form.wep_key_2.value.length;
	if (keylength != 0){
		if (document.security_form.WEP2Select.options.selectedIndex == 0){
			if(keylength != 5 && keylength != 13) {
				alert('������ 5 ���� 13 �� wep ��Կ2 �ַ� !!');
				return false;
			}
			if(checkInjection(document.security_form.wep_key_2.value)== false){
				alert('Wep��Կ2���зǷ��ַ�.');
				return false;
			}			
		}
		if (document.security_form.WEP2Select.options.selectedIndex == 1){
			if(keylength != 10 && keylength != 26) {
				alert('������ 10 ���� 26 �� wep ��Կ2 16������ ! !');
				return false;
			}
			if(checkHex(document.security_form.wep_key_2.value) == false){
				alert('��Ч�� Wep��Կ2 ��ʽ!');
				return false;
			}
		}
	}

	keylength = document.security_form.wep_key_3.value.length;
	if (keylength != 0){
		if (document.security_form.WEP3Select.options.selectedIndex == 0){
			if(keylength != 5 && keylength != 13) {
				alert('������ 5 ���� 13 �� wep ��Կ3 �ַ� !!');
				return false;
			}
			if(checkInjection(document.security_form.wep_key_3.value)== false){
				alert('Wep��Կ3 ���зǷ��ַ�.');
				return false;
			}
		}
		if (document.security_form.WEP3Select.options.selectedIndex == 1){
			if(keylength != 10 && keylength != 26) {
				alert('������ 10 ���� 26 �� wep ��Կ3 16������ !');
				return false;
			}
			if(checkHex(document.security_form.wep_key_3.value) == false){
				alert('��Ч�� Wep��Կ3 ��ʽ!');
				return false;
			}			
		}
	}

	keylength = document.security_form.wep_key_4.value.length;
	if (keylength != 0){
		if (document.security_form.WEP4Select.options.selectedIndex == 0){
			if(keylength != 5 && keylength != 13) {
				alert('������ 5 ���� 13 �� wep ��Կ4 �ַ� !');
				return false;
			}
			if(checkInjection(document.security_form.wep_key_4.value)== false){
				alert('Wep��Կ4 ���зǷ��ַ�.');
				return false;
			}			
		}
		if (document.security_form.WEP4Select.options.selectedIndex == 1){
			if(keylength != 10 && keylength != 26) {
				alert('������ 10 ���� 26 �� wep ��Կ4 16������ !');
				return false;
			}

			if(checkHex(document.security_form.wep_key_4.value) == false){
				alert('��Ч�� Wep��Կ3 ��ʽ!');
				return false;
			}			
		}
	}
	return true;
}
	
function submit_apply()
{

	if (checkData() == true){
		changed = 0;

		document.security_form.submit();
//		opener.location.reload();
	}
}

function LoadFields(MBSSID)
{
	var result;
	// Security Policy
	sp_select = document.getElementById("security_mode");

	sp_select.options.length = 0;
	//alert(AuthMode[0]);
    sp_select.options[sp_select.length] = new Option("Disable",	"Disable",	false, AuthMode[MBSSID] == "Disable");
    sp_select.options[sp_select.length] = new Option("OPEN",	"OPEN",		false, AuthMode[MBSSID] == "OPEN");
    sp_select.options[sp_select.length] = new Option("SHARED",	"SHARED", 	false, AuthMode[MBSSID] == "SHARED");
    sp_select.options[sp_select.length] = new Option("WEPAUTO", "WEPAUTO",	false, AuthMode[MBSSID] == "WEPAUTO");
//    sp_select.options[sp_select.length] = new Option("WPA",		"WPA",		false, AuthMode[MBSSID] == "WPA");
    sp_select.options[sp_select.length] = new Option("WPA", "WPAPSK",	false, AuthMode[MBSSID] == "WPAPSK");
//    sp_select.options[sp_select.length] = new Option("WPA2",	"WPA2",		false, AuthMode[MBSSID] == "WPA2");
    sp_select.options[sp_select.length] = new Option("WPA2","WPA2PSK",	false, AuthMode[MBSSID] == "WPA2PSK");
    sp_select.options[sp_select.length] = new Option("WPAPSKWPA2PSK","WPAPSKWPA2PSK",	false, AuthMode[MBSSID] == "WPAPSKWPA2PSK");
 //   sp_select.options[sp_select.length] = new Option("WPA1WPA2","WPA1WPA2",	false, AuthMode[MBSSID] == "WPA1WPA2");

	/* 
	 * until now we only support 8021X WEP for MBSSID[0]
	 */
	if(MBSSID == 0)
//		sp_select.options[sp_select.length] = new Option("802.1X",	"IEEE8021X",false, AuthMode[MBSSID] == "IEEE8021X");

	// WEP
	document.getElementById("WEP1").value = Key1Str[MBSSID];
	document.getElementById("WEP2").value = Key2Str[MBSSID];
	document.getElementById("WEP3").value = Key3Str[MBSSID];
	document.getElementById("WEP4").value = Key4Str[MBSSID];

	document.getElementById("WEP1Select").selectedIndex = (Key1Type[MBSSID] == "0" ? 1 : 0);
	document.getElementById("WEP2Select").selectedIndex = (Key2Type[MBSSID] == "0" ? 1 : 0);
	document.getElementById("WEP3Select").selectedIndex = (Key3Type[MBSSID] == "0" ? 1 : 0);
	document.getElementById("WEP4Select").selectedIndex = (Key4Type[MBSSID] == "0" ? 1 : 0);

	document.getElementById("wep_default_key").selectedIndex = parseInt(DefaultKeyID[MBSSID]) - 1 ;

	// SHARED && NONE
	if(AuthMode[MBSSID] == "SHARED" && EncrypType[MBSSID] == "NONE")
		document.getElementById("security_shared_mode").selectedIndex = 1;
	else
		document.getElementById("security_shared_mode").selectedIndex = 0;

	// WPA
	if(EncrypType[MBSSID] == "TKIP")
		document.security_form.cipher[0].checked = true;
	else if(EncrypType[MBSSID] == "AES")
		document.security_form.cipher[1].checked = true;
	else if(EncrypType[MBSSID] == "TKIPAES")
		document.security_form.cipher[2].checked = true;

	document.getElementById("passphrase").value = WPAPSK[MBSSID];
	document.getElementById("keyRenewalInterval").value = RekeyInterval[MBSSID];
	document.getElementById("PMKCachePeriod").value = PMKCachePeriod[MBSSID];
	//document.getElementById("PreAuthentication").value = PreAuth[MBSSID];
	if(PreAuth[MBSSID] == "0")
		document.security_form.PreAuthentication[0].checked = true;
	else
		document.security_form.PreAuthentication[1].checked = true;

	//802.1x wep
	if(IEEE8021X[MBSSID] == "1"){
		if(EncrypType[MBSSID] == "WEP")
		{	
			document.security_form.ieee8021x_wep[1].checked = true;
			document.getElementById("div_wep").style.visibility = "visible";
			document.getElementById("div_wep").style.display = style_display_on();
		}
		else
			document.security_form.ieee8021x_wep[0].checked = true;
	}
	
	document.getElementById("RadiusServerIP").value = RADIUS_Server[MBSSID];
	document.getElementById("RadiusServerPort").value = RADIUS_Port[MBSSID];
	document.getElementById("RadiusServerSecret").value = RADIUS_Key[MBSSID];			
	document.getElementById("RadiusServerSessionTimeout").value = session_timeout_interval[MBSSID];
	
	securityMode(0);
	
	if(IEEE8021X[MBSSID] == "1"){
		if(EncrypType[MBSSID] == "WEP")
		{
			document.getElementById("div_wep").style.visibility = "visible";
			document.getElementById("div_wep").style.display = style_display_on();
		}
		else
			document.security_form.ieee8021x_wep[0].checked = true;
	}
}


function ShowAP(MBSSID)
{
	var i;
	for(i=0; i<MBSSID_MAX; i++){
		document.getElementById("apselect_"+i).selectedIndex	= AccessPolicy[i];
		document.getElementById("AccessPolicy_"+i).style.visibility = "hidden";
		document.getElementById("AccessPolicy_"+i).style.display = "none";
	}

	document.getElementById("AccessPolicy_"+MBSSID).style.visibility = "visible";
	if (window.ActiveXObject) {			// IE
		document.getElementById("AccessPolicy_"+MBSSID).style.display = "block";
	}else if (window.XMLHttpRequest) {	// Mozilla, Safari,...
		document.getElementById("AccessPolicy_"+MBSSID).style.display = "table";
	}
}

/*function LoadAP()
{
	for(var i=0; i<SSID.length; i++){
		var j=0;
		var aplist = new Array;

		if(AccessControlList[i].length != 0){
			aplist = AccessControlList[i].split(";");
			for(j=0; j<aplist.length; j++){
				document.getElementById("newap_"+i+"_"+j).value = aplist[j];
			}

			// hide the lastest <td>
			if(j%2){
				document.getElementById("newap_td_"+i+"_"+j).style.visibility = "hidden";
				document.getElementById("newap_td_"+i+"_"+j).style.display = "none";
				j++;
			}
		}

		// hide <tr> left
		for(; j<ACCESSPOLICYLIST_MAX; j+=2){
			document.getElementById("id_"+i+"_"+j).style.visibility = "hidden";
			document.getElementById("id_"+i+"_"+j).style.display = "none";
		}
	}
}
*/
function selectMBSSIDChanged()
{
	// check if any security settings changed
	if(changed){
		ret = confirm("��ȷ�Ϻ��Ըı���?");
		if(!ret){
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
	var title = document.getElementById("sp_title");
	title.innerHTML = "\"" + t + "\"";
}

function delap(mbssid, num)
{
	makeRequest("/goform/APDeleteAccessPolicyList", mbssid+ "," +num, deleteAccessPolicyListHandler);
}
function initAll()
{

	makeRequest("/goform/wirelessGetSecurity", "n/a", securityHandler);
}

function UpdateMBSSIDList()
{
	document.security_form.ssidIndex.length = 0;

	for(var i=0; i<SSID.length; i++){
		var j = document.security_form.ssidIndex.options.length;
		document.security_form.ssidIndex.options[j] = new Option(SSID[i], i, false, false);
	}
	
	document.security_form.ssidIndex.options.selectedIndex = defaultShownMBSSID;
	old_MBSSID = defaultShownMBSSID;
	changeSecurityPolicyTableTitle(SSID[defaultShownMBSSID]);
}

function setChange(c){
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
	if(type == 0)
	{
		document.getElementById("div_wep").style.visibility = "hidden";
		document.getElementById("div_wep").style.display = "none";
	}
	else{
		document.getElementById("div_wep").style.visibility = "visible";
		document.getElementById("div_wep").style.display = style_display_on();
	}
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

</script>
</head>
<body onLoad="initAll()">
<form method="post" name="security_form"  action="/goform/APSecurity">
<table width="90%" border="0" cellpadding="0" cellspacing="0" align="center">
	<tr>
    	<td style="padding-top:30px;">
	   		<table width="100%" border="0" cellpadding="0" cellspacing="0">
          		<tr>
            		<td class="nav">��ǰ·��:KN-WR922 &gt;&gt;�������� &gt;&gt;��ȫ���� </td>
          		</tr>
		  		<tr>
            		<td>&nbsp;</td>
          		</tr>
          		<tr>
          <td class="space" style="padding-left:10px; padding-right:10px;">&nbsp;&nbsp;&nbsp;��ҳ�������������м���,��ͬ��SSID����ѡ��ͬ�ļ��ܷ���,������Ҫ����ѡ��ͬ��ȫ����ļ��ܷ�ʽ���������
            <input type="button" class="button5" value="����" onclick=popHelp('help.htm#wirelessSec')></td>
      		 </tr>
		  <tr>
        	<td>&nbsp;</td>
      	  </tr>
          		<tr>
            		<td>
			  			<table class="space" width="100%" border="0" cellspacing="0" cellpadding="0">
              				<tr>
            					<td class="titlebg" colspan="2" id="secureSelectSSID">SSIDѡ��</td>
          					</tr>
                			<tr>
							  	<td width="25%" class="contentpadding" id="secureSSIDChoice">SSID ѡ��:</td>
							  	<td width="75%"><select name="ssidIndex" size="1" onChange="selectMBSSIDChanged()"></select></td>
                			</tr>
              			</table>
					</td>
          		</tr>
          		<tr>
            		<td>
			  			<table class="space" width="100%" border="0" cellspacing="0" cellpadding="0">
               				<tr>
            					<td class="titlebg" colspan="2"><span id="sp_title">Security Policy</span></td>
          					</tr>
							<tr id="div_security_infra_mode" name="div_security_infra_mode"> 
							  <td width="25%" class="contentpadding" id="secureSecureMode">��ȫģʽ:</td>
							  <td width="75%"><select name="security_mode" id="security_mode" size="1" onChange="securityMode(1)"></select></td>
							</tr>
			  				<tr id="div_security_shared_mode" name="div_security_shared_mode" style="visibility: hidden; display:none"> 
							  <td width="25%" class="contentpadding" id="secureEncrypType">��������:</td>
							  <td width="75%"><select name="security_shared_mode" id="security_shared_mode" size="1" onChange="securityMode(1)">
							<option value=WEP>WEP</option>
							<option value=None id="secureEncrypTypeNone">��</option></select></td>
							</tr>
              			</table>
					</td>
          		</tr>
				<tr id="div_8021x_wep" name="div_8021x_wep" style="visibility: hidden; display:none">
            		<td>
			  			<table class="space" width="100%" border="0" cellspacing="0" cellpadding="0">
              				<tr>
            					<td class="titlebg" colspan="2" id="secure8021XWEP">802.1x WEP</td>
          					</tr>
							<tr>
							  <td width="25%" class="contentpadding" id="secure1XWEP">WEP:</td>
							  <td width="75%"><input name="ieee8021x_wep" id="ieee8021x_wep" value="0" type="radio" onClick="onIEEE8021XWEPClick(0)">����&nbsp;<input name="ieee8021x_wep" id="ieee8021x_wep" value="1" type="radio" onClick="onIEEE8021XWEPClick(1)">����</td>
							</tr>
              			</table>
					</td>
          		</tr>
         		<tr id="div_wep" name="div_wep" style="visibility: hidden; display:none">
            		<td>
			  			<table class="space" width="100%" border="0" cellspacing="0" cellpadding="0">
              				<tr>
            					<td class="titlebg" colspan="3" id="secureWEP">WEP</td>
          					</tr>
                			<tr>
                  				<td width="25%" class="contentpadding" id="secureWEPDefaultKey">Ĭ����Կ:</td>
                  				<td colspan="2"><select name="wep_default_key" id="wep_default_key" size="1" onChange="setChange(1)">
									<option value="1" id="secureWEPDefaultKey1">��Կ1</option>
									<option value="2" id="secureWEPDefaultKey2">��Կ2</option>
									<option value="3" id="secureWEPDefaultKey3">��Կ3</option>
					 		  <option value="4" id="secureWEPDefaultKey4">��Կ4</option></select></td>
							</tr>
							<tr>
							  <td width="25%" id="secureWEPKey" class="contentpadding" >WEP ��Կ:</td>
							  <td id="secureWEPKey1">WEP ��Կ1:</td>
							  <td><input type="password" name="wep_key_1" id="WEP1" maxlength="26" value="" onKeyUp="setChange(1)">&nbsp;&nbsp;<select id="WEP1Select" name="WEP1Select" onChange="setChange(1)"> 
					<option value="1">ASCII</option>
					<option value="0">Hex</option></select></td>
							</tr>
							<tr>
							  <td>&nbsp;</td>
							  <td width="17%" id="secureWEPKey2">WEP ��Կ2:</td>
							  <td width="58%"><input type="password" name="wep_key_2" id="WEP2" maxlength="26" value="" onKeyUp="setChange(1)">&nbsp;&nbsp;<select id="WEP2Select" name="WEP2Select" onChange="setChange(1)">
					<option value="1">ASCII</option>
					<option value="0">Hex</option></select></td>
							</tr>
							<tr>
							  <td>&nbsp;</td>
							  <td width="17%" id="secureWEPKey3">WEP ��Կ3:</td>
							  <td width="58%"><input type="password" name="wep_key_3" id="WEP3" maxlength="26" value="" onKeyUp="setChange(1)">&nbsp;&nbsp;<select id="WEP3Select" name="WEP3Select" onChange="setChange(1)">
					<option value="1">ASCII</option>
					<option value="0">Hex</option></select></td>
							</tr>
							<tr>
							  <td>&nbsp;</td>
							  <td width="17%" id="secureWEPKey4">WEP ��Կ4:</td>
							  <td width="58%"><input type="password" name="wep_key_4" id="WEP4" maxlength="26" value="" onKeyUp="setChange(1)">&nbsp;&nbsp;<select id="WEP4Select" name="WEP4Select" onChange="setChange(1)">
					<option value="1">ASCII</option>
					<option value="0">Hex</option></select></td>
							</tr>
              			</table>
					</td>
          		</tr>
		  		<tr id="div_wpa" name="div_wpa" style="visibility: hidden;">
            		<td>
			  			<table class="space" width="100%" border="0" cellspacing="0" cellpadding="0">
              				<tr>
            					<td class="titlebg" colspan="2" id="secreWPA">WPA</td>
          					</tr>
							<tr id="div_wpa_algorithms" name="div_wpa_algorithms" style="visibility:hidden; display:none"> 
							  <td width="25%" class="contentpadding" id="secureWPAAlgorithm">WPA �㷨:</td>
							  <td width="75%"><input name="cipher" id="cipher" value="0" type="radio" onClick="onWPAAlgorithmsClick(0)">TKIP&nbsp;<input name="cipher" id="cipher" value="1" type="radio" onClick="onWPAAlgorithmsClick(1)">AES &nbsp;<input name="cipher" id="cipher" value="2" type="radio" onClick="onWPAAlgorithmsClick(2)">TKIPAES</td>
							</tr>
			  				<tr id="wpa_passphrase" name="wpa_passphrase" style="visibility: hidden; display:none">
							  <td width="25%" class="contentpadding" id="secureWPAPassPhrase">ͨ�п���:</td>
							  <td width="75%"><input type="password" name="passphrase" id="passphrase" size="28" maxlength="64" value="" onKeyUp="setChange(1)"></td>
							</tr>
			  				<tr id="wpa_key_renewal_interval" name="wpa_key_renewal_interval" style="visibility: hidden; display:none">
							  <td width="25%" class="contentpadding" id="secureWPAKeyRenewInterval">��Կ��֤����:</td>
							  <td width="75%"><input name="keyRenewalInterval" id="keyRenewalInterval" size="4" maxlength="4" value="" onKeyUp="setChange(1)"> ��</td>
							</tr>
			  				<tr id="wpa_PMK_Cache_Period" name="wpa_PMK_Cache_Period" style="visibility: hidden; display:none">
							  <td width="25%" class="contentpadding" id="secureWPAPMKCachePeriod">PMK ��������:</td>
							  <td width="75%"><input name="PMKCachePeriod" id="PMKCachePeriod" size="4" maxlength="4" value="" onKeyUp="setChange(1)"> ��</td>
							</tr>
			  				<tr id="wpa_preAuthentication" name="wpa_preAuthentication" style="visibility: hidden; display:none">
							  <td width="25%" class="contentpadding" id="secureWPAPreAuth">Ԥ��֤:</td>
							  <td width="75%"><input name="PreAuthentication" id="PreAuthentication" value="0" type="radio" onClick="onPreAuthenticationClick(0)">����&nbsp;<input name="PreAuthentication" id="PreAuthentication" value="1" type="radio" onClick="onPreAuthenticationClick(1)">����</td>
							</tr>
              			</table>
					</td>
          		</tr>
		  		
				<!--
				<tr id="div_wep_8021x" name="div_wep_8021x" style="visibility: hidden; display:none">
            		<td>
			  			<table class="space" width="100%" border="0" cellspacing="0" cellpadding="0">
                			<tr>
                  				<td width="25%" class="contentpadding" id="secureWEPDefaultKey">Ĭ����Կ:</td>
                  				<td colspan="2"><select name="wep_default_key" id="wep_default_key" size="1" onChange="setChange(1)">
									<option value="1" id="secureWEPDefaultKey1">��Կ1</option>
									<option value="2" id="secureWEPDefaultKey2">��Կ2</option>
									<option value="3" id="secureWEPDefaultKey3">��Կ3</option>
					 		  <option value="4" id="secureWEPDefaultKey4">��Կ4</option></select></td>
							</tr>
							<tr>
							  <td width="25%" id="secureWEPKey" class="contentpadding" >WEP ��Կ:</td>
							  <td id="secureWEPKey1">WEP ��Կ1:</td>
							  <td><input name="wep_key_1" id="WEP1" maxlength="26" value="" onKeyUp="setChange(1)">&nbsp;&nbsp;<select id="WEP1Select" name="WEP1Select" onChange="setChange(1)"> 
					<option value="1">ASCII</option>
					<option value="0">Hex</option></select></td>
							</tr>
							<tr>
							  <td>&nbsp;</td>
							  <td width="17%" id="secureWEPKey2">WEP ��Կ2:</td>
							  <td width="58%"><input name="wep_key_2" id="WEP2" maxlength="26" value="" onKeyUp="setChange(1)">&nbsp;&nbsp;<select id="WEP2Select" name="WEP2Select" onChange="setChange(1)">
					<option value="1">ASCII</option>
					<option value="0">Hex</option></select></td>
							</tr>
							<tr>
							  <td>&nbsp;</td>
							  <td width="17%" id="secureWEPKey3">WEP ��Կ3:</td>
							  <td width="58%"><input name="wep_key_3" id="WEP3" maxlength="26" value="" onKeyUp="setChange(1)">&nbsp;&nbsp;<select id="WEP3Select" name="WEP3Select" onChange="setChange(1)">
					<option value="1">ASCII</option>
					<option value="0">Hex</option></select></td>
							</tr>
							<tr>
							  <td>&nbsp;</td>
							  <td width="17%" id="secureWEPKey4">WEP ��Կ4:</td>
							  <td width="58%"><input name="wep_key_4" id="WEP4" maxlength="26" value="" onKeyUp="setChange(1)">&nbsp;&nbsp;<select id="WEP4Select" name="WEP4Select" onChange="setChange(1)">
					<option value="1">ASCII</option>
					<option value="0">Hex</option></select></td>
							</tr>
              			</table>
					</td>
          		</tr>
				-->
				<tr id="div_radius_server" name="div_radius_server" style="visibility: hidden; display:none">
					<td>
					  <table class="space" width="100%" border="0" cellspacing="0" cellpadding="0">
					  	<tr>
							<td class="titlebg" colspan="2" id="secureRadius">Radius ������</td>
				  		</tr>
						<tr>
						  <td width="25%" class="contentpadding" id="secureRadiusIPAddr">IP ��ַ:</td>
						  <td width="75%"><input name="RadiusServerIP" id="RadiusServerIP" size="16" maxlength="32" value="" onKeyUp="setChange(1)"></td>
						</tr>
						<tr>
						  <td width="25%" class="contentpadding" id="secureRadiusPort">�˿�:</td>
						  <td width="75%"><input name="RadiusServerPort" id="RadiusServerPort" size="5" maxlength="5" value="" onKeyUp="setChange(1)"></td>
						</tr>
						<tr>
						  <td width="25%" class="contentpadding" id="secureRadiusSharedSecret">������Կ:</td>
						  <td width="75%"><input type="password" name="RadiusServerSecret" id="RadiusServerSecret" size="16" maxlength="64" value="" onKeyUp="setChange(1)"></td>
						</tr>
						<tr>
						  <td width="25%" class="contentpadding" id="secureRadiusSessionTimeout">�Ự��ʱ:</td>
						  <td width="75%"><input name="RadiusServerSessionTimeout" id="RadiusServerSessionTimeout" size="3" maxlength="4" value="" onKeyUp="setChange(1)"></td>
						</tr>
						<tr>
						  <td width="25%" class="contentpadding" id="secureRadiusIdleTimeout">���г�ʱ:</td>
						  <td width="75%"><input name="RadiusServerIdleTimeout" id="RadiusServerIdleTimeout" size="3" maxlength="4" value="" onKeyUp="setChange(1)" ></td>
						</tr>
					  </table>
					</td>
				</tr>
				<tr style="display:none; visibility:hidden">
					<td>
<script language="JavaScript" type="text/javascript">
var aptable;

for(aptable = 0; aptable < MBSSID_MAX; aptable++){
    //alert("1");
	document.write(" <table class=space width=100% id=AccessPolicy_"+ aptable +" border=0 cellspacing=0 cellpadding=0 >");
	//alert("2")
	//alert("2-1")
	//alert("2-2")
	document.write(" <tr> <td class=titlebg colspan=2>"+"���ʲ���"+"</td></tr>");
	//alert("3")
	document.write(" <tr> <td class=contentpadding >"+"����"+"</td>");
	//alert("4")
	document.write(" <td> <select name=apselect_"+ aptable + " id=apselect_"+aptable+" size=1 onchange=\"setChange(1)\">");
	//alert("5")
	document.write(" 			<option value=0 >"+"����"+"</option> <option value=1 >"+"����"+"</option><option value=2 >"+"�ܾ�"+"</option></select> </td></tr>");
	//alert("6")

	for(i=0; i< ACCESSPOLICYLIST_MAX/2; i++){
		input_name = "newap_"+ aptable +"_" + (2*i);
		td_name = "newap_td_"+ aptable +"_" + (2*i);

		document.write(" <tr id=id_"+aptable+"_");
		document.write(i*2);
		document.write("> <td id=");
		document.write(td_name);
		document.write("> <input class=button value=\" ɾ�� \" onclick=\"delap("+aptable+", ");
		document.write(2*i);
		document.write(")\" type=button > <input id=");
		document.write(input_name);
		document.write("size=16 maxlength=20 readonly></td>");

		input_name = "newap_" + aptable + "_" + (2*i+1);
		td_name = "newap_td_" + aptable + "_" + (2*i+1);
		document.write("<td id=");
		document.write(td_name);
		document.write("> <input class=button value=\" ɾ�� \" onclick=\"delap("+aptable+", ");
		document.write(2*i+1);
		document.write(")\" type=button> <input id=");
		document.write(input_name);
		document.write(" size=16 maxlength=20 readonly></td> </tr>");
	}

	document.write("<tr><td class=contentpadding  >"+"����"+"</td>");
	document.write("<td><input name=newap_text_"+aptable+" id=newap_text_"+aptable+" size=16 maxlength=20></td></tr></table>");
}
</script>
				   </td>
				</tr>
				<tr>
					<td>&nbsp;</td>
				</tr>
				<tr>
					<td class="contentpadding"><input type="hidden" value="/wir_security.asp" name="submit-url"><input type="button" class="button" value="�� ��" id="secureApply" onClick="submit_apply()" />&nbsp;&nbsp;<input type="reset" class="button"  id="secureCancel" value="ȡ ��" onClick="window.location.reload()"></td>
				</tr>
				<tr>
					<td>&nbsp;</td>
				</tr>
	  		 </table>
	 	 </td>
  	</tr>
	<tr>
	   <td>&nbsp;</td>
	</tr>
</table>
</form>
</body>
</html>