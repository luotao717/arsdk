<html>
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Expires" content="-1">
<meta http-equiv="Content-Type" content="text/html; charset=<% getCharset(); %>">
<link rel="stylesheet" href="../style/normal_ws.css" type="text/css">
<script language="javascript" src="../js/language_<% getCfgZero(1, "LanguageType"); %>.js"></script>
<script language="javascript" src="../js/common.js"></script>
<script language="javascript">
function resetClick()
{
	if ( !confirm(JS_msg11) )
		return false;
	else
		return true;
}
</script>
</head>
<body>
<table width=700><tr><td>
<table width=100% border=0 cellpadding=3 cellspacing=1> 
<tr><td class="title"><script>dw(MM_restore_default)</script></td></tr>
<tr style="display:none"><td><script>dw(JS_msg124)</script></td></tr>
<tr><td><hr></td></tr>
</table>

<form method="post" name="LoadDefaultSettings" action="/goform/LoadDefaultSettings">
<table width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr>
    <td class="thead"><script>dw(JS_msg127)</script>:</td>
    <td><script>dw('<input type="submit" class=button3 value="'+BT_restore_default+'" name="LoadDefault" onClick="return resetClick()">')</script></td>
  </tr>
</table>
</form>

</td></tr></table>
</body></html>
