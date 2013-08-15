<html>
<head>
<title><% getTitle(); %></title>
<meta http-equiv="Content-Type" content="text/html; charset=<% getCharset(); %>">
<link href="<% getStyle(); %>" rel="stylesheet" type="text/css">
<link rel="shortcut icon" href="<% getFavicon(); %>">
<script type="text/javascript" src="js/language_<% getInfo("language"); %>.js"> </script>
</head>
<body>
<p>&nbsp;</p>
<p>&nbsp;</p>
<form action="/goform/formLogin" method="post" name="Login">
<table align="center"><tr><td>
<table class="login_table" height="250" width="330" border="0" cellspacing="0" cellpadding="0">
  <tr>
    <td class="login_logo" colspan="2">&nbsp;</td>
  </tr>
  <tr>
    <td colspan="2">&nbsp;</td>
  </tr>
  <tr>
    <td class="login_title" colspan="2"><script>dw(MM_adm_login)</script></td>
  </tr>
  <tr>
    <td colspan="2">&nbsp;</td>
  </tr>
  <tr>
    <td width="100" class="login_text"><script>dw(MM_username)</script></td>
    <td><input type="text" name="username" class="login_input" maxlength="30"></td>
  </tr>
  <tr>
    <td width="100" class="login_text"><script>dw(MM_password)</script>&nbsp;&nbsp;</td>
    <td><input type="password" name="password" class="login_input" maxlength="30"></td>
  </tr>
  <tr>
    <td width="100"></td>
    <td><input type="checkbox" name="ch">111</td>
  </tr>
  <tr>
    <td colspan="2" height="15"></td>
  </tr>
  <tr>
    <td colspan="2" align="center"><script>dw('<input type="submit" name="button" class="login_button" value="'+BT_login+'">&nbsp;&nbsp;\
	<input type="reset" id="reset" class="login_button" value="'+BT_reset+'">')</script></td>
  </tr>
  <tr>
    <td colspan="2" height="25"></td>
  </tr>
</table>
</td></tr></table>
<script>document.Login.username.focus();</script>
</form>
</body>
</html>
