<html>
<head>
<title>WR922</title>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312" />
<link href="images/kingnet.css" type="text/css" rel="stylesheet">
<script type="text/javascript" src="common.js"> </script>
<script language="JavaScript" type="text/javascript">
function saveClick()
{
  return true;
}

function updateState()
{
	var enable = <% getCfgZero(1, "web_wan_access_enable"); %>;
		document.formWebAccess.webWanAccess.options.selectedIndex = 1*enable;
}
</script>
</head>
<body onLoad="updateState()">
<form action=/goform/formWebWanAccess method=POST name="formWebAccess">
<table width="90%" border="0" cellpadding="0" cellspacing="0" align="center">
  <tr>
    <td style="padding-top:30px;"><table width="100%" border="0" cellpadding="0" cellspacing="0">
      <tr>
        <td class="nav" colspan="2">��ǰ·��:KN-WR922 &gt;&gt;�������� &gt;&gt;Զ�̿��� </td>
      </tr>
      <tr>
        <td>&nbsp;</td>
      </tr>
	  <tr>
            <td class="space" style="padding-left:10px; padding-right:10px;">&nbsp;&nbsp;&nbsp;������ҪԶ�̹���·����ʱ�������������ܣ�����������ͨ��INTERNET������·����������Ϊ�˰�ȫ����һ���������ر����� </td>
          </tr>
		  <tr>
		    <td>&nbsp;</td>
		  </tr>
      <tr>
        <td><table class="space" width="100%" border="0" cellspacing="0" cellpadding="0">
          <tr>
            <td class="titlebg" colspan="2">Web Զ�̿���</td>
          </tr>
		  <tr>
            <td width="25%" class="contentpadding">Web Զ�̿���:</td>
            <td width="75%"><select name="webWanAccess" size="1">
					<option value=0 >����</option>
					<option value=1 >����</option>
					</select></td>
          </tr>
        </table></td>
      </tr>
	  <tr><td>&nbsp;</td></tr>
      <tr>
        <td class="contentpadding"><input type="hidden" value="/remote_control.asp" name="submit-url"><input type="submit" class="button"  value="�� ��" onClick="return saveClick()"/>&nbsp;&nbsp;<input type="reset" class="button"  value="ȡ ��" onClick="window.location.reload()"></td>
      </tr>
	  <tr><td>&nbsp;</td></tr>
    </table></td>
  </tr>
</table>
</form>
</body>
</html>
