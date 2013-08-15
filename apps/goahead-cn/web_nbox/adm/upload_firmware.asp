<html>
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Expires" content="-1">
<meta http-equiv="Content-Type" content="text/html; charset=<% getCharset(); %>">
<link rel="stylesheet" href="../style/normal_ws.css" type="text/css">
<script language="javascript" src="../js/language_<% getCfgZero(1, "LanguageType"); %>.js"></script>
<script language="javascript" src="../js/common.js"></script>
<script language="javascript">
document.write('<div id="loading" style="display: none;"><br>'+JS_msg91+'<br></div>');
var storageb = '<% getStorageBuilt(); %>';
var isStorageBuilt = <% getStorageBuilt(); %>;
var firmware_path = "";
var secs;
var timerID = null;
var timerRunning = false;
var timeout = 3;
var delay = 1000;

function InitializeTimer()
{
	if(!isStorageBuilt)
		return;
    // Set the length of the timer, in seconds
    secs = timeout;
    StopTheClock();
    StartTheTimer();
}

function StopTheClock()
{
    if(timerRunning)
        clearTimeout(timerID);
		
    timerRunning = false;
}

function StartTheTimer()
{
	if (!isStorageBuilt)
		return;

    if (secs==0)
	{
        StopTheClock();
        timerHandler();
        secs = timeout;
        StartTheTimer();
    }
	else
	{
        self.status = secs;
        secs = secs - 1;
        timerRunning = true;
        timerID = self.setTimeout("StartTheTimer()", delay);
    }
}

function timerHandler()
{
	if (!isStorageBuilt)
		return;

	makeRequest("/goform/storageGetFirmwarePath", "n/a");
}

var http_request = false;
function makeRequest(url, content) 
{
	if (!isStorageBuilt)
		return;

    http_request = false;
    if (window.XMLHttpRequest) { // Mozilla, Safari,...
        http_request = new XMLHttpRequest();
        if (http_request.overrideMimeType) {
            http_request.overrideMimeType('text/xml');
        }
    } else if (window.ActiveXObject) { // IE
        try {
            http_request = new ActiveXObject("Msxml2.XMLHTTP");
        } catch (e) {
            try {
            http_request = new ActiveXObject("Microsoft.XMLHTTP");
            } catch (e) {}
        }
    }
    if (!http_request) {
        alert(JS_msg5);
        return false;
    }
    http_request.onreadystatechange = alertContents;
    http_request.open('GET', url, true);
    http_request.send(content);
}

function alertContents() 
{
	if (!isStorageBuilt)
		return;

	if (http_request.readyState == 4) {
		if (http_request.status == 200) {
			updateStorageStatus( http_request.responseText);
		} else {
			//alert(JS_msg6);
		}
	}
}

function updateStorageStatus(str)
{
	if (!isStorageBuilt)
		return;

	if (firmware_path == str)
		return;

	firmware_path = str;
	var paths = new Array();
	paths = firmware_path.split("\n");

	if(paths.length)
	{
		document.UploadFirmwareUSB.firmware_path.length = 0;
		for(var i=0; i<paths.length; i++)
		{
			var j = document.UploadFirmwareUSB.firmware_path.options.length;
			document.UploadFirmwareUSB.firmware_path.options[j] = new Option(paths[i].substring(12), paths[i], false, false);
		}
	}
}

var _singleton = 0;
function uploadFirmwareCheck()
{
	if (_singleton)
		return false;
		
	if (document.UploadFirmware.filename.value == "")
	{
		alert(JS_msg10);
		return false;
	}

	StopTheClock();

	//document.UploadFirmware.UploadFirmwareSubmit.disabled = true;
	//document.UploadFirmware.filename.disabled = true;
	document.ScanUSBFirmware.UploadFirmwareUSBScan.disabled = true;
	document.UploadFirmwareUSB.firmware_path.disabled = true;
	document.UploadFirmwareUSB.UploadFirmwareUSBSubmit.disabled = true;
	document.ForceMemUpgrade.ForceMemUpgradeSelect.disabled = true;
	document.ForceMemUpgrade.ForceMemUpgradeSubmit.disabled = true;

    document.getElementById("loading").style.display="block";
	_singleton = 1;
	return true;
}

function uploadFirmwareUSBCheck()
{
	if (_singleton)
		return false;
		
	if (!firmware_path.length)
	{
		alert(JS_msg95);
		return false;
	}
	
	StopTheClock();

	document.UploadFirmware.UploadFirmwareSubmit.disabled = true;
	//document.UploadFirmwareUSB.UploadFirmwareUSBSubmit.disabled = true;
	document.ForceMemUpgrade.ForceMemUpgradeSelect.disabled = true;
	document.ForceMemUpgrade.ForceMemUpgradeSubmit.disabled = true;

    document.getElementById("loading").style.display="block";
	_singleton = 1;
	return true;
}

function Load_Setting()
{
	if (isStorageBuilt)
	{		
		makeRequest("/goform/storageGetFirmwarePath", "n/a");
		InitializeTimer();
	}

	if (<% getCfgZero(1, "Force_mem_upgrade"); %>)
		document.ForceMemUpgrade.ForceMemUpgradeSelect.options.selectedIndex = 1;
	else
		document.ForceMemUpgrade.ForceMemUpgradeSelect.options.selectedIndex = 0;

    document.UploadFirmware.UploadFirmwareSubmit.disabled = false;
    if (storageb == "1")
	{
	    //document.getElementById("div_uploadFWUSBTable").style.display = "";
		//document.getElementById("div_uploadFWUSBTable_space").style.display = "";
		
	    //document.getElementById("div_memupgrade").style.display = "";
		//document.getElementById("div_memupgrade_submit").style.display = "";
	    //document.getElementById("div_memupgrade_space").style.display = "";

		document.getElementById("div_uploadFWUSBTable").style.display = "none";
		document.getElementById("div_uploadFWUSBTable_space").style.display = "none";
		
	    document.getElementById("div_memupgrade").style.display = "none";
		document.getElementById("div_memupgrade_submit").style.display = "none";		
	    document.getElementById("div_memupgrade_space").style.display = "none";
		
	    document.ForceMemUpgrade.ForceMemUpgradeSubmit.disabled = false;
    }
    else
	{
	    document.getElementById("div_uploadFWUSBTable").style.display = "none";
		document.getElementById("div_uploadFWUSBTable_space").style.display = "none";
		
	    document.getElementById("div_memupgrade").style.display = "none";
		document.getElementById("div_memupgrade_submit").style.display = "none";		
	    document.getElementById("div_memupgrade_space").style.display = "none";
		
	    document.ForceMemUpgrade.ForceMemUpgradeSubmit.disabled = true;
    }
	
//	document.UploadFirmware.filename.disabled = false;
	document.getElementById("loading").style.display="none";
}

function checkDate(str)
{
	var month = ["Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"];
	var week = [MM_sun, MM_mon, MM_tue, MM_wed, MM_thu, MM_fri, MM_sat];
	
	if ((str.substring(4,5)) == " ")
		str = str.replace(" ","");
	else
		str = str;
	
	var t = str.split(" ");	
	for (var j=0; j<12; j++)
	{
		if (t[0] == month[j]) 
			t[0] = j + 1;
	}
	
	return t[2] + "-" + t[0] + "-" + t[1];
}

function resetForm()
{
	location=location; 
}
</script>
</head>
<body onLoad="Load_Setting()">
<table width=700><tr><td>
<table width=100% border=0 cellpadding=3 cellspacing=1> 
<tr><td class="title"><script>dw(MM_firmware)</script></td></tr>
<tr style="display:none"><td><script>dw(JS_msg123)</script></td></tr>
<tr><td><hr></td></tr>
</table>

<form method="post" name="UploadFirmware" action="/cgi-bin/upload.cgi" enctype="multipart/form-data">
<table width=100% border=0 cellpadding=3 cellspacing=1> 
<tr>
  	<td class="thead"><script>dw(MM_cur_firmware)</script>:</td>
  	<td><% getSdkVersion(); %></td>
</tr>
<tr>
  	<td class="thead"><script>dw(MM_firmware_date)</script>:</td>
  	<td><script>dw(checkDate("<% getSysBuildTime(); %>"));</script></td>
</tr>
<tr>
    <td class="thead"><script>dw(MM_select_firmware_file)</script>:</td>
	<td><input type="file" name="filename" size="20" maxlength="256"></td>
</tr>
</table>

<br>
<table width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr>
    <td>
      <script>dw('<input type="submit" value="'+BT_upgrade+'" class=button name="UploadFirmwareSubmit" onClick="return uploadFirmwareCheck();"> &nbsp; &nbsp;\
      <input type=button class=button value="'+BT_reset+'" onClick="resetForm();">')</script>
    </td>
  </tr>
</table>
</form>

<form method="get" name="UploadFirmwareUSB" action="/cgi-bin/usb_upgrade.cgi">
<table id="div_uploadFWUSBTable_space" width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr>
    <tr><td><hr></td></tr>
  </tr>
</table>

<table id="div_uploadFWUSBTable" width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr>
    <td class="thead"><script>dw(MM_update_from_usb_disk)</script>:</td>
    <td><select name="firmware_path"></select></td>
  </tr>
  <tr>
 	<td colspan="2">&nbsp;</td>
  </tr>
  <tr>
    <td><script>dw('<input type="submit" value="'+BT_upgrade+'" class=button name="UploadFirmwareUSBSubmit" onClick="return uploadFirmwareUSBCheck();">')</script></td>
</form>
<form method="get" name="ScanUSBFirmware" action="/goform/ScanUSBFirmware">
    <td><script>dw('<input type="submit" value="'+BT_scan+'" class=button name="UploadFirmwareUSBScan">')</script></td>
</form>
  </tr>
</table>

<form method="post" name="ForceMemUpgrade" action="/goform/forceMemUpgrade">
<table id="div_memupgrade_space" width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr>
    <tr><td><hr></td></tr>
  </tr>
</table>

<table id="div_memupgrade" width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr>
	<td class="thead"><script>dw(MM_force_upgrade_via)</script>:</td>
	<td><select name="ForceMemUpgradeSelect">
        <option value="0"><script>dw(MM_no)</script></option>
        <option value="1"><script>dw(MM_yes)</script></option>
		</select></td>
  </tr>
</table>

<br>
<table id="div_memupgrade_submit" width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr>
    <td>
      <script>dw('<input type="submit" value="'+BT_upgrade+'" class=button name="ForceMemUpgradeSubmit"')</script>
    </td>
  </tr>
</table>
</form>

</td></tr></table>
</body></html>
