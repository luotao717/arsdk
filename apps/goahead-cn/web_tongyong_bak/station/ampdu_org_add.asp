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
	<% setRefreshSta11nCfg(); %>
}

function getConnectedBSSID()
{
	var tmp = "<% getStaConnectedBSSID(); %>";
	document.write(tmp);
}

function submit_apply()
{
	document.sta_org_add.submit();
	opener.location.reload();
	window.close();
}

function selectedBSSID(tmp)
{
	document.sta_org_add.selectedbssid.value = tmp;

	document.sta_org_add.mpdu_apply.disabled = true;
	if (document.sta_org_add.mac.checked)
		document.sta_org_add.mpdu_apply.disabled = false;
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
<h2>Add AMPDU Originator</h2>
<form method=post name="sta_org_add" action="/goform/setStaOrgAdd">
<input type=hidden name=selectedbssid value="">
<table width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr>
    <td class="title" colspan="2">MPDU Aggregation</td>
  </tr>
  <tr>
    <td class="thead">TID</td>
    <td><select name="tid">
	<option value=0 selected>0</option>
	<option value=1>1</option>
	<option value=2>2</option>
	<option value=3>3</option>
	<option value=4>4</option>
	<option value=5>5</option>
	<option value=6>6</option>
	<option value=7>7</option>
      </select></td>
  </tr>
  <tr>
    <td class="thead">BA Window size</td>
    <td><input type=text name="ba_window_size" value=32></td>
  </tr>
</table>
<br />

<table width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr>
    <td class="title" colspan="2">Connected BSSIDs</td>
  </tr>
  <tr>
    <td class="thead">BSSID</td>
  </tr>
  <% getStaConnectedBSSID(); %>
</table>
<table width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr>
    <td>
      <input type=button class=button value="Apply" name="mpdu_apply" onClick="submit_apply()"> &nbsp; &nbsp;
      <input type=button class=button value="Reset" onClick="resetForm();">
    </td>
  </tr>
</table>
</form>
</td></tr></table>
</body></html>
