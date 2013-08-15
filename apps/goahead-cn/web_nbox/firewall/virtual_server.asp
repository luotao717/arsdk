<html>
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Expires" content="-1">
<meta http-equiv="Content-Type" content="text/html; charset=<% getCharset(); %>">
<link rel="stylesheet" href="../style/normal_ws.css" type="text/css">
<script language="javascript" src="../js/language_<% getCfgZero(1, "LanguageType"); %>.js"></script>
<script language="javascript" src="../js/common.js"></script>
<script language="javascript">
var rules_num_singleport = <% getSinglePortForwardRuleNumsASP(); %> ;
var lanIP = "<% getLanIp(); %>";
var lanMask = "<% getLanNetmask(); %>";

function Load_Setting()
{
	singlePortChange();
}

function deleteClick()
{
    return true;
}

function singlePortFormCheck()
{
	if (rules_num_singleport > 15)
	{
		alert(JS_msg19);
		return false;
	}
	
	if (!document.singlePortForward.singlePortForwardEnabled.options.selectedIndex)	
		return true;
	
	if (document.singlePortForward.ip_address.value == "" && 
		document.singlePortForward.publicPort.value == "" && 
		document.singlePortForward.privatePort.value   == "" && 
		document.singlePortForward.comment.value  == "")	
		return true;
	
	// exam IP address
	if (!ipCheck(document.singlePortForward.ip_address, MM_ipaddr, 0)) 
		return false;
	
	if (!subnetCheck(document.singlePortForward.ip_address.value, lanMask, lanIP)) 
	{
		alert(JS_msg20);
		document.singlePortForward.ip_address.focus();
		return false;
	}
	
	// exam public port
	if (!portCheckNullMsg(document.singlePortForward.publicPort, MM_pub_port)) 
		return false;
	
	//exam private port
	if (!portCheckNullMsg(document.singlePortForward.privatePort, MM_pri_port)) 
		return false;
	
	return true;
}

function singlePortChange()
{
    if (!rules_num_singleport)
	{
 		disableButton(document.singlePortForwardDelete.deleteSelSinglePortForward);
 		disableButton(document.singlePortForwardDelete.reset);
	}
	else
	{
        enableButton(document.singlePortForwardDelete.deleteSelSinglePortForward);
        enableButton(document.singlePortForwardDelete.reset);
	}

	if (document.singlePortForward.singlePortForwardEnabled.options.selectedIndex == 1)
	{
		enableTextField(document.singlePortForward.ip_address);
		enableTextField(document.singlePortForward.publicPort);
		enableTextField(document.singlePortForward.privatePort);
		enableTextField(document.singlePortForward.protocol);
		enableTextField(document.singlePortForward.comment);
	}
	else
	{
		disableTextField(document.singlePortForward.ip_address);
		disableTextField(document.singlePortForward.publicPort);
		disableTextField(document.singlePortForward.privatePort);
		disableTextField(document.singlePortForward.protocol);
		disableTextField(document.singlePortForward.comment);
	}
}

function resetForm()
{
	location=location; 
}
</script>
</head>
<body onLoad="Load_Setting()">
<table width=700><tr><td>
<table width=100% border=0 cellpadding=3 cellspacing=1> 
<tr><td class="title"><script>dw(MM_vserver_settings)</script></td></tr>
<tr><td><script>dw(JS_msg138)</script></td></tr>
<tr><td><hr></td></tr>
</table>

<form method=post name="singlePortForward" action=/goform/singlePortForward>
<input type="hidden" name="submit-url" value="/firewall/port_forward.asp">
<table width=100% border=0 cellpadding=3 cellspacing=1> 
<tr>
	<td class="title2" colspan="2"><script>dw(MM_vserver_settings)</script><hr></td>
</tr>
<tr>
	<td class="thead"><script>dw(MM_function)</script>:</td>
	<td><select onChange="singlePortChange()" name="singlePortForwardEnabled">
	<option value=0 <% getSinglePortForwardEnableASP(0); %>><script>dw(MM_disable)</script></option>
    <option value=1 <% getSinglePortForwardEnableASP(1); %>><script>dw(MM_enable)</script></option>
    </select></td>
</tr>
<tr>
	<td class="thead"><script>dw(MM_ipaddr)</script>:</td>
	<td><input type="text" maxlength="15" name="ip_address"></td>
</tr>
<tr>
	<td class="thead"><script>dw(MM_pub_port)</script>:</td>
	<td><input type="text" maxlength="5" size="5" name="publicPort"> (1-65535)</td>
</tr>
<tr>
    <td class="thead"><script>dw(MM_pri_port)</script>:</td>
    <td><input type="text" maxlength="5" size="5" name="privatePort"> (1-65535)</td>
</tr>
<tr>
	<td class="thead"><script>dw(MM_protocol)</script>:</td>
	<td><select name="protocol">
   		<option selected value="TCP&UDP">TCP & UDP</option>
		<option value="TCP">TCP</option>
   		<option value="UDP">UDP</option>
   		</select></td>
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
      <script>dw('<input type=submit class=button value="'+BT_apply+'" onClick="return singlePortFormCheck()"> &nbsp; &nbsp;\
      <input type=button class=button value="'+BT_reset+'" onClick="resetForm();">')</script>
    </td>
  </tr>
</table>
</form>

<br>
<form action=/goform/singlePortForwardDelete method=POST name="singlePortForwardDelete">
<input type="hidden" name="submit-url" value="/firewall/port_forward.asp">
<table width=100% border=0 cellpadding=3 cellspacing=1> 
	<tr>
		<td class="title2" colspan="6"><script>dw(MM_vserver_list)</script>:<script>document.write(JS_msg93);</script></td>
	</tr>
	<tr bgcolor=#C0C0C0>
		<td>No.</td>
		<td align=center><script>dw(MM_ipaddr)</script></td>
		<td align=center><script>dw(MM_pub_port)</script></td>
		<td align=center><script>dw(MM_pri_port)</script></td>
		<td align=center><script>dw(MM_protocol)</script></td>
		<td align=center><script>dw(MM_comment)</script></td>
	</tr>
	<% showSinglePortForwardRulesASP(); %>
</table>

<br>
<table width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr>
    <td>
      <script>dw('<input type=submit class=button value="'+BT_delete+'" name="deleteSelSinglePortForward" onClick="return deleteClick()"> &nbsp; &nbsp;\
      <input type=button class=button value="'+BT_reset+'" name="reset" onClick="resetForm();">')</script>
    </td>
  </tr>
</table>
</form>

</td></tr></table>
</body></html>
