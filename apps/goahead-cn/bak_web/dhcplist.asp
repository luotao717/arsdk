<html>
<head>
<title>WR922</title>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312" />
<link href="images/kingnet.css" type="text/css" rel="stylesheet">
<script type="text/javascript" src="common.js"> </script>
<SCRIPT src="helpScript.js"></SCRIPT>
<script language="JavaScript" type="text/javascript">
</script>
</head>
<body>
<table width="90%" border="0" cellpadding="0" cellspacing="0" align="center">
  <tr>
    <td style="padding-top:30px;">
	   <table width="100%" border="0" cellpadding="0" cellspacing="0">
          <tr>
            <td class="nav">当前路径:KN-WR922 &gt;&gt;LAN 设置 &gt;&gt;DHCP 客户列表</td>
          </tr>
		  <tr>
		    <td>&nbsp;</td>
		  </tr>
		  <tr>
            <td class="space" style="padding-left:10px; padding-right:10px;">&nbsp;&nbsp;&nbsp;本页面显示通过DHCP方式连接到本路由器的客户端的相关信息,详情请点<input type="button" class="button5" value="帮助" onclick=popHelp('help.htm#dhcpclientlist')>.</td>
          </tr>
		  <tr>
		    <td>&nbsp;</td>
		  </tr>
		  <tr>
            <td class="titlebg" colspan="3">DHCP 客户 </td>
          </tr>
		  <tr>
		    <td><table class="space" width="100%" border="0" cellpadding="0" cellspacing="0">	
			  <tr>
				 <td width="26%" class="contentpadding">MAC 地址</td>
				 <td width="23%">IP 地址</td>
				 <td width="51%">时间</td>
		      </tr>
		   <% getDhcpCliList(); %>
		    </table></td>
		  </tr>
   	   </table>
	</td>
  </tr>
</table>
</body>
</html>