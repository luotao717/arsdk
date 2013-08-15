<html>
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Expires" content="-1">
<meta http-equiv="Content-Type" content="text/html; charset=<% getCharset(); %>">
<link rel="stylesheet" href="../style/normal_ws.css" type="text/css">
<script language="javascript" src="../js/language_<% getCfgZero(1, "LanguageType"); %>.js"></script>
<script language="javascript" src="../js/common.js"></script>
<script language="javascript">
var PhyMode  = '<% getCfgZero(1, "WirelessMode"); %>';
var meshenable = '<% getCfgZero(1, "MeshEnabled"); %>';

function hiddenBasicWeb()
{
	document.mesh_form.MeshID.disabled = true;
	document.mesh_form.HostName.disabled = true;
	document.mesh_form.AutoLinkEnable.disabled = true;
	document.getElementById("div_mesh_settings").style.display = "none";
	document.getElementById("manual_link").style.display = "none";
	document.mesh_manual_link.mpmac.disabled = true;
}

function hiddenSecureWeb()
{
	document.getElementById("div_open_secure_mode").style.display = "none";
	document.mesh_form.open_encrypt_type.disabled= true;
	document.getElementById("div_wpa_algorithms").style.display = "none";
	document.mesh_form.wpa_cipher[0].disabled = true;
	document.mesh_form.wpa_cipher[1].disabled = true;
	document.getElementById("div_wep").style.display = "none";
	document.mesh_form.wep_key.disabled = true;
	document.mesh_form.wep_select.disabled = true;
	document.getElementById("div_wpa").style.display = "none";
	document.mesh_form.passphrase.disabled = true;
}

function Load_Setting()
{
	var AuthMode = '<% getCfgGeneral(1,"MeshAuthMode"); %>';
	var EncrypType = '<% getCfgGeneral(1,"MeshEncrypType"); %>';

	if (meshenable == "1"){
		document.mesh_form.MeshEnable.options.selectedIndex = 1;
		var autolink = '<% getCfgZero(1, "MeshAutoLink"); %>';
		if (autolink == "1")
			document.mesh_form.AutoLinkEnable[1].checked = true;
		else
			document.mesh_form.AutoLinkEnable[0].checked = true;

		if (AuthMode == "OPEN" || AuthMode == ""){
			document.mesh_form.security_mode.options.selectedIndex = 0;
			if (EncrypType == "NONE"){
				document.mesh_form.open_encrypt_type.options.selectedIndex = 0;
			}
			else if (EncrypType == "WEP"){
				var WEPKeyType = '<% getCfgGeneral(1,"MeshWEPKEYType"); %>';
				document.mesh_form.open_encrypt_type.options.selectedIndex = 1;
				document.mesh_form.wep_select.options.selectedIndex = (WEPKeyType == "0" ? 1 : 0);
			}
		}
		else if (AuthMode == "WPANONE"){
			document.mesh_form.security_mode.options.selectedIndex = 1;
			if (EncrypType == "TKIP")
				document.mesh_form.wpa_cipher[0].checked = true;
			else if (EncrypType == "AES")
				document.mesh_form.wpa_cipher[1].checked = true;
		}
	}
	else{
		document.mesh_form.MeshEnable.options.selectedIndex = 0;
	}
	switch_mesh_capacity();
}

function switch_mesh_capacity()
{
	hiddenBasicWeb();
	hiddenSecureWeb();
	if (document.mesh_form.MeshEnable.options.selectedIndex == 1){
		document.getElementById("div_mesh_settings").style.display = "";
		document.mesh_form.MeshID.disabled = false;
		document.mesh_form.HostName.disabled = false;
		document.mesh_form.AutoLinkEnable.disabled = false;
		switch_autolink();
		switch_security_mode();
	}
}

function switch_autolink()
{
	if (document.mesh_form.AutoLinkEnable[0].checked == true){
			document.getElementById("manual_link").style.display = "";
		document.mesh_manual_link.mpmac.disabled = false;
	}
	else{
		document.getElementById("manual_link").style.display = "none";
		document.mesh_manual_link.mpmac.disabled = true;
	}
}

function switch_security_mode()
{
	hiddenSecureWeb();
	if (document.mesh_form.security_mode.options.selectedIndex == 1){
		document.getElementById("div_wpa_algorithms").style.display = "";
		document.mesh_form.wpa_cipher[0].disabled = false;
		document.mesh_form.wpa_cipher[1].disabled = false;
		document.getElementById("div_wpa").style.display = "";
		document.mesh_form.passphrase.disabled = false;
	}
	else{
		document.getElementById("div_open_secure_mode").style.display = "";
		document.mesh_form.open_encrypt_type.disabled= false;
		switch_open_encrypt();
	}
}

function switch_open_encrypt()
{
	if (document.mesh_form.open_encrypt_type.options.selectedIndex == 1){
		document.getElementById("div_wep").style.display = "";
		document.mesh_form.wep_key.disabled = false;
		document.mesh_form.wep_select.disabled = false;
	}
	else{
		document.getElementById("div_wep").style.display = "none";
		document.mesh_form.wep_key.disabled = true;
		document.mesh_form.wep_select.disabled = true;
	}
}

function formCheck()
{
	if (document.mesh_form.MeshEnable.options.selectedIndex == 1){
		if (document.mesh_form.MeshID.value == ""){
			alert("Please enter Mesh ID!");
			document.mesh_form.MeshID.focus();
			return false;
		}
		if (document.mesh_form.security_mode.options.selectedIndex == 0){
			var encrypt_type = document.mesh_form.open_encrypt_type.options.selectedIndex;
			if (encrypt_type == 1 && !check_wep())
				return false;
		}
		else if (document.mesh_form.security_mode.options.selectedIndex == 1){
			if (check_wpa() == false)
				return false;
		}

	}
	return true;
}

function check_wep()
{
	var keylength = document.mesh_form.wep_key.value.length;
	if (keylength == 0){
		alert('Please input WEP key!');
		document.mesh_form.wep_key.focus();
		return false;
	}
	
	if (keylength != 0){
		if (document.mesh_form.wep_select.options.selectedIndex == 0){
			if (keylength != 5 && keylength != 13) {
				alert('Please input 5 or 13 characters of WEP Key!');
				document.mesh_form.wep_key.focus();
				return false;
			}
			
			if (!stringCheck(document.mesh_form.wep_key, "WEP Key"))
				return false;
		}
		if (document.mesh_form.wep_select.options.selectedIndex == 1){
			if(keylength != 10 && keylength != 26) {
				alert('Please input 10 or 26 characters of WEP key!');
				document.mesh_form.wep_key.focus();
				return false;
			}
			
			if (!hexCheck(document.mesh_form.wep_key, "WEP Key"))
				return false;
		}
	}
	return true;
}

function check_wpa()
{
	if (document.mesh_form.wpa_cipher[0].checked != true && document.mesh_form.wpa_cipher[1].checked != true){
		alert('Please choose a WPA Algorithms!');
		return false;
	}
	
	var keyvalue = document.mesh_form.passphrase.value;
	if (keyvalue.length == 0){
		alert('Please input WPA Key!');
		document.mesh_form.passphrase.focus();
		return false;
	}

	if (keyvalue.length < 8){
		alert("WPA Key length should be larger than 8!");
		document.mesh_form.passphrase.focus();
		return false;
	}
	
	if (!stringCheck(document.mesh_form.passphrase, "WPA Key"))
		return false;

	return true;
}

function checkData()
{
	if (!blankCheck(document.mesh_manual_link.mpmac, "MAC address")) 
		return false;
		
	if (!macCheck(document.mesh_manual_link.mpmac, "MAC address"))  
		return false;
			
	return true;
}

function mesh_link_submit(parm)
{
	if (checkData()){
		document.mesh_manual_link.link_action.value = parm;
		document.mesh_manual_link.submit();
	}
}

function web_refresh()
{
	window.location.reload();
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
<tr><td class="title">Mesh Setup</td></tr>
<tr><td>A Mesh network is an IEEE 802 LAN comprised of IEEE 802.11 links and control elements to forward
frames among the network members.</td></tr>
<tr><td><hr></td></tr>
</table>

<form method="post" name="mesh_form" action="/goform/wirelessMesh">
<table width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr>
    <td class="thead">Mesh Capacity:</td>
    <td><select name="MeshEnable" onChange="switch_mesh_capacity()">
        <option value="0" selected>Disable</option>
		<option value="1">Enable</option>
    	</select></td>
  </tr>
</table>

<br />
<table id="div_mesh_settings" width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr> 
    <td class="thead">Mesh ID (MID):</td>
    <td><input type="text" name="MeshID" maxlength="32" value="<% getCfgGeneral(1, "MeshId"); %>"></td>
  </tr>
  <tr> 
    <td class="thead">Host Name:</td>
    <td><input type="text" name="HostName" maxlength="32" value="<% getCfgGeneral(1, "MeshHostName"); %>"></td>
  </tr>
  <tr>
    <td class="thead">Auto Link:</td>
    <td><input type="radio" name="AutoLinkEnable" value="0" onClick="switch_autolink()" checked>Disable<input type="radio" name="AutoLinkEnable" value="1" onClick="switch_autolink()">Enable</td>
  </tr>
</table>

<br />
<table width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr> 
    <td class="thead">Security Mode:</td>
    <td><select name="security_mode" onChange="switch_security_mode()">
        <option value="OPEN" selected>OPEN</option>
        <option value="WPANONE">WPANONE</option>
      </select></td>
  </tr>
  <tr id="div_open_secure_mode"> 
    <td class="thead">Encrypt Type:</td>
    <td><select name="open_encrypt_type" onChange="switch_open_encrypt()">
	<option value="NONE" selected>None</option>
	<option value="WEP">WEP</option>
      </select></td>
  </tr>
  <tr id="div_wpa_algorithms"> 
    <td class="thead">WPA Algorithms:</td>
    <td><input name="wpa_cipher" value="TKIP" type="radio" checked>TKIP<input name="wpa_cipher" value="AES" type="radio">AES</td>
  </tr>
</table>

<table id="div_wep" width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr> 
    <td class="thead">WEP Key:</td>
    <td><input name="wep_key" id="WEP" maxlength="26" value="<% getCfgGeneral(1, "MeshWEPKEY"); %>"> <select name="wep_select"> 
        <option value="1">ASCII</option>
	<option value="0" selected>Hex</option>
      </select></td>
  </tr>
</table>

<table id="div_wpa" width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr>
    <td class="thead">Pass Phrase:</td>
    <td><input name="passphrase" size="32" maxlength="64" value="<% getCfgGeneral(1,"MeshWPAKEY"); %>"></td>
  </tr>
</table>

<br />
<table width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr>
    <td>
      <input type="submit" class=button value="Apply" onClick="return formCheck()"> &nbsp; &nbsp;
      <input type="button" class=button value="Reset" onClick="resetForm();">
    </td>
  </tr>
</table>
</form>  

<br />
<form method="post" name="mesh_manual_link" action="/goform/meshManualLink">
<input type="hidden" name="link_action" value="">
<table id="manual_link" width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr>
    <td class="title2" colspan="2">Manual Mesh Link<hr></td>
  </tr>
  <tr>
    <td class="thead">Mesh Point MAC Address:</td>
    <td><input type="text" name="mpmac" size=20 maxlength=17 value=""></td>
  </tr>
</table>

<br />
<table width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr>
    <td>
      <input type="button" class=button value="ADD" onClick="mesh_link_submit('add')"> &nbsp; &nbsp;
      <input type="button" class=button value="DEL" onClick="mesh_link_submit('del')">
    </td>
  </tr>
</table>
</form>  

<br />
<table id="mesh_info" width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr>
    <td class="title2" colspan="7">Current Mesh Network Table:</td>
  </tr>
  <tr bgcolor=#C0C0C0 align="center">
    <td>&nbsp;&nbsp;</td>
    <td>Neighbor MAC Address</td>
    <td>RSSI</td>
    <td>Mesh ID</td>
    <td>Host Name</td>
    <td>Channel</td>
    <td>Encrypt Type</td>
  </tr>
  <% ShowMeshState(); %>
</table>

<br>
<table width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr>
    <td>
      <input type="button" class=button name="refresh" value="Refresh" onClick="web_refresh()">
    </td>
  </tr>
</table>

</td></tr></table>
</blockquote>
</body></html>

