<html>
<head>
<title><% getTitle(); %></title>
<meta http-equiv="Content-Type" content="text/html; charset=<% getCharset(); %>" />
<link href="images/kingnet.css" type="text/css" rel="stylesheet">
<script language="javascript" src="js/language_<% getCfgZero(1, "LanguageType"); %>.js"></script>
<STYLE type="text/css">
td {
	font-size: 12px; 
	line-height: 18px; 
	text-decoration: none;
}
body{
	background-color:#009cdc;
	scrollbar-face-color: #009cdc;
	scrollbar-highlight-color: #009cdc;
	scrollbar-shadow-color: #009cdc;
	scrollbar-3dlight-color: #009cdc;
	scrollbar-arrow-color: #009cdc;
	scrollbar-track-color: #009cdc;
	scrollbar-darkshadow-color: #009cdc;	
}
</STYLE>
<script language="javascript">
function Echange(nowthis,v_num){
	var cc=document.getElementsByTagName("div");
	var sj_q=0;
	for (i=0;i<cc.length;i++)
	{
		if (cc[i].id == "sj")
		{		
			if (sj_q==v_num)
			{
				if (cc[i].style.display!="none")
					cc[i].style.display="none";
				else
					cc[i].style.display="";
			}
			else
				cc[i].style.display="none";
			sj_q++;
		}
	}
}
</script>
</head>
<body style="overflow-x:hidden;">
<table width="172" border="0" cellpadding="0" cellspacing="0">
	<tr>
        <td><img src="images/kingnet_nav.gif"></td>
    </tr>
    <tr>
        <td valign="top">
			<div class="column"><a href="adm/wizard.asp" target="mainFrame"><script>dw(MM_easywizard)</script></a></div>
         	<div class="column"><a href="wireless/stainfo.asp" target="mainFrame" onClick="Echange(this,0)"><script>dw(MM_wireless)</script></a></div>
	       	<div class="xiala" id="sj" style="display:none">
              <ul>
               <li><a href="wireless/stainfo.asp" target="mainFrame"><script>dw(MM_wireless_status)</script></a></li>
                <li><a href="wireless/basic.asp" target="mainFrame"><script>dw(MM_basic_settings)</script></a></li>  
              </ul>
          </div>
		  <div class="column"><a href="adm/settings.asp" target="mainFrame" onClick="Echange(this,1)"><script>dw(MM_management)</script></a></div>
          <div class="xiala" id="sj" style="display:none">
              <ul>
			  	<li><a href="internet/dhcpcliinfo.asp" target="mainFrame"><script>dw(MM_client_list)</script></a></li>
                <li><a href="adm/statistic.asp" target="mainFrame"><script>dw(MM_statistics)</script></a></li>
                <li><a href="adm/settings.asp" target="mainFrame"><script>dw(MM_sysconfig)</script></a></li>
                <li><a href="adm/password.asp" target="mainFrame"><script>dw(MM_admin_settings)</script></a></li>
              </ul>
		     </div>
		  <div class="column"><a href="adm/status.asp" target="mainFrame"><script>dw(MM_sysstatus)</script></a></div>
		   </td>
     </tr>
</table>
</body>
</html>
