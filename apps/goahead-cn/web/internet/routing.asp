<html>
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Expires" content="-1">
<meta http-equiv="Content-Type" content="text/html; charset=<% getCharset(); %>">
<link rel="stylesheet" href="../style/normal_ws.css" type="text/css">
<script language="javascript" src="../js/language_<% getCfgZero(1, "LanguageType"); %>.js"></script>
<script language="javascript" src="../js/common.js"></script>
<script language="javascript">
var opmode = "<% getCfgZero(1, "OperationMode"); %>";
var lan_ip = "<% getLanIp(); %>";
var wan_ip = "<% getWanIp(); %>";
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
//var idle = new Array();
var comment = new Array();
var entries = new Array();
var all_str = <% getRoutingTable(); %>;

entries = all_str.split(";");

function deleteClick()
{
	return true;
}

function formCheck()
{
	if (!isIpAddrMsg(document.addrouting.dest.value, MM_destip)) 
		return false; 
    
	if (document.addrouting.hostnet.selectedIndex == 1) 
		if (!isMaskAddrMsg(document.addrouting.netmask.value, MM_submask)) 
			return false; 
		
	if (!isIpAddrMsg(document.addrouting.gateway.value, MM_gateway_address)) 
		return false; 
	
	if (document.addrouting.interface.selectedIndex == 0){
		if (!isIpSubnet(document.addrouting.gateway.value, "255.255.255.0", document.addrouting.dest.value)) {
			alert(JS_msg13);
			return false;
		}
	}

	return true;
}

function Load_Setting()
{
	document.addrouting.hostnet.selectedIndex = 0;
	document.getElementById("routingNetmaskRow").style.display = "none";
	document.addrouting.interface.selectedIndex = 0;
	document.addrouting.gateway.value = lan_ip;
	document.addrouting.custom_interface.value = "";
	document.dynamicRouting.RIPSelect.selectedIndex = <% getCfgZero(1, "RIPEnable"); %>;
		
	if (!<% getDynamicRoutingBuilt(); %>)
		document.getElementById("dynamicRoutingDiv").style.display = "none";
		
	var i;
	var j=0;
	
	for (i=0; i<entries.length; i++){
		var one_entry = entries[i].split(",");
		category[i] = parseInt(one_entry[8]);
	}

	for (i=0; i<entries.length; i++){
		if (category[i] > -1)  j++;
	}
	
	if (j==0){
		document.delRouting.deleteSelRouter.disabled = true;
 		document.delRouting.reset.disabled = true;
	}
	else{
		document.delRouting.deleteSelRouter.disabled = false;
 		document.delRouting.reset.disabled = false;
	}
}

function hostnetChange()
{
	if (document.addrouting.hostnet.selectedIndex == 1){
		document.getElementById("routingNetmaskRow").style.display = "";
		document.addrouting.netmask.value = "255.255.255.255";
		document.addrouting.netmask.readOnly = true;
	}
	else{
		document.getElementById("routingNetmaskRow").style.display = "none";
		document.addrouting.netmask.value = "255.255.255.255";
		document.addrouting.netmask.readOnly = true;
	}
}

function interfaceChange()
{	
	wanip_tmp = wan_ip.split('.');
	if (document.addrouting.interface.selectedIndex == 1){
		if (wan_ip == "")
			alert(JS_msg48);
		else
			document.addrouting.gateway.value = wanip_tmp[0]+"."+wanip_tmp[1]+"."+wanip_tmp[2]+".";
	}
	else{
		document.addrouting.gateway.value = lan_ip;
	}
}

function resetForm()
{
	location=location; 
}
</script>
</head>
<body onLoad="Load_Setting()" class="mainbody">
<blockquote>
<table width=700><tr><td>
<table width=100% border=0 cellpadding=3 cellspacing=1> 
<tr><td class="title"><script>dw(MM_routing_table)</script></td></tr>
<tr><td><script>dw(JS_msg_routing)</script></td></tr>
<tr><td><hr></td></tr>
</table>

<form action="/goform/delRouting" method="post" name="delRouting">
<input type="hidden" name="submit-url" value="/internet/routing.asp">
<p><b><script>dw(MM_routing_table_list)</script>:<script>document.write(JS_msg1);</script></b></p>
<table width=100% border=1 cellpadding=3 style="border-collapse: collapse" bordercolor="#C8C8C8">
	<tr bgcolor=#f2f2f2>
		<td align="center"><b>No.</b></td>
		<td align="center"><b><script>dw(MM_destip)</script></b></td>
		<td align="center"><b><script>dw(MM_submask)</script></b></td>
		<td align="center"><b><script>dw(MM_gateway_address)</script></b></td>
		<td align="center"><b><script>dw(MM_flags)</script></b></td>
		<td align="center"><b>Metric</b></td>
		<td align="center"><b>Ref</b></td>
		<td align="center"><b>Use</b></td>
		<td align="center"><b><script>dw(MM_interface)</script></b></td>
		<td align="center"><b><script>dw(MM_comment)</script></b></td>
	</tr>
	<script language="javascript">
	var i;
	for(i=0; i<entries.length; i++)
	{
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
		//idle[i] = parseInt(one_entry[10]);
		comment[i] = one_entry[11];
		if(comment[i] == " " || comment[i] == "")
			comment[i] = "&nbsp";
	}

	for(i=0; i<entries.length; i++)
	{
		if(category[i] > -1)
		{
			document.write("<tr align=center>");
			document.write("<td>");
			document.write(i+1);
			document.write("<input type=checkbox name=DR"+ category[i] + " value=\""+ destination[i] + " " + netmask[i] + " " + true_interface[i] +"\">");
			document.write("</td>");
		}
		else
		{
			document.write("<tr align=center>");
			document.write("<td>"); 	document.write(i+1);			 	document.write("</td>");
		}

		document.write("<td>"); 	document.write(destination[i]); 	document.write("</td>");
		document.write("<td>"); 	document.write(netmask[i]);		document.write("</td>");
		document.write("<td>"); 	document.write(gateway[i]); 		document.write("</td>");
		document.write("<td>"); 	document.write(flags[i]);			document.write("</td>");
		document.write("<td>"); 	document.write(metric[i]);		document.write("</td>");
		document.write("<td>"); 	document.write(ref[i]);			document.write("</td>");
		document.write("<td>"); 	document.write(use[i]);			document.write("</td>");
		document.write("<td>"); 	document.write(interface[i] + "(" +true_interface[i] + ")");		document.write("</td>");
		document.write("<td>"); 	document.write(comment[i]);		document.write("</td>");
		document.write("</tr>\n");
	}
	</script>
</table>

<br>
<table width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr>
    <td>
      <script>dw('<input type=submit class=button value="'+BT_delete+'" name="deleteSelRouter" onClick="return deleteClick()"> &nbsp; &nbsp;\
      <input type=button class=button value="'+BT_reset+'" name="reset" onClick="resetForm();">')</script>
    </td>
  </tr>
</table>
</form>

<br>
<form method="post" name="addrouting" action="/goform/addRouting">
<input type="hidden" name="submit-url" value="/internet/routing.asp">
<table width=100% border=0 cellpadding=3 cellspacing=1> 
<tr>
    <td class="title2" colspan="2"><script>dw(MM_add)</script><hr></td>
</tr>
<tr>
	<td class="thead"><script>dw(MM_destip)</script>:</td>
	<td><input type="text" name="dest" maxlength="15"></td>
</tr>
<tr>
	<td class="thead"><script>dw(MM_host)</script>/<script>dw(MM_net)</script>:</td>
	<td><select name="hostnet" onChange="hostnetChange()">
		<option value="host"><script>dw(MM_host)</script></option>
		<option value="net"><script>dw(MM_net)</script></option></select></td>
</tr>
<tr id="routingNetmaskRow">
	<td class="thead"><script>dw(MM_submask)</script>:</td>
	<td><input type="text" name="netmask" maxlength="15"></td>
</tr>
<tr>
	<td class="thead"><script>dw(MM_gateway_address)</script>:</td>
	<td><input type="text" name="gateway" maxlength="15"></td>
</tr>
<tr>
	<td class="thead"><script>dw(MM_interface)</script>:</td>
	<td><select name="interface" onChange="interfaceChange()">
		<option value="LAN">LAN</option>
		<script language="javascript">
		if(opmode == "1")
			document.write("<option value=\"WAN\">WAN</option>");
		</script>
		</select>
		<input style="display:none" type="text" name="custom_interface">
	</td>
</tr>
<tr>
	<td class="thead"><script>dw(MM_comment)</script>:</td>
	<td><input type="text" name="comment" maxlength="10"></td>
</tr>
</table>

<br>
<table width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr>
    <td>
      <script>dw('<input type=submit class=button value="'+BT_apply+'" onClick="return formCheck()"> &nbsp; &nbsp;\
      <input type=button class=button value="'+BT_reset+'" onClick="resetForm();">')</script>
    </td>
  </tr>
</table>
</form>

<div id=dynamicRoutingDiv>
<form method=post name="dynamicRouting" action=/goform/dynamicRouting>
<input type="hidden" name="submit-url" value="/internet/routing.asp">
<table width=100% border=0 cellpadding=3 cellspacing=1> 
<tr>
	<td class="title2" colspan="2">Set Dynamic routing<hr></td>
</tr>
<tr>
	<td class="thead">RIP:</td>
	<td><select name="RIPSelect">
	<option value=0><script>dw(MM_disable)</script></option>
	<option value=1><script>dw(MM_enable)</script></option></select></td>
</tr>
</table>

<br>
<table width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr>
    <td>
      <script>dw('<input type=submit class=button value="'+BT_apply+'"> &nbsp; &nbsp;\
      <input type=button class=button value="'+BT_reset+'" onClick="resetForm();">')</script>
    </td>
  </tr>
</table>
</form>
</div>

</td></tr></table>
</blockquote>
</body></html>
