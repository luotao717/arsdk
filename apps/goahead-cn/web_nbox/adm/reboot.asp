<html>
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Expires" content="-1">
<meta http-equiv="Content-Type" content="text/html; charset=<% getCharset(); %>">
<link rel="stylesheet" href="../style/normal_ws.css" type="text/css">
<script language="javascript" src="../js/language_<% getCfgZero(1, "LanguageType"); %>.js"></script>
<script language="javascript" src="../js/common.js"></script>
<script language="javascript">
function rebootClick()
{
	if ( !confirm(JS_msg87) ) 
		return false;
	else
		return true;
}
</script>
</head>
<body>
<table width=700><tr><td>
<table width=100% border=0 cellpadding=3 cellspacing=1> 
<tr><td class="title"><script>dw(MM_reboot)</script></td></tr>
<tr style="display:none"><td><script>dw(JS_msg124)</script></td></tr>
<tr><td><hr></td></tr>
</table>

<form method="post" name="RebootSystem" action="/goform/RebootSystem">
<table width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr>
    <td class="thead"><script>dw(JS_msg128)</script>:</td>
    <td><script>dw('<input type="submit" class=button3 value="'+BT_reboot+'" name="Reboot" onClick="return rebootClick()">')</script></td>
  </tr>
</table>
</form>

</td></tr></table>
</body></html>
