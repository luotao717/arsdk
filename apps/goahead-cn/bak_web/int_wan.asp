<html>
<head>
<title>WR922</title>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312" />
<link href="images/kingnet.css" type="text/css" rel="stylesheet">
<script type="text/javascript" src="common.js"> </script>
<SCRIPT src="helpScript.js"></SCRIPT>
<script language="JavaScript">
var Times = 0; 
var flag = true;
//var pppoe_plus = <% getCfgZero(1, "pppoe_plus_enable"); %>;
var pppoe_specific = <% getCfgZero(1, "pppoe_specific"); %>;

function leavePage() 
{
	if(flag)
	{
		Times += 1;
		document.getElementById("msg").innerHTML= Times +" 秒";
		setTimeout("leavePage()", 1000);
	}
	else
	{
		document.getElementById("msg").style.visibility = "hidden";
		document.getElementById("msg").style.display="none";
	}
}
</script>
<script language="JavaScript" type="text/javascript">
function style_display_on()
{
	if (window.ActiveXObject)
	{ // IE
		return "block";
	}
	else if (window.XMLHttpRequest)
	{ // Mozilla, Safari,...
		return "table-row";
	}
}
</script>
<script language="JavaScript" type="text/javascript">
var strInfo;
var http_request = false;
function connectAutoCheckSubmit()
{
    http_request = false;
	
 	document.getElementById("autoCheckInfo").value ="正在检测，请稍候...";
	document.wanCfg.connectAutoCheck.disabled=true;
	Times = 0;
	flag=true;	
	
	document.getElementById("msg").style.visibility = "visible";
	document.getElementById("msg").style.display = style_display_on();
	setTimeout('leavePage()', 1000);
	
	if (window.XMLHttpRequest) { // Mozilla, Safari,...
        http_request = new XMLHttpRequest();
        if (http_request.overrideMimeType) {
            http_request.overrideMimeType('text/xml');
        }
    } else if (window.ActiveXObject) { // IE
        try {
            http_request = new ActiveXObject("Msxml2.XMLHTTP");
        } catch (e) {
            try {
            http_request = new ActiveXObject("Microsoft.XMLHTTP");
            } catch (e) {}
        }
    }
    if (!http_request) {
        alert('Cannot create an XMLHTTP instance');
        return false;
    }
    http_request.onreadystatechange = doFillCheckInfo;

    http_request.open('POST', '/goform/formConnectAutoCheck', true);
    http_request.send('n\a');
	
}

function doFillCheckInfo()
{
    if (http_request.readyState == 4) 
	{
		if (http_request.status == 200) 
		{
			document.getElementById("autoCheckInfo").value = http_request.responseText;
			strInfo=document.getElementById("autoCheckInfo").value;
			//strInfo=http_request.responseText.substring(5);
			//alert(strInfo);
			if(strInfo=="pppoe")
			{
				alert("您的WAN接入类型为PPPOE(ADSL)！");
				flag=false;
				//alert("Connect type is pppoe!");
				//document.tcpip.pppConnectType.options[2].selected;
				document.wanCfg.connectionType.options[2].selected=true;
				connectionTypeSwitch();
			}
			else if(strInfo=="linkN")
			{
				alert("检测错误，请确认您的物理线路是否连接好，然后再试一次！");
				flag=false;
				//alert("Cabel check error,please make sure the cabel connect now,then try a again!");
				//document.wanCfg.connectionType.options[1].selected=true;
				//document.getElementByName("wanType").options[1].selected;
				//connectionTypeSwitch();
				//alert("111");
			}
			else if(strInfo=="dhcpc")
			{
				alert("您的WAN接入类型是DHCP获取方式！");
				flag=false;
				//alert("Connect type is dhcp!");
				document.wanCfg.connectionType.options[1].selected=true;
				//document.getElementByName("wanType").options[1].selected;
				connectionTypeSwitch();
				//alert("111");
			}
			else
			{
				alert("您的WAN接入类型可能是静态IP方式！");
				flag=false;
				//alert("May be is static ip!");
				//document.tcpip.pppConnectType.options[0[.selected;
				document.wanCfg.connectionType.options[0].selected=true;
				connectionTypeSwitch();
			}
		}
		else 
		{
			alert("没有获取到MAC地址！");
			flag=false;
			//alert("Can\'t get the mac address.");
		}
		document.wanCfg.connectAutoCheck.disabled=false;
	}
	
}
//var http_request = false;
function macCloneMacFillSubmit()
{
    http_request = false;
    if (window.XMLHttpRequest) { // Mozilla, Safari,...
        http_request = new XMLHttpRequest();
        if (http_request.overrideMimeType) {
            http_request.overrideMimeType('text/xml');
        }
    } else if (window.ActiveXObject) { // IE
        try {
            http_request = new ActiveXObject("Msxml2.XMLHTTP");
        } catch (e) {
            try {
            http_request = new ActiveXObject("Microsoft.XMLHTTP");
            } catch (e) {}
        }
    }
    if (!http_request) {
        alert('Cannot create an XMLHTTP instance');
        return false;
    }
    http_request.onreadystatechange = doFillMyMAC;

    http_request.open('POST', '/goform/getMyMAC', true);
    http_request.send('n\a');
}

function doFillMyMAC()
{
    if (http_request.readyState == 4) {
		if (http_request.status == 200) {
			document.getElementById("macCloneMac").value = http_request.responseText;
		} else {
			alert("没有获取到MAC地址.");
		}
	}
}


function macCloneSwitch()
{
	if (document.wanCfg.macCloneEnbl.options.selectedIndex == 1) {
		document.getElementById("macCloneMacRow").style.visibility = "visible";
		document.getElementById("macCloneMacRow").style.display = style_display_on();
	}
	else {
		document.getElementById("macCloneMacRow").style.visibility = "hidden";
		document.getElementById("macCloneMacRow").style.display = "none";
	}
}

var pppConnectStatus=0;
function setPPPConnected()
{
   pppConnectStatus = 1;
}
function pppTypeSelection()
{
  if ( document.wanCfg.pppConnectType.selectedIndex == 1) {
  	if (pppConnectStatus==0) {
  		enableButton(document.wanCfg.pppConnect);
		disableButton(document.wanCfg.pppDisconnect);
	}
	else {
 		disableButton(document.wanCfg.pppConnect);
		enableButton(document.wanCfg.pppDisconnect);
	}
	disableTextField(document.wanCfg.pppIdleTime);
	//enableTextField(document.wanCfg.pppIdleTime);
  }
  else {
	disableButton(document.wanCfg.pppConnect);
	disableButton(document.wanCfg.pppDisconnect);
	if (document.wanCfg.pppConnectType.selectedIndex == 2)
		enableTextField(document.wanCfg.pppIdleTime);
	else
		disableTextField(document.wanCfg.pppIdleTime);
  }
  //added by maocw 2005-11-30
  //if ( document.tcpip.pppConnectType.selectedIndex == 0)
  	//{
  	//if (pppConnectStatus==1) 
		//{
  		//disableButton(document.tcpip.pppConnect);
		//enableButton(document.tcpip.pppDisconnect);
		//}
  	//}
  //ended by maocw 2005-11-30
}
function pppConnectClick(connect)
{
  //if (document.wanCfg.pppConnectType.selectedIndex == 1 && pppConnectStatus==connect) {
      //if (document.wanCfg.pppUserName.value=="") {
          //alert('PPP user name cannot be empty!');
	  //document.wanCfg.pppUserName.value = document.wanCfg.pppUserName.defaultValue;
	 // document.wanCfg.pppUserName.focus();
	 // return false;
     // }
     // if (document.wanCfg.pppPassword.value=="") {
          //alert('PPP password cannot be empty!');
	 // document.wanCfg.pppPassword.value = document.wanCfg.pppPassword.defaultValue;
	  //document.wanCfg.pppPassword.focus();
	  //return false;
     // }
      //return true;
 // }
 // if (document.wanCfg.pppConnectType.selectedIndex == 0 && pppConnectStatus==connect) 
  	//{
	// return true;
  	//}
  //return false;
   return true;
  
}


function connectionTypeSwitch()
{
	document.getElementById("static").style.visibility = "hidden";
	document.getElementById("static").style.display = "none";
	document.getElementById("dhcp").style.visibility = "hidden";
	document.getElementById("dhcp").style.display = "none";
	document.getElementById("pppoe").style.visibility = "hidden";
	document.getElementById("pppoe").style.display = "none";
	document.getElementById("l2tp").style.visibility = "hidden";
	document.getElementById("l2tp").style.display = "none";
	document.getElementById("pptp").style.visibility = "hidden";
	document.getElementById("pptp").style.display = "none";
	document.getElementById("dhcpplus").style.visibility = "hidden";
	document.getElementById("dhcpplus").style.display = "none";
	if (document.wanCfg.connectionType.options.selectedIndex == 0) {
		document.getElementById("static").style.visibility = "visible";
		document.getElementById("static").style.display = style_display_on();
	}
	else if (document.wanCfg.connectionType.options.selectedIndex == 1) {
		document.getElementById("dhcp").style.visibility = "visible";
		document.getElementById("dhcp").style.display = style_display_on();
	}
	else if (document.wanCfg.connectionType.options.selectedIndex == 2) {
		document.getElementById("pppoe").style.visibility = "visible";
		document.getElementById("pppoe").style.display = style_display_on();
	}
//	else if (document.wanCfg.connectionType.options.selectedIndex == 3) {
//		document.getElementById("l2tp").style.visibility = "visible";
//		document.getElementById("l2tp").style.display = style_display_on();
//		l2tpOPModeSwitch(); //added by 3.1
//	}
//	else if (document.wanCfg.connectionType.options.selectedIndex == 3) {
//		document.getElementById("pptp").style.visibility = "visible";
//		document.getElementById("pptp").style.display = style_display_on();
//		pptpOPModeSwitch(); //added by 3.1
//	}
	else if (document.wanCfg.connectionType.options.selectedIndex == 3) {
		document.getElementById("dhcpplus").style.visibility = "visible";
		document.getElementById("dhcpplus").style.display = style_display_on();
	}
	else {
		document.getElementById("static").style.visibility = "visible";
		document.getElementById("static").style.display = style_display_on();
	}
	if( pppoe_specific == 2 )
			document.wanCfg.specific.options.selectedIndex = 2;
	else if (pppoe_specific == 1 )
			document.wanCfg.specific.options.selectedIndex = 1;
	else if (pppoe_specific == 3)
			document.wanCfg.specific.options.selectedIndex = 3;
	else
		document.wanCfg.specific.options.selectedIndex = 0;
}

function style_display_on()
{
	if (window.ActiveXObject) { // IE
		return "block";
	}
	else if (window.XMLHttpRequest) { // Mozilla, Safari,...
		return "table-row";
	}
}
function l2tpModeSwitch()
{
	if (document.wanCfg.l2tpMode.selectedIndex == 0) {
		document.getElementById("l2tpIp").style.visibility = "visible";
		document.getElementById("l2tpIp").style.display = style_display_on();
		document.getElementById("l2tpNetmask").style.visibility = "visible";
		document.getElementById("l2tpNetmask").style.display = style_display_on();
		document.getElementById("l2tpGateway").style.visibility = "visible";
		document.getElementById("l2tpGateway").style.display = style_display_on();
	}
	else {
		document.getElementById("l2tpIp").style.visibility = "hidden";
		document.getElementById("l2tpIp").style.display = "none";
		document.getElementById("l2tpNetmask").style.visibility = "hidden";
		document.getElementById("l2tpNetmask").style.display = "none";
		document.getElementById("l2tpGateway").style.visibility = "hidden";
		document.getElementById("l2tpGateway").style.display = "none";
	}
}

function pptpModeSwitch()
{
	if (document.wanCfg.pptpMode.selectedIndex == 0) {
		document.getElementById("pptpIp").style.visibility = "visible";
		document.getElementById("pptpIp").style.display = style_display_on();
		document.getElementById("pptpNetmask").style.visibility = "visible";
		document.getElementById("pptpNetmask").style.display = style_display_on();
		document.getElementById("pptpGateway").style.visibility = "visible";
		document.getElementById("pptpGateway").style.display = style_display_on();
	}
	else {
		document.getElementById("pptpIp").style.visibility = "hidden";
		document.getElementById("pptpIp").style.display = "none";
		document.getElementById("pptpNetmask").style.visibility = "hidden";
		document.getElementById("pptpNetmask").style.display = "none";
		document.getElementById("pptpGateway").style.visibility = "hidden";
		document.getElementById("pptpGateway").style.display = "none";
	}
}
//add pptp l2tp by 3.1
function l2tpOPModeSwitch()
{
	document.wanCfg.l2tpRedialPeriod.disabled = true;
	document.wanCfg.l2tpIdleTime.disabled = true;
	if (document.wanCfg.l2tpOPMode.options.selectedIndex == 0) 
		document.wanCfg.l2tpRedialPeriod.disabled = false;
	else if (document.wanCfg.l2tpOPMode.options.selectedIndex == 1)
		document.wanCfg.l2tpIdleTime.disabled = false;
}
function pptpOPModeSwitch()
{
	document.wanCfg.pptpRedialPeriod.disabled = true;
	document.wanCfg.pptpIdleTime.disabled = true;
	if (document.wanCfg.pptpOPMode.options.selectedIndex == 0) 
		document.wanCfg.pptpRedialPeriod.disabled = false;
	else if (document.wanCfg.pptpOPMode.options.selectedIndex == 1)
		document.wanCfg.pptpIdleTime.disabled = false;
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
		alert("IP地址不能为空.");
		field.value = field.defaultValue;
		field.focus();
		return false;
	}

	if (isAllNum(field.value) == 0) {
		alert('必须在 [0-9] 范围.');
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
			alert('IP地址格式错误.');
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
			alert('IP地址格式错误.');
			field.value = field.defaultValue;
			field.focus();
			return false;
		}
	}
	return true;
}

function CheckValue()
{
	if (document.wanCfg.connectionType.selectedIndex == 0) {      //STATIC
		if (!checkIpAddr(document.wanCfg.staticIp, false))
			return false;
		if (!checkIpAddr(document.wanCfg.staticNetmask, true))
			return false;
		if (document.wanCfg.staticGateway.value != "")
			if (!checkIpAddr(document.wanCfg.staticGateway, false))
				return false;
		if (document.wanCfg.staticPriDns.value != "")
			if (!checkIpAddr(document.wanCfg.staticPriDns, false))
				return false;
		if (document.wanCfg.staticSecDns.value != "")
			if (!checkIpAddr(document.wanCfg.staticSecDns, false))
				return false;
				
			if (document.wanCfg.staticGateway.value!="0.0.0.0") {
		if ( checkIpAddr1(document.wanCfg.staticGateway, 'Error gateway! ') == false )
	    		return false;
	    	if ( !checkSubnet(document.wanCfg.staticIp.value,document.wanCfg.staticNetmask.value,document.wanCfg.staticGateway.value)) {
			alert('网关和所设的IP地址不在同一个子网内.');
			document.wanCfg.staticGateway.value = document.wanCfg.staticGateway.defaultValue;
			document.wanCfg.staticGateway.focus();
			return false;
      		}
	}
		
				
	}
	else if (document.wanCfg.connectionType.selectedIndex == 1) { //DHCP
	}
	else if (document.wanCfg.connectionType.selectedIndex == 2){ //PPPOE
		if (1*document.wanCfg.pppoeMtu.value < 66 || 1*document.wanCfg.pppoeMtu.value > 65535) {
			alert("MTU 超出范围 66 ~ 65535!");
			return false;
		}
		if (1*document.wanCfg.pppoeMru.value < 66 || 1*document.wanCfg.pppoeMru.value > 65535) {
			alert("MRU 超出范围 66 ~ 65535!");
			return false;
		}
		if (document.wanCfg.pppoePass.value != document.wanCfg.pppoePass2.value) {
			alert("密码不匹配!");
			document.wanCfg.pppoePass2.focus();
			return false;
		}
		if (document.wanCfg.pppIdleTime.value == "" )
	{
		alert('空闲时间不能为空');
		document.wanCfg.pppIdleTime.focus();
		document.wanCfg.pppIdleTime.select();
		return false;
	}
	if (isNaN(document.wanCfg.pppIdleTime.value) || document.wanCfg.pppIdleTime.value < 1 || document.wanCfg.pppIdleTime.value > 1000)
	{
		alert('无效的空闲时间');
		document.wanCfg.pppIdleTime.focus();
//		document.wanCfg.pppIdleTime.select();
		return false;
	}
	}
/*
	else if (document.wanCfg.connectionType.selectedIndex == 3) { //L2TP
		if (document.wanCfg.l2tpOPMode.options.selectedIndex == 0)
		{
			if (document.wanCfg.l2tpRedialPeriod.value == "")
			{
				alert("Please specify Redial Period");
				document.wanCfg.l2tpRedialPeriod.focus();
				document.wanCfg.l2tpRedialPeriod.select();
				return false;
			}
		}
		else if (document.wanCfg.l2tpOPMode.options.selectedIndex == 1)
		{
			if (document.wanCfg.l2tpIdleTime.value == "")
			{
				alert("Please specify Idle Time");
				document.wanCfg.l2tpIdleTime.focus();
				document.wanCfg.l2tpIdleTime.select();
				return false;
			}
		}
	}
	else if (document.wanCfg.connectionType.selectedIndex == 4) { //PPTP
		if (1*document.wanCfg.pptpMtu.value < 66 || 1*document.wanCfg.pptpMtu.value > 65535) {
			alert("MTU 超出范围 66 ~ 65535!");
			return false;
		}
		if (document.wanCfg.pptpPass.value != document.wanCfg.pptpPass2.value) {
			alert("密码不匹配!");
			return false;
		}
		if (!checkIpAddr(document.wanCfg.pptpServer, false))
			return false;
		if (document.wanCfg.pptpMode.selectedIndex == 0) {
			if (!checkIpAddr(document.wanCfg.pptpIp, false))
				return false;
			if (!checkIpAddr(document.wanCfg.pptpNetmask, true))
				return false;
			if (!checkIpAddr(document.wanCfg.pptpGateway, false))
				return false;
		}
		if (document.wanCfg.pptpOPMode.options.selectedIndex == 0)
		{
			if (document.wanCfg.pptpRedialPeriod.value == "")
			{
				alert("Please specify Redial Period");
				document.wanCfg.pptpRedialPeriod.focus();
				document.wanCfg.pptpRedialPeriod.select();
				return false;
			}
		}
		else if(document.wanCfg.pptpOPMode.options.selectedIndex == 1)
		{
			if (document.wanCfg.pptpIdleTime.value == "")
			{
				alert("Please specify Idle Time");
				document.wanCfg.pptpIdleTime.focus();
				document.wanCfg.pptpIdleTime.select();
				return false;
			}
		}
	}
	*/
	else if (document.wanCfg.connectionType.selectedIndex == 3) { //DHCP+
		if (document.wanCfg.dhcppuser.value == "" )
		{
			alert('dhcp+用户名不能为空');
			document.wanCfg.dhcppuser.focus();
			return false;
		}
		if (document.wanCfg.dhpcppassword.value == "" )
		{
			alert('dhcp+密码不能为空');
			document.wanCfg.dhpcppassword.focus();
			return false;
		}
	}
	else
		return false;
		
	if (document.wanCfg.macCloneEnbl.options.selectedIndex == 1) {
		var re = /[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}/;
		if (document.wanCfg.macCloneMac.value.length == 0) {
			alert("MAC 地址不能为空!");
			document.wanCfg.macCloneMac.focus();
			return false;
		}
		if (!re.test(document.wanCfg.macCloneMac.value)) {
			alert("请输入正确MAC地址格式! (XX:XX:XX:XX:XX:XX)");
			document.wanCfg.macCloneMac.focus();
			return false;
		}
	}
	syncWithHost();
	return true;
}

function initValue()
{
	var mode = "<% getCfgGeneral(1, "wanConnectionMode"); %>";
	var pptpMode = <% getCfgZero(1, "wan_pptp_mode"); %>;
	var clone = <% getCfgZero(1, "macCloneEnabled"); %>;


	if (mode == "STATIC") {
		document.wanCfg.connectionType.options.selectedIndex = 0;
	}
	else if (mode == "DHCP") {
		document.wanCfg.connectionType.options.selectedIndex = 1;
	}
	else if (mode == "PPPOE") {
		document.wanCfg.connectionType.options.selectedIndex = 2;
	}
/*
	else if (mode == "L2TP") {
		var l2tp_opmode = "<% getCfgGeneral(1, "wan_l2tp_opmode"); %>";
		var l2tp_optime = "<% getCfgGeneral(1, "wan_l2tp_optime"); %>";
		
		document.wanCfg.connectionType.options.selectedIndex = 3;
		if (l2tp_opmode == "Manual")
		{
			document.wanCfg.l2tpOPMode.options.selectedIndex = 2;
		}
		else if (l2tp_opmode == "OnDemand")
		{
			document.wanCfg.l2tpOPMode.options.selectedIndex = 1;
			if (l2tp_optime != "")
				document.wanCfg.l2tpIdleTime.value = l2tp_optime;
		}
		else if (l2tp_opmode == "KeepAlive")
		{
			document.wanCfg.l2tpOPMode.options.selectedIndex = 0;
			if (l2tp_optime != "")
				document.wanCfg.l2tpRedialPeriod.value = l2tp_optime;
		}
		l2tpOPModeSwitch();
	}
	else if (mode == "PPTP") {
		var pptp_opmode = "<% getCfgGeneral(1, "wan_pptp_opmode"); %>";
		var pptp_optime = "<% getCfgGeneral(1, "wan_pptp_optime"); %>";

		document.wanCfg.connectionType.options.selectedIndex = 4;
		document.wanCfg.pptpMode.options.selectedIndex = 1*pptpMode;
		pptpModeSwitch();
		if (pptp_opmode == "Manual")
		{
			document.wanCfg.pptpOPMode.options.selectedIndex = 2;
			if (pptp_optime != "")
				document.wanCfg.pptpIdleTime.value = pptp_optime;
		}
		if (pptp_opmode == "OnDemand")
		{
			document.wanCfg.pptpOPMode.options.selectedIndex = 1;
			if (pptp_optime != "")
				document.wanCfg.pptpIdleTime.value = pptp_optime;
		}
		else if (pptp_opmode == "KeepAlive")
		{
			document.wanCfg.pptpOPMode.options.selectedIndex = 0;
			if (pptp_optime != "")
				document.wanCfg.pptpRedialPeriod.value = pptp_optime;
		}
		pptpOPModeSwitch();
	}
	*/
	else if (mode == "DHCPPLUS") {
		document.wanCfg.connectionType.options.selectedIndex = 3;
	}
	else {
		document.wanCfg.connectionType.options.selectedIndex = 0;
	}
	connectionTypeSwitch();
	if (clone == 1)
		document.wanCfg.macCloneEnbl.options.selectedIndex = 1;
	else
		document.wanCfg.macCloneEnbl.options.selectedIndex = 0;
	macCloneSwitch();
	pppTypeSelection();
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
	document.wanCfg.synctime.value = tmp;
}
</script>
</head>
<body onLoad="initValue()">
<form method=post name="wanCfg" action="/goform/setWan" onSubmit="return CheckValue()">
<table width="90%" border="0" cellpadding="0" cellspacing="0" align="center">
  <tr>
    <td style="padding-top:30px;">
	   <table width="100%" border="0" cellpadding="0" cellspacing="0">
          <tr>
            <td class="nav">当前路径:KN-WR922 &gt;&gt;WAN 设置 &gt;&gt;WAN 设置 </td>
          </tr>
		  <tr>
		    <td>&nbsp;</td>
		  </tr>
          <tr>
            <td class="space" style="padding-left:10px; padding-right:10px;">&nbsp;&nbsp;&nbsp;本页面用来设置从WAN 口连接到互联网(Internet)的相关参数. 我们提供了五种接入互联网的方式供您选择，您必须选择其中的一种接入方式，通常ISP(Internet 提供商)会告诉您所采用的接入方式如果您不清楚的话，可以向您的ISP咨询，也可以用我们的自动侦测功能进行侦测。<font color="cc0000">注意：当您点击自动侦测按钮进行自动侦测后，请务必不要关闭本页面直到检测结果出来，否则会造成假死机状态，整个检测过程不会超过两分钟，请耐心等候！</font>要了解更多的帮助信息请点<input type="button" class="button5" value="帮助" onclick=popHelp('help.htm#ConnToISP')></td>
          </tr>
		  <tr>
		    <td>&nbsp;</td>
		  </tr>
          <tr>
            <td>
			  <table class="space" width="100%" border="0" cellspacing="0" cellpadding="0">
                <tr>
                   <td class="titlebg" colspan="2">WAN 设置 </td>
                </tr>
                <tr>
                  <td width="25%" class="contentpadding">WAN 连接类型:</td>
                  <td width="75%"><select name="connectionType" size="1" onChange="connectionTypeSwitch();">
      <option value="STATIC">静态IP(fixed IP)</option>
      <option value="DHCP">DHCP (Auto Config)</option>
      <option value="PPPOE">PPPOE (ADSL)</option>
<!--      
      <option value="L2TP">L2TP</option>     
      <option value="PPTP">PPTP</option>
-->
	<option value="DHCPPLUS">DHCP+</option>
      </select>&nbsp;&nbsp;<input class="button2" type="button" name="connectAutoCheck" id="connectAutoCheck" value="自动侦测" onClick="connectAutoCheckSubmit();" >&nbsp;<input type="text" name="autoCheckInfo" size="18" maxlength="25" value=""><div style="font-size:16px;font-weight:bold;color:#FF0000" id="msg"> </div></td>
                </tr>
              </table>			
			</td>
          </tr>
          <tr id="static">
            <td>
			  <table class="space" width="100%" border="0" cellspacing="0" cellpadding="0">
       		    <tr>
                  <td colspan="2" class="titlebg">静态IP模式</td>
                </tr>
				<tr>
                  <td class="contentpadding">IP 地址:</td>
                  <td><input name="staticIp" maxlength=15 value="<% getCfgGeneral(1, "wan_static_ip"); %>"> (必填)</td>
                </tr>
                <tr>
                  <td class="contentpadding">子网掩码:</td>
                  <td><input name="staticNetmask" maxlength=15 value="<% getCfgGeneral(1, "wan_static_mask"); %>"> (必填)</td>
                </tr>
				<tr>
                  <td class="contentpadding">默认网关:</td>
                  <td><input name="staticGateway" maxlength=15 value="<% getCfgGeneral(1, "wan_static_gateway"); %>"> (必填)</td>
                </tr>
				<tr>
                  <td class="contentpadding">MTU:</td>
                  <td><input name="statiMtu" maxlength=4 value="<% getCfgGeneral(1, "wan_static_mtu"); %>">                   
                     (如非必要，请勿更改)</td>
                </tr>
				<tr>
                  <td width="25%" class="contentpadding">首选DNS服务器:</td>
                  <td width="75%"><input name="staticPriDns" maxlength=15 value="<% getCfgGeneral(1, "wan_static_dns1"); %>"> (必填)</td>
                </tr>
				<tr>
                  <td width="25%" class="contentpadding">备用DNS服务器:</td>
                  <td width="75%"><input name="staticSecDns" maxlength=15 value="<% getCfgGeneral(1, "wan_static_dns2"); %>">
                  (选填) </td>
                </tr>
              </table>			
			</td>
          </tr>
          <tr id=dhcp>
            <td>
			  <table class="space" width="100%" border="0" cellspacing="0" cellpadding="0">
              	<tr>
            	  <td colspan="2" class="titlebg">DHCP 模式</td>
          		</tr>
		  		<tr>
                  <td width="25%" class="contentpadding">当前状态:</td>
                  <td width="75%"><% getWanDhcpStatus(); %></td>
                </tr>
<!--                <tr>
                  <td class="contentpadding">主机名 (选填):</td>
                  <td><input type=text name="hostname" size=10 maxlength=32 value="<% getCfgGeneral(1, "hostname"); %>"> 
                  (更多的帮助信息请<a href="#" onclick=popHelp('help.htm#dynamic')>点击这里</a>)</td>
                </tr>
-->
				<tr id="dhcpIPMtu">
                  <td class="contentpadding">MTU:</td>
                  <td><input name="dhcpMtu" maxlength=4 value="<% getCfgGeneral(1, "wan_dhcp_mtu"); %>">(如非必要，请勿更改)</td>
                </tr>
				<tr id="dhcpIpStatus">
                  <td class="contentpadding">IP地址:</td>
                  <td><% getWanIp(); %></td>
                </tr>
				<tr id="dhcpMaskStatus">
                  <td class="contentpadding">子网掩码:</td>
                  <td><% getWanNetmask(); %></td>
                </tr>
				<tr id="dhcpGateStatus">
                  <td class="contentpadding">网关:</td>
                  <td><% getWanGateway(); %></td>
                </tr>
              </table>
			</td>
		  </tr>
		  <tr id=dhcpplus>
            <td>
			  <table class="space" width="100%" border="0" cellspacing="0" cellpadding="0">
              	<tr>
            	  <td colspan="2" class="titlebg">DHCP+ 模式</td>
          		</tr>
                <tr id="dhcppusername">
                  <td class="contentpadding">账号:</td>
                  <td width="75%"><input name="dhcppuser" maxlength=32 value="<% getCfgGeneral(1, "wan_dhcppuser"); %>"> (必填)</td>
                </tr>
                <tr id="dhcpppassword">
                  <td class="contentpadding">密码:</td>
                  <td width="75%"><input type="password" name="dhpcppassword" maxlength=32 value="<% getCfgGeneral(1, "wan_dhcpppassword"); %>"> (必填)</td>
                </tr>
				<tr id="dhcpIpStatus">
                  <td class="contentpadding">IP地址:</td>
                  <td><% getWanIp(); %>	 </td>
                </tr>
				<tr id="dhcpMaskStatus">
                  <td class="contentpadding">子网掩码:</td>
                  <td><% getWanNetmask(); %></td>
                </tr>
				<tr id="dhcpGateStatus">
                  <td class="contentpadding">网关:</td>
                  <td><% getWanGateway(); %></td>
                </tr>
              </table>
			</td>
		  </tr>
<script>
var mode = "<% getCfgGeneral(1, "wanConnectionMode"); %>";
document.getElementById("dhcpIpStatus").style.visibility = "hidden";
document.getElementById("dhcpIpStatus").style.display = "none";
document.getElementById("dhcpMaskStatus").style.visibility = "hidden";
document.getElementById("dhcpMaskStatus").style.display = "none";
document.getElementById("dhcpGateStatus").style.visibility = "hidden";
document.getElementById("dhcpGateStatus").style.display = "none";
if (mode == "DHCP") {
	document.getElementById("dhcpIpStatus").style.visibility = "visible";
	document.getElementById("dhcpIpStatus").style.display = "block";
	document.getElementById("dhcpMaskStatus").style.visibility = "visible";
	document.getElementById("dhcpMaskStatus").style.display = "block";
	document.getElementById("dhcpGateStatus").style.visibility = "visible";
	document.getElementById("dhcpGateStatus").style.display = "block";
}
</script>
          <tr id=pppoe>
            <td>
			  <table class="space" width="100%" border="0" cellspacing="0" cellpadding="0">

               <tr>
        	  <td colspan="2" class="titlebg">PPPoE模式
          		</tr>
<!--
				<tr>
                  <td width="25%" class="contentpadding" >PPPOE+:</td>
                  <td width="75%"><select name="pppoe_plus_en" size="1" onChange="">
      <option value="0">禁用</option>
      <option value="1">启用</option></select> (如果您的ADSL拨号方式为PPPOE+，请启用它)</td>
                </tr>
-->
				<tr>
				  <td class="contentpadding">当前状态:</td>
				  <td><% if ( getIndex("pppConnectStatus") ) write("已连接"); else write("未连接");%></td>
				</tr>
				<tr>
                  <td width="25%" class="contentpadding" >特殊方式:</td>
                  <td width="75%"><select name="specific" size="1" onChange="">
      <option value="0">禁用</option>
      <option value="1">特殊方式一</option>
	  <option value="2">特殊方式二</option>
	  <option value="3">特殊方式三</option></select>
	  </td>
                </tr>
                <tr>
                  <td width="25%" class="contentpadding">用户名:</td>
                  <td width="75%"><input name="pppoeUser" maxlength=32 value="<% getCfgGeneral(1, "wan_pppoe_Encrypt_user"); %>"> (必填)</td>
                </tr>
                <tr>
                  <td class="contentpadding">密码:</td>
                  <td><input type="password" name="pppoePass" maxlength=32 value="<% getCfgGeneral(1, "wan_pppoe_pass"); %>"> (必填)</td>
                </tr>
				<tr>
                  <td class="contentpadding">重复密码:</td>
                  <td><input type="password" name="pppoePass2" maxlength=32 value="<% getCfgGeneral(1, "wan_pppoe_pass"); %>"> (必填)</td>
                </tr>
				<tr>
                  <td class="contentpadding">连接方式:</td>
                  <td><select size="1" name="pppConnectType" onChange="pppTypeSelection()">
        <% var type = getIndex("pppConnectType");
	     	if ( type == 0 ) {
	      	  	write( "<option selected value=\"0\">\xd7\xd4\xb6\xaf\xc1\xac\xbd\xd3</option>" );
    		  	write( "<option value=\"2\">\xca\xd6\xb6\xaf\xc1\xac\xbd\xd3</option>" );
				write( "<option value=\"1\">\xb0\xb4\xd0\xe8\xc1\xac\xbd\xd3</option>" );
	     	}
	     	if ( type == 1 ) {
	      	  	write( "<option value=\"0\">\xd7\xd4\xb6\xaf\xc1\xac\xbd\xd3</option>" );
    		  	write( "<option value=\"2\">\xca\xd6\xb6\xaf\xc1\xac\xbd\xd3</option>" );
				write( "<option selected value=\"1\">\xb0\xb4\xd0\xe8\xc1\xac\xbd\xd3</option>" );
	     	}
	     	if ( type == 2 ) {
	      	  	write( "<option value=\"0\">\xd7\xd4\xb6\xaf\xc1\xac\xbd\xd3</option>" );
    		  	write( "<option selected value=\"2\">\xca\xd6\xb6\xaf\xc1\xac\xbd\xd3</option>" );
				write( "<option value=\"1\">\xb0\xb4\xd0\xe8\xc1\xac\xbd\xd3</option>" );
	     	}  %>
        </select>&nbsp;&nbsp;<input type="submit" class="button"  value="连 接" name="pppConnect" onClick="return pppConnectClick(0)">&nbsp;&nbsp;<input type="submit" class="button"  value="断 开" name="pppDisconnect"  onClick="return pppConnectClick(1)"><% if ( getIndex("pppConnectStatus") ) write("\n<script> setPPPConnected(); </script>\n"); %>
        为手动连接时，按钮才生效。更多信息请<a href="#" onclick=popHelp('help.htm#pppoeType')>点击这里</a>。</td>
    			</tr>
    			<tr>
      			  <td class="contentpadding">空闲时间:</td>
      			  <td><input type="text" name="pppIdleTime" size="10" maxlength="10" value="<% getCfgGeneral(1,"pppIdleTime"); %>" onclick = chackvalue()>&nbsp;(1-1000 min) 当空闲多少时间后，网络自动断开。只在按需连接方式生效</td>
    			</tr>
				<tr>
                  <td width="25%" class="contentpadding">MTU:</td>
                  <td width="75%"><input name="pppoeMtu" maxlength=5 value="<% getCfgGeneral(1, "wan_pppoe_mtu"); %>"> (如非必要，请勿更改)</td>
                </tr>
				<tr>
                  <td width="25%" class="contentpadding">MRU:</td>
                  <td width="75%"><input name="pppoeMru" maxlength=5 value="<% getCfgGeneral(1, "wan_pppoe_mru"); %>"> (如非必要，请勿更改)</td>
                </tr>
              </table>			
			</td>
          </tr>
          <tr id=l2tp>
            <td>
			  <table class="space" width="100%" border="0" cellspacing="0" cellpadding="0">
              	<tr>
            	  <td colspan="2" class="titlebg">L2TP 模式</td>
          		</tr>
				<tr>
                  <td class="contentpadding">L2TP 服务器IP地址:</td>
                  <td><input name="l2tpServer" maxlength="15" value="<% getCfgGeneral(1, "wan_l2tp_server"); %>"> (必填)</td>
                </tr>
				<tr>
                  <td width="25%" class="contentpadding">用户名:</td>
                  <td width="75%"><input name="l2tpUser" maxlength="20" value="<% getCfgGeneral(1, "wan_l2tp_user"); %>"> (必填)</td>
                </tr>
				<tr>
                  <td width="25%" class="contentpadding">密码:</td>
                  <td width="75%"><input type="password" name="l2tpPass" maxlength="32" value="<%  getCfgGeneral(1, "wan_l2tp_pass"); %>"> (必填)</td>
                </tr>
				<tr>
				  <td class="contentpadding">MTU</td>
				  <td><input name="l2tpMtu" maxlength="5" value="<% getCfgGeneral(1, "wan_l2tp_mtu"); %>"> (如非必要，请勿更改)</td>
				</tr>
				<tr>
				  <td class="contentpadding">地址模式:</td>
				  <td><select name="l2tpMode" size="1" onChange="l2tpModeSwitch()">
					  <option value="0">静态</option>
					  <option value="1">动态</option></select> (相应的帮助信息，<a href="#" onclick=popHelp('help.htm#l2tpType')>请点击这里</a>).</td>
				</tr>
				<tr id="l2tpIp">
				  <td class="contentpadding">IP地址:</td>
				  <td><input name="l2tpIp" maxlength=15 value="<% getCfgGeneral(1, "wan_l2tp_ip"); %>"></td>
				</tr>
				<tr id="l2tpNetmask">
				  <td class="contentpadding">子网掩码:</td>
				  <td><input name="l2tpNetmask" maxlength=15 value="<% getCfgGeneral(1, "wan_l2tp_netmask"); %>"></td>
				</tr>
				<tr id="l2tpGateway">
				  <td class="contentpadding">默认网关:</td>
				  <td><input name="l2tpGateway" maxlength=15 value="<% getCfgGeneral(1, "wan_l2tp_gateway"); %>"></td>
				</tr>
<tr>
  <td class="contentpadding" id="wL2tpOPMode">连接方式</td>
  <td>
    <select name="l2tpOPMode" size="1" onChange="l2tpOPModeSwitch()">
      <option value="KeepAlive" id="wL2tpKeepAlive">持续连接</option>
      <option value="OnDemand" id="wL2tpOnDemand">按需连接</option>
      <option value="Manual" id="wL2tpManual">手动连接</option>
	  </select>
  </td>
</tr>
<tr>
  <td class="contentpadding">保持连接模式:</td>
   <td>每<input type="text" name="l2tpRedialPeriod" maxlength="5" size="3" value="60">秒重拨一次</td>
</tr>
<tr>
	<td class="contentpadding">按需连接模式:</td>
	<td>空闲<input type="text" name="l2tpIdleTime" maxlength="3" size="2" value="5">分断开</td>
</tr>
              </table>			
			</td>
          </tr>
          <tr id=pptp>
            <td>
			  <table class="space" width="100%" border="0" cellspacing="0" cellpadding="0">
                <tr>
                  <td colspan="2" class="titlebg">PPTP 模式</td>
                </tr>
                <tr>
                  <td class="contentpadding">PPTP 服务器IP地址:</td>
                  <td><input name="pptpServer" maxlength="15" value="<% getCfgGeneral(1, "wan_pptp_server"); %>"> (必填)</td>
                </tr>
                <tr>
                  <td class="contentpadding">用户名:</td>
                  <td><input name="pptpUser" maxlength="20" value="<% getCfgGeneral(1, "wan_pptp_user"); %>"> (必填)</td>
                </tr>
				<tr>
                  <td class="contentpadding">密码:</td>
                  <td><input type="password" name="pptpPass" maxlength="32" value="<% getCfgGeneral(1, "wan_pptp_pass"); %>"> (必填)</td>
                </tr>
				<tr>
                  <td width="25%" class="contentpadding">重复密码:</td>
                  <td width="75%"><input type="password" name="pptpPass2" maxlength="32" value="<%getCfgGeneral(1, "wan_pptp_pass"); %>"> (必填)</td>
                </tr>
				<tr>
                  <td width="25%" class="contentpadding">MTU:</td>
                  <td width="75%"><input name="pptpMtu" maxlength="5" value="<% getCfgGeneral(1, "wan_pptp_mtu"); %>"> (如非必要，请勿更改)</td>
                </tr>
				<tr>
                  <td class="contentpadding">地址模式:</td>
                  <td><select name="pptpMode" size="1" onChange="pptpModeSwitch()">
      <option value="0">静态</option>
      <option value="1">动态</option></select>（相关帮助信息，<a href="#" onclick=popHelp('help.htm#pptpType')>请点击这里</a>）.</td>
                </tr>
				<tr>
				<td class="contentpadding">IP地址:</td>
                  <td><input name="pptpIp" maxlength=15 value="<% getCfgGeneral(1, "wan_pptp_ip"); %>"></td>
                </tr>
				<tr>
                  <td class="contentpadding">子网掩码:</td>
                  <td><input name="pptpNetmask" maxlength=15 value="<% getCfgGeneral(1, "wan_pptp_netmask"); %>"></td>
                </tr>
				<tr>
                  <td class="contentpadding">默认网关:</td>
                  <td><input name="pptpGateway" maxlength=15 value="<% getCfgGeneral(1, "wan_pptp_gateway"); %>"></td>
                </tr>
<tr>
  <td class="contentpadding" id="wPptpOPMode">连接方式</td>
  <td>
    <select name="pptpOPMode" size="1" onChange="pptpOPModeSwitch()">
      <option value="KeepAlive" id="wPptpKeepAlive">持续连接</option>
      <option value="OnDemand" id="wPptpOnDemand">按需连接</option>
      <option value="Manual" id="wPptpManual">手动连接</option>
	  </select>
  </td>
</tr>
<tr>
  <td class="contentpadding">保持连接模式:</td>
  <td>每<input type="text" name="pptpRedialPeriod" maxlength="5" size="3" value="60">秒重拨一次
  </td>
</tr>
<tr>
  <td class="contentpadding">按需连接模式:</td>
  <td>空闲<input type="text" name="pptpIdleTime" maxlength="3" size="2" value="5">分断开</td>
 </tr>
              </table>			
			</td>
          </tr>
		  <tr id=l2tpmac>
            <td><table class="space" width="100%" border="0" cellspacing="0" cellpadding="0">
                <tr>
                  <td colspan="2" class="titlebg">MAC地址克隆</td>
                </tr>
				<tr>
                  <td width="25%" class="contentpadding" >启用:</td>
                  <td width="75%"><select name="macCloneEnbl" size="1" onChange="macCloneSwitch()">
      <option value="0">禁用</option>
      <option value="1">启用</option></select> (如果您需要同时修改WAN口的MAC地址，请启用它)</td>
                </tr>
				<tr  id="macCloneMacRow">
                  <td width="25%" class="contentpadding">MAC地址:</td>
                  <td width="75%"><input name="macCloneMac" id="macCloneMac" maxlength=17 value="<% getCfgGeneral(1, "macCloneMac"); %>">&nbsp;<input type="button" class="button2" name="macCloneMacFill" id="macCloneMacFill" value="用本PC MAC" onClick="macCloneMacFillSubmit();" > MAC地址的格式如 <font color="cc0000">00:ed:23:da:eb:18 </font></td>
                </tr>
            </table></td>
          </tr>
		  <tr><td>&nbsp;</td></tr>
	  	  <tr>
        	<td class="contentpadding"><input type="hidden" value="" name="synctime"><input type="hidden" value="/int_wan.asp" name="submit-url"><input type="submit" class="button"  value="提 交" />&nbsp;&nbsp;<input type="reset" class="button"  value="取 消" onClick="window.location.reload()"> </td>
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
