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
        <td class="nav" colspan="2">当前路径:KN-WR922 &gt;&gt;服务设置 &gt;&gt;远程控制 </td>
      </tr>
      <tr>
        <td>&nbsp;</td>
      </tr>
	  <tr>
            <td class="space" style="padding-left:10px; padding-right:10px;">&nbsp;&nbsp;&nbsp;当您需要远程管理路由器时可以启动本功能，它可以让您通过INTERNET来管理路由器。但是为了安全着想一般情况下请关闭它。 </td>
          </tr>
		  <tr>
		    <td>&nbsp;</td>
		  </tr>
      <tr>
        <td><table class="space" width="100%" border="0" cellspacing="0" cellpadding="0">
          <tr>
            <td class="titlebg" colspan="2">Web 远程控制</td>
          </tr>
		  <tr>
            <td width="25%" class="contentpadding">Web 远程控制:</td>
            <td width="75%"><select name="webWanAccess" size="1">
					<option value=0 >禁用</option>
					<option value=1 >启用</option>
					</select></td>
          </tr>
        </table></td>
      </tr>
	  <tr><td>&nbsp;</td></tr>
      <tr>
        <td class="contentpadding"><input type="hidden" value="/remote_control.asp" name="submit-url"><input type="submit" class="button"  value="提 交" onClick="return saveClick()"/>&nbsp;&nbsp;<input type="reset" class="button"  value="取 消" onClick="window.location.reload()"></td>
      </tr>
	  <tr><td>&nbsp;</td></tr>
    </table></td>
  </tr>
</table>
</form>
</body>
</html>
