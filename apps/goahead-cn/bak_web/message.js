//��ʾ����
var msg= "";
var msgOk = "<img src=images/info_success.gif>";
var msgErr = "<img src=images/info_error.gif>&nbsp;";

//------------------------------------------------------------------------------------
//�жϲ���Ϊ��
function IsEmpty(s)
{
	if(s.value == ""){
		msg = "ѡ���Ϊ�ա�";
		return msg;
	}
	msg = "Yes";
	return msg;
}
//�ж�WPA��Կ�Ϸ�
function IsWpaOk(s)
{
	if(s.value == ""){
		msg = "ѡ���Ϊ�ա�";
		return msg;
	}
	if(s.value.length < 8){
		msg = "�������벻������8���ַ���";
		return msg;
	}
	msg = "Yes";
	return msg;
}
//�ж�ֻ��������
function IsNumNull(s)
{
	if(s.value == "" || isNaN(s.value)){
		msg = "ѡ���Ϊ�գ�����ֻ�������֡�";
		return msg;
	}
	msg = "Yes";
	return msg;
}
//�жϲ������Ƿ��ַ�
function IsIllegalChar(s)
{
	var re = /[~!@#$\%\^\&\*\(\)]{1,}/;
	if(re.test(s.value)){
		msg = "�����ǷǷ��ַ���";
		return msg;
	}
	msg = "Yes";
	return msg;
}
//�ж�MAC��ַ��ʽ
function IsMac(s)
{
	var re = /[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}/;
	if(!re.test(s.value)){
		msg = "MAC��ַֻ�ܰ���[0~9][A~F][a~f][:]����ȷ�ĸ�ʽ��[00:E0:4C:ad:1e:21]��";
		return msg;
	}
	msg = "Yes";
	return msg;
}
function IsMacNull(s)
{
	if(s.value == ""){
		msg = "ѡ���Ϊ�ա�";
		return msg;
	}
	
	var re = /[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}/;
	if(!re.test(s.value)) 
	{
		msg = "MAC��ַֻ�ܰ���[0~9][A~F][a~f][:]����ȷ�ĸ�ʽ��[00:E0:4C:ad:1e:21]��";
		return msg;
	}
	msg = "Yes";
	return msg;
}
//�ж�IP��ַ
function IsIp(s)
{
	if(!IsIpAddr(s.value)){
		msg = "ֻ�������ֺ�СԲ�㣬��ÿ��ȡֵ[0~255]��";
		return msg;
	}
	
	if(!IsRange(s.value,1,0,255) || !IsRange(s.value,2,0,255) || !IsRange(s.value,3,0,255) || !IsRange(s.value,4,1,254)){
		msg = "��ȷ��IP��ʽ��[192.168.13.100]��";
		return msg;
	}
	msg = "Yes";
	return msg;
}
function IsIpNull(s)
{
	if(s.value == "" || !IsIpAddr(s.value)){
		msg = "ѡ���Ϊ�գ���ֻ�������ֺ�СԲ�㣬��ÿ��ȡֵ[0~255]��";
		return msg;
	}
	
	if(!IsRange(s.value,1,0,255) || !IsRange(s.value,2,0,255) || !IsRange(s.value,3,0,255) || !IsRange(s.value,4,1,254)){
		msg = "��ȷ��IP��ʽ��[192.168.13.100]��";
		return msg;
	}
	msg = "Yes";
	return msg;
}
//�ж���������
function IsMask(s)
{
	if(s.value == "" || !IsIpAddr(s.value)){
		msg = "ѡ���Ϊ�գ���ֻ�������ֺ�СԲ�㣬��ÿ��ȡֵ[0~255]��";
		return msg;
	}
	
	if(!IsRange(s.value,1,0,256) || !IsRange(s.value,2,0,256) || !IsRange(s.value,3,0,256) || !IsRange(s.value,4,0,256)){
		msg = "��ȷ�����������ʽ��[255.255.255.0]��";
		return msg;
	}
	msg = "Yes";
	return msg;
}
//
function IsMinMaxNull(s,min,max)
{
	if(s.value == "" || isNaN(s.value) || s.value <min || s.value >max){
		msg = "ѡ���Ϊ�գ�����ֻ����["+ min +"~"+ max +"]������֡�";
		return msg;
	}
	msg = "Yes";
	return msg;
}
//�ж϶˿ں�
function IsPort(s)
{
	if(isNaN(s.value) || s.value <1 || s.value >65535){
		msg = "�˿ں�ֻ����[1~65535]������֡�";
		return msg;
	}
	msg = "Yes";
	return msg;
}
function IsPortNull(s)
{
	if(s.value == "" || isNaN(s.value) || s.value <1 || s.value >65535){
		msg = "�˿ںŲ���Ϊ�գ�����ֻ����[1~65535]������֡�";
		return msg;
	}
	msg = "Yes";
	return msg;
}
//��ȡID
function getById(id)
{
	return document.getElementById(id);	
}
//�����������ʽ
function classHide(id)
{
	getById(id).className = "";
}
//��ʾ�������ʽ
function classShow(id)
{
	getById(id).className = "errText";
}
//������ʾ
function divHide(id)
{
	getById(id).style.display = "none";
}
//��ʾ��ʾ
function divShow(id)
{
	getById(id).style.display = "";
}
//��ȷ��ʾ
function showOk(id)
{
	getById(id).innerHTML = msgOk;
}
//������ʾ
function showErr(id)
{
	getById(id).innerHTML = msgErr+msg;
}

//�ж�IP��ַ��ʽ
function IsIpAddr(str)
{
	var re=/^(\d{1,3}\.){3}\d{1,3}$/;
	if(!re.test(str)) 
		return 0;
	return 1;
}


//ȡֵ��Χ
function IsRange(str, num, min, max)
{
	d = atoi(str, num);
	if(d > max || d < min)
		return 0;
	return 1;
}

