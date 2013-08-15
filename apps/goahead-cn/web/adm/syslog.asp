<html>
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Expires" content="-1">
<meta http-equiv="Content-Type" content="text/html; charset=<% getCharset(); %>">
<link rel="stylesheet" href="../style/normal_ws.css" type="text/css">
<script language="javascript" src="../js/language_<% getCfgZero(1, "LanguageType"); %>.js"></script>
<script language="javascript" src="../js/common.js"></script>
<script language="javascript">
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
        alert(JS_msg5);
        return false;
    }
    http_request.onreadystatechange = alertContents;
    http_request.open('POST', url, true);
    http_request.send(content);
}

function alertContents() 
{
    if (http_request.readyState == 4) {
        if (http_request.status == 200) {
			uploadLogField(http_request.responseText);
        } else {
            //alert(JS_msg6);
        }
    }
}

function uploadLogField(str)
{
	if (str == "-1")
		document.getElementById("syslog").value = ""
	else
		document.getElementById("syslog").value = str;
}

function updateLog()
{
	makeRequest("/goform/syslog", "n/a", false);
}

function clearLogClick()
{
	document.getElementById("syslog").value = "";
	return true;
}

function refreshLogClick()
{
	updateLog();
	return true;
}
</script>
</head>
<body onLoad="updateLog()" class="mainbody">
<blockquote>
<table width=700><tr><td>
<table width=100% border=0 cellpadding=3 cellspacing=1> 
<tr><td class="title"><script>dw(MM_syslog)</script></td></tr>
<tr><td><script>dw(JS_msg_log)</script></td></tr>
<tr><td><hr></td></tr>
</table>

<form method="post" name="clearLog" action="/goform/clearlog">
<input type="hidden" name="submit-url" value="/adm/syslog.asp">
<table width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr>
    <td><textarea name="syslog" id="syslog" style="font-size:9pt;width:90%" rows="20" wrap="off" readonly="1"></textarea></td>
  </tr>
</table>

<br>
<table width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr>
    <td>
      <script>dw('<input type="submit" class=button value="'+BT_clear+'" onClick="clearLogClick();"> &nbsp; &nbsp;\
      <input type="button" class=button value="'+BT_refresh+'" onClick="refreshLogClick();">')</script>
    </td>
  </tr>
</table>
</form>

</td></tr></table>
</blockquote>
</body></html>
