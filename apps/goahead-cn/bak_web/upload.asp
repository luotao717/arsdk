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
document.write('<div id="loading"><br><br><br>���������̼� <br><br>���Ժ�...</div>');

var _singleton = 0;
function formCheck()
{
	if(_singleton)
		return false;
	if(document.UploadFirmware.filename.value == ""){
		alert("�ļ�����.");
		return false;
	}
	if(document.UploadFirmware.filename.value.indexOf('NOR2M4MLT')==-1){
		alert("�ļ�������˶�����汾");
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
	ret = confirm("ȷ���Ƿ���Ҫ����BOOT!");
	if(!ret)
		return false;
	if(_singleton)
		return false;
	if(document.UploadBootloader.filename.value == ""){
		alert("����Bootloader :��ѡ���ļ�.");
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
            <td class="nav">��ǰ·��:KN-WR922 &gt;&gt;ϵͳ���� &gt;&gt;�̼�����</td>
          </tr>
          <tr>
            <td>&nbsp;</td>
          </tr>
		  <tr>
            <td class="space" style="padding-left:10px; padding-right:10px;">&nbsp;&nbsp;&nbsp; ��ҳ����������·�������������ģ������������ǵ���վ��<a href="http://www.kingnet.com.cn">www.kingnet.com.cn</a>�������ض�Ӧ��Ʒ���ͺŵ����������������<span class="STYLE2">ע�⣺1���뽫�豸�ָ����������Ժ��ٽ���������2�������ȷ�������ص����������ƥ��ģ���������ϵͳ�𻵡�3������������������ر�������PC��·��������ͨ�Ͳ�Ҫ�رյ�Դ�������������ϵͳ�𻵡�</span>ϵͳ�����ɹ��󣬻������ʾ��Ϣ�������ĵȺ� </td>
          </tr>
		  <tr>
		    <td>&nbsp;</td>
		  </tr>
<form method="post" name="UploadFirmware" action="/cgi-bin/upload.cgi" enctype="multipart/form-data">
          <tr>
            <td><table class="space" width=100% border="0" cellpadding="0" cellspacing="0">
				<tr>
					<td class="titlebg" colspan="2" >�̼�����</td>
				</tr>
				<tr>
				   <td colspan="2" height="10"></td>
				</tr>
				<tr>
					<td width=25% class="contentpadding STYLE2">��ǰ����汾:</td>
					<td width=75%><span class="STYLE2">
				    <% getSdkVersion(); %>
					  </span></td>
				</tr>
				<tr>
					<td width=25% class="contentpadding">ѡ���ļ�:</td>
					<td width=75%><input name="filename" size="20" maxlength="256" type="file"></td>
				</tr>
				
			</table></td>
		  </tr>
		  <tr><td>&nbsp;</td></tr>
		  <tr>
		    <td class="contentpadding"><input value="�� ��" class="button" name="UploadFirmwareSubmit" onClick="return formCheck()" type="submit">&nbsp;&nbsp;<input value="ȡ ��" class="button" name="reset" type="reset"></td>
		  </tr>
		  <tr><td>&nbsp;</td></tr>
		 
		  
</form>
 <!--         
          <form method="post" name="UploadBootloader" action="/cgi-bin/upload_bootloader.cgi" enctype="multipart/form-data">
                    <tr><td><table width=100% border="0" cellpadding="0" cellspacing="0">
<tr>
  <td class="titlebg" colspan="2" >BOOT����</td>
</tr>
<tr>
	<td width=25% class="contentpadding">ѡ���ļ�</span>:</td>
	<td width=75%>
  		 <input name="filename" size="20" maxlength="256" type="file">	</td>
</tr>
          
          
          </table></td></tr>
           <tr><td>&nbsp;</td></tr>
<tr><td >
<input value="�� ��" name="UploadBootloaderSubmit" onClick="return formBootloaderCheck()" type="submit">
 &nbsp;&nbsp;
<input value="ȡ ��" name="reset" type="reset"></td></tr>   </form>       
  -->        
        </table>
	 </td>
  </tr>
  <tr>
     <td>&nbsp;</td>
  </tr>
</table></body></html>
