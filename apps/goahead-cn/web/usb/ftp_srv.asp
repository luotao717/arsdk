<html>
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Expires" content="-1">
<meta http-equiv="Content-Type" content="text/html; charset=<% getCharset(); %>">
<link rel="stylesheet" href="../style/normal_ws.css" type="text/css">
<script language="javascript" src="../js/language_<% getCfgZero(1, "LanguageType"); %>.js"></script>
<script language="javascript" src="../js/common.js"></script>
<script language="javascript">
var usb_state = 1*"<% getIndex(1, "usbStatus"); %>";
var ftpenabled = '<% getCfgZero(1, "FtpEnabled"); %>';
var anonymous = '<% getCfgZero(1, "FtpAnonymous"); %>';
var ftpname = '<% getCfgGeneral(1, "FtpName"); %>';
var port = '<% getCfgGeneral(1, "FtpPort"); %>';
var maxsessions = '<% getCfgGeneral(1, "FtpMaxSessions"); %>';
var adddir = '<% getCfgZero(1, "FtpAddDir"); %>';
var rename = '<% getCfgZero(1, "FtpRename"); %>';
var remove = '<% getCfgZero(1, "FtpRemove"); %>';
var readfile = '<% getCfgZero(1, "FtpRead"); %>';
var writefile = '<% getCfgZero(1, "FtpWrite"); %>';
var download = '<% getCfgZero(1, "FtpDownload"); %>';
var upload = '<% getCfgZero(1, "FtpUpload"); %>';

function Load_Setting()
{
	if (usb_state == 0){
		document.getElementById("div_no_usbdivice").style.display = "";
		document.getElementById("div_usbdivice").style.display = "none";
	}
	else{
		document.getElementById("div_no_usbdivice").style.display = "none";
		document.getElementById("div_usbdivice").style.display = "";
	}
	
	document.storage_ftp.ftp_anonymous[0].disabled = true;
	document.storage_ftp.ftp_anonymous[1].disabled = true;
	document.storage_ftp.ftp_name.disabled = true;
	document.storage_ftp.ftp_port.disabled = true;
	document.storage_ftp.ftp_max_sessions.disabled = true;
	document.storage_ftp.ftp_adddir[0].disabled = true;
	document.storage_ftp.ftp_adddir[1].disabled = true;
	document.storage_ftp.ftp_rename[0].disabled = true;
	document.storage_ftp.ftp_rename[1].disabled = true;
	document.storage_ftp.ftp_remove[0].disabled = true;
	document.storage_ftp.ftp_remove[1].disabled = true;
	document.storage_ftp.ftp_read[0].disabled = true;
	document.storage_ftp.ftp_read[1].disabled = true;
	document.storage_ftp.ftp_write[0].disabled = true;
	document.storage_ftp.ftp_write[1].disabled = true;
	document.storage_ftp.ftp_download[0].disabled = true;
	document.storage_ftp.ftp_download[1].disabled = true;
	document.storage_ftp.ftp_upload[0].disabled = true;
	document.storage_ftp.ftp_upload[1].disabled = true;

	if (ftpenabled == "1"){
		document.storage_ftp.ftp_enabled[0].checked = true;
		document.storage_ftp.ftp_anonymous[0].disabled = false;
		document.storage_ftp.ftp_anonymous[1].disabled = false;
		if (anonymous == 1)
			document.storage_ftp.ftp_anonymous[0].checked = true;
		else
			document.storage_ftp.ftp_anonymous[1].checked = true;
		
		document.storage_ftp.ftp_name.disabled = false;
		document.storage_ftp.ftp_name.value = ftpname;

		document.storage_ftp.ftp_port.disabled = false;
		document.storage_ftp.ftp_port.value = port;

		document.storage_ftp.ftp_max_sessions.disabled = false;
		document.storage_ftp.ftp_max_sessions.value = maxsessions;

		document.storage_ftp.ftp_adddir[0].disabled = false;
		document.storage_ftp.ftp_adddir[1].disabled = false;
		if (adddir == 1)
			document.storage_ftp.ftp_adddir[0].checked = true;
		else
			document.storage_ftp.ftp_adddir[1].checked = true;

		document.storage_ftp.ftp_rename[0].disabled = false;
		document.storage_ftp.ftp_rename[1].disabled = false;
		if (rename == 1)
			document.storage_ftp.ftp_rename[0].checked = true;
		else
			document.storage_ftp.ftp_rename[1].checked = true;
		
		document.storage_ftp.ftp_remove[0].disabled = false;
		document.storage_ftp.ftp_remove[1].disabled = false;
		if (remove == 1)
			document.storage_ftp.ftp_remove[0].checked = true;
		else
			document.storage_ftp.ftp_remove[1].checked = true;

		document.storage_ftp.ftp_read[0].disabled = false;
		document.storage_ftp.ftp_read[1].disabled = false;
		if (readfile == 1)
			document.storage_ftp.ftp_read[0].checked = true;
		else
			document.storage_ftp.ftp_read[1].checked = true;

		document.storage_ftp.ftp_write[0].disabled = false;
		document.storage_ftp.ftp_write[1].disabled = false;
		if (writefile == 1)
			document.storage_ftp.ftp_write[0].checked = true;
		else
			document.storage_ftp.ftp_write[1].checked = true;

		document.storage_ftp.ftp_download[0].disabled = false;
		document.storage_ftp.ftp_download[1].disabled = false;
		if (download == 1)
			document.storage_ftp.ftp_download[0].checked = true;
		else
			document.storage_ftp.ftp_download[1].checked = true;

		document.storage_ftp.ftp_upload[0].disabled = false;
		document.storage_ftp.ftp_upload[1].disabled = false;
		if (upload == 1)
			document.storage_ftp.ftp_upload[0].checked = true;
		else
			document.storage_ftp.ftp_upload[1].checked = true;
	}
	else{
		document.storage_ftp.ftp_enabled[1].checked = true;
	}
}

function formCheck()
{
	if (document.storage_ftp.ftp_enabled[0].checked == true){
		if (!isBlankMsg(document.storage_ftp.ftp_name.value, MM_ftp_name)) 
			return false;
		
		if (!isPortMsg(document.storage_ftp.ftp_port.value, MM_ftp_port)) 
			return false;
		
		if (!isNumberMsg(document.storage_ftp.ftp_max_sessions.value, MM_ftp_max_sessions)) 
			return false;
	}
	
	return true;
}

function ftp_enable_switch()
{
	if (document.storage_ftp.ftp_enabled[1].checked == true){
		document.storage_ftp.ftp_anonymous[0].disabled = true;
		document.storage_ftp.ftp_anonymous[1].disabled = true;
		document.storage_ftp.ftp_name.disabled = true;
		document.storage_ftp.ftp_port.disabled = true;
		document.storage_ftp.ftp_max_sessions.disabled = true;
		document.storage_ftp.ftp_adddir[0].disabled = true;
		document.storage_ftp.ftp_adddir[1].disabled = true;
		document.storage_ftp.ftp_rename[0].disabled = true;
		document.storage_ftp.ftp_rename[1].disabled = true;
		document.storage_ftp.ftp_remove[0].disabled = true;
		document.storage_ftp.ftp_remove[1].disabled = true;
		document.storage_ftp.ftp_read[0].disabled = true;
		document.storage_ftp.ftp_read[1].disabled = true;
		document.storage_ftp.ftp_write[0].disabled = true;
		document.storage_ftp.ftp_write[1].disabled = true;
		document.storage_ftp.ftp_download[0].disabled = true;
		document.storage_ftp.ftp_download[1].disabled = true;
		document.storage_ftp.ftp_upload[0].disabled = true;
		document.storage_ftp.ftp_upload[1].disabled = true;
	}
	else{
		document.storage_ftp.ftp_anonymous[0].disabled = false;
		document.storage_ftp.ftp_anonymous[1].disabled = false;
		document.storage_ftp.ftp_name.disabled = false;
		document.storage_ftp.ftp_port.disabled = false;
		document.storage_ftp.ftp_max_sessions.disabled = false;
		document.storage_ftp.ftp_adddir[0].disabled = false;
		document.storage_ftp.ftp_adddir[1].disabled = false;
		document.storage_ftp.ftp_rename[0].disabled = false;
		document.storage_ftp.ftp_rename[1].disabled = false;
		document.storage_ftp.ftp_remove[0].disabled = false;
		document.storage_ftp.ftp_remove[1].disabled = false;
		document.storage_ftp.ftp_read[0].disabled = false;
		document.storage_ftp.ftp_read[1].disabled = false;
		document.storage_ftp.ftp_write[0].disabled = false;
		document.storage_ftp.ftp_write[1].disabled = false;
		document.storage_ftp.ftp_download[0].disabled = false;
		document.storage_ftp.ftp_download[1].disabled = false;
		document.storage_ftp.ftp_upload[0].disabled = false;
		document.storage_ftp.ftp_upload[1].disabled = false;
	}
}

function resetForm()
{
	location=location; 
}
</script>
</head>
<body onLoad="Load_Setting()" class="mainbody">
<blockquote>
<table id="div_no_usbdivice" style="display:none" width=770><tr><td>
<table width=100% border=0 cellpadding=3 cellspacing=1> 
<tr><td><img src="../graphics/warning.gif" align="absmiddle">&nbsp;&nbsp;
<script>dw(JS_msg_usb)</script>&nbsp;&nbsp;
<script>dw('<input type=button class=button value="'+BT_refresh+'" onClick="window.location.reload()">')</script></td></tr>
</table>
</td></tr></table>

<table id="div_usbdivice" width=770><tr><td>
<table width=100% border=0 cellpadding=3 cellspacing=1> 
<tr><td class="title"><script>dw(MM_ftp_srv_settings)</script></td></tr>
<tr><td><script>dw(JS_msg_ftp)</script></td></tr>
<tr><td><hr></td></tr>
</table>

<form method=post name=storage_ftp action="/goform/storageFtpSrv">
<input type="hidden" name="submit-url" value="/usb/ftp_srv.asp">
<table width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr> 
    <td class="thead">FTP <script>dw(MM_function)</script>;</td>
    <td><input type=radio name=ftp_enabled value="1" onClick="ftp_enable_switch()"><script>dw(MM_enable)</script>
    <input type=radio name=ftp_enabled value="0" onClick="ftp_enable_switch()" checked><script>dw(MM_disable)</script></td>
  </tr>
  <tr>
    <td class="thead"><script>dw(MM_ftp_name)</script>:</td>
    <td><input type=text name=ftp_name maxlength=16 value="FTPServer"></td>
  </tr>
  <tr> 
    <td class="thead"><script>dw(MM_ftp_anonymous_login)</script>:</td>
    <td><input type=radio name=ftp_anonymous value="1"><script>dw(MM_enable)</script>
    <input type=radio name=ftp_anonymous value="0" checked><script>dw(MM_disable)</script></td>
  </tr>
  <tr>
    <td class="thead"><script>dw(MM_ftp_port)</script>:</td>
    <td><input type=text name=ftp_port size=5 maxlength=5 value="21"> (1-65535)</td>
  </tr>
  <tr>
    <td class="thead"><script>dw(MM_ftp_max_sessions)</script>:</td>
    <td><input type=text name=ftp_max_sessions size=2 maxlength=2 value="10"></td>
  </tr>
  <tr>
    <td class="thead"><script>dw(MM_ftp_create_dir)</script>:</td>
    <td><input type=radio name=ftp_adddir value="1" checked><script>dw(MM_enable)</script>
    <input type=radio name=ftp_adddir value="0"><script>dw(MM_disable)</script></td>
  </tr>
  <tr>
    <td class="thead"><script>dw(MM_ftp_rename_file_dir)</script>:</td>
    <td><input type=radio name=ftp_rename value="1" checked><script>dw(MM_enable)</script>
    <input type=radio name=ftp_rename value="0"><script>dw(MM_disable)</script></td>
  </tr>
  <tr>
    <td class="thead"><script>dw(MM_ftp_remove_file_dir)</script>:</td>
    <td><input type=radio name=ftp_remove value="1" checked><script>dw(MM_enable)</script>
    <input type=radio name=ftp_remove value="0"><script>dw(MM_disable)</script></td>
  </tr>
  <tr>
    <td class="thead"><script>dw(MM_ftp_readfile)</script>:</td>
    <td><input type=radio name=ftp_read value="1" checked><script>dw(MM_enable)</script>
    <input type=radio name=ftp_read value="0"><script>dw(MM_disable)</script></td>
  </tr>
  <tr>
    <td class="thead"><script>dw(MM_ftp_writefile)</script>:</td>
    <td><input type=radio name=ftp_write value="1" checked><script>dw(MM_enable)</script>
    <input type=radio name=ftp_write value="0"><script>dw(MM_disable)</script></td>
  </tr>
  <tr>
    <td class="thead"><script>dw(MM_ftp_download_capability)</script>:</td>
    <td><input type=radio name=ftp_download value="1" checked><script>dw(MM_enable)</script>
    <input type=radio name=ftp_download value="0"><script>dw(MM_disable)</script></td>
  </tr>
  <tr>
    <td class="thead"><script>dw(MM_ftp_upload_capability)</script>:</td>
    <td><input type=radio name=ftp_upload value="1" checked><script>dw(MM_enable)</script>
    <input type=radio name=ftp_upload value="0"><script>dw(MM_disable)</script></td>
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
