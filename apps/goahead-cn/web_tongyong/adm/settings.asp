<html>
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Expires" content="-1">
<meta http-equiv="Content-Type" content="text/html; charset=<% getCharset(); %>">
<link rel="stylesheet" href="../style/normal_ws.css" type="text/css">
<script language="javascript" src="../js/language_<% getCfgZero(1, "LanguageType"); %>.js"></script>
<script language="javascript" src="../js/common.js"></script>
<script language="javascript">
function reloadfileCheck()
{
	var str=document.ImportSettings.filename.value;
	if (str == "") { 
		alert(JS_msg7);
		return false;
	} 
	
	if (str.toLowerCase().indexOf(".dat") == -1) {
		alert(JS_msg8);
		return false;
	}
	return true;
}

function resetClick()
{
	if ( !confirm(JS_msg11) )
		return false;
	else
		return true;
}

function rebootClick()
{
	if ( !confirm(JS_msg87) ) 
		return false;
	else
		return true;
}
</script>
</head>
<body class="mainbody">
<blockquote>
<table width=700><tr><td>
<table width=100% border=0 cellpadding=3 cellspacing=1> 
<tr><td class="title"><script>dw(MM_sysconfig)</script></td></tr>
<tr><td><script>dw(JS_msg_settings)</script></td></tr>
<tr><td><hr></td></tr>
</table>
<br>
<table width=100% border=0 cellpadding=3 cellspacing=1> 
<form method="post" name="ExportSettings" action="/cgi-bin/ExportSettings.sh">
  <tr>
    <td class="thead"><script>dw(MM_save_config_file)</script>:</td>
    <td><script>dw('<input type="submit" class=button3 value="'+BT_save+'" name="Export">')</script></td>
  </tr>
</form>

<form method="post" name="ImportSettings" action="/cgi-bin/upload_settings.cgi" enctype="multipart/form-data">
  <tr>
    <td class="thead"><script>dw(MM_update_config_file)</script>:</td>
    <td><input type="File" name="filename" size="20" maxlength="256"> <script>dw('<input type=submit class=button3 value="'+BT_update+'" onClick="return reloadfileCheck()">')</script></td>
  </tr>
</form>

<form method="post" name="LoadDefaultSettings" action="/goform/LoadDefaultSettings">
  <tr>
    <td class="thead"><script>dw(MM_load_factory_default)</script>:</td>
    <td><script>dw('<input type="submit" class=button3 value="'+BT_restore_default+'" name="LoadDefault" onClick="return resetClick()">')</script></td>
  </tr>
</form>

<form method="post" name="RebootSystem" action="/goform/RebootSystem">
  <tr>
    <td class="thead"><script>dw(MM_reboot_device_system)</script>:</td>
    <td><script>dw('<input type="submit" class=button3 value="'+BT_reboot+'" name="Reboot" onClick="return rebootClick()">')</script></td>
  </tr>
</form>
</table>

</td></tr></table>
</blockquote>
</body></html>
