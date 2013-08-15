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
	alert("请选择您所要删除的条目.");
	return false;
}

function AddWebsURLFilterClick()
{
	if(document.websURLFilter.addURLFilter.value == ""){
		alert("请填入URL.");
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
            <td class="nav">当前路径:KN-WR922 &gt;&gt;网络安全 &gt;&gt;域名过滤</td>
          </tr>
        <% checkIfUnderBridgeModeASP(); %>
          <tr>
            <td>&nbsp;</td>
          </tr>
		  <tr>
            <td class="space" style="padding-left:10px; padding-right:10px;">&nbsp;&nbsp;&nbsp;本页面是用来设置路由器的URL(域名)过滤列表的。如果您不想让局域网的机器访问某个网站，可以使用此功能。如填入sohu.com ，这样不仅www.sohu.com不能访问，其它的sohu.com的二级或者三级域名也将不能访问。 </td>
          </tr>
		  <tr>
		    <td>&nbsp;</td>
		  </tr>
		  <form action=/goform/websURLFilter method=POST name="websURLFilter">
          <tr>
		    <td>          
			  <table class="space" width=100% border="0" cellpadding="0" cellspacing="0">
				<tr>
				   <td colspan="4" class="titlebg">添加一条过滤条目 </td>
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
			<td class="contentpadding"><input type="hidden" value="/url_filter.asp" name="submit-url"><input class="button3" type="submit" value="提交" id="WebURLFilterAdd" name="addwebsurlfilter" onClick="return AddWebsURLFilterClick()">&nbsp;&nbsp;
<input class="button3" type="Reset" value="取消" id="WebURLFilterReset" name="reset"></td>
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
					<td id="WebURLFilterCurrent" colspan="5" class="titlebg">当前域名过滤列表</td>
				  </tr>
				  <tr>
					<td width="19%" id="WebURLFilterNo"> NO. </td>
					<td width="81%" id="WebURLFilterURL"> URL 地址 </td>

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
			<td class="contentpadding"><input type="hidden" value="/url_filter.asp" name="submit-url"><input class="button3" type="submit" value="删除所选项" id="WebURLFilterDel" name="deleteSelPortForward" onClick="return deleteWebsURLClick()">&nbsp;&nbsp;
<input class="button3" type="reset" value="取消" id="WebURLFilterReset" name="reset"></td>
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