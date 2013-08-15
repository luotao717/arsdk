<html>
<head>
<title>WR922</title>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312" />
<link href="images/kingnet.css" type="text/css" rel="stylesheet">
<SCRIPT src="helpScript.js"></SCRIPT>
<script language="JavaScript" type="text/javascript">
var opmode;
var old_mode;
function changeMode()
{
	var nat_en = "<% getCfgZero(1, "natEnabled"); %>";
	var dpbsta = "<% getDpbSta(); %>";
	var ec_en = "<% getCfgZero(1, "ethConvert"); %>";
	var mii_built = "<% getMiiInicBuilt(); %>"
	var mii_en = "<% getCfg2Zero(1, "InicMiiEnable"); %>";

	document.getElementById("eth_conv").style.visibility = "hidden";
	document.getElementById("eth_conv").style.display = "none";
	document.getElementById("eth_conv").style.disable = true;
	document.getElementById("nat").style.visibility = "hidden";
	document.getElementById("nat").style.display = "none";
	document.getElementById("nat").style.disable = true;
	document.getElementById("miiInic").style.visibility = "hidden";
	document.getElementById("miiInic").style.display = "none";
	document.getElementById("miiInic").style.disable = true;

	if (document.opmode.opMode[0].checked) {
		opmode = 0;
		if (dpbsta == "1") {
			document.getElementById("eth_conv").style.visibility = "visible";
			document.getElementById("eth_conv").style.display = "block";
			document.getElementById("eth_conv").style.disable = false;
			if (ec_en == "1") {
				document.opmode.ethConv.options.selectedIndex = 1;
			}
		}
		if (mii_built == "1") {
			document.getElementById("miiInic").style.visibility = "visible";
			document.getElementById("miiInic").style.display = "block";
			document.getElementById("miiInic").style.disable = false;
			if (mii_en == "1") {
				document.opmode.miiMode.options.selectedIndex = 1;
			}
		}
	}
	else if (document.opmode.opMode[1].checked || document.opmode.opMode[3].checked) {
		opmode = 1;
		document.getElementById("nat").style.visibility = "visible";
		document.getElementById("nat").style.display = "block";
		document.getElementById("nat").style.disable = false;
		if (nat_en == "1") {
			document.opmode.natEnbl.options.selectedIndex = 1;
		}
	}
	else if (document.opmode.opMode[2].checked) {
		opmode = 2;
		//keep the nat table hidden, and always enable NAT
		document.getElementById("nat").style.disable = false;
		document.opmode.natEnbl.options.selectedIndex = 1;
	}
}

function initValue()
{
	opmode = "<% getCfgZero(1, "OperationMode"); %>";
	old_mode = opmode;

	var apcli = "<% getWlanApcliBuilt(); %>";
	var sta = "<% getStationBuilt(); %>";

	if (apcli == "0") {
		document.getElementById("apclidt").style.visibility = "hidden";
		document.getElementById("apclidt").style.display = "none";
		document.getElementById("apclidd").style.visibility = "hidden";
		document.getElementById("apclidd").style.display = "none";
	}
	if (sta == "0") {
		document.getElementById("stadt").style.visibility = "hidden";
		document.getElementById("stadt").style.display = "none";
		document.getElementById("stadd").style.visibility = "hidden";
		document.getElementById("stadd").style.display = "none";
	}

	if (opmode == "0")
		document.opmode.opMode[0].checked = true;
	else if (opmode == "1")
		document.opmode.opMode[1].checked = true;
	else if (opmode == "2")
		document.opmode.opMode[2].checked = true;
	else if (opmode == "3")
		document.opmode.opMode[3].checked = true;
	changeMode();
}

function msg()
{
	if(document.opmode.opMode[1].checked == true && <% isOnePortOnly(); %> ){
		alert("In order to access web page please \nchange or alias your IP address to 172.32.1.1");
	}
}
</script></head>

<body onLoad="initValue()">
<table width="90%" border="0" cellpadding="0" cellspacing="0" align="center">
	<tr>
    	<td style="padding-top:30px;">
	   		<table width="100%" border="0" cellpadding="0" cellspacing="0">
          		<tr>
            		<td class="nav">��ǰ·��:KN-WR922 &gt;&gt;ϵͳ���� &gt;&gt;ϵͳģʽ</td>
          		</tr>
		  		<tr>
            		<td>&nbsp;</td>
          		</tr>
				<tr>
            <td class="space" style="padding-left:10px; padding-right:10px;">&nbsp;&nbsp;&nbsp;�ṩ����ģʽ����ѡ���Ž�ģʽ��·��ģʽ��·��ģʽΪĬ�ϵĹ���ģʽ���������<input type="button" class="button5" value="����" onclick=popHelp('help.htm#opmode')> </td>
          </tr>
		  <tr>
		    <td>&nbsp;</td>
		  </tr>
				<tr>
            		<td class="titlebg">ϵͳģʽ</td>
          		</tr>
 <form method="post" name="opmode" action="/goform/setOpMode">
          		<tr>
				  <td><table class="space" width="100%" border="0" cellspacing="0" cellpadding="0" >
					  <tr>
						<td colspan="2" class="contentpadding">       
						<dl>
						  <dt><input type="radio" id="opMode" name="opMode" value="0" onClick="changeMode()"><b>�Ž�:</b></dt>
						  <dd>���е���̫���ӿں����߽ӿڽ�����һ���Žӿ�.</dd>
						  <dt><input type="radio" id="opMode" name="opMode" value="1" onClick="changeMode()"><b>·��ģʽ:</b></dt>
						  <dd>��һ����̫���ӿ�ΪWAN��. ��������̫���ӿں����߽ӿ���ΪLAN���Žӳ�һ���ӿ�.</dd>
						  <dt id="stadt"><input type="radio" id="opMode" name="opMode" value="2" onClick="changeMode()"><b>����ISP:</b></dt>
						  <dd id="stadd">���߽ӿ���ΪWAN��, ��̫���ӿ�Ϊlan ��.</dd>
						  <dt id="apclidt"><input type="radio" id="opMode" name="opMode" value="3" onClick="changeMode()"><b>AP Client:</b></dt>
						  <dd id="apclidd">The wireless apcli interface is treated as WAN port, and the wireless ap interface and the ethernet ports are LAN ports.</dd>
						</dl></td>
					  </tr>   
					  <tr id="nat">
						<td width="25%" class="contentpadding">NAT:</td>
						<td width="75%"><select id="natEnbl" name="natEnbl" size="1">
								  <option value="0">����</option>
								  <option value="1">����</option></select>
						(����ѡ��·��ģʽʱ���������ñ�����)</td>
					  </tr>
					  <tr id="eth_conv">
						 <td class="contentpadding">Ethernet Converter:</td>
						 <td><select id="ethConv" name="ethConv" size="1">
						<option value="0">Disable</option>
						<option value="1">Enable</option></select></td>
					  </tr>
					  <tr id="miiInic">
						<td class="contentpadding" id="oMiiMode">INIC Mii Mode:</td>
						<td><select id="miiMode" name="miiMode" size="1">
						 <option value="0" id="oMiiModeD">Disable</option>
						 <option value="1" id="oMiiModeE">Enable</option></select></td>
					  </tr>
				   </table></td>
			    </tr>
				<tr><td>&nbsp;</td></tr>
				<tr> 
				  <td><input type="hidden" value="/work_mode.asp" name="submit-url"><input type="button" class="button" value="�� ��" id="oApply" onClick="msg(); document.opmode.submit();">&nbsp;&nbsp;<input type="reset" class="button" value="ȡ ��" onClick="window.location.reload()"></td>
				</tr> 
				<tr><td>&nbsp;</td></tr>
 </form>
            </table>
		</td>
	</tr>
</table>
</body>
</html>
