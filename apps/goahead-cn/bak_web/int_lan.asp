<html>
<head>
<title>WR922</title>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312" />
<link href="images/kingnet.css" type="text/css" rel="stylesheet">
<script type="text/javascript" src="common.js"> </script>
<SCRIPT src="helpScript.js"></SCRIPT>
<script language="JavaScript" type="text/javascript">
var lan2 = '<% getCfgZero(1, "Lan2Enabled"); %>';
var upnp = <% getCfgZero(1, "upnpEnabled"); %>; 
function display_on()
{
	if (window.ActiveXObject) { // IE
		return "block";
	}
	else if (window.XMLHttpRequest) { // Mozilla, Firefox, Safari,...
		return "table-row";
	}
}




function atoi(str, num)
{
	i = 1;
	if (num != 1) {
		while (i != num && str.length != 0) {
			if (str.charAt(0) == '.') {
				i++;
			}
			str = str.substring(1);
		}
		if (i != num)
			return -1;
	}

	for (i=0; i<str.length; i++) {
		if (str.charAt(i) == '.') {
			str = str.substring(0, i);
			break;
		}
	}
	if (str.length == 0)
		return -1;
	return parseInt(str, 10);
}

function checkRange(str, num, min, max)
{
	d = atoi(str, num);
	if (d > max || d < min)
		return false;
	return true;
}

function isAllNum(str)
{
	for (var i=0; i<str.length; i++) {
		if ((str.charAt(i) >= '0' && str.charAt(i) <= '9') || (str.charAt(i) == '.' ))
			continue;
		return 0;
	}
	return 1;
}

function checkIpAddr(field, ismask)
{
	if (field.value == "") {
		alert("IP地址不能为空!");
		field.value = field.defaultValue;
		field.focus();
		return false;
	}

	if (isAllNum(field.value) == 0) {
		alert('必须为[0-9].');
		field.value = field.defaultValue;
		field.focus();
		return false;
	}

	if (ismask) {
		if ((!checkRange(field.value, 1, 0, 256)) ||
				(!checkRange(field.value, 2, 0, 256)) ||
				(!checkRange(field.value, 3, 0, 256)) ||
				(!checkRange(field.value, 4, 0, 256)))
		{
			alert('IP地址格式错误!');
			field.value = field.defaultValue;
			field.focus();
			return false;
		}
	}
	else {
		if ((!checkRange(field.value, 1, 0, 255)) ||
				(!checkRange(field.value, 2, 0, 255)) ||
				(!checkRange(field.value, 3, 0, 255)) ||
				(!checkRange(field.value, 4, 1, 254)))
		{
			alert('IP地址格式错误!');
			field.value = field.defaultValue;
			field.focus();
			return false;
		}
	}
	return true;
}


function CheckValue()
{       if (!checkIpAddr(document.lanCfg.lanIp, false))
			return false;
		if (!checkIpAddr(document.lanCfg.lanNetmask, true))
			return false;
			
		if (document.lanCfg.dhcpStatic1Mac.value != "") {
			var re = /[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}/;
			if (!re.test(document.lanCfg.dhcpStatic1Mac.value)) {
				alert("请填入正确的MAC地址格式! (XX:XX:XX:XX:XX:XX)");
				document.lanCfg.dhcpStatic1Mac.focus();
				return false;
			}
			if (!checkIpAddr(document.lanCfg.dhcpStatic1Ip, false))
				return false;
			document.lanCfg.dhcpStatic1.value = document.lanCfg.dhcpStatic1Mac.value + ';' + document.lanCfg.dhcpStatic1Ip.value;
		}
		if (document.lanCfg.dhcpStatic2Mac.value != "") {
			var re = /[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}/;
			if (!re.test(document.lanCfg.dhcpStatic2Mac.value)) {
				alert("请填入正确的MAC地址格式! (XX:XX:XX:XX:XX:XX)");
				document.lanCfg.dhcpStatic2Mac.focus();
				return false;
			}
			if (!checkIpAddr(document.lanCfg.dhcpStatic2Ip, false))
				return false;
			document.lanCfg.dhcpStatic2.value = document.lanCfg.dhcpStatic2Mac.value + ';' + document.lanCfg.dhcpStatic2Ip.value;
		}
		if (document.lanCfg.dhcpStatic3Mac.value != "") {
			var re = /[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}/;
			if (!re.test(document.lanCfg.dhcpStatic3Mac.value)) {
				alert("请填入正确的MAC地址格式! (XX:XX:XX:XX:XX:XX)");
				document.lanCfg.dhcpStatic3Mac.focus();
				return false;
			}
			if (!checkIpAddr(document.lanCfg.dhcpStatic3Ip, false))
				return false;
			document.lanCfg.dhcpStatic3.value = document.lanCfg.dhcpStatic3Mac.value + ';' + document.lanCfg.dhcpStatic3Ip.value;
		}
        
		        if(document.lanCfg.lanDhcpType.options.selectedIndex == 1)	
				{	
				if (!checkIpAddr(document.lanCfg.dhcpStart, false))
				     return false;
				if(!checkIpAddr(document.lanCfg.dhcpEnd, false))
				     return false;
					 
				if ( !checkClientRange(document.lanCfg.dhcpStart.value,document.lanCfg.dhcpEnd.value) ) {
		alert('DHCP范围错误!');
		document.lanCfg.dhcpStart.focus();
		return false;
        } 
		}
			
		if ( !checkSubnet(document.lanCfg.dhcpStart.value,document.lanCfg.lanNetmask.value,document.lanCfg.dhcpEnd.value)) {
		alert('DHCP范围错误!');
		document.lanCfg.dhcpEnd.value = document.lanCfg.dhcpEnd.defaultValue;
		document.lanCfg.dhcpEnd.focus();
		return false;
	}
					 
       document.lanCfg.dhcpGateway.value=document.lanCfg.lanIp.value;
       document.lanCfg.dhcpPriDns.value=document.lanCfg.lanIp.value; 
	   document.lanCfg.dhcpMask.value=document.lanCfg.lanNetmask.value;
	   
	   
	    var dhcprangestart="<% getCfgGeneral(1, "dhcpStart"); %>";
//		alert(dhcprangestart);
		if(document.lanCfg.dhcpStart.value==dhcprangestart){
//			alert("test");
			document.lanCfg.dhcpStart.value = getDigit(document.lanCfg.lanIp.value, 1)+'.'+getDigit(document.lanCfg.lanIp.value, 2)+'.'+getDigit(document.lanCfg.lanIp.value, 3)+'.'+getDigit(document.lanCfg.dhcpStart.value, 4);
//			alert(document.lanCfg.dhcpRangeStart.value);
			document.lanCfg.dhcpEnd.value = getDigit(document.lanCfg.lanIp.value, 1)+'.'+getDigit(document.lanCfg.lanIp.value, 2)+'.'+getDigit(document.lanCfg.lanIp.value, 3)+'.'+getDigit(document.lanCfg.dhcpEnd.value, 4);
//			alert(document.lanCfg.dhcpRangeEnd.value);	
		}
		var dhcprangeend="<% getCfgGeneral(1, "dhcpEnd"); %>";
		if(document.lanCfg.dhcpEnd.value==dhcprangeend){
			document.lanCfg.dhcpEnd.value = getDigit(document.lanCfg.lanIp.value, 1)+'.'+getDigit(document.lanCfg.lanIp.value, 2)+'.'+getDigit(document.lanCfg.lanIp.value, 3)+'.'+getDigit(document.lanCfg.dhcpEnd.value, 4);
//			alert(document.lanCfg.dhcpRangeEnd.value);	
			document.lanCfg.dhcpStart.value = getDigit(document.lanCfg.lanIp.value, 1)+'.'+getDigit(document.lanCfg.lanIp.value, 2)+'.'+getDigit(document.lanCfg.lanIp.value, 3)+'.'+getDigit(document.lanCfg.dhcpStart.value, 4);
//			alert(document.lanCfg.dhcpRangeStart.value);
		}
		
		if (document.lanCfg.lan2enabled[0].checked == true)
	{
		if (!checkIpAddr(document.lanCfg.lan2Ip, false))
			return false;
		if (!checkIpAddr(document.lanCfg.lan2Netmask, true))
			return false;
	}
	syncWithHost();
}
	
function upnpChange()
{
	if(document.lanCfg.upnpEnbl.options.selectedIndex == 1){//enable
		if(upnp == 1){
			document.lanCfg.upnpTable.disabled = false;	
		}else{
			document.lanCfg.upnpTable.disabled = true;	
		}
	}
}


function dhcpTypeSwitch()
{
	document.getElementById("start").style.visibility = "hidden";
	document.getElementById("start").style.display = "none";
	document.lanCfg.dhcpStart.disabled = true;
	document.getElementById("end").style.visibility = "hidden";
	document.getElementById("end").style.display = "none";
	document.lanCfg.dhcpEnd.disabled = true;
	document.getElementById("mask").style.visibility = "hidden";
	document.getElementById("mask").style.display = "none";
	document.lanCfg.dhcpMask.disabled = true;
	document.getElementById("pridns").style.visibility = "hidden";
	document.getElementById("pridns").style.display = "none";
	document.lanCfg.dhcpPriDns.disabled = true;
	document.getElementById("secdns").style.visibility = "hidden";
	document.getElementById("secdns").style.display = "none";
	document.lanCfg.dhcpSecDns.disabled = true;
	document.getElementById("gateway").style.visibility = "hidden";
	document.getElementById("gateway").style.display = "none";
	document.lanCfg.dhcpGateway.disabled = true;
	document.getElementById("lease").style.visibility = "hidden";
	document.getElementById("lease").style.display = "none";
	document.lanCfg.dhcpLease.disabled = true;
	document.getElementById("staticlease1").style.visibility = "hidden";
	document.getElementById("staticlease1").style.display = "none";
	document.lanCfg.dhcpLease.disabled = true;
	document.getElementById("staticlease2").style.visibility = "hidden";
	document.getElementById("staticlease2").style.display = "none";
	document.lanCfg.dhcpLease.disabled = true;
	document.getElementById("staticlease3").style.visibility = "hidden";
	document.getElementById("staticlease3").style.display = "none";
	document.lanCfg.dhcpLease.disabled = true;
	
	if (document.lanCfg.lanDhcpType.options.selectedIndex == 1)
	{
		document.getElementById("start").style.visibility = "visible";
		document.getElementById("start").style.display = display_on();
		document.lanCfg.dhcpStart.disabled = false;
		document.getElementById("end").style.visibility = "visible";
		document.getElementById("end").style.display = display_on();
		document.lanCfg.dhcpEnd.disabled = false;
		document.getElementById("mask").style.visibility = "visible";
		document.getElementById("mask").style.display = "none";
		document.lanCfg.dhcpMask.disabled = false;
		document.getElementById("pridns").style.visibility = "visible";
		document.getElementById("pridns").style.display = "none";
		document.lanCfg.dhcpPriDns.disabled = false;
		document.getElementById("secdns").style.visibility = "visible";
		document.getElementById("secdns").style.display = "none";
		document.lanCfg.dhcpSecDns.disabled = false;
		document.getElementById("gateway").style.visibility = "visible";
		document.getElementById("gateway").style.display = "none";
		document.lanCfg.dhcpGateway.disabled = false;
		document.getElementById("lease").style.visibility = "visible";
		document.getElementById("lease").style.display = "none";
		document.lanCfg.dhcpLease.disabled = false;
	}
}
function initValue()
{
	var opmode = "<% getCfgZero(1, "OperationMode"); %>";
	var dhcp = <% getCfgZero(1, "dhcpEnabled"); %>;
	var stp = <% getCfgZero(1, "stpEnabled"); %>;
	var lltd = <% getCfgZero(1, "lltdEnabled"); %>;
	var igmp = <% getCfgZero(1, "igmpEnabled"); %>;
	//var upnp = <% getCfgZero(1, "upnpEnabled"); %>;
	var radvd = <% getCfgZero(1, "radvdEnabled"); %>;
	var pppoe = <% getCfgZero(1, "pppoeREnabled"); %>;
	var dns = <% getCfgZero(1, "dnsPEnabled"); %>;
	var wan = "<% getCfgZero(1, "wanConnectionMode"); %>";
	var lltdb = "<% getLltdBuilt(); %>";
	var igmpb = "<% getIgmpProxyBuilt(); %>";
	var upnpb = "<% getUpnpBuilt(); %>";
	var radvdb = "<% getRadvdBuilt(); %>";
	var pppoeb = "<% getPppoeRelayBuilt(); %>";
	var dnsp = "<% getDnsmasqBuilt(); %>";
	if (lan2 == "1")
	{
		var lan2_ip = '<% getCfgGeneral(1, "lan2_ipaddr"); %>';
		var lan2_nm = '<% getCfgGeneral(1, "lan2_netmask"); %>';

		document.lanCfg.lan2enabled[0].checked = true;
		document.lanCfg.lan2Ip.disabled = false;
		document.lanCfg.lan2Ip.value = lan2_ip;
		document.lanCfg.lan2Netmask.disabled = false;
		document.lanCfg.lan2Netmask.value = lan2_nm;
	}
	else
	{
		document.lanCfg.lan2enabled[1].checked = true;
		document.lanCfg.lan2Ip.disabled = true;
		document.lanCfg.lan2Netmask.disabled = true;
	}

	document.lanCfg.lanDhcpType.options.selectedIndex = 1*dhcp;
	dhcpTypeSwitch();
	document.lanCfg.stpEnbl.options.selectedIndex = 1*stp;
	document.lanCfg.lltdEnbl.options.selectedIndex = 1*lltd;
//	document.lanCfg.igmpEnbl.options.selectedIndex = 1*igmp;
	document.lanCfg.upnpEnbl.options.selectedIndex = 1*upnp;
//	document.lanCfg.radvdEnbl.options.selectedIndex = 1*radvd;
//	document.lanCfg.pppoeREnbl.options.selectedIndex = 1*pppoe;
//	document.lanCfg.dnspEnbl.options.selectedIndex = 1*dns;

	//gateway, dns only allow to configure at bridge mode
	if (opmode != "0") {
		document.getElementById("brGateway").style.visibility = "hidden";
		document.getElementById("brGateway").style.display = "none";
		document.getElementById("brPriDns").style.visibility = "hidden";
		document.getElementById("brPriDns").style.display = "none";
		document.getElementById("brSecDns").style.visibility = "hidden";
		document.getElementById("brSecDns").style.display = "none";
	}

//	if (wan == "PPPOE" || wan == "L2TP" || wan == "PPTP") {
//		document.getElementById("igmpProxy").style.visibility = "hidden";
//		document.getElementById("igmpProxy").style.display = "none";
//	}
//	else {
//		document.getElementById("igmpProxy").style.visibility = "visible";
//		document.getElementById("igmpProxy").style.display = display_on();
//	}

	if (lltdb == "0") {
		document.getElementById("lltd").style.visibility = "hidden";
		document.getElementById("lltd").style.display = "none";
		document.lanCfg.lltdEnbl.options.selectedIndex = 0;
	}
//	if (igmpb == "0") {
//		document.getElementById("igmpProxy").style.visibility = "hidden";
//		document.getElementById("igmpProxy").style.display = "none";
//		document.lanCfg.igmpEnbl.options.selectedIndex = 0;
//	}
	if (upnpb == "0") {
		document.getElementById("upnp").style.visibility = "hidden";
		document.getElementById("upnp").style.display = "none";
		document.lanCfg.upnpEnbl.options.selectedIndex = 0;
	}
//	if (radvdb == "0") {
//		document.getElementById("radvd").style.visibility = "hidden";
//		document.getElementById("radvd").style.display = "none";
//		document.lanCfg.radvdEnbl.options.selectedIndex = 0;
//	}
//	if (pppoeb == "0") {
//		document.getElementById("pppoerelay").style.visibility = "hidden";
//		document.getElementById("pppoerelay").style.display = "none";
//		document.lanCfg.pppoeREnbl.options.selectedIndex = 0;
//	}
//	if (dnsp == "0") {
//		document.getElementById("dnsproxy").style.visibility = "hidden";
//		document.getElementById("dnsproxy").style.display = "none";
//		document.lanCfg.dnspEnbl.options.selectedIndex = 0;
//	}
		document.getElementById("lan2en").style.visibility = "hidden";
		document.getElementById("lan2en").style.display = "none";
		document.getElementById("lan2ip").style.visibility = "hidden";
		document.getElementById("lan2ip").style.display = "none";
		document.getElementById("lan2mask").style.visibility = "hidden";
		document.getElementById("lan2mask").style.display = "none";
	document.getElementById("11d").style.visibility = "hidden";
	document.getElementById("11d").style.display = "none";
	document.getElementById("lltd").style.visibility = "hidden";
	document.getElementById("lltd").style.display = "none";
//	document.getElementById("igmpProxy").style.visibility = "hidden";
//	document.getElementById("igmpProxy").style.display = "none";
	//document.getElementById("upnp").style.visibility = "hidden";
	//document.getElementById("upnp").style.display = "none";
//	document.getElementById("radvd").style.visibility = "hidden";
//	document.getElementById("radvd").style.display = "none";
//	document.getElementById("pppoerelay").style.visibility = "hidden";
//	document.getElementById("pppoerelay").style.display = "none";
//	document.getElementById("dnsproxy").style.visibility = "hidden";
//	document.getElementById("dnsproxy").style.display = "none";
}


function moreLanState()
{
	var wan = "<% getCfgZero(1, "wanConnectionMode"); %>";
	if(document.lanCfg.moreLan.checked)
	{
		document.getElementById("11d").style.visibility = "visible";
		document.getElementById("11d").style.display = display_on();
//		if (wan == "PPPOE" || wan == "L2TP" || wan == "PPTP")
//		{
//			document.getElementById("igmpProxy").style.visibility = "hidden";
//			document.getElementById("igmpProxy").style.display = "none";
//	    }
//		else
//		{
//			document.getElementById("igmpProxy").style.visibility = "visible";
//			document.getElementById("igmpProxy").style.display = display_on();
//		}
		document.getElementById("lan2en").style.visibility = "visible";
		document.getElementById("lan2en").style.display = display_on();
		document.getElementById("lan2ip").style.visibility = "visible";
		document.getElementById("lan2ip").style.display = display_on();
		document.getElementById("lan2mask").style.visibility = "visible";
		document.getElementById("lan2mask").style.display = display_on();
		document.getElementById("lltd").style.visibility = "visible";
		document.getElementById("lltd").style.display = display_on();
		//document.getElementById("upnp").style.visibility = "visible";
		//document.getElementById("upnp").style.display = display_on();
//		document.getElementById("radvd").style.visibility = "visible";
//		document.getElementById("radvd").style.display = display_on();
//		document.getElementById("pppoerelay").style.visibility = "visible";
//		document.getElementById("pppoerelay").style.display = display_on();
//		document.getElementById("dnsproxy").style.visibility = "visible";
//		document.getElementById("dnsproxy").style.display = display_on();
	}
	else
	{
		document.getElementById("lan2en").style.visibility = "hidden";
		document.getElementById("lan2en").style.display = "none";
		document.getElementById("lan2ip").style.visibility = "hidden";
		document.getElementById("lan2ip").style.display = "none";
		document.getElementById("lan2mask").style.visibility = "hidden";
		document.getElementById("lan2mask").style.display = "none";
		document.getElementById("11d").style.visibility = "hidden";
		document.getElementById("11d").style.display = "none";
		document.getElementById("lltd").style.visibility = "hidden";
		document.getElementById("lltd").style.display = "none";
//		document.getElementById("igmpProxy").style.visibility = "hidden";
//		document.getElementById("igmpProxy").style.display = "none";
		//document.getElementById("upnp").style.visibility = "hidden";
		//document.getElementById("upnp").style.display = "none";
//		document.getElementById("radvd").style.visibility = "hidden";
//		document.getElementById("radvd").style.display = "none";
//		document.getElementById("pppoerelay").style.visibility = "hidden";
//		document.getElementById("pppoerelay").style.display = "none";
//		document.getElementById("dnsproxy").style.visibility = "hidden";
//		document.getElementById("dnsproxy").style.display = "none";
	}
}

function lan2_enable_switch()
{
	if (document.lanCfg.lan2enabled[1].checked == true)
	{
		document.lanCfg.lan2Ip.disabled = true;
		document.lanCfg.lan2Netmask.disabled = true;
	}
	else
	{
		document.lanCfg.lan2Ip.disabled = false;
		document.lanCfg.lan2Netmask.disabled = false;
	}
}

function openUpnpTb()
{
window.open("upnpstatus.asp","UpnpStatus");
}
function syncWithHost()
{
	var currentTime = new Date();

	var seconds = currentTime.getSeconds();
	var minutes = currentTime.getMinutes();
	var hours = currentTime.getHours();
	var month = currentTime.getMonth() + 1;
	var day = currentTime.getDate();
	var year = currentTime.getFullYear();

	var seconds_str = " ";
	var minutes_str = " ";
	var hours_str = " ";
	var month_str = " ";
	var day_str = " ";
	var year_str = " ";

	if(seconds < 10)
		seconds_str = "0" + seconds;
	else
		seconds_str = ""+seconds;

	if(minutes < 10)
		minutes_str = "0" + minutes;
	else
		minutes_str = ""+minutes;

	if(hours < 10)
		hours_str = "0" + hours;
	else
		hours_str = ""+hours;

	if(month < 10)
		month_str = "0" + month;
	else
		month_str = ""+month;

	if(day < 10)
		day_str = "0" + day;
	else
		day_str = day;

	var tmp = month_str + day_str + hours_str + minutes_str + year + " ";
	document.lanCfg.synctime.value = tmp;
}
</script>
</head>
<body onLoad="initValue()">
<form method=post name="lanCfg" action="/goform/setLan" onSubmit="return CheckValue()">
<table width="90%" border="0" cellpadding="0" cellspacing="0" align="center">
  <tr>
    <td style="padding-top:30px;">
	   <table width="100%" border="0" cellpadding="0" cellspacing="0">
          <tr>
            <td class="nav">当前路径:KN-WR922 &gt;&gt;LAN 设置 &gt;&gt;LAN 设置 </td>
          </tr>
		  <tr>
		    <td>&nbsp;</td>
		  </tr>
          <tr>
            <td class="space" style="padding-left:10px; padding-right:10px;">&nbsp;&nbsp;&nbsp;本页面用来设置LAN(本地局域网)参数：一般情况下您只需要设置其中的IP地址，子网掩码以及DHCP的地址池就可以了。IP地址为路由器设备的地址，如果您修改了它，那下次登陆就必须用修改后的IP地址进行登陆.<font color="cc0000">注意您的LAN IP地址不能和WAN口的IP地址在同一个网段，否则会导致系统异常</font>。更多帮助信息请点
              <input type="button" class="button5" value="帮助" onclick=popHelp('help.htm#UnitCfg')></td>
          </tr>
		  <tr>
		    <td>&nbsp;</td>
		  </tr>
          <tr>
            <td>
			  <table class="space" width="100%" border="0" cellspacing="0" cellpadding="0">
                <tr>
            	  <td class="titlebg" colspan="2">LAN 设置 </td>
          		</tr>
				<tr>
  					<td class="contentpadding" id="lHostname" width="25%">主机名</td>
  					<td width="75%"><input name="hostname" maxlength=16 value="<% getCfgGeneral(1, "HostName"); %>"></td>
				</tr>
				
				
                <tr>
                  <td width="25%" id="lIp" class="contentpadding">IP 地址:</td>
                  <td width="75%"><input name="lanIp" maxlength=15 value="<% getLanIp(); %>">
                  （必填）
                    <input type="hidden" name="hostname2" maxlength="16" value="<% getCfgGeneral(1, "HostName"); %>" /></td>
                </tr>
                <tr>
                  <td id="lNetmask" class="contentpadding">子网掩码:</td>
                  <td><input name="lanNetmask" maxlength=15 value="<% getLanNetmask(); %>">
                  （必填）</td>
                </tr>
				<tr id="brGateway">
				  <td class="contentpadding" id="lGateway">默认网关</td>
				  <td><input name="lanGateway" maxlength=15 value="<% getWanGateway(); %>"></td>
				</tr>
				<tr id="brPriDns">
				  <td class="contentpadding" id="lPriDns">主 DNS服务器:</td>
				  <td><input name="lanPriDns" maxlength=15 value="<% getDns(1); %>"></td>
				</tr>
				<tr id="brSecDns">
				  <td class="contentpadding" id="lSecDns">次 DNS服务器:</td>
				  <td><input name="lanSecDns" maxlength=15 value="<% getDns(2); %>"></td>
				</tr>
				<tr>
                  <td class="contentpadding" id="lDhcpType">DHCP 方式:</td>
                  <td><select name="lanDhcpType" size="1" onChange="dhcpTypeSwitch();">
      <option value="DISABLE" id="lDhcpTypeD">禁用</option>
      <option value="SERVER" id="lDhcpTypeS">启用</option></select></td>
                </tr>
				<tr id="start">
                  <td class="contentpadding" id="lDhcpStart">DHCP 开始 IP:</td>
                  <td><input name="dhcpStart" maxlength=15 value="<% getCfgGeneral(1, "dhcpStart"); %>">
                  （DHCP为开启时必填）</td>
                </tr>
				<tr id="end">
                  <td class="contentpadding" id="lDhcpEnd">DHCP 结束 IP:</td>
                  <td><input name="dhcpEnd" maxlength=15  value="<% getCfgGeneral(1, "dhcpEnd"); %>">
                  （DHCP为开启时必填）</td>
                </tr>
				<tr id="mask">
                  <td class="contentpadding" id="lDhcpNetmask">DHCP 子网掩码:</td>
                  <td><input name="dhcpMask" maxlength=15 value="<% getCfgGeneral(1, "dhcpMask"); %>"></td>
                </tr>
				<tr id="pridns">
                  <td class="contentpadding" id="lDhcpPriDns">DHCP 主 DNS:</td>
                  <td><input name="dhcpPriDns" maxlength=15 value="<% getCfgGeneral(1, "dhcpPriDns"); %>"></td>
                </tr>
				<tr id="secdns">
                  <td class="contentpadding" id="lDhcpSecDns">DHCP 次 DNS:</td>
                  <td><input name="dhcpSecDns" maxlength=15 value="<% getCfgGeneral(1, "dhcpSecDns"); %>"></td>
                </tr>
				<tr id="gateway">
                  <td class="contentpadding" id="lDhcpGateway">DHCP 默认网关:</td>
                  <td><input name="dhcpGateway" maxlength=15 value="<% getCfgGeneral(1, "dhcpGateway"); %>"></td>
                </tr>
				<tr id="lease">
                  <td class="contentpadding" id="lDhcpLease">DHCP 租约时间:</td>
                 <td><input name="dhcpLease" maxlength=8 value="<% getCfgGeneral(1, "dhcpLease"); %>"></td>
                </tr>
				<tr id="staticlease1">
                  <td class="contentpadding" id="lDhcpStatic1">静态设定:</td>
                  <td><input type=hidden name=dhcpStatic1 value=""> MAC: <input name="dhcpStatic1Mac" maxlength=17 value="<% getCfgNthGeneral(1, "dhcpStatic1", 0); %>"> IP: <input name="dhcpStatic1Ip" maxlength=15 value="<% getCfgNthGeneral(1, "dhcpStatic1", 1); %>"></td>
                </tr>
				<tr id="staticlease2">
                  <td class="contentpadding" id="lDhcpStatic2">静态设定:</td>
                  <td><input type=hidden name=dhcpStatic2 value=""> MAC: <input name="dhcpStatic2Mac" maxlength=17 value="<% getCfgNthGeneral(1, "dhcpStatic2", 0); %>"> IP: <input name="dhcpStatic2Ip" maxlength=15 value="<% getCfgNthGeneral(1, "dhcpStatic2", 1); %>"></td>
                </tr>
				<tr id="staticlease3">
                  <td class="contentpadding" id="lDhcpStatic3">静态设定:</td>
                  <td><input type=hidden name=dhcpStatic3 value=""> MAC: <input name="dhcpStatic3Mac" maxlength=17 value="<% getCfgNthGeneral(1, "dhcpStatic3", 0); %>"> IP: <input name="dhcpStatic3Ip" maxlength=15 value="<% getCfgNthGeneral(1, "dhcpStatic3", 1); %>" /></td>
				</tr>
				<tr id="upnp">
                  <td class="contentpadding" id="lUpnp">UPNP:</td>
                  <td><select name="upnpEnbl" size="1" onChange="upnpChange()">
                    <option value="0" id="lUpnpD">禁用</option>
                    <option value="1" id="lUpnpE">启用</option>
                  </select>                    &nbsp;&nbsp;&nbsp;&nbsp;<input type="button" class="button1" name="upnpTable" value="upnp设备信息" onClick="openUpnpTb()" /></td>
                </tr>
				<tr id="lanAdv"> 
                  <td class="contentpadding" id="lStp">更多选项:</td>
                  <td><input type="checkbox" name="moreLan" value="ON" onClick="moreLanState()" /> (一般情况下,不用进行配置)</td>
                </tr>
<tr id="lan2en">
  <td class="contentpadding" id="lLan2">LAN2</td>
  <td>
    <input type="radio" name="lan2enabled" value="1" onClick="lan2_enable_switch()"><font id="lLan2Enable">启用</font>&nbsp;
    <input type="radio" name="lan2enabled" value="0" onClick="lan2_enable_switch()" checked><font id="lLan2Disable">禁用</font>
  </td>
</tr>
<tr id="lan2ip">
  <td class="contentpadding" id="lLan2Ip">LAN2 IP地址</td>
  <td><input name="lan2Ip" maxlength=15 value=""></td>
</tr>
<tr id="lan2mask">
  <td class="contentpadding" id="lLan2Netmask">LAN2 子网掩码</td>
  <td><input name="lan2Netmask" maxlength=15 value=""></td>
</tr>

				<tr id="11d"> 
                  <td class="contentpadding" id="lStp">802.1d 生成树:</td>
                  <td><select name="stpEnbl" size="1">
      <option value="0" id="lStpD">禁用</option>
      <option value="1" id="lStpE">启用</option></select></td>
                </tr>
				<tr id="lltd"> 
                  <td class="contentpadding" id="lLltd">LLTD:</td>
                  <td><select name="lltdEnbl" size="1">
                    <option value="0">禁用</option>
                    <option value="1">启用</option></select></td>
                </tr>
<!--
				<tr id="igmpProxy">
                  <td class="contentpadding" id="lIgmpp">IGMP 代理:</td>
                  <td><select name="igmpEnbl" size="1">
      <option value="0" id="lIgmppD">禁用</option>
      <option value="1" id="lIgmppE">启用</option></select></td>
                </tr>
				
				<tr id="radvd">
				  <td class="contentpadding" id="lRadvd">路由器通告:</td>
				  <td>
					<select name="radvdEnbl" size="1">
					  <option value="0" id="lRadvdD">禁用</option>
					  <option value="1" id="lRadvdE">启用</option></select></td>
				</tr>
				<tr id="pppoerelay">
                  <td class="contentpadding" id="lPppoer">PPPOE中继:</td>
                  <td><select name="pppoeREnbl" size="1">
      <option value="0" id="lPppoerD">禁用</option>
      <option value="1" id="lPppoerE">启用</option></select></td>
                </tr>
				<tr id="dnsproxy">
				  <td class="contentpadding" id="lDnsp">DNS 代理:</td>
				  <td><select name="dnspEnbl" size="1">
					  <option value="0" id="lDnspD">禁用</option>
					  <option value="1" id="lDnspE">启用</option></select></td>
				</tr>
-->
              </table>			
			</td>
          </tr>
		  <tr><td>&nbsp;</td></tr>
	  	  <tr>
        	<td class="contentpadding"><input type="hidden" value="" name="synctime"><input type="hidden" value="/int_lan.asp" name="submit-url"><input type="submit" class="button"  value="提 交" />&nbsp;&nbsp;<input type="reset" class="button"  value="取 消" onClick="window.location.reload()"></td>
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
