<html>
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Expires" content="-1">
<meta http-equiv="Content-Type" content="text/html; charset=<% getCharset(); %>">
<link rel="stylesheet" href="../style/normal_ws.css" type="text/css">
<script language="javascript" src="../js/language_<% getCfgZero(1, "LanguageType"); %>.js"></script>
<script language="javascript" src="../js/common.js"></script>
<script language="javascript">
function Load_Setting()
{
	var baenable = <% getCfgZero(1, "staPolicy"); %>;
	var autoba = <% getCfgZero(1, "HT_AutoBA"); %>;
	var density = <% getCfgZero(1, "HT_MpduDensity"); %>;
	var amsdu = <% getCfgZero(1, "HT_AMSDU"); %>;

	if (baenable)
		document.sta_11n_configuration.a_mpdu_enable.checked = true;
	else
		document.sta_11n_configuration.a_mpdu_enable.checked = false;
	Mpdu_Aggregtion_Click();
	if (baenable) {
		if (autoba == 1) {
			document.sta_11n_configuration.autoBA[1].checked = true;
			Auto_BA_Click(1);
		} else {
			document.sta_11n_configuration.autoBA[0].checked = true;
			Auto_BA_Click(0);
			document.sta_11n_configuration.mpdu_density.options.selectedIndex = density;
		}
	}

	if (amsdu)
		document.sta_11n_configuration.a_msdu_enable.checked = true;
}

function open_org_add_page()
{
	window.open("ampdu_org_add.asp","ampdu_org_add","toolbar=no, location=yes, scrollbars=yes, resizable=no, width=660, height=600");
}

function open_org_del_page()
{
	cwin = window.open("station/ampdu_org_del.asp","ampdu_org_del","toolbar=no, location=yes, scrollbars=yes, resizable=no, width=660, height=600");
}

function getBssid()
{
	cwin.document.forms["sta_org_del"].selectedbssid.value = document.sta_11n_configuration.selectedbssid.value;
}

function open_rev_del_page()
{
	window.open("ampdu_rev_del.asp","ampdu_rev_del","toolbar=no, location=yes, scrollbars=yes, resizable=no, width=660, height=600");
}

function Mpdu_Aggregtion_Click()
{
	document.sta_11n_configuration.mpdu_density.disabled = true;
	document.sta_11n_configuration.autoBA[0].disabled = true;
	document.sta_11n_configuration.autoBA[1].disabled = true;

	if (document.sta_11n_configuration.a_mpdu_enable.checked == true) {
		document.sta_11n_configuration.mpdu_density.disabled = false;
		document.sta_11n_configuration.autoBA[0].disabled = false;
		document.sta_11n_configuration.autoBA[1].disabled = false;
	}
}

function Auto_BA_Click(autoba)
{
	document.sta_11n_configuration.mpdu_density.disabled = true;
	if (autoba == 0)
		document.sta_11n_configuration.mpdu_density.disabled = false;
}

function submit_apply()
{
	if (document.sta_11n_configuration.a_mpdu_enable.checked == false &&
	    document.sta_11n_configuration.a_msdu_enable.checked == false)
		document.sta_11n_configuration.action.value == "none";
	document.sta_11n_configuration.submit();
}

function selectedBSSID(tmp)
{
	document.sta_11n_configuration.selectedbssid.value = tmp;
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
<h2>Station 11n Configurations</h2>
<p>The Status page shows the settings and current operation status of the Station.</p>
<hr />
<form method=post name="sta_11n_configuration" action="/goform/setSta11nCfg">
<input type=hidden name="action" value="">
<table width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr>
    <td class="title" colspan="2">11n Configuration</td>
  </tr>
  <tr>
    <td class="thead" rowspan="2">MPDU Aggregation</td>
    <td><input type="checkbox" name="a_mpdu_enable" onClick="Mpdu_Aggregtion_Click()">Enable</td>
  </tr>
  <tr>
    <td><input type="radio" name="autoBA" value=0 checked onClick="Auto_BA_Click(0)">Manual<input type="radio" name="autoBA" value=1 onClick="Auto_BA_Click(1)">Auto</td>
  </tr>
  <tr>
    <td class="thead">MPDU density</td>
    <td><select name="mpdu_density">
        <option value=0 selected>0</option>
        <option value=1>1</option>
        <option value=2>2</option>
        <option value=3>3</option>
        <option value=4>4</option>
        <option value=5>5</option>
        <option value=6>6</option>
        <option value=7>7</option>
      </select></td>
  </tr>
  <tr>
    <td class="thead">Aggregation MSDU(A-MSDU)</td>
    <td><input type="checkbox" name="a_msdu_enable">Enable</td>
  </tr>
</table>
<table width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr>
    <td>
      <input type=submit class=button value="Apply" onClick="submit_apply()"> &nbsp; &nbsp;
      <input type=button class=button value="Reset" onClick="resetForm();">
    </td>
  </tr>
</table>
</form>

</td></tr></table>
</body></html>
