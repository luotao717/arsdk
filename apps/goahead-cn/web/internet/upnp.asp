<html>
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Expires" content="-1">
<meta http-equiv="Content-Type" content="text/html; charset=<% getCharset(); %>">
<link rel="stylesheet" href="../style/normal_ws.css" type="text/css">
<script language="javascript" src="../js/language_<% getCfgZero(1, "LanguageType"); %>.js"></script>
<script language="javascript" src="../js/common.js"></script>
<script language="javascript">
function Load_Setting()
{
	document.upnpCfg.upnpEnbl.options.selectedIndex = 1*<% getCfgZero(1, "upnpEnabled"); %>;
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
<tr><td class="title"><script>dw(MM_upnp_settings)</script></td></tr>
<tr><td><script>dw(JS_msg_upnp)</script></td></tr>
<tr><td><hr></td></tr>
</table>

<form method=post name="upnpCfg" action="/goform/setUpnp">
<input type="hidden" name="submit-url" value="/internet/upnp.asp">
<table width=100% border=0 cellpadding=3 cellspacing=1> 
<tr>
  <td class="thead">UPnP:</td>
  <td><select name="upnpEnbl">
      <option value="0"><script>dw(MM_disable)</script></option>
      <option value="1"><script>dw(MM_enable)</script></option>
    </select></td>
</tr>
</table>

<br>
<table width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr>
    <td>
      <script>dw('<input type=submit class=button value="'+BT_apply+'"> &nbsp; &nbsp;\
      <input type=button class=button value="'+BT_reset+'" onClick="resetForm();">')</script>
    </td>
  </tr>
</table>
</form>

</td></tr></table>
</blockquote>
</body></html>
