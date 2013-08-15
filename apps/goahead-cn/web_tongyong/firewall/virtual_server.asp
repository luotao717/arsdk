<html>
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Expires" content="-1">
<meta http-equiv="Content-Type" content="text/html; charset=<% getCharset(); %>">
<link rel="stylesheet" href="../style/normal_ws.css" type="text/css">
<script language="javascript" src="../js/language_<% getCfgZero(1, "LanguageType"); %>.js"></script>
<script language="javascript" src="../js/common.js"></script>
<script language="javascript">
var rules_num = <% getSinglePortForwardRuleNumsASP(); %>;
var lanIP = "<% getLanIp(); %>";
var lanMask = "<% getLanNetmask(); %>";

function Load_Setting()
{
	updateState();
}

function deleteClick()
{
   	for(i=0; i< rules_num; i++) {
		var tmp = eval("document.singlePortForwardDelete.delRule"+i);
		if(tmp.checked == true)
			return true;
	}
	alert(JS_msg18);
	return false;
}

function formCheck()
{
	if (rules_num >= 15) {
		alert(JS_msg19);
		return false;
	}
	
	if ((document.singlePortForwardBasicSettings.singlePortForwardEnabled.options.selectedIndex == 0) && 
		(document.singlePortForward.ip_address.value == "" && document.singlePortForward.publicPort.value == "" && 
		 document.singlePortForward.privatePort.value == "" &&	document.singlePortForward.comment.value == ""))
		return true;
	
	if (!isIpAddrMsg(document.singlePortForward.ip_address.value, MM_ipaddr)) 
		return false;
	
	if (!isIpSubnet(document.singlePortForward.ip_address.value, lanMask, lanIP)) {
		alert(JS_msg20);
		return false;
	}
	
	if (!isPortMsg(document.singlePortForward.publicPort.value)) 
		return false;
	
	if (!isPortMsg(document.singlePortForward.privatePort.value)) 
		return false;
	
	return true;
}

function updateState()
{
    if (!rules_num) {
 		disableButton(document.singlePortForwardDelete.deleteSelSinglePortForward);
 		disableButton(document.singlePortForwardDelete.reset);
	}
	else{
        enableButton(document.singlePortForwardDelete.deleteSelSinglePortForward);
        enableButton(document.singlePortForwardDelete.reset);
	}

	if (document.singlePortForwardBasicSettings.singlePortForwardEnabled.options.selectedIndex == 1){
		enableTextField(document.singlePortForward.ip_address);
		enableTextField(document.singlePortForward.publicPort);
		enableTextField(document.singlePortForward.privatePort);
		enableTextField(document.singlePortForward.protocol);
		enableTextField(document.singlePortForward.comment);
		enableButton(document.singlePortForward.apply);
        enableButton(document.singlePortForward.reset);
	}
	else{
		disableTextField(document.singlePortForward.ip_address);
		disableTextField(document.singlePortForward.publicPort);
		disableTextField(document.singlePortForward.privatePort);
		disableTextField(document.singlePortForward.protocol);
		disableTextField(document.singlePortForward.comment);
		disableButton(document.singlePortForward.apply);
 		disableButton(document.singlePortForward.reset);
	}
}

var xml = false;
function portForwardSubmit(value)
{
	xml = false;
    if (window.XMLHttpRequest) { // Mozilla, Safari,...
        xml = new XMLHttpRequest();
        if (xml.overrideMimeType) {
            xml.overrideMimeType('text/xml');
        }
    } else if (window.ActiveXObject) { // IE
        try {
            xml = new ActiveXObject("Msxml2.XMLHTTP");
        } catch (e) {
            try {
            xml = new ActiveXObject("Microsoft.XMLHTTP");
            } catch (e) {}
        }
    }
    if (!xml) {
        alert(JS_msg5);
        return false;
    }
    xml.onreadystatechange = doChange;
	xml.open('POST', '/goform/singlePortForwardBasicSettings', true);
	xml.send('singleforwardEnabled='+value);
}

function doChange()
{
    if (xml.readyState == 4){
		if (xml.status == 200){
			window.location.reload();
		} else {
			//alert(JS_msg6);
		}
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
<tr><td class="title"><script>dw(MM_vserver_settings)</script></td></tr>
<tr><td><script>dw(JS_msg_virtual_server)</script></td></tr>
<tr><td><hr></td></tr>
</table>

<form method=post name="singlePortForwardBasicSettings" action=/goform/singlePortForwardBasicSettings>
<input type="hidden" name="submit-url" value="/firewall/virtual_server.asp">
<input type="hidden" name="singleforwardEnabled">
<table width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr>
	<td class="thead"><script>dw(MM_port_forwarding)</script>:</td>
	<td><select onChange="updateState();portForwardSubmit(this.value)" name="singlePortForwardEnabled">
	<option value=0 <% getSinglePortForwardEnableASP(0); %>><script>dw(MM_disable)</script></option>
    <option value=1 <% getSinglePortForwardEnableASP(1); %>><script>dw(MM_enable)</script></option>
    </select></td>
  </tr>
</table>
</form>

<form method=post name="singlePortForward" action=/goform/singlePortForward>
<input type="hidden" name="submit-url" value="/firewall/virtual_server.asp">
<table width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr>
    <td class="title2" colspan="2"><script>dw(MM_add)</script><hr></td>
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
   		<option selected value="TCP&UDP">TCP+UDP</option>
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
      <script>dw('<input type=submit class=button value="'+BT_apply+'" name=apply onClick="return formCheck()"> &nbsp; &nbsp;\
      <input type=button class=button value="'+BT_reset+'" name=reset onClick="resetForm();">')</script>
    </td>
  </tr>
</table>
</form>

<br>
<form action=/goform/singlePortForwardDelete method=POST name="singlePortForwardDelete">
<input type="hidden" name="submit-url" value="/firewall/virtual_server.asp">
<p><b><script>dw(MM_vserver_list)</script>:<script>document.write(JS_msg51);</script></b></p>
<table width=100% border=1 cellpadding=3 style="border-collapse: collapse" bordercolor="#C8C8C8">
	<tr bgcolor=#f2f2f2>
		<td><b>No.</b></td>
		<td align=center><b><script>dw(MM_ipaddr)</script><b></td>
		<td align=center><b><script>dw(MM_pub_port)</script></b></td>
		<td align=center><b><script>dw(MM_pri_port)</script></b></td>
		<td align=center><b><script>dw(MM_protocol)</script></b></td>
		<td align=center><b><script>dw(MM_comment)</script></b></td>
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
</blockquote>
</body></html>
