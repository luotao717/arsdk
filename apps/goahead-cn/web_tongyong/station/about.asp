<html>
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Expires" content="-1">
<meta http-equiv="Content-Type" content="text/html; charset=<% getCharset(); %>">
<link rel="stylesheet" href="../style/normal_ws.css" type="text/css">
<script language="javascript" src="../js/language_<% getCfgZero(1, "LanguageType"); %>.js"></script>
<script language="javascript" src="../js/common.js"></script>
</head>
<body onLoad="Load_Setting()" class="mainbody">
<blockquote>
<table width=700><tr><td>
<h2>Station About</h2>
<p>The Status page shows the settings and current operation status of the Station.</p>
<hr />
<table width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr>
    <td class="title" colspan="2">About</td>
  </tr>
  <tr>
    <td class="thead">Driver Version</td>
    <td><% getStaDriverVer(); %></td>
  </tr>
  <tr>
    <td class="thead">MAC Address</td>
    <td><% getStaMacAddr(); %></td>
  </tr>
</table>
</td></tr></table>
</body></html>
