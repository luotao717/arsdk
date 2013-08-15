<html>
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Expires" content="-1">
<meta http-equiv="Content-Type" content="text/html; charset=<% getCharset(); %>">
<link rel="stylesheet" href="../style/normal_ws.css" type="text/css">
<script language="javascript" src="../js/language_<% getCfgZero(1, "LanguageType"); %>.js"></script>
<script language="javascript" src="../js/common.js"></script>
<script language="javascript">
var usb_state = 1*"<% getIndex(1, "usbStatus"); %>";
var dir_count = 0;
var part_count = 0;
var Waiting = false;

function Load_Setting()
{
	if (usb_state == 0){
		document.getElementById("div_no_usbdivice").style.display = "";
		document.getElementById("div_usbdivice").style.display = "none";
	}
	else{
		document.getElementById("div_no_usbdivice").style.display = "none";
		document.getElementById("div_usbdivice").style.display = "";
	}
	
	if (Waiting)
		document.getElementById("waitTable").style.display = "";
	else
		document.getElementById("waitTable").style.display = "none";
}

function checkSelect()
{
	if (dir_count <= 0){
		alert(JS_msg34);
		return false;
	}
	else if (dir_count == 1){
		if (document.storage_disk_adm.dir_path.checked == false){
			alert(JS_msg35);
			return false;
		}
		document.storage_disk_adm.selectDirIndex.value = 0;
	}
	else{
		for(i=0;i<dir_count;i++){
			if (document.storage_disk_adm.dir_path[i].checked == true)	{
				document.storage_disk_adm.selectDirIndex.value = i;
				break;
			}
		}
		if (i == dir_count){
			alert(JS_msg35);
			return false;
		}
	}
	
	return true;
}

function formCheck()
{
	if (part_count <= 0){
		alert(JS_msg41);
		return false;
	}
	else if (part_count == 1){
		if (document.storage_disk_adm.disk_part.checked == false){
			alert(JS_msg35);
			return false;
		}
		document.storage_disk_adm.selectPartIndex.value = 0;
	}
	else if (part_count > 1){
		for(i=0;i<part_count;i++){
			if (document.storage_disk_adm.disk_part[i].checked == true){
				document.storage_disk_adm.selectPartIndex.value = i;
				break;
			}
		}
		if (i == part_count){
			alert(JS_msg35);
			return false;
		}
	}
	return true;
}

function submit_apply(parm)
{
	if (parm == "delete"){
		if (!checkSelect())	{
			window.location.reload();
			return;
		}
		document.storage_disk_adm.hiddenButton.value = parm;
		document.storage_disk_adm.submit();
	}
	else if (parm == "format"){
		if (formCheck()){
			var format_ok = confirm(JS_msg42);
			if (format_ok == true){
				Waiting = true;
				Load_Setting();
				document.storage_disk_adm.hiddenButton.value = parm;
				document.storage_disk_adm.submit();
			}
		}
		return;
	}
	else if (parm == "remove"){
		var remove_ok = confirm(JS_msg43);
		if (remove_ok == true){
			document.storage_disk_adm.hiddenButton.value = parm;
			document.storage_disk_adm.submit();
		}
		return;
	}
}

function open_diskadd_window()
{
	window.open("storage_disk_adddir.asp","storage_disk_add","toolbar=no, location=no, scrollbars=yes, resizable=no, width=640, height=440");
}

function open_diskrepart_window()
{
	Waiting = true;
	window.open("storage_disk_part.asp","storage_disk_part","toolbar=no, location=no, scrollbars=yes, resizable=no, width=640, height=440");
}
</script>
</head>
<body onLoad="Load_Setting()" class="mainbody">
<blockquote>
<table id="div_no_usbdivice" style="display:none" width=770><tr><td>
<table width=100% border=0 cellpadding=3 cellspacing=1> 
<tr><td><img src="../graphics/warning.gif" align="absmiddle">&nbsp;&nbsp;
<script>dw(JS_msg_usb)</script>&nbsp;&nbsp;
<script>dw('<input type=button class=button value="'+BT_refresh+'" onClick="window.location.reload()">')</script></td></tr>
</table>
</td></tr></table>

<table id="div_usbdivice" width=770><tr><td>
<table width=100% border=0 cellpadding=3 cellspacing=1> 
<tr><td class="title"><script>dw(MM_disk_settings)</script></td></tr>
<tr><td><script>dw(JS_msg_disk)</script></td></tr>
<tr><td><hr></td></tr>
</table>

<form method=post name=storage_disk_adm action="/goform/storageDiskAdm">
<input type="hidden" name="submit-url" value="/usb/storage_disk_admin.asp">
<input type=hidden name=hiddenButton value="">
<input type=hidden name=selectDirIndex value="">
<input type=hidden name=selectPartIndex value="">
<table width=100% border=1 cellpadding=3 style="border-collapse: collapse" bordercolor="#C8C8C8">
  <tr bgcolor=#f2f2f2 align="center">
    <td></td>
    <td><b><script>dw(MM_dir_path)</script></b></td>
    <td><b><script>dw(MM_partition)</script></b></td>
  </tr>
  <% ShowAllDir(); %>
<script language="javascript">
dir_count = parseInt('<% getCount(1, "AllDir"); %>');
</script>
</table>

<br>
<table width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr>
    <td>
      <script>dw('<input type=button class=button value="'+BT_add+'" onClick="open_diskadd_window()"> &nbsp; &nbsp;\
	  <input type=button class=button value="'+BT_delete+'" onClick=submit_apply(\"delete\")> &nbsp; &nbsp;\
      <input type=button class=button value="'+BT_remove+'" onClick=submit_apply(\"remove\")>')</script>
    </td>
  </tr>
</table>

<!--
<br>
<table width=100% border=1 cellpadding=3 style="border-collapse: collapse" bordercolor="#C8C8C8">
  <tr bgcolor=#f2f2f2 align="center">
    <td><b>No.</b></td>
    <td><b><script>dw(MM_partition)</script></b></td>
    <td><b><script>dw(MM_path)</script></b></td>
  </tr>
  <% ShowPartition(); %>
<script language="javascript">
part_count = parseInt('<% getCount(1, "AllPart"); %>');
</script>
</table>

<br>
<table width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr>
    <td>
      <script>dw('<input type="button" class="button" value="Format" onClick=submit_apply(\"format\")> &nbsp; &nbsp;\
      <input type="button" class="button" value="Re-allocate" name="realloc_parted" onClick="open_diskrepart_window()">')</script>
    </td>
  </tr>
</table>
-->
</form>

<br><br><br>
<center>
<table id="waitTable" width=100% border=0 cellpadding=3 cellspacing=1>
  <tr>
    <td align"certer"><h2><script>dw(JS_msg44)</script></h2></td>
  </tr>
  <tr>
    <td align="center"><img src="../graphics/ajax-loader.gif" border="0"></td>
  </tr>
</table>
</center>

</td></tr></table>
</blockquote>
</body></html>
