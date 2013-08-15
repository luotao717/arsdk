<html>
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Expires" content="-1">
<meta http-equiv="Content-Type" content="text/html; charset=<% getCharset(); %>">
<link rel="stylesheet" href="../style/normal_ws.css" type="text/css">
<script language="javascript" src="../js/language_<% getCfgZero(1, "LanguageType"); %>.js"></script>
<script language="javascript" src="../js/common.js"></script>
<script language="javascript">
document.write('<div id="loading" style="display: none;"><br><br><br>Uploading firmware <br><br> Please be patient and don\'t remove usb device if it presented...</div>');
var secs;
var timerID = null;
var timerRunning = false;
var timeout = 2;
var delay = 1000;

function InitializeTimer()
{
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
    if (secs==0){
        StopTheClock();
        timerHandler();
        secs = timeout;
        StartTheTimer();
    }
	else{
        self.status = secs;
        secs = secs - 1;
        timerRunning = true;
        timerID = self.setTimeout("StartTheTimer()", delay);
    }
}

function timerHandler()
{
	makeRequest("/goform/getAntenna", "n/a");
}

var http_request = false;
function makeRequest(url, content) 
{
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
        alert('Giving up :( Cannot create an XMLHTTP instance');
        return false;
    }
    http_request.onreadystatechange = alertContents;
    http_request.open('GET', url, true);
    http_request.send(content);
}

function alertContents() 
{
	if (http_request.readyState == 4) {
		if (http_request.status == 200) {
			updateAntennaStatus( http_request.responseText);
		} else {
			//alert('There was a problem with the request.');
		}
	}
}

function updateAntennaStatus(str)
{
	document.getElementById("UpdateAntenna").innerHTML = "Ant" + str;
}

function Load_Setting()
{
	makeRequest("/goform/getAntenna", "n/a");
	InitializeTimer();

	mode = "<% getCfgGeneral(1, "AntennaDiversity"); %>"
	if(mode == "Disable")
		document.AntennaDiversity.ADSelect.options.selectedIndex = 0;
	else if(mode == "Enable_Algorithm1")
		document.AntennaDiversity.ADSelect.options.selectedIndex = 1;
	else if(mode == "Antenna0")
		document.AntennaDiversity.ADSelect.options.selectedIndex = 2;
	else if(mode == "Antenna2")
		document.AntennaDiversity.ADSelect.options.selectedIndex = 3;
	else
		document.AntennaDiversity.ADSelect.options.selectedIndex = 0;
}

function resetForm()
{
	location=location; 
}
</script>
</head>
<body onLoad="Load_Setting()" class="mainbody">
<blockquote>
<table width=700><tr><td>
<table width=100% border=0 cellpadding=3 cellspacing=1> 
<tr><td class="title">Antenna Diversity</td></tr>
<tr><td>Configure the Antenna Diversity setting to increase the performance.</td></tr>
<tr><td><hr></td></tr>
</table>

<form method="post" name="AntennaDiversity" action="/goform/AntennaDiversity">
<table width=100% border=0 cellpadding=3 cellspacing=1> 
<tr>
	<td class="thead">Antenna Diversity Mode:</td>
	<td><select name="ADSelect">
        <option value="Disable">Disable</option>
        <option value="Enable_Algorithm1">Enable Algorithm1(BBP)</option>
        <option value="Antenna0">fixed at Ant0</option>
        <option value="Antenna2">fixed at Ant2</option>
    	</select></td>
</tr>
<tr>
	<td class="thead">Antenna:</td>
	<td id="UpdateAntenna">&nbsp;</td>
</tr>
</table>

<br>
<table width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr>
    <td>
      <input type=submit class=button value="Apply"> &nbsp; &nbsp;
      <input type=button class=button value="Reset" onClick="resetForm();">
    </td>
  </tr>
</table>
</form>

</td></tr></table>
</blockquote>
</body></html>
