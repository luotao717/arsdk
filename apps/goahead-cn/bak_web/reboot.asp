<html>
<head>
<title>WR922</title>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312" />
<link href="images/kingnet.css" type="text/css" rel="stylesheet">
<script language="JavaScript" type="text/javascript">
function refresh_all()
{	
   top.location.href = "http://192.168.1.88/home.asp"
} 
function update()
{ 
    self.setTimeout("refresh_all()", 35000);
	return true;
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
	document.ExportSettings.synctime.value = tmp;
}
</script>
</head>
<body>

<table width="90%" border="0" cellpadding="0" cellspacing="0" align="center">
  <tr>
    <td style="padding-top:30px;">
	  <table width="100%" border="0" cellpadding="0" cellspacing="0">
        <tr>
          <td class="nav">当前路径:KN-WR922 &gt;&gt;系统管理 &gt;&gt;重启</td>
        </tr>
		<tr>
		    <td>&nbsp;</td>
		  </tr>
          <tr>
            <td class="space" style="padding-left:10px; padding-right:10px;">&nbsp;&nbsp;&nbsp;当您改变某些配置时，为了使您的配置生效，可能需要重新启动系统。注意：启动时间不超过一分钟，请耐心等候。</td>
          </tr>
        <tr>
          <td>&nbsp;</td>
      	</tr>
		<tr>
		  <td><form method="post" name="ExportSettings" action="/goform/re_boot" onSubmit="syncWithHost()">
			<table class="space" width="100%" border="0" cellspacing="0" cellpadding="0" >
			  <tr>
				<td class="titlebg" colspan="2">重启系统 </td>
			  </tr>
			  <tr>
				<td colspan="2" height="10"></td>
			  </tr>
			  <tr>
				<td width="25%" class="contentpadding">重启请按:</td>
				<td width="75%"><input type="hidden" value="" name="synctime"><input class="button" value="重 启" name="Export" type="submit" ></td>
			  </tr>
			  <tr>
				<td colspan="2" height="10"></td>
			  </tr>
			</table>
		  </form></td>
		</tr>
	  </table>
	</td>
  </tr>
</table>
</body>
</html>
