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
	var mediasrvebl = '<% getCfgZero(1, "mediaSrvEnabled"); %>';

	document.storage_media.media_enabled[1].checked = true;
	document.storage_media.media_name.value == '<% getCfgGeneral(1, "mediaSrvName"); %>';
	document.storage_media.media_dir_add.disabled = true;
	document.storage_media.media_dir_del.disabled = true;

	if (mediasrvebl == "1"){
		document.storage_media.media_enabled[0].checked = true;
		document.storage_media.media_dir_add.disabled = false;
		document.storage_media.media_dir_del.disabled = false;
	}
}

function formCheck()
{
	if (document.storage_media.media_name.value == ""){
		alert('Please specify Media Server Name');
		document.storage_media.media_name.focus();
		return false;
	}
	else if (document.storage_media.media_name.value.match(/[ `~!@#$%^&*\()+\|{}\[\]:;\"\'<,>.\/\\?]/)){
		alert('Don\'t enter /[ `~!@#$%^&*\()+\|{}\[\]:;\"\'<,>.\/\\?]/ in this feild');
		document.storage_media.media_name.focus();
		return false;
	}
	return true;
}

function CheckSelect()
{
	if (dir_count <= 0){
		alert("No any option can be choosed!");
		return false;
	}
	else if (dir_count == 1){
		if (document.storage_media.media_dir.checked == false)	{
			alert("please select one option!");
			return false;
		}
	}
	else{
		for(i=0;i<dir_count;i++){
			if (document.storage_media.media_dir[i].checked == true){
				break;
			}
		}
		if (i == dir_count){
			alert("please select one option!");
			return false;
		}
	}
	return true;
}

function open_media_diradd_window()
{
	window.open("media_adddir.asp","Media_Dir_Add","toolbar=no, location=no, scrollbars=yes, resizable=no, width=640, height=440")
}

function media_enable_switch()
{
	if (document.storage_media.media_enabled[1].checked == true){
		document.storage_media.media_dir_add.disabled = true;
		document.storage_media.media_dir_del.disabled = true;
	}
	else{
		document.storage_media.media_dir_add.disabled = false;
		document.storage_media.media_dir_del.disabled = false;
	}
}

function submit_apply(parm)
{
	if (parm == "delete"){
		if (CheckSelect()){
			document.storage_media.hiddenButton.value = parm;
			document.storage_media.submit();
		}
	}
	else if (parm == "apply"){
		if (formCheck()){
			document.storage_media.hiddenButton.value = parm;
			document.storage_media.submit();
		}
	}
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
<h1>Media Server Setup </h1>
<p></p>
<hr />
<form method=post name=storage_media action="/goform/storageMediaSrv">
<input type="hidden" name="submit-url" value="/usb/media_srv.asp">
<input type=hidden name=hiddenButton value="">
<table width="600" border="0" cellspacing="1" cellpadding="3">
  <tr> 
    <td class="title" colspan="2">Media Server Setup</td>
  </tr>
  <tr> 
    <td class="thead">Media Server</td>
    <td><input type=radio name=media_enabled value="1" onClick="media_enable_switch()">Enable<input type=radio name=media_enabled value="0" onClick="media_enable_switch()" checked>Disable</td>
  </tr>
  <tr>
    <td class="thead">Media Server Name</td>
    <td><input type=text name=media_name maxlength=16 value="RalinkSoC"></td>
  </tr>
</table>
<br>
<table width="600" border="0" cellspacing="1" cellpadding="3">
  <tr>
    <td class="title" colspan="2">Media Server Shared Directory</td>
  </tr>
  <tr align="center">
    <td bgcolor="#C0C0C0">&nbsp;</td>
    <td bgcolor="#C0C0C0">Path</td>
  </tr>
  <% ShowMediaDir(); %>
<script language="javascript">dir_count = parseInt('<% getCount(1, "AllMediaDir"); %>');</script>
</table>
<table width="600" border="0" cellpadding="2" cellspacing="1">
  <tr>
    <td>
      <input type=button class="button" value="Add" name="media_dir_add" onClick="open_media_diradd_window()"> &nbsp; &nbsp;
      <input type=button class="button" value="Delete" name="media_dir_del" onClick="submit_apply('delete')">
    </td>
  </tr>
</table>
<br />
<table width="600" border="0" cellpadding="2" cellspacing="1">
  <tr>
    <td>
      <input type=button class="button" value="Apply" onClick="submit_apply('apply')"> &nbsp; &nbsp;
      <input type=button class="button" value="Reset" onClick="resetForm();">
    </td>
  </tr>
</table>
</form>

</td></tr></table>
</body></html>
