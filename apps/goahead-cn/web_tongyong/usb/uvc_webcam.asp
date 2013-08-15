<html>
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Expires" content="-1">
<meta http-equiv="Content-Type" content="text/html; charset=<% getCharset(); %>">
<link rel="stylesheet" href="../style/normal_ws.css" type="text/css">
<script language="javascript" src="../js/language_<% getCfgZero(1, "LanguageType"); %>.js"></script>
<script language="javascript" src="../js/common.js"></script>
<script language="javascript">
function Load_Setting()
{
	var webcamebl = '<% getCfgZero(1, "WebCamEnabled"); %>';

	document.webcam.enabled[1].checked = true;
	document.webcam.resolution.disabled = true;
	document.webcam.fps.disabled = true;
	document.webcam.port.disabled = true;

	if (webcamebl == "1"){
		var resolution = '<% getCfgGeneral(1, "WebCamResolution"); %>';
		var fps = '<% getCfgGeneral(1, "WebCamFPS"); %>';
		var port = '<% getCfgGeneral(1, "WebCamPort"); %>';

		document.webcam.resolution.disabled = false;
		document.webcam.fps.disabled = false;
		document.webcam.port.disabled = false;

		document.webcam.enabled[0].checked = true;
		switch(resolution)	{
			case "160x120":
				document.webcam.resolution.options.selectedIndex = 0;
				break;
			case "320x240":
				document.webcam.resolution.options.selectedIndex = 1;
				break;
			case "640x480":
				document.webcam.resolution.options.selectedIndex = 2;
				break;
		}
		switch(fps){
			case "5":
				document.webcam.fps.options.selectedIndex = 0;
				break;
			case "10":
				document.webcam.fps.options.selectedIndex = 1;
				break;
			case "15":
				document.webcam.fps.options.selectedIndex = 2;
				break;
			case "20":
				document.webcam.fps.options.selectedIndex = 3;
				break;
			case "25":
				document.webcam.fps.options.selectedIndex = 4;
				break;
			case "30":
				document.webcam.fps.options.selectedIndex = 5;
				break;
		}
		document.webcam.port.value = port;
	}
}

function formCheck()
{
	if (document.webcam.port.value == ""){
		alert('Please specify Web Camera Port');
		document.webcam.port.focus();
		return false;
	}
	if (isOnlyNum( document.webcam.port.value ) == false){
		alert("Invalid port number, It should be the decimal number (1-65535).");
		document.webcam.port.focus();
		return false;
	}
	if (!checkPort(document.webcam.port.value)){
		alert("Invalid port number, It should be the decimal number (1-65535).");
		document.webcam.port.focus();
		return false;
	}
	return true;
}

function webcam_enable_switch()
{
	if (document.webcam.enabled[1].checked == true){
		document.webcam.resolution.disabled = true;
		document.webcam.fps.disabled = true;
		document.webcam.port.disabled = true;
	}
	else{
		document.webcam.resolution.disabled = false;
		document.webcam.fps.disabled = false;
		document.webcam.port.disabled = false;
	}
}

function resetForm()
{
	location=location; 
}
</script>
</head>
<body onLoad="Load_Setting()" class="mainbody">
<blockquote>
<table><tr><td>
<h1>Web Camera Setup </h1>
<p></p>
<hr />
<form method=post name=webcam action="/goform/webcamra">
<input type="hidden" name="submit-url" value="/usb/uvc_webcam.asp">
<table width="600" border="0" cellspacing="1" cellpadding="3">
  <tr> 
    <td class="title" colspan="2">Web Camera Setup</td>
  </tr>
  <tr> 
    <td class="thead">Capability</td>
    <td><input type="radio" name="enabled" value="1" onClick="webcam_enable_switch()">Enable<input type="radio" name="enabled" value="0" onClick="webcam_enable_switch()">Disable</td>
  </tr>
  <tr> 
    <td class="thead">Resolution</td>
    <td><select name="resolution">
        <option value="160x120">160x120</option>
        <option value="320x240">320x240</option>
        <option value="640x480" selected>640x480</option>
      </select></td>
  </tr>
  <tr>
    <td class="thead">Frames Per Second</td>
    <td><select name="fps">
        <option value="5">5</option>
        <option value="10">10</option>
        <option value="15">15</option>
        <option value="20">20</option>
        <option value="25" selected>25</option>
        <option value="30">30</option>
      </select></td>
  </tr>
  <tr>
    <td class="thead">Port</td>
    <td><input type="text" name="port" size="5" maxlength="5" value="8080"> (1-65535)</td>
  </tr>
</table>
<table width="600" border="0" cellpadding="2" cellspacing="1">
  <tr>
    <td>
      <input type=submit class="button" value="Apply" onClick="return formCheck()"> &nbsp; &nbsp;
      <input type=button class="button" value="Reset" onClick="resetForm();">
    </td>
  </tr>
</table>
</form>

</td></tr></table>
</blockquote>
</body></html>
