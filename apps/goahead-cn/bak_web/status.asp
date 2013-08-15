<html>
<head>
<title>WR922</title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<meta http-equiv=refresh content="6">
<link href="images/kingnet.css" type="text/css" rel="stylesheet">
<script language="JavaScript" type="text/javascript">
var w_status = '<% getCfgZero(1, "RadioOff"); %>';
var w_mode='<% getCfgZero(1, "WirelessMode"); %>';
var wds_mode='<% getCfgZero(1, "WdsEnable"); %>'; 
var wps_mode='<% getWPSModeASP(); %>' ; 
var security_mode='<% getCfgZero(1, "AuthMode"); %> '; 
var s_EncrypType='<% getCfgZero(1, "EncrypType"); %> '; 
var s_IEEE8021X='<% getCfgZero(1, "IEEE8021X"); %> '; 
//alert(s_EncrypType);
//alert(s_IEEE8021X);


function display_on()
{
	if (window.ActiveXObject) { // IE
		return "block";
	}
	else if (window.XMLHttpRequest) { // Mozilla, Firefox, Safari,...
		return "table-row";
	}
}

function initValue()
{   
	//alert("111");
//makeRequest("/goform/wirelessGetSecurity", "n/a", securityHandler);
//alert("555");
//alert(tempSecMode);
	document.getElementById("dhcpenable").style.visibility = "hidden";
	document.getElementById("dhcpenable").style.display = "none";

	document.getElementById("dhcpstart").style.visibility = "hidden";
    document.getElementById("dhcpstart").style.display = "none";
    
    document.getElementById("dhcpend").style.visibility = "hidden";
    document.getElementById("dhcpend").style.display = "none";
	
	var dhcp = <% getCfgZero(1, "dhcpEnabled"); %>;
	if(dhcp==1)
	{
	   document.getElementById("dhcpstart").style.visibility = "visible";
	   document.getElementById("dhcpstart").style.display = display_on();

	   document.getElementById("dhcpend").style.visibility = "visible";
	   document.getElementById("dhcpend").style.display = display_on();

	}
	else
	{
	   document.getElementById("dhcpenable").style.visibility = "visible";
	   document.getElementById("dhcpenable").style.display = display_on();
	}
	   
	if(w_status==1)
	{
        document.getElementById("w_mode_type").style.visibility = "hidden";
		document.getElementById("w_mode_type").style.display = "none";

		document.getElementById("wds_mode_type").style.visibility = "hidden";
		document.getElementById("wds_mode_type").style.display = "none";
		
		document.getElementById("wps_mode_type").style.visibility = "hidden";
		document.getElementById("wps_mode_type").style.display = "none";

		document.getElementById("ssid_type").style.visibility = "hidden";
		document.getElementById("ssid_type").style.display = "none";
		
		document.getElementById("security_mode_type").style.visibility = "hidden";
		document.getElementById("security_mode_type").style.display = "none";
		
	}   
	else
	{
		document.getElementById("w_mode_type").style.visibility = "visible";
		document.getElementById("w_mode_type").style.display = display_on();

		document.getElementById("wds_mode_type").style.visibility = "visible";
		document.getElementById("wds_mode_type").style.display = display_on();
		
		document.getElementById("wps_mode_type").style.visibility = "visible";
		document.getElementById("wps_mode_type").style.display = display_on();

		document.getElementById("ssid_type").style.visibility = "visible";
		document.getElementById("ssid_type").style.display = display_on();
		
		document.getElementById("security_mode_type").style.visibility = "visible";
		document.getElementById("security_mode_type").style.display = display_on();
	}


}

</script>

</head>
<body onLoad="initValue()"><table width="90%" border="0" cellpadding="0" cellspacing="0" align="center">
  <tr>
    <td style="padding-top:30px;">
	   <table width="100%" border="0" cellpadding="0" cellspacing="0">
		  <tr>
            <td class="nav">当前路径:KN-WR922 &gt;&gt;系统状态 &gt;&gt;当前状态<td>
          </tr>
          <tr>
            <td>&nbsp;</td>
          </tr>
		  <tr>
            <td class="space" style="padding-left:10px; padding-right:10px;">&nbsp;&nbsp;&nbsp;本页面显示系统的一些基本状态信息，包括无线，LAN和WAN的基本信息。</td>
          </tr>
		  <tr>
		    <td>&nbsp;</td>
		  </tr>
          <tr>
		    <td><table class="space" width=100% border="0" cellpadding="0" cellspacing="0">          
				<tr>
				   <td colspan="2" class="titlebg">系统信息</td>
				</tr>          
				<tr>
					<td width=25% class="contentpadding">软件版本:</td>
					<td width=75%><% getSdkVersion(); %> (<% getSysBuildTime(); %>)	</td>
				</tr>
				<tr>
					<td width=25% class="contentpadding">系统运行时间:</td>
					<td width=75%><% getSysUptime(); %>	</td>
				</tr>  
				<tr>
					<td width=25% class="contentpadding">硬件版本:</td>
					<td width=75%><% getPlatform(); %>	</td>
				</tr>  
				<tr>
					<td width=25% class="contentpadding">系统模式:</td>
					<td width=75%><% getOpMode(1); %>	</td>
				</tr> 
			</table></td>
		  </tr>
		   <tr>
		    <td><table class="space" width=100% border="0" cellpadding="0" cellspacing="0">  
				<tr>
				   <td colspan="2" class="titlebg">WAN 配置信息</td>
				</tr> 
				<tr>
					<td width=25% class="contentpadding">连接方式:</td>
					<td width=75%><% getCfgGeneral(1, "wanConnectionMode"); %>	</td>
				</tr> 
				<tr>
					<td width=25% class="contentpadding">WAN IP 地址:</td>
					<td width=75%><% getWanIp(); %>	</td>
				</tr> 
				<tr>
					<td width=25% class="contentpadding">子网掩码:</td>
					<td width=75%><% getWanNetmask(); %>	</td>
				</tr> 
				<tr>
					<td width=25% class="contentpadding">默认网关:</td>
					<td width=75%><% getWanGateway(); %>	</td>
				</tr> 
				<tr>
					<td width=25% class="contentpadding">主DNS服务器:</td>
					<td width=75%><% getDns(1); %>	</td>
				</tr> 
				<tr>
					<td width=25% class="contentpadding">次DNS服务器:</td>
					<td width=75%><% getDns(2); %>	</td>
				</tr>
				<tr>
					<td width=25% class="contentpadding">WAN MAC 地址:</td>
					<td width=75%><% getWanMac(); %>	</td>
				</tr>
			</table></td>
		  </tr>
		  <tr>
		    <td><table class="space" width=100% border="0" cellpadding="0" cellspacing="0">  
				<tr>
				   <td colspan="2" class="titlebg">无线配置信息</td>
				</tr> 
				<script language="JavaScript">
				if(1 == 1*w_status)
	 			 document.write("<tr><td width=25% class=\"contentpadding\">无线状态</td><td width=75% >禁用</td></tr>");
	  			else
	  			 document.write("<tr><td width=25% class=\"contentpadding\">无线状态</td><td width=75% >启用</td></tr>");
				</script>
				
				<script language="JavaScript">
				if(9 == 1*w_mode)
                 {  document.write("<tr  id=w_mode_type><td width=25% class=\"contentpadding\">网络模式:</td><td width=75% >11b/g/n</td></tr>")};
                if(0 == 1*w_mode)
                 {  document.write("<tr  id=w_mode_type><td width=25% class=\"contentpadding\">网络模式:</td><td width=75% >11b/g</td></tr>")};
                 if(1 == 1*w_mode)
                 {  document.write("<tr  id=w_mode_type><td width=25% class=\"contentpadding\">网络模式:</td><td width=75% >11b</td></tr>")};
                 if(4 == 1*w_mode)
                 {  document.write("<tr  id=w_mode_type><td width=25% class=\"contentpadding\">网络模式:</td><td width=75% >11g</td></tr>")};
                 </script>
                 
				<script language="JavaScript">
				if(0 == 1*wds_mode)
                 {  document.write("<tr id=wds_mode_type><td width=25% class=\"contentpadding\">WDS模式:</td><td width=75% >禁用</td></tr>")};
                if(4 == 1*wds_mode)
                 {  document.write("<tr id=wds_mode_type><td width=25% class=\"contentpadding\">WDS模式:</td><td width=75% >懒人模式</td></tr>")};
                 if(2 == 1*wds_mode)
                 {  document.write("<tr id=wds_mode_type><td width=25% class=\"contentpadding\">WDS模式:</td><td width=75% >桥接模式</td></tr>")};
                 if(3 == 1*wds_mode)
                 {  document.write("<tr id=wds_mode_type><td width=25% class=\"contentpadding\">WDS模式:</td><td width=75% >中继模式</td></tr>")};
                 </script>
                 

				<script language="JavaScript">
				if(0 == 1*wps_mode)
	 			 document.write("<tr id=wps_mode_type><td width=25% class=\"contentpadding\">WPS模式:</td><td width=75% >禁用</td></tr>");
	  			else
	  			 document.write("<tr id=wps_mode_type><td width=25% class=\"contentpadding\">WPS模式:</td><td width=75% >启用</td></tr>");
				</script>
				
				<tr id="ssid_type">
					<td width=25% class="contentpadding">SSID:</td>
					<td width=75%><% getCfgGeneral(1, "SSID1"); %>	</td>
			    </tr>
			 
               <script language="JavaScript">
			   var ssid2 = '<% getCfgGeneral(1, "SSID2"); %>'; 
			   var ssid3 = '<% getCfgGeneral(1, "SSID3"); %>';
			   var ssid4 = '<% getCfgGeneral(1, "SSID4"); %>';
			   var ssid5 = '<% getCfgGeneral(1, "SSID5"); %>';
			   var ssid6 = '<% getCfgGeneral(1, "SSID6"); %>';
			   var ssid7 = '<% getCfgGeneral(1, "SSID7"); %>';
               var n=security_mode.indexOf(";");
			   var ssid_smode=security_mode.substring(0,n); 
               var ss_mode = security_mode.substr(0,4);
			   
			   var m=s_EncrypType.indexOf(";");	
			   var ssid_sEncrypType=s_EncrypType.substring(0,m); 			   		   
               var ss_EncrypType = s_EncrypType.substr(0,4);
			   
			   var v=s_IEEE8021X.indexOf(";");	
			   var ssid_sIEEE8021X=s_IEEE8021X.substring(0,v); 			   		   			   
     
              if((ssid2==""&&ssid3==""&&ssid4==""&&ssid5==""&&ssid6==""&&ssid7=="")==1){
               if((ss_mode == "OPEN")&&(ss_EncrypType == "NONE")&&(1*s_IEEE8021X== 0))
                    document.write("<tr id=security_mode_type><td width=25% class=\"contentpadding\">安全模式:</td><td width=75% >禁用</td></tr>");
			   else if((ss_mode == "OPEN")&&(1*s_IEEE8021X== 1))
                    document.write("<tr id=security_mode_type><td width=25% class=\"contentpadding\">安全模式:</td><td width=75% >802.1x</td></tr>");	
			    else
					document.write("<tr id=security_mode_type><td width=25% class=contentpadding>安全模式:</td><td width=75% >"+security_mode+"</td></tr>");	
				}
			   else{
			   if((ssid_smode == "OPEN")&&(ssid_sEncrypType == "NONE")&&(1*ssid_sIEEE8021X== 0))
                    document.write("<tr id=security_mode_type><td width=25% class=\"contentpadding\">安全模式:</td><td width=75% >禁用</td></tr>");					
               else if((ssid_smode == "OPEN")&&(1*ssid_sIEEE8021X== 1))
                    document.write("<tr id=security_mode_type><td width=25% class=\"contentpadding\">安全模式:</td><td width=75% >802.1x</td></tr>");
				else
					document.write("<tr id=security_mode_type><td width=25% class=contentpadding>安全模式:</td><td width=75% >"+ssid_smode+"</td></tr>");
              }
                 </script>           
         </td>
         </tr>	  
			</table></td>	
		  <tr>
		    <td><table class="space" width=100% border="0" cellpadding="0" cellspacing="0">  
				<tr>
				   <td colspan="2" class="titlebg">WAN 配置信息</td>
				</tr> 
				<tr>
					<td width=25% class="contentpadding">连接方式:</td>
					<td width=75%><% getCfgGeneral(1, "wanConnectionMode"); %>	</td>
				</tr> 
				<tr>
					<td width=25% class="contentpadding">WAN IP 地址:</td>
					<td width=75%><% getWanIp(); %>	</td>
				</tr> 
				<tr>
					<td width=25% class="contentpadding">子网掩码:</td>
					<td width=75%><% getWanNetmask(); %>	</td>
				</tr> 
				<tr>
					<td width=25% class="contentpadding">默认网关:</td>
					<td width=75%><% getWanGateway(); %>	</td>
				</tr> 
				<tr>
					<td width=25% class="contentpadding">主DNS服务器:</td>
					<td width=75%><% getDns(1); %>	</td>
				</tr> 
				<tr>
					<td width=25% class="contentpadding">次DNS服务器:</td>
					<td width=75%><% getDns(2); %>	</td>
				</tr>
				<tr>
					<td width=25% class="contentpadding">WAN MAC 地址:</td>
					<td width=75%><% getWanMac(); %>	</td>
				</tr>
			</table></td>
		  </tr>
		  <tr>
		    <td><table class="space" width=100% border="0" cellpadding="0" cellspacing="0"> 
				<tr>
				   <td colspan="2" class="titlebg">本地网络(LAN)</td>
				</tr> 
				<tr>
					<td width=25% class="contentpadding">LAN IP 地址:</td>
					<td width=75%><% getLanIp(); %>	</td>
				</tr> 
				<tr>
					<td width=25% class="contentpadding">LAN 子网掩码:</td>
					<td width=75%><% getLanNetmask(); %>	</td>
				</tr>
				<tr id="dhcpenable">
					<td width=25% class="contentpadding">DHCP 状态:</td>
					<td width=75%>禁用</td>
				</tr>
				<tr id="dhcpstart">
					<td width=25% class="contentpadding">DHCP 开始地址:</td>
					<td width=75%><% getCfgZero(1, "dhcpStart"); %>	</td>
				</tr>
				<tr id="dhcpend">
					<td width=25% class="contentpadding">DHCP 结束地址:</td>
					<td width=75%><% getCfgZero(1, "dhcpEnd"); %>	</td>
				</tr>
				<tr id="dhcpend">
					<td width=25% class="contentpadding">LAN MAC 地址:</td>
					<td width=75%><% getLanMac(); %>	</td>
				</tr>            
             </table></td>
		   </tr>
        </table>
	 </td>
  </tr>
  <tr>
     <td>&nbsp;</td>
  </tr>
</table>
</body>
</html>
