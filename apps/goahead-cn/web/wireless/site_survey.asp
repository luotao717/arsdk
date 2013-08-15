<html>
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Expires" content="-1">
<meta http-equiv="Content-Type" content="text/html; charset=<% getCharset(); %>">
<link rel="stylesheet" href="../style/normal_ws.css" type="text/css">
<script language="javascript" src="../js/language_<% getCfgZero(1, "LanguageType"); %>.js"></script>
<script language="javascript" src="../js/common.js"></script>
<script language="javascript">
function trim(str)
{ 
     return str.replace(/(^\s*)|(\s*$)/g, "");
}

function select_SSID()
{
	var h,k;
	var tmpi,tmpj;
	var auth_str;
	//alert(i);
	for(h=0;h<i;h++)
	{
		//alert(h);
		if(i == 1)
		{
			//alert(document.sta_site_survey.selectap.value);
			if(document.sta_site_survey.selectap.checked == true)
			{
				tmpi=h;
				tmpj=document.sta_site_survey.selectap.value;
				break;
			}
		}
		else
		{
			if(document.sta_site_survey.selectap[h].checked == true)
			{
				tmpi=h;
				tmpj=document.sta_site_survey.selectap[h].value;
				break;
			}
		}
	}
	
	//window.opener.document.
	window.opener.document.apcli_form.apcli_ssid.value=str.split("#")[tmpi].split(";")[1];
	window.opener.document.apcli_form.apcli_bssid.value=tmpj;
	
	window.opener.document.apcli_form.apcli_channel.value=str.split("#")[tmpi].split(";")[0];

	auth_str=str.split("#")[tmpi].split(";")[3];			
	if(trim(auth_str)=="NONE")
	{
		window.opener.document.apcli_form.apcli_mode.options.selectedIndex=0;
		window.opener.document.apcli_form.apcli_enc_open.options.selectedIndex=0;
		
		window.opener.document.getElementById("div_apcli_key1").style.display="none";
		window.opener.document.getElementById("div_aplci_enc").style.display="none";
		window.opener.document.getElementById("div_apcli_wpapsk").style.display="none";
		
		window.opener.document.apcli_form.apcli_default_key.options.selectedIndex=0;
		window.opener.document.apcli_form.apcli_key1type.options.selectedIndex=0;
	} 
	else if(trim(auth_str)=="WEP")
	{
		window.opener.document.apcli_form.apcli_mode.options.selectedIndex=0;
		window.opener.document.apcli_form.apcli_enc.options.selectedIndex=1;
		
		window.opener.document.getElementById("div_apcli_key1").style.display="";
		window.opener.document.getElementById("div_aplci_enc").style.display="none";
		window.opener.document.getElementById("div_apcli_wpapsk").style.display="none";
		
		window.opener.document.apcli_form.apcli_default_key.options.selectedIndex=0;
		window.opener.document.apcli_form.apcli_key1type.options.selectedIndex=0;
		window.opener.document.apcli_form.apcli_key1.value="";
		window.opener.document.apcli_form.apcli_key1.focus();
	}
	else if(auth_str.search("WPA") != -1)
	{
		window.opener.document.getElementById("div_apcli_key1").style.display="none";
		window.opener.document.getElementById("div_aplci_enc").style.display="";
		window.opener.document.getElementById("div_apcli_wpapsk").style.display="";
			
		if(auth_str.split("/")[0] =="WPAPSK" || auth_str.split("/")[0] =="WPA1PSK")
		{
			window.opener.document.apcli_form.apcli_mode.options.selectedIndex=2;
			
			if(auth_str.split("/")[1] == "AES" || auth_str.split("/")[1] == "TKIPAES")
				window.opener.document.apcli_form.apcli_enc.options.selectedIndex=1;
			else if(auth_str.split("/")[1] == "TKIP")
				window.opener.document.apcli_form.apcli_enc.options.selectedIndex=0;
		}
		else if(auth_str.split("/")[0] =="WPA2PSK" || auth_str.split("/")[0] =="WPA1PSKWPA2PSK")
		{
			window.opener.document.apcli_form.apcli_mode.options.selectedIndex=3;
			
			if(auth_str.split("/")[1] == "AES" || auth_str.split("/")[1] == "TKIPAES")
				window.opener.document.apcli_form.apcli_enc.options.selectedIndex=1;
			else if(auth_str.split("/")[1] == "TKIP")
				window.opener.document.apcli_form.apcli_enc.options.selectedIndex=0;
		}

		window.opener.document.apcli_form.apcli_wpapsk.value="";
		window.opener.document.apcli_form.apcli_wpapsk.focus();
	}
				
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
