<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312" />
<style>
.on {display:block}
.off {display:none}
</style>
<link href="images/kingnet.css" type="text/css" rel="stylesheet" />
<script type="text/javascript" src="common.js"> </script>
<script type="text/javascript" src="message.js"> </script>
<script type="text/javascript">
function onBlur(id)
{
	var form = document.wizard;
	if(id == "staticIp")
	{
		if(IsIpNull(getById(id)) == "Yes")
		{
			classHide(id);
			divShow("msg_staticIp");
			showOk("msg_staticIp");
		}
		else{
			classShow(id);
			divShow("msg_staticIp");
			showErr("msg_staticIp");
			form.staticIp.focus();
			return false;
		}
	}
	else if(id == "staticNetmask"){
		if(IsMask(getById(id)) == "Yes")
		{
			classHide(id);
			divShow("msg_staticNetmask");
			showOk("msg_staticNetmask");
		}
		else{
			classShow(id);
			divShow("msg_staticNetmask");
			showErr("msg_staticNetmask");
			form.staticNetmask.focus();
			return false;
		}
	}
	else if(id == "staticGateway"){
		if(IsIpNull(getById(id)) == "Yes")
		{
			classHide(id);
			divShow("msg_staticGateway");
			showOk("msg_staticGateway");
		}
		else{
			classShow(id);
			divShow("msg_staticGateway");
			showErr("msg_staticGateway");
			form.staticGateway.focus();
			return false;
		}
	}
	else if(id == "staticPriDns"){
		if(IsIpNull(getById(id)) == "Yes")
		{
			classHide(id);
			divShow("msg_staticPriDns");
			showOk("msg_staticPriDns");
		}
		else{
			classShow(id);
			divShow("msg_staticPriDns");
			showErr("msg_staticPriDns");
			form.staticPriDns.focus();
			return false;
		}
	}
	else if(id == "staticSecDns"){
		if(form.staticSecDns.value !="")
		{
			if(IsIpNull(getById(id)) == "Yes")
			{
				classHide(id);
				divShow("msg_staticSecDns");
				showOk("msg_staticSecDns");
			}
			else{
				classShow(id);
				divShow("msg_staticSecDns");
				showErr("msg_staticSecDns");
				form.staticSecDns.focus();
				return false;
			}
		}
		else{
			classHide(id);
			divHide("msg_staticSecDns");
		}
	}
	else if(id == "pppoeUser"){
		if(IsEmpty(getById(id)) == "Yes")
		{
			classHide(id);
			divShow("msg_pppoeUser");
			showOk("msg_pppoeUser");
		}
		else{
			classShow(id);
			divShow("msg_pppoeUser");
			showErr("msg_pppoeUser");
			form.pppoeUser.focus();
			return false;
		}
	}
	else if(id == "pppoePass"){
		if(IsEmpty(getById(id)) == "Yes")
		{
			classHide(id);
			divShow("msg_pppoePass");
			getById("msg_pppoePass").innerHTML = msgOk+"&nbsp;"+"您所输入的宽带密码为："+form.pppoePass.value;
		}
		else{
			classShow(id);
			getById("msg_pppoePass");
			showErr("msg_pppoePass");
			form.pppoePass.focus();
			return false;
		}
	}
	else if(id == "ssid"){
		if(IsEmpty(getById(id)) == "Yes")
		{
			classHide(id);
			divShow("msg_ssid");
			showOk("msg_ssid");
		}
		else{
			classShow(id);
			divShow("msg_ssid");
			showErr("msg_ssid");
			form.ssid.focus();
			return false;
		}
	}
	else if(id == "passphrase"){
		if(IsWpaOk(getById(id)) == "Yes")
		{
			classHide(id);
			divShow("msg_passphrase");
			getById("msg_passphrase").innerHTML = msgOk+"&nbsp;"+"您所设置的无线密码为："+form.passphrase.value;
		}
		else{
			classShow(id);
			divShow("msg_passphrase");
			showErr("msg_passphrase");
			form.passphrase.focus();
			return false;
		}
	}
	return true;
}

function cancelClick()
{
	window.location="status.asp" ;
}

function wizardShowDiv(show,id) 
{
	if(show)
		getById(id).className = "on";
    else	    
    	getById(id).className = "off";
}

function wizardHideDiv()
{
	wizardShowDiv(false, "top_div");
	wizardShowDiv(false, "wan_div");
	wizardShowDiv(false, "wlan_div");
}

function wizardWanHideDiv()
{
	wizardHideDiv();
	wizardShowDiv(false, "static_div");
  	wizardShowDiv(false, "dhcpc_div");
	wizardShowDiv(false, "pppoe_div");
}

function saveClick_wanSelect(next)
{
	var form = document.wizard;
	if(next)
	{
		wizardWanHideDiv();
		if(form.wanconnectionType[0].checked)
			wizardShowDiv(true, "static_div");
		else if(form.wanconnectionType[1].checked)
			wizardShowDiv(true, "dhcpc_div");
		else if(form.wanconnectionType[2].checked)
			wizardShowDiv(true, "pppoe_div");
	}
	else{
		wizardHideDiv();
		wizardShowDiv(true, "top_div");
	}
}

function saveClick_wan(next)
{
	if(next)
	{
		if(saveChanges_wan())
		{
			wizardWanHideDiv();
			wizardShowDiv(true, "wlan_div");
		}
		else
			return false;
	}
	else{
		wizardWanHideDiv();
		wizardShowDiv(true, "wan_div");
	}
}

function saveChanges_wan()
{
	if(document.wizard.wanconnectionType[0].checked)
	{
	  	if(!onBlur("staticIp") || !onBlur("staticNetmask") || !onBlur("staticGateway") || !onBlur("staticPriDns") || !onBlur("staticSecDns"))
			return false;
  	}
  	else if(document.wizard.wanconnectionType[2].checked){
	   	if(!onBlur("pppoeUser") || !onBlur("pppoePass"))
			return false;
  	}
   	return true;
}

function saveClick_wlan(next)
{
	if(next)
	{
		if(saveChanges_wlan()) 
			return true;
		else
			return false;
	}
	else{
		wizardHideDiv();
		wizardShowDiv(true, "wan_div");
	}
}

function saveChanges_wlan()
{
  	if(document.wizard.security_mode.options.selectedIndex == 1)
	{
	   	if(!onBlur("passphrase") || !onBlur("ssid"))
			return false;
  	}
	else{
		if(!onBlur("ssid"))
			return false;
	}
   	return true;
}

function wirelessModeChange()
{
	var form = document.wizard;
	getById("div_11b_channel").style.display = "none";
	getById("div_11g_channel").style.display = "none";

	var wmode = form.wirelessmode.options.selectedIndex;
	wmode = 1*wmode;
	if ((wmode == 0) || (wmode == 4) || (wmode == 9))
	{
		if (wmode == 0)
			form.wirelessmode.options.selectedIndex = 0;
		else if (wmode == 4)
			form.wirelessmode.options.selectedIndex = 2;
		else if (wmode == 9)
			form.wirelessmode.options.selectedIndex = 3;	
		getById("div_11g_channel").style.display = "";
	}
	else if (wmode == 1){
		form.wirelessmode.options.selectedIndex = 1;
		getById("div_11b_channel").style.display = "";
	}
}
	
function securityMode()
{
	if(document.wizard.security_mode.selectedIndex==1)
		getById("wpa_pwd").style.display = "";
	else if(document.wizard.security_mode.selectedIndex==0)
		getById("wpa_pwd").style.display = "none";
}

function initValue()
{
	var form = document.wizard;
	var wmode = "<% getCfgGeneral(1, "wanConnectionMode"); %>";
	if(wmode == "STATIC")
		form.wanconnectionType[0].checked=true;
	else if(wmode == "DHCP")
		form.wanconnectionType[1].checked=true;
	else if(wmode == "PPPOE")
		form.wanconnectionType[2].checked=true;

	var PhyMode  = '<% getCfgZero(1, "WirelessMode"); %>';
	PhyMode = 1*PhyMode;
	getById("div_11b_channel").style.display = "none";
	getById("div_11g_channel").style.display = "none";
	if ((PhyMode == 0) || (PhyMode == 4) || (PhyMode == 9))
	{
		if (PhyMode == 0)
			form.wirelessmode.options.selectedIndex = 0;
		else if (PhyMode == 4)
			form.wirelessmode.options.selectedIndex = 2;
		else if (PhyMode == 9)
			form.wirelessmode.options.selectedIndex = 3;	
		getById("div_11g_channel").style.display = "";
	}
	else if (PhyMode == 1){
		form.wirelessmode.options.selectedIndex = 1;
		getById("div_11b_channel").style.display = "";
	}
}
</script>
</head>
<body id="main">
<form method=post name="wizard" action="/goform/formWizardEasy">
<table width=90% border=0 cellspacing=0 cellpadding=0>
  <tr>
	<td><table width=100% border=0 cellspacing=0 cellpadding=0>
	  <tr>
		<td class=titlebg>设置向导</td>
  	  </tr>
	  <tr>
		<td class=space>
		  <span id="top_div" class="on">
			<table width="100%" border="0" cellspacing="2" cellpadding="2">
				<tr>
                  <td class=itemleft>用这个向导，您可以设置上网所需的基本网络参数。即使您对网络知识和这个产品不太熟悉，您也可以按照提示轻松地完成设置。如果您是一位专家，您也可以退出这个向导程序，直接到菜单项中选择您需要修改的设置项进行设置。</td>
				</tr>
				<tr>
                  <td class=itemleft>要继续，请单击“下一步”。</td>
				</tr>
				<tr>
                  <td class=itemleft>要退出设置向导，请单击“退出向导”。</td>
				</tr>
				<tr>
				  <td id="button" align="center"><input type="button" value=" 退出向导 " name="cancel" onClick='cancelClick();'>&nbsp;&nbsp;<input type="button" value=" 下一步 " onClick='wizardHideDiv();wizardShowDiv(true,"wan_div");'></td>	
				</tr>
			</table>	
		  </span>
		  <span id="wan_div" class="off">
			<table width="100%" border="0" cellspacing="2" cellpadding="2">
				<tr>
				  <td class=itemleft>本路由器支持三种常用的上网方式，请您根据自身情况进行选择。</td>
				</tr>
				<tr>
                  <td class=itemleft><input type="radio" name="wanconnectionType" value="STATIC"> 以太网宽带，网络服务商提供的固定IP地址（静态IP）</td>
				</tr>
				<tr>
                  <td class=itemleft><input type="radio" name="wanconnectionType" value="DHCP"> 以太网宽带，自动从网络服务商获取IP地址（动态IP）</td>
				</tr>
				<tr>
                  <td class=itemleft><input type="radio" name="wanconnectionType" value="PPPOE" checked> ADSL虚拟拨号（PPPoE） </td>
				</tr>			
				<tr>
				  <td id="button" align="center"><input type="button" class="bt" value=" 上一步 " onClick='saveClick_wanSelect(0)' >&nbsp;&nbsp;<input type="button" class="bt" value=" 下一步 " onClick='saveClick_wanSelect(1)'></td>
				</tr>
			</table>	
		  </span>
		  <span id="static_div" class="off">
			<table width="100%" border="0" cellspacing="2" cellpadding="2">
				<tr>
				  <td colspan="2" class=itemleft>您申请以太网宽带服务，并具有固定IP地址时，网络服务商将提供给您一些基本的网络参数，请对应填入下框。如您遗忘或不太清楚，请咨询您的网络服务商。</td>
				</tr>
				<tr>
                  <td class=itemleft>IP 地址:</td>
                  <td class=itemright><input onblur="onBlur(this.id)" id="staticIp" name="staticIp" maxlength=15 value="<% getCfgGeneral(1, "wan_static_ip"); %>"> <span id="msg_staticIp" class="errDiv" style="display:none"></span></td>
                </tr>
				<tr>
				  <td class=itemleft>子网掩码:</td>
				  <td class=itemright><input onblur="onBlur(this.id)" id="staticNetmask" name="staticNetmask" maxlength=15 value="<% getCfgGeneral(1, "wan_static_mask"); %>"> <span id="msg_staticNetmask" class="errDiv" style="display:none"></span></td>
				</tr>
				<tr>
				  <td class=itemleft>默认网关:</td>
				  <td class=itemright><input onblur="onBlur(this.id)" id="staticGateway" name="staticGateway" maxlength=15 value="<% getCfgGeneral(1, "wan_static_gateway"); %>"> <span id="msg_staticGateway" class="errDiv" style="display:none"></span></td>
				</tr>
				<tr>
				  <td class=itemleft>首选 DNS:</td>
				  <td class=itemright><input onblur="onBlur(this.id)" id="staticPriDns" name="staticPriDns" maxlength=15 value="<% getCfgGeneral(1, "wan_static_dns1"); %>"> <span id="msg_staticPriDns" class="errDiv" style="display:none"></span></td>
				</tr>
				<tr>
                  <td class=itemleft>备用 DNS:</td>
                  <td class=itemright><input onblur="onBlur(this.id)" id="staticSecDns" name="staticSecDns" maxlength=15 value="<% getCfgGeneral(1, "wan_static_dns2"); %>"> <span id="msg_staticSecDns" class="errDiv" style="display:none"></span></td>
                </tr>
            	<tr>
				  <td id="button" align="center" colspan="2"><input type="button" class="bt" value=" 上一步 " onClick='saveClick_wan(0)' >&nbsp;&nbsp;<input type="button" class="bt" value=" 下一步 " onClick='saveClick_wan(1)'></td>
				</tr>
			</table>		
		  </span>
		  <span id="dhcpc_div" class="off">
			<table width="100%" border="0" cellspacing="2" cellpadding="2">
				<tr>
                  <td class=itemleft>自动获取IP地址。</td>
                </tr>
				<tr>
				  <td id="button" align="center"><input type="button" class="bt" value=" 上一步 " onClick='saveClick_wan(0)' >&nbsp;&nbsp;<input type="button" class="bt" value=" 下一步 " onClick='saveClick_wan(1)'></td>	
				</tr>
			</table>	
		  </span>
		  <span id="pppoe_div" class="off">
			<table width="100%" border="0" cellspacing="2" cellpadding="2">
				<tr>
				  <td colspan="2" class=itemleft>您申请ADSL虚拟拨号服务时，网络服务商将提供给您上网帐号及密码，请对应填入下框。如您遗忘或不太清楚，请咨询您的网络服务商。</td>
				</tr>
				<tr>
				  <td class=itemleft>上网帐号:</td>
				  <td class=itemright><input onblur="onBlur(this.id)" id="pppoeUser" name="pppoeUser" maxlength=32 value="<% getCfgGeneral(1, "wan_pppoe_user"); %>"> <span id="msg_pppoeUser" class="errDiv" style="display:none"></span></td>
				</tr>
				<tr>
				  <td class=itemleft>上网密码:</td>
				  <td class=itemright><input onblur="onBlur(this.id)" id="pppoePass" type="password" name="pppoePass" maxlength=32 value="<% getCfgGeneral(1, "wan_pppoe_pass"); %>"> <span id="msg_pppoePass" class="errDiv" style="display:none"></span></td>
				</tr>
            	<tr>
				  <td id="button" align="center" colspan="2"><input type="button" class="bt" value=" 上一步 " onClick='saveClick_wan(0)' >&nbsp;&nbsp;<input type="button" class="bt" value=" 下一步 " onClick='saveClick_wan(1)'></td>
			    </tr>
			</table>		
		  </span>
		  <span id="wlan_div" class="off">
			<table width="100%" border="0" cellspacing="2" cellpadding="2">
				<tr>
				  <td colspan="2" class=itemleft>本向导页面设置路由器无线网络的基本参数。</td>
				</tr>
				<tr>
				  <td class=itemleft>SSID:</td>
				  <td class=itemright><input onblur="onBlur(this.id)" id="ssid" type=text name="ssid" maxlength=32 value="<% getCfgGeneral(1, "SSID1"); %>"> <span id="msg_ssid" class="errDiv" style="display:none"></span></td>
				</tr>
				<tr id="div_11g_channel">
				  <td class=itemleft>频段:</td>
				  <td class=itemright><select name="sz11gChannel">
					<option value=0>自动选择</option><% getWlan11gChannels(); %></select></td>
				</tr>
				<tr>
				  <td class=itemleft>无线加密:</td>
				  <td class=itemright><select name="security_mode" id="security_mode" onChange="securityMode()">
				    <option value="Disable">不加密</option>
				    <option value="WPAPSK">加密(WPA)</option></select></td>
				</tr>
				<tr id="wpa_pwd" style="display:none">
				  <td class=itemleft>无线连接密码:</td>
				  <td class=itemright><input onblur="onBlur(this.id)" id="passphrase" type="password" name="passphrase" value="<% getCfgGeneral(1, "WPAPSK1"); %>"> <span id="msg_passphrase" class="errDiv" style="display:none"></span></td>
				</tr>
            	<tr>
				  <td id="button" align="center" colspan="2"><input type="button" class="bt" value=" 上一步 " onClick="saveClick_wlan(0)" >&nbsp;&nbsp;<input type="submit" class="bt" value="完 成" onclick="return saveClick_wlan(1)"></td>	
				</tr>
			</table>	
		  </span>
		</td>
	  </tr>
	</table></td>
  </tr>
</table>
</form>
</body>
</html>
