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
    	alert('无效的文件名!')
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
            <td class="nav">当前路径:KN-WR922 &gt;&gt;系统管理 &gt;&gt;保存/加载配置文件</td>
          </tr>
<form method="post" name="ExportSettings" action="/cgi-bin/ExportSettings.sh">
          <tr>
            <td>&nbsp;</td>
          </tr>
		  <tr>
            <td class="space" style="padding-left:10px; padding-right:10px;">&nbsp;&nbsp;本页提供保存(导出)和加载配置文件功能，详细信息请点<input type="button" class="button5" value="帮助" onclick=popHelp('help.htm#saveconf')>。</td>
          </tr>
		  <tr>
		    <td>&nbsp;</td>
		  </tr>
          <tr>
            <td>
			  <table class="space" width="100%" border="0" cellspacing="0" cellpadding="0" >
				  <tr>
					<td class="titlebg" colspan="2">保存配置文件</td>
				  </tr>
				  <tr>
				   <td colspan="2" height="10"></td>
				  </tr>
				  <tr>
					<td width="25%" class="contentpadding">导出按钮:</td>
					<td width="75%"><input value="导 出" name="Export" class="button" type="submit"></td>
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
            			<td class="titlebg" colspan="2">加载配置文件</td>
          			</tr>
					<tr>
				         <td colspan="2" height="10"></td>
				    </tr>
          			<tr>
                  		<td width="25%" class="contentpadding">选择文件:</td>
                  		<td width="75%"><input type="File" name="filename" size="20" maxlength="256">&nbsp;&nbsp;<input type="submit" class="button" value="导 入" onClick="return AdmFormCheck()">&nbsp;&nbsp;<input type="reset" class="button"  value="取 消"></td>
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
