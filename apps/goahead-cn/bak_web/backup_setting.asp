<html>
<head>
<title>WR922</title>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312" />
<link href="images/kingnet.css" type="text/css" rel="stylesheet">
<SCRIPT src="helpScript.js"></SCRIPT>
<script language="JavaScript" type="text/javascript">
function AdmFormCheck()
{
	str=document.ImportSettings.filename.value;
	if (str.indexOf("KINGNET_WR922_Settings.dat")!=-1)
    {
    	return true;
    }
    else
    {
    	alert('��Ч���ļ���!')
    	return false;
    }
}

</script>
</head>
<body>
<table width="90%" border="0" cellpadding="0" cellspacing="0" align="center">
  <tr>
    <td style="padding-top:30px;">
	   <table width="100%" border="0" cellpadding="0" cellspacing="0">
          <tr>
            <td class="nav">��ǰ·��:KN-WR922 &gt;&gt;ϵͳ���� &gt;&gt;����/���������ļ�</td>
          </tr>
<form method="post" name="ExportSettings" action="/cgi-bin/ExportSettings.sh">
          <tr>
            <td>&nbsp;</td>
          </tr>
		  <tr>
            <td class="space" style="padding-left:10px; padding-right:10px;">&nbsp;&nbsp;��ҳ�ṩ����(����)�ͼ��������ļ����ܣ���ϸ��Ϣ���<input type="button" class="button5" value="����" onclick=popHelp('help.htm#saveconf')>��</td>
          </tr>
		  <tr>
		    <td>&nbsp;</td>
		  </tr>
          <tr>
            <td>
			  <table class="space" width="100%" border="0" cellspacing="0" cellpadding="0" >
				  <tr>
					<td class="titlebg" colspan="2">���������ļ�</td>
				  </tr>
				  <tr>
				   <td colspan="2" height="10"></td>
				  </tr>
				  <tr>
					<td width="25%" class="contentpadding">������ť:</td>
					<td width="75%"><input value="�� ��" name="Export" class="button" type="submit"></td>
				  </tr>
				  <tr>
				   <td colspan="2" height="10"></td>
				  </tr>
         	  </table>
			</td>
		 </tr>
</form>
       	 <tr>
            <td>&nbsp;</td>
         </tr>
<form method="post" name="ImportSettings" action="/cgi-bin/upload_settings.cgi" enctype="multipart/form-data">
         <tr>
		 	<td>
         		<table class="space" width="100%" border="0" cellspacing="0" cellpadding="0">
					<tr>
            			<td class="titlebg" colspan="2">���������ļ�</td>
          			</tr>
					<tr>
				         <td colspan="2" height="10"></td>
				    </tr>
          			<tr>
                  		<td width="25%" class="contentpadding">ѡ���ļ�:</td>
                  		<td width="75%"><input type="File" name="filename" size="20" maxlength="256">&nbsp;&nbsp;<input type="submit" class="button" value="�� ��" onClick="return AdmFormCheck()">&nbsp;&nbsp;<input type="reset" class="button"  value="ȡ ��"></td>
                	</tr>
					<tr>
				        <td colspan="2" height="10"></td>
				    </tr>
         		</table>
			</td>
		 </tr>
</form>		
		 <tr>
			<td>&nbsp;</td>
		 </tr>
       </table>
	</td>
  </tr>
</table>
</body>
</html>
