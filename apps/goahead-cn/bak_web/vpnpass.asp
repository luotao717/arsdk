<html>
<head>
<title>WR922</title>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312" />
<link href="images/kingnet.css" type="text/css" rel="stylesheet">
<script type="text/javascript" src="common.js"> </script>
<SCRIPT src="helpScript.js"></SCRIPT>
<script language="JavaScript" type="text/javascript">
function initValue()
{
	var l2tp = <% getCfgZero(1, "l2tpPassThru"); %>;
	var ipsec = <% getCfgZero(1, "ipsecPassThru"); %>;
	var pptp = <% getCfgZero(1, "pptpPassThru"); %>;
	var wanping = <% getCfgZero(1, "wan_ping_enable"); %>;
	var policy3 = <% getCfgZero(1, "xingkongjisu"); %>;
	var policy1 = <% getCfgZero(1, "ttl_115_117_drop"); %>;
	var policy2 = <% getCfgZero(1, "shandong_rizhao"); %>;
	var mss_en = <% getCfgZero(1, "mss_en"); %>;


	document.vpnpass.l2tpPT.options.selectedIndex = 1*l2tp;
	document.vpnpass.ipsecPT.options.selectedIndex = 1*ipsec;
	document.vpnpass.pptpPT.options.selectedIndex = 1*pptp;
	document.vpnpass.wan_ping.options.selectedIndex = 1*wanping;
	document.vpnpass.mss.options.selectedIndex = 1*mss_en;
	if(policy1 == 1)
		document.vpnpass.teshu_policy1.checked = true;
	else
		document.vpnpass.teshu_policy1.checked = false;
	if(policy2 == 1)
		document.vpnpass.teshu_policy2.checked = true;
	else
		document.vpnpass.teshu_policy2.checked = false;
//	if(policy3 == 1)
//		document.vpnpass.teshu_policy3.checked = true;
//	else
//		document.vpnpass.teshu_policy3.checked = false;
}
</script>
</head>
<body onLoad="initValue()">
<form method=post name="vpnpass" action="/goform/setVpnPaThru">
<table width="90%" border="0" cellpadding="0" cellspacing="0" align="center">
  <tr>
    <td style="padding-top:30px;">
	   <table width="100%" border="0" cellpadding="0" cellspacing="0">
          <tr>
            <td class="nav">当前路径:KN-WR922 &gt;&gt;WAN 设置 &gt;&gt;高级设置</td>
          </tr>
		  <tr>
		    <td>&nbsp;</td>
		  </tr>
		  <tr>
            <td class="space" style="padding-left:10px; padding-right:10px;">&nbsp;&nbsp;&nbsp;本页面用来设置WAN口的一些其它的参数. 详情请点<input type="button" class="button5" value="帮助" onclick=popHelp('help.htm#wanadv')> </td>
          </tr>
		  <tr>
		    <td>&nbsp;</td>
		  </tr>
		  <tr>
		    <td><table class="space" width="100%" border="0" cellpadding="0" cellspacing="0">
			  <tr> 
				<td class="titlebg" colspan="2">高级设置</td>
			  </tr>
			  <tr>
				<td class="contentpadding" width="25%">允许WAN的PING:</td>
				<td><select name="wan_ping" size="1">
					<option value="0">禁用</option>
					<option value="1">启用</option></select>
				(为了安全，您可以禁用它)</td>
			  </tr>
			  <tr>
				<td class="contentpadding" width="25%">L2TP 穿透:</td>
				<td><select name="l2tpPT" size="1">
					<option value="0">禁用</option>
					<option value="1">启用</option></select></td>
			  </tr>
			  <tr>
				<td class="contentpadding" width="25%">IPSec 穿透:</td>
				<td><select name="ipsecPT" size="1">
					<option value="0">禁用</option>
					<option value="1">启用</option></select></td>
			  </tr>
			  <tr>
				<td class="contentpadding" width="25%">PPTP 穿透:</td>
				<td><select name="pptpPT" size="1">
					<option value="0">禁用</option>
					<option value="1">启用</option></select></td>
			  </tr>
			  <tr>
				<td class="contentpadding" width="25%">MSS策略:</td>
				<td><select name="mss" size="1">
					<option value="0">禁用</option>
					<option value="1">启用</option></select></td>
			  </tr>
<!--			  <tr>
                  <td width="25%" class="contentpadding" >特殊策略:</td>
                  <td width="75%"><select name="xingkongjisu" size="1">
					<option value="0">禁用</option>
					<option value="1">启用</option></select></td>
               </tr>
-->
			   <tr>
                  <td width="25%" class="contentpadding" >特殊策略:</td>
                  <td width="75%">特殊策略1<input name="teshu_policy1" type="checkbox" value="1">
                  特殊策略2
                    <input name="teshu_policy2" type="checkbox" value="1">
<!--                    
                    特殊策略3
                    <input name="teshu_policy3" type="checkbox" value="1">
 -->                   
                    </td>
			   </tr>
			</table>
		  </td>
		</tr>
		<tr><td>&nbsp;</td></tr>
		<tr>
		  <td class="contentpadding"><input type="hidden" value="/vpnpass.asp" name="submit-url"><input type="submit" class="button"  value="提 交" />&nbsp;&nbsp;<input type="reset" class="button"  value="取 消" onClick="window.location.reload()"></td>
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
