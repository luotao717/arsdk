<html>
<head>
<title>WR922</title>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312" />
<link href="images/kingnet.css" type="text/css" rel="stylesheet">
<script language="JavaScript" type="text/javascript">
var MAX_RULES = 32;
var rules_num = <% getPortForwardRuleNumsASP(); %> ;
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
        alert("IP地址不能为空.");
        field.value = field.defaultValue;
        field.focus();
        return false;
    }

    if ( isAllNum(field.value) == 0) {
        alert('必须在[0-9] 范围.');
        field.value = field.defaultValue;
        field.focus();
        return false;
    }

    if( (!checkRange(field.value,1,0,255)) ||
        (!checkRange(field.value,2,0,255)) ||
        (!checkRange(field.value,3,0,255)) ||
        (!checkRange(field.value,4,1,254)) ){
        alert('IP 地址格式错误.');
        field.value = field.defaultValue;
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
	if(rules_num >= (MAX_RULES-1) ){
		alert("The rule number is exceeded "+ MAX_RULES +".");
		return false;
	}

	if(!document.portForward.portForwardEnabled.options.selectedIndex){
		// user choose disable
		return true;
	}

	if(	document.portForward.ip_address.value == "" &&
		document.portForward.fromPort.value == "" &&
		document.portForward.toPort.value   == "" &&
		document.portForward.comment.value  == "")
		return true;

	// exam IP address
    if(document.portForward.ip_address.value == ""){
        alert("没有设置IP地址.");
        document.portForward.ip_address.focus();
        return false;
    }

    if(! checkIpAddr(document.portForward.ip_address) ){
 //       alert("IP 地址格式错误.");
        document.portForward.ip_address.focus();
        return false;
    }

    if(isAllNum(document.portForward.ip_address.value) == 0){
        alert("无效IP地址.");
        document.portForward.ip_address.focus();
        return false;
    }

	// exam Port
	if(document.portForward.fromPort.value == ""){
		alert("没有设置端口.");
		document.portForward.fromPort.focus();
		return false;
	}

	if(isAllNum( document.portForward.fromPort.value ) == 0){
		alert("无效的端口号.");
		document.portForward.fromPort.focus();
		return false;
	}

	d1 = atoi(document.portForward.fromPort.value, 1);
	if(d1 > 65535 || d1 < 1){
		alert("无效的端口号!");
		document.portForward.fromPort.focus();
		return false;
	}
	
	if(document.portForward.toPort.value != ""){
		if(isAllNum( document.portForward.toPort.value ) == 0){
			alert("无效的端口号.");
			document.portForward.toPort.focus();
			return false;
		}
		d2 = atoi(document.portForward.toPort.value, 1);
		if(d2 > 65535 || d2 < 1){
			alert("无效的端口号.");
			document.portForward.toPort.focus();
			return false;
		}
		if(d1 > d2){
			alert("无效的端口范围.");
			document.portForward.fromPort.focus();
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

function updateState()
{
    if(! rules_num ){
 		disableTextField(document.portForwardDelete.deleteSelPortForward);
 		disableTextField(document.portForwardDelete.reset);
	}else{
        enableTextField(document.portForwardDelete.deleteSelPortForward);
        enableTextField(document.portForwardDelete.reset);
	}

	if(document.portForward.portForwardEnabled.options.selectedIndex == 1){
		enableTextField(document.portForward.ip_address);
		enableTextField(document.portForward.fromPort);
		enableTextField(document.portForward.toPort);
		enableTextField(document.portForward.protocol);
		enableTextField(document.portForward.comment);
	}else{
		disableTextField(document.portForward.ip_address);
		disableTextField(document.portForward.fromPort);
		disableTextField(document.portForward.toPort);
		disableTextField(document.portForward.protocol);
		disableTextField(document.portForward.comment);
	}
}

</script>
</head>
<body onLoad="updateState()">

<table width="90%" border="0" cellpadding="0" cellspacing="0" align="center">
  <tr>
    <td style="padding-top:30px;">
	   <table width="100%" border="0" cellpadding="0" cellspacing="0">
          <tr>
            <td class="nav">当前路径:KN-WR922 &gt;&gt;服务设置 &gt;&gt;虚拟主机</td>
          </tr>
        <% checkIfUnderBridgeModeASP(); %>
          <tr>
            <td>&nbsp;</td>
          </tr>
		  <tr>
            <td class="space" style="padding-left:10px; padding-right:10px;">&nbsp;&nbsp;&nbsp;当您需要把局域网内某台主机的一些端口开发给互联网的用户时，可以使用本功能。 </td>
          </tr>
		  <tr>
		    <td>&nbsp;</td>
		  </tr>
		  <form method=post name="portForward" action=/goform/portForward>
          <tr>
		    <td>          
			  <table class="space" width=100% border="0" cellpadding="0" cellspacing="0">
				<tr>
				   <td colspan="4" class="titlebg">虚拟主机设置 </td>
				</tr>
                <tr>
                  <td width="25%" class="contentpadding">虚拟主机设置:</td>
                  <td width="75%"><select onChange="updateState()" name="portForwardEnabled" size="1">
	<option value=0 <% getPortForwardEnableASP(0); %> >禁用</option>
    <option value=1 <% getPortForwardEnableASP(1); %> >启用</option></select></td>
                </tr>
				<tr>
					<td id="forwardVirtualSrvIPAddr" class="contentpadding" >IP地址:</td>
					<td><input type="text" size="16" name="ip_address">	</td>
				</tr>
				<tr>
					<td id="forwardVirtualSrvPortRange" class="contentpadding" >端口范围:</td>
					<td><input type="text" size="5" name="fromPort">-<input type="text" size="5" name="toPort"></td>
				</tr>
				<tr>
					<td id="forwardVirtualSrvProtocol" class="contentpadding" >协议:</td>
					<td><select name="protocol">
						<option select value="TCP&UDP">TCP&UDP</option>
						<option value="TCP">TCP</option>
						<option value="UDP">UDP</option></select></td>
				</tr>
				<tr>
					<td id="forwardVirtualSrvComment" class="contentpadding" >说明:</td>
					<td><input type="text" name="comment" size="16" maxlength="32"></td>
				</tr>
			  </table>
			</td>
		  </tr>
		  <tr>
			<td>&nbsp;</td>
		  </tr>
		  <tr>
			<td class="contentpadding"><input type="hidden" value="/virtual_server.asp" name="submit-url"><input type="submit" class="button"  value="提 交" id="forwardVirtualSrvApply" name="addFilterPort" onClick="return formCheck()">&nbsp;&nbsp;<input type="reset" class="button"  value="取 消" name="reset"></td>
		  </tr> 
		  </form> 
		  <tr>
            <td>&nbsp;</td>
          </tr>  
		  <form action=/goform/portForwardDelete method=POST name="portForwardDelete">     
		  <tr>
		    <td> 
               <table class="space" width=100% border="0" cellpadding="0" cellspacing="0">	
				  <tr>
					<td id="forwardCurrentVirtualSrv" colspan="5" class="titlebg">当前虚拟主机列表</td>
				  </tr>
				  <tr>
					<td id="forwardCurrentVirtualSrvNo"> NO. </td>
					<td align=center id="forwardCurrentVirtualSrvIP"> IP 地址 </td>
					<td align=center id="forwardCurrentVirtualSrvPort"> 端口范围 </td>
					<td align=center id="forwardCurrentVirtualSrvProtocol"> 协议 </td>
					<td align=center id="forwardCurrentVirtualSrvComment"> 说明 </td>
				  </tr>
				<% showPortForwardRulesASP(); %>
				</table>
			  </td>
		  </tr>
		  <tr>
            <td>&nbsp;</td>
          </tr> 
		  <tr>
			<td class="contentpadding"><input type="hidden" value="/virtual_server.asp" name="submit-url"><input type="submit" class="button3"  value="删除所选项" name="deleteSelPortForward" onClick="return deleteClick()">&nbsp;&nbsp;<input type="reset" class="button"  value="取 消" name="reset"></td>
		   </tr> 
		  <tr>
            <td>&nbsp;</td>
          </tr> 
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