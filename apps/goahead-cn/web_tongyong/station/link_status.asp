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
	var n_mode = "<% getLinkingMode(); %>";
	if (n_mode == "0")
		document.getElementById("linkHT").style.display = "none";
	else
		document.getElementById("linkHT").style.display = "";
}
</script>
</head>
<body onLoad="Load_Setting()" class="mainbody">
<blockquote>
<table width=700><tr><td>
<h2>Station Link Status</h2>
<p>The Status page shows the settings and current operation status of the Station.</p>
<hr />
<form method=post name="sta_link_status" action="/goform/setStaDbm">
<table width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr>
    <td colspan="3" class="title">Link Status</td>
  </tr>
  <tr>
    <td class="thead">Status</td>
    <td colspan="2"><% getStaLinkStatus(); %></td>
  </tr>
  <tr>
    <td class="thead">Extra Info</td>
    <td colspan="2"><% getStaExtraInfo(); %></td>
  </tr>
  <tr>
    <td class="thead">Channel</td>
    <td colspan="2"><% getStaLinkChannel(); %></td>
  </tr>
  <tr>
    <td class="thead">Link Speed</td>
    <td>Tx(Mbps)&nbsp;&nbsp;<% getStaLinkTxRate(); %></td>
    <td>Rx(Mbps)&nbsp;&nbsp;<% getStaLinkRxRate(); %></td>
  </tr>
  <tr>
    <td class="thead">Throughput</td>
    <td>Tx(Mbps)&nbsp;&nbsp;<% getStaTxThroughput(); %></td>
    <td>Rx(Mbps)&nbsp;&nbsp;<% getStaRxThroughput(); %></td>
  </tr>
  <tr>
    <td class="thead">Link Quality</td>
    <td colspan="2"><% getStaLinkQuality(); %></td>
  </tr>
  <tr>
    <td class="thead">Signal Strength 1</td>
    <td><% getStaSignalStrength(); %></td>
    <td rowspan="4"><input type="checkbox" name="dbmChecked" <% dbm = getStaDbm(); if (dbm == "1") write("checked"); %> OnClick="submit();">dBm format</td>
  </tr>
  <tr>
    <td class="thead">Signal Strength 2</td>
    <td><% getStaSignalStrength_1(); %></td>
  </tr>
  <tr>
    <td class="thead">Signal Strength 3</td>
    <td><% getStaSignalStrength_2(); %></td>
  </tr>
  <tr>
    <td class="thead">Noise Level</td>
    <td><% getStaNoiseLevel(); %></td>
  </tr>
</table>
<br />

<table id="linkHT"width="600" border="1" cellspacing="1" cellpadding="3" bordercolor="#9BABBD">
  <tr>
    <td class="title" colspan="2">HT</td>
  </tr>
  <% getStaHT(); %>
  <tr>
    <td class="thead">SNR0</td>
    <td><% getStaSNR(0); %></td>
  </tr>
  <tr>
    <td class="thead">SNR1</td>
    <td><% getStaSNR(1); %></td>
  </tr>
  <tr>
    <td class="thead">SNR2</td>
    <td><% getStaSNR(2); %></td>
  </tr>
  <tr>
    <td class="thead">Stream SNR</td>
    <td><% getStaStreamSNR(); %></td>
  </tr>
</table>
<input type=hidden name=dummyData value="1">
</form>

</td></tr></table>
</body></html>
