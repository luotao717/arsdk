<html>
<head>
<title>WR922</title>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312" />
<link href="images/kingnet.css" type="text/css" rel="stylesheet">
<script language="JavaScript" type="text/javascript">
function AdmFormCheck()
{
	if (document.Adm.admuser.value == "") {
		alert("请输入管理员账号.");
		return false;
	}
	if (document.Adm.admpass.value == "") {
		alert("请输入管理员密码.");
		return false;
	}
	if (document.Adm.admpass1.value == "") {
		alert("请再次确认管理员密码.");
		return false;
	}
		
	if(document.Adm.admpass.value != document.Adm.admpass1.value) {
		alert("密码不匹配!");
		return false;
	}
	return true;
}
</script>
</head>
<body>
<form method="post" name="Adm" action="/goform/setSysAdm">
<table width="90%" border="0" cellpadding="0" cellspacing="0" align="center">
  <tr>
    <td style="padding-top:30px;">
	   <table width="100%" border="0" cellpadding="0" cellspacing="0">
          <tr>
            <td class="nav">当前路径:KN-WR922 &gt;&gt;系统管理 &gt;&gt;密码管理</td>
          </tr>
		  <tr>
		    <td>&nbsp;</td>
		  </tr>
          <tr>
            <td class="space" style="padding-left:10px; padding-right:10px;">&nbsp;&nbsp;&nbsp;本页面用来设置系统的登录用户名及其密码，为了安全着想，请您务必改变默认的密码，否则可能会造成其它人员采用默认密码登录路由器进行破坏。</td>
          </tr>
		  <tr>
            <td>&nbsp;</td>
          </tr>
          <tr>
            <td class="titlebg">密码管理</td>
          </tr>
          <tr>
            <td>
			  <table class="space" width="100%" border="0" cellspacing="0" cellpadding="0">
                <tr>
                  <td width="25%" class="contentpadding">用户名:</td>
                  <td width="75%"><input type="text" name="admuser" size="16" maxlength="16" value="<% getCfgGeneral(1, "Login"); %>" /></td>
                </tr>
                <tr>
                  <td class="contentpadding">新密码:</td>
                  <td><input type="password" name="admpass" size="16" maxlength="32" value="<% getCfgGeneral(1, "Password"); %>"></td>
                </tr>
				<tr>
                  <td class="contentpadding">确认密码:</td>
                  <td><input type="password" name="admpass1" size="16" maxlength="32" value="<% getCfgGeneral(1, "Password"); %>"></td>
                </tr>
              </table>
            </td>
          </tr>
		  <tr><td>&nbsp;</td></tr>
		  <tr> 
			<td colspan="2" class="contentpadding"><input type="hidden" value="/modify_password.asp" name="submit-url"><input type=submit class=button value="提 交" onClick="return AdmFormCheck()">&nbsp;&nbsp;<input type=reset class=button  value="取 消" onClick="window.location.reload()"></td>
		  </tr>
		  <tr><td>&nbsp;</td></tr>
        </table>
     </td>
  </tr>
  <tr>
    <td>&nbsp;</td>
  </tr>
</table>
</form>
</body></html>
