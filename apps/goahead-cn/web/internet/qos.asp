<html>
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Expires" content="-1">
<meta http-equiv="Content-Type" content="text/html; charset=<% getCharset(); %>">
<link rel="stylesheet" href="../style/normal_ws.css" type="text/css">
<script language="javascript" src="../js/language_<% getCfgZero(1, "LanguageType"); %>.js"></script>
<script language="javascript" src="../js/common.js"></script>
<script language="javascript">
var rules_num = <% getIpQosRuleNumsASP(); %>;
var lanIP = "<% getLanIp(); %>";
var lanMask = "<% getLanNetmask(); %>";
var max_length = 99999999;

function Load_Setting()
{
	if (!rules_num)	{
 		disableButton(document.ipQosDelete.deleteSelQos);
 		disableButton(document.ipQosDelete.reset);
	}
	else{
        enableButton(document.ipQosDelete.deleteSelQos);
        enableButton(document.ipQosDelete.reset);
	}
			
	updateState();
}

function deleteClick()
{
   	for(i=1; i<= rules_num; i++){
		var tmp = eval("document.ipQosDelete.delRule"+i);
		if(tmp.checked == true)
			return true;
	}
	alert(JS_msg18);
	return false;
}

function formCheck()
{
	if ( rules_num >= 10 ) {
		alert(JS_msg22);
		return false;
	}
	
	if (document.ipQosBasicSettings.enabled.options.selectedIndex == 1){	
		if (!document.ipQos.automaticUplinkSpeed.options.selectedIndex == 1) {
			if (!isNumberRange(document.ipQos.manualUplinkSpeed.value, 100, max_length)) { 
				alert(MM_manual_uplink_speed + JS_msg98);
				return false;
			}
		}
		
		if (!document.ipQos.automaticDownlinkSpeed.options.selectedIndex == 1) {
			if (!isNumberRange(document.ipQos.manualDownlinkSpeed.value, 100, max_length)) { 
				alert(MM_manual_downlink_speed + JS_msg98);
				return false;
			}
		}
	}
	
	if ((document.ipQosBasicSettings.enabled.options.selectedIndex == 1) && 		
		(document.ipQos.ipStart.value=="" && document.ipQos.ipEnd.value=="" &&	document.ipQos.mac.value=="" &&
		document.ipQos.bandwidth.value=="" && document.ipQos.bandwidth_downlink.value=="" && document.ipQos.comment.value==""))
		return true;
	
	//ip address		
	if (!isIpAddrMsg(document.ipQos.ipStart.value, MM_sipaddr)) 
		return false;
	
	if (!isIpSubnet(document.ipQos.ipStart.value, lanMask, lanIP)) {
		alert(JS_msg20);
		return false;
	}
	
	if (!isIpAddrMsg(document.ipQos.ipEnd.value, MM_eipaddr)) 
		return false;

	if (!isIpSubnet(document.ipQos.ipEnd.value, lanMask, lanIP)) {
		alert(JS_msg20);
		return false;
	}
	
	if (!isIpRange(document.ipQos.ipStart.value, document.ipQos.ipEnd.value)) {
		alert(JS_msg24);
		return false;
	}
	
	if ((document.ipQos.ipStart.value == lanIP) || (document.ipQos.ipEnd.value == lanIP)) {
		alert(JS_msg26);
		document.ipQos.ipStart.focus();
		return false;		
	}
	
	//upload bw
	if (!isNumberRange(document.ipQos.bandwidth.value, 1, max_length)) { 
		alert(MM_upload_bw + JS_msg99);
		return false;
	}
	
	if (!isNumberRange(document.ipQos.bandwidth_downlink.value, 1, max_length)) { 
		alert(MM_download_bw + JS_msg99);
		return false;
	}
	
	return true;
}

function updateState()
{
	if (document.ipQosBasicSettings.enabled.options.selectedIndex==1) {  	
		enableTextField(document.ipQos.automaticUplinkSpeed);
		enableTextField(document.ipQos.automaticDownlinkSpeed);
		enableTextField(document.ipQos.manualUplinkSpeed);
		enableTextField(document.ipQos.manualDownlinkSpeed);
		enableTextField(document.ipQos.addressType);
		enableTextField(document.ipQos.mode);
		enableTextField(document.ipQos.bandwidth);
		enableTextField(document.ipQos.bandwidth_downlink);
		enableTextField(document.ipQos.comment);
		
		updateLinkState();
		
		enableButton(document.ipQos.apply);
		enableButton(document.ipQos.reset);
	}
	else{
		disableTextField(document.ipQos.automaticUplinkSpeed);
		disableTextField(document.ipQos.automaticDownlinkSpeed);
		disableTextField(document.ipQos.manualUplinkSpeed);
		disableTextField(document.ipQos.manualDownlinkSpeed);
		disableTextField(document.ipQos.addressType);
		disableTextField(document.ipQos.ipStart);
		disableTextField(document.ipQos.ipEnd);
		disableTextField(document.ipQos.mac);
		disableTextField(document.ipQos.mode);
		disableTextField(document.ipQos.bandwidth);
		disableTextField(document.ipQos.bandwidth_downlink);
		disableTextField(document.ipQos.comment);
		
		disableButton(document.ipQos.apply);
		disableButton(document.ipQos.reset);
	}
}

function updateLinkState()
{
	if (document.ipQos.automaticUplinkSpeed.options.selectedIndex == 1)
		disableTextField(document.ipQos.manualUplinkSpeed);
	else
		enableTextField(document.ipQos.manualUplinkSpeed);
	
	if (document.ipQos.automaticDownlinkSpeed.options.selectedIndex == 1)
		disableTextField(document.ipQos.manualDownlinkSpeed);
	else
		enableTextField(document.ipQos.manualDownlinkSpeed);
}

var xml = false;
function qosSubmit(value)
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
	xml.open('POST', '/goform/ipQosBasicSettings', true);
	xml.send('qosEnabled='+value);
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
<tr><td class="title"><script>dw(MM_qos_settings)</script></td></tr>
<tr><td><script>dw(JS_msg_qos)</script></td></tr>
<tr><td><hr></td></tr>
</table>

<form action=/goform/ipQosBasicSettings method=POST name="ipQosBasicSettings">
<input type="hidden" value="/internet/qos.asp" name="submit-url">
<input type="hidden" name="qosEnabled">
<table width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr>
	<td class="thead">QoS:</td>
	<td><select onChange="updateState();qosSubmit(this.value)" name="enabled">
	<option value="0" <% getIpQosEnableASP(0); %>><script>dw(MM_disable)</script></option>
    <option value="1" <% getIpQosEnableASP(1); %>><script>dw(MM_enable)</script></option>
    </select></td>
  </tr>
</table>
</form>

<form action=/goform/ipQos method=POST name="ipQos">
<input type="hidden" value="/internet/qos.asp" name="submit-url">
<table width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr>
    <td class="title2" colspan="2"><script>dw(MM_add)</script><hr></td>
  </tr>
  
  <tr style="display:none">
	<td class="thead"><script>dw(MM_auto_uplink_speed)</script>:</td>
	<td><select onChange="updateLinkState();" name="automaticUplinkSpeed">
	<option value="0" selected><script>dw(MM_disable)</script></option>
    <option value="1"><script>dw(MM_enable)</script></option>
    </select></td>
  </tr>
  <tr>
	<td class=thead><script>dw(MM_manual_uplink_speed)</script>:</td>
	<td><input type="text" name="manualUplinkSpeed" size="8" maxlength="8"  value="<% getCfgGeneral(1, "ManualUplinkSpeed"); %>" > (Kbps)</td>
  </tr>
  <tr style="display:none">
	<td class="thead"><script>dw(MM_auto_downlink_speed)</script>:</td>
	<td><select onChange="updateLinkState();" name="automaticDownlinkSpeed">
	<option value="0" selected><script>dw(MM_disable)</script></option>
    <option value="1"><script>dw(MM_enable)</script></option>
    </select></td>
  </tr>
  <tr>
 	<td class=thead><script>dw(MM_manual_downlink_speed)</script>:</td>
	<td><input type="text" name="manualDownlinkSpeed" size="8" maxlength="8"  value="<% getCfgGeneral(1, "ManualDownlinkSpeed"); %>" > (Kbps)</td>
  </tr>
  <tr style="display:none">
	<td class="thead"><script>dw(MM_address_type)</script>:</td>
	<td><select name="addressType">
    	<option value="0" selected>IP</option>
        <option value="1">MAC</option></select></td>
  </tr>
  <tr>
	<td class="thead"><script>dw(MM_ipaddr)</script>:</td>
	<td><input type="text" name="ipStart" maxlength="15" value=""> - <input type="text" name="ipEnd" maxlength="15" value=""></td>
  </tr>
  <tr style="display:none">
	<td class="thead"><script>dw(MM_macaddr)</script>:</td>
	<td><input type="text" name="mac" maxlength="17"></td>
  </tr>
  <tr style="display:none">
	<td class=thead><script>dw(MM_mode)</script>:</td>
	<td><select size="1" name="mode">
    <option value="1"><script>dw(MM_minimum_bw)</script></option>
    <option selected value="2"><script>dw(MM_maximum_bw)</script></option>
    </select></td>
  </tr>
  <tr>
	<td class=thead><script>dw(MM_upload_bw)</script>:</td>
	<td><input type="text" name="bandwidth" size="8" maxlength="8"> (Kbps)</td>
  </tr>
  <tr>
	<td class=thead><script>dw(MM_download_bw)</script>:</td>
	<td><input type="text" name="bandwidth_downlink" size="8" maxlength="8"> (Kbps)</td>
  </tr>
  <tr>
	<td class=thead><script>dw(MM_comment)</script>:</td>
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
<form action=/goform/ipQosDelete method=POST name="ipQosDelete">
<input type="hidden" value="/internet/qos.asp" name="submit-url">
<p><b><script>dw(MM_ip_qos_list)</script>:<script>document.write(JS_msg1);</script></b></p>
<table width=100% border=1 cellpadding=3 style="border-collapse: collapse" bordercolor="#C8C8C8">
	<tr bgcolor=#f2f2f2>
		<td><b>No.</b></td>
		<td align=center><b><script>dw(MM_ipaddr)</script></b></td>
        <!--<td align=center><b><script>dw(MM_mode)</script></b></td>-->
		<td align=center><b><script>dw(MM_upload_bw)</script></b></td>
		<td align=center><b><script>dw(MM_download_bw)</script></b></td>
		<td align=center><b><script>dw(MM_comment)</script></b></td>
	</tr>
	<% showIpQosRulesASP(); %>
</table>

<br>
<table width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr>
    <td>
      <script>dw('<input type=submit class=button value="'+BT_delete+'" name="deleteSelQos" onClick="return deleteClick()"> &nbsp; &nbsp;\
      <input type=button class=button value="'+BT_reset+'" name="reset" onClick="resetForm();">')</script>
    </td>
  </tr>
</table>
</form>

</td></tr></table>
</blockquote>
</body></html>
