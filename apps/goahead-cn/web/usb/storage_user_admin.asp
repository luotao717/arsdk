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
var count = 0;
var user0 = '<% getCfgGeneral(1, "Login"); %>';
var guest = '<% getCfgZero(1, "FtpAnonymous"); %>';
var user1 = '<% getCfgGeneral(1, "User1"); %>';
var upw1 = '<% getCfgGeneral(1, "User1Passwd"); %>';
var ftpuser1 = '<% getCfgZero(1, "FtpUser1"); %>';
var smbuser1 = '<% getCfgZero(1, "SmbUser1"); %>';
var user2 = '<% getCfgGeneral(1, "User2"); %>';
var upw2 = '<% getCfgGeneral(1, "User2Passwd"); %>';
var ftpuser2 = '<% getCfgZero(1, "FtpUser2"); %>';
var smbuser2 = '<% getCfgZero(1, "SmbUser2"); %>';
var user3 = '<% getCfgGeneral(1, "User3"); %>';
var upw3 = '<% getCfgGeneral(1, "User3Passwd"); %>';
var ftpuser3 = '<% getCfgZero(1, "FtpUser3"); %>';
var smbuser3 = '<% getCfgZero(1, "SmbUser3"); %>';
var user4 = '<% getCfgGeneral(1, "User4"); %>';
var upw4 = '<% getCfgGeneral(1, "User4Passwd"); %>';
var ftpuser4 = '<% getCfgZero(1, "FtpUser4"); %>';
var smbuser4 = '<% getCfgZero(1, "SmbUser4"); %>';
var user5 = '<% getCfgGeneral(1, "User5"); %>';
var upw5 = '<% getCfgGeneral(1, "User5Passwd"); %>';
var ftpuser5 = '<% getCfgZero(1, "FtpUser5"); %>';
var smbuser5 = '<% getCfgZero(1, "SmbUser5"); %>';
var user6 = '<% getCfgGeneral(1, "User6"); %>';
var upw6 = '<% getCfgGeneral(1, "User6Passwd"); %>';
var ftpuser6 = '<% getCfgZero(1, "FtpUser6"); %>';
var smbuser6 = '<% getCfgZero(1, "SmbUser6"); %>';
var user7 = '<% getCfgGeneral(1, "User7"); %>';
var upw7 = '<% getCfgGeneral(1, "User7Passwd"); %>';
var ftpuser7 = '<% getCfgZero(1, "FtpUser7"); %>';
var smbuser7 = '<% getCfgZero(1, "SmbUser7"); %>';
var user8 = '<% getCfgGeneral(1, "User8"); %>';
var upw8 = '<% getCfgGeneral(1, "User8Passwd"); %>';
var ftpuser8 = '<% getCfgZero(1, "FtpUser8"); %>';
var smbuser8 = '<% getCfgZero(1, "SmbUser8"); %>';
var user9 = '<% getCfgGeneral(1, "User8"); %>';
var upw9 = '<% getCfgGeneral(1, "User8Passwd"); %>';
var ftpuser9 = '<% getCfgZero(1, "FtpUser8"); %>';
var smbuser9 = '<% getCfgZero(1, "SmbUser8"); %>';
var user10 = '<% getCfgGeneral(1, "User8"); %>';
var upw10 = '<% getCfgGeneral(1, "User8Passwd"); %>';
var ftpuser10 = '<% getCfgZero(1, "FtpUser8"); %>';
var smbuser10 = '<% getCfgZero(1, "SmbUser8"); %>';

var smbb = '<% getSmbBuilt(); %>';

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
	
	for (i=1;i<11;i++){
		if (eval('user'+i) != "")
			count++;
	}
}

function submit_apply(parm)
{
	document.storage_user_adm.hiddenButton.value = parm;
	document.storage_user_adm.submit();
}

function open_useradd_window()
{
	document.storage_user_adm.hiddenUser0.value = user0;
	document.storage_user_adm.hiddenUser1.value = user1;
	document.storage_user_adm.hiddenUser2.value = user2;
	document.storage_user_adm.hiddenUser3.value = user3;
	document.storage_user_adm.hiddenUser4.value = user4;
	document.storage_user_adm.hiddenUser5.value = user5;
	document.storage_user_adm.hiddenUser6.value = user6;
	document.storage_user_adm.hiddenUser7.value = user7;
	document.storage_user_adm.hiddenUser8.value = user8;
	document.storage_user_adm.hiddenUser9.value = user9;
	document.storage_user_adm.hiddenUser10.value = user10;
	
	if (count > 10)
		alert(JS_msg37);
	else
		window.open("storage_user_add.asp","Storage_User_Add","toolbar=no, location=no, scrollbars=yes, resizable=no, width=640, height=440");
}

function open_useredit_window()
{
	var i = 0;
	var index = 0;

	if (count == 0){
		alert(JS_msg38);
	}
	else{
		while (i <= count)	{
			if (count == 1)	{
				if (document.storage_user_adm.storage_user_select.checked == true){
					index = document.storage_user_adm.storage_user_select.value;
					break;
				}
			}
			else if (count > 1){
				if (document.storage_user_adm.storage_user_select[i].checked == true){
					index = document.storage_user_adm.storage_user_select[i].value;
					break;
				}
			}
			i++;
		}
		if (index > 0){
			document.storage_user_adm.hiddenUser0.value = user0;
			document.storage_user_adm.hiddenUser1.value = user1;
			document.storage_user_adm.hiddenUser2.value = user2;
			document.storage_user_adm.hiddenUser3.value = user3;
			document.storage_user_adm.hiddenUser4.value = user4;
			document.storage_user_adm.hiddenUser5.value = user5;
			document.storage_user_adm.hiddenUser6.value = user6;
			document.storage_user_adm.hiddenUser7.value = user7;
			document.storage_user_adm.hiddenUser8.value = user8;
			document.storage_user_adm.selectIndex.value = index;
			document.storage_user_adm.selectUser.value = eval('user'+index);
			document.storage_user_adm.selectPassword.value = eval('upw'+index);
			document.storage_user_adm.selectFtp.value = eval('ftpuser'+index);
			document.storage_user_adm.selectSmb.value = eval('smbuser'+index);
			
			window.open("storage_user_edit.asp","Storage_User_Edit","toolbar=no, location=no, scrollbars=yes, resizable=no, width=640, height=440");
		}
		else{
			alert(JS_msg39);
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
<tr><td class="title"><script>dw(MM_user_settings)</script></td></tr>
<tr><td><script>dw(JS_msg_user)</script></td></tr>
<tr><td><hr></td></tr>
</table>
<form method=post name=storage_user_adm action="/goform/storageAdm">
<input type="hidden" name="submit-url" value="/usb/storage_user_admin.asp">
<input type=hidden name=hiddenButton value="">
<input type=hidden name=hiddenUser0 value="">
<input type=hidden name=hiddenUser1 value="">
<input type=hidden name=hiddenUser2 value="">
<input type=hidden name=hiddenUser3 value="">
<input type=hidden name=hiddenUser4 value="">
<input type=hidden name=hiddenUser5 value="">
<input type=hidden name=hiddenUser6 value="">
<input type=hidden name=hiddenUser7 value="">
<input type=hidden name=hiddenUser8 value="">
<input type=hidden name=hiddenUser9 value="">
<input type=hidden name=hiddenUser10 value="">
<input type=hidden name=selectIndex value="">
<input type=hidden name=selectUser value="">
<input type=hidden name=selectPassword value="">
<input type=hidden name=selectFtp value="">
<input type=hidden name=selectSmb value="">
<table width=100% border=1 cellpadding=3 style="border-collapse: collapse" bordercolor="#C8C8C8">
  <tr bgcolor=#f2f2f2 align="center">
    <td><b>No.</b></td>
    <td><b><script>dw(MM_username)</script></b></td>
    <td><b><script>dw(MM_allow_to_use_ftp)</script></b></td>
    <td id="div_smb_1" style="display:none"><b><script>dw(MM_allow_to_use_smb)</script></b></td>
  </tr>
<script>
document.write("<tr align=center><td>"+"--"+"</td><td>"+user0+"</td><td>");
document.write(MM_enable+"</td><td id='div_smb_0' style='display:none'>");
document.write(MM_enable+"</td><tr>");

if (guest == "1")
	document.write("<tr align=center><td>"+"--"+"</td><td>"+"anonymous"+"</td><td>"+MM_enable+"</td><td id='div_smb_2' style='display:none'>"+MM_disable+"</td><tr>");
else
	document.write("<tr align=center><td>"+"--"+"</td><td>"+"anonymous"+"</td><td>"+MM_disable+"</td><td id='div_smb_2' style='display:none'>"+MM_disable+"</td><tr>");

for (var i=1;i<11;i++)
{
	if (eval("user"+i) != "")
	{
		var items = "<tr align=center><td><input type=radio name=storage_user_select value="+i+">";
		items += "</td><td>";
		items += eval("user"+i);
		items += "</td><td>";
		if (eval("ftpuser"+i) == "1")
		{
			items += MM_enable;
			items += "</td><td>";
		}
		else
		{
			items += MM_disable;
			items += "</td><td>";
		}
		if (smbb == "1")
		{
			if (eval("smbuser"+i) == "1")
				items += MM_enable;	
			else
				items += MM_disable;
		}
		
		items += "</td><tr>";		
		document.write(items);	
	}
}

if (smbb == "1")
{
	document.getElementById("div_smb_0").style.display = "";
	document.getElementById("div_smb_1").style.display = "";
	document.getElementById("div_smb_2").style.display = "";
}
</script>
</table>

<br>
<table width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr>
    <td>
      <script>dw(' <input type="button" class="button" value='+BT_add+' onClick="open_useradd_window()">&nbsp;&nbsp;\
      <input type="button" class="button" value="'+BT_delete+'" onClick=submit_apply(\"delete\")>')</script>
    </td>
  </tr>
</table>
</form>

</td></tr></table>
</blockquote>
</body></html>
