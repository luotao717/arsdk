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
	var txbf = "<% getTxBfBuilt(); %>";
	if (txbf == "1")
		document.getElementById("div_info_normal").style.display = "none";
	else
		document.getElementById("div_info_txbf").style.display = "none";
}
</script>
</head>
<body onLoad="Load_Setting()">
<table width=700><tr><td>
<table width=100% border=0 cellpadding=3 cellspacing=1> 
<tr><td class="title"><script>dw(MM_wireless_status)</script></td></tr>
<tr style="display:none"><td><script>dw(JS_msg92)</script></td></tr>
<tr><td><hr></td></tr>
</table>
<br>
<br>
<table id="div_info_normal" width=100% border=1 cellpadding=3 style="border-collapse: collapse" bordercolor="#C8C8C8">
  <tr bgcolor=#f2f2f2>
    <td align=center><b><script>dw(MM_macaddr)</script></b></td>
    <td align=center><b>Aid</b></td>
    <td align=center><b>PSM</b></td>
    <td align=center><b>Mimo<br>PS</b></td>
    <td align=center><b>MCS</b></td>
    <td align=center><b><script>dw(MM_bandwidth)</script></b></td>
    <td align=center><b>SGI</b></td>
    <td align=center><b>STBC</b></td>
  </tr>
  <% getWlanStaInfo("ra0"); %>
</table>

<table id="div_info_txbf" width=100% border=1 cellpadding=3 style="border-collapse: collapse" bordercolor="#C8C8C8">
  <tr bgcolor=#f2f2f2>
    <td align=center><b><script>dw(MM_macaddr)</script></b></td>
    <td align=center><b>Aid</b></td>
    <td align=center><b>PSM</b></td>
    <td align=center><b>MIMO<br>PS</b></td>
    <td align=center><b><script>dw(MM_tx_rate)</script></b></td>
    <td align=center><b>TxBF</b></td>
    <td align=center><b>RSSI</b></td>
    <td align=center><b>Stream<br>SNR</b></td>
    <td align=center><b>Snd Rsp<br>SNR</b></td>
    <td align=center><b>Last<br>RX Rate</b></td>
    <td align=center><b>Connect<br>Time</b></td>
  </tr>
  <% getWlanStaInfo("ra0"); %>
</table>
<br><br>
</td></tr></table>
</body></html>
