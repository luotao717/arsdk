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
var smbenabled = '<% getCfgZero(1, "SmbEnabled"); %>';
var smbwg = '<% getCfgGeneral(1, "HostName"); %>';
var smbnetbios = '<% getCfgGeneral(1, "SmbNetBIOS"); %>';
var dir_count = 0;

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
	
	document.storage_smb.smb_workgroup.disabled = true;
	document.storage_smb.smb_netbios.disabled = true;

	if (smbenabled == "1"){
		document.storage_smb.smb_enabled[0].checked = true;
		document.storage_smb.smb_workgroup.disabled = false;
		document.storage_smb.smb_workgroup.value = smbwg;
		document.storage_smb.smb_netbios.disabled = false;
		document.storage_smb.smb_netbios.value = smbnetbios;
	}
	else{
		document.storage_smb.smb_enabled[1].checked = true;
	}
}

function checkSelect()
{
	if (dir_count <= 0){
		alert(JS_msg34);
		return false;
	}
	else if (dir_count == 1){
		if (document.storage_smb.smb_dir.checked == false)	{
			alert(JS_msg35);
			return false;
		}
		document.storage_smb.selectIndex.value = 0;
		document.storage_smb.selectDir.value = document.storage_smb.smb_dir.value;
		document.storage_smb.selectPermit.value = document.storage_smb.smb_dir_permit.value;
		document.storage_smb.selectPath.value = document.storage_smb.smb_dir_path.value;
	}
	else{
		for(i=0;i<dir_count;i++){
			if (document.storage_smb.smb_dir[i].checked == true){
				document.storage_smb.selectIndex.value = i;
				document.storage_smb.selectDir.value = document.storage_smb.smb_dir[i].value;
				document.storage_smb.selectPermit.value = document.storage_smb.smb_dir_permit[i].value;
				document.storage_smb.selectPath.value = document.storage_smb.smb_dir_path[i].value;
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
	if (document.storage_smb.smb_enabled[0].checked == true){
		if (!isBlankMsg(document.storage_smb.smb_workgroup.value, MM_workgroup)) 
			return false;
				
		if (!isBlankMsg(document.storage_smb.smb_netbios.value, MM_netbios)) 
			return false;
	}
	
	return true;
}

function smb_enable_switch()
{
	if (document.storage_smb.smb_enabled[1].checked == true){
		document.storage_smb.smb_workgroup.disabled = true;
		document.storage_smb.smb_netbios.disabled = true;
	}
	else{
		document.storage_smb.smb_workgroup.disabled = false;
		document.storage_smb.smb_netbios.disabled = false;
	}
}

function open_diradd_window()
{
	document.storage_smb.hidden_user1.value = '<% getCfgGeneral(1, "User1"); %>';
	document.storage_smb.hidden_user2.value = '<% getCfgGeneral(1, "User2"); %>';
	document.storage_smb.hidden_user3.value = '<% getCfgGeneral(1, "User3"); %>';
	document.storage_smb.hidden_user4.value = '<% getCfgGeneral(1, "User4"); %>';
	document.storage_smb.hidden_user5.value = '<% getCfgGeneral(1, "User5"); %>';
	document.storage_smb.hidden_user6.value = '<% getCfgGeneral(1, "User6"); %>';
	document.storage_smb.hidden_user7.value = '<% getCfgGeneral(1, "User7"); %>';
	document.storage_smb.hidden_user8.value = '<% getCfgGeneral(1, "User8"); %>';
	
	window.open("smb_adddir.asp","Samba_Dir_Add","toolbar=no, location=no, scrollbars=yes, resizable=no, width=640, height=440");
}

function open_diredit_window()
{
	if (checkSelect()){
		document.storage_smb.hidden_user1.value = '<% getCfgGeneral(1, "User1"); %>';
		document.storage_smb.hidden_user2.value = '<% getCfgGeneral(1, "User2"); %>';
		document.storage_smb.hidden_user3.value = '<% getCfgGeneral(1, "User3"); %>';
		document.storage_smb.hidden_user4.value = '<% getCfgGeneral(1, "User4"); %>';
		document.storage_smb.hidden_user5.value = '<% getCfgGeneral(1, "User5"); %>';
		document.storage_smb.hidden_user6.value = '<% getCfgGeneral(1, "User6"); %>';
		document.storage_smb.hidden_user7.value = '<% getCfgGeneral(1, "User7"); %>';
		document.storage_smb.hidden_user8.value = '<% getCfgGeneral(1, "User8"); %>';

		window.open("smb_editdir.asp","Samba_Dir_Edit","toolbar=no, location=no, scrollbars=yes, resizable=no, width=640, height=440");
	}
}

function submit_apply(parm)
{
	if (parm == "delete"){
		if (checkSelect()){
			document.storage_smb.hiddenButton.value = parm;
			document.storage_smb.submit();
		}
	}
	else if (parm == "apply"){
		if (formCheck()){
			document.storage_smb.hiddenButton.value = parm;
			document.storage_smb.submit();
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
<table id="div_no_usbdivice" style="display:none" width=770><tr><td>
<table width=100% border=0 cellpadding=3 cellspacing=1> 
<tr><td><img src="../graphics/warning.gif" align="absmiddle">&nbsp;&nbsp;
<script>dw(JS_msg_usb)</script>&nbsp;&nbsp;
<script>dw('<input type=button class=button value="'+BT_refresh+'" onClick="window.location.reload()">')</script></td></tr>
</table>
</td></tr></table>

<table id="div_usbdivice" width=770><tr><td>
<table width=100% border=0 cellpadding=3 cellspacing=1> 
<tr><td class="title"><script>dw(MM_smb_settings)</script></td></tr>
<tr><td><script>dw(JS_msg_samba)</script></td></tr>
<tr><td><hr></td></tr>
</table>
<form method=post name=storage_smb action="/goform/storageSmbSrv">
<input type="hidden" name="submit-url" value="/usb/smb_srv.asp">
<input type=hidden name=selectIndex value="">
<input type=hidden name=selectDir value="">
<input type=hidden name=selectPermit value="">
<input type=hidden name=selectPath value="">
<input type=hidden name=hiddenButton value="">
<input type=hidden name=hidden_user1 value="">
<input type=hidden name=hidden_user2 value="">
<input type=hidden name=hidden_user3 value="">
<input type=hidden name=hidden_user4 value="">
<input type=hidden name=hidden_user5 value="">
<input type=hidden name=hidden_user6 value="">
<input type=hidden name=hidden_user7 value="">
<input type=hidden name=hidden_user8 value="">
<table width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr> 
    <td class="thead">Samba <script>dw(MM_function)</script>:</td>
    <td><input type=radio name=smb_enabled value="1" onClick="smb_enable_switch()"><script>dw(MM_enable)</script>
    <input type=radio name=smb_enabled value="0" onClick="smb_enable_switch()" checked><script>dw(MM_disable)</script></td>
  </tr>
  <tr>
    <td class="thead"><script>dw(MM_workgroup)</script>:</td>
    <td><input type=text name=smb_workgroup maxlength=16 value="SambaServer"></td>
  </tr>
  <tr>
    <td class="thead"><script>dw(MM_netbios)</script>:</td>
    <td><input type=text name=smb_netbios maxlength=16 value="Netbios"></td>
  </tr>
</table>

<br>
<table width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr>
    <td>
      <script>dw('<input type=button class=button value="'+BT_apply+'" onClick=submit_apply(\"apply\")> &nbsp; &nbsp;\
      <input type=button class=button value="'+BT_reset+'" onClick="resetForm();">')</script>
    </td>
  </tr>
</table>

<br>
<p><b><script>dw(MM_sharing_dir_list)</script></b></p>
<table width=100% border=1 cellpadding=3 style="border-collapse: collapse" bordercolor="#C8C8C8">
  <tr bgcolor=#f2f2f2 align="center">
    <td></td>
    <td><b><script>dw(MM_dir_name)</script></b></td>
    <td><b><script>dw(MM_dir_path)</script></b></td>
    <td><b><script>dw(MM_allows_users)</script></b></td>
  </tr>
  <% ShowSmbDir(); %>
<script language="javascript">
dir_count = parseInt('<% getCount(1, "AllSmbDir"); %>');
</script>
</table>

<br>
<table width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr>
    <td>
      <script>dw('<input type=button class="button" value="'+BT_add+'" onClick="open_diradd_window()"> &nbsp; &nbsp;\
      <input type=button class="button" value="'+BT_delete+'" onClick=submit_apply(\"delete\")>')</script>
    </td>
  </tr>
</table>
</form>

</td></tr></table>
</body></html>
