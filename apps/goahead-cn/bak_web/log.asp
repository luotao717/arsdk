<html>
<head>
<title>WR922</title>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312" />
<link href="images/kingnet.css" type="text/css" rel="stylesheet">
<script type="text/javascript" src="common.js"> </script>
<script language="JavaScript" type="text/javascript">

var secs
var timerID = null
var timerRunning = false
var timeout = 4
var delay = 1000


function InitializeTimer()
{
    // Set the length of the timer, in seconds
//  timeout = default
    secs = timeout
    StopTheClock()
    StartTheTimer()
}

function StopTheClock()
{
    if(timerRunning)
        clearTimeout(timerID)
    timerRunning = false
}

function StartTheTimer()
{
    if (secs==0)
    {
        StopTheClock()

		updateLog();

        secs = timeout
        StartTheTimer()
    }else{
        self.status = secs
        secs = secs - 1
        timerRunning = true
        timerID = self.setTimeout("StartTheTimer()", delay)
    }
}

var http_request = false;
function makeRequest(url, content) {
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
    http_request.open('POST', url, true);
    http_request.send(content);
}

function alertContents() {
    if (http_request.readyState == 4) {
        if (http_request.status == 200) {
			uploadLogField(http_request.responseText);
        } else {
            alert('回复产生错误.');
        }
    }
}

function uploadLogField(str)
{
	document.getElementById("syslog").value = str;
}

function updateLog()
{
	makeRequest("/goform/syslog", "n/a", false);
}


function pageInit()
{
	updateLog();
	InitializeTimer();
}

function clearlogclick()
{
	document.getElementById("syslog").value = "";
	return true;
}

</script>


</head>
<body onLoad="pageInit()">
<table width="90%" border="0" cellpadding="0" cellspacing="0" align="center">
  <tr>
    <td style="padding-top:30px;">
	   <table width="100%" border="0" cellpadding="0" cellspacing="0">
          <tr>
            <td class="nav">当前路径:KN-WR922 &gt;&gt;系统管理 &gt;&gt;系统日志</td>
          </tr>
          <tr>
            <td>&nbsp;</td>
          </tr>
		  <tr>
            <td class="space" style="padding-left:10px; padding-right:10px;">&nbsp;&nbsp;&nbsp;本页面是用来显示系统运行的一些信息，主要用来帮助排查系统故障。 </td>
          </tr>
		  <tr>
		    <td>&nbsp;</td>
		  </tr>
		  <tr>
		    <td><table class="space" border="0" cellpadding="2" cellspacing="1" width="100%">
				<tr>
				  <td class="titlebg">系统日志</td>
				</tr>
				<tr>
				  <td class="contentpadding"><textarea name="syslog" id="syslog" cols="80" rows="20" wrap="off" readonly="1"></textarea></td>
				</tr>
				
			 </table></td>
		  </tr>
		  <tr><td>&nbsp;</td></tr>
		<form method="post" name="SubmitClearLog" action="/goform/clearlog">
		  <tr>
			<td class="contentpadding"><input type="submit" class="button"  value="清 除" name="clearlog" align="left" onClick="clearlogclick();"></td>
		  </tr>
		</form>
		  <tr><td>&nbsp;</td></tr>
	   </table>
	 </td>
  </tr>
  <tr><td>&nbsp;</td></tr>
</table>
</body>
</html>
