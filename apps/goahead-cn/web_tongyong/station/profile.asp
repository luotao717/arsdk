<html>
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Expires" content="-1">
<meta http-equiv="Content-Type" content="text/html; charset=<% getCharset(); %>">
<link rel="stylesheet" href="../style/normal_ws.css" type="text/css">
<script language="javascript" src="../js/language_<% getCfgZero(1, "LanguageType"); %>.js"></script>
<script language="javascript" src="../js/common.js"></script>
<script language="javascript">
function open_profile_page() 
{
	window.open("add_profile_page.asp","profile_page","toolbar=no, location=yes, scrollbars=yes, resizable=no, width=660, height=600");
}

function edit_profile_page()
{
	document.sta_profile.hiddenButton.value = 'edit';
	document.sta_profile.submit();
	window.open("edit_profile_page.asp","profile_page","toolbar=no, location=yes, scrollbars=yes, resizable=no, width=660, height=600");
}

function selectedProfileChange()
{	
	document.sta_profile.deleteProfileButton.disabled=false;
	document.sta_profile.editProfileButton.disabled=false;
	document.sta_profile.activateProfileButton.disabled=false;
}

function submit_apply(parm)
{
	document.sta_profile.hiddenButton.value = parm;
	document.sta_profile.submit();
}
</script>
</head>
<body>
<table><tr><td>
<h2>Station Profile</h2>
<p>The Status page shows the settings and current operation status of the Station.</p>
<hr />
<form method="post" name="sta_profile" action="/goform/setStaProfile">
<table width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr> 
    <td class="title" colspan="7">Pofile List</td>
  </tr>
  <tr>
    <td bgcolor="#C0C0C0">&nbsp;</td>
    <td bgcolor="#C0C0C0">Profile</td>
    <td bgcolor="#C0C0C0">SSID</td>
    <td bgcolor="#C0C0C0">Channel</td>
    <td bgcolor="#C0C0C0">Authentication</td>
    <td bgcolor="#C0C0C0">Encryption</td>
    <td bgcolor="#C0C0C0">Network Type</td>
  </tr>
  <% getStaProfile(); %>
</table>
<br />
<center><h4>Note: At present, STA only guarantees to store Two profiles!</h4></center>

<table width=100% border=0 cellpadding=3 cellspacing=1> 
<tr align="center">
  <td>
    <input type="button" name="addProfileButton" class=button value="Add" onClick="open_profile_page()"> &nbsp; &nbsp;
    <input type="button" name="deleteProfileButton" class=button value="Delete" disabled onClick="submit_apply('delete')"> &nbsp; &nbsp;
    <input type="button" name="editProfileButton" class=button value="Edit" disabled onClick="edit_profile_page()"> &nbsp; &nbsp;
    <input type="button" name="activateProfileButton" class=button value="Activate" disabled onClick="submit_apply('activate')"> &nbsp; &nbsp;
  </td>
</tr>
</table>
<input type=hidden name=hiddenButton value="">
</form>
</td></tr></table>
</body></html>
