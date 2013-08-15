<html>
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Expires" content="-1">
<meta http-equiv="Content-Type" content="text/html; charset=<% getCharset(); %>">
<link rel="stylesheet" href="../style/normal_ws.css" type="text/css">
<script language="javascript" src="../js/language_<% getCfgZero(1, "LanguageType"); %>.js"></script>
<script language="javascript" src="../js/common.js"></script>
<script language="javascript">
var path_count = 0;

function formCheck()
{
	if (!isAscciMsg(document.smb_adddir.dir_name.value, MM_dir_name)) 
		return false;

	if (path_count <= 0){
		alert(JS_msg36);
		return false;
	}
	else if (path_count == 1){
		if (document.smb_adddir.dir_path.checked == false){
			alert(JS_msg35);
			return false;
		}
	}
	else if (path_count > 1){
		for(i=0;i<path_count;i++){
			if (document.smb_adddir.dir_path[i].checked == true)
				break;
		}
		
		if (i == path_count){
			alert(JS_msg35);
			return false;
		}
	}
	
	return true;
}

function submit_apply()
{
	if (formCheck() == true){
		document.smb_adddir.submit();
		opener.location.reload();
		window.close();
	}
}

function addDirClose()
{
	opener.location.reload();
}
</script>
</head>
<body onUnload="addDirClose()">
<table width=600><tr><td>
<table width=100% border=0 cellpadding=3 cellspacing=1> 
<tr><td class="title"><script>dw(MM_add_smb_settings)</script></td></tr>
<tr><td><script>dw(JS_msg_samba)</script></td></tr>
<tr><td><hr></td></tr>
</table>

<form method=post name="smb_adddir" action="/goform/SmbDirAdd">
<input type="hidden" name="submit-url" value="/usb/smb_srv.asp">
<table width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr> 
    <td class="thead"><script>dw(MM_dir_name)</script>:</td>
    <td><input type=text name=dir_name maxlength=16 value=""></td>
  </tr>
  <tr>
    <td class="thead"><script>dw(MM_access_user)</script>:</td>
    <td><script language="javascript">
    for (i=1;i<=8;i++)
    {
      	var user = eval('opener.document.forms[0].hidden_user'+i+'.value');

      	document.write("&nbsp;");
      	if (user != "")
      	{
			document.write("<input type=\"checkbox\" name=\"allow_user\" value=\""+user+"\">");
			document.write(user);
			document.write("<br />");
      	}
    }
    </script></td>
  </tr>
</table>

<br>
<p><b><script>dw(MM_access_path)</script></b></p>
<table width=100% border=1 cellpadding=3 style="border-collapse: collapse" bordercolor="#C8C8C8">
  <tr bgcolor=#f2f2f2 align="center">
    <td></td>
    <td><b><script>dw(MM_dir_path)</script></b></td>
    <td><b><script>dw(MM_partition)</script></b></td>
  </tr>
  <% ShowAllDir(); %>
<script language="javascript">
path_count = parseInt('<% getCount(1, "AllDir"); %>');
</script>
</table>

<br>
<table width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr>
    <td>
      <script>dw('<input type=button class=button value="'+BT_apply+'" onClick="submit_apply()"> &nbsp; &nbsp;\
      <input type=button class=button value="'+BT_close+'" onClick="window.close()">')</script>
    </td>
  </tr>
</table>
</form>

</td></tr></table>
</body></html>
