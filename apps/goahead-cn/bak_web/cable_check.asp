<html>
<head>
<title>WR922</title>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312" />
<meta http-equiv=refresh content="3">
<link href="images/kingnet.css" type="text/css" rel="stylesheet">
<script language="JavaScript" type="text/javascript">
function showPortStatus()
{
	var str = "<% getPortStatus(); %>";
	var all = new Array();

	if(str == "-1"){
		document.write("not support");
		return ;
	}

	all = str.split(",");
	for(i=0; i< all.length-1; i+=3){
		document.write("<td>");
		if(all[i] == "1"){
			if(all[i+1] == "10")
				document.write("<img src=/images/10.gif> ");
			else if(all[i+1] == "100")
				document.write("<img src=/images/100.gif> ");

//			if(all[i+2] == "F")
//				document.write("Full ");
//			else(all[i+2] == "H")
//				document.write("Half ");
		}else if(all[i] == "0"){
				document.write("<img src=/images/empty.gif> ");
		}
		document.write("</td>");
	}
}
</script>
</head>
<body>
<table width="90%" border="0" cellpadding="0" cellspacing="0" align="center">
  <tr>
    <td style="padding-top:30px;">
	   <table width="100%" border="0" cellpadding="0" cellspacing="0">
          <tr>
            <td class="nav">当前路径:KN-WR922 &gt;&gt;系统状态 &gt;&gt;线路检测</td>
          </tr>
		  <tr>
		    <td>&nbsp;</td>
		  </tr>
          <tr>
            <td class="space" style="padding-left:10px; padding-right:10px;">&nbsp;&nbsp;&nbsp;页面是检测机器物理线路接线状态。&nbsp;&nbsp;<img src="images/100.gif">&nbsp;代表100M接通状态；&nbsp;&nbsp;<img src="images/10.gif">&nbsp;代表10M接通状态；&nbsp;&nbsp;<img src="images/empty.gif">&nbsp;代表未接通状态。</td>
          </tr>
          <tr>
            <td>&nbsp;</td>
          </tr>
		  <tr>
            <td colspan="8" class="titlebg">以太网端口状态</td>
          </tr>
		  <tr>
		    <td>
              <table class="space" width=100% border="0" cellpadding="0" cellspacing="0">
               <tr>
                 <td colspan="5" height="10"></td>
               </tr>
			   <tr align="center"><script type="text/javascript">showPortStatus();</script></tr>
			   <tr align="center">
			     <td>WAN</td>
				 <td>LAN2</td>
				 <td>LAN1</td>
			   </tr>	  
	          </table>
			</td>
		  </tr>	  
		</table>
	 </td>
  </tr>
  <tr><td>&nbsp;</td></tr>
</table>
</body>
</html>
