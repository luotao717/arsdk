<html>
<head>
<title>WR922</title>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312" />
<link href="images/kingnet.css" type="text/css" rel="stylesheet">
<script language="JavaScript" type="text/javascript">
function formCheck()
{
	if( document.SystemCommand.command.value == ""){
		alert("����������.");
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
           <td class="nav">��ǰ·��:KN-WR922 &gt;&gt;ϵͳ״̬ &gt;&gt;����ͳ��</td>
         </tr>
	  <tr>
		<td>&nbsp;</td>
	  </tr>
      <tr>
        <td class="space" style="padding-left:10px; padding-right:10px;">&nbsp;&nbsp;&nbsp;��ҳ����ʾϵͳ���е�һЩ������Ϣ�������ڴ�ʹ�������WAN�ڵ��ա�������Ϣ�Լ�LAN�ڵ��ա�������Ϣ�ȡ�</td>
      </tr>
      <tr>
        <td>&nbsp;</td>
      </tr>
      <tr>
		<td><table class="space" width="100%" border="0" cellpadding="0" cellspacing="0">          
		   <tr>
			 <td colspan="2" class="titlebg">�ڴ������Ϣ</td>
		   </tr>
		   <tr>
			 <td width="25%" class="contentpadding">���ڴ�:</td>
			 <td width="75%"><% getMemTotalASP(); %></td>
		   </tr>
		   <tr>
			 <td class="contentpadding">ʣ���ڴ�:</td>
			 <td><% getMemLeftASP(); %></td>
		   </tr>
		</table></td>
	  </tr>
	  <tr>
	    <td><table class="space" width="100%" border="0" cellpadding="0" cellspacing="0">
		   <tr>
			 <td colspan="2" class="titlebg">WAN�� ������Ϣ</td>
		   </tr> 
		   <tr>
			 <td width="25%" class="contentpadding">�հ���:</td>
			 <td width="75%"><% getWANRxPacketASP(); %></td>
		   </tr> 
		   <tr>
			 <td class="contentpadding">�հ��ֽ�:</td>
			 <td><% getWANRxByteASP(); %></td>
		   </tr> 
		   <tr>
			 <td class="contentpadding">���Ͱ���:</td>
			 <td><% getWANTxPacketASP(); %></td>
		   </tr> 
		   <tr>
			 <td class="contentpadding">���Ͱ��ֽ���:</td>
			 <td><% getWANTxByteASP(); %></td>
		   </tr>
	    </table></td>
	 </tr>
	 <tr>
	    <td><table class="space" width="100%" border="0" cellpadding="0" cellspacing="0">
	   	   <tr>
			 <td colspan="2" class="titlebg">LAN�� ������Ϣ</td>
		   </tr> 
		   <tr>
			 <td width="25%" class="contentpadding">���հ���:</td>
			 <td width="75%"><% getLANRxPacketASP(); %></td>
		   </tr> 
		   <tr>
			 <td class="contentpadding">�հ��ֽ���:</td>
			 <td><% getLANRxByteASP(); %></td>
	   	   </tr> 
		   <tr>
			 <td class="contentpadding">���Ͱ���:</td>
			 <td><% getLANRxPacketASP(); %></td>
		   </tr> 
		   <tr>
			 <td class="contentpadding">���Ͱ��ֽ���:</td>
			 <td><% getLANRxByteASP(); %></td>
		   </tr>
	   <!--<tr>
		     <td colspan="2" class="titlebg">���нӿ�:</td>
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