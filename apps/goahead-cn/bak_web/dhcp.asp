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
{		if ( checkIpAddr1(document.dhcpCfg.dhcpMask, '��Ч����������! ') == false )
	    		return false;
		if ( checkIpAddr1(document.dhcpCfg.dhcpPriDns, '��Ч DNS! ') == false )
	    		return false;		
        if ( checkIpAddr1(document.dhcpCfg.dhcpSecDns, '��Ч DNS! ') == false )
	    		return false;
        if ( checkIpAddr1(document.dhcpCfg.dhcpGateway, '��Ч����! ') == false )
	    		return false;
		if(!validateKey(document.dhcpCfg.dhcpLease.value)){
		    alert('����������.');
			document.dhcpCfg.dhcpLease.value = document.dhcpCfg.dhcpLease.defaultValue;
			document.dhcpCfg.dhcpLease.focus();
			return false;
		}
		
			
			
			if ( !checkSubnet("<% getCfgZero(1, "dhcpStart"); %>",document.dhcpCfg.dhcpMask.value,document.dhcpCfg.dhcpGateway.value)) {
			alert('���غ�����IP������ͬһ��������.');
			document.dhcpCfg.dhcpGateway.value = document.dhcpCfg.dhcpGateway.defaultValue;
			document.dhcpCfg.dhcpGateway.focus();
			return false;
      		}
			
			if (document.dhcpCfg.dhcpStatic1Mac.value != "") {
			var re = /[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}/;
			if (!re.test(document.dhcpCfg.dhcpStatic1Mac.value)) {
				alert("��������ȷ��MAC��ַ��ʽ! (XX:XX:XX:XX:XX:XX)");
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
				alert("��������ȷ��MAC��ַ��ʽ! (XX:XX:XX:XX:XX:XX)");
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
				alert("��������ȷ��MAC��ַ��ʽ! (XX:XX:XX:XX:XX:XX)");
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
            <td class="nav">��ǰ·��:KN-WR922 &gt;&gt;LAN ���� &gt;&gt;DHCP ���� </td>
          </tr>
		  <tr>
		    <td>&nbsp;</td>
		  </tr>
          <tr>
            <td class="space" style="padding-left:10px; padding-right:10px;">&nbsp;&nbsp;&nbsp;��ҳ��������DHCP��������һЩ����������������,һ������²�����Ҫ����Щ���޸�,����ϵͳĬ�ϵľͿ�����.�����������Ҫ��ĳ̨PC����̶���IP��ַ,����ʹ�þ�̬�趨����.������Ϣ���<input type="button" class="button5" value="����" onclick=popHelp('help.htm#dhcpserver')></td>
          </tr>
		  <tr>
		    <td>&nbsp;</td>
		  </tr>
          <tr>
            <td class="titlebg">DHCP ���� </td>
          </tr>
          <tr>
            <td><table class="space" width="100%" border="0" cellspacing="0" cellpadding="0">
              <tr id="enable">
                <td colspan="2">&nbsp;&nbsp;&nbsp;<font color="cc0000">ע��:����Ҫ���ô���������DHCP����.</font></td>
              </tr>
			  <tr id="mask">
                <td class="contentpadding" width="25%">DHCP ��������:</td>
                <td><input name="dhcpMask" maxlength=15 value="<% getCfgGeneral(1, "dhcpMask"); %>"> 
                (��Ǳ�Ҫ�������޸�)</td>
              </tr>
			  <tr id="pridns">
                <td class="contentpadding">DHCP �� DNS:</td>
                <td width="75%"><input name="dhcpPriDns" maxlength=15 value="<% getCfgGeneral(1, "dhcpPriDns"); %>"> 
                (��Ǳ�Ҫ�������޸�)</td>
              </tr>
			  <tr id="secdns">
                <td class="contentpadding">DHCP �� DNS:</td>
                <td><input name="dhcpSecDns" maxlength=15 value="<% getCfgGeneral(1, "dhcpSecDns"); %>"> 
                (��Ǳ�Ҫ�������޸�)</td>
              </tr>
			  <tr id="gateway">
                <td class="contentpadding">DHCP Ĭ������:</td>
                <td><input name="dhcpGateway" maxlength=15 value="<% getCfgGeneral(1, "dhcpGateway"); %>"> 
                (��Ǳ�Ҫ�������޸�)</td>
              </tr>
			  <tr id="lease">
                <td class="contentpadding">DHCP ��Լʱ��:</td>
                <td><input name="dhcpLease" maxlength=8 value="<% getCfgGeneral(1, "dhcpLease"); %>"> 
                (����Ϊ��λ��Ĭ����86400��Ҳ����1��)</td>
              </tr>
			  <tr id="staticlease1">
                <td class="contentpadding">��̬�趨:</td>
                <td><input type=hidden name=dhcpStatic1 value=""> MAC: <input name="dhcpStatic1Mac" maxlength=17 value="<% getCfgNthGeneral(1, "dhcpStatic1", 0); %>"> IP: <input name="dhcpStatic1Ip" maxlength=15 value="<% getCfgNthGeneral(1, "dhcpStatic1", 1); %>"></td>
              </tr>
			  <tr id="staticlease2">
                <td class="contentpadding">��̬�趨:</td>
                <td><input type=hidden name=dhcpStatic2 value=""> MAC: <input name="dhcpStatic2Mac" maxlength=17 value="<% getCfgNthGeneral(1, "dhcpStatic2", 0); %>"> IP: <input name="dhcpStatic2Ip" maxlength=15 value="<% getCfgNthGeneral(1, "dhcpStatic2", 1); %>"></td>
              </tr>
			  <tr id="staticlease3">
                <td class="contentpadding">��̬�趨:</td>
                <td><input type=hidden name=dhcpStatic3 value=""> MAC: <input name="dhcpStatic3Mac" maxlength=17 value="<% getCfgNthGeneral(1, "dhcpStatic3", 0); %>"> IP: <input name="dhcpStatic3Ip" maxlength=15 value="<% getCfgNthGeneral(1, "dhcpStatic3", 1); %>"></td>
              </tr>
			  <tr>
                <td colspan="2">&nbsp;&nbsp;&nbsp;ע��MAC��ַ�ĸ�ʽ�� <font color="cc0000">00:14:25:22:A1:67 </font></td>
              </tr>
            </table></td>
          </tr>
		  <tr><td>&nbsp;</td></tr>
		  <tr id=button>
            <td class="contentpadding"><input type="hidden" value="/dhcp.asp" name="submit-url"><input type="submit" class="button"  value="�� ��" />&nbsp;&nbsp;<input type="reset" class="button"  value="ȡ ��" onClick="window.location.reload()"></td>
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
