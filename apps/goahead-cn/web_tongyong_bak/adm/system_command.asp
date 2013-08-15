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
	if (!isEmptyMsg(document.SystemCommand.command.value, "Command")) 
		return false;

	return true;
}

function setFocus()
{
	document.SystemCommand.command.focus();
}

function resetForm()
{
	location=location; 
}
</script>
</head>
<body onLoad="setFocus()" class="mainbody">
<blockquote>
<table width=700><tr><td>
<table width=100% border=0 cellpadding=3 cellspacing=1> 
<tr><td class="title">System Command</td></tr>
<tr><td>Run a system command as root.</td></tr>
<tr><td><hr></td></tr>
</table>

<form method="post" name="SystemCommand" action="/goform/SystemCommand">
<input type="hidden" name="submit-url" value="/adm/system_command.asp">
<table width=100% border=0 cellpadding=3 cellspacing=1> 
<tr>
  <td class="thead">Command:</td>
  <td><input type="text" name="command" size="30" maxlength="256" ></td>
</tr>
<tr>
  <td colspan=2><textarea style="font-size:9pt;width:90%" rows="20" wrap="off" readonly="1"><% showSystemCommandASP(); %></textarea></td>
</tr>
</table>

<br>
<table width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr>
    <td>
      <input type=submit class=button value="Apply" onClick="return formCheck()"> &nbsp; &nbsp;
      <input type=button class=button value="Reset" onClick="resetForm();">
    </td>
  </tr>
</table>
</form>
<!--
<form method="post" name="repeatLastSystemCommand" action="/goform/repeatLastSystemCommand">
<input value="Repeat Last Command" name="repeatLastCommand" type="submit">
</form>
-->

</td></tr></table>
</blockquote>
</body></html>
