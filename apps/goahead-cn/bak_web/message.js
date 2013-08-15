//提示定义
var msg= "";
var msgOk = "<img src=images/info_success.gif>";
var msgErr = "<img src=images/info_error.gif>&nbsp;";

//------------------------------------------------------------------------------------
//判断不能为空
function IsEmpty(s)
{
	if(s.value == ""){
		msg = "选项不能为空。";
		return msg;
	}
	msg = "Yes";
	return msg;
}
//判断WPA密钥合法
function IsWpaOk(s)
{
	if(s.value == ""){
		msg = "选项不能为空。";
		return msg;
	}
	if(s.value.length < 8){
		msg = "无线密码不能少于8个字符。";
		return msg;
	}
	msg = "Yes";
	return msg;
}
//判断只能是数字
function IsNumNull(s)
{
	if(s.value == "" || isNaN(s.value)){
		msg = "选项不能为空，并且只能是数字。";
		return msg;
	}
	msg = "Yes";
	return msg;
}
//判断不包含非法字符
function IsIllegalChar(s)
{
	var re = /[~!@#$\%\^\&\*\(\)]{1,}/;
	if(re.test(s.value)){
		msg = "不能是非法字符。";
		return msg;
	}
	msg = "Yes";
	return msg;
}
//判断MAC地址格式
function IsMac(s)
{
	var re = /[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}/;
	if(!re.test(s.value)){
		msg = "MAC地址只能包含[0~9][A~F][a~f][:]，正确的格式如[00:E0:4C:ad:1e:21]。";
		return msg;
	}
	msg = "Yes";
	return msg;
}
function IsMacNull(s)
{
	if(s.value == ""){
		msg = "选项不能为空。";
		return msg;
	}
	
	var re = /[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}/;
	if(!re.test(s.value)) 
	{
		msg = "MAC地址只能包含[0~9][A~F][a~f][:]，正确的格式如[00:E0:4C:ad:1e:21]。";
		return msg;
	}
	msg = "Yes";
	return msg;
}
//判断IP地址
function IsIp(s)
{
	if(!IsIpAddr(s.value)){
		msg = "只能是数字和小圆点，且每段取值[0~255]。";
		return msg;
	}
	
	if(!IsRange(s.value,1,0,255) || !IsRange(s.value,2,0,255) || !IsRange(s.value,3,0,255) || !IsRange(s.value,4,1,254)){
		msg = "正确的IP格式如[192.168.13.100]。";
		return msg;
	}
	msg = "Yes";
	return msg;
}
function IsIpNull(s)
{
	if(s.value == "" || !IsIpAddr(s.value)){
		msg = "选项不能为空，且只能是数字和小圆点，且每段取值[0~255]。";
		return msg;
	}
	
	if(!IsRange(s.value,1,0,255) || !IsRange(s.value,2,0,255) || !IsRange(s.value,3,0,255) || !IsRange(s.value,4,1,254)){
		msg = "正确的IP格式如[192.168.13.100]。";
		return msg;
	}
	msg = "Yes";
	return msg;
}
//判断子网掩码
function IsMask(s)
{
	if(s.value == "" || !IsIpAddr(s.value)){
		msg = "选项不能为空，且只能是数字和小圆点，且每段取值[0~255]。";
		return msg;
	}
	
	if(!IsRange(s.value,1,0,256) || !IsRange(s.value,2,0,256) || !IsRange(s.value,3,0,256) || !IsRange(s.value,4,0,256)){
		msg = "正确的子网掩码格式如[255.255.255.0]。";
		return msg;
	}
	msg = "Yes";
	return msg;
}
//
function IsMinMaxNull(s,min,max)
{
	if(s.value == "" || isNaN(s.value) || s.value <min || s.value >max){
		msg = "选项不能为空，并且只能是["+ min +"~"+ max +"]间的数字。";
		return msg;
	}
	msg = "Yes";
	return msg;
}
//判断端口号
function IsPort(s)
{
	if(isNaN(s.value) || s.value <1 || s.value >65535){
		msg = "端口号只能是[1~65535]间的数字。";
		return msg;
	}
	msg = "Yes";
	return msg;
}
function IsPortNull(s)
{
	if(s.value == "" || isNaN(s.value) || s.value <1 || s.value >65535){
		msg = "端口号不能为空，并且只能是[1~65535]间的数字。";
		return msg;
	}
	msg = "Yes";
	return msg;
}
//获取ID
function getById(id)
{
	return document.getElementById(id);	
}
//隐藏输入框样式
function classHide(id)
{
	getById(id).className = "";
}
//显示输入框样式
function classShow(id)
{
	getById(id).className = "errText";
}
//隐藏提示
function divHide(id)
{
	getById(id).style.display = "none";
}
//显示提示
function divShow(id)
{
	getById(id).style.display = "";
}
//正确提示
function showOk(id)
{
	getById(id).innerHTML = msgOk;
}
//错误提示
function showErr(id)
{
	getById(id).innerHTML = msgErr+msg;
}

//判断IP地址格式
function IsIpAddr(str)
{
	var re=/^(\d{1,3}\.){3}\d{1,3}$/;
	if(!re.test(str)) 
		return 0;
	return 1;
}


//取值范围
function IsRange(str, num, min, max)
{
	d = atoi(str, num);
	if(d > max || d < min)
		return 0;
	return 1;
}

