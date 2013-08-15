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
	if (txbf != "1")
		document.getElementById("div_stats_txbf").style.display = "none";
}
</script>
</head>
<body onLoad="Load_Setting()" class="mainbody">
<blockquote>
<table width=700><tr><td>
<table width=100% border=0 cellpadding=3 cellspacing=1> 
<tr><td class="title">AP Wireless Statistics</td></tr>
<tr><td>This page is used to show the wireless TX and RX statistics.</td></tr>
<tr><td><hr></td></tr>
</table>
<br>
<table width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr>
    <td class="tcenter" colspan="2">Transmit Statistics</td>
  </tr>
  <tr>
    <td class="thead">Tx Success:</td>
    <td><% getApStats("TxSucc"); %></td>
  </tr>
  <tr>
    <td class="thead">Tx Retry Count:</td>
    <td><% getApStats("TxRetry"); %></td>
  <tr>
    <td class="thead">Tx Fail after retry:</td>
    <td><% getApStats("TxFail"); %></td>
  <tr>
    <td class="thead">RTS Sucessfully Receive CTS:</td>
    <td><% getApStats("RTSSucc"); %></td>
  </tr>
  <tr>
    <td class="thead">RTS Fail To Receive CTS:</td>
    <td><% getApStats("RTSFail"); %></td>
  </tr>
</table>

<table width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr>
    <td class="tcenter" colspan="2">Receive Statistics:</td>
  </tr>
  <tr>
    <td class="thead">Frames Received Successfully:</td>
    <td><% getApStats("RxSucc"); %></td>
  </tr>
  <tr>
    <td class="thead">Frames Received With CRC Error:</td>
    <td><% getApStats("FCSError"); %></td>
  </tr>
</table>

<table width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr>
    <td class="tcenter" colspan="2">SNR:</td>
  </tr>
  <tr>
    <td class="thead">SNR:</td>
    <td><% getApSNR(0); %>, <% getApSNR(1); %>, <% getApSNR(2); %></td>
  </tr>
</table>

<table id="div_stats_txbf" width=100% border=0 cellpadding=3 cellspacing=1> 
  <% getApTxBFStats("ra0"); %>
</table>

<form method=post name="ap_statistics" action="/goform/resetApCounters">
<input type=hidden name=dummyData value="1">
<table width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr>
    <td>	
      <input type="submit" class=button3 value="Reset Counters">
    </td>
  </tr>
</table>
</form>

</td></tr></table>
</blockquote>
</body></html>
