<html>
<head>
<title>WR922</title>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312" />
<link href="images/kingnet.css" type="text/css" rel="stylesheet">
<script language="JavaScript" type="text/javascript">

function AdmFormCheck()
{
	if (document.Adm.admuser.value == "") {
		alert("请填入帐号!");
		return false;
	}
	if (document.Adm.admpass.value == "") {
		alert("请填入密码!");
		return false;
	}
	return true;
}
function DDNSFormCheck()
{
	if(  document.DDNS.DDNSProvider.value != "none" && 
		(document.DDNS.Account.value == "" ||
		 document.DDNS.Password.value == "" ||
		 document.DDNS.DDNS.value == "")){
		alert("请填入账号，密码，域名.");
		return false;
	}

	return true;
}

function enableTextField (field)
{
  if(document.all || document.getElementById)
    field.disabled = false;
  else {
    field.onfocus = field.oldOnFocus;
  }
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

function DDNSupdateState()
{
	//alert("123");
	if(document.DDNS.DDNSEnabled.checked){
		enableTextField(document.DDNS.Account);
		enableTextField(document.DDNS.Password);
		enableTextField(document.DDNS.DDNS);
	}else{
		disableTextField(document.DDNS.Account);
		disableTextField(document.DDNS.Password);
		disableTextField(document.DDNS.DDNS);
	}
}
function initValue()
{
	var ddns_provider = "<% getCfgGeneral(1, "DDNSProvider"); %>";
    if (ddns_provider == "dyndns")
		document.DDNS.DDNSProvider.options.selectedIndex = 0;
	else if (ddns_provider == "qdns")
		document.DDNS.DDNSProvider.options.selectedIndex = 1;
		else if (ddns_provider == "88ip")
		document.DDNS.DDNSProvider.options.selectedIndex = 2;
	DDNSupdateState();
}		

</script>
</head>
<body onLoad="initValue()">
<form method="post" name="DDNS" action="/goform/DDNS">
<table width="90%" border="0" cellpadding="0" cellspacing="0" align="center">
  <tr>
    <td style="padding-top:30px;">
	   <table width="100%" border="0" cellpadding="0" cellspacing="0">
          <tr>
            <td class="nav">当前路径:KN-WR922 &gt;&gt;服务设置 &gt;&gt;DDNS</td>
          </tr>
		  <tr>
		    <td>&nbsp;</td>
		  </tr>
          <tr>
            <td class="space" style="padding-left:10px; padding-right:10px;">&nbsp;&nbsp;&nbsp;DDNS（动态域名服务）.是帮助您把路由器WAN口的IP地址动态更新到您的域名上的功能，当然您需要到DDNS服务提供商那里申请帐号和相应域名。</td>
          </tr>
		  <tr>
            <td>&nbsp;</td>
          </tr>
          <tr>
            <td class="titlebg">DDNS 状态</td>
          </tr>
          <tr>
            <td>
			  <table class="space" width="100%" id="div_11n_plugfest" name="div_11n_plugfest" border="0" cellspacing="0" cellpadding="0">
                <tr>
                  <td width="25%" class="contentpadding">DDNS 状态:</td>
                  <td width="75%"><% getCfgGeneral(1, "DDNSStatus"); %></td>
                </tr>
              </table>
			</td>
          </tr>
          <tr>
            <td class="titlebg" colspan="2">DDNS 设置 </td>
          </tr>
		  <tr>
		    <td><table class="space" width="100%" id="div_11n_plugfest" name="div_11n_plugfest" border="0" cellspacing="0" cellpadding="0">
              <tr>
                <td class="contentpadding">启动DDNS服务:</td>
                <td><input type="checkbox" name="DDNSEnabled" value="ON" <% if (getIndex("DDNSEnabled")) write("checked"); %> onClick="DDNSupdateState()" /></td>
              </tr>
              <tr>
                <td width="25%" class="contentpadding">DDNS 服务商:</td>
                <td width="75%">
            <select onChange="DDNSupdateState()" name="DDNSProvider">
			<option value="dyndns">dyndns </option>
			<option value="qdns">3322</option>
			<option value="88ip">88ip </option></select>
            （您需要选择您的DDNS服务i提供商）</td>
              </tr>
		      <tr>
                 <td width="25%" class="contentpadding">账号:</td>
                 <td width="75%"><input size="16" name="Account" value="<% getCfgGeneral(1, "DDNSAccount"); %>" type="text">
                 （您在服务商那里申请的帐号。启动本功能后必填）</td>
              </tr>
              <tr>
                 <td class="contentpadding">密码:</td>
                 <td><input size="16" name="Password" value="<% getCfgGeneral(1, "DDNSPassword"); %>" type="password"> 
                 （帐号相对应的密码。启动本功能后必填）</td>
              </tr>
			  <tr>
                  <td width="25%" class="contentpadding">DDNS:</td>
                  <td width="75%"><input size="32" name="DDNS" value="<% getCfgGeneral(1, "DDNS"); %>" type="text"> 
                  （一般为您所申请的域名，启动本功能后必填）</td>
              </tr>
            </table></td>
          </tr> 
          <tr><td>&nbsp;</td></tr>
		  <tr> 
            <td class="contentpadding"><input type="hidden" value="/ddns.asp" name="submit-url"><input type=submit class=button  value="提 交" onClick="return DDNSFormCheck()">&nbsp;&nbsp;<input type=reset class=button value="取 消" onClick="window.location.reload()"></td>
          </tr>
		  <tr><td>&nbsp;</td></tr>
       </table>
	</td>
  </tr>
  <tr><td>&nbsp;</td></tr>
</table>
</form>
</body>
</html>
