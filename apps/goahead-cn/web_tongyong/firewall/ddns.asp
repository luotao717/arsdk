<html>
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Expires" content="-1">
<meta http-equiv="Content-Type" content="text/html; charset=<% getCharset(); %>">
<link rel="stylesheet" href="../style/normal_ws.css" type="text/css">
<script language="javascript" src="../js/language_<% getCfgZero(1, "LanguageType"); %>.js"></script>
<script language="javascript" src="../js/common.js"></script>
<script language="javascript">
function formCheck()
{
	if (document.DDNS.DDNSEnabled.options.selectedIndex == 1) {
		if (!isBlankMsg(document.DDNS.Account.value, MM_username)) 
			return false;
		
		if (!isBlankMsg(document.DDNS.Password.value, MM_password)) 
			return false;
		
		if (!isBlankMsg(document.DDNS.DDNS.value, MM_domainname)) 
			return false;
	}
	
	return true;
}

function updateState()
{
	if (document.DDNS.DDNSEnabled.options.selectedIndex == 1){
		document.DDNS.DDNSProvider.disabled = false;
		document.DDNS.Account.disabled = false;
		document.DDNS.Password.disabled = false;
		document.DDNS.DDNS.disabled = false;
		document.getElementById("div_ddns_provider").style.display = "";
	}
	else{
		document.DDNS.DDNSProvider.disabled = true;
		document.DDNS.Account.disabled = true;
		document.DDNS.Password.disabled = true;
		document.DDNS.DDNS.disabled = true;
		document.getElementById("div_ddns_provider").style.display = "none";
	}	
}

function updateProvider()
{
	if (document.DDNS.DDNSProvider.value == "dyndns.org")
		document.getElementById("div_ddns_provider").innerHTML="<a href='http://dyn.com/dns/' target='_blank'>"+MM_register+"</a>";
	else if (document.DDNS.DDNSProvider.value == "no-ip.com")
		document.getElementById("div_ddns_provider").innerHTML="<a href='http://www.no-ip.com/newUser.php/' target='_blank'>"+MM_register+"</a>";
	else if (document.DDNS.DDNSProvider.value == "3322.org")
		document.getElementById("div_ddns_provider").innerHTML="<a href='http://www.pubyun.com/accounts/signup/' target='_blank'>"+MM_register+"</a>";
	else
		document.getElementById("div_ddns_provider").innerHTML="";
}

function Load_Setting()
{
	var ddns_enable = 1*"<% getCfgGeneral(1, "DDNSEnabled"); %>";
	var ddns_provider = "<% getCfgGeneral(1, "DDNSProvider"); %>";
	
	document.DDNS.DDNSEnabled.options.selectedIndex = ddns_enable;
	updateState();
	
	document.DDNS.DDNSProvider.value = ddns_provider;
	updateProvider();
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
<tr><td class="title"><script>dw(MM_ddns_settings)</script></td></tr>
<tr><td><script>dw(JS_msg_ddns)</script></td></tr>
<tr><td><hr></td></tr>
</table>

<form method="post" name="DDNS" action="/goform/DDNS">
<input type="hidden" name="submit-url" value="/firewall/ddns.asp">
<table width=100% border=0 cellpadding=3 cellspacing=1> 
<tr>
  <td class="thead">DDNS:</td>
  <td><select onChange="updateState()" name="DDNSEnabled">
	<option value=0><script>dw(MM_disable)</script></option>
    <option value=1><script>dw(MM_enable)</script></option></select></td>
</tr>
<tr>
  <td class="thead"><script>dw(MM_cur_status)</script>:</td>
  <td><script>dw(<% getIndex(1, "ddnsStatus"); %>)</script></td>
</tr>
<tr>
  <td class="thead"><script>dw(MM_provider)</script>:</td>
  <td><select onChange="updateProvider()" name="DDNSProvider">
  		<% getDdnsCombox(); %>
    </select>&nbsp;&nbsp;&nbsp;&nbsp;<span id="div_ddns_provider"> </span></td>
</tr>
<tr>
  <td class="thead"><script>dw(MM_domainname)</script>:</td>
  <td><input type="text" name="DDNS" value="<% getCfgGeneral(1, "DDNS"); %>"> </td>
</tr>
<tr>
  <td class="thead"><script>dw(MM_username)</script>:</td>
  <td><input type="text" name="Account" value="<% getCfgGeneral(1, "DDNSAccount"); %>"> </td>
</tr>
<tr>
  <td class="thead"><script>dw(MM_password)</script>:</td>
  <td><input type="text" name="Password" value="<% getCfgGeneral(1, "DDNSPassword"); %>"> </td>
</tr>
</table>

<br>
<table width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr>
    <td>
      <script>dw('<input type=submit class=button value="'+BT_apply+'" onClick="return formCheck()"> &nbsp; &nbsp;\
      <input type=button class=button value="'+BT_reset+'" onClick="resetForm();">')</script>
    </td>
  </tr>
</table>
</form>

</td></tr></table>
</blockquote>
</body></html>
