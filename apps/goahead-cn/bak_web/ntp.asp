<html>
<head>
<title>WR922</title>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312" />
<link href="images/kingnet.css" type="text/css" rel="stylesheet">
<script language="JavaScript" type="text/javascript">
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
			// refresh
			window.location.reload();
        } else {
            alert('There was a problem with the request.');
        }
    }
}
function atoi(str, num)
{
    i=1;
    if(num != 1 ){
        while (i != num && str.length != 0){
            if(str.charAt(0) == '.'){
                i++;
            }
            str = str.substring(1);
        }
        if(i != num )
            return -1;
    }

    for(i=0; i<str.length; i++){
        if(str.charAt(i) == '.'){
            str = str.substring(0, i);
            break;
        }
    }
    if(str.length == 0)
        return -1;
    return parseInt(str, 10);
}

function isAllNum(str)
{
	for (var i=0; i<str.length; i++){
	    if((str.charAt(i) >= '0' && str.charAt(i) <= '9') || (str.charAt(i) == '.' ))
			continue;
		return 0;
	}
	return 1;
}



function NTPFormCheck()
{    
//   if(document.NTP.NTPServerIP.value == ""){
//	    alert("请填入NTP服务器IP!");
//		return false;
//	}

	if(document.NTP.NTPServerIP.value != ""){
		if( document.NTP.NTPSync.value == ""){
			alert("请填入同步时间.");
			return false;
		}
	}
	if(isAllNum(document.NTP.NTPSync.value) == 0){
		alert("同步时间只能为数字");
		return false;
	}
	if(( atoi(document.NTP.NTPSync.value,1) > 300) || (atoi(document.NTP.NTPSync.value,1) < 1)){
		alert("同步时间为(1~300)");
		return false;
	}		
	return true;
}



function disableTextField (field)
{
  if(document.all || document.getElementById){
    field.disabled = true;
  }else {
    field.oldOnFocus = field.onfocus;
    field.onfocus = skip;
  }
}

function enableTextField (field)
{
  if(document.all || document.getElementById)
    field.disabled = false;
  else {
    field.onfocus = field.oldOnFocus;
  }
}



function initValue()
{
	var tz = "<% getCfgGeneral(1, "TZ"); %>";

	if (tz == "UCT_-11")
		document.NTP.time_zone.options.selectedIndex = 0;
	else if (tz == "UCT_-10")
		document.NTP.time_zone.options.selectedIndex = 1;
	else if (tz == "NAS_-09")
		document.NTP.time_zone.options.selectedIndex = 2;
	else if (tz == "PST_-08")
		document.NTP.time_zone.options.selectedIndex = 3;
	else if (tz == "MST_-07")
		document.NTP.time_zone.options.selectedIndex = 4;
	else if (tz == "MST_-07")
		document.NTP.time_zone.options.selectedIndex = 5;
	else if (tz == "CST_-06")
		document.NTP.time_zone.options.selectedIndex = 6;
	else if (tz == "UCT_-06")
		document.NTP.time_zone.options.selectedIndex = 7;
	else if (tz == "UCT_-05")
		document.NTP.time_zone.options.selectedIndex = 8;
	else if (tz == "EST_-05")
		document.NTP.time_zone.options.selectedIndex = 9;
	else if (tz == "AST_-04")
		document.NTP.time_zone.options.selectedIndex = 10;
	else if (tz == "UCT_-04")
		document.NTP.time_zone.options.selectedIndex = 11;
	else if (tz == "UCT_-03")
		document.NTP.time_zone.options.selectedIndex = 12;
	else if (tz == "EBS_-03")
		document.NTP.time_zone.options.selectedIndex = 13;
	else if (tz == "NOR_-02")
		document.NTP.time_zone.options.selectedIndex = 14;
	else if (tz == "EUT_-01")
		document.NTP.time_zone.options.selectedIndex = 15;
	else if (tz == "UCT_000")
		document.NTP.time_zone.options.selectedIndex = 16;
	else if (tz == "GMT_000")
		document.NTP.time_zone.options.selectedIndex = 17;
	else if (tz == "MET_001")
		document.NTP.time_zone.options.selectedIndex = 18;
	else if (tz == "MEZ_001")
		document.NTP.time_zone.options.selectedIndex = 19;
	else if (tz == "UCT_001")
		document.NTP.time_zone.options.selectedIndex = 20;
	else if (tz == "EET_002")
		document.NTP.time_zone.options.selectedIndex = 21;
	else if (tz == "SAS_002")
		document.NTP.time_zone.options.selectedIndex = 22;
	else if (tz == "IST_003")
		document.NTP.time_zone.options.selectedIndex = 23;
	else if (tz == "MSK_003")
		document.NTP.time_zone.options.selectedIndex = 24;
	else if (tz == "UCT_004")
		document.NTP.time_zone.options.selectedIndex = 25;
	else if (tz == "UCT_005")
		document.NTP.time_zone.options.selectedIndex = 26;
	else if (tz == "UCT_006")
		document.NTP.time_zone.options.selectedIndex = 27;
	else if (tz == "UCT_007")
		document.NTP.time_zone.options.selectedIndex = 28;
	else if (tz == "CST_008")
		document.NTP.time_zone.options.selectedIndex = 29;
	else if (tz == "CCT_008")
		document.NTP.time_zone.options.selectedIndex = 30;
	else if (tz == "SST_008")
		document.NTP.time_zone.options.selectedIndex = 31;
	else if (tz == "AWS_008")
		document.NTP.time_zone.options.selectedIndex = 32;
	else if (tz == "JST_009")
		document.NTP.time_zone.options.selectedIndex = 33;
	else if (tz == "KST_009")
		document.NTP.time_zone.options.selectedIndex = 34;
	else if (tz == "UCT_010")
		document.NTP.time_zone.options.selectedIndex = 35;
	else if (tz == "AES_010")
		document.NTP.time_zone.options.selectedIndex = 36;
	else if (tz == "UCT_011")
		document.NTP.time_zone.options.selectedIndex = 37;
	else if (tz == "UCT_012")
		document.NTP.time_zone.options.selectedIndex = 38;
	else if (tz == "NZS_012")
		document.NTP.time_zone.options.selectedIndex = 39;

}

function syncWithHost()
{
	var currentTime = new Date();

	var seconds = currentTime.getSeconds();
	var minutes = currentTime.getMinutes();
	var hours = currentTime.getHours();
	var month = currentTime.getMonth() + 1;
	var day = currentTime.getDate();
	var year = currentTime.getFullYear();

	var seconds_str = " ";
	var minutes_str = " ";
	var hours_str = " ";
	var month_str = " ";
	var day_str = " ";
	var year_str = " ";

	if(seconds < 10)
		seconds_str = "0" + seconds;
	else
		seconds_str = ""+seconds;

	if(minutes < 10)
		minutes_str = "0" + minutes;
	else
		minutes_str = ""+minutes;

	if(hours < 10)
		hours_str = "0" + hours;
	else
		hours_str = ""+hours;

	if(month < 10)
		month_str = "0" + month;
	else
		month_str = ""+month;

	if(day < 10)
		day_str = "0" + day;
	else
		day_str = day;

	var tmp = month_str + day_str + hours_str + minutes_str + year + " ";
	makeRequest("/goform/NTPSyncWithHost", tmp);
}

</script>
</head>
<body onLoad="initValue()">
<table width="90%" border="0" cellpadding="0" cellspacing="0" align="center">
  <tr>
    <td style="padding-top:30px;">
	   <table width="100%" border="0" cellpadding="0" cellspacing="0">
          <tr>
            <td class="nav">当前路径:KN-WR922 &gt;&gt;服务设置 &gt;&gt;NTP 设置</td>
          </tr>
		  <tr>
            <td>&nbsp;</td>
          </tr>
		  <tr>
            <td class="space" style="padding-left:10px; padding-right:10px;">&nbsp;&nbsp;&nbsp;本页面对路由器自己的时间进行设置，您可以点击"和本机同步"按钮让路由器时间和本台PC时间同步，也可以填入时间服务器从internet上进行时间更新。 </td>
          </tr>
		  <tr>
		    <td>&nbsp;</td>
		  </tr>
		  <tr>
            <td class="titlebg">NTP 设置 </td>
          </tr><form method="post" name="NTP" action="/goform/NTP">
		  <tr>
            <td>
			  <table class="space" width="100%" border="0" cellspacing="0" cellpadding="0">
				<tr>
				  <td width="25%" class="contentpadding" id="manNTPCurrentTime">当前时间:</td>
				  <td width="75%"><input size="28" name="ntpcurrenttime" value="<% getCurrentTimeASP(); %>" type="text" readonly="1">&nbsp;&nbsp;<input type="button" class="button3" value="和本机同步" id="manNTPSyncWithHost" name="manNTPSyncWithHost" onClick="syncWithHost()"></td>
				</tr>
                <tr>
                  <td width="25%" class="contentpadding">时区:</td>
                  <td width="75%"><select name="time_zone">
      <option value="UCT_-11">(GMT-11:00) Midway Island, Samoa</option>
      <option value="UCT_-10">(GMT-10:00) Hawaii</option>
      <option value="NAS_-09">(GMT-09:00) Alaska</option>
      <option value="PST_-08">(GMT-08:00) Pacific Time</option>
      <option value="MST_-07">(GMT-07:00) Mountain Time</option>
      <option value="MST_-07">(GMT-07:00) Arizona</option>
      <option value="CST_-06">(GMT-06:00) Central Time</option>
      <option value="UCT_-06">(GMT-06:00) Middle America</option>
      <option value="UCT_-05">(GMT-05:00) Indiana East, Colombia</option>
      <option value="EST_-05">(GMT-05:00) Eastern Time</option>
      <option value="AST_-04">(GMT-04:00) Atlantic Time, Brazil West</option>
      <option value="UCT_-04">(GMT-04:00) Bolivia, Venezuela</option>
      <option value="UCT_-03">(GMT-03:00) Guyana</option>
      <option value="EBS_-03">(GMT-03:00) Brazil East, Greenland</option>
      <option value="NOR_-02">(GMT-02:00) Mid-Atlantic</option>
      <option value="EUT_-01">(GMT-01:00) Azores Islands</option>
      <option value="UCT_000">(GMT) Gambia, Liberia, Morocco</option>
      <option value="GMT_000">(GMT) England</option>
      <option value="MET_001">(GMT+01:00) Czech Republic, N</option>
      <option value="MEZ_001">(GMT+01:00) Germany</option>
      <option value="UCT_001">(GMT+01:00) Tunisia</option>
      <option value="EET_002">(GMT+02:00) Greece, Ukraine, Turkey</option>
      <option value="SAS_002">(GMT+02:00) South Africa</option>
      <option value="IST_003">(GMT+03:00) Iraq, Jordan, Kuwait</option>
      <option value="MSK_003">(GMT+03:00) Moscow Winter Time</option>
      <option value="UCT_004">(GMT+04:00) Armenia</option>
      <option value="UCT_005">(GMT+05:00) Pakistan, Russia</option>
      <option value="UCT_006">(GMT+06:00) Bangladesh, Russia</option>
      <option value="UCT_007">(GMT+07:00) Thailand, Russia</option>
      <option value="CST_008">(GMT+08:00) 中国, 中国香港</option>
      <option value="CCT_008">(GMT+08:00) Taipei</option>
      <option value="SST_008">(GMT+08:00) Singapore</option>
      <option value="AWS_008">(GMT+08:00) Australia (WA)</option>
      <option value="JST_009">(GMT+09:00) Japan, Korea</option>
      <option value="KST_009">(GMT+09:00) Korean</option>
      <option value="UCT_010">(GMT+10:00) Guam, Russia</option>
      <option value="AES_010">(GMT+10:00) Australia (QLD, TAS,NSW,ACT,VIC)</option>
      <option value="UCT_011">(GMT+11:00) Solomon Islands</option>
      <option value="UCT_012">(GMT+12:00) Fiji</option>
      <option value="NZS_012">(GMT+12:00) New Zealand</option></select></td>
                </tr>
                <tr>
                  <td class="contentpadding">NTP 服务器:</td>
                  <td><input size="32" maxlength="17" name="NTPServerIP" value="<% getCfgGeneral(1, "NTPServerIP"); %>" type="text">&nbsp;&nbsp;<font color="#808080">例:&nbsp;time.nist.gov</font></td>
                </tr>
                <tr>
                  <td class="contentpadding">NTP 同步时间：</td>
                  <td><input size="4" name="NTPSync" value="<% getCfgGeneral(1, "NTPSync"); %>" type="text">
                  小时
                  (表示多少时间同步一次)</td>
                </tr>
              </table>
			</td>
          </tr>
		  <tr><td>&nbsp;</td></tr>
		  <tr> 
			<td colspan="2" class="contentpadding"><input type="hidden" value="/ntp.asp" name="submit-url"><input type=submit class=button value="提 交" onClick="return NTPFormCheck()">&nbsp;&nbsp;<input type=reset class=button  value="取 消" onClick="window.location.reload()"></td>
		  </tr>
		  <tr><td>&nbsp;</td></tr>
		  </form>
	  </table>
    </td>
  </tr>
  <tr><td>&nbsp;</td></tr>
</table>

</body>
</html>
