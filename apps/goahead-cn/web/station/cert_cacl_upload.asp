<html>
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Expires" content="-1">
<meta http-equiv="Content-Type" content="text/html; charset=<% getCharset(); %>">
<link rel="stylesheet" href="../style/normal_ws.css" type="text/css">
<script language="javascript" src="../js/language_<% getCfgZero(1, "LanguageType"); %>.js"></script>
<script language="javascript" src="../js/common.js"></script>
<script language="javascript">
function submit_apply()
{
	document.cert_ca_client_upload.submit();
	opener.ClientCertUpdate();
	window.close();
}
</script>
</head>
<body>
<table><tr><td>
<form method="post" name="cert_ca_client_upload" action="/cgi-bin/upload_caclcert.cgi" enctype="multipart/form-data">
<table width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr>
    <td class="title" colspan="2">Upload Certificate</td>
  </tr>
  <tr>
    <td class="thead">Client Certificate</td>
    <td><input type="file" name="ca_client_cert_file" maxlength="256"></td>
  </tr>
</table>
<table width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr align="center">
    <td >
      <input type="button" class=button value="Apply" onClick="submit_apply()">
      <input type="reset" class=button value="Reset">
    </td>
  </tr>
</table>
</form>
</td></tr></table>
</body></html>
