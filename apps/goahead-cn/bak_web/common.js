function disableButton (button) {
  //if (verifyBrowser() == "ns")
  //	return;
  if (document.all || document.getElementById)
    button.disabled = true;
  else if (button) {
    button.oldOnClick = button.onclick;
    button.onclick = null;
    button.oldValue = button.value;
    button.value = 'DISABLED';
  }
}

function enableButton (button) {
  //if (verifyBrowser() == "ns")
  //	return;
  if (document.all || document.getElementById)
    button.disabled = false;
  else if (button) {
    button.onclick = button.oldOnClick;
    button.value = button.oldValue;
  }
}
function disableTextField (field) {
  if (document.all || document.getElementById)
    field.disabled = true;
  else {
    field.oldOnFocus = field.onfocus;
    field.onfocus = skip;
  }
}

function enableTextField (field) {
  if (document.all || document.getElementById)
    field.disabled = false;
  else {
    field.onfocus = field.oldOnFocus;
  }
}


function checkSubnet(ip, mask, client)
{
  ip_d = getDigit(ip, 1);
  mask_d = getDigit(mask, 1);
  client_d = getDigit(client, 1);
  if ( (ip_d & mask_d) != (client_d & mask_d ) )
	return false;

  ip_d = getDigit(ip, 2);
  mask_d = getDigit(mask, 2);
  client_d = getDigit(client, 2);
  if ( (ip_d & mask_d) != (client_d & mask_d ) )
	return false;

  ip_d = getDigit(ip, 3);
  mask_d = getDigit(mask, 3);
  client_d = getDigit(client, 3);
  if ( (ip_d & mask_d) != (client_d & mask_d ) )
	return false;

  ip_d = getDigit(ip, 4);
  mask_d = getDigit(mask, 4);
  client_d = getDigit(client, 4);
  if ( (ip_d & mask_d) != (client_d & mask_d ) )
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
function checkRange(str, num, min, max)
{
	d = atoi(str, num);
	if (d > max || d < min)
		return false;
	return true;
}

function atoi(str, num)
{
	i = 1;
	if (num != 1) {
		while (i != num && str.length != 0) {
			if (str.charAt(0) == '.') {
				i++;
			}
			str = str.substring(1);
		}
		if (i != num)
			return -1;
	}

	for (i=0; i<str.length; i++) {
		if (str.charAt(i) == '.') {
			str = str.substring(0, i);
			break;
		}
	}
	if (str.length == 0)
		return -1;
	return parseInt(str, 10);
}
function checkIpAddr(field, ismask)
{
	if (field.value == "") {
		alert("Error. IP address is empty.");
		field.value = field.defaultValue;
		field.focus();
		return false;
	}

	if (isAllNum(field.value) == 0) {
		alert('It should be a [0-9] number.');
		field.value = field.defaultValue;
		field.focus();
		return false;
	}

	if (ismask) {
		if ((!checkRange(field.value, 1, 0, 256)) ||
				(!checkRange(field.value, 2, 0, 256)) ||
				(!checkRange(field.value, 3, 0, 256)) ||
				(!checkRange(field.value, 4, 0, 256)))
		{
			alert('IP adress format error.');
			field.value = field.defaultValue;
			field.focus();
			return false;
		}
	}
	else {
		if ((!checkRange(field.value, 1, 0, 255)) ||
				(!checkRange(field.value, 2, 0, 255)) ||
				(!checkRange(field.value, 3, 0, 255)) ||
				(!checkRange(field.value, 4, 1, 254)))
		{
			alert('IP adress format error.');
			field.value = field.defaultValue;
			field.focus();
			return false;
		}
	}
	return true;
}



function checkIpAddr1(field, msg)
{
  if (field.value=="") {
	alert("IP 地址不能为空！ 形式为：xxx.xxx.xxx.xxx.");
	field.value = field.defaultValue;
	field.focus();
	return false;
  }
   if ( validateKey(field.value) == 0) {
      alert(msg + ' 只能为0－9之间的数字');
      field.value = field.defaultValue;
      field.focus();
      return false;
   }
   if ( !checkDigitRange(field.value,1,0,255) ) {
      alert(msg+' 超出范围（0-255）.');
      field.value = field.defaultValue;
      field.focus();
      return false;
   }
   if ( !checkDigitRange(field.value,2,0,255) ) {
      alert(msg + ' 超出范围（0-255）.');
      field.value = field.defaultValue;
      field.focus();
      return false;
   }
   if ( !checkDigitRange(field.value,3,0,255) ) {
      alert(msg + ' 超出范围（0-255）.');
      field.value = field.defaultValue;
      field.focus();
      return false;
   }
   if ( !checkDigitRange(field.value,4,0,255) ) {
      alert(msg + ' 超出范围（0-255）.');
      field.value = field.defaultValue;
      field.focus();
      return false;
   }
   return true;
}

function validateKey(str)
{
   for (var i=0; i<str.length; i++) {
    if ( (str.charAt(i) >= '0' && str.charAt(i) <= '9') ||
    		(str.charAt(i) == '.' ) )
			continue;
	return false;
  }
  return true;
}

function checkDigitRange(str, num, min, max)
{
  d = getDigit(str,num);
  if ( d > max || d < min )
      	return false;
  return true;
}

function getDigit(str, num)
{
  i=1;
  if ( num != 1 ) {
  	while (i!=num && str.length!=0) {
		if ( str.charAt(0) == '.' ) {
			i++;
		}
		str = str.substring(1);
  	}
  	if ( i!=num )
  		return -1;
  }
  for (i=0; i<str.length; i++) {
  	if ( str.charAt(i) == '.' ) {
		str = str.substring(0, i);
		break;
	}
  }
  if ( str.length == 0)
  	return -1;
  d = parseInt(str, 10);
  return d;
}

function checkClientRange(start,end)
{
  start_d = getDigit(start,4);
  start_d += getDigit(start,3)*256;
  start_d += getDigit(start,2)*256;
  start_d += getDigit(start,1)*256;

  end_d = getDigit(end,4);
  end_d += getDigit(end,3)*256;
  end_d += getDigit(end,2)*256;
  end_d += getDigit(end,1)*256;

  if ( start_d < end_d )
	return true;

  return false;
}