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
function Load_Setting()
{
	var printersrvebl = '<% getCfgZero(1, "PrinterSrvEnabled"); %>';
	document.getElementById("div_no_usbdivice").style.display = "none";
	document.getElementById("div_usbdivice").style.display = "";
	if (printersrvebl == "1")
		document.printer.enabled[0].checked = true;
	else
		document.printer.enabled[1].checked = true;
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
<tr><td class="title"><script>dw(MM_printer_srv_settings)</script></td></tr>
<tr><td><script>dw(JS_msg_printer)</script></td></tr>
<tr><td><hr></td></tr>
</table>

<form method=post name=printer action="/goform/printersrv">
<input type="hidden" name="submit-url" value="/usb/p910printer_srv.asp">
<table width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr> 
    <td class="thead"><script>dw(MM_function)</script>:</td>
    <td><input type="radio" name="enabled" value="1"><script>dw(MM_enable)</script>
    <input type="radio" name="enabled" value="0"><script>dw(MM_disable)</script></td>
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
