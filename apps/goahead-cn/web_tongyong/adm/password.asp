<html>
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Expires" content="-1">
<meta http-equiv="Content-Type" content="text/html; charset=<% getCharset(); %>">
<link rel="stylesheet" href="../style/normal_ws.css" type="text/css">
<script language="javascript" src="../js/language_<% getCfgZero(1, "LanguageType"); %>.js"></script>
<script language="javascript" src="../js/common.js"></script>
<script language="javascript">
function formCheck()
{
	if (!isAbcMsg(document.Adm.admuser.value, MM_username)) 
		return false;
	
	if (!isAscciMsg(document.Adm.admpass.value, MM_password)) 
		return false;
	
	return true;
}

function Load_Setting()
{
	var watchdogb = "<% getWatchDogBuilt(); %>";
	if (watchdogb == "1") {
		document.getElementById("div_watchdog").style.display = "";
		document.Adm.admwatchdog.disabled = false;
		var watchdogcap = "<% getCfgZero(1, "WatchDogEnable"); %>";
		if (watchdogcap == "1")
			document.Adm.admwatchdog[0].checked = true;
		else
			document.Adm.admwatchdog[1].checked = true;
	} 
	else {
		document.getElementById("div_watchdog").style.display = "none";
		document.Adm.admwatchdog.disabled = true;
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
<table width=700><tr><td>
<table width=100% border=0 cellpadding=3 cellspacing=1> 
<tr><td class="title"><script>dw(MM_admin_settings)</script></td></tr>
<tr><td><script>dw(JS_msg_password)</script></td></tr>
<tr><td><hr></td></tr>
</table>

<form method="post" name="Adm" action="/goform/setSysAdm">
<input type="hidden" name="submit-url" value="/adm/password.asp">
<table width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr>
    <td class="thead"><script>dw(MM_username)</script>:</td>
    <td><input type="text" name="admuser" maxlength="16" value="<% getCfgGeneral(1, "Login"); %>"></td>
  </tr>
  <tr>
    <td class="thead"><script>dw(MM_password)</script>:</td>
    <td><input type="password" name="admpass" maxlength="32" value="<% getCfgGeneral(1, "Password"); %>"></td>
  </tr>
  <tr id="div_watchdog">
    <td class="thead">WatchDog:</td>
    <td><input type="radio" name="admwatchdog" value="1"><script>dw(MM_enable)</script>
    <input type="radio" name="admwatchdog" value="0"><script>dw(MM_disable)</script></td>
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
</blockquote>
</body></html>
