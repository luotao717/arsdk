<html>
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Expires" content="-1">
<meta http-equiv="Content-Type" content="text/html; charset=<% getCharset(); %>">
<link rel="stylesheet" href="../style/normal_ws.css" type="text/css">
<script language="javascript" src="../js/language_<% getCfgZero(1, "LanguageType"); %>.js"></script>
<script language="javascript" src="../js/common.js"></script>
<script language="javascript">
var flag = eval(location.href.split("#")[1]);

function trim(str)
{ 
     return str.replace(/(^\s*)|(\s*$)/g, "");
}

function select_SSID()
{
	var h,k;
	var tmpi,tmpj;
	
	for(h=0;h<i;h++)
	{
		if(document.sta_site_survey.selectap[h].checked == true)
		{
			tmpi=h;
			tmpj=document.sta_site_survey.selectap[h].value;
			break;
		}
	}
	
	//window.opener.document.
	if (flag == 1)
		window.opener.document.wds_form.wds_1.value=tmpj;
	else if (flag == 2)
		window.opener.document.wds_form.wds_2.value=tmpj;
	else if (flag == 3)
		window.opener.document.wds_form.wds_3.value=tmpj;
	else if (flag == 4)
		window.opener.document.wds_form.wds_4.value=tmpj;
	
	window.close(); 
}

function reload_table()
{
	window.location.href="site_survey.asp";
}
</script>
</head>
<body>
<table width=600><tr><td>
<table width=100% border=0 cellpadding=3 cellspacing=1> 
<tr><td class="title"><script>dw(MM_site_survey_table)</script></td></tr>
<tr><td><script>dw(JS_msg_sitesurvey)</script></td></tr>
<tr><td><hr></td></tr>
</table>

<form method=post name="sta_site_survey">
<table width=100% border=0 cellpadding=3 cellspacing=1>
<tr>
	<td align="right"><script>dw('<input type=button class=button3 value='+BT_search_ap+' name=refresh onClick="reload_table()">')</script></td>
</tr>
</table>
<br>
<table width=100% border=1 cellpadding=3 style="border-collapse: collapse" bordercolor="#C8C8C8">
  <tr align="center">
    <td bgcolor="#f2f2f2"><script>dw(MM_channel)</script></td>
    <td bgcolor="#f2f2f2">SSID</td>
    <td bgcolor="#f2f2f2">BSSID</td>
    <td bgcolor="#f2f2f2"><script>dw(MM_encryp_type)</script></td>
    <td bgcolor="#f2f2f2"><script>dw(MM_signal)</script>(%)</td>
    <td bgcolor="#f2f2f2"><script>dw(MM_network_mode)</script></td>
    <!--<td bgcolor="#f2f2f2">&nbsp;</td>-->
  </tr>
  <script>
    var str= "<% ApcliScan(); %>";
    var i=0,j=0;
	var str1;
	while(str.split("#")[i]!='' && typeof(str.split(";")[i])!= "undefined" && str.split("#")[i]!=' ')
	{
		document.write("<tr>");
		str1=str.split("#")[i];
		j=0;
		for(j=0;j<6;j++)
		{
			if(!j)
				document.write("<td><input type=radio name=selectap value="+str1.split(";")[2]+">");
			else
				document.write("<td>");
				
			if(j==4)
				document.write(str1.split(";")[j]+"%");
			else
				document.write(str1.split(";")[j]);
			document.write("</td>");
		}
		document.write("</tr>");
		i++;
	}
	document.write('<tr><td colspan=7 align=right><input type=button class=button name=search_AP value="'+MM_select+'" onclick=\"select_SSID()\"></td></tr>');
  </script>
</table>
</form>

</td></tr></table>
</body></html>
