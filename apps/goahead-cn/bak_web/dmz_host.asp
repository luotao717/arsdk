<html>
<head>
<title>WR922</title>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312" />
<link href="images/kingnet.css" type="text/css" rel="stylesheet">
<script language="JavaScript" type="text/javascript">

function checkRange(str, num, min, max)
{
	d = atoi(str,num);
	if(d > max || d < min)
		return false;
	return true;
}
function isAllNum(str)
{
	for (var i=0; i<str.length; i++) {
		if ((str.charAt(i) >= '0' && str.charAt(i) <= '9') || (str.charAt(i) == '.' ))
			continue;
		return 0;
	}
	return 1;
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


function checkIPAddr(field)
{
    if(field.value == ""){
        alert("IP��ַ����Ϊ��.");
        field.value = field.defaultValue;
        field.focus();
        return false;
    }

    if ( isAllNum(field.value) == 0) {
        alert('������ [0-9] ��Χ.');
        field.value = field.defaultValue;
        field.focus();
        return false;
    }

    if( (!checkRange(field.value,1,0,255)) ||
        (!checkRange(field.value,2,0,255)) ||
        (!checkRange(field.value,3,0,255)) ||
        (!checkRange(field.value,4,1,254)) ){
        alert('IP ��ַ��ʽ����.');
        field.value = field.defaultValue;
        field.focus();
        return false;
    }

   return true;
}

function formCheck()
{
	if(!document.DMZ.DMZEnabled.options.selectedIndex){
		// user choose disable
		return true;
	}

	if(document.DMZ.DMZIPAddress.value == ""){
		alert("û������IP��ַ.");
		document.DMZ.DMZIPAddress.focus();
		return false;
	}

	if(! checkIPAddr(document.DMZ.DMZIPAddress) ){
//		alert("IP address format error.");
		document.DMZ.DMZIPAddress.focus();
		return false;
	}

	return true;
}

function disableTextField (field)
{
  if(document.all || document.getElementById)
    field.disabled = true;
  else {
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

function updateState()
{
	if(document.DMZ.DMZEnabled.options.selectedIndex == 1){
		enableTextField(document.DMZ.DMZIPAddress);
	}else{
		disableTextField(document.DMZ.DMZIPAddress);
	}
}

</script>
</head>
<body onLoad="updateState()">
<form method=post name="DMZ" action=/goform/DMZ>
<table width="90%" border="0" cellpadding="0" cellspacing="0" align="center">
  <tr>
    <td style="padding-top:30px;">
	   <table width="100%" border="0" cellpadding="0" cellspacing="0">
          <tr>
            <td class="nav">��ǰ·��:KN-WR922 &gt;&gt;�������� &gt;&gt;DMZ ���� </td>
          </tr>
          <tr>
            <td>&nbsp;</td>
          </tr>
		  <tr>
            <td class="space" style="padding-left:10px; padding-right:10px;">&nbsp;&nbsp;&nbsp;������Ѿ������ڵ�ĳ̨�������ŵ���������ʱ�򣬿���ʹ�ñ����ܣ��������������ϵ�������������·����WAN��IP���൱�ڷ����������õ���̨��������������ע�⣺���������ú���̨������������¶�ڻ������ϣ������ܵ�·�����ı�����������ʹ�á� </td>
          </tr>
		  <tr>
		    <td>&nbsp;</td>
		  </tr>
          <tr>
		    <td><table class="space" width=100% border="0" cellpadding="0" cellspacing="0">
				<tr>
				   <td colspan="2" class="titlebg">DMZ ��������</td>
				</tr>
                <tr>
                  <td width="25%" class="contentpadding">DMZ ����:</td>
                  <td width="75%"><select onChange="updateState()" name="DMZEnabled" size="1">
	<option value=0 <% getDMZEnableASP(0); %> >����</option>
    <option value=1 <% getDMZEnableASP(1); %> >����</option>
    </select></td>
                </tr>
				<tr>
				  <td class="contentpadding" >DMZ ����IP��ַ:</td>
				  <td><input type="text" size="24" name="DMZIPAddress" value=<% showDMZIPAddressASP(); %> >
				  (�������뿪�ŵ�ĳ̨������������IP��ַ)</td>
				</tr>
            </table></td>
		  </tr>
		  <tr><td>&nbsp;</td></tr>
		  <tr id=button>
            <td class="contentpadding"><input type="hidden" value="/dmz_host.asp" name="submit-url"><input type="submit" class="button"  value="�� ��" name="addDMZ" onClick="return formCheck()">&nbsp;&nbsp;<input type="reset" class="button"  value="ȡ ��" name="reset"></td>
          </tr>
		  <tr><td>&nbsp;</td></tr>
        </table>
	</td>
  </tr>
  <tr>
    <td>&nbsp;</td>
  </tr>
</table>
</form>
</body>
</html>
