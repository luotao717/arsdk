<html>
<head>
<title>WR922</title>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312" />
<link href="images/kingnet.css" type="text/css" rel="stylesheet">
<script language="JavaScript" type="text/javascript">
function AdmFormCheck()
{
	if (document.Adm.admuser.value == "") {
		alert("���������Ա�˺�.");
		return false;
	}
	if (document.Adm.admpass.value == "") {
		alert("���������Ա����.");
		return false;
	}
	if (document.Adm.admpass1.value == "") {
		alert("���ٴ�ȷ�Ϲ���Ա����.");
		return false;
	}
		
	if(document.Adm.admpass.value != document.Adm.admpass1.value) {
		alert("���벻ƥ��!");
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
            <td class="nav">��ǰ·��:KN-WR922 &gt;&gt;ϵͳ���� &gt;&gt;�������</td>
          </tr>
		  <tr>
		    <td>&nbsp;</td>
		  </tr>
          <tr>
            <td class="space" style="padding-left:10px; padding-right:10px;">&nbsp;&nbsp;&nbsp;��ҳ����������ϵͳ�ĵ�¼�û����������룬Ϊ�˰�ȫ���룬������ظı�Ĭ�ϵ����룬������ܻ����������Ա����Ĭ�������¼·���������ƻ���</td>
          </tr>
		  <tr>
            <td>&nbsp;</td>
          </tr>
          <tr>
            <td class="titlebg">�������</td>
          </tr>
          <tr>
            <td>
			  <table class="space" width="100%" border="0" cellspacing="0" cellpadding="0">
                <tr>
                  <td width="25%" class="contentpadding">�û���:</td>
                  <td width="75%"><input type="text" name="admuser" size="16" maxlength="16" value="<% getCfgGeneral(1, "Login"); %>" /></td>
                </tr>
                <tr>
                  <td class="contentpadding">������:</td>
                  <td><input type="password" name="admpass" size="16" maxlength="32" value="<% getCfgGeneral(1, "Password"); %>"></td>
                </tr>
				<tr>
                  <td class="contentpadding">ȷ������:</td>
                  <td><input type="password" name="admpass1" size="16" maxlength="32" value="<% getCfgGeneral(1, "Password"); %>"></td>
                </tr>
              </table>
            </td>
          </tr>
		  <tr><td>&nbsp;</td></tr>
		  <tr> 
			<td colspan="2" class="contentpadding"><input type="hidden" value="/modify_password.asp" name="submit-url"><input type=submit class=button value="�� ��" onClick="return AdmFormCheck()">&nbsp;&nbsp;<input type=reset class=button  value="ȡ ��" onClick="window.location.reload()"></td>
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
