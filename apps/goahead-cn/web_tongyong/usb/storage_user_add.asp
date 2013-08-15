<html>
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Expires" content="-1">
<meta http-equiv="Content-Type" content="text/html; charset=<% getCharset(); %>">
<link rel="stylesheet" href="../style/normal_ws.css" type="text/css">
<script language="javascript" src="../js/language_<% getCfgZero(1, "LanguageType"); %>.js"></script>
<script language="javascript" src="../js/common.js"></script>
<script language="javascript">
var ftpb = '<% getFtpBuilt(); %>';
var smbb = '<% getSmbBuilt(); %>';

function Load_Setting()
{
	document.getElementById("div_ftp").style.display = "none";
	document.getElementById("div_smb").style.display = "none";
	document.storage_adduser.adduser_ftp[0].disabled = true;
	document.storage_adduser.adduser_ftp[1].disabled = true;
	document.storage_adduser.adduser_smb[0].disabled = true;
	document.storage_adduser.adduser_smb[1].disabled = true;
	
	if (ftpb == "1"){
		document.getElementById("div_ftp").style.display = "";
		document.storage_adduser.adduser_ftp[0].disabled = false;
		document.storage_adduser.adduser_ftp[1].disabled = false;
	}
	if (smbb == "1"){
		document.getElementById("div_smb").style.display = "";
		document.storage_adduser.adduser_smb[0].disabled = false;
		document.storage_adduser.adduser_smb[1].disabled = false;
	}
}

function formCheck()
{
	if (!isAscciMsg(document.storage_adduser.adduser_name.value, MM_username)) 
		return false;
	
	var i = 0;
	while(i < 9){
		if (document.storage_adduser.adduser_name.value == eval('opener.document.forms[0].hiddenUser'+i+'.value') 
			|| document.storage_adduser.adduser_name.value == "anonymous"){
			alert(JS_msg40);
			return false;
		}
		i++;
	}
	
	return true;
}

function addUserClose()
{
	opener.location.reload();
}

function submit_apply()
{
	if (formCheck() == true){
		document.storage_adduser.submit();
		window.close();
	}
}
</script>
</head>
<body onLoad="Load_Setting()" onUnload="addUserClose()">
<table width=600><tr><td>
<table width=100% border=0 cellpadding=3 cellspacing=1> 
<tr><td class="title"><script>dw(MM_user_settings)</script></td></tr>
<tr><td><script>dw(JS_msg_user)</script></td></tr>
<tr><td><hr></td></tr>
</table>

<form method=post name="storage_adduser" action="/goform/StorageAddUser">
<input type="hidden" name="submit-url" value="/usb/storage_user_admin.asp">
<table width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr>
    <td class="thead"><script>dw(MM_username)</script>:</td>
    <td><input type=text name=adduser_name maxlength=8 value=""></td>
  </tr>
  <tr>
    <td class="thead"><script>dw(MM_password)</script>:</td>
    <td><input type="password" name="adduser_pw" maxlength="16" value=""></td>
  </tr>
  <tr id="div_ftp" style="display:none"> 
    <td class="thead"><script>dw(MM_ftp_srv_settings)</script>:</td>
    <td><input type=radio name=adduser_ftp value="1"><script>dw(MM_enable)</script>
    <input type=radio name=adduser_ftp value="0" checked><script>dw(MM_disable)</script></td>
  </tr>
  <tr id="div_smb" style="display:none"> 
    <td class="thead"><script>dw(MM_smb_settings)</script>:</td>
    <td><input type=radio name=adduser_smb value="1"><script>dw(MM_enable)</script>
    <input type=radio name=adduser_smb value="0" checked><script>dw(MM_disable)</script></td>
  </tr>
</table>

<br>
<table width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr>
    <td>
      <script>dw('<input type=button class=button value="'+BT_apply+'" onClick="submit_apply()"> &nbsp; &nbsp;\
      <input type=button class=button value="'+BT_reset+'" onClick="window.close()">')</script>
    </td>
  </tr>
</table>
</form>

</td></tr></table>
</body></html>
