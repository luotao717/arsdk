<html>
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Expires" content="-1">
<meta http-equiv="Content-Type" content="text/html; charset=<% getCharset(); %>">
<link rel="stylesheet" href="../style/normal_ws.css" type="text/css">
<script language="javascript" src="../js/language_<% getCfgZero(1, "LanguageType"); %>.js"></script>
<script language="javascript" src="../js/common.js"></script>
<script language="javascript">
</script>
</head>
<body class="mainbody">
<blockquote>
<table width=700><tr><td>
<table width=100% border=0 cellpadding=3 cellspacing=1> 
<tr><td class="title"><script>dw(MM_admin_settings)</script></td></tr>
<tr><td><script>dw(JS_msg_password)</script></td></tr>
<tr><td><hr></td></tr>
</table>

<form method="post" name="Adm" action="/goform/setSysAdm">
<input type="hidden" name="submit-url" value="/adm/password.asp">
<table width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr>
    <td class="thead"><script>dw(MM_username)</script>:</td>
    <td><input type="text" name="admuser" maxlength="16" value="<% getCfgGeneral(1, "Login"); %>"></td>
  </tr>
 </table>
<br>
<table width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr>
    <td>
      
    </td>
  </tr>
</table>
</form>

</td></tr></table>
</blockquote>
</body></html>
