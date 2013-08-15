<html>
<head>
<title></title>
<meta http-equiv="Content-Type" content="text/html; charset=<% getCharset(); %>">
<link href="style/menu.css" rel="stylesheet" type="text/css" />
<script language="javascript" src="js/language_<% getCfgZero(1, "LanguageType"); %>.js"></script>
<script language="javascript" src="js/forbidView.js"></script>
</head>
<body style="border-left:1px solid #eceded;background-color:#f6f6f6">
<script type="text/javascript">
var opmode = '<% getCfgZero(1, "OperationMode"); %>';
var dhcpen = '<% getCfgZero(1, "dhcpEnabled"); %>';
var dpbsta = '<% getDpbSta(); %>';
var vpnen = '<% getVPNBuilt(); %>';
var ethconv = '<% getCfgZero(1, "ethConvert"); %>';
var meshb = '<% getMeshBuilt(); %>';
var hsb = '<% getHSBuilt(); %>';
var wdsb = '<% getWDSBuilt(); %>';
var wscb = '<% getWSCBuilt(); %>';
var usbb = '<% getUSBBuilt(); %>';
var storageb = '<% getStorageBuilt(); %>';
var ftpb = '<% getFtpBuilt(); %>';
var smbb = '<% getSmbBuilt(); %>';
var mediab = '<% getMediaBuilt(); %>';
var webcamb = '<% getWebCamBuilt(); %>';
var printersrvb = '<% getPrinterSrvBuilt(); %>';
var itunesb = '<% getiTunesBuilt(); %>';
var syslogb = '<% getSysLogBuilt(); %>';
var swqos = '<% getSWQoSBuilt(); %>';
var ad = '<% isAntennaDiversityBuilt(); %>';
//var wizardb = '<% getWizardBuilt(); %>';
var ipv6b = '<% getIPv6Built(); %>';
var pktfilterb = '<% getPktFilterBuilt(); %>';
var ddnsb = "<% getDDNSBuilt(); %>";
var apclib = "<% getWlanApcliBuilt(); %>";
var apcli_en = '<% getCfgZero(1, "apClient"); %>';
var upnpb = "<% getUpnpBuilt(); %>";

a = new Menu('a');
//  nodeID, parent nodeID,  Name,  URL
a.add(1,   0, MM_sysstatus,        "adm/status.asp",	"view");
if (opmode != '0')
	a.add(2,   0, MM_easywizard,        "adm/wizard.asp",	"view");
////////////////////////network//////////////////////////
if (opmode != '0')
{
	a.add(3,   0, MM_network,        	"internet/wan.asp",	"view");
	a.add(301,   3, MM_wan_settings,        	"internet/wan.asp");
	a.add(302,   3, MM_lan_settings,        	"internet/lan.asp");
	if (opmode != '3')
		a.add(303,   3, MM_clone_mac_settings,        	"internet/clone_mac.asp");
	if (ddnsb == '1')
		a.add(304,  3, MM_ddns_settings,        	"firewall/ddns.asp");
}
else
{
	a.add(3,   0, MM_network,        	"internet/lan.asp",	"view");
	a.add(302,   3, MM_lan_settings,        	"internet/lan.asp");
}
////////////////////////wireless//////////////////////////
if (!((opmode == '0' && dpbsta == '1' && ethconv == '1') || opmode == '2'))
{
	a.add(4,   0, MM_wireless,        	"wireless/basic.asp",	"view");
	a.add(402,   4, MM_basic_settings,        		"wireless/basic.asp");
	//a.add(403,   4, MM_security_settings,        	"wireless/security.asp");
	a.add(404,   4, MM_wireless_mac,        	"wireless/macfilter.asp");
	a.add(405,   4, MM_multiple_settings,        	"wireless/multipleap.asp");
	if (wscb == "1")
		a.add(406,   4, MM_wps_settings,        		"wps/wps.asp");
	a.add(407,   4, MM_bridge_settings,        	"wireless/opmode.asp");
	a.add(408,   4, MM_advanced_settings,        	"wireless/advanced.asp");
	a.add(401,   4, MM_wireless_status,        	"wireless/stainfo.asp");
}
////////////////////////dhcp//////////////////////////
a.add(5,   0, MM_dhcp_server,        	"internet/dhcp.asp",	"view");
a.add(501,   5, MM_dhcp_server_settings,        "internet/dhcp.asp");
if (dhcpen == '1')
{
	a.add(502,   5, "DHCP Server"+ MM_client_list,        "internet/dhcpcliinfo.asp");
	a.add(503,   5, MM_static_dhcp_settings,        "internet/static_dhcp.asp");
}

////////////////////////qos//////////////////////////
if (opmode == '1' && swqos == '1')
	a.add(6,   0, MM_qos,        	"internet/qos.asp",	"view");
////////////////////////routing//////////////////////////
a.add(7,   0, MM_routing_table,      	"internet/routing.asp",	"view");
////////////////////////nat//////////////////////////
if (opmode != '0')
{
	a.add(8,   0, "NAT",      	"firewall/port_forward.asp",	"view");
	a.add(801,  8, MM_port_forwarding,        	"firewall/port_forward.asp");
	//a.add(802,  8, MM_vserver,        			"firewall/virtual_server.asp");
	a.add(803,  8, MM_dmz_settings,        		"firewall/dmz.asp");
	if (upnpb == "1")
		a.add(804,  8, MM_upnp_settings,        		"internet/upnp.asp");
}
////////////////////////firewall//////////////////////////
if (opmode != '0')
{
	if (pktfilterb != '0') 
	{
		a.add(9,   0, MM_firewall,      	"firewall/system_firewall.asp",	"view");
		a.add(901,  9, MM_sys_firewall,        		"firewall/system_firewall.asp");
		a.add(902,  9, MM_ipportf,        			"firewall/port_filtering.asp");
		a.add(903,  9, MM_urlf,        				"firewall/url_filtering.asp");
	}
	a.add(904,  9, MM_remote,        				"firewall/remote.asp");
}
////////////////////////usb//////////////////////////
if (usbb == "1")
{
	var usbapp = 1*webcamb | 1*itunesb;
	if (usbapp == 1)
		a.add(10,   0, "USB",      			"usb/uvc_webcam.asp",	"view");
	if (webcamb == "1")
		a.add(1001,  10, "Web Camera",        		"usb/uvc_webcam.asp");
	if (itunesb == "1")
		a.add(1002,  10, "iTunes Server",        		"usb/itunes_srv.asp");
	
	////////////////////////storage//////////////////////////
	if (storageb == "1")
	{
		a.add(11,   0, MM_storage,      	"usb/storage_user_admin.asp",	"view");
		a.add(1101,  11, MM_user,        				"usb/storage_user_admin.asp");
		a.add(1102,  11, MM_disk,        				"usb/storage_disk_admin.asp");
		if (ftpb == "1")
			a.add(1103,  11, MM_ftp,        				"usb/ftp_srv.asp");
		if (smbb == "1")
			a.add(1104,  11, MM_samba,        			"usb/smb_srv.asp");
		if (mediab == "1")
			a.add(1105,  11, MM_media,        			"usb/media_srv.asp");
		if (printersrvb == "1")
			a.add(1106,  11, MM_printer_srv,        		"usb/p910printer_srv.asp");
	}
}
////////////////////////management//////////////////////////
a.add(12,   0, MM_management,      			"adm/statistic.asp",	"view");
a.add(1201,  12, MM_statistics,        			"adm/statistic.asp");
a.add(1202,  12, MM_ntp_settings,        			"adm/ntp.asp");
if (syslogb == "1")
	a.add(1203,  12, MM_syslog,        			"adm/syslog.asp");
a.add(1204,  12, MM_firmware,        				"adm/upload_firmware.asp");
a.add(1205,  12, MM_sysconfig,        			"adm/settings.asp");
a.add(1206,  12, MM_restore_default,        		"adm/load_default.asp");
a.add(1207,  12, MM_reboot,        				"adm/reboot.asp");
a.add(1208,  12, MM_admin_settings,        			"adm/password.asp");
document.write(a);
</script>
</body>
</html>
