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
	if (path_count <= 0){
		alert("No Directory");
		return false;
	}
	else if (path_count == 1){
		if (document.media_adddir.dir_path.checked == false){
			alert("Please choose one option");
			return false;
		}
	}
	else if (path_count > 1){
		for(i=0;i<path_count;i++){
			if (document.media_adddir.dir_path[i].checked == true)
				break;
		}
		if (i == path_count){
			alert("Please choose one option");
			return false;
		}
	}

	return true;
}

function submit_apply()
{
	if (formCheck() == true){
		document.media_adddir.submit();
		opener.location.reload();
		window.close();
	}
}
</script>
</head>
<body onUnload="opener.location.reload()">
<table><tr><td>
<form method=post name="media_adddir" action="/goform/MediaDirAdd">
<input type="hidden" name="submit-url" value="/usb/media_adddir.asp">
<table width="600" border="0" cellspacing="1" cellpadding="3">
  <tr> 
    <td class="title" colspan="3">Access Path</td>
  </tr>
  <tr align="center"> 
    <td bgcolor="#C0C0C0">&nbsp;</td>
    <td bgcolor="#C0C0C0">Path</td>
    <td bgcolor="#C0C0C0">Partition</td>
  </tr>
  <% ShowAllDir(); %>
<script language="javascript">
path_count = parseInt('<% getCount(1, "AllDir"); %>');
</script>
</table>
<table width="600" border="0" cellpadding="2" cellspacing="1">
  <tr>
    <td>
      <input type=button class="button" value="Apply" onClick="submit_apply()"> &nbsp; &nbsp;
      <input type=button class="button" value="Close" onClick="window.close()">
    </td>
  </tr>
</table>
</form>

</td></tr></table>
</body></html>
