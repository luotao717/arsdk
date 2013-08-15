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
var selectedAPSSID;
var selectedAPBSSID;
var selectedAPStatus;

var http_request = false;
function makeRequest(url, content, sync)
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
		alert('Cannot create an XMLHTTP instance');
		return false;
	}
	http_request.onreadystatechange = alertContents;

	http_request.open('POST', url, (sync == 1) ? false : true);
	http_request.send(content);
}

function isSafeForShell(str)
{
    for(i=0; i < str.length; i++){
		if(str.charAt(i) == ',' ) return false;
		if(str.charAt(i) == '\\') return false;
		if(str.charAt(i) == ';' ) return false;
		if(str.charAt(i) == '\'') return false;
		if(str.charAt(i) == '\n') return false;
		if(str.charAt(i) == '`' ) return false;
		if(str.charAt(i) == '\"') return false;
	}
	return true;
}

function alertContents() 
{
	if (http_request.readyState == 4) {
		if (http_request.status == 200) {
			WPSStaUpdateStatus( http_request.responseText);
		} else {
//			alert('There was a problem with the request.');
		}
	}
}

function updateWPSStaStatus()
{
	makeRequest("/goform/updateWPSStaStatus", "n/a");
}

function WPSStaUpdateStatus(str)
{
	document.getElementById("WPSInfo").value = str;
}

var WPSAPListStr = "<% getStaWPSBSSIDListASP(0); %>";
var WPSAPList = new Array();
WPSAPList = WPSAPListStr.split("\n\n");


var need_submit_registrar_setting_flag = 0;
var PINCode =  "<% getWPSSTAPINCodeASP(); %>";
var RegistrarSSID = "<% getWPSSTARegSSIDASP(); %>";
var RegistrarAuth = "<% getWPSSTARegAuthASP(); %>";
var RegistrarEncry = "<% getWPSSTARegEncryASP(); %>";
var RegistrarKeyType =  "<% getWPSSTARegKeyTypeASP(); %>";
var RegistrarKeyIndex = "<% getWPSSTARegKeyIndexASP(); %>";
var RegistrarKey =  "<% getWPSSTARegKeyASP(); %>";

function InitRegistrarSetting()
{
	enableSubmitButton(false);

	//SSID
	document.getElementById("SSID").value = RegistrarSSID;

	if (RegistrarAuth == ""){
		RegistrarAuth = "WPA2PSK";
		RegistrarEncry = "AES";
		RegistrarKeyType = "1";
		RegistrarKeyIndex = "1";
		RegistrarKey = "12345678";
		need_submit_registrar_setting_flag = 1;
		enableSubmitButton(true);
	}

	//Auth
	if (RegistrarAuth == "OPEN"){
		document.getElementById("Authenication").options.selectedIndex = 0;
		AuthChange();
		if (RegistrarEncry == "NONE"){
			document.getElementById("EncryptTypeSelect").options.selectedIndex = 0;
		}
		else if (RegistrarEncry == "WEP"){
			document.getElementById("EncryptTypeSelect").options.selectedIndex = 1;

			// wep key type
			if (RegistrarKeyType == "0")
				document.getElementById("KeyTypeSelect").options.selectedIndex = 0;
			else if (RegistrarKeyType == "1")
				document.getElementById("KeyTypeSelect").options.selectedIndex = 1;
			else
				return;

			// wep key index
			if (RegistrarKeyIndex == "1")
				document.getElementById("KeyIndexSelect").options.selectedIndex = 0;
			else if (RegistrarKeyIndex == "2")
				document.getElementById("KeyIndexSelect").options.selectedIndex = 1;
			else if (RegistrarKeyIndex == "3")
				document.getElementById("KeyIndexSelect").options.selectedIndex = 2;
			else if (RegistrarKeyIndex == "4")
				document.getElementById("KeyIndexSelect").options.selectedIndex = 3;

			document.getElementById("Key").value = RegistrarKey;
		}
		else{
			alert("internal error 1");
			return;
		}
	}
	else if (RegistrarAuth == "WPAPSK"){
		document.getElementById("Authenication").options.selectedIndex = 1;
		AuthChange();
		if (RegistrarEncry == "TKIP"){
			document.getElementById("EncryptTypeSelect").options.selectedIndex = 0;
		}
		else if (RegistrarEncry == "AES"){
			document.getElementById("EncryptTypeSelect").options.selectedIndex = 1;
		}
		else{
			alert("internal error 2");
			return;
		}
		document.getElementById("Key").value = RegistrarKey;
//		document.getElementById("SSID").value = RegistrarKey;
	}
	else if (RegistrarAuth == "WPA2PSK"){
		document.getElementById("Authenication").options.selectedIndex = 2;	
		AuthChange();
		if (RegistrarEncry == "TKIP"){
			document.getElementById("EncryptTypeSelect").options.selectedIndex = 0;
		}
		else if (RegistrarEncry == "AES"){
			document.getElementById("EncryptTypeSelect").options.selectedIndex = 1;
		}
		else{
			alert("Internal error 2");
			return;
		}
		document.getElementById("Key").value = RegistrarKey;
	}
	else{
		alert("Internal error 3;");
		return;
	}

	EncryChange();
}

function GTLTConvert(str)
{
	var i;
	var result = "";
	for(i=0; i < str.length; i++){
		if(str.charAt(i) == '>')
			result = result + "&gt;";
		else if(str.charAt(i) == '<')
			result = result + "&lt;";
		else
			result = result + str.charAt(i);
	}
	return result;
}

function VersionTranslate(num)
{
	if (num == 16)
		return "1.0";
	else if (num == 17)
		return "1.1";
	else
		return "?";
}

function StateTranslate(num)
{
	if (num == 1)
		return "Unconf.";
	else if (num ==2)
		return "Conf.";
	else
		return "?";
}

function WPSAPClick(num)
{
	document.getElementById("APInfo").value = "";

	var WPSAPInfo = new Array;
	WPSAPInfo = WPSAPList[num].split("\n");	

	var i;
	for(i=8; i< WPSAPInfo.length; i++)
		document.getElementById("APInfo").value += (WPSAPInfo[i] + "\n") ;
}

function RegistrarTableShow(show)
{
	if (show)
		document.getElementById("registrarSettingTable").style.display = "";
	else
		document.getElementById("registrarSettingTable").style.display = "none";	
}

var STAMode = <% getWPSSTAModeASP(); %>;
function Load_Setting()
{
	if (WPSAPList.length)
		WPSAPClick(0);

	STATimerFlag = 1;
	InitializeTimer(3);

	InitRegistrarSetting();

	document.getElementById("STAWPSMode").options.selectedIndex = STAMode ? 1: 0;
	STAWPSModeChange();
}

function STAWPSModeChange()
{
	if (document.getElementById("STAWPSMode").value == "Registrar"){
		// make flash keep this status
		makeRequest("/goform/WPSSTAMode", "1", 1);
		if (STAMode == 0)
			window.location.reload();
		STAMode = 1;
		document.getElementById("PIN").readOnly = false;
		document.getElementById("PIN").value = "";
		RegistrarTableShow(true);
		document.getElementById("gen_new_pin").style.display = "none";
		need_submit_registrar_setting_flag = 1;
		enableSubmitButton(true);
		document.getElementById("stawpsPBCStart").disabled = true;
	}
	else{
		STAMode = 0;
		document.getElementById("PIN").readOnly = true;
		document.getElementById("PIN").value = PINCode;
		RegistrarTableShow(false);
		document.getElementById("gen_new_pin").style.display = "";

		// make flash keep this status
		makeRequest("/goform/WPSSTAMode", "0", 1);
		document.getElementById("stawpsPBCStart").disabled = false;
	}
}

function getSelectedSSID()
{
	var i;
	for(i=0; i< document.wpssta_form.WPSAPSelect.length ; i++){
		if(document.wpssta_form.WPSAPSelect[i].checked == true)
			break;
	}

	if(i == document.wpssta_form.WPSAPSelect.length){
		alert("Please select a WPS AP to process!");
		return;
	}

	var WPSAP;
	WPSAP = new Array();
	WPSAP = WPSAPList[i].split("\n");
	selectedAPSSID = WPSAP[0]
 	selectedAPBSSID = WPSAP[1]
	selectedAPStatus = WPSAP[6];
}

function PINStart()
{
	if (STAMode == 0){ // enrollee
		getSelectedSSID();
		makeRequest("/goform/WPSSTAPINEnr", selectedAPBSSID);
	}
	else if (STAMode == 1){	// Registrar
		if (!checkPIN(document.getElementById("PIN").value)){
			alert('PIN code error, please try again!');
			return;
		}

		if (need_submit_registrar_setting_flag){
			alert("This is the first time you running WPS registrar mode and the registrar settings need to be submitted!");
			return;
		}
		
		if (document.getElementById("submitButton").disabled == false){
			ret = confirm("You have changed the registrar settings but did NOT submit them.\nDo you really want running WPS without these settings?");
			if (!ret)
				return;
		}

		if (document.getElementById("Authenication").value == "OPEN" && document.getElementById("EncryptTypeSelect").value == "NONE")
			alert("This configuration does not provide any safety mechanism!");
			
		getSelectedSSID();
		var ret = 0;
		if (selectedAPStatus == 2) 
		{
			if (confirm("!!! The choosed AP ("+selectedAPSSID+") already stayed at configured state. !!!\nDo you continue?"))
				ret = 1;
		}
		
		makeRequest("/goform/WPSSTAPINReg", document.getElementById("PIN").value+" "+ selectedAPBSSID+" "+ret);
	} 
}

function PBCStart()
{
	if (STAMode == 0) // enrollee
		makeRequest("/goform/WPSSTAPBCEnr", "n/a1");
	else if (STAMode == 1)	// Registrar
		makeRequest("/goform/WPSSTAPBCReg", "n/a2");
}

function CancelSelected()
{
	makeRequest("/goform/WPSSTAStop", "n/a3");
}

function genNewPin()
{
	makeRequest("/goform/WPSSTAGenNewPIN", "n/a4", 1);
	//update PIN
	PINCode = http_request.responseText;	
	document.getElementById("PIN").value = PINCode;
}

function checkPIN(wsc_pin_code)
{
	var tmp_str = wsc_pin_code.replace("-", "");
	var pincode = tmp_str.replace(" ", "");

	document.getElementById("PIN").value = pincode;
	if(pincode.length == 4)
		return true;
	if (pincode.length != 8)
		return false;

	accum = 0;
	accum += 3 * (parseInt(pincode / 10000000) % 10);
	accum += 1 * (parseInt(pincode / 1000000) % 10);
	accum += 3 * (parseInt(pincode / 100000) % 10);
	accum += 1 * (parseInt(pincode / 10000) % 10);
	accum += 3 * (parseInt(pincode / 1000) % 10);
	accum += 1 * (parseInt(pincode / 100) % 10);
	accum += 3 * (parseInt(pincode / 10) % 10);
	accum += 1 * (parseInt(pincode / 1) % 10);
			
	if ((accum % 10) != 0)
		return false;
	return true;
}

function RegistrarSettingSubmit()
{
	var ret;
	if (!isSafeForShell(document.getElementById("SSID").value)){
		alert("The SSID contains dangerous characters!");
		return false;
	}

	if (!isSafeForShell(document.getElementById("Key").value)){
		alert("The Key contains dangerous characters!");
		return false;
	}

	if (document.getElementById("Authenication").value == "OPEN"){
		if (document.getElementById("EncryptTypeSelect").value == "NONE"){
			if (!confirm("No any security settings are selected. Are you sure?"))
				return false;
		}
		else if (document.getElementById("EncryptTypeSelect").value == "WEP"){
			if (document.getElementById("KeyTypeSelect").value == "1"){
				if (document.getElementById("Key").value.length != 5 && document.getElementById("Key").value.length != 13){
					alert("Invalid WEP Key!");
					return false;
				}
			}
			else if (document.getElementById("KeyTypeSelect").value == "0"){
				if (document.getElementById("Key").value.length != 10 && document.getElementById("Key").value.length != 26){
					alert("Invalid WEP Key!");
					return false;
				}
				
				if (!hexCheck(document.getElementById("Key"), "WEP Key"))
					return false;
			}
		}
	}
	else if (document.getElementById("Authenication").value == "WPAPSK" || document.getElementById("Authenication").value == "WPA2PSK" ){
		if (document.getElementById("Key").value.length < 8 || document.getElementById("Key").value.length > 64){
			alert("Key length invalid.");
			return false;
		}
	}
	else{
		return false;
	}

	enableSubmitButton(false);
	need_submit_registrar_setting_flag = 0;
	document.getElementById("hiddenPIN").value = document.getElementById("PIN").value;

	makeRequest("/goform/WPSSTARegistrarSetupSSID", document.getElementById("SSID").value);
	makeRequest("/goform/WPSSTARegistrarSetupRest",	document.getElementById("Authenication").value + " " +
												document.getElementById("EncryptTypeSelect").value + " " +
												document.getElementById("KeyTypeSelect").value + " " +
												document.getElementById("KeyIndexSelect").value);
	makeRequest("/goform/WPSSTARegistrarSetupKey", document.getElementById("Key").value);
	return true;
}

function enableSubmitButton(enable)
{
	document.getElementById("submitButton").disabled = (!enable);
}

function AuthChange()
{
	encry_select = document.getElementById("EncryptTypeSelect");
	auth = document.getElementById("Authenication").value;
	
	document.getElementById("KeyRow").style.display = "none";
	document.getElementById("keytype").style.display = "none";
	document.getElementById("keyindex").style.display = "none";

	if(auth == "OPEN"){
		encry_select.options.length = 0;
		encry_select.options[encry_select.length] = new Option("NONE", "NONE",	false, false);
		encry_select.options[encry_select.length] = new Option("WEP", "WEP", false, false);
	}
	else if(auth == "WPAPSK" || auth == "WPA2PSK"){
		encry_select.options.length = 0;
		encry_select.options[encry_select.length] = new Option("TKIP", "TKIP",	false, false);
		encry_select.options[encry_select.length] = new Option("AES", "AES", false, false);
	}
	else{
		return;
	}
	EncryChange();
}

function EncryChange()
{
	encry_select = document.getElementById("EncryptTypeSelect");
	auth = document.getElementById("Authenication").value;
	if(auth == "OPEN" && encry_select.value == "NONE" ){
		document.getElementById("KeyRow").style.display = "none";
		document.getElementById("keytype").style.display = "none";
		document.getElementById("keyindex").style.display = "none";
	}
	else if(auth == "OPEN" && encry_select.value == "WEP" ){
		document.getElementById("KeyRow").style.display = "";
		document.getElementById("keytype").style.display = "";
		document.getElementById("keyindex").style.display = "";
	}
	else{
		document.getElementById("KeyRow").style.display = "";
		document.getElementById("keytype").style.display = "none";
		document.getElementById("keyindex").style.display = "none";
	}
}

function RefreshClick()
{
	makeRequest("/goform/WPSSTABSSIDListReset", "n/a", 1);
	location.href=location.href;
}
</script>
</head>
<body onLoad="Load_Setting()" class="mainbody">
<blockquote>
<table width=700><tr><td>
<table width=100% border=0 cellpadding=3 cellspacing=1> 
<tr><td class="title">WPS Setup (STA)</td></tr>
<tr><td>You could setup security easily by choosing PIN or PBC method to do WPS Setting.</td></tr>
<tr><td><hr></td></tr>
</table>

<form method="post" name="wpssta_form">
<table width=100% border=0 cellpadding=3 cellspacing=1> 
<tr>
  <td class="title2" colspan="9"> WPS AP site survey:</td>
</tr>
<script language="javascript">
//	document.write("<tr><td>No.</td><td>SSID</td><td>BSSID</td><td>RSSI</td><td>Ch.</td><td>Auth.</td><td>Encrypt</td><td>Ver.</td><td>Status</td></tr>");
	document.write("<tr><td>No.</td><td>SSID</td><td>BSSID</td><td>RSSI</td><td>Ch.</td><td>Auth.</td><td>Encrypt</td><td>Status</td></tr>");

	var i;
	for(i=0; i < WPSAPList.length -1; i++){
		var WPSAP;
		WPSAP = new Array();
		WPSAP = WPSAPList[i].split("\n");

		document.write("<tr>");
	    // ID: ???
		//document.write();

		//Radio
		document.write("<td>");
//		document.write(i+1);
		document.write("<input type=radio name=WPSAPSelect value=");
		document.write(i);
		document.write(" onClick=\"WPSAPClick(");
		document.write(i);
		document.write(");\"");
		if(i==0)
			document.write(" checked ");
		document.write("></td>");

	    // SSID
		document.write("<td>");
		document.write( GTLTConvert(  WPSAP[0] )  );
		document.write("</td>");

		//BSSID
		document.write("<td>");
		document.write(WPSAP[1]);
		document.write("</td>");

		//RSSI
		document.write("<td>");
		document.write(WPSAP[2]);
		document.write("%</td>");

	    // Channel
		document.write("<td>");
		document.write(WPSAP[3]);
		document.write("</td>");

		//Auth
		document.write("<td>");
		document.write(WPSAP[4]);
		document.write("</td>");

        //Encry
		document.write("<td>");
		document.write(WPSAP[5]);
		document.write("</td>");

        //Version
		//document.write("<td>");
		//document.write( VersionTranslate (WPSAP[6] ) );
		//document.write("</td>");

        //wsc_state
		document.write("<td>");
		//document.write( StateTranslate ( WPSAP[7] ) );
		document.write( StateTranslate ( WPSAP[6] ) );
		document.write("</td>");

		// extend
		//document.write("<td>");
		//document.write(WPSAP[8]);
		//document.write("</td>");
		document.write("</tr>");
	}
</script>
</table>
</form>

<textarea name="APInfo" id="APInfo" cols="63" rows="5" wrap="off" readonly="1"></textarea>
<br><br>
<input value="Refresh" id="stawpsRefresh" onClick="RefreshClick();" type="button">
<font id="stawpsMode_txt">Mode</font>:
<select name="STAWPSMode" id="STAWPSMode" onChange="STAWPSModeChange()">
<option value=Enrollee>Enrollee</option>
<option value=Registrar>Registrar</option>
</select>
<font id="stawpsPIN">PIN</font>:<input value="" name="PIN" id="PIN" size="10" maxlength="10" type="text">
<input value="PIN Start" onClick="PINStart();" type="button">
<input value="PBC Start" onClick="PBCStart();" type="button">
<input value="Reset" onClick="CancelSelected();" type="button">
<input name="gen_new_pin" id="gen_new_pin" value="Renew PIN" onClick="genNewPin();" type="button">
<br>
<br>
<table id="registrarSettingTable" width=100% border=0 cellpadding=3 cellspacing=1> 
<form method="post" name="registrar_form" action="/goform/WPSSTARegistrar">
<tr>
  <td class="title2" colspan="2"> Registrar Setup:</td>
</tr>
<tr> 
  <td class="thead">SSID:</td> 
  <td><input value="" name="SSID" id="SSID" size="32" maxlength="32" type="text" onKeyUp="enableSubmitButton(true);">  </td>
</tr>
<tr> 
  <td class="thead">Authenication:</td> 
  <td><select name="Authenication" id="Authenication" onChange="AuthChange();enableSubmitButton(true);">
		<option value=OPEN>OPEN</option>
		<option value=WPAPSK>WPA-PSK</option>
		<option value=WPA2PSK>WPA2-PSK</option>
     </select></td>
</tr>
<tr id="encrypttype"> 
	<td class="thead">Encrypt Type:</td> 
    <td><select name="EncryptType" id="EncryptTypeSelect" onChange="EncryChange();enableSubmitButton(true);">
	<!-- javascript would update this -->
    </select></td>
</tr>
<tr id="keytype"> 
  <td class="thead">WEP Key Type:</td> 
  <td><select name="KeyType" id="KeyTypeSelect" onChange="enableSubmitButton(true);">
		<option value="0">Hex</option>
		<option value="1">ASCII</option>
    </select></td>
</tr>
<tr id="keyindex"> 
  <td class="thead">WEP Key Index:</td> 
  <td><select name="KeyIndex" id="KeyIndexSelect" onChange="enableSubmitButton(true);">
		<option value=1>1</option>
		<option value=2>2</option>
		<option value=3>3</option>
		<option value=4>4</option>
	</select></td>
</tr>
<tr id="KeyRow"> 
  <td class="thead">WEP Key:</td> 
  <td><input value="" name="Key" id="Key" size="32" maxlength="64" type="text" onKeyUp="enableSubmitButton(true);">  </td>
</tr>
<tr>
	<td colspan="2">
		<input type="hidden" name="hiddenPIN" id="hiddenPIN" value="">
		<input name="submitButton" id="submitButton" value="Submit" class="button" onClick="RegistrarSettingSubmit();" type="button"> 
	</td>
</tr>
</form>
</table>
<br>

<table width=100% border=0 cellpadding=3 cellspacing=1> 
<tr>
  <td class="title2">WPS Status:</td>
</tr>
<tr>
  <td><textarea name="WPSInfo" id="WPSInfo" style="width:90%" rows="2" wrap="off" readonly="1"></textarea></td>
</tr>
</table>

</td></tr></table>
</blockquote>
</body></html>

