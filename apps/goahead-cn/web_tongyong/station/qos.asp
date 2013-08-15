<html>
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Expires" content="-1">
<meta http-equiv="Content-Type" content="text/html; charset=<% getCharset(); %>">
<link rel="stylesheet" href="../style/normal_ws.css" type="text/css">
<script language="javascript" src="../js/language_<% getCfgZero(1, "LanguageType"); %>.js"></script>
<script language="javascript" src="../js/common.js"></script>
<script language="javascript">
function WMM_Click()
{
	document.sta_qos.wmm_ps_enable.disabled = false;
	document.sta_qos.wmm_dls_enable.disabled = false;

	if (document.sta_qos.wmm_enable.checked == false)
	{
		document.sta_qos.wmm_ps_enable.disabled = true;
		document.sta_qos.wmm_dls_enable.disabled = true;
		document.sta_qos.wmm_ps_enable.checked = false;
		document.sta_qos.wmm_dls_enable.checked = false;
	}
	WMM_PS_Click();
	WMM_DLS_Click();
}

function WMM_PS_Click()
{
	document.sta_qos.wmm_ps_mode_acbe.disabled = true;
	document.sta_qos.wmm_ps_mode_acbk.disabled = true;
	document.sta_qos.wmm_ps_mode_acvi.disabled = true;
	document.sta_qos.wmm_ps_mode_acvo.disabled = true;
	
	if (document.sta_qos.wmm_ps_enable.checked == true)
	{
		document.sta_qos.wmm_ps_mode_acbe.disabled = false;
		document.sta_qos.wmm_ps_mode_acbk.disabled = false;
		document.sta_qos.wmm_ps_mode_acvi.disabled = false;
		document.sta_qos.wmm_ps_mode_acvo.disabled = false;
	}
}

function WMM_DLS_Click()
{
	document.sta_qos.mac0.disabled = false;
	document.sta_qos.mac1.disabled = false;
	document.sta_qos.mac2.disabled = false;
	document.sta_qos.mac3.disabled = false;
	document.sta_qos.mac4.disabled = false;
	document.sta_qos.mac5.disabled = false;
	document.sta_qos.timeout.disabled = false;

	if (document.sta_qos.wmm_dls_enable.checked == false)
	{
		document.sta_qos.mac0.disabled = true;
		document.sta_qos.mac1.disabled = true;
		document.sta_qos.mac2.disabled = true;
		document.sta_qos.mac3.disabled = true;
		document.sta_qos.mac4.disabled = true;
		document.sta_qos.mac5.disabled = true;
		document.sta_qos.timeout.disabled = true;
	}
}

function submit_apply(btntype)
{
	document.sta_qos.button_type.value = btntype;  // 1: wmm , 2: dls setup, 3: tear down
	if (btntype == 2 && document.sta_qos.wmm_dls_enable.checked == true)
	{
		if (document.sta_qos.timeout.value < 1 || document.sta_qos.timeout.value > 65535)
		{
			alert("Range of DLS Timeout is 1 ~ 65535");
			return;
		}
	}
	document.sta_qos.submit();
}

function Move_To_Dls()
{

}

function Load_Setting()
{
	var wmm = "<% getCfgZero(1, "WmmCapable"); %>";

	if (wmm == "1") {
		document.sta_qos.wmm_enable.checked = true;
		var ps = "<% getCfgZero(1, "APSDCapable"); %>";
		if (ps == "1") {
			document.sta_qos.wmm_ps_enable.checked = true;
			var AckPolicy = "<% getCfgGeneral(1, "AckPolicy", 0); %>";
			var ack = AckPolicy.split(";");
			if (ack[0] == "1")
				document.sta_qos.wmm_ps_mode_acbe.checked = true;
			if (ack[1] == "1")
				document.sta_qos.wmm_ps_mode_acbk.checked = true;
			if (ack[2] == "1")
				document.sta_qos.wmm_ps_mode_acvi.checked = true;
			if (ack[3] == "1")
				document.sta_qos.wmm_ps_mode_acvo.checked = true;

		} else {
			WMM_PS_Click();
		}
		var dls = "<% getCfgZero(1, "DLSCapable"); %>";
		if (dls == "1")
			document.sta_qos.wmm_dls_enable.checked = true;
		else
			WMM_DLS_Click();
	} else {
		WMM_Click();
	}
}
</script>
</head>
<body onLoad="Load_Setting()" class="mainbody">
<blockquote>
<table width=700><tr><td>
<h2>Station Advanced Configurations</h2>
<p>The Status page shows the settings and current operation status of the Station.</p>
<hr />
<form method="post" name="sta_qos" action="/goform/setStaQoS">
<table width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr>
    <td class="title" colspan="2">Qos Configuration</td>
  </tr>
  <tr>
    <td class="thead">WMM</td>
    <td><input type=checkbox name=wmm_enable OnClick="WMM_Click()">Enable</td>
  </tr>
  <tr>
    <td class="thead">WMM Power Saving</td>
    <td><input type=checkbox name=wmm_ps_enable onClick="WMM_PS_Click()">Enable</td>
  </tr>
  <tr>
    <td class="thead">PS Mode</td>
    <td><input type=checkbox name=wmm_ps_mode_acbe>AC_BE &nbsp;&nbsp;
      <input type=checkbox name=wmm_ps_mode_acbk>AC_BK &nbsp;&nbsp;
      <input type=checkbox name=wmm_ps_mode_acvi>AC_VI &nbsp;&nbsp;
      <input type=checkbox name=wmm_ps_mode_acvo>AC_VO</td>
  </tr>
</table>

<table width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr align="center">
    <td >
      <input type="button" name="WMMButton" class=button value="WMM Apply" onClick="submit_apply(1)"> &nbsp; &nbsp;
    </td>
  </tr>
</table>
<br />

<table div="wmm_dls_settings" width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr>
    <td class="title" colspan="2">Direct Link Setup</td>
  </tr>
  <tr>
    <td class="thead">Direct Link Setup</td>
    <td><input type=checkbox name=wmm_dls_enable onClick="WMM_DLS_Click()">Enable</td>
  </tr>
  <tr>
    <td class="thead">MAC Address</td>
    <td><input type=text size=2 maxlength=2 name=mac0>&nbsp;-&nbsp;
      <input type=text size=2 maxlength=2 name=mac1>&nbsp;-&nbsp;
      <input type=text size=2 maxlength=2 name=mac2>&nbsp;-&nbsp;
      <input type=text size=2 maxlength=2 name=mac3>&nbsp;-&nbsp;
      <input type=text size=2 maxlength=2 name=mac4>&nbsp;-&nbsp;
      <input type=text size=2 maxlength=2 name=mac5></td>
  </tr>
  <tr>
    <td class="thead">Timeout Value</td>
    <td><input type=text name=timeout align="right" id="qosSecond"> sec</td>
  </tr>
</table>

<table width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr align="center">
    <td >
      <input type="button" name="DlsSetupButton" class=button value="DLS Apply" onClick="submit_apply(2)"> &nbsp; &nbsp;
    </td>
  </tr>
</table>
<br />

<table div="wmm_dls_status" width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr>
    <td class="title" colspan="2">DLS Status</td>
  </tr>
  <tr>
    <td class="thead">MAC Address</td>
    <td>Timeout</td>
  </tr>
  <% getStaDLSList(); %>
</table>
<table width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr align="center">
    <td >
<!--  <input type="button" name="MoveToDlsButton" class=button value="Move To Dls Setup" onClick="Move_To_Dls()"> &nbsp; &nbsp;-->
      <input type="button" name="DlsStatusButton" class=button value="Tear Down" onClick="submit_apply(3)"> &nbsp; &nbsp;
    </td>
  </tr>
</table>

<input type=hidden name=button_type value="">
</form>

</td></tr></table>
</body></html>
