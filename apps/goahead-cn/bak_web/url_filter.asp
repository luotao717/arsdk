<html>
<head>
<title>WR922</title>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312" />
<link href="images/kingnet.css" type="text/css" rel="stylesheet">
<script language="JavaScript" type="text/javascript">
var URLFilterNum = 0;
//var HostFilterNum = 0;
function deleteClick()
{
    return true;
}

function formCheck()
{
   return true;
}
/*
function updateState()
{
	initTranslation();
	if (document.webContentFilter.websFilterProxy.value == "1")
		document.webContentFilter.websFilterProxy.checked = true;
	if (document.webContentFilter.websFilterJava.value == "1")
		document.webContentFilter.websFilterJava.checked = true;
	if (document.webContentFilter.websFilterActivex.value == "1")
		document.webContentFilter.websFilterActivex.checked = true;

	if (document.webContentFilter.websFilterCookies.value == "1")
		document.webContentFilter.websFilterCookies.checked = true;

}
*/
//function webContentFilterClick()
//{
	//document.webContentFilter.websFilterProxy.value = document.webContentFilter.websFilterProxy.checked ? "1": "0";
	//document.webContentFilter.websFilterJava.value = document.webContentFilter.websFilterJava.checked ? "1": "0";
	//document.webContentFilter.websFilterActivex.value = document.webContentFilter.websFilterActivex.checked ? "1": "0";
	//document.webContentFilter.websFilterCookies.value = document.webContentFilter.websFilterCookies.checked ? "1": "0";
	//return true;
//}

function deleteWebsURLClick()
{
	for(i=0; i< URLFilterNum; i++){
		var tmp = eval("document.websURLFilterDelete.DR"+i);
		if(tmp.checked == true)
			return true;
	}
	alert("��ѡ������Ҫɾ������Ŀ.");
	return false;
}

function AddWebsURLFilterClick()
{
	if(document.websURLFilter.addURLFilter.value == ""){
		alert("������URL.");
		return false;
	}
	return true;
}
/*
function deleteWebsHostClick()
{
	for(i=0; i< HostFilterNum; i++){
		var tmp = eval("document.websHostFilterDelete.DR"+i);
		if(tmp.checked == true)
			return true;
	}
	alert("Please select the rule to be deleted.");
	return false;
}

function AddWebsHostFilterClick()
{
	if(document.websHostFilter.addHostFilter.value == ""){
		alert("Please enter a host filter.");
		return false;
	}
	return true;
}
*/
</script>
</head>
<body>

<table width="90%" border="0" cellpadding="0" cellspacing="0" align="center">
  <tr>
    <td style="padding-top:30px;">
	   <table width="100%" border="0" cellpadding="0" cellspacing="0">
          <tr>
            <td class="nav">��ǰ·��:KN-WR922 &gt;&gt;���簲ȫ &gt;&gt;��������</td>
          </tr>
        <% checkIfUnderBridgeModeASP(); %>
          <tr>
            <td>&nbsp;</td>
          </tr>
		  <tr>
            <td class="space" style="padding-left:10px; padding-right:10px;">&nbsp;&nbsp;&nbsp;��ҳ������������·������URL(����)�����б�ġ�����������þ������Ļ�������ĳ����վ������ʹ�ô˹��ܡ�������sohu.com ����������www.sohu.com���ܷ��ʣ�������sohu.com�Ķ���������������Ҳ�����ܷ��ʡ� </td>
          </tr>
		  <tr>
		    <td>&nbsp;</td>
		  </tr>
		  <form action=/goform/websURLFilter method=POST name="websURLFilter">
          <tr>
		    <td>          
			  <table class="space" width=100% border="0" cellpadding="0" cellspacing="0">
				<tr>
				   <td colspan="4" class="titlebg">���һ��������Ŀ </td>
				</tr>
				<tr>
					<td width="25%" class="contentpadding" id="urlFilterU" >URL:</td>
					<td width="75%"><input name="addURLFilter" size="16" maxlength="32" type="text"></td>
				</tr>
			  </table>
			</td>
		  </tr>
		  <tr>
			<td>&nbsp;</td>
		  </tr>
		  <tr>
			<td class="contentpadding"><input type="hidden" value="/url_filter.asp" name="submit-url"><input class="button3" type="submit" value="�ύ" id="WebURLFilterAdd" name="addwebsurlfilter" onClick="return AddWebsURLFilterClick()">&nbsp;&nbsp;
<input class="button3" type="Reset" value="ȡ��" id="WebURLFilterReset" name="reset"></td>
		  </tr> 
		  </form> 
		  <tr>
            <td>&nbsp;</td>
          </tr>  
		  <form action=/goform/websURLFilterDelete method=POST name="websURLFilterDelete">
		  <tr>
		    <td> 
               <table class="space" width=100% border="0" cellpadding="0" cellspacing="0">	
				  <tr>
					<td id="WebURLFilterCurrent" colspan="5" class="titlebg">��ǰ���������б�</td>
				  </tr>
				  <tr>
					<td width="19%" id="WebURLFilterNo"> NO. </td>
					<td width="81%" id="WebURLFilterURL"> URL ��ַ </td>

				  </tr>
				<script language="JavaScript" type="text/javascript">
	var i;
	var entries = new Array();
	var all_str = "<% getCfgGeneral(1, "websURLFilters"); %>";

	if(all_str.length){
		entries = all_str.split(";");
		for(i=0; i<entries.length; i++){
			document.write("<tr><td>");
			document.write(i+1);
			document.write("<input type=checkbox name=DR"+i+"></td>");
	
			document.write("<td>"+ entries[i] +"</td>");
			document.write("</tr>\n");
		}

		URLFilterNum = entries.length;
	}
	</script>
				</table>
			  </td>
		  </tr>
		  <tr>
            <td>&nbsp;</td>
          </tr> 
		  <tr>
			<td class="contentpadding"><input type="hidden" value="/url_filter.asp" name="submit-url"><input class="button3" type="submit" value="ɾ����ѡ��" id="WebURLFilterDel" name="deleteSelPortForward" onClick="return deleteWebsURLClick()">&nbsp;&nbsp;
<input class="button3" type="reset" value="ȡ��" id="WebURLFilterReset" name="reset"></td>
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