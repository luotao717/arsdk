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
        <td class="nav">��ǰ·��:KN-WR922 &gt;&gt;���簲ȫ &gt;&gt;����ǽ</td>
      </tr>
      <tr>
        <td>&nbsp;</td>
      </tr>
	   <tr>
            <td class="space" style="padding-left:10px; padding-right:10px;">&nbsp;&nbsp;&nbsp;��ҳ����������·�����ķ���ǽ���е�����,һ�������,��ֻ��Ҫ����Ĭ�����þͿ�����.ע�⣺�ı�������ú�����������·����һ�顣���������Ϣ����<input type="button" class="button5" value="����" onclick=popHelp('help.htm#secFirewall')>
            ��ť.</td>
          </tr>
		  <tr>
		    <td>&nbsp;</td>
		  </tr>
      <tr>
        <td><table class="space" width="100%" border="0" cellspacing="0" cellpadding="0">
          <tr>
            <td class="titlebg" colspan="2">����ǽ���� </td>
          </tr>
		  <tr id="ourFir">
            <td width="25%" class="contentpadding">����ǽ:</td>
            <td width="75%"><select name="firewall" size="1">
	<option value=0 >�ر�</option>
    <option value=1 >����</option></select></td>
          </tr>
		  <tr id="spiFir">
            <td width="25%" class="contentpadding">SPI(״̬)����ǽ:</td>
            <td width="75%"><select name="spiFWEnabled" size="1">
	<option value=0 id="sysfwSPIFWDisable">�ر�</option>
    <option value=1 id="sysfwSPIFWDisable">����</option></select></td>
          </tr>
        </table></td>
      </tr>
	  <tr><td>&nbsp;</td></tr>
	  <tr>
        <td class="contentpadding"><input type="hidden" value="/firewall.asp" name="submit-url"><input type="submit" class="button"  value="�� ��" onClick="return saveClick()"/>&nbsp;&nbsp;<input type="reset" class="button"  value="ȡ ��" onClick="window.location.reload()"></td>
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
