<html>
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Expires" content="-1">
<meta http-equiv="Content-Type" content="text/html; charset=<% getCharset(); %>">
<link rel="stylesheet" href="../style/normal_ws.css" type="text/css">
<script language="javascript" src="../js/language_<% getCfgZero(1, "LanguageType"); %>.js"></script>
<script language="javascript" src="../js/common.js"></script>
<script language="javascript">
var mode = '<% getCfgGeneral(1, "ApCliAuthMode"); %>';
var enc = '<% getCfgGeneral(1, "ApCliEncrypType"); %>';

var keyid = '<% getCfgGeneral(1, "ApCliDefaultKeyID"); %>';
var key1type = '<% getCfgGeneral(1, "ApCliKey1Type"); %>';
//var key2type = '<% getCfgGeneral(1, "ApCliKey2Type"); %>';
//var key3type = '<% getCfgGeneral(1, "ApCliKey3Type"); %>';
//var key4type = '<% getCfgGeneral(1, "ApCliKey4Type"); %>';

function SecurityModeSwitch()
{
	document.getElementById("div_apcli_key1").style.display = "none";
	document.getElementById("div_aplci_enc").style.display = "none";
	document.getElementById("div_apcli_wpapsk").style.display = "none";
	
	if (document.apcli_form.apcli_mode.options.selectedIndex == 1)//WEP
	{
		document.getElementById("div_apcli_key1").style.display = "";
		document.apcli_form.apcli_default_key.options.selectedIndex = 0;
		document.apcli_form.apcli_enc_open.options.selectedIndex = 1;
	}	
	else if (document.apcli_form.apcli_mode.options.selectedIndex >= 2)//WPAPSK, WPA2PSK
	{
		document.getElementById("div_aplci_enc").style.display = "";
		document.getElementById("div_apcli_wpapsk").style.display = "";
	}
	else 
	{
		document.apcli_form.apcli_default_key.options.selectedIndex = 0;
		document.apcli_form.apcli_enc_open.options.selectedIndex = 0;
	}
}

function Load_Setting()
{
	if (mode == "OPEN" && enc == "NONE")
		document.apcli_form.apcli_mode.options.selectedIndex = 0;//Disable
	else if (mode == "OPEN" && enc == "WEP")
		document.apcli_form.apcli_mode.options.selectedIndex = 1;//WEP
	else if (mode == "WPAPSK")
		document.apcli_form.apcli_mode.options.selectedIndex = 2;//WPAPSK
	else if (mode == "WPA2PSK")
		document.apcli_form.apcli_mode.options.selectedIndex = 3;//WPA2PSK
	else
		document.apcli_form.apcli_mode.options.selectedIndex = 0;//Disable

	SecurityModeSwitch();
	
	if (enc == "NONE")
		document.apcli_form.apcli_enc_open.options.selectedIndex = 0;//NONE
	else if (enc == "WEP")
	{
		document.apcli_form.apcli_enc_open.options.selectedIndex = 1;//WEP	
		document.apcli_form.apcli_key1type.options.selectedIndex = 0;
	}
	
	if (enc == "TKIP")
		document.apcli_form.apcli_enc.options.selectedIndex = 0;//TKIP
	else if (enc == "AES")
		document.apcli_form.apcli_enc.options.selectedIndex = 1;//AES
}

function check_wep()
{
	if (document.apcli_form.apcli_enc_open.value == "WEP") 
	{
		if (document.apcli_form.apcli_key1.value.length == 0) 
		{
			alert(JS_msg52+'!');
			document.apcli_form.apcli_key1.focus();
			return false;
		}
	}

	if (document.apcli_form.apcli_key1type.options.selectedIndex == 0) 
	{
		if (document.apcli_form.apcli_key1.value.length != 0 && 
			document.apcli_form.apcli_key1.value.length != 5 && 
			document.apcli_form.apcli_key1.value.length != 13) 
		{
			alert(JS_msg53+"!");
			document.apcli_form.apcli_key1.focus();
			return false;
			
			if (!stringCheck(document.apcli_form.apcli_key1, MM_wep_key))
				return false;
		}
	}
	
	return true;
}

function check_wpa()
{
	var keyvalue = document.apcli_form.apcli_wpapsk.value;
	if (keyvalue.length == 0)
	{
		alert(JS_msg16);
		document.apcli_form.apcli_wpapsk.focus();
		return false;
	}
	
	if (keyvalue.length < 8)
	{
		alert(JS_msg17);
		document.apcli_form.apcli_wpapsk.focus();
		return false;
	}
	
	if (!stringCheck(document.apcli_form.apcli_wpapsk, MM_wpa_key))
		return false;
	
	return true;
}

function formCheck()
{
	if (!ssidCheck(document.apcli_form.apcli_ssid, MM_ssid)) 
		return false;

	if (document.apcli_form.apcli_bssid.value != '') 
	{
		if (!macCheck(document.apcli_form.apcli_bssid, "(BSSID)"+MM_macaddr))  
			return false;
	}

	if (document.apcli_form.apcli_mode.options.selectedIndex == 1)
		return check_wep();
	else if (document.apcli_form.apcli_mode.options.selectedIndex > 1)
		return check_wpa();
		
	return true;
}

function resetForm()
{
	location=location; 
}

function open_search_ap() 
{ 
	window.open("site_survey.asp", "newwindow", "height=500, width=700, toolbar=no, menubar=no, scrollbars=yes, resizable=yes, location=no, status=no"); 
} 

function opera()
{
	var i;
	var a = window.parent.document.getElementsByTagName('iframe');  
	for (var i=0; i<a.length; i++){  
		if (a[i].name == self.name) {  
			a[i].height = document.body.scrollHeight; 
			return;  
		}  
	}  
}
</script>
</head>
<body onLoad="opera();Load_Setting()">
<table width=700><tr><td>
<table width=100% border=0 cellpadding=3 cellspacing=1> 
<tr><td><hr></td></tr>
</table>

<form method=post name=apcli_form action="/goform/wirelessApcli">
<input type="hidden" name="submit-url" value="/wireless/apcli.asp">
<input type="hidden" name="apcli_channel" value="<% getCfgGeneral(1, "Channel"); %>">
<table width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr> 
    <td class="thead"><script>dw(MM_ssid)</script>:</td>
    <td><input type=text name="apcli_ssid" size=32 maxlength=32 value="<% getCfgToHTML(1, "ApCliSsid"); %>"> <script>dw('<input type=button class=button name="search_AP" value='+BT_search_ap+' onClick="open_search_ap()">')</script></td>
  </tr>
  <tr> 
    <td class="thead">BSSID (<script>dw(MM_macaddr)</script>):</td>
    <td><input type=text name="apcli_bssid" size=17 maxlength=17 value="<% getCfgGeneral(1, "ApCliBssid"); %>"> (<script>dw(MM_optional)</script>)</td>
  </tr>
  <tr> 
    <td class="thead"><script>dw(MM_security_mode)</script>:</td>
    <td><select name="apcli_mode" onChange="SecurityModeSwitch();">
        <option value="OPEN"><script>dw(MM_disable)</script></option>
        <option value="OPEN">WEP</option>
        <option value="WPAPSK">WPAPSK</option>
        <option value="WPA2PSK">WPA2PSK</option>
      	</select></td>
  </tr>
  <tr style="display:none">
    <td class="thead"><script>dw(MM_encryp_type)</script>:</td>
    <td><select name="apcli_enc_open">
    	<option value="NONE"><script>dw(MM_none)</script></option>
        <option value="WEP">WEP</option></select></td>
  </tr>
  
  <tr style="display:none">
    <td class="thead">WEP <script>dw(MM_default_key)</script>:</td>
    <td><select name="apcli_default_key">
        <option value="1"><script>dw(MM_key)</script> 1</option>
        <option value="2"><script>dw(MM_key)</script> 2</option>
        <option value="3"><script>dw(MM_key)</script> 3</option>
        <option value="4"><script>dw(MM_key)</script> 4</option>
        </select></td>
  </tr>  
  <!--key 1-->
  <tr style="display:none">
    <td class="thead"><script>dw(MM_encryp_type)</script>:</td>
    <td><select name="apcli_key1type"> 
        <option value="1"><script>dw(MM_ascii)</script></option>
		<option value="0"><script>dw(MM_hex)</script></option>
        </select></td>
  </tr>
  <tr id="div_apcli_key1">
    <td class="thead"><script>dw(MM_wep_key)</script>:</td>
    <td><input name="apcli_key1" maxlength="26" value="<% getCfgGeneral(1, "ApCliKey1Str"); %>"></td>
  </tr>
  
  <!--key 2-->
  <tr style="display:none">
    <td class="thead"><script>dw(MM_encryp_type)</script> 2:</td>
    <td><select name="apcli_key2type"> 
        <option value="1"><script>dw(MM_ascii)</script></option>
		<option value="0"><script>dw(MM_hex)</script></option>
      </select> <input name="apcli_key2" maxlength="26" value="<% getCfgGeneral(1, "ApCliKey2Str"); %>"></td>
  </tr>
  
  <!--key 3-->
  <tr style="display:none">
    <td class="thead"><script>dw(MM_encryp_type)</script> 3:</td>
    <td><select name="apcli_key3type"> 
        <option value="1"><script>dw(MM_ascii)</script></option>
		<option value="0"><script>dw(MM_hex)</script></option>
      </select> <input name="apcli_key3" maxlength="26" value="<% getCfgGeneral(1, "ApCliKey3Str"); %>"></td>
  </tr>
  
  <!--key 4-->
  <tr style="display:none">
    <td class="thead"><script>dw(MM_encryp_type)</script> 4:</td>
    <td><select name="apcli_key4type"> 
        <option value="1"><script>dw(MM_ascii)</script></option>
		<option value="0"><script>dw(MM_hex)</script></option>
      </select> <input name="apcli_key4" maxlength="26" value="<% getCfgGeneral(1, "ApCliKey4Str"); %>"></td>
  </tr>
  
  <tr id="div_aplci_enc">
    <td class="thead"><script>dw(MM_encryp_type)</script>:</td>
    <td><select name="apcli_enc">
        <option value="TKIP">TKIP</option>
        <option value="AES">AES</option></select></td>
  </tr>
  <tr id="div_apcli_wpapsk"> 
    <td class="thead"><script>dw(MM_passphrase)</script>:</td>
    <td><input type=text name="apcli_wpapsk" size=32 maxlength=64 value="<% getCfgGeneral(1, "ApCliWPAPSK"); %>"></td>
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
