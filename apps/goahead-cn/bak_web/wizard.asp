<html>
<head>
<title>WR922</title>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312" />
<link href="images/kingnet.css" type="text/css" rel="stylesheet">
<style>
.on {display:on}
.off {display:none}
</style>
<script type="text/javascript" src="common.js"> </script>
<script language="JavaScript" type="text/javascript">
////////////////////////////////////////////////use///////////////////////////////////////////
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
function cancelClick(){
	
	window.location="wizard.asp" ;
}
function show_div(show,id) {
	if(show)
	{   
		document.getElementById(id).className  = "on" ;
	    
	}
    	else	    
    		document.getElementById(id).className  = "off" ;
}

function wizardHideDiv()
{
	//alert("111");
	show_div(false, "top_div");
	show_div(false, "lan_div");
	show_div(false, "wan_div");
	show_div(false, "wlan1_div");
	show_div(false, "wlan2_div");
 }
function saveClick_lan(next){
	form =  document.wizard;
		if(next)
		{
			if(saveChanges_lan(form) ==false) return false ;
			else
			{
				wizardHideDiv();
				show_div(true, ("wan_div"));
			}
		}
		else
		{
			wizardHideDiv();
			show_div(true, "top_div");
		}
}
function saveChanges_lan(form)
{
  if (!checkIpAddr(document.wizard.lanIp, false))
			return false;
  if (!checkIpAddr(document.wizard.lanNetmask, true))
			return false;
			
		
  if(document.wizard.lanDhcpType.options.selectedIndex == 1)	
  {	
		if (!checkIpAddr(document.wizard.dhcpStart, false))
				 return false;
		if(!checkIpAddr(document.wizard.dhcpEnd, false))
				 return false;
					 
		if ( !checkClientRange(document.wizard.dhcpStart.value,document.wizard.dhcpEnd.value) ) {
		alert('DHCP范围错误!');
		document.wizard.dhcpStart.focus();
		return false;
        } 
	}
			
	if ( !checkSubnet(document.wizard.dhcpStart.value,document.wizard.lanNetmask.value,document.wizard.dhcpEnd.value)) {
		alert('DHCP范围错误!');
		document.wizard.dhcpEnd.value = document.wizard.dhcpEnd.defaultValue;
		document.wizard.dhcpEnd.focus();
		return false;
	}
	var dhcprangestart="<% getCfgGeneral(1, "dhcpStart"); %>";
//		alert(dhcprangestart);
	if(document.wizard.dhcpStart.value==dhcprangestart){
//			alert("test");
			document.wizard.dhcpStart.value = getDigit(document.wizard.lanIp.value, 1)+'.'+getDigit(document.wizard.lanIp.value, 2)+'.'+getDigit(document.wizard.lanIp.value, 3)+'.'+getDigit(document.wizard.dhcpStart.value, 4);
//			alert(document.lanCfg.dhcpRangeStart.value);
			document.wizard.dhcpEnd.value = getDigit(document.wizard.lanIp.value, 1)+'.'+getDigit(document.wizard.lanIp.value, 2)+'.'+getDigit(document.wizard.lanIp.value, 3)+'.'+getDigit(document.wizard.dhcpEnd.value, 4);
//			alert(document.lanCfg.dhcpRangeEnd.value);	
		}
		var dhcprangeend="<% getCfgGeneral(1, "dhcpEnd"); %>";
		if(document.wizard.dhcpEnd.value==dhcprangeend){
			document.wizard.dhcpEnd.value = getDigit(document.wizard.lanIp.value, 1)+'.'+getDigit(document.wizard.lanIp.value, 2)+'.'+getDigit(document.wizard.lanIp.value, 3)+'.'+getDigit(document.wizard.dhcpEnd.value, 4);
//			alert(document.lanCfg.dhcpRangeEnd.value);	
			document.wizard.dhcpStart.value = getDigit(document.wizard.lanIp.value, 1)+'.'+getDigit(document.wizard.lanIp.value, 2)+'.'+getDigit(document.wizard.lanIp.value, 3)+'.'+getDigit(document.wizard.dhcpStart.value, 4);
//			alert(document.lanCfg.dhcpRangeStart.value);
		}
}
function saveClick_wan(next){
	form =  document.wizard;
		if(next)
		{
			if(saveChanges_wan(form) ==false) return false ;
			else
			{
				wizardHideDiv();
				show_div(true, "wlan1_div");
			}
		}
		else
		{
			wizardHideDiv();
			show_div(true, "lan_div");
		}
	
}
function connectionTypeSwitch()
{
  if(!document.getElementById){
  	alert('Error! Your browser must have CSS support !');
  	return;
  }
  if(document.wizard.wanconnectionType.selectedIndex == 0){	//static ip
    show_div(false, "static_div");
  	show_div(false, "dhcpc_div");
	show_div(false, "pppoe_div");
  	show_div(true, "static_div");
	//alert("start");
	//document.getElementById(pppoe_div).className  = "on" ;
	//document.getElementById(static_div).className  = "on" ;
	//alert("end");
  }
  else if(document.wizard.wanconnectionType.selectedIndex == 1){//Dhcp
  	//alert("dhcp");
    show_div(false, "static_div");
  	show_div(false, "dhcpc_div");
	show_div(false, "pppoe_div");
  	show_div(true, "dhcpc_div");
  }
  else if(document.wizard.wanconnectionType.selectedIndex == 2){ //ppPOE
  	//alert("pppoe");
  	show_div(false, "static_div");
  	show_div(false, "dhcpc_div");
	show_div(false, "pppoe_div");
  	show_div(true, "pppoe_div");
  }
}
function saveChanges_wan(form)
{
  var wanType = form.wanconnectionType.selectedIndex ;
  if ( wanType == 0 ){ //static IP
	  if ( checkIpAddr(form.staticIp, false) == false )
	    return false;
  	  if (checkIpAddr(form.staticNetmask,true) == false)
  		return false ;

	  if (form.staticGateway.value!="" && form.staticGateway.value!="0.0.0.0") {

	    if ( checkIpAddr(form.staticGateway, false) == false )
	      return false;
	    //if ( !checkSubnet(form.wan_ip.value,form.wan_mask.value,form.wan_gateway.value)) {
	      //alert('Invalid gateway address!\nIt should be located in the same subnet of current IP address.');
	      //form.wan_gateway.value = form.wan_gateway.defaultValue;
	      //form.wan_gateway.focus();
	      //return false;
	    //}
	  }
	  else
	      form.staticGateway.value = '0.0.0.0';  
	  if(checkIpAddr(form.staticPriDns,false)==false)
	   		return false;
	  if(checkIpAddr(form.staticSecDns,false)==false)
	   		return false;
  }
  else if ( wanType == 1){ //dhcp wanType
  	  if (form.dhcpMtuSize != null){
	     d2 = getDigit(form.dhcpMtuSize.value, 1);
	     if ( validateKey(form.dhcpMtuSize.value) == 0 ||
			(d2 > 1492 || d2 < 1400) ) {
			alert("Invalid MTU size! You should set a value between 1400-1492.");
			form.dhcpMtuSize.value = form.dhcpMtuSize.defaultValue;
			form.dhcpMtuSize.focus();
			return false;
	     }
	  } 
  }
  else if ( wanType == 2){ //pppoe wanType
	   if (form.pppoeUser.value=="") {
		  alert('PPP user name cannot be empty!');
		  form.pppoeUser.value = form.pppoeUser.defaultValue;
		  form.pppoeUser.focus();
		  return false;
	   }
	   if (form.pppoePass.value=="") {
		  alert('PPP password cannot be empty!');
		  form.pppoePass.value = form.ppoepPass.defaultValue;
		  form.pppoePass.focus();
		  return false;
	   }
	   if (form.pppoePass.value != form.pppoePass2.value) {
			alert("密码不匹配!");
			return false;
		}
  }
  
   return true;
}
function saveClick_wlan1(next){
	form =  document.wizard;
		if(next)
		{
			
			wizardHideDiv();
			show_div(true, ("wlan2_div"));
		}
		else
		{
			wizardHideDiv();
			show_div(true, "wan_div");
		}
}
function saveClick_wlan2(next){
	form =  document.wizard;
		if(next)
		{
			if(checkData() ==false) return false ;
			else
			{
				//alert("submit");
				return true;
			}
		}
		else
		{
			wizardHideDiv();
			show_div(true, "wlan1_div");
		}
	
}
//function wirelessModeChange()
//{
	//var wmode;
	//document.getElementById("div_11b_channel").style.visibility = "hidden";
	//document.getElementById("div_11b_channel").style.display = "none";
	//document.wireless_basic.sz11bChannel.disabled = true;
	//document.getElementById("div_11g_channel").style.visibility = "hidden";
	//document.getElementById("div_11g_channel").style.display = "none";
	//wmode = document.wireless_basic.wirelessmode.options.selectedIndex;
	//wmode = 1*wmode;
//}






///////////////////////////////////////////////use///////////////////////////////////////
function display_on()
{
	if (window.ActiveXObject) { // IE
		return "block";
	}
	else if (window.XMLHttpRequest) { // Mozilla, Firefox, Safari,...
		return "table-row";
	}
}
/////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////use/////////////////////////////////////////
function wirelessModeChange()
{
	var wmode;
	document.getElementById("div_11b_channel").style.visibility = "hidden";
	document.getElementById("div_11b_channel").style.display = "none";
	document.wizard.sz11bChannel.disabled = true;
	document.getElementById("div_11g_channel").style.visibility = "hidden";
	document.getElementById("div_11g_channel").style.display = "none";
	document.wizard.sz11gChannel.disabled = true;
	//document.getElementById("div_11n").style.display = "none";
	
	wmode = document.wizard.wirelessmode.options.selectedIndex;

	wmode = 1*wmode;
	if (wmode == 0)
	{
		document.wizard.wirelessmode.options.selectedIndex = 0;
		document.getElementById("div_11g_channel").style.visibility = "visible";
		document.getElementById("div_11g_channel").style.display = style_display_on();
		document.wizard.sz11gChannel.disabled = false;
	}
	else if (wmode == 1)
	{
		document.wizard.wirelessmode.options.selectedIndex = 1;
		document.getElementById("div_11b_channel").style.visibility = "visible";
		document.getElementById("div_11b_channel").style.display = style_display_on();
		document.wizard.sz11bChannel.disabled = false;
	}
	else if (wmode == 2)
	{
		document.wizard.wirelessmode.options.selectedIndex = 2;
		document.getElementById("div_11g_channel").style.visibility = "visible";
		document.getElementById("div_11g_channel").style.display = style_display_on();
		document.wizard.sz11gChannel.disabled = false;
	}
	
	//else if (wmode == 5)
	else if (wmode == 3)
	{
		document.wizard.wirelessmode.options.selectedIndex = 3;
		document.getElementById("div_11g_channel").style.visibility = "visible";
		document.getElementById("div_11g_channel").style.display = style_display_on();
		document.wizard.sz11gChannel.disabled = false;
		//if (window.ActiveXObject) // IE
			//document.getElementById("div_11n").style.display = "block";
		//else if (window.XMLHttpRequest)  // Mozilla, Safari,...
			//document.getElementById("div_11n").style.display = "table";
		//if (document.wireless_basic.sz11gChannel.options.selectedIndex == 0)
		//{
		//	document.getElementById("extension_channel").style.visibility = "hidden";
			//document.getElementById("extension_channel").style.display = "none";
		//	document.wireless_basic.n_extcha.disabled = true;
		//}

		//insertExtChannelOption();
	}


}
///////////////////////////////////////////////////////////////////////////////////////////////////////////

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
}
	


////////////////////////////////////////////////use////////////////////////////////////////
function dhcpTypeSwitch()
{
	document.getElementById("start").style.visibility = "hidden";
	document.getElementById("start").style.display = "none";
	document.wizard.dhcpStart.disabled = true;
	document.getElementById("end").style.visibility = "hidden";
	document.getElementById("end").style.display = "none";
	document.wizard.dhcpEnd.disabled = true;
	if (document.wizard.lanDhcpType.options.selectedIndex == 1)
	{
		document.getElementById("start").style.visibility = "visible";
		document.getElementById("start").style.display = display_on();
		document.wizard.dhcpStart.disabled = false;
		document.getElementById("end").style.visibility = "visible";
		document.getElementById("end").style.display = display_on();
		document.wizard.dhcpEnd.disabled = false;
	}
}
/////////////////////////////////////////////////////////////////////////////////////////
/*
function initValue()
{
	var opmode = "<% getCfgZero(1, "OperationMode"); %>";
	var dhcp = <% getCfgZero(1, "dhcpEnabled"); %>;
	var stp = <% getCfgZero(1, "stpEnabled"); %>;
	var lltd = <% getCfgZero(1, "lltdEnabled"); %>;
	var igmp = <% getCfgZero(1, "igmpEnabled"); %>;
	var upnp = <% getCfgZero(1, "upnpEnabled"); %>;
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

	document.lanCfg.lanDhcpType.options.selectedIndex = 1*dhcp;
	dhcpTypeSwitch();
	document.lanCfg.stpEnbl.options.selectedIndex = 1*stp;
	document.lanCfg.lltdEnbl.options.selectedIndex = 1*lltd;
	document.lanCfg.igmpEnbl.options.selectedIndex = 1*igmp;
	document.lanCfg.upnpEnbl.options.selectedIndex = 1*upnp;
	document.lanCfg.radvdEnbl.options.selectedIndex = 1*radvd;
	document.lanCfg.pppoeREnbl.options.selectedIndex = 1*pppoe;
	document.lanCfg.dnspEnbl.options.selectedIndex = 1*dns;

	//gateway, dns only allow to configure at bridge mode
	if (opmode != "0") {
		document.getElementById("brGateway").style.visibility = "hidden";
		document.getElementById("brGateway").style.display = "none";
		document.getElementById("brPriDns").style.visibility = "hidden";
		document.getElementById("brPriDns").style.display = "none";
		document.getElementById("brSecDns").style.visibility = "hidden";
		document.getElementById("brSecDns").style.display = "none";
	}

	if (wan == "PPPOE" || wan == "L2TP" || wan == "PPTP") {
		document.getElementById("igmpProxy").style.visibility = "hidden";
		document.getElementById("igmpProxy").style.display = "none";
	}
	else {
		document.getElementById("igmpProxy").style.visibility = "visible";
		document.getElementById("igmpProxy").style.display = display_on();
	}

	if (lltdb == "0") {
		document.getElementById("lltd").style.visibility = "hidden";
		document.getElementById("lltd").style.display = "none";
		document.lanCfg.lltdEnbl.options.selectedIndex = 0;
	}
	if (igmpb == "0") {
		document.getElementById("igmpProxy").style.visibility = "hidden";
		document.getElementById("igmpProxy").style.display = "none";
		document.lanCfg.igmpEnbl.options.selectedIndex = 0;
	}
	if (upnpb == "0") {
		document.getElementById("upnp").style.visibility = "hidden";
		document.getElementById("upnp").style.display = "none";
		document.lanCfg.upnpEnbl.options.selectedIndex = 0;
	}
	if (radvdb == "0") {
		document.getElementById("radvd").style.visibility = "hidden";
		document.getElementById("radvd").style.display = "none";
		document.lanCfg.radvdEnbl.options.selectedIndex = 0;
	}
	if (pppoeb == "0") {
		document.getElementById("pppoerelay").style.visibility = "hidden";
		document.getElementById("pppoerelay").style.display = "none";
		document.lanCfg.pppoeREnbl.options.selectedIndex = 0;
	}
	if (dnsp == "0") {
		document.getElementById("dnsproxy").style.visibility = "hidden";
		document.getElementById("dnsproxy").style.display = "none";
		document.lanCfg.dnspEnbl.options.selectedIndex = 0;
	}
}
*/



///////////////////////////////////////init wireless sec ////////////////////////////////////////
var MBSSID_MAX 				= 8;
var ACCESSPOLICYLIST_MAX	= 64;

var changed = 0;

var old_MBSSID;

var defaultShownMBSSID = 0;
var SSID = new Array();
var PreAuth = new Array();
var AuthMode = new Array();
var EncrypType = new Array();
var DefaultKeyID = new Array();
var Key1Type = new Array();
var Key1Str = new Array();
var Key2Type = new Array();
var Key2Str = new Array();
var Key3Type = new Array();
var Key3Str = new Array();
var Key4Type = new Array();
var Key4Str = new Array();
var WPAPSK = new Array();
var RekeyMethod = new Array();
var RekeyInterval = new Array();
var PMKCachePeriod = new Array();
var IEEE8021X = new Array();
var RADIUS_Server = new Array();
var RADIUS_Port = new Array();
var RADIUS_Key = new Array();
var session_timeout_interval = new Array();
var AccessPolicy = new Array();
var AccessControlList = new Array();
function initAll()
{

	makeRequest("/goform/wirelessGetSecurity", "n/a", securityHandler);
}
var http_request = false;
function makeRequest(url, content, handler) {
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
		alert('Giving up :( Cannot create an XMLHTTP instance');
		return false;
	}
	http_request.onreadystatechange = handler;
	http_request.open('POST', url, true);
	http_request.send(content);
}
function securityHandler() {
	if (http_request.readyState == 4) {
		if (http_request.status == 200) {
			parseAllData(http_request.responseText);
			//UpdateMBSSIDList();
			LoadFields(defaultShownMBSSID);

			// load Access Policy for MBSSID[selected]
			//LoadAP();
			//ShowAP(defaultShownMBSSID);

//			if(<% getWPSModeASP(); %> && <% isWPSConfiguredASP(); %>){
//				alert("Info: The security settings has been assigned under active WPS functions.\nYou still could change security setting manually but the existed WPS settings would be overwritted.");
//			}

		} else {
			alert('There was a problem with the request.');
		}
	}
}

function parseAllData(str)
{
	var all_str = new Array();
	all_str = str.split("\n");

	defaultShownMBSSID = parseInt(all_str[0]);

	for (var i=0; i<all_str.length-2; i++) {
		var fields_str = new Array();
		fields_str = all_str[i+1].split("\r");

		SSID[i] = fields_str[0];
		PreAuth[i] = fields_str[1];
		AuthMode[i] = fields_str[2];
		EncrypType[i] = fields_str[3];
		DefaultKeyID[i] = fields_str[4];
		Key1Type[i] = fields_str[5];
		Key1Str[i] = fields_str[6];
		Key2Type[i] = fields_str[7];
		Key2Str[i] = fields_str[8];
		Key3Type[i] = fields_str[9];
		Key3Str[i] = fields_str[10];
		Key4Type[i] = fields_str[11];
		Key4Str[i] = fields_str[12];
		WPAPSK[i] = fields_str[13];
		RekeyMethod[i] = fields_str[14];
		RekeyInterval[i] = fields_str[15];
		PMKCachePeriod[i] = fields_str[16];
		IEEE8021X[i] = fields_str[17];
		RADIUS_Server[i] = fields_str[18];
		RADIUS_Port[i] = fields_str[19];
		RADIUS_Key[i] = fields_str[20];
		session_timeout_interval[i] = fields_str[21];
		AccessPolicy[i] = fields_str[22];
		AccessControlList[i] = fields_str[23];
		//alert(SSID[0]);
		/* !!!! IMPORTANT !!!!*/
		if(IEEE8021X[i] == "1")
			AuthMode[i] = "IEEE8021X";

		if(AuthMode[i] == "OPEN" && EncrypType[i] == "NONE" && IEEE8021X[i] == "0")
			AuthMode[i] = "Disable";
	}
}
function LoadFields(MBSSID)
{
	var result;
	// Security Policy
	//alert(MBSSID);
	sp_select = document.getElementById("security_mode");

	sp_select.options.length = 0;
	//alert(AuthMode[0]);
    sp_select.options[sp_select.length] = new Option("Disable",	"Disable",	false, AuthMode[MBSSID] == "Disable");
    sp_select.options[sp_select.length] = new Option("OPEN",	"OPEN",		false, AuthMode[MBSSID] == "OPEN");
    sp_select.options[sp_select.length] = new Option("SHARED",	"SHARED", 	false, AuthMode[MBSSID] == "SHARED");
   // sp_select.options[sp_select.length] = new Option("WEPAUTO", "WEPAUTO",	false, AuthMode[MBSSID] == "WEPAUTO");
   // sp_select.options[sp_select.length] = new Option("WPA",		"WPA",		false, AuthMode[MBSSID] == "WPA");
   // sp_select.options[sp_select.length] = new Option("WPA-PSK", "WPAPSK",	false, AuthMode[MBSSID] == "WPAPSK");
   // sp_select.options[sp_select.length] = new Option("WPA2",	"WPA2",		false, AuthMode[MBSSID] == "WPA2");
   // sp_select.options[sp_select.length] = new Option("WPA2-PSK","WPA2PSK",	false, AuthMode[MBSSID] == "WPA2PSK");
   // sp_select.options[sp_select.length] = new Option("WPAPSKWPA2PSK","WPAPSKWPA2PSK",	false, AuthMode[MBSSID] == "WPAPSKWPA2PSK");
    //sp_select.options[sp_select.length] = new Option("WPA1WPA2","WPA1WPA2",	false, AuthMode[MBSSID] == "WPA1WPA2");

	/* 
	 * until now we only support 8021X WEP for MBSSID[0]
	 */
	//if(MBSSID == 0)sp_select.options[sp_select.length] = new Option("802.1X",	"IEEE8021X",false, AuthMode[MBSSID] == "IEEE8021X");
	if(MBSSID == 0)
	{
	if(AuthMode[0]=="Disable") document.getElementById("security_mode").selectedIndex=0;
	else if(AuthMode[0]=="OPEN") document.getElementById("security_mode").selectedIndex=1;
	else if(AuthMode[0]=="SHARED") document.getElementById("security_mode").selectedIndex=2;
	else
	document.getElementById("security_mode").selectedIndex=0;
	}
	// WEP
	document.getElementById("WEP1").value = Key1Str[MBSSID];
	document.getElementById("WEP2").value = Key2Str[MBSSID];
	document.getElementById("WEP3").value = Key3Str[MBSSID];
	document.getElementById("WEP4").value = Key4Str[MBSSID];
	//alert(Key1Str[0]);
	document.getElementById("WEP1Select").selectedIndex = (Key1Type[MBSSID] == "0" ? 1 : 0);
	document.getElementById("WEP2Select").selectedIndex = (Key2Type[MBSSID] == "0" ? 1 : 0);
	document.getElementById("WEP3Select").selectedIndex = (Key3Type[MBSSID] == "0" ? 1 : 0);
	document.getElementById("WEP4Select").selectedIndex = (Key4Type[MBSSID] == "0" ? 1 : 0);

	document.getElementById("wep_default_key").selectedIndex = parseInt(DefaultKeyID[MBSSID]) - 1 ;

	// SHARED && NONE
	if(AuthMode[MBSSID] == "SHARED" && EncrypType[MBSSID] == "NONE")
		document.getElementById("security_shared_mode").selectedIndex = 1;
	else
		document.getElementById("security_shared_mode").selectedIndex = 0;

	// WPA
	//if(EncrypType[MBSSID] == "TKIP")
		//document.security_form.cipher[0].checked = true;
	//else if(EncrypType[MBSSID] == "AES")
		//document.security_form.cipher[1].checked = true;
	//else if(EncrypType[MBSSID] == "TKIPAES")
		//document.security_form.cipher[2].checked = true;

	//document.getElementById("passphrase").value = WPAPSK[MBSSID];
	//document.getElementById("keyRenewalInterval").value = RekeyInterval[MBSSID];
	//document.getElementById("PMKCachePeriod").value = PMKCachePeriod[MBSSID];
	//document.getElementById("PreAuthentication").value = PreAuth[MBSSID];
	//if(PreAuth[MBSSID] == "0")
		//document.security_form.PreAuthentication[0].checked = true;
	//else
		//document.security_form.PreAuthentication[1].checked = true;

	//802.1x wep
	//if(IEEE8021X[MBSSID] == "1"){
		//if(EncrypType[MBSSID] == "WEP")
			//document.security_form.ieee8021x_wep[1].checked = true;
		//else
			//document.security_form.ieee8021x_wep[0].checked = true;
	//}
	
	//document.getElementById("RadiusServerIP").value = RADIUS_Server[MBSSID];
	//document.getElementById("RadiusServerPort").value = RADIUS_Port[MBSSID];
	//document.getElementById("RadiusServerSecret").value = RADIUS_Key[MBSSID];			
	//document.getElementById("RadiusServerSessionTimeout").value = session_timeout_interval[MBSSID];
	
	securityMode(0);

}
function securityMode(c_f)
{
	var security_mode;


	//changed = c_f;

	hideWep();


	document.getElementById("div_security_shared_mode").style.visibility = "hidden";
	document.getElementById("div_security_shared_mode").style.display = "none";
	//document.getElementById("div_wpa").style.visibility = "hidden";
	//document.getElementById("div_wpa").style.display = "none";
	//document.getElementById("div_wpa_algorithms").style.visibility = "hidden";
	//document.getElementById("div_wpa_algorithms").style.display = "none";
	//document.getElementById("wpa_passphrase").style.visibility = "hidden";
	//document.getElementById("wpa_passphrase").style.display = "none";
	//document.getElementById("wpa_key_renewal_interval").style.visibility = "hidden";
	//document.getElementById("wpa_key_renewal_interval").style.display = "none";
	//document.getElementById("wpa_PMK_Cache_Period").style.visibility = "hidden";
	//document.getElementById("wpa_PMK_Cache_Period").style.display = "none";
	//document.getElementById("wpa_preAuthentication").style.visibility = "hidden";
	//document.getElementById("wpa_preAuthentication").style.display = "none";
	//document.security_form.cipher[0].disabled = true;
	//document.security_form.cipher[1].disabled = true;
	//document.security_form.cipher[2].disabled = true;
	//document.security_form.passphrase.disabled = true;
	//document.security_form.keyRenewalInterval.disabled = true;
	//document.security_form.PMKCachePeriod.disabled = true;
	//document.security_form.PreAuthentication.disabled = true;

	// 802.1x
	//document.getElementById("div_radius_server").style.visibility = "hidden";
	//document.getElementById("div_radius_server").style.display = "none";
	//document.getElementById("div_8021x_wep").style.visibility = "hidden";
	//document.getElementById("div_8021x_wep").style.display = "none";
	//document.security_form.ieee8021x_wep.disable = true;
	//document.security_form.RadiusServerIP.disable = true;
	//document.security_form.RadiusServerPort.disable = true;
	//document.security_form.RadiusServerSecret.disable = true;	
	//document.security_form.RadiusServerSessionTimeout.disable = true;
	//document.security_form.RadiusServerIdleTimeout.disable = true;	

	security_mode = document.wizard.security_mode.value;

	if (security_mode == "OPEN" || security_mode == "SHARED" ||security_mode == "WEPAUTO"){
		showWep(security_mode);
	}else if (security_mode == "WPAPSK" || security_mode == "WPA2PSK" || security_mode == "WPAPSKWPA2PSK"){
		<!-- WPA -->
		document.getElementById("div_wpa").style.visibility = "visible";
		if (window.ActiveXObject) { // IE
			document.getElementById("div_wpa").style.display = "block";
		}
		else if (window.XMLHttpRequest) { // Mozilla, Safari,...
			document.getElementById("div_wpa").style.display = "table";
		}

		document.getElementById("div_wpa_algorithms").style.visibility = "visible";
		document.getElementById("div_wpa_algorithms").style.display = style_display_on();
		document.security_form.cipher[0].disabled = false;
		document.security_form.cipher[1].disabled = false;

		// deal with TKIP-AES mixed mode
		if(security_mode == "WPAPSK" && document.security_form.cipher[2].checked)
			document.security_form.cipher[2].checked = false;
		// deal with TKIP-AES mixed mode
		if(security_mode == "WPA2PSK" || security_mode == "WPAPSKWPA2PSK")
			document.security_form.cipher[2].disabled = false;

		document.getElementById("wpa_passphrase").style.visibility = "visible";
		document.getElementById("wpa_passphrase").style.display = style_display_on();
		document.security_form.passphrase.disabled = false;

		document.getElementById("wpa_key_renewal_interval").style.visibility = "visible";
		document.getElementById("wpa_key_renewal_interval").style.display = style_display_on();
		document.security_form.keyRenewalInterval.disabled = false;
	}else if (security_mode == "WPA" || security_mode == "WPA2" || security_mode == "WPA1WPA2") //wpa enterprise
	{
		document.getElementById("div_wpa").style.visibility = "visible";
		if (window.ActiveXObject) { // IE
			document.getElementById("div_wpa").style.display = "block";
		}else if (window.XMLHttpRequest) { // Mozilla, Safari,...
			document.getElementById("div_wpa").style.display = "table";
		}

		document.getElementById("div_wpa_algorithms").style.visibility = "visible";
		document.getElementById("div_wpa_algorithms").style.display = style_display_on();
		document.security_form.cipher[0].disabled = false;
		document.security_form.cipher[1].disabled = false;
		document.getElementById("wpa_key_renewal_interval").style.visibility = "visible";
		document.getElementById("wpa_key_renewal_interval").style.display = style_display_on();
		document.security_form.keyRenewalInterval.disabled = false;
	
		<!-- 802.1x -->
		document.getElementById("div_radius_server").style.visibility = "visible";
		document.getElementById("div_radius_server").style.display = style_display_on();
		document.security_form.RadiusServerIP.disable = false;
		document.security_form.RadiusServerPort.disable = false;
		document.security_form.RadiusServerSecret.disable = false;	
		document.security_form.RadiusServerSessionTimeout.disable = false;
		document.security_form.RadiusServerIdleTimeout.disable = false;	

		// deal with TKIP-AES mixed mode
		if(security_mode == "WPA" && document.security_form.cipher[2].checked)
			document.security_form.cipher[2].checked = false;
		// deal with TKIP-AES mixed mode
		if(security_mode == "WPA2"){
			document.security_form.cipher[2].disabled = false;
			document.getElementById("wpa_preAuthentication").style.visibility = "visible";
			document.getElementById("wpa_preAuthentication").style.display = style_display_on();
			document.security_form.PreAuthentication.disabled = false;
			document.getElementById("wpa_PMK_Cache_Period").style.visibility = "visible";
			document.getElementById("wpa_PMK_Cache_Period").style.display = style_display_on();
			document.security_form.PMKCachePeriod.disabled = false;
		}

		// deal with WPA1WPA2 mixed mode
		if(security_mode == "WPA1WPA2"){
			document.security_form.cipher[2].disabled = false;
		}

	}else if (security_mode == "IEEE8021X"){ // 802.1X-WEP
		document.getElementById("div_8021x_wep").style.visibility = "visible";
		document.getElementById("div_8021x_wep").style.display = style_display_on();

		document.getElementById("div_radius_server").style.visibility = "visible";
		document.getElementById("div_radius_server").style.display = style_display_on();
		document.security_form.ieee8021x_wep.disable = false;
		document.security_form.RadiusServerIP.disable = false;
		document.security_form.RadiusServerPort.disable = false;
		document.security_form.RadiusServerSecret.disable = false;	
		document.security_form.RadiusServerSessionTimeout.disable = false;
		//document.security_form.RadiusServerIdleTimeout.disable = false;
	}
}


function hideWep()
{
	document.getElementById("div_wep").style.visibility = "hidden";
	document.getElementById("div_wep").style.display = "none";
}
function showWep(mode)
{
	<!-- WEP -->
	document.getElementById("div_wep").style.visibility = "visible";

	if (window.ActiveXObject) { // IE 
		document.getElementById("div_wep").style.display = "block";
	}
	else if (window.XMLHttpRequest) { // Mozilla, Safari...
		document.getElementById("div_wep").style.display = "table";
	}

	if(mode == "SHARED"){
		document.getElementById("div_security_shared_mode").style.visibility = "visible";
		document.getElementById("div_security_shared_mode").style.display = style_display_on();
	}
	//document.security_form.wep_auth_type.disabled = false;
}
function checkData()
{
	var securitymode;
//	var ssid = document.security_form.Ssid.value;
	
	securitymode = document.wizard.security_mode.value;
	if (securitymode == "OPEN" || securitymode == "SHARED" ||securitymode == "WEPAUTO")
	{
		if(! check_Wep(securitymode) )
			return false;
	}

	return true;
}
function check_Wep(securitymode)
{
	var defaultid = document.wizard.wep_default_key.value;
	var key_input;

	if ( defaultid == 1 )
		var keyvalue = document.wizard.wep_key_1.value;
	else if (defaultid == 2)
		var keyvalue = document.wizard.wep_key_2.value;
	else if (defaultid == 3)
		var keyvalue = document.wizard.wep_key_3.value;
	else if (defaultid == 4)
		var keyvalue = document.wizard.wep_key_4.value;

	if (keyvalue.length == 0 &&  (securitymode == "SHARED" || securitymode == "OPEN")){ // shared wep  || md5
		alert('请输入 wep 密钥'+defaultid+' !');
		return false;
	}

	var keylength = document.wizard.wep_key_1.value.length;
	if (keylength != 0){
		if (document.wizard.WEP1Select.options.selectedIndex == 0){
			if(keylength != 5 && keylength != 13) {
				alert('请输入 5 或者 13 个 wep 密钥1 字符 !');
				return false;
			}
			if(checkInjection(document.wizard.wep_key_1.value)== false){
				alert('Wep 密钥1含有非法字符.');
				return false;
			}
		}
		if (document.wizard.WEP1Select.options.selectedIndex == 1){
			if(keylength != 10 && keylength != 26) {
				alert('请输入 10 或者 26 个 wep 密钥1 字符 ! !');
				return false;
			}
			if(checkHex(document.wizard.wep_key_1.value) == false){
				alert('无效的 Wep 密钥1 格式!');
				return false;
			}
		}
	}

	keylength = document.wizard.wep_key_2.value.length;
	if (keylength != 0){
		if (document.wizard.WEP2Select.options.selectedIndex == 0){
			if(keylength != 5 && keylength != 13) {
				alert('请输入 5 或者 13 个 wep 密钥2 字符 !!');
				return false;
			}
			if(checkInjection(document.wizard.wep_key_2.value)== false){
				alert('Wep 密钥2含有非法字符.');
				return false;
			}			
		}
		if (document.wizard.WEP2Select.options.selectedIndex == 1){
			if(keylength != 10 && keylength != 26) {
				alert('请输入 10 或者 26 个 wep 密钥2 字符 ! !');
				return false;
			}
			if(checkHex(document.wizard.wep_key_2.value) == false){
				alert('无效的 Wep 密钥2 格式!');
				return false;
			}
		}
	}

	keylength = document.wizard.wep_key_3.value.length;
	if (keylength != 0){
		if (document.wizard.WEP3Select.options.selectedIndex == 0){
			if(keylength != 5 && keylength != 13) {
				alert('请输入 5 或者 13 个 wep 密钥3 字符 !!');
				return false;
			}
			if(checkInjection(document.wizard.wep_key_3.value)== false){
				alert('Wep 密钥3含有非法字符.');
				return false;
			}
		}
		if (document.wizard.WEP3Select.options.selectedIndex == 1){
			if(keylength != 10 && keylength != 26) {
				alert('请输入 10 或者 26 个 wep 密钥3 字符 !');
				return false;
			}
			if(checkHex(document.wizard.wep_key_3.value) == false){
				alert('无效的 Wep 密钥3 格式!');
				return false;
			}			
		}
	}

	keylength = document.wizard.wep_key_4.value.length;
	if (keylength != 0){
		if (document.wizard.WEP4Select.options.selectedIndex == 0){
			if(keylength != 5 && keylength != 13) {
				alert('请输入 5 或者 13 个 wep 密钥4 字符 !');
				return false;
			}
			if(checkInjection(document.wizard.wep_key_4.value)== false){
				alert('Wep 密钥4含有非法字符.');
				return false;
			}			
		}
		if (document.wizard.WEP4Select.options.selectedIndex == 1){
			if(keylength != 10 && keylength != 26) {
				alert('请输入 10 或者 26 个 wep 密钥4 字符 !');
				return false;
			}

			if(checkHex(document.wizard.wep_key_4.value) == false){
				alert('无效的 Wep 密钥3 格式!');
				return false;
			}			
		}
	}
	return true;
}
function checkInjection(str)
{
	var len = str.length;
	for (var i=0; i<str.length; i++) {
		if ( str.charAt(i) == '\r' || str.charAt(i) == '\n'){
				return false;
		}else
	        continue;
	}
    return true;
}
function checkHex(str){
	var len = str.length;

	for (var i=0; i<str.length; i++) {
		if ((str.charAt(i) >= '0' && str.charAt(i) <= '9') ||
			(str.charAt(i) >= 'a' && str.charAt(i) <= 'f') ||
			(str.charAt(i) >= 'A' && str.charAt(i) <= 'F') ){
				continue;
		}else
	        return false;
	}
    return true;
}
</script>
</head>
<body onLoad="initAll()">
<form method=post name="wizard" action="/goform/formWizard" onSubmit="return saveClick_wlan2(1)">
<table width="90%" border="0" cellpadding="0" cellspacing="0" align="center">
  <tr>
    <td style="padding-top:30px;">
	   <table width="100%" border="0" cellpadding="0" cellspacing="0">
          <tr>
            <td class="nav">当前路径:KN-WR922 &gt;&gt;设置向导 </td>
          </tr>
          <tr>
            <td>&nbsp;</td>
          </tr>
          <tr>
            <td>
			<span id = "top_div" class = "on" >
			  <table class="space" width="100%" border="0" cellspacing="0" cellpadding="0">
                <tr>
                  <td class="titlebg" colspan="2">如果用户是第一次配置此无线路由器，请在快速配置向导的指引下进行配置。 </td>
                </tr>
                <tr>
                  <td colspan="2" id="top_tit" class="contentpadding">
欢迎使用快速配置向导!此向导将引导您完成下列配置。点击 '下一步' 按钮开始。</td>
                </tr>
                <tr>
                  <td width="25%" class="contentpadding" id="top_1">步骤1:</td>
                  <td width="75%">说明页</td>
                </tr>
				<tr id="topt2">
				  <td class="contentpadding" id="top_2">步骤2:</td>
				  <td>局域网口配置(LAN)</td>
				</tr>
				<tr id="topt3">
				  <td class="contentpadding" id="top_3">步骤3:</td>
				  <td>广域网口配置(WAN)</td>
				</tr>
				<tr id="topt4">
				  <td class="contentpadding" id="top_4">步骤4:</td>
				  <td>无线局域网(WLAN)</td>
				</tr>
				<tr>
                  <td class="contentpadding" id="top_5">步骤5:</td>
                  <td>无线网安全性配置</td>
                </tr>
				<tr>
                  <td colspan="2" height="10"></td>
                </tr>
				<tr>
                  <td colspan="2" class="contentpadding" id="blankline2_top"><input type="button" class="button" value=" 下一步 " name="next" onClick='wizardHideDiv();show_div(true, "lan_div");'></td>
                </tr>
				<tr>
                  <td colspan="2" height="10"></td>
                </tr>
              </table>
			</span>
			  
			<span id = "lan_div" class = "off" >
			  <table class="space" width="100%" border="0" cellspacing="0" cellpadding="0">
              	<tr>
            	  <td class="titlebg" colspan="2">2. 局域网口配置(LAN) </td>
                </tr>
                <tr>
                  <td colspan="2" id="lanmean" class="contentpadding">此页面用来配置本设备局域网口的有关参数。<br>在这里，用户可以设定 IP地址、子网掩码、DHCP 等等。</td>
                </tr>
                <tr>
                  <td width="25%" class="contentpadding" id="lIp">IP 地址:</td>
                  <td width="75%"><input name="lanIp" maxlength=15 value="<% getLanIp(); %>"><input type="hidden" name="hostname2" maxlength="16" value="<% getCfgGeneral(1, "HostName"); %>" /></td>
                </tr>
				<tr id="lan_netmask_tr">
				  <td class="contentpadding" id="lNetmask">子网掩码:</td>
				  <td><input name="lanNetmask" maxlength=15 value="<% getLanNetmask(); %>"></td>
				</tr>
				<tr id="lan_dhcptype_tr">
				  <td class="contentpadding" id="lDhcpType">DHCP 方式:</td>
				  <td><select name="lanDhcpType" size="1" onChange="dhcpTypeSwitch();">
					  <option value="DISABLE" id="lDhcpTypeD">禁用</option>
					  <option value="SERVER" id="lDhcpTypeS">启用</option></select></td>
				</tr>
				<tr id="start">
				  <td class="contentpadding" id="lDhcpStart">DHCP 开始 IP:</td>
				  <td><input name="dhcpStart" maxlength=15 value="<% getCfgGeneral(1, "dhcpStart"); %>"></td>
				</tr>
				<tr id="end">
                  <td class="contentpadding" id="lDhcpEnd">DHCP结束 IP:</td>
                  <td><input name="dhcpEnd" maxlength=15 value="<% getCfgGeneral(1, "dhcpEnd"); %>"></td>
                </tr>
				<tr>
                  <td colspan="2" height="10"></td>
                </tr>
				<tr>
                  <td colspan="2" class="contentpadding" id="blankline_lan2"><input type="button" class="button" value=" 上一步 " name="back" onClick='return saveClick_lan(0)' >&nbsp;&nbsp;<input type="button" class="button" value=" 下一步 " name="next"  onClick='return saveClick_lan(1)'></td>
                </tr>
				<tr>
                  <td colspan="2" height="10"></td>
                </tr>
              </table>
<script>
var dhcp = <% getCfgZero(1, "dhcpEnabled"); %>;
document.wizard.lanDhcpType.options.selectedIndex = 1*dhcp;
dhcpTypeSwitch();
</script>
			</span>
			  
			<span id ="wan_div" class = "off" >
			   <table class="space" width="100%" border="0" cellspacing="0" cellpadding="0">
                 <tr>
            	  <td class="titlebg" colspan="2">3. 广域网口配置(WAN) </td>
                 </tr>
                 <tr>
                  <td colspan="2" id="wanmean" class="contentpadding">
此页面用来配置与互联网相连的广域网接口的一些参数。 在这里，用户可以改变广域网口连接到互联网的接入方式。<br>有下面三种接入方式可以选择：分别是固定IP、PPPoE、动态IP。</td>
                 </tr>
                 <tr>
                  <td width="25%" class="contentpadding" id="connecttype_wan">WAN 连接类型:</td>
                  <td width="75%"><select name="wanconnectionType" size="1" onChange="connectionTypeSwitch();">
						  <option value="STATIC">静态IP (fixed IP)</option>
						  <option value="DHCP">DHCP (Auto Config)</option>
						  <option value="PPPOE">PPPOE (ADSL)</option>
						  <!--
						  <option value="L2TP">L2TP</option>
						  <option value="PPTP">PPTP</option>
						  -->
						</select></td>
                 </tr>
				</table> 
			   <span id = "static_div" class = "off" >
			   <table class="space" width="100%" border="0" cellspacing="0" cellpadding="0">
				 <tr>
            	  <td class="titlebg" colspan="2">静态IP (fixed IP)</td>
                 </tr>
				 
				 <tr>
                  <td width="25%" class="contentpadding">IP 地址:</td>
                  <td width="75%"><input name="staticIp" maxlength=15 value="<% getCfgGeneral(1, "wan_static_ip"); %>"></td>
                 </tr>
				 <tr id="wan_netmask">
				  <td class="contentpadding">子网掩码:</td>
				  <td><input name="staticNetmask" maxlength=15 value="<% getCfgGeneral(1, "wan_static_mask"); %>"></td>
				 </tr>
				 <tr id="wan_gateway">
				  <td class="contentpadding">默认网关:</td>
				  <td><input name="staticGateway" maxlength=15 value="<% getCfgGeneral(1, "wan_static_gateway"); %>"></td>
				 </tr>
				 <tr id="wan_dns1">
				  <td class="contentpadding">首选DNS服务器:</td>
				  <td><input name="staticPriDns" maxlength=15 value="<% getCfgGeneral(1, "wan_static_dns1"); %>"></td>
				 </tr>
				 <tr>
                  <td class="contentpadding">备用DNS服务器:</td>
                  <td><input name="staticSecDns" maxlength=15 value="<% getCfgGeneral(1, "wan_static_dns2"); %>"></td>
                 </tr>
				 <tr>
                  <td colspan="2" height="10"></td>
                 </tr>
				 <tr>
                  <td colspan="2" class="contentpadding" id="blankline_wan2"><input type="button" class="button" value="取 消" name="cancel" onClick='cancelClick();'>&nbsp;&nbsp;<input type="button" class="button" value=" 上一步 " name="back" onClick='return saveClick_wan(0)' >&nbsp;&nbsp;<input type="button" class="button" value=" 下一步 " name="next"  onClick='return saveClick_wan(1)'></td>
                 </tr>
				<tr>
                  <td colspan="2" height="10"></td>
                </tr>
			   </table>	
			   </span>
			   <span id = "dhcpc_div" class = "off" >
               <table class="space" width="100%" border="0" cellspacing="0" cellpadding="0">
			    <tr>
            	  <td class="titlebg" colspan="2">DHCP (Auto Config)</td>
                </tr>
<!--
				<tr>
                  <td width="25%" class="contentpadding">主机名 (可选):</td>
                  <td width="75%"><input type=text name="hostname" size=28 maxlength=32 value="">	</td>
                </tr>
-->
				<tr>
                  <td colspan="2" height="10"></td>
                </tr>
				<tr>
                  <td colspan="2" class="contentpadding" id="blankline_dhcpwan2"><input type="button" class="button" value="取 消" name="cancel" onClick='cancelClick();'>&nbsp;&nbsp;<input type="button" class="button" value=" 上一步 " name="back" onClick='return saveClick_wan(0)' >&nbsp;&nbsp;<input type="button" class="button" value=" 下一步 " name="next"  onClick='return saveClick_wan(1)'></td>
                </tr>
				<tr>
                  <td colspan="2" height="10"></td>
                </tr>
			  </table>
			   </span>
			   <span id = "pppoe_div" class = "off" >
               <table class="space" width="100%" border="0" cellspacing="0" cellpadding="0">
				<tr>
            	  <td class="titlebg" colspan="2">PPPOE (ADSL)</td>
                </tr>
				<tr>
                  <td width="25%" class="contentpadding">用户名:</td>
                  <td width="75%"><input name="pppoeUser" maxlength=32 value="<% getCfgGeneral(1, "wan_pppoe_user"); %>"></td>
                </tr>
				<tr id="pppoepwd_wan">
				  <td class="contentpadding">密码:</td>
				  <td><input type="password" name="pppoePass" maxlength=32 value="<% getCfgGeneral(1, "wan_pppoe_pass"); %>"></td>
				</tr>
				<tr id="pppoepwdr_wan">
				  <td class="contentpadding">重复密码:</td>
				  <td><input type="password" name="pppoePass2" maxlength=32 value="<% getCfgGeneral(1, "wan_pppoe_pass"); %>"></td>
				</tr>
				<tr>
                  <td colspan="2" height="10"></td>
                </tr>
				<tr>
                  <td colspan="2" class="contentpadding" id="blankline_wanppp2"><input type="button" class="button" value="取 消" name="cancel" onClick='cancelClick();'>&nbsp;&nbsp;<input type="button" class="button" value=" 上一步 " name="back" onClick='return saveClick_wan(0)' >&nbsp;&nbsp;<input type="button" class="button" value=" 下一步 " name="next"  onClick='return saveClick_wan(1)'></td>
                </tr>
				<tr>
                  <td colspan="2" height="10"></td>
                </tr>
			  </table>	
			   </span>
<script>
var mode = "<% getCfgGeneral(1, "wanConnectionMode"); %>";
//alert(mode);
if (mode == "STATIC") {
	document.wizard.wanconnectionType.options.selectedIndex = 0;
}
else if (mode == "DHCP") {
	document.wizard.wanconnectionType.options.selectedIndex = 1;
}
else if (mode == "PPPOE") {
	document.wizard.wanconnectionType.options.selectedIndex = 2;
}
else {
	document.wizard.connectionType.options.selectedIndex = 0;
}
connectionTypeSwitch();
</script>
		    </span>	
			<span id = "wlan1_div" class = "off" >
			   <table class="space" width="100%" border="0" cellspacing="0" cellpadding="0">
              	<tr>
                  <td class="titlebg" colspan="2">4. 无线基本设置 </td>
                </tr>
                <tr>
                  <td colspan="2" id="wlan1mean" class="contentpadding">此页面用来配置无线客户端连接到本路由器的参数。<br>在这里，用户可以设置SSID，模式，频段等内容</td>
                </tr>
                <tr>
                  <td width="25%" class="contentpadding" id="basicNetMode">网络模式:</td>
                  <td width="75%"><select name="wirelessmode" id="wirelessmode" size="1" onChange="wirelessModeChange()">
                                    <option value=0>11b/g</option>
                                    <option value=1>11b</option>
                                    <option value=4>11g</option>
									<!--
                                    <option value=2>11a</option>
									
                                    <option value=8>11a/n</option>
									-->
                                    <option value=9>11b/g/n</option></select></td>
                </tr>
				<tr id="div_ssid">
				  <td class="contentpadding" id="basicSSID">网络名称(SSID):</td>
				  <td><input type=text name=ssid size=20 maxlength=32 value="<% getCfgGeneral(1, "SSID1"); %>"></td>
				</tr>
				<tr id="div_11b_channel" name="div_11b_channel" style="visibility:visible; display:none">
				  <td class="contentpadding">频段:</td>
				  <td><select id="sz11bChannel" name="sz11bChannel" size="1" onChange="ChannelOnChange()">
					<option value=0 id="basicFreqBAuto">自动选择</option>
						<% getWlan11bChannels(); %></select></td>
				</tr>
				<tr id="div_11g_channel" name="div_11g_channel" style="visibility:visible; display:none">
				  <td class="contentpadding">频段:</td>
				  <td><select id="sz11gChannel" name="sz11gChannel" size="1" onChange="ChannelOnChange()">
					<option value=0 id="basicFreqGAuto">自动选择</option>
						<% getWlan11gChannels(); %></select></td>
				</tr>
				<tr>
                  <td colspan="2" height="10"></td>
                </tr>
				<tr>
                  <td colspan="2" class="contentpadding" id="blankline_wlan2"><input type="button" class="button" value="取 消" name="cancel" onClick="cancelClick();">&nbsp;&nbsp;<input type="button" class="button" value=" 上一步 " name="back" onClick="return saveClick_wlan1(0)" >&nbsp;&nbsp;<input type="button" class="button" value=" 下一步 "  name="next"  onClick="return saveClick_wlan1(1)"></td>
                </tr>
				<tr>
                  <td colspan="2" height="10"></td>
                </tr>
              </table>
<script>
var PhyMode  = '<% getCfgZero(1, "WirelessMode"); %>';
PhyMode = 1*PhyMode;
//alert(PhyMode);
document.getElementById("div_11b_channel").style.visibility = "hidden";
document.getElementById("div_11b_channel").style.display = "none";
//document.wireless_basic.sz11bChannel.disabled = true;
document.getElementById("div_11g_channel").style.visibility = "hidden";
document.getElementById("div_11g_channel").style.display = "none";
//alert("ttt");
//document.wireless_basic.sz11gChannel.disabled = true;
if ((PhyMode == 0) || (PhyMode == 4) || (PhyMode == 9))
{
	if (PhyMode == 0)
		document.wizard.wirelessmode.options.selectedIndex = 0;
	else if (PhyMode == 4)
		document.wizard.wirelessmode.options.selectedIndex = 2;
	else if (PhyMode == 9)
	{
//alert("yyy");
		document.wizard.wirelessmode.options.selectedIndex = 3;
//alert("zzz");
	}

	document.getElementById("div_11g_channel").style.visibility = "visible";
	document.getElementById("div_11g_channel").style.display = style_display_on();
	document.wizard.sz11gChannel.disabled = false;
}
else if (PhyMode == 1)
{
	document.wizard.wirelessmode.options.selectedIndex = 1;
	document.getElementById("div_11b_channel").style.visibility = "visible";
	document.getElementById("div_11b_channel").style.display = style_display_on();
	document.wizard.sz11bChannel.disabled = false;
}
</script>
			</span>	
			  
			<span id = "wlan2_div" class = "off" >
			   <table class="space" width="100%" border="0" cellspacing="0" cellpadding="0">
                <tr>
            	  <td class="titlebg" colspan="2">5. 无线安全性配置 </td>
          	    </tr>
                <tr>
                  <td colspan="2" id="wlan2mean" class="contentpadding">
本页面设置无线网络的安全性。<br>设置密钥并开启WEP或者WPA功能，就能够阻止未授权用户对无线网的访问。</td>
                </tr>
                <tr>
                  <td width="25%" class="contentpadding" id="SSIDNAME">SSID名称:</td>
                  <td width="75%"><input readonly type=text name=secssid size=20 maxlength=32 value="<% getCfgGeneral(1, "SSID1"); %>"></td>
                </tr>
				<tr id="sec_tr">
				  <td class="contentpadding" id="secureSecureMode">安全模式:</td>
				  <td><select name="security_mode" id="security_mode" size="1" onChange="securityMode(1)"></select></td>
				</tr>
				<tr id="div_security_shared_mode" name="div_security_shared_mode" style="visibility: hidden;">
				  <td class="contentpadding" id="secureEncrypType">加密类型:</td>
				  <td><select name="security_shared_mode" id="security_shared_mode" size="1" onChange="securityMode(1)">
						<option value=WEP>WEP</option>
						<option value=None id="secureEncrypTypeNone">无</option></select></td>
				</tr>
				<tr id="secwep8">
                  <td colspan="2" id="sectable">
				      <table width="100%" border="0" cellspacing="0" cellpadding="0" id="div_wep" name="div_wep" style="visibility:hidden">
						<tr>
						  <td class="contentpadding" width="25%" id="secureWEPDefaultKey">默认密钥:</td>
						  <td colspan="2"><select name="wep_default_key" id="wep_default_key" size="1" >
			<option value="1" id="secureWEPDefaultKey1">密钥1</option>
			<option value="2" id="secureWEPDefaultKey2">密钥2</option>
			<option value="3" id="secureWEPDefaultKey3">密钥3</option>
			<option value="4" id="secureWEPDefaultKey4">密钥4</option></select></td>
						</tr>
						<tr>
						  <td class="contentpadding" id="secureWEPKey">WEP 密钥:</td>
						  <td id="secureWEPKey1">WEP 密钥1:</td>
						  <td width="69%"><input type="password" name="wep_key_1" id="WEP1" maxlength="26" value="" >&nbsp;&nbsp;<select id="WEP1Select" name="WEP1Select" > 
				<option value="1">ASCII</option>
				<option value="0">Hex</option></select></td>
						</tr>
						<tr>
						  <td>&nbsp;</td>
						  <td width="11%" id="secureWEPKey2">WEP 密钥2:</td>
						  <td><input type="password" name="wep_key_2" id="WEP2" maxlength="26" value="" >&nbsp;&nbsp;<select id="WEP2Select" name="WEP2Select" >
				<option value="1">ASCII</option>
				<option value="0">Hex</option></select></td>
						</tr>
						<tr>
						  <td>&nbsp;</td>
						  <td width="11%" id="secureWEPKey3">WEP 密钥3:</td>
						  <td><input type="password" name="wep_key_3" id="WEP3" maxlength="26" value="" >&nbsp;&nbsp;<select id="WEP3Select" name="WEP3Select" >
				<option value="1">ASCII</option>
				<option value="0">Hex</option></select></td>
						</tr>
						<tr>
						  <td>&nbsp;</td>
						  <td width="11%" id="secureWEPKey4">WEP 密钥4:</td>
						  <td><input type="password" name="wep_key_4" id="WEP4" maxlength="26" value="" >&nbsp;&nbsp;<select id="WEP4Select" name="WEP4Select" >
				<option value="1">ASCII</option>
				<option value="0">Hex</option></select></td>
						</tr>
              		  </table>
				  </td>
                </tr>
				<tr>
                  <td colspan="2" height="10"></td>
                </tr>
				<tr>
                  <td colspan="2" class="contentpadding" id="blankline_lan2"><input type="button" class="button" value="取 消" name="cancel" onClick="cancelClick();">&nbsp;&nbsp;<input type="button" class="button" value=" 上一步 " name="back" onClick='return saveClick_wlan2(0)' >&nbsp;&nbsp;<input type="submit" class="button" value="完 成" name="wancheng"></td>
                </tr>
				<tr>
                  <td colspan="2" height="10"></td>
                </tr>
              </table>
			</span>
			 </td>
          </tr>
	  </table>
	</td>
  </tr>
</table>
</form>
</body>
</html>
