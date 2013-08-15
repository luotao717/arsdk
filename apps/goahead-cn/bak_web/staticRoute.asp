<html>
<head>
<title>WR922</title>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312" />
<link href="images/kingnet.css" type="text/css" rel="stylesheet">
<script type="text/javascript" src="lang/b28n.js"></script>
<script language="JavaScript" type="text/javascript">
//Butterlate.setTextDomain("internet");
var opmode = "<% getCfgZero(1, "OperationMode"); %>";

var destination = new Array();
var gateway = new Array();
var netmask = new Array();
var flags = new Array();
var metric = new Array();
var ref = new Array();
var use = new Array();
var true_interface = new Array();
var category = new Array();
var interface = new Array();
var idle = new Array();
var comment = new Array();

function deleteClick()
{
	return true;
}

function checkRange(str, num, min, max)
{
    d = atoi(str,num);
    if(d > max || d < min)
        return false;
    return true;
}

function checkIpAddr(field)
{
    if(field.value == ""){
        field.focus();
        return false;
    }

    if ( isAllNum(field.value) == 0) {
        field.focus();
        return false;
    }

    if( (!checkRange(field.value,1,0,255)) ||
        (!checkRange(field.value,2,0,255)) ||
        (!checkRange(field.value,3,0,255)) ||
        (!checkRange(field.value,4,0,255)) ){
        field.focus();
        return false;
    }

   return true;
}


function atoi(str, num)
{
	i=1;
	if(num != 1 ){
		while (i != num && str.length != 0){
			if(str.charAt(0) == '.'){
				i++;
			}
			str = str.substring(1);
		}
	  	if(i != num )
			return -1;
	}
	
	for(i=0; i<str.length; i++){
		if(str.charAt(i) == '.'){
			str = str.substring(0, i);
			break;
		}
	}
	if(str.length == 0)
		return -1;
	return parseInt(str, 10);
}

function isAllNum(str)
{
	for (var i=0; i<str.length; i++){
	    if((str.charAt(i) >= '0' && str.charAt(i) <= '9') || (str.charAt(i) == '.' ))
			continue;
		return 0;
	}
	return 1;
}

function formCheck()
{
	if( document.addrouting.dest.value != "" && !checkIpAddr(document.addrouting.dest )){
		alert("目的格式错误.");
		return false;
	}
	if( document.addrouting.netmask.value != "" && !checkIpAddr(document.addrouting.netmask )){
		alert("子网掩码格式错误.");
		return false;
	}
	if( document.addrouting.gateway.value != "" && !checkIpAddr(document.addrouting.gateway)){
		alert("T网关格式错误.");
		return false;
	}

	if(	document.addrouting.dest.value == ""){
		alert("请填入目的地址.");
		return false;
	}

    if( document.addrouting.hostnet.selectedIndex == 1 &&
		document.addrouting.netmask.value == ""){
		alert("请填入子网掩码.");
        return false;
    }

	if(document.addrouting.interface.value == "Custom" &&
		document.addrouting.custom_interface.value == ""){
		alert("请填入自定义接口名字.");
		return false;
	}


	return true;
}


function display_on()
{
  if(window.XMLHttpRequest){ // Mozilla, Firefox, Safari,...
    return "table-row";
  } else if(window.ActiveXObject){ // IE
    return "block";
  }
}

function disableTextField (field)
{
  if(document.all || document.getElementById)
    field.disabled = true;
  else {
    field.oldOnFocus = field.onfocus;
    field.onfocus = skip;
  }
}

function enableTextField (field)
{
  if(document.all || document.getElementById)
    field.disabled = false;
  else {
    field.onfocus = field.oldOnFocus;
  }
}

function initTranslation()
{
	var e;
	e = document.getElementById("routingTitle");
	e.innerHTML = _("routing title");
	e = document.getElementById("routingIntroduction");
	e.innerHTML = _("routing Introduction");
	e = document.getElementById("routingAddRule");
	e.innerHTML = _("routing add rule");
	e = document.getElementById("routingDest");
	e.innerHTML = _("routing routing dest");
	e = document.getElementById("routingRange");
	e.innerHTML = _("routing range");
	e = document.getElementById("routingNetmask");
	e.innerHTML = _("routing netmask");
	e = document.getElementById("routingGateway");
	e.innerHTML = _("routing gateway");
	e = document.getElementById("routingInterface");
	e.innerHTML = _("routing interface");
	e = document.getElementById("routingCustom");
	e.innerHTML = _("routing custom");
	e = document.getElementById("routingComment");
	e.innerHTML = _("routing comment");
	e = document.getElementById("routingSubmit");
	e.value = _("routing submit");
	e = document.getElementById("routingReset");
	e.value = _("routing reset");
	e = document.getElementById("routingCurrentRoutingTableRules");
	e.innerHTML = _("routing del title");
	e = document.getElementById("routingNo");
	e.innerHTML = _("routing Number");
	e = document.getElementById("routingDelDest");
	e.innerHTML = _("routing del dest");
	e = document.getElementById("routingDelNetmask");
	e.innerHTML = _("routing del netmask");
	e = document.getElementById("routingDelGateway");
	e.innerHTML = _("routing del gateway");
	e = document.getElementById("routingDelFlags");
	e.innerHTML = _("routing del flags");
	e = document.getElementById("routingDelMetric");
	e.innerHTML = _("routing del metric");
	e = document.getElementById("routingDelRef");
	e.innerHTML = _("routing del ref");
	e = document.getElementById("routingDelUse");
	e.innerHTML = _("routing del use");
	e = document.getElementById("routingDelInterface");
	e.innerHTML = _("routing del interface");
	e = document.getElementById("routingDelComment");
	e.innerHTML = _("routing del comment");
	e = document.getElementById("routingDel");
	e.value = _("routing del");
	e = document.getElementById("routingDelReset");
	e.value = _("routing del reset");
	e = document.getElementById("routing host");
	e.innerHTML = _("routing host");
	e = document.getElementById("routing net");
	e.innerHTML = _("routing net");
	e = document.getElementById("routing LAN");
	e.innerHTML = _("routing LAN");
	if(document.getElementById("routing WAN")){
		e = document.getElementById("routing WAN");
		e.innerHTML = _("routing WAN");
	}
	e = document.getElementById("dynamicRoutingTitle");
	e.innerHTML = _("routing dynamic Title");
	e = document.getElementById("dynamicRoutingTitle2");
	e.innerHTML = _("routing dynamic Title2");
	e = document.getElementById("RIPDisable");
	e.innerHTML = _("routing dynamic rip disable");
	e = document.getElementById("RIPEnable");
	e.innerHTML = _("routing dynamic rip enable");
	e = document.getElementById("dynamicRoutingApply");
	e.value = _("routing dynamic rip apply");
	e = document.getElementById("dynamicRoutingReset");
	e.value = _("routing dynamic rip reset");
}

function onInit()
{
	//initTranslation();

	document.addrouting.hostnet.selectedIndex = 0;

	document.addrouting.netmask.readOnly = true;
	document.getElementById("routingNetmaskRow").style.visibility = "hidden";
	document.getElementById("routingNetmaskRow").style.display = "none";

	document.addrouting.interface.selectedIndex = 0;
	document.addrouting.custom_interface.value = "";
	document.addrouting.custom_interface.readOnly = true;

	document.dynamicRouting.RIPSelect.selectedIndex = <% getCfgZero(1, "RIPEnable"); %>;

	mydiv = document.getElementById("dynamicRoutingDiv");
	if(! <% getDynamicRoutingBuilt(); %>){
//alert("1")
		mydiv.style.display = "none";
//alert("2")
		mydiv.style.visibility = "hidden";
	}

}

function wrapDel(str, idle)
{
	if(idle == 1){
		document.write("<del>" + str + "</del>");
	}else
		document.write(str);
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
function hostnetChange()
{
	if(document.addrouting.hostnet.selectedIndex == 1){
		document.getElementById("routingNetmaskRow").style.visibility = "visible";
		document.getElementById("routingNetmaskRow").style.display = style_display_on();
		document.addrouting.netmask.readOnly = false;
		document.addrouting.netmask.focus();

	}else{
		document.addrouting.netmask.value = "";
		document.addrouting.netmask.readOnly = true;
		document.getElementById("routingNetmaskRow").style.visibility = "hidden";
		document.getElementById("routingNetmaskRow").style.display = "none";
	}
}
function interfaceChange()
{
	if(document.addrouting.interface.selectedIndex == 2){
		document.addrouting.custom_interface.readOnly = false;
		document.addrouting.custom_interface.focus();
	}else{
		document.addrouting.custom_interface.value = "";
		document.addrouting.custom_interface.readOnly = true;
	}
}

</script>
</head>
<body onLoad="onInit()">
<table width="90%" border="0" cellpadding="0" cellspacing="0" align="center">
  <tr>
    <td style="padding-top:30px;">
	  <table width="100%" border="0" cellpadding="0" cellspacing="0">
       <tr>
         <td class="nav">当前路径:KN-WR922 &gt;&gt;网络安全 &gt;&gt;静态路由设置</td>
       </tr>
	   <tr>
	     <td>&nbsp;</td>
	   </tr>
	   <tr>
            <td class="space" style="padding-left:10px; padding-right:10px;">&nbsp;&nbsp;&nbsp;本页面是用来设置路由器静态路由表的，如果您需要为特定的主机增加特定的路由，可以使用本功能。注意：系统自有的路由表是删除不了的，您只可以删除自己设置的路由表条目。 </td>
        </tr>
		  <tr>
		    <td>&nbsp;</td>
		  </tr>
	   <form method="post" name="addrouting" action="/goform/addRouting">
       <tr>
         <td>
           <table class="space" width="100%" border="0" cellspacing="0" cellpadding="0">
            <tr>
              <td colspan="2" class="titlebg">增加静态路由条目</td>
          	</tr>
			<tr>
              <td class="contentpadding" id="routingDest">目的:</td>
              <td><input size="16" name="dest" type="text" />
              (必填)</td>
            </tr>
			<tr>
              <td width="25%" class="contentpadding">主机/网络:</td>
              <td width="75%"><select name="hostnet" onChange="hostnetChange()">
                    <option select="" value="host" id="routing host">主机</option>
                    <option value="net"  id="routing net">网络</option></select></td>
			</tr>
			<tr id="routingNetmaskRow">
              <td width="25%" class="contentpadding" id="routingNetmask">子网掩码:</td>
              <td width="75%"><input size="16" name="netmask" type="text" />
              (当选择类型为网络时，必填)</td>
			</tr>
			<tr>
              <td width="25%" class="contentpadding" id="routingGateway">网关:</td>
              <td width="75%"><input size="16" name="gateway" type="text" />
              (必填)</td>
			</tr>
			<tr>
              <td width="25%" class="contentpadding" id="routingInterface">接口:</td>
              <td width="75%"><select name="interface" onChange="interfaceChange()">
				<option select="" value="LAN" id="routing LAN">LAN</option>
				<script language="JavaScript" type="text/javascript">
					if(opmode == "1")
						document.write("<option value=\"WAN\" id=\"routing WAN\">WAN</option>");
				</script>
				<option value="Custom" id="routingCustom">自定义</option></select>&nbsp;<input size="16" name="custom_interface" type="text">
				(一般来说用下拉列表选择是WAN或者LAN 就可以了)</td>
			</tr>
			<tr>
              <td width="25%" class="contentpadding" id="routingComment">说明:</td>
              <td width="75%"><input name="comment2" size="16" maxlength="32" type="text" />
              (选填)</td>
			</tr>
            
           </table>
		 </td>
       </tr>
	   <tr><td>&nbsp;</td></tr>
       <tr>
         <td class="contentpadding" ><input class="button" value="提 交" id="routingSubmit" name="addFilterPort" onClick="return formCheck()" type="submit">&nbsp;&nbsp;<input value="取 消" id="routingReset" name="reset" class="button" type="reset"><input type="hidden" value="/staticRoute.asp" name="submit-url"></td>
       </tr>
	   </form>
	   <tr>
	     <td>&nbsp;</td>
	   </tr><form action="/goform/delRouting" method="post" name="delRouting">
	   <tr>
	     <td>
		   <table class="space" width=100% border="0" cellpadding="0" cellspacing="0">	
			 <tr>
			   <td colspan="10" class="titlebg" id="routingCurrentRoutingTableRules">当前路由表</td>
			 </tr>
			 <tr>
			    <td id="routingNo"> NO.</td>
				<td id="routingDelDest"> 目的地址</td>
				<td id="routingDelNetmask"> 子网掩码</td>
				<td id="routingDelGateway"> 网关</td>
				<td id="routingDelFlags"> 标记</td>
				<td id="routingDelMetric"> Metric</td>
				<td id="routingDelRef"> Ref</td>
				<td id="routingDelUse"> Use</td>
				<td id="routingDelInterface"> 接口</td>
				<td id="routingDelComment"> 说明</td>
			  </tr>
	<script language="JavaScript" type="text/javascript">
	var i;
	var entries = new Array();
	var all_str = <% getRoutingTable(); %>;

	entries = all_str.split(";");
	for(i=0; i<entries.length; i++){
		var one_entry = entries[i].split(",");


		true_interface[i] = one_entry[0];
		destination[i] = one_entry[1];
		gateway[i] = one_entry[2];
		netmask[i] = one_entry[3];
		flags[i] = one_entry[4];
		ref[i] = one_entry[5];
		use[i] = one_entry[6];
		metric[i] = one_entry[7];
		category[i] = parseInt(one_entry[8]);
		interface[i] = one_entry[9];
		idle[i] = parseInt(one_entry[10]);
		comment[i] = one_entry[11];
		if(comment[i] == " " || comment[i] == "")
			comment[i] = "&nbsp";
	}

	for(i=0; i<entries.length; i++){
		if(category[i] > -1){
			document.write("<tr>");
			document.write("<td>");
			document.write(i+1);
			document.write("<input type=checkbox name=DR"+ category[i] + 
				" value=\""+ destination[i] + " " + netmask[i] + " " + true_interface[i] +"\">");
			document.write("</td>");
		}else{
			document.write("<tr>");
			document.write("<td>"); 	document.write(i+1);			 	document.write("</td>");
		}

		document.write("<td>"); 	wrapDel(destination[i], idle[i]); 	document.write("</td>");
		document.write("<td>"); 	wrapDel(netmask[i], idle[i]);		document.write("</td>");
		document.write("<td>"); 	wrapDel(gateway[i], idle[i]); 		document.write("</td>");
		document.write("<td>"); 	wrapDel(flags[i], idle[i]);			document.write("</td>");
		document.write("<td>"); 	wrapDel(metric[i], idle[i]);		document.write("</td>");
		document.write("<td>"); 	wrapDel(ref[i], idle[i]);			document.write("</td>");
		document.write("<td>"); 	wrapDel(use[i], idle[i]);			document.write("</td>");

//		if(interface[i] == "LAN")
//			interface[i] = _("routing LAN");
//		else if(interface[i] == "WAN")
//			interface[i] = _("routing WAN");
//		else if(interface[i] == "Custom")
//			interface[i] = _("routing custom");

		document.write("<td>"); 	wrapDel(interface[i] + "(" +true_interface[i] + ")", idle[i]);		document.write("</td>");
		document.write("<td>"); 	wrapDel(comment[i], idle[i]);		document.write("</td>");
		document.write("</tr>\n");
	}
	</script>
    		 
			</table>
		 </td>
	   </tr>
	   <tr>
	     <td>&nbsp;</td>
	   </tr>
       <tr>
         <td class="contentpadding"><input value="删除所选项" id="routingDel" name="deleteSelPortForward" onClick="return deleteClick()" class="button3" type="submit">&nbsp;&nbsp;<input value="取 消" id="routingDelReset" name="reset" class="button" type="reset"><input type="hidden" value="/staticRoute.asp" name="submit-url"></td>
       </tr>
		</form>
	   <tr>
	     <td>&nbsp;</td>
	   </tr>
<div id=dynamicRoutingDiv>
<form method=post name="dynamicRouting" action=/goform/dynamicRouting>
       <tr style="visibility: hidden;">
         <td>
           <table class="space" id=l2tp width="100%" border="0" cellspacing="0" cellpadding="0">
             <tr>
               <td colspan="2" class="titlebg">动态路由设置</td>
             </tr>
			 <tr>
			   <td width="25%" class="contentpadding" id="portFilterMac">RIP:</td>
			   <td><select name="RIPSelect" size="1">
                    <option value="0" id="RIPDisable">禁用</option>
                    <option value="1" id="RIPEnable">启用</option></select></td>
			 </tr>
		   </table>
         </td>
	   </tr>
	   <tr style="visibility: hidden;">
		  <td>&nbsp;</td>
	   </tr>
	   <tr style="visibility: hidden;">
		  <td class="contentpadding"><input type="submit" class="button"  value="提 交" id="dynamicRoutingApply" name="dynamicRoutingApply" >&nbsp;&nbsp;<input type="reset" class="button"  value="取 消" id="dynamicRoutingReset" name="dynamicRoutingReset"><input type="hidden" value="/port_filter.asp" name="submit-url2" /></td>
	   </tr>
	   <tr style="visibility: hidden;">
		  <td>&nbsp;</td>
	   </tr>
</form>
</div>
	 </table>
	</td>
  </tr>
  <tr>
	<td>&nbsp;</td>
  </tr>
</table>
</body>
</html>
