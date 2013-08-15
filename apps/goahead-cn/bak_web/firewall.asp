<html>
<head>
<title>WR922</title>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312" />
<link href="images/kingnet.css" type="text/css" rel="stylesheet">
<script type="text/javascript" src="common.js"> </script>
<SCRIPT src="helpScript.js"></SCRIPT>
<script language="JavaScript" type="text/javascript">
function saveClick()
{
  return true;
}

function updateState()
{
	var enable = '<% getCfgGeneral(1, "FirewallSetType"); %>';
	if (enable == "ON")
		document.Firewall_Set.firewall.options.selectedIndex = 1;
		else
		document.Firewall_Set.firewall.options.selectedIndex = 0;
	document.getElementById("ourFir").style.visibility = "hidden";
	document.getElementById("ourFir").style.display = "none";
	var spi = "<% getCfgGeneral(1, "SPIFWEnabled"); %>";
	if(spi == "1")
		document.Firewall_Set.spiFWEnabled.options.selectedIndex = 1;
	else
		document.Firewall_Set.spiFWEnabled.options.selectedIndex = 0;
}
</script>
</head>
<body onLoad="updateState()">
<form action=/goform/FirewallSet method=POST name="Firewall_Set">
<table width="90%" border="0" cellpadding="0" cellspacing="0" align="center">
  <tr>
    <td style="padding-top:30px;"><table width="100%" border="0" cellpadding="0" cellspacing="0">
      <tr>
        <td class="nav">当前路径:KN-WR922 &gt;&gt;网络安全 &gt;&gt;防火墙</td>
      </tr>
      <tr>
        <td>&nbsp;</td>
      </tr>
	   <tr>
            <td class="space" style="padding-left:10px; padding-right:10px;">&nbsp;&nbsp;&nbsp;本页面是用来对路由器的防火墙进行调整的,一般情况下,您只需要保持默认配置就可以了.注意：改变该项配置后请重新启动路由器一遍。更多帮助信息请点击<input type="button" class="button5" value="帮助" onclick=popHelp('help.htm#secFirewall')>
            按钮.</td>
          </tr>
		  <tr>
		    <td>&nbsp;</td>
		  </tr>
      <tr>
        <td><table class="space" width="100%" border="0" cellspacing="0" cellpadding="0">
          <tr>
            <td class="titlebg" colspan="2">防火墙设置 </td>
          </tr>
		  <tr id="ourFir">
            <td width="25%" class="contentpadding">防火墙:</td>
            <td width="75%"><select name="firewall" size="1">
	<option value=0 >关闭</option>
    <option value=1 >开启</option></select></td>
          </tr>
		  <tr id="spiFir">
            <td width="25%" class="contentpadding">SPI(状态)防火墙:</td>
            <td width="75%"><select name="spiFWEnabled" size="1">
	<option value=0 id="sysfwSPIFWDisable">关闭</option>
    <option value=1 id="sysfwSPIFWDisable">开启</option></select></td>
          </tr>
        </table></td>
      </tr>
	  <tr><td>&nbsp;</td></tr>
	  <tr>
        <td class="contentpadding"><input type="hidden" value="/firewall.asp" name="submit-url"><input type="submit" class="button"  value="提 交" onClick="return saveClick()"/>&nbsp;&nbsp;<input type="reset" class="button"  value="取 消" onClick="window.location.reload()"></td>
      </tr>
	  <tr><td>&nbsp;</td></tr>
    </table></td>
  </tr>
  <tr>
     <td>&nbsp;</td>
   </tr>
</table>
</form>
</body>
</html>
