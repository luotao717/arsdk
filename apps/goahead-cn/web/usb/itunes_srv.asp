<html>
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Expires" content="-1">
<meta http-equiv="Content-Type" content="text/html; charset=<% getCharset(); %>">
<link rel="stylesheet" href="../style/normal_ws.css" type="text/css">
<script language="javascript" src="../js/language_<% getCfgZero(1, "LanguageType"); %>.js"></script>
<script language="javascript" src="../js/common.js"></script>
<script language="javascript">
var dir_count = 0;

function Load_Setting()
{
	var iTunesSrvebl = '<% getCfgZero(1, "iTunesEnable"); %>';
	var iTunesSrvName = '<% getCfgGeneral(1, "iTunesSrvName"); %>';

	if (iTunesSrvebl == "1"){
		document.itunes_form.enabled[0].checked = true;
		var iTunesDir = '<% getCfgZero(1, "iTunesDir"); %>';
		for (var i=0;i<dir_count;i++){
			if (iTunesDir == document.itunes_form.dir_path[i].value){
				document.itunes_form.dir_path[i].checked = true;
				break;
			}
		}
	}
	else{
		document.itunes_form.enabled[1].checked = true;
	}
	if (iTunesSrvName != "")
		document.itunes_form.srv_name.value = iTunesSrvName;
}

function formCheck()
{
	if (document.itunes_form.srv_name.value == ""){
		alert("please provide iTunes Server Name!");
		return false;
	}
	if (dir_count <= 0){
		alert("No any option can be choosed!");
		return false;
	}
	else if (dir_count == 1){
		if (document.itunes_form.dir_path.checked == false){
			alert("please select one option!");
			return false;
		}
		// document.itunes_form.selectDirIndex.value = 0;
	}
	else{
		for(i=0;i<dir_count;i++){
			if (document.itunes_form.dir_path[i].checked == true)
				break;
		}
		if (i == dir_count)	{
			alert("please select one option!");
			return false;
		}
	}
	return true;
}

function resetForm()
{
	location=location; 
}
</script>
</head>
<body onLoad="Load_Setting()" class="mainbody">
<blockquote>
<table><tr><td>
<h1>iTunes Server Setup </h1>
<p></p>
<hr />
<form method=post name=itunes_form action="/goform/iTunesSrv">
<input type="hidden" name="submit-url" value="/usb/itunes_srv.asp">
<table width="600" border="0" cellspacing="1" cellpadding="3">
  <tr> 
    <td class="title" colspan="2">iTunes Server Setup</td>
  </tr>
  <tr> 
    <td class="thead">Capability</td>
    <td><input type="radio" name="enabled" value="1">Enable<input type="radio" name="enabled" value="0">Disable</td>
  </tr>
  <tr> 
    <td class="thead">Server Name</td>
    <td><input type="text" name="srv_name" value="Ralink" maxlength=32></td>
  </tr>
</table>
<br />
<table width="600" border="0" cellspacing="1" cellpadding="3">
  <tr> 
    <td class="title" colspan="3">Media Library Selection</td>
  </tr>
  <tr align=center> 
    <td bgcolor="#C0C0C0">&nbsp;</td>
    <td bgcolor="#C0C0C0">Directory Path</td>
    <td bgcolor="#C0C0C0">Partition</td>
  </tr>
  <% ShowAllDir(); %>
<script language="javascript">
dir_count = parseInt('<% getCount(1, "AllDir"); %>');
</script>
</table>
<table width="600" border="0" cellpadding="2" cellspacing="1">
  <tr>
    <td>
      <input type=submit class="button" value="Apply" onClick="return formCheck()"> &nbsp; &nbsp;
      <input type=button class="button" value="Reset" onClick="resetForm();">
    </td>
  </tr>
</table>
</form>

</td></tr></table>
</body></html>

