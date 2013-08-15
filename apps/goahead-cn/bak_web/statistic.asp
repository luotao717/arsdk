<html>
<head>
<title>WR922</title>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312" />
<link href="images/kingnet.css" type="text/css" rel="stylesheet">
<script language="JavaScript" type="text/javascript">
function formCheck()
{
	if( document.SystemCommand.command.value == ""){
		alert("请输入命令.");
		return false;
	}

	return true;
}

</script>

</head>
<body>
<table width="90%" border="0" cellpadding="0" cellspacing="0" align="center">
  <tr>
    <td style="padding-top:30px;">
	   <table width="100%" border="0" cellpadding="0" cellspacing="0">
		 <tr>
           <td class="nav">当前路径:KN-WR922 &gt;&gt;系统状态 &gt;&gt;数据统计</td>
         </tr>
	  <tr>
		<td>&nbsp;</td>
	  </tr>
      <tr>
        <td class="space" style="padding-left:10px; padding-right:10px;">&nbsp;&nbsp;&nbsp;本页面显示系统运行的一些数据信息，包括内存使用情况、WAN口的收、发包信息以及LAN口的收、发包信息等。</td>
      </tr>
      <tr>
        <td>&nbsp;</td>
      </tr>
      <tr>
		<td><table class="space" width="100%" border="0" cellpadding="0" cellspacing="0">          
		   <tr>
			 <td colspan="2" class="titlebg">内存基本信息</td>
		   </tr>
		   <tr>
			 <td width="25%" class="contentpadding">总内存:</td>
			 <td width="75%"><% getMemTotalASP(); %></td>
		   </tr>
		   <tr>
			 <td class="contentpadding">剩余内存:</td>
			 <td><% getMemLeftASP(); %></td>
		   </tr>
		</table></td>
	  </tr>
	  <tr>
	    <td><table class="space" width="100%" border="0" cellpadding="0" cellspacing="0">
		   <tr>
			 <td colspan="2" class="titlebg">WAN口 基本信息</td>
		   </tr> 
		   <tr>
			 <td width="25%" class="contentpadding">收包数:</td>
			 <td width="75%"><% getWANRxPacketASP(); %></td>
		   </tr> 
		   <tr>
			 <td class="contentpadding">收包字节:</td>
			 <td><% getWANRxByteASP(); %></td>
		   </tr> 
		   <tr>
			 <td class="contentpadding">发送包数:</td>
			 <td><% getWANTxPacketASP(); %></td>
		   </tr> 
		   <tr>
			 <td class="contentpadding">发送包字节数:</td>
			 <td><% getWANTxByteASP(); %></td>
		   </tr>
	    </table></td>
	 </tr>
	 <tr>
	    <td><table class="space" width="100%" border="0" cellpadding="0" cellspacing="0">
	   	   <tr>
			 <td colspan="2" class="titlebg">LAN口 基本信息</td>
		   </tr> 
		   <tr>
			 <td width="25%" class="contentpadding">接收包数:</td>
			 <td width="75%"><% getLANRxPacketASP(); %></td>
		   </tr> 
		   <tr>
			 <td class="contentpadding">收包字节数:</td>
			 <td><% getLANRxByteASP(); %></td>
	   	   </tr> 
		   <tr>
			 <td class="contentpadding">发送包数:</td>
			 <td><% getLANRxPacketASP(); %></td>
		   </tr> 
		   <tr>
			 <td class="contentpadding">发送包字节数:</td>
			 <td><% getLANRxByteASP(); %></td>
		   </tr>
	   <!--<tr>
		     <td colspan="2" class="titlebg">所有接口:</td>
		   </tr>-->
        </table></td>
	 </tr>          
    </table></td>
  </tr> 
  <tr>
    <td>&nbsp;</td>
  </tr>        
</table>
</body>
</html>