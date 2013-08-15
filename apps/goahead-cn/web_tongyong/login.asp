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
<table height="96" width="600" border="0" cellspacing="0" cellpadding="0">
  <tr>
    <td class="top_left">&nbsp;</td>
    <td class="top_center">&nbsp;</td>
    <td class="top_right" align="right">&nbsp;</td>
  </tr>
</table>

<table height="44" width="600" border="0" cellspacing="0" cellpadding="0">
  <tr>
    <td width="6"></td>
    <td width="588" class="login_table"><table width="100%" border="0" cellspacing="0" cellpadding="0">
      <tr>
        <td class="title_adm_left"><script>dw(MM_adm_login)</script></td>
        <td class="title_adm_right">&nbsp;</td>
      </tr>
    </table></td>
    <td width="6"></td>
  </tr>
</table>

<table height="160" width="600" border="0" cellspacing="0" cellpadding="0">
  <tr>
    <td width="6"></td>
    <td width="588" class="login_table"><table width="100%" border="0" cellspacing="5" cellpadding="0">
      <tr>
        <td colspan="3">&nbsp;</td>
      </tr>
      <tr>
        <td width="140">&nbsp;</td>
        <td width="100" class="login_text"><script>dw(MM_username)</script></td>
        <td><input type="text" name="username" class="login_input" maxlength="30"></td>
      </tr>
      <tr>
        <td>&nbsp;</td>
        <td class="login_text"><script>dw(MM_password)</script></td>
        <td><input type="password" name="password" class="login_input" maxlength="30"></td>
      </tr>
      <tr>
        <td colspan="3" height="15"></td>
      </tr>
      <tr>
        <td colspan="3" align="center"><script>dw('<input type="submit" name="button" class="login_button" value="'+BT_login+'">&nbsp;&nbsp;\<input type="reset" id="reset" class="login_button" value="'+BT_reset+'">')</script></td>
      </tr>
      <tr>
        <td colspan="3" height="15"></td>
      </tr>
    </table></td>
    <td width="6"></td>
  </tr>
</table>

<table height="41" width="600" border="0" cellspacing="0" cellpadding="0">
  <tr>
  	<td class="bottom_left">&nbsp;</td>
	<td class="bottom_center1">&nbsp;</td>
	<td class="bottom_center">&nbsp;</td>
	<td class="bottom_center1">&nbsp;</td>
	<td class="bottom_right">&nbsp;</td>
  </tr>
</table>
</td></tr></table>
<script>document.Login.username.focus();</script>
</form>
</body>
</html>
