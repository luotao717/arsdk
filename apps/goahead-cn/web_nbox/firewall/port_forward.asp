<html>
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Expires" content="-1">
<meta http-equiv="Content-Type" content="text/html; charset=<% getCharset(); %>">
<link rel="stylesheet" href="../style/normal_ws.css" type="text/css">
<script language="javascript" src="../js/language_<% getCfgZero(1, "LanguageType"); %>.js"></script>
<script language="javascript" src="../js/common.js"></script>
<script language="javascript">
var rules_num = <% getPortForwardRuleNumsASP(); %> ;
var lanIP = "<% getLanIp(); %>";
var lanMask = "<% getLanNetmask(); %>";

function Load_Setting()
{
	updateState();
}

function deleteClick()
{
   	for(i=0; i< rules_num; i++)
	{
		var tmp = eval("document.portForwardDelete.delRule"+i);
		if(tmp.checked == true)
			return true;
	}
	alert(JS_msg18);
	return false;
   // return true;
}

function formCheck()
{
	if (rules_num >= 15)
	{
		alert(JS_msg19);
		return false;
	}
	
	if ((document.portForwardLBasicSettings.portForwardEnabled.options.selectedIndex == 0) && 
		(document.portForward.ip_address.value == "" && document.portForward.fromPort.value == "" && 
		 document.portForward.toPort.value == "" &&	document.portForward.comment.value == ""))
		return true;
			
	// exam IP address
	if (!ipCheck(document.portForward.ip_address, MM_ipaddr, 0)) 
		return false;
	
	if (!subnetCheck(document.portForward.ip_address.value, lanMask, lanIP)) 
	{
		alert(JS_msg20);
		document.portForward.ip_address.focus();
		return false;
	}
	
	// exam Port
	if (!portCheckNullMsg(document.portForward.fromPort, MM_1st_port)) 
		return false;	
	
	if (document.portForward.toPort.value != "")
	{
		if (!portCheckMsg(document.portForward.toPort, MM_2nd_port)) 
			return false;
		
		if (!portRangeCheck(document.portForward.fromPort.value,document.portForward.toPort.value)) 
		{
			alert(JS_msg27);
			document.portForward.fromPort.focus();
			return false;
		}
   }
   
   return true;
}

function updateState()
{
    if (!rules_num)
	{
 		disableButton(document.portForwardDelete.deleteSelPortForward);
 		disableButton(document.portForwardDelete.reset);
	}
	else
	{
        enableButton(document.portForwardDelete.deleteSelPortForward);
        enableButton(document.portForwardDelete.reset);
	}
	
    if (document.portForwardLBasicSettings.portForwardEnabled.options.selectedIndex == 1)
	{
		enableTextField(document.portForward.ip_address);
		enableTextField(document.portForward.fromPort);
		enableTextField(document.portForward.toPort);
		enableTextField(document.portForward.protocol);
		enableTextField(document.portForward.comment);
		enableButton(document.portForward.apply);
        enableButton(document.portForward.reset);
	}
	else
	{
		disableTextField(document.portForward.ip_address);
		disableTextField(document.portForward.fromPort);
		disableTextField(document.portForward.toPort);
		disableTextField(document.portForward.protocol);
		disableTextField(document.portForward.comment);
		disableButton(document.portForward.apply);
 		disableButton(document.portForward.reset);
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
	xml.open('POST', '/goform/portForwardBasicSettings', true);
	xml.send('forwardEnabled='+value);
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
<body onLoad="Load_Setting()">
<table width=700><tr><td>
<table width=100% border=0 cellpadding=3 cellspacing=1> 
<tr><td class="title"><script>dw(MM_port_forwarding_settings)</script></td></tr>
<tr style="display:none"><td><script>dw(JS_msg139)</script></td></tr>
<tr><td><hr></td></tr>
</table>

<form method=post name="portForwardLBasicSettings" action=/goform/portForwardBasicSettings>
<input type="hidden" name="submit-url" value="/firewall/port_forward.asp">
<input type="hidden" name="forwardEnabled">
<table width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr>
	<td class="thead"><script>dw(MM_port_forwarding)</script>:</td>
	<td><select onChange="updateState();portForwardSubmit(this.value)" name="portForwardEnabled">
	<option value=0 <% getPortForwardEnableASP(0); %>><script>dw(MM_disable)</script></option>
    <option value=1 <% getPortForwardEnableASP(1); %>><script>dw(MM_enable)</script></option>
    </select></td>
  </tr>
</table>
</form>

<form method=post name="portForward" action=/goform/portForward>
<input type="hidden" name="submit-url" value="/firewall/port_forward.asp">
<table width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr>
    <td class="title2" colspan="2"><script>dw(MM_add)</script><hr></td>
  </tr>
  <tr>
	<td class="thead"><script>dw(MM_ipaddr)</script>:</td>
	<td><input type="text" maxlength="15" name="ip_address"></td>
  </tr>
  <tr>
	<td class="thead"><script>dw(MM_portrange)</script>:</td>
	<td><input type="text" maxlength="5" size="5" name="fromPort"> - <input type="text" maxlength="5" size="5" name="toPort"> (1-65535)</td>
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
<form action=/goform/portForwardDelete method=POST name="portForwardDelete">
<input type="hidden" name="submit-url" value="/firewall/port_forward.asp">
<p><b><script>dw(MM_port_forwarding_list)</script>:<script>document.write(JS_msg93);</script></b></p>
<table width=100% border=1 cellpadding=3 style="border-collapse: collapse" bordercolor="#C8C8C8">
	<tr bgcolor=#f2f2f2>
		<td><b>No.</b></td>
		<td align=center><b><script>dw(MM_ipaddr)</script></b></td>
		<td align=center><b><script>dw(MM_portrange)</script></b></td>
		<td align=center><b><script>dw(MM_protocol)</script></b></td>
		<td align=center><b><script>dw(MM_comment)</script></b></td>
	</tr>
	<% showPortForwardRulesASP(); %>
</table>

<br>
<table width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr>
    <td>
      <script>dw('<input type=submit class=button value="'+BT_delete+'" name="deleteSelPortForward" onClick="return deleteClick()"> &nbsp; &nbsp;\
      <input type=button class=button value="'+BT_reset+'" name="reset" onClick="resetForm();">')</script>
    </td>
  </tr>
</table>
</form>

</td></tr></table>
</body></html>
