<html>
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Expires" content="-1">
<meta http-equiv="Content-Type" content="text/html; charset=<% getCharset(); %>">
<link rel="stylesheet" href="../style/normal_ws.css" type="text/css">
<script language="javascript" src="../js/language_<% getCfgZero(1, "LanguageType"); %>.js"></script>
<script language="javascript" src="../js/common.js"></script>
<script language="javascript">
var rules_num = 0;
var entries = new Array();
var all_str = "<% getCfgGeneral(1, "websURLFilters"); %>";

function Load_Setting()
{
	if (!rules_num) {
 		disableButton(document.websURLFilterDelete.deleteSelUrl);
 		disableButton(document.websURLFilterDelete.reset);
	}
	else{
        enableButton(document.websURLFilterDelete.deleteSelUrl);
        enableButton(document.websURLFilterDelete.reset);
	}
	
	updateState();
}

function deleteWebsURLClick()
{
	for(i=0; i< rules_num; i++){
		var tmp = eval("document.websURLFilterDelete.DR"+i);
		if(tmp.checked == true)
			return true;
	}
	alert(JS_msg18);
	return false;
}

function AddWebsURLFilterClick()
{
	if (rules_num >= 15){
		alert(JS_msg19);
		return false;
	}
	
	if ((document.websURLBasicSettings.URLFilterEnabled.options.selectedIndex == 0) && (document.websURLFilter.addURLFilter.value == ""))
		return true;
		
	if (!isAscciMsg(document.websURLFilter.addURLFilter.value, MM_url_address)) 
		return false;
	
	var p = all_str.split(";");
	for(i=0; i<p.length; i++){
		if (document.websURLFilter.addURLFilter.value==p[i]){
			alert(JS_msg9);
			return false;
		}
	}
	
	return true;
}

function updateState()
{
	if (document.websURLBasicSettings.URLFilterEnabled.options.selectedIndex == 1){
		enableTextField(document.websURLFilter.addURLFilter);
		enableButton(document.websURLFilter.apply);
		enableButton(document.websURLFilter.reset);
	}
	else{
		disableTextField(document.websURLFilter.addURLFilter);
		disableButton(document.websURLFilter.apply);
		disableButton(document.websURLFilter.reset);
	}
}

var xml = false;
function urlFilterSubmit(value)
{
	xml = false;
    if (window.XMLHttpRequest) { // Mozilla, Safari,...
        xml = new XMLHttpRequest();
        if (xml.overrideMimeType) {
            xml.overrideMimeType('text/xml');
        }
    } else if (window.ActiveXObject) { // IE
        try {
            xml = new ActiveXObject("Msxml2.XMLHTTP");
        } catch (e) {
            try {
            xml = new ActiveXObject("Microsoft.XMLHTTP");
            } catch (e) {}
        }
    }
    if (!xml) {
        alert(JS_msg5);
        return false;
    }
    xml.onreadystatechange = doChange;
	xml.open('POST', '/goform/websURLBasicSettings', true);
	xml.send('webURLEnabled='+value);
}

function doChange()
{
    if (xml.readyState == 4){
		if (xml.status == 200){
			window.location.reload();
		} else {
			//alert(JS_msg6);
		}
	}
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
<tr><td class="title"><script>dw(MM_urlf)</script></td></tr> 
<tr><td><script>dw(JS_msg_urlf)</script></td></tr>
<tr><td><hr></td></tr>
</table>

<form method=post name="websURLBasicSettings" action=/goform/websURLBasicSettings>
<input type="hidden" name="submit-url" value="/firewall/url_filtering.asp">
<input type="hidden" name="webURLEnabled">
<table width=100% border=0 cellpadding=3 cellspacing=1> 
<tr>
	<td class="thead"><script>dw(MM_urlf)</script>:</td>
	<td><select onChange="updateState();urlFilterSubmit(this.value)" name="URLFilterEnabled">
	<option value=0 <% getWebURLEnableASP(0); %>><script>dw(MM_disable)</script></option>
    <option value=1 <% getWebURLEnableASP(1); %>><script>dw(MM_enable)</script></option>
    </select></td>
  </tr>
</table>
</form>

<form action=/goform/websURLFilter method=POST name="websURLFilter">
<input type="hidden" name="submit-url" value="/firewall/url_filtering.asp">
<table width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr>
    <td class="title2" colspan="2"><script>dw(MM_add)</script><hr></td>
  </tr>
  <tr>
	<td class="thead"><script>dw(MM_url_address)</script>:</td>
	<td><input name="addURLFilter" size="32" maxlength="32" type="text"> </td>
  </tr>
</table>

<br>
<table width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr>
    <td>
      <script>dw('<input type=submit class=button value="'+BT_apply+'" name=apply onClick="return AddWebsURLFilterClick()"> &nbsp; &nbsp;\
      <input type=button class=button value="'+BT_reset+'" name=reset onClick="resetForm();">')</script>
    </td>
  </tr>
</table>
</form>

<form action=/goform/websURLFilterDelete method=POST name="websURLFilterDelete">
<input type="hidden" name="submit-url" value="/firewall/url_filtering.asp">
<p><b><script>dw(MM_urlf_list)</script>:<script>document.write(JS_msg51);</script></b></p>
<table width=100% border=1 cellpadding=3 style="border-collapse: collapse" bordercolor="#C8C8C8">
	<tr bgcolor=#f2f2f2>
		<td><b>No.</b></td>
		<td align="center"><b><script>dw(MM_url_address)</script></b></td>
	</tr>
	<script language="javascript">
	var i;
	
	if(all_str.length){
		entries = all_str.split(";");
		for(i=0; i<entries.length; i++){
			document.write("<tr><td>");
			document.write(i+1);
			document.write("<input type=checkbox name=DR"+i+"></td>");
			document.write("<td align=center>"+ entries[i] +"</td>");
			document.write("</tr>\n");
		}

		rules_num = entries.length;
	}
	</script>
</table>

<br>
<table width=100% border=0 cellpadding=3 cellspacing=1> 
  <tr>
    <td>
      <script>dw('<input type=submit class=button value="'+BT_delete+'" name="deleteSelUrl" onClick="return deleteWebsURLClick()"> &nbsp; &nbsp;\
      <input type=button class=button value="'+BT_reset+'" name="reset" onClick="resetForm();">')</script>
    </td>
  </tr>
</table>
</form>

</td></tr></table>
</blockquote>
</body></html>
