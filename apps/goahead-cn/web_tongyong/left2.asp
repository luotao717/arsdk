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
			<div class="column"><a href="internet/lan.asp" target="mainFrame" onClick="Echange(this,0)"><script>dw(MM_network)</script></a></div>
          <div class="xiala" id="sj" style="display:none">
              <ul>
                <li><a href="internet/lan.asp" target="mainFrame"><script>dw(MM_lan_settings)</script></a></li>
              </ul>
          </div>
         	<div class="column"><a href="wireless/stainfo.asp" target="mainFrame" onClick="Echange(this,1)"><script>dw(MM_wireless)</script></a></div>
	       	<div class="xiala" id="sj" style="display:none">
              <ul>
               <li><a href="wireless/stainfo.asp" target="mainFrame"><script>dw(MM_wireless_status)</script></a></li>
                <li><a href="wireless/basic.asp" target="mainFrame"><script>dw(MM_basic_settings)</script></a></li>
				<li><a href="wireless/macfilter.asp" target="mainFrame"><script>dw(MM_wireless_mac)</script></a></li>
				<!--
				<li><a href="wireless/multipleap.asp" target="mainFrame"><script>dw(MM_multiple_settings)</script></a></li>
				<li><a href="wireless/wps.asp" target="mainFrame"><script>dw(MM_wps_settings)</script></a></li>
				-->
				<li><a href="wireless/advanced.asp" target="mainFrame"><script>dw(MM_advanced_settings)</script></a></li>          
              </ul>
          </div>
          <div class="column"><a href="usb/storage_user_admin.asp" target="mainFrame" onClick="Echange(this,2)"><script>dw(MM_storage)</script></a></div>
          <div class="xiala" id="sj" style="display:none">
              <ul>
 				<li><a href="usb/storage_user_admin.asp" target="mainFrame"><script>dw(MM_user)</script></a></li>
				<li><a href="usb/storage_disk_admin.asp" target="mainFrame"><script>dw(MM_disk)</script></a></li>
				<!--
				<li><a href="usb/ftp_srv.asp" target="mainFrame"><script>dw(MM_ftp)</script></a></li>
				-->
				<li><a href="usb/smb_srv.asp" target="mainFrame"><script>dw(MM_samba)</script></a></li> 
				<li><a href="usb/minidlna.asp" target="mainFrame"><script>dw(MM_minidlna_settings)</script></a></li>   
              </ul>
          </div>
          <div class="column"><a href="adm/ntp.asp" target="mainFrame" onClick="Echange(this,3)"><script>dw(MM_service_settings)</script></a></div>
          <div class="xiala" id="sj" style="display:none">
              <ul>
				<li><a href="adm/ntp.asp" target="mainFrame"><script>dw(MM_ntp_settings)</script></a></li>                
              </ul>
          </div>
          <div class="column"><a href="adm/statistic.asp" target="mainFrame" onClick="Echange(this,4)"><script>dw(MM_management)</script></a></div>
          <div class="xiala" id="sj" style="display:none">
              <ul>
                <li><a href="wireless/opmode.asp" target="mainFrame"><script>dw(MM_bridge_settings)</script></a></li>
                <li><a href="adm/statistic.asp" target="mainFrame"><script>dw(MM_statistics)</script></a></li>
                <li><a href="adm/syslog.asp" target="mainFrame"><script>dw(MM_syslog)</script></a></li>
                <li><a href="adm/upload_firmware.asp" target="mainFrame"><script>dw(MM_firmware)</script></a></li>
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
