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
	var txbf = "<% getStaTxBfBuilt(); %>";
	if (txbf != "1")
		document.getElementById("div_stats_txbf").style.display = "none";
}
</script>
</head>
<body onLoad="Load_Setting()" class="mainbody">
<blockquote>
<table width=700><tr><td>
<h2>Station Statistics</h2>
<p>The Status page shows the settings and current operation status of the Station.</p>
<hr />
<form method=post name="sta_statistics" action="/goform/resetStaCounters">
<table width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr>
    <td class="title" colspan="2">Transmit Statistics</td>
  </tr>
  <% getStaStatsTx(); %>
  <!--
  <tr>
    <td class="title" colspan="2">Receive Statistics</td>
  </tr>
  <tr>
    <td class="thead">Frames Received Successfully</td>
    <td><% getStaStatsRxOk(); %></td>
  </tr>
  -->
  <tr>
    <td class="thead">Frames Received With CRC Error </td>
    <td><% getStaStatsRxCRCErr(); %></td>
  </tr>
  <tr>
    <td class="thead">Frames Dropped Due To Out-of-Resource</td>
    <td><% getStaStatsRxNoBuf(); %></td>
  </tr>
  <tr>
    <td class="thead">Duplicate Frames Received </td>
    <td><% getStaStatsRxDup(); %></td>
  </tr>
</table>
<br />
<table id="div_stats_txbf" width=100% border=0 cellpadding=3 cellspacing=1> 
  <% getStaStatsTxBf(); %>
</table>
<input type=hidden name=dummyData value="1">
<table width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr>
    <td>
      <input type=submit class=button value="Reset Counters">
    </td>
  </tr>
</table>
</form>

</td></tr></table>
</body></html>
