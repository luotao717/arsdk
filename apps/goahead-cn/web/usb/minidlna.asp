

<html>
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Expires" content="-1">
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<link rel="stylesheet" href="../style/normal_ws.css" type="text/css">
<script language="javascript" src="../js/language_<% getCfgZero(1, "LanguageType"); %>.js"></script>
<script language="javascript" src="../js/common.js"></script>
<script language="JavaScript">
var dlna_enable =  "<%  getCfgGeneral(1, "DlnaEnabled"); %>";
var dlna_port = "<%  getCfgGeneral(1, "DlnaPort"); %>";
var dlna_friend_name = "<%  getCfgGeneral(1, "DlnaName"); %>";
var dlna_Scan = "<% getCfgGeneral(1, "DlnaScan"); %>";
var usb_state = 1*"<% getIndex(1, "usbStatus"); %>";
//var dlna_enable = '1';
//var dlna_port = '8200';
//var enable_tivo = '1';
//var strict_dlna = '1';
//var dlna_friend_name = 'NBox';
var index;
//var dlna_port = parseInt('');
//var lan_ip = '10.0.0.1';

function initValue()
{
	if (usb_state == 0)	{
		document.getElementById("div_no_usbdivice").style.display = "";
		document.getElementById("div_usbdivice").style.display = "none";
	}else{
		document.getElementById("div_no_usbdivice").style.display = "none";
		document.getElementById("div_usbdivice").style.display = "";
		document.DLNA.DLNAPort.value = dlna_port;
		document.DLNA.friend_name.value = dlna_friend_name;
		if(dlna_Scan=="1"){
			document.DLNA.rescan.checked=true;
			document.DLNA.rescan_h.value = 1;
		}

		if(dlna_enable == "1")	
			document.DLNA.DLNAEnabled[0].checked = true;
		else
			document.DLNA.DLNAEnabled[1].checked = true;
	
		//if(enable_tivo == "1")
			//document.DLNA.TIVOEnabled[0].checked = true;
		//else		
			//document.DLNA.TIVOEnabled[1].checked = true;	
		
		//if(strict_dlna == "1")
		//	document.DLNA.StrictDLNA[0].checked = true;
		//else		
			//document.DLNA.StrictDLNA[1].checked = true;
	
		dlna_enable_switch();
	
		if(document.DLNA.DLNAEnabled[0].checked == true){
			document.getElementById("AddDir").style.display = "";
			document.getElementById("DelDir").style.display = "";
		}else{
			document.getElementById("AddDir").style.display = "none";
			document.getElementById("DelDir").style.display = "none";
		}
	}
}

function CheckValue()
{
	var reg_l = /^\w+$/;
	if(document.DLNA.DLNAEnabled[0].checked == true){
		if (document.DLNA.DLNAPort.value == ""){
			alert(MM_dlna8);
			document.DLNA.DLNAPort.focus();
			return false;
		}
		else if (!portCheckNullMsg(document.DLNA.DLNAPort,MM_port)){
			return false;
		}

		if (document.DLNA.friend_name.value == ""){
			alert(MM_dlna10);
			document.DLNA.friend_name.focus();
			return false;
		}
		else if(!reg_l.test(document.DLNA.friend_name.value)){
			alert(MM_dlna11);
			document.DLNA.friend_name.focus();
			return false;
		}
	}
	return true;
}

function CheckValue_dir()
{
	var rule = new Array();
	var item = new Array();
	var ruleStr="<% Dlna_Dir(); %>";
	rule = ruleStr.split(';');
	if(document.DLNA.DLNAEnabled[0].checked == true){
		if(index==4){
			alert(MM_dlna12);
			return false;
		}

		if (document.AddDir.direct.value == ""){
			alert(MM_dlna13);
			document.AddDir.direct.focus();
			return false;
		}
		for(i=0; i<rule.length; i++){
			if(rule[i].indexOf(',') != -1){
				item = rule[i].split(',');
				if(document.AddDir.direct.value == item[1] && 
					((item[0]=="A" && document.AddDir.media_type.value=="audio") ||
					 (item[0]=="V" && document.AddDir.media_type.value=="video") ||
					 (item[0]=="P" && document.AddDir.media_type.value=="images"))){
					alert(MM_dlna14);
					return false;
				}
			}else{
				if(document.AddDir.media_type.value=="all" && document.AddDir.direct.value == rule[i]){
					alert(MM_dlna15);
					return false;
				}
			}
		}
	}else{
		alert(MM_dlna16);
		return false;
	}

	return true;
}

function dlna_enable_switch()
{
	if (document.DLNA.DLNAEnabled[0].checked == true){
		document.DLNA.DLNAPort.disabled = false;
	//	document.DLNA.TIVOEnabled.disabled = false;
	//	document.DLNA.StrictDLNA.disabled = false;
		document.DLNA.friend_name.disabled = false;
	}else{
		document.DLNA.DLNAPort.disabled = true;
	//	document.DLNA.TIVOEnabled.disabled = true;
		//document.DLNA.StrictDLNA.disabled = true;
		document.DLNA.friend_name.disabled = true;
	}
}

function rescan_enable()
{
	if(document.DLNA.rescan.checked)
		document.DLNA.rescan_h.value = 1;
	else
		document.DLNA.rescan_h.value = 0;
}

function changedelte()
{
	var i;
	for (i=0; i<index; i++){ 
	    if(document.getElementById("del_en_"+i).checked == true) {
		//	document.DelDir.selectIndex.value=i;
	    	document.DelDir.dirdelete.disabled=false;
			return true;
	    }
	}

	document.DelDir.dirdelete.disabled=true;
}

function open_diradd_window()
{
	window.open("minidlna_add_dir.asp","media_path","toolbar=no, location=yes, scrollbars=yes, resizable=no, width=640, height=480")
}
</script>
</head>

<body class="contentbody" onLoad="initValue()">
<table id="div_no_usbdivice" style="display:none" width=700><tr><td>
<table width=100% border=0 cellpadding=3 cellspacing=1> 
<tr><td><img src="../graphics/warning.gif" align="absmiddle">&nbsp;&nbsp;
<script>dw(JS_msg_usb)</script>&nbsp;&nbsp;
<script>dw('<input type=button class=button value="'+BT_refresh+'" onClick="window.location.reload()">')</script></td></tr>
</table>
</td></tr></table>

<table id="div_usbdivice" width=700><tr><td>
<form method=post name="DLNA"  action="/goform/DLNA_mini">
<table table width=100% border=0 cellpadding=3 cellspacing=1>
<tr><td class="title"><script>dw(MM_dlna17)</script></td></tr>
<tr><td><hr></td></tr>
</table>

<table table width=100% border=0 cellpadding=3 cellspacing=1>
<tr><td class="title3" colspan="2"><script>dw(MM_dlna18)</script></td></tr>
<tr>
<td class="thead"><script>dw(MM_dlna19)</script> </td>
<td><input class=radio type=radio name="DLNAEnabled" value="1" onClick="dlna_enable_switch()"><script>dw(MM_enable)</script>
<input class=radio type=radio name="DLNAEnabled" value="0" onClick="dlna_enable_switch()" checked><script>dw(MM_disable)</script></td>
</tr>
<tr>
<td class="thead"><script>dw(MM_dlna20)</script></td>
<td><input type=text name="DLNAPort" maxlength=6 value="" size="6" class="navi_text"></td>
</tr>
<tr style="display:none;"> 
<td class="thead">TiVO </td>
<td><input class=radio type=radio name="TIVOEnabled" value="1" checked>Ativar
<input class=radio type=radio name="TIVOEnabled" value="0">Desativar </td>
</tr>
<tr style="display:none;"> 
<td class="thead">Reduzir Imagens JPEG muito grandes </td>
<td><input class=radio type=radio name="StrictDLNA" value="1" checked>Ativar
<input class=radio type=radio name="StrictDLNA" value="0">Desativar</td>
</tr>
<tr>
<td class="thead"><script>dw(MM_dlna22)</script> </td>
<td><input type=text name="friend_name" maxlength=32 value="" size="32"  class="navi_text"></td>
</tr>
<tr>
<td class="thead"><script>dw(MM_dlna21)</script> </td>
<td><input type="checkbox" name="rescan" onclick=rescan_enable();>
<input type="hidden" name="rescan_h" value=0></td>
</tr>
</table>

<br>
<table width=100% border=0 cellpadding=3 cellspacing=1> 
<tr>
<td>
<script>dw('<input type=submit class=button value="'+BT_apply+'" onClick="return CheckValue()"> &nbsp; &nbsp;\
<input type=button class=button value="'+BT_reset+'" onClick="resetForm();">')</script>
</td>
</tr>
</table>
</form>

<br>
<br>
<form method=post name="DelDir" id="DelDir" style="display:none;" action="/goform/DelDir">
<table table width=100% border=0 cellpadding=3 cellspacing=3>
<tr><td class="title3" colspan="4"><script>dw(MM_dlna23)</script></td></tr>
<tr class="title4">
<td><script>dw(MM_dlna24)</script></td>
<td><script>dw(MM_dlna1)</script></td>
<td><script>dw(MM_dlna26)</script></td>
<td><script>dw(BT_delete)</script></td>
</tr>
<script language="JavaScript" type="text/javascript">	
var rule=new Array;
var item=new Array;
var i;
var type;
var dir;
var ruleStr="<% Dlna_Dir(); %>";
if (ruleStr != "") {
	rule=ruleStr.split(";");
	for (i=0; i<rule.length-1; i++) { 
		index = i + 1;
		item=rule[i].split(",");
		if(item[0] == "A")
			type =  "Somente Áudio";
		else if(item[0] == "V")
			type =  "Somente Vídeo";
		else if(item[0] == "P")
			type =  "Somente Imagens";
		else
			type = "Todo o Tipo de Mídia";
		
		if(type == "Todo o Tipo de Mídia")
			dir = item[0];
		else
			dir = item[1];
		document.write("<tr>");
		document.write("<td class='leftnone' align=\"center\">"+index+"</td>");
		document.write("<td align=\"center\">"+dir+"</td>");
		document.write("<td align=\"center\">"+type+"</td>");
		document.write("<td class='rightnone' align=\"center\"><input type=checkbox name='del_en_"+i+"'id='del_en_"+i+"' onClick=\"changedelte()\"></td>");
		document.write("</tr>");
	} 	
	checks =rule.length-1;
}
</script>
</table>

<br>
<table width=100% border=0 cellpadding=3 cellspacing=1> 
<tr>
<td>
<script>dw('<input type=submit class=button value="'+BT_delete+'" name="dirdelete" disabled>')</script>
</td>
</tr>
</table>
</form>

<form method=post name="AddDir" id="AddDir" style="display:none;" action="/goform/AddDir">
<table table width=100% border=0 cellpadding=3 cellspacing=1>
<tr><td class="title3" colspan="2"><script>dw(MM_dlna27)</script> </td></tr>
<tr>
<td class="thead"><script>dw(MM_dlna1)</script> </td>
<td><input type=text name="direct" maxlength=32 value="" size="32" class="navi_text"> <script>dw('<input type="button" class="button" onClick="open_diradd_window();" value="'+BT_scan+'"/>')</script></td>
</tr>
<tr>
<td class="thead"><script>dw(MM_dlna28)</script> </td>
<td><select class="list" name="media_type">
<option value="all"><script>dw(MM_dlna29)</script> </option>
<option value="audio"><script>dw(MM_dlna30)</script> </option>
<option value="video"><script>dw(MM_dlna31)</script></option>
<option value="images"><script>dw(MM_dlna32)</script></option>
</select></td>
</tr>
</table>

<br>
<table width=100% border=0 cellpadding=3 cellspacing=1> 
<tr>
<td>
<script>dw('<input type=submit class=button value="'+BT_add+'" onClick="return CheckValue_dir();">')</script>
</td>
</tr>
</table>
</form>

<br>
<br>
</td></tr></table>
</body></html>

