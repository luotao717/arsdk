<html>
<head>
<title>WR922</title>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312" />
<link href="images/kingnet.css" type="text/css" rel="stylesheet">
<script language="JavaScript" type="text/javascript">
//var URLFilterNum = 0;
var HostFilterNum = 0;
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
/*
function deleteWebsURLClick()
{
	for(i=0; i< URLFilterNum; i++){
		var tmp = eval("document.websURLFilterDelete.DR"+i);
		if(tmp.checked == true)
			return true;
	}
	alert("请选择您所要删除的条目.");
	return false;
}
*/
/*
function AddWebsURLFilterClick()
{
	if(document.websURLFilter.addURLFilter.value == ""){
		alert("请填入URL.");
		return false;
	}
	return true;
}
*/

function deleteWebsHostClick()
{
	for(i=0; i< HostFilterNum; i++){
		var tmp = eval("document.websHostFilterDelete.DR"+i);
		if(tmp.checked == true)
			return true;
	}
	alert("请选择要删除的条目。");
	return false;
}

function AddWebsHostFilterClick()
{
	if(document.websHostFilter.addHostFilter.value == ""){
		alert("请输入关键字。");
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
            <td class="nav">当前路径:KN-WR922 &gt;&gt;网络安全 &gt;&gt;主机过滤</td>
          </tr>
        <% checkIfUnderBridgeModeASP(); %>
          <tr>
            <td>&nbsp;</td>
          </tr>
		  <tr>
            <td class="space" style="padding-left:10px; padding-right:10px;">&nbsp;&nbsp;&nbsp;本页面是用来设置路由器的主机(关键字)过滤列表的。如果您不想让局域网的机器访问某类网站，可以使用此功能。如填入sports ，这样所有关于sports的URL将不能访问，如sports.163.com,sports.sina.com.cn等都不能访问.注意：不支持中文。 </td>
          </tr>
		  <tr>
		    <td>&nbsp;</td>
		  </tr>
		  <form action=/goform/websHostFilter method=POST name="websHostFilter">
          <tr>
		    <td>          
			  <table class="space" width=100% border="0" cellpadding="0" cellspacing="0">
				<tr>
				   <td colspan="4" class="titlebg">添加一条过滤条目 </td>
				</tr>
				<tr>
					<td width="25%" class="contentpadding" id="WebsHostFilterKeyword" >主机关键字</td>
					<td width="75%"><input name="addHostFilter" size="16" maxlength="32" type="text"> (注意：不支持中文)</td>
				</tr>
			  </table>
			</td>
		  </tr>
		  <tr>
			<td>&nbsp;</td>
		  </tr>
		  <tr>
			<td class="contentpadding"><input type="hidden" value="/host_filter.asp" name="submit-url"><input type="submit" value="提 交"  class="button" id="WebsHostFilterAdd" name="addwebscontentfilter" onClick="return AddWebsHostFilterClick()">&nbsp;&nbsp;
<input type="reset"  class="button" value="取 消" id="WebsHostFilterReset" name="reset"></td>
		  </tr> 
		  </form> 
		  <tr>
            <td>&nbsp;</td>
          </tr>  
<form action=/goform/websHostFilterDelete method=POST name="websHostFilterDelete">
		  <tr>
		    <td> 
               <table class="space" width=100% border="0" cellpadding="0" cellspacing="0">	
				  <tr>
					<td id="WebsHostFilterCurrent" colspan="5" class="titlebg">当前主机(关键字)过滤列表</td>
				  </tr>
				  <tr>
					<td width="19%" id="WebsHostFilterNo"> NO. </td>
					<td width="81%" id="WebsHostFilterHost"> 主机(关键字) </td>

				  </tr>
				<script language="JavaScript" type="text/javascript">
	var i;
	var entries = new Array();
	var all_str = "<% getCfgGeneral(1, "websHostFilters"); %>";

	if(all_str.length){
		entries = all_str.split(";");

		for(i=0; i<entries.length; i++){
			document.write("<tr><td>");
			document.write(i+1);
			document.write("<input type=checkbox name=DR"+i+"></td>");

			document.write("<td>"+ entries[i] +"</td>");
			document.write("</tr>\n");
		}

		HostFilterNum = entries.length;
	}
	</script>
				</table>
			  </td>
		  </tr>
		  <tr>
            <td>&nbsp;</td>
          </tr> 
		  <tr>
			<td class="contentpadding"><input type="hidden" value="/host_filter.asp" name="submit-url"><input type="submit" value="删除所选项"  class="button3" id="WebsHostFilterDel" name="deleteSelPortForward" onClick="return deleteWebsHostClick()">&nbsp;&nbsp;<input type="reset" value="取 消"  class="button" id="WebsHostFilterReset" name="reset"></td>
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