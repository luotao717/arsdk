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
	var opmode = <% getCfgZero(1, "OperationMode"); %>;	
	var wifi_off = <% getCfgZero(1, "WiFiOff"); %>;
			
	if (opmode == 0){
		document.getElementById("div_wan_br").style.display = "none";
		document.getElementById("div_wan").style.display = "none";
	}
	else{
		document.getElementById("div_wan_br").style.display = "";
		document.getElementById("div_wan").style.display = "";
	}
	
	if (wifi_off == 1){
		document.getElementById("div_wirless_br").style.display = "none";
		document.getElementById("div_wirless").style.display = "none";
	}
	else{	
		document.getElementById("div_wirless_br").style.display = "";
		document.getElementById("div_wirless").style.display = "";
	}
}
</script>
</head>
<body onLoad="Load_Setting()" class="mainbody">
<blockquote>
<table width=700><tr><td>
<table width=100% border=0 cellpadding=3 cellspacing=1> 
<tr><td class="title"><script>dw(MM_statistics)</script></td></tr>
<tr><td><script>dw(JS_msg_statistic)</script></td></tr>
<tr><td><hr></td></tr>
</table>
<br>
<table class="list">
<tr><td class="tcenter"><script>dw(MM_memory_info)</script></td></tr>
<tr><td><table class="list1">
<tr>
  <td class="thead"><script>dw(MM_memory_total)</script>:</td>
  <td> <% getMemTotalASP(); %></td>
</tr>
<tr>
  <td class="thead"><script>dw(MM_memory_left)</script>:</td>
  <td> <% getMemLeftASP(); %></td>
</tr>
</table></td></tr>

<tr id="div_wan_br"><td class="tcenter"><script>dw(MM_wan_iface)</script></td></tr>
<tr id="div_wan"><td><table class="list1">
<tr>
  <td class="thead"><script>dw(MM_rx_packets)</script>:</td>
  <td> <% getWANRxPacketASP(); %></td>
</tr>
<!--<tr>
  <td class="thead"><script>dw(MM_rx_bytes)</script>:</td>
  <td> <% getWANRxByteASP(); %></td>
</tr>-->
<tr>
  <td class="thead"><script>dw(MM_tx_packets)</script>:</td>
  <td> <% getWANTxPacketASP(); %></td>
</tr>
<!--<tr>
  <td class="thead"><script>dw(MM_tx_bytes)</script>:</td>
  <td> <% getWANTxByteASP(); %></td>
</tr>-->
</table></td></tr>

<tr id="div_wirless_br"><td class="tcenter"><script>dw(MM_wireless_iface)</script></td></tr>
<tr id="div_wirless"><td><table class="list1">
<tr>
  <td class="thead"><script>dw(MM_rx_packets)</script>:</td>
  <td> <% getApStats("RxSucc"); %></td>
</tr>
<tr>
  <td class="thead"><script>dw(MM_tx_packets)</script>:</td>
  <td> <% getApStats("TxSucc"); %></td>
</tr>
</table></td></tr>

<tr><td class="tcenter"><script>dw(MM_lan_iface)</script></td></tr>
<tr><td><table class="list1">
<tr>
  <td class="thead"><script>dw(MM_rx_packets)</script>:</td>
  <td> <% getLANRxPacketASP(); %></td>
</tr>
<!--<tr>
  <td class="thead"><script>dw(MM_rx_bytes)</script>:</td>
  <td> <% getLANRxByteASP(); %></td>
</tr>-->
<tr>
  <td class="thead"><script>dw(MM_tx_packets)</script>:</td>
  <td> <% getLANTxPacketASP(); %></td>
</tr>
<!--<tr>
  <td class="thead"><script>dw(MM_tx_bytes)</script>:</td>
  <td> <% getLANTxByteASP(); %></td>
</tr>-->
</table></td></tr>
</table>
<br><br>
</td></tr></table>
</blockquote>
</body></html>
