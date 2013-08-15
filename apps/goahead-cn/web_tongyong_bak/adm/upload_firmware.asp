<html>
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Expires" content="-1">
<meta http-equiv="Content-Type" content="text/html; charset=<% getCharset(); %>">
<link rel="stylesheet" href="../style/normal_ws.css" type="text/css">
<script language="javascript" src="../js/language_<% getCfgZero(1, "LanguageType"); %>.js"></script>
<script language="javascript" src="../js/common.js"></script>
<script language="javascript">
document.write('<div id="loading" style="display: none;"><br>'+JS_msg90+'<br></div>');

var _singleton = 0;
function uploadFirmwareCheck()
{
	if (_singleton)
		return false;
		
	if (document.UploadFirmware.filename.value == ""){
		alert(JS_msg10);
		return false;
	}

	//document.UploadFirmware.UploadFirmwareSubmit.disabled = true;
	//document.UploadFirmware.filename.disabled = true;
    document.getElementById("loading").style.display="block";
	_singleton = 1;
	return true;
}

function Load_Setting()
{
    document.UploadFirmware.UploadFirmwareSubmit.disabled = false;
//	document.UploadFirmware.filename.disabled = false;
	document.getElementById("loading").style.display="none";
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
<tr><td class="title"><script>dw(MM_firmware)</script></td></tr>
<tr><td><script>dw(JS_msg_firmware)</script></td></tr>
<tr><td><hr></td></tr>
</table>

<form method="post" name="UploadFirmware" action="/cgi-bin/upload.cgi" enctype="multipart/form-data">
<table width=100% border=0 cellpadding=3 cellspacing=1> 
<tr>
  	<td class="thead"><script>dw(MM_cur_firmware)</script>:</td>
  	<td><% getSdkVersion(); %></td>
</tr>
<tr>
  	<td class="thead"><script>dw(MM_firmware_date)</script>:</td>
  	<td><script>dw(checkDate("<% getSysBuildTime(); %>"));</script></td>
</tr>
<tr>
    <td class="thead"><script>dw(MM_select_firmware_file)</script>:</td>
	<td><input type="file" name="filename" size="20" maxlength="256"></td>
</tr>
</table>

<br>
<table width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr>
    <td>
      <script>dw('<input type="submit" value="'+BT_upgrade+'" class=button name="UploadFirmwareSubmit" onClick="return uploadFirmwareCheck();"> &nbsp; &nbsp;\
      <input type=button class=button value="'+BT_reset+'" onClick="resetForm();">')</script>
    </td>
  </tr>
</table>
</form>

</td></tr></table>
</blockquote>
</body></html>
