<html>
<head>
<title>WR922</title>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312" />
<link href="images/kingnet.css" type="text/css" rel="stylesheet">
<style type="text/css">
<!--
#loading {
       width: 250px;
       height: 200px;
       background-color: #3399ff;
       position: absolute;
       left: 50%;
       top: 50%;
       margin-top: -150px;
       margin-left: -250px;
       text-align: center;
}
.STYLE2 {color: #CC0000}
-->
</style>
<script language="JavaScript" type="text/javascript">
document.write('<div id="loading"><br><br><br>正在升级固件 <br><br>请稍候...</div>');

var _singleton = 0;
function formCheck()
{
	if(_singleton)
		return false;
	if(document.UploadFirmware.filename.value == ""){
		alert("文件错误.");
		return false;
	}
	if(document.UploadFirmware.filename.value.indexOf('NOR2M4MLT')==-1){
		alert("文件错误，请核对软件版本");
		return false;
	}
//	document.UploadBootloader.UploadBootloaderSubmit.disabled = true;
//	document.UploadBootloader.reset.disabled = true;

    document.getElementById("loading").style.display="block";

	_singleton = 1;
	return true;
}

function formBootloaderCheck()
{
	ret = confirm("确认是否需要升级BOOT!");
	if(!ret)
		return false;
	if(_singleton)
		return false;
	if(document.UploadBootloader.filename.value == ""){
		alert("升级Bootloader :请选择文件.");
		return false;
	}

	document.UploadFirmware.UploadFirmwareSubmit.disabled = true;
	document.UploadFirmware.reset.disabled = true;

	document.getElementById("loading").style.display="block";

	_singleton = 1;
	return true;
}


function pageInit(){
    document.UploadFirmware.UploadFirmwareSubmit.disabled = false;
    document.UploadFirmware.reset.disabled = false;
//    document.UploadBootloader.UploadBootloaderSubmit.disabled = false;
//    document.UploadBootloader.reset.disabled = false;

//	document.UploadFirmware.filename.disabled = false;

	document.getElementById("loading").style.display="none";
}
</script></head>
<body onLoad="pageInit()">
<table width="90%" border="0" cellpadding="0" cellspacing="0" align="center">
  <tr>
    <td style="padding-top:30px;">
	   <table width="100%" border="0" cellpadding="0" cellspacing="0">
          <tr>
            <td class="nav">当前路径:KN-WR922 &gt;&gt;系统管理 &gt;&gt;固件升级</td>
          </tr>
          <tr>
            <td>&nbsp;</td>
          </tr>
		  <tr>
            <td class="space" style="padding-left:10px; padding-right:10px;">&nbsp;&nbsp;&nbsp; 本页面是用来给路由器进行升级的，您可以在我们的网站（<a href="http://www.kingnet.com.cn">www.kingnet.com.cn</a>）上下载对应产品和型号的新软件进行升级。<span class="STYLE2">注意：1、请将设备恢复出厂设置以后再进行升级。2、请务必确认您下载的升级软件是匹配的，否则会造成系统损坏。3、在升级过程中请务必保持您的PC与路由器的连通和不要关闭电源，否则容易造成系统损坏。</span>系统升级成功后，会出现提示信息，请耐心等候。 </td>
          </tr>
		  <tr>
		    <td>&nbsp;</td>
		  </tr>
<form method="post" name="UploadFirmware" action="/cgi-bin/upload.cgi" enctype="multipart/form-data">
          <tr>
            <td><table class="space" width=100% border="0" cellpadding="0" cellspacing="0">
				<tr>
					<td class="titlebg" colspan="2" >固件升级</td>
				</tr>
				<tr>
				   <td colspan="2" height="10"></td>
				</tr>
				<tr>
					<td width=25% class="contentpadding STYLE2">当前软件版本:</td>
					<td width=75%><span class="STYLE2">
				    <% getSdkVersion(); %>
					  </span></td>
				</tr>
				<tr>
					<td width=25% class="contentpadding">选择文件:</td>
					<td width=75%><input name="filename" size="20" maxlength="256" type="file"></td>
				</tr>
				
			</table></td>
		  </tr>
		  <tr><td>&nbsp;</td></tr>
		  <tr>
		    <td class="contentpadding"><input value="提 交" class="button" name="UploadFirmwareSubmit" onClick="return formCheck()" type="submit">&nbsp;&nbsp;<input value="取 消" class="button" name="reset" type="reset"></td>
		  </tr>
		  <tr><td>&nbsp;</td></tr>
		 
		  
</form>
 <!--         
          <form method="post" name="UploadBootloader" action="/cgi-bin/upload_bootloader.cgi" enctype="multipart/form-data">
                    <tr><td><table width=100% border="0" cellpadding="0" cellspacing="0">
<tr>
  <td class="titlebg" colspan="2" >BOOT升级</td>
</tr>
<tr>
	<td width=25% class="contentpadding">选择文件</span>:</td>
	<td width=75%>
  		 <input name="filename" size="20" maxlength="256" type="file">	</td>
</tr>
          
          
          </table></td></tr>
           <tr><td>&nbsp;</td></tr>
<tr><td >
<input value="提 交" name="UploadBootloaderSubmit" onClick="return formBootloaderCheck()" type="submit">
 &nbsp;&nbsp;
<input value="取 消" name="reset" type="reset"></td></tr>   </form>       
  -->        
        </table>
	 </td>
  </tr>
  <tr>
     <td>&nbsp;</td>
  </tr>
</table></body></html>
