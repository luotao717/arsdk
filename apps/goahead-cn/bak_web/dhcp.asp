<html>
<head>
<title>WR922</title>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312" />
<link href="images/kingnet.css" type="text/css" rel="stylesheet">
<script type="text/javascript" src="common.js"> </script>
<SCRIPT src="helpScript.js"></SCRIPT>
<script language="JavaScript" type="text/javascript">
function display_on()
{
	if (window.ActiveXObject) { // IE
		return "block";
	}
	else if (window.XMLHttpRequest) { // Mozilla, Firefox, Safari,...
		return "table-row";
	}
}

   function dhcpTypeSwitch()
{     var dhcp = <% getCfgZero(1, "dhcpEnabled"); %>;

	document.getElementById("enable").style.display = display_on();

	document.getElementById("button").style.display = "none";
	
	document.getElementById("mask").style.visibility = "hidden";
	document.getElementById("mask").style.display = "none";
	document.dhcpCfg.dhcpMask.disabled = true;
	document.getElementById("pridns").style.visibility = "hidden";
	document.getElementById("pridns").style.display = "none";
	document.dhcpCfg.dhcpPriDns.disabled = true;
	document.getElementById("secdns").style.visibility = "hidden";
	document.getElementById("secdns").style.display = "none";
	document.dhcpCfg.dhcpSecDns.disabled = true;
	document.getElementById("gateway").style.visibility = "hidden";
	document.getElementById("gateway").style.display = "none";
	document.dhcpCfg.dhcpGateway.disabled = true;
	document.getElementById("lease").style.visibility = "hidden";
	document.getElementById("lease").style.display = "none";
	document.dhcpCfg.dhcpLease.disabled = true;
	document.getElementById("staticlease1").style.visibility = "hidden";
	document.getElementById("staticlease1").style.display = "none";
	document.dhcpCfg.dhcpLease.disabled = true;
	document.getElementById("staticlease2").style.visibility = "hidden";
	document.getElementById("staticlease2").style.display = "none";
	document.dhcpCfg.dhcpLease.disabled = true;
	document.getElementById("staticlease3").style.visibility = "hidden";
	document.getElementById("staticlease3").style.display = "none";
	document.dhcpCfg.dhcpLease.disabled = true;
	if ( dhcp== 1)
	{

	    document.getElementById("enable").style.display = "none";

	    document.getElementById("button").style.display = display_on();
		
		document.getElementById("mask").style.visibility = "visible";
		document.getElementById("mask").style.display = display_on();
		document.dhcpCfg.dhcpMask.disabled = false;
		document.getElementById("pridns").style.visibility = "visible";
		document.getElementById("pridns").style.display = display_on();
		document.dhcpCfg.dhcpPriDns.disabled = false;
		document.getElementById("secdns").style.visibility = "visible";
		document.getElementById("secdns").style.display = display_on();
		document.dhcpCfg.dhcpSecDns.disabled = false;
		document.getElementById("gateway").style.visibility = "visible";
		document.getElementById("gateway").style.display = display_on();
		document.dhcpCfg.dhcpGateway.disabled = false;
		document.getElementById("lease").style.visibility = "visible";
		document.getElementById("lease").style.display = display_on();
		document.dhcpCfg.dhcpLease.disabled = false;
		document.getElementById("staticlease1").style.visibility = "visible";
		document.getElementById("staticlease1").style.display = display_on();
		document.dhcpCfg.dhcpLease.disabled = false;
		document.getElementById("staticlease2").style.visibility = "visible";
		document.getElementById("staticlease2").style.display = display_on();
		document.dhcpCfg.dhcpLease.disabled = false;
		document.getElementById("staticlease3").style.visibility = "visible";
		document.getElementById("staticlease3").style.display = display_on();
		document.dhcpCfg.dhcpLease.disabled = false;
	}
}

function CheckValue()
{		if ( checkIpAddr1(document.dhcpCfg.dhcpMask, '无效的子网掩码! ') == false )
	    		return false;
		if ( checkIpAddr1(document.dhcpCfg.dhcpPriDns, '无效 DNS! ') == false )
	    		return false;		
        if ( checkIpAddr1(document.dhcpCfg.dhcpSecDns, '无效 DNS! ') == false )
	    		return false;
        if ( checkIpAddr1(document.dhcpCfg.dhcpGateway, '无效网关! ') == false )
	    		return false;
		if(!validateKey(document.dhcpCfg.dhcpLease.value)){
		    alert('必须是数字.');
			document.dhcpCfg.dhcpLease.value = document.dhcpCfg.dhcpLease.defaultValue;
			document.dhcpCfg.dhcpLease.focus();
			return false;
		}
		
			
			
			if ( !checkSubnet("<% getCfgZero(1, "dhcpStart"); %>",document.dhcpCfg.dhcpMask.value,document.dhcpCfg.dhcpGateway.value)) {
			alert('网关和所设IP必须在同一个子网内.');
			document.dhcpCfg.dhcpGateway.value = document.dhcpCfg.dhcpGateway.defaultValue;
			document.dhcpCfg.dhcpGateway.focus();
			return false;
      		}
			
			if (document.dhcpCfg.dhcpStatic1Mac.value != "") {
			var re = /[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}/;
			if (!re.test(document.dhcpCfg.dhcpStatic1Mac.value)) {
				alert("请填入正确的MAC地址格式! (XX:XX:XX:XX:XX:XX)");
				document.dhcpCfg.dhcpStatic1Mac.focus();
				return false;
			}
			if (!checkIpAddr(document.dhcpCfg.dhcpStatic1Ip, false))
				return false;
			document.dhcpCfg.dhcpStatic1.value = document.dhcpCfg.dhcpStatic1Mac.value + ';' + document.dhcpCfg.dhcpStatic1Ip.value;
		}
		if (document.dhcpCfg.dhcpStatic2Mac.value != "") {
			var re = /[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}/;
			if (!re.test(document.dhcpCfg.dhcpStatic2Mac.value)) {
				alert("请填入正确的MAC地址格式! (XX:XX:XX:XX:XX:XX)");
				document.dhcpCfg.dhcpStatic2Mac.focus();
				return false;
			}
			if (!checkIpAddr(document.dhcpCfg.dhcpStatic2Ip, false))
				return false;
			document.dhcpCfg.dhcpStatic2.value = document.dhcpCfg.dhcpStatic2Mac.value + ';' + document.dhcpCfg.dhcpStatic2Ip.value;
		}
		if (document.dhcpCfg.dhcpStatic3Mac.value != "") {
			var re = /[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}/;
			if (!re.test(document.dhcpCfg.dhcpStatic3Mac.value)) {
				alert("请填入正确的MAC地址格式! (XX:XX:XX:XX:XX:XX)");
				document.dhcpCfg.dhcpStatic3Mac.focus();
				return false;
			}
			if (!checkIpAddr(document.dhcpCfg.dhcpStatic3Ip, false))
				return false;
			document.dhcpCfg.dhcpStatic3.value = document.dhcpCfg.dhcpStatic3Mac.value + ';' + document.dhcpCfg.dhcpStatic3Ip.value;
		}
			
			
}
</script>
</head>
<body onLoad="dhcpTypeSwitch()">
<form method=post name="dhcpCfg" action="/goform/setDhcp" onSubmit="return CheckValue()">
<table width="90%" border="0" cellpadding="0" cellspacing="0" align="center">
  <tr>
    <td style="padding-top:30px;">
	   <table width="100%" border="0" cellpadding="0" cellspacing="0">
          <tr>
            <td class="nav">当前路径:KN-WR922 &gt;&gt;LAN 设置 &gt;&gt;DHCP 设置 </td>
          </tr>
		  <tr>
		    <td>&nbsp;</td>
		  </tr>
          <tr>
            <td class="space" style="padding-left:10px; padding-right:10px;">&nbsp;&nbsp;&nbsp;本页面用来对DHCP服务器的一些其它参数进行设置,一般情况下并不需要对这些作修改,保持系统默认的就可以了.另外如果您需要给某台PC分配固定的IP地址,可以使用静态设定功能.更多信息请点<input type="button" class="button5" value="帮助" onclick=popHelp('help.htm#dhcpserver')></td>
          </tr>
		  <tr>
		    <td>&nbsp;</td>
		  </tr>
          <tr>
            <td class="titlebg">DHCP 设置 </td>
          </tr>
          <tr>
            <td><table class="space" width="100%" border="0" cellspacing="0" cellpadding="0">
              <tr id="enable">
                <td colspan="2">&nbsp;&nbsp;&nbsp;<font color="cc0000">注意:如想要设置此项，您必须打开DHCP服务.</font></td>
              </tr>
			  <tr id="mask">
                <td class="contentpadding" width="25%">DHCP 子网掩码:</td>
                <td><input name="dhcpMask" maxlength=15 value="<% getCfgGeneral(1, "dhcpMask"); %>"> 
                (如非必要，请勿修改)</td>
              </tr>
			  <tr id="pridns">
                <td class="contentpadding">DHCP 主 DNS:</td>
                <td width="75%"><input name="dhcpPriDns" maxlength=15 value="<% getCfgGeneral(1, "dhcpPriDns"); %>"> 
                (如非必要，请勿修改)</td>
              </tr>
			  <tr id="secdns">
                <td class="contentpadding">DHCP 次 DNS:</td>
                <td><input name="dhcpSecDns" maxlength=15 value="<% getCfgGeneral(1, "dhcpSecDns"); %>"> 
                (如非必要，请勿修改)</td>
              </tr>
			  <tr id="gateway">
                <td class="contentpadding">DHCP 默认网关:</td>
                <td><input name="dhcpGateway" maxlength=15 value="<% getCfgGeneral(1, "dhcpGateway"); %>"> 
                (如非必要，请勿修改)</td>
              </tr>
			  <tr id="lease">
                <td class="contentpadding">DHCP 租约时间:</td>
                <td><input name="dhcpLease" maxlength=8 value="<% getCfgGeneral(1, "dhcpLease"); %>"> 
                (以秒为单位，默认是86400，也就是1天)</td>
              </tr>
			  <tr id="staticlease1">
                <td class="contentpadding">静态设定:</td>
                <td><input type=hidden name=dhcpStatic1 value=""> MAC: <input name="dhcpStatic1Mac" maxlength=17 value="<% getCfgNthGeneral(1, "dhcpStatic1", 0); %>"> IP: <input name="dhcpStatic1Ip" maxlength=15 value="<% getCfgNthGeneral(1, "dhcpStatic1", 1); %>"></td>
              </tr>
			  <tr id="staticlease2">
                <td class="contentpadding">静态设定:</td>
                <td><input type=hidden name=dhcpStatic2 value=""> MAC: <input name="dhcpStatic2Mac" maxlength=17 value="<% getCfgNthGeneral(1, "dhcpStatic2", 0); %>"> IP: <input name="dhcpStatic2Ip" maxlength=15 value="<% getCfgNthGeneral(1, "dhcpStatic2", 1); %>"></td>
              </tr>
			  <tr id="staticlease3">
                <td class="contentpadding">静态设定:</td>
                <td><input type=hidden name=dhcpStatic3 value=""> MAC: <input name="dhcpStatic3Mac" maxlength=17 value="<% getCfgNthGeneral(1, "dhcpStatic3", 0); %>"> IP: <input name="dhcpStatic3Ip" maxlength=15 value="<% getCfgNthGeneral(1, "dhcpStatic3", 1); %>"></td>
              </tr>
			  <tr>
                <td colspan="2">&nbsp;&nbsp;&nbsp;注：MAC地址的格式如 <font color="cc0000">00:14:25:22:A1:67 </font></td>
              </tr>
            </table></td>
          </tr>
		  <tr><td>&nbsp;</td></tr>
		  <tr id=button>
            <td class="contentpadding"><input type="hidden" value="/dhcp.asp" name="submit-url"><input type="submit" class="button"  value="提 交" />&nbsp;&nbsp;<input type="reset" class="button"  value="取 消" onClick="window.location.reload()"></td>
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
