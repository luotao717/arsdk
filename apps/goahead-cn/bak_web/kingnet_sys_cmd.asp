<html>
<head>
<title>WR922</title>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312" />
<link href="images/kingnet.css" type="text/css" rel="stylesheet">
<script language="JavaScript" type="text/javascript">
function formCheck()
{
	if( document.SystemCommand.command.value == ""){
		alert("请输入命令.");
		return false;
	}

	return true;
}

function setFocus()
{
	document.SystemCommand.command.focus();
}

</script>

</head>
<body onLoad="setFocus()">
<form method="post" name="SystemCommand" action="/goform/SystemCommand">
<table width="90%" border="0" cellpadding="0" cellspacing="0" align="center">
  <tr>
    <td style="padding-top:30px;">
	   <table width="100%" border="0" cellpadding="0" cellspacing="0">
          <tr>
            <td class="nav">当前路径:KN-WR922 &gt;&gt;系统管理 &gt;&gt;系统命令 </td>
          </tr>
          <tr>
            <td>&nbsp;</td>
          </tr>
		  <tr>
            <td class="space" style="padding-left:10px; padding-right:10px;">&nbsp;&nbsp;&nbsp;本页面用来运行一些系统自带的小程序，如ifconfig,等等。 </td>
          </tr>
		  <tr>
		    <td>&nbsp;</td>
		  </tr>
          <tr>
            <td><table class="space" width=100% border="0" cellpadding="0" cellspacing="0">
              <tr>
                <td colspan="2" class="titlebg">系统命令</td>
			  </tr>
			  <tr>
				<td width=25% class="contentpadding"> 命令:	</td>
				<td width=75%><input type="text" name="command" size="30" maxlength="256" >	</td>
			  </tr>
			  <tr>
			    <td colspan=2 class="contentpadding"><textarea cols="80" rows="18" wrap="off" readonly="1"><% showSystemCommandASP(); %></textarea></td>
			  </tr>
            </table></td>
          </tr>
		  <tr><td>&nbsp;</td></tr>
		  <tr>
			<td class="contentpadding"><input value="提 交" name="SystemCommandSubmit" onClick="return formCheck()" type="submit" class="button">&nbsp;&nbsp;<input class="button" value="取 消" name="reset" type="reset"></td>
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
</body>
</html>