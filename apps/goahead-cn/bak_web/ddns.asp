<html>
<head>
<title>WR922</title>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312" />
<link href="images/kingnet.css" type="text/css" rel="stylesheet">
<script language="JavaScript" type="text/javascript">

function AdmFormCheck()
{
	if (document.Adm.admuser.value == "") {
		alert("�������ʺ�!");
		return false;
	}
	if (document.Adm.admpass.value == "") {
		alert("����������!");
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
		alert("�������˺ţ����룬����.");
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
            <td class="nav">��ǰ·��:KN-WR922 &gt;&gt;�������� &gt;&gt;DDNS</td>
          </tr>
		  <tr>
		    <td>&nbsp;</td>
		  </tr>
          <tr>
            <td class="space" style="padding-left:10px; padding-right:10px;">&nbsp;&nbsp;&nbsp;DDNS����̬��������.�ǰ�������·����WAN�ڵ�IP��ַ��̬���µ����������ϵĹ��ܣ���Ȼ����Ҫ��DDNS�����ṩ�����������ʺź���Ӧ������</td>
          </tr>
		  <tr>
            <td>&nbsp;</td>
          </tr>
          <tr>
            <td class="titlebg">DDNS ״̬</td>
          </tr>
          <tr>
            <td>
			  <table class="space" width="100%" id="div_11n_plugfest" name="div_11n_plugfest" border="0" cellspacing="0" cellpadding="0">
                <tr>
                  <td width="25%" class="contentpadding">DDNS ״̬:</td>
                  <td width="75%"><% getCfgGeneral(1, "DDNSStatus"); %></td>
                </tr>
              </table>
			</td>
          </tr>
          <tr>
            <td class="titlebg" colspan="2">DDNS ���� </td>
          </tr>
		  <tr>
		    <td><table class="space" width="100%" id="div_11n_plugfest" name="div_11n_plugfest" border="0" cellspacing="0" cellpadding="0">
              <tr>
                <td class="contentpadding">����DDNS����:</td>
                <td><input type="checkbox" name="DDNSEnabled" value="ON" <% if (getIndex("DDNSEnabled")) write("checked"); %> onClick="DDNSupdateState()" /></td>
              </tr>
              <tr>
                <td width="25%" class="contentpadding">DDNS ������:</td>
                <td width="75%">
            <select onChange="DDNSupdateState()" name="DDNSProvider">
			<option value="dyndns">dyndns </option>
			<option value="qdns">3322</option>
			<option value="88ip">88ip </option></select>
            ������Ҫѡ������DDNS����i�ṩ�̣�</td>
              </tr>
		      <tr>
                 <td width="25%" class="contentpadding">�˺�:</td>
                 <td width="75%"><input size="16" name="Account" value="<% getCfgGeneral(1, "DDNSAccount"); %>" type="text">
                 �����ڷ���������������ʺš����������ܺ���</td>
              </tr>
              <tr>
                 <td class="contentpadding">����:</td>
                 <td><input size="16" name="Password" value="<% getCfgGeneral(1, "DDNSPassword"); %>" type="password"> 
                 ���ʺ����Ӧ�����롣���������ܺ���</td>
              </tr>
			  <tr>
                  <td width="25%" class="contentpadding">DDNS:</td>
                  <td width="75%"><input size="32" name="DDNS" value="<% getCfgGeneral(1, "DDNS"); %>" type="text"> 
                  ��һ��Ϊ������������������������ܺ���</td>
              </tr>
            </table></td>
          </tr> 
          <tr><td>&nbsp;</td></tr>
		  <tr> 
            <td class="contentpadding"><input type="hidden" value="/ddns.asp" name="submit-url"><input type=submit class=button  value="�� ��" onClick="return DDNSFormCheck()">&nbsp;&nbsp;<input type=reset class=button value="ȡ ��" onClick="window.location.reload()"></td>
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
