<html>
<head>
<title>WR922</title>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312" />
<link href="images/kingnet.css" type="text/css" rel="stylesheet">
<SCRIPT src="helpScript.js"></SCRIPT>
<script language="JavaScript" type="text/javascript">
var MAX_RULES = 32;

var secs
var timerID = null
var timerRunning = false
var timeout = 3
var delay = 1000

var rules_num = <% getIPPortRuleNumsASP(); %>;

function InitializeTimer(){
	// Set the length of the timer, in seconds
	secs = timeout
	StopTheClock()
	StartTheTimer()
}

function StopTheClock(){
	if(timerRunning)
		clearTimeout(timerID)
	timerRunning = false
}

function StartTheTimer(){
	if (secs==0){
		StopTheClock()

		timerHandler();

		secs = timeout
		StartTheTimer()
    }else{
		self.status = secs
		secs = secs - 1
		timerRunning = true
		timerID = self.setTimeout("StartTheTimer()", delay)
	}
}

var http_request = false;
function makeRequest(url, content) {
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
    http_request.onreadystatechange = alertContents;
    http_request.open('POST', url, true);
    http_request.send(content);
}

function alertContents() {
	if (http_request.readyState == 4) {
		if (http_request.status == 200) {
			updatePacketCount( http_request.responseText);
		} else {
			//alert('There was a problem with the request.');
		}
	}
}

function updatePacketCount(str){
	var pc = new Array();
	pc = str.split(" ");
	for(i=0; i < pc.length; i++){
		e = document.getElementById("pktCnt" + i);
		e.innerHTML = pc[i];
	}
}

function timerHandler(){
	makeRequest("/goform/getRulesPacketCount", "n/a");
}
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
	if(field.value == "")
		return false;

	if (isAllNumAndSlash(field.value) == 0){
		return false;
	}

	var ip_pair = new Array();
	ip_pair = field.value.split("/");

	if(ip_pair.length > 2){
		return false;
	}

	if(ip_pair.length == 2){
		// sub mask
		if(!ip_pair[1].length)
			return false;
		if(!isNumOnly(ip_pair[1])){
			return false;
		}
		tmp = parseInt(ip_pair[1], 10);
		if(tmp < 0 || tmp > 32){
			return false;
		}
	}

    if( (!checkRange(ip_pair[0],1,0,255)) ||
		(!checkRange(ip_pair[0],2,0,255)) ||
		(!checkRange(ip_pair[0],3,0,255)) ||
		(!checkRange(ip_pair[0],4,0,254)) ){
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
	    if((str.charAt(i) >= '0' && str.charAt(i) <= '9') || (str.charAt(i) == '.'))
			continue;
		return 0;
	}
	return 1;
}

function isAllNumAndSlash(str)
{
	for (var i=0; i<str.length; i++){
	    if( (str.charAt(i) >= '0' && str.charAt(i) <= '9') || (str.charAt(i) == '.') || (str.charAt(i) == '/'))
			continue;
		return 0;
	}
	return 1;
}



function isNumOnly(str)
{
	for (var i=0; i<str.length; i++){
	    if((str.charAt(i) >= '0' && str.charAt(i) <= '9') )
			continue;
		return 0;
	}
	return 1;
}

function ipportFormCheck()
{
	if(rules_num >= (MAX_RULES-1) ){
		alert("The rule number is exceeded "+ MAX_RULES +".");
		return false;
	}

	if( document.ipportFilter.sip_address.value == "" && 
		document.ipportFilter.dip_address.value == "" &&
		document.ipportFilter.sFromPort.value == "" &&
		document.ipportFilter.dFromPort.value == "" &&
		document.ipportFilter.mac_address.value == ""){
		alert("������һ����IP���߶˿�ֵ.");
		return false;
	}

	if(document.ipportFilter.sFromPort.value != ""){
		d1 = atoi(document.ipportFilter.sFromPort.value, 1);
		if(isAllNum( document.ipportFilter.sFromPort.value ) == 0){
			alert("��Ч�Ķ˿ں�: Դ�˿�.");
			document.ipportFilter.sFromPort.focus();
			return false;
		}
		if(d1 > 65535 || d1 < 1){
			alert("��Ч�Ķ˿ں�: Դ�˿�.");
			document.ipportFilter.sFromPort.focus();
			return false;
		}
		
		if(document.ipportFilter.sToPort.value != ""){
			if(isAllNum( document.ipportFilter.sToPort.value ) == 0){
				alert("��Ч�Ķ˿ں�: Դ�˿�.");
				return false;
			}		
			d2 = atoi(document.ipportFilter.sToPort.value, 1);
			if(d2 > 65535 || d2 < 1){
				alert("��Ч��Դ�˿ں�.");
				return false;
			}
			if(d1 > d2){
			alert("��Ч��Դ�˿ڷ�Χ.");
			return false;
			}
		}
	}

	if(document.ipportFilter.dFromPort.value != ""){
		d1 = atoi(document.ipportFilter.dFromPort.value, 1);
		if(isAllNum( document.ipportFilter.dFromPort.value ) == 0){
			alert("��Ч�Ķ˿ں�: Ŀ�Ķ˿�.");
			return false;
		}
		if(d1 > 65535 || d1 < 1){
			alert("��Ч�Ķ˿ں�: Ŀ�Ķ˿�.");
			return false;
		}
		
		if(document.ipportFilter.dToPort.value != ""){
			if(isAllNum( document.ipportFilter.dToPort.value ) == 0){
				alert("��Ч�Ķ˿ں�: Ŀ�Ķ˿�.");
				return false;
			}		
			d2 = atoi(document.ipportFilter.dToPort.value, 1);
			if(d2 > 65535 || d2 < 1){
				alert("��Ч��Ŀ�Ķ˿�.");
				return false;
			}
			if(d1 > d2){
			alert("��Ч��Ŀ�Ķ˿ڷ�Χ.");
			return false;
			}
		}
	}
	// check ip address format
	if(document.ipportFilter.sip_address.value != ""){
		if(! checkIpAddr(document.ipportFilter.sip_address) ){
			alert("ԴIP��ַ��ʽ����.");
			return false;
		}
    }
	
	if(document.ipportFilter.dip_address.value != ""){
		if(! checkIpAddr(document.ipportFilter.dip_address) ){
			alert("Ŀ��IP��ַ��ʽ����.");
			return false;
		}
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

function defaultPolicyChanged()
{
	if( document.BasicSettings.defaultFirewallPolicy.options.selectedIndex == 0){
		document.ipportFilter.action.options.selectedIndex = 0;
	}else
		document.ipportFilter.action.options.selectedIndex = 1;
}
	
function updateState()
{
    if(! rules_num ){
 		disableTextField(document.ipportFilterDelete.deleteSelFilterPort);
 		disableTextField(document.ipportFilterDelete.reset);
	}else{
        enableTextField(document.ipportFilterDelete.deleteSelFilterPort);
        enableTextField(document.ipportFilterDelete.reset);
	}

	if( document.BasicSettings.defaultFirewallPolicy.options.selectedIndex == 0){
		document.ipportFilter.action.options.selectedIndex = 0;
	}else
		document.ipportFilter.action.options.selectedIndex = 1;

	protocolChange();

	if( document.BasicSettings.portFilterEnabled.options.selectedIndex == 1)
		InitializeTimer();	// update packet count
}
function actionChanged()
{
	if( document.BasicSettings.defaultFirewallPolicy.options.selectedIndex != 
		document.ipportFilter.action.options.selectedIndex)
		alert("�������ļ�ʹ���Ժ�Ĭ�ϲ�������ͬ�ģ�������ѡ��");
}

function protocolChange()
{
	if( document.ipportFilter.protocol.options.selectedIndex == 1 ||
		document.ipportFilter.protocol.options.selectedIndex == 2){
		document.ipportFilter.dFromPort.disabled = false;
		document.ipportFilter.dToPort.disabled = false;
		document.ipportFilter.sFromPort.disabled = false;
		document.ipportFilter.sToPort.disabled = false;
	}else{
		document.ipportFilter.dFromPort.disabled = true;
		document.ipportFilter.dToPort.disabled = true;
		document.ipportFilter.sFromPort.disabled = true;
		document.ipportFilter.sToPort.disabled = true;

		document.ipportFilter.dFromPort.value = 
			document.ipportFilter.dToPort.value = 
			document.ipportFilter.sFromPort.value = 
			document.ipportFilter.sToPort.value = "";
	}
}
function CheckMac()
{
	var re = /[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}/;
		if (document.ipportFilter.mac_address.value.length == 0) {
			return true;
		}
		if (!re.test(document.ipportFilter.mac_address.value)) {
			alert("��������ȷMAC��ַ��ʽ! (XX:XX:XX:XX:XX:XX)");
			document.ipportFilter.mac_address.focus();
			return false;
		}
	return true;
}
</script>
</head>
<body onLoad="updateState()">
<table width="90%" border="0" cellpadding="0" cellspacing="0" align="center">
  <tr>
    <td style="padding-top:30px;">
	   <table width="100%" border="0" cellpadding="0" cellspacing="0">
          <tr>
            <td class="nav">��ǰ·��:KN-WR922 &gt;&gt;���簲ȫ &gt;&gt;MAC/IP/Port ���� </td>
          </tr>
		  <tr>
        	<td>&nbsp;</td>
      	  </tr>
		   <tr>
            <td class="space" style="padding-left:10px; padding-right:10px;">&nbsp;&nbsp;&nbsp;��ҳ����������·���������Ŀͻ��˽������ƹ����.��ϸ��Ϣ���<input type="button" class="button5" value="����" onclick=popHelp('help.htm#ipfilter')> </td>
          </tr>
		  <tr>
		    <td>&nbsp;</td>
		  </tr>
		  <form method=post name="BasicSettings" action=/goform/BasicSettings>
          <tr>
            <td>
              <table class="space" id=l2tp width="100%" border="0" cellspacing="0" cellpadding="0">
                <tr>
            	  <td colspan="2" class="titlebg">��������</td>
          	    </tr>
				<tr>
                  <td class="contentpadding">MAC/IP/Port ����:</td>
                  <td><select onChange="updateState()" name="portFilterEnabled" size="1">
	                  <option value=0 <% getIPPortFilterEnableASP(0); %> >����</option>
                      <option value=1 <% getIPPortFilterEnableASP(1); %> >����</option></select>
                  ����ʹ�ã���ѡ������</td>
                </tr>
				<tr>
                  <td width="25%" class="contentpadding">Ĭ�ϲ���:</td>
                  <td width="75%"><select onChange="defaultPolicyChanged()" name="defaultFirewallPolicy">
		<option value=0 <% getDefaultFirewallPolicyASP(0); %> id="portBasicDefaultPolicyAccept">����</option>
		<option value=1 <% getDefaultFirewallPolicyASP(1); %> id="portBasicDefaultPolicyDrop">����</option></select>&nbsp;( ��ƥ���κι���İ�������)</td>
                </tr>
              </table>
			</td>
          </tr>
		  <tr><td>&nbsp;</td></tr>
		  <tr> 
			<td colspan="2" class="contentpadding"><input type="submit" class="button" value="�� ��" id="portBasicApply" name="addDMZ" onClick="return formCheck()">&nbsp;&nbsp;<input type="reset" class="button" value="ȡ ��" id="portBasicReset" name="reset"><input type="hidden" value="/port_filter.asp" name="submit-url">
			ע�⣺�ı���������ѡ��ʱ��������ύһ�κ��ڽ����������д��</td>
		  </tr>
		  <tr><td>&nbsp;</td></tr>
		  </form>
		  <form method=post name="ipportFilter" action=/goform/ipportFilter>
          <tr>
            <td>
              <table class="space" id=l2tp width="100%" border="0" cellspacing="0" cellpadding="0">
                <tr>
            	  <td colspan="4" class="titlebg">MAC/IP/Port ����</span></td>
          	    </tr>
			    <tr>
				  <td id="portFilterMac" class="contentpadding">MAC ��ַ:</td>
				  <td colspan="3"><input type="text" size="17" maxlength="17" name="mac_address" onChange="return CheckMac()"> (��: 00:13:d3:1f:45:10)</td>
			    </tr>
				<tr>
				  <td width="18%" class="contentpadding">ԴIP��ַ:</td>
				  <td width="19%"><input type="text" size="16" name="sip_address">
					<!-- we dont support ip range in kernel 2.4.30 
					-<input type="text" size="16" name="sip_address2">
					-->	</td>
				  <td width="12%">�˿ڷ�Χ:</td>
				  <td width="51%"><input type="text" size="5" name="sFromPort" id="sFromPort">-<input type="text" size="5" name="sToPort" id="sToPort"></td>
				</tr>
				<tr>
				  <td class="contentpadding">Ŀ��IP��ַ:</td>
				  <td><input type="text" size="16" name="dip_address">
						<!-- we dont support ip range in kernel 2.4.30 
						-<input type="text" size="16" name="dip_address2">
						-->	</td>
				  <td >�˿ڷ�Χ</td>
				  <td><input type="text" size="5" name="dFromPort" id="dFromPort">-<input type="text" size="5" name="dToPort" id="dToPort"></td>
				</tr>
				<tr>
				  <td class="contentpadding">Э��:</td>
				  <td colspan="3"><select onChange="protocolChange()" name="protocol" id="procotol">
						<option value="None">��</option>
						<option value="TCP">TCP</option>
						<option value="UDP">UDP</option>
						<option value="ICMP">ICMP</option></select>
				  (�޴�������Э������ݰ�)</td>
				</tr>
				<tr>
				  <td class="contentpadding">����:</td>
				  <td colspan="3"><select onChange="actionChanged()" name="action">
					<option value="Drop" id="portFilterActionDrop">����</option>
					<option value="Accept" id="portFilterActionAccept">����</option></select></td>
				</tr>
				<tr>
				  <td class="contentpadding">˵��:</td>
				  <td colspan="3"><input type="text" name="comment" size="16" maxlength="32"><input type="hidden" value="/port_filter.asp" name="submit-url"></td>
				</tr>
			</table>
           </td>
		  </tr>
		  <tr><td>&nbsp;</td></tr>
		  <tr> 
			<td colspan="2" class="contentpadding"><input type="submit" class="button" value="�� ��" id="portFilterApply" name="addFilterPort" onClick="return ipportFormCheck()">&nbsp;&nbsp;<input type="reset" class="button" value="ȡ ��" id="portFilterReset" name="reset"></td>
		  </tr>
		  <tr><td>&nbsp;</td></tr>
		  </form>
 		  <form action=/goform/ipportFilterDelete method=POST name="ipportFilterDelete">
		  <tr>
		    <td><table class="space" width=100% border="0" cellpadding="0" cellspacing="0">	
				<tr>
				  <td colspan="10" class="titlebg">��ǰ MAC/IP/Port �����б�</span></td>
				</tr>
				<tr>
					<td width="35" id="portCurrentFilterNo">NO.</td>
					<td width="89" align=center id="portCurrentFilterMac"> MAC ��ַ</td>
					<td width="102" align=center id="portCurrentFilterDIP"> Ŀ��IP��ַ</td>
					<td width="117" align=center id="portCurrentFilterSIP"> ԴIP��ַ</td>
					<td width="83" align=center id="portCurrentFilterProtocol"> Э��</td>
					<td width="102" align=center id="portCurrentFilterDPort"> Ŀ�Ķ˿ڷ�Χ</td>
					<td width="117" align=center id="portCurrentFilterSPort"> Դ�˿ڷ�Χ</td>
					<td width="63" align=center id="portCurrentFilterAction"> ����</td>
					<td width="101" align=center id="portCurrentFilterComment"> ˵��</td>
					<td width="41" align=center id="portCurrentFilterPacketCount"> ����</td>
				</tr>
				<% showIPPortFilterRulesASP(); %>
			 </table>
		   </td>
		 </tr>
		 <tr><td>&nbsp;</td></tr>
		  <tr> 
			<td colspan="2" class="contentpadding"><input type="submit" class="button2"  value="ɾ����ѡ��" id="portCurrentFilterDel" name="deleteSelFilterPort" onClick="return deleteClick()">&nbsp;&nbsp;<input type="reset" class="button"  value="ȡ ��" id="portCurrentFilterReset" name="reset"><input type="hidden" value="/port_filter.asp" name="submit-url"></td>
		  </tr>
		  <tr><td>&nbsp;</td></tr>
		</form>
	   </table>
	 </td>
  </tr>
  <tr>
    <td>&nbsp;</td>
  </tr>
</table>
</body>
</html>
