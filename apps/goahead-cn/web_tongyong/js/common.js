function stringCheck(v,m)
{
	var t = /[;,\r\n]{1,}/;
	if (t.test(v.value)){
		alert(m+JS_msg59);
		v.value=v.defaultValue;
		v.focus();
		return 0;
	}
	return 1;
}
function numCheckMsg(v,m) 
{
	var t = /[^0-9]{1,}/;
	if (t.test(v.value)){
		alert(m+JS_msg61);
		v.value=v.defaultValue;
		v.focus();
		return 0;
	}
	return 1;
}
function skip () { this.blur(); }
function disableTextField(field){
  	if (document.all || document.getElementById)
    	field.disabled = true;
  	else {
    	field.oldOnFocus = field.onfocus;
    	field.onfocus = skip;
  	}
}
function enableTextField(field){
  	if (document.all || document.getElementById)
    	field.disabled = false;
  	else
    	field.onfocus = field.oldOnFocus;
}
function disableButton(button){
  	if (document.all || document.getElementById)
    	button.disabled = true;
  	else if (button) {
		button.oldOnClick = button.onclick;
		button.onclick = null;
		button.oldValue = button.value;
		button.value = 'DISABLED';
  	}
}
function enableButton(button){
  	if (document.all || document.getElementById)
    	button.disabled = false;
  	else if (button) {
    	button.onclick = button.oldOnClick;
    	button.value = button.oldValue;
  	}
}
function atoi(s,n){
    i=1;
    if (n!=1) {
        while (i!=n && s.length!=0){
            if (s.charAt(0)=='.') i++;
            s = s.substring(1);
        }
        if (i!=n) return -1;
    }
    for (i=0; i<s.length; i++){
        if (s.charAt(i)=='.'){ 
			s=s.substring(0, i); 
			break; 
		}
    }
    if (s.length==0) return -1;
    return parseInt(s, 10);
}
//去掉两端空格
function trim(s){
    return s.replace(/(^\s*)|(\s*$)/g, "");
}
//获取ID
function getById(id){
	return document.getElementById(id);	
}
//获取Name
function getByName(name){
	return document.getElementByName(name);	
}
//判断不能为空
function isEmpty(s){
	if(s==""||s==null) return 0;
	return 1;
}
function isEmptyMsg(s,m){
	if(s==""||s==null) {alert(m+JS_msg115); return 0;}
	return 1;
}
//判断不能为空
function isBlank(s){
	if(s==""||s==null||(s.indexOf(" ")>=0)) return 0;
	return 1;
}
function isBlankMsg(s,m){
	if(s==""||s==null||(s.indexOf(" ")>=0)) {alert(m+JS_msg58); return 0;}
	return 1;
}
//判断只能是数字
function isNumber(s){
	var re=/[^0-9]/;
	if(re.test(s)) return 0;
	return 1;
}
function isNumberMsg(s,m){
	if(!isNumber(s)) {alert(m+JS_msg108); return 0;}
	return 1;
}
//判断数字的范围
function isNumberRange(s,min,max){
	if(!isNumber(s)) return 0;
	if(s<min||s>max) return 0;
	return 1;
}
//判断只能是包括字母，数字，下划线，中杠线，小数点，@，并且不能包括空格符，换行符，回车符
function isAscci(s){
	var re=/[^A-Za-z0-9_\.@-]/;
	if(re.test(s)) return 0;
	return 1;
}
function isAscciMsg(s,m){
	if(!isEmpty(s)) {alert(m+JS_msg58); return 0;}
	if(!isAscci(s)) {alert(m+JS_msg109); return 0;}
	return 1;
}
//判断只能是包括字母，数字
function isAbc(s){
	var re=/[^A-Za-z0-9]/;
	if(re.test(s)) return 0;
	return 1;
}
function isAbcMsg(s,m){
	if(!isEmpty(s)) {alert(m+JS_msg58); return 0;}
	if(!isAbc(s)) {alert(m+JS_msg110); return 0;}
	return 1;
}
//判断只能是16进制编码
function isHex(s){
	var re=/[^A-Fa-f0-9]/;
	if(re.test(s)) return 0;
	return 1;	
}
function isHexMsg(s,m){
	if(!isHex(s)) {alert(m+JS_msg111); return 0;}
	return 1;
}
//判断端口号
function isPort(s){
	if(!isNumber(s)) return 0;
	if(parseInt(s)<1||parseInt(s)>65535) return 0;
	return 1;
}
function isPortMsg(s){
	if(!isEmpty(s)) {alert(JS_msg112); return 0;}
	if(!isPort(s)) {alert(JS_msg113); return 0;}
	return 1;
}
function isPortRange(s1,s2){
	if(parseInt(s1)>parseInt(s2)) {alert(JS_msg114); return 0;}
	return 1;
}
//判断MAC地址格式，不能包括多播和广播地址
function isMac(s){
	var re=/[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}/;
	if(!re.test(s)) return 0;
	if(s.length!=17) return 0;
	if((s.toUpperCase()=="FF:FF:FF:FF:FF:FF")||(s.toUpperCase()=="00:00:00:00:00:00")) return 0;
	for(var k=0;k<s.length;k++){if((s.charAt(1)&0x01)||(s.charAt(1).toUpperCase()=='B')||(s.charAt(1).toUpperCase()=='D')||(s.charAt(1).toUpperCase()=='F')) return 0;}
	return 1;
}
function isMacMsg(s,m){
	if(!isEmpty(s)) {alert(m+JS_msg115); return 0;}
	if(!isMac(s)) {alert(m+JS_msg116); return 0;}
	return 1;
}
//判断IP地址
function isIpCharset(s){
	var re=/[^0-9.]/;
	if(re.test(s)) return 0;
	var v=s.split(".");
	if(v.length!=4) return 0;
	return 1;
}
function ipTest(s,n,min,max){
	var d=atoi(s,n);
	if(d<min||d>max) return 0;
	return 1;
}
function isIpAddr(s){
	if(!isIpCharset(s)) return 0;
	if(!ipTest(s,1,1,255)||!ipTest(s,2,0,255)||!ipTest(s,3,0,255)||!ipTest(s,4,1,254)) return 0;
	return 1;
}
function isIpAddrMsg(s,m){
	if(!isEmpty(s)) {alert(m+JS_msg58); return 0;}
	if(!isIpAddr(s)) {alert(m+JS_msg117); return 0;}
	return 1;
}
//判断子网掩码
function maskTest(s,n){
  	var d=atoi(s,n);
  	if(!(d==0||d==128||d==192||d==224||d==240||d==248||d==252||d==254||d==255)) return 0;
  	return 1;
}
function isMaskAddr(s){
	if(!isIpCharset(s)) return 0;
	if(!maskTest(s,1)||!maskTest(s,2)||!maskTest(s,3)||!maskTest(s,4)) return 0;
	return 1;
}
function isMaskAddrMsg(s,m){
	if(!isEmpty(s)) {alert(m+JS_msg58); return 0;}
	if(!isMaskAddr(s)) {alert(m+JS_msg118); return 0;}
	return 1;
}
//判断两个IP地址必须在同一网段
function isIpSubnet(s1,mn,s2){
  	var ip1=s1.split(".");
   	var ip2=s2.split(".");
   	var ip3=mn.split(".");
   	//if(ip1.length!=4||ip2.length!=4||ip3.length!=4) return 0;
   	for(var k=0;k<=3;k++){
		if((ip1[k]&ip3[k])!=(ip2[k]&ip3[k])) return 0;
	}
   	return 1;
}
//判断两个IP地址，并且起始不能大于结束地址
function isIpRange(sip,eip){
	var ip1=sip.split(".");
	var ip2=eip.split(".");
	//if(ip1.length!=4||ip2.length!=4) return 0;
	for(var k=0;k<4;k++){
		var a=Number(ip1[3]);
		var b=Number(ip2[3]);
      	if(a>=b) {alert(JS_msg119); return 0;}
	}
	return 1;
}
function isServerIp(s) {
	if (!isAscci(s) && !isIpAddr(s)) return 0;
	return 1;
}
function checkDate(str) {
	var month = ["Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"];
	var week = [MM_sun, MM_mon, MM_tue, MM_wed, MM_thu, MM_fri, MM_sat];
	
	if ((str.substring(4,5)) == " ") str = str.replace(" ","");
	else str = str;
	
	var t = str.split(" ");	
	for (var j=0; j<12; j++){
		if (t[0] == month[j]) t[0] = j + 1;
	}	
	return t[2] + "-" + t[0] + "-" + t[1];
}
//------------------------------------------------------------------
function Cfg(i,n,v){
	this.i=i;
    this.n=n;
    this.v=this.o=v;
}
var CA = new Array();
function addCfg(n,i,v){
	CA.length++;
    CA[CA.length-1]= new Cfg(i,n,v);
}
function idxOfCfg(kk){
    if (kk=='undefined') { alert("Undefined"); return -1; }
    for (var i=0; i< CA.length ;i++){
        if ( CA[i].n != 'undefined' && CA[i].n==kk ) return i;
    }
    return -1;
}
function getCfg(n){
	var idx=idxOfCfg(n)
	if (idx >=0) return CA[idx].v;
	else return "";
}
function cfg2Form(f){
	for (var i=0;i<CA.length;i++){
        var e=eval('f.'+CA[i].n);
        if (e){
			if (e.name=='undefined') continue;
			if (e.length && e[0].type=='text'){
				//e.defaultValue=e.value;
			}
			else if (e.length && e[0].type=='radio'){
				for (var j=0;j<e.length;j++)
					e[j].checked=e[j].defaultChecked=(e[j].value==CA[i].v);
			}
			else if (e.type=='checkbox'){
				e.checked=e.defaultChecked=Number(CA[i].v);
			}
			else if (e.type=='select-one'){
				for (var j=0;j<e.options.length;j++)
					 e.options[j].selected=e.options[j].defaultSelected=(e.options[j].value==CA[i].v);
			}
			else{
				e.value=getCfg(e.name);
			}
			if (e.defaultValue!='undefined') e.defaultValue=e.value;
		}
    }
}

function openWindow(url, windowName, wide, high) 
{
	if (document.all)
		var xMax = screen.width, yMax = screen.height;
	else if (document.layers)
		var xMax = window.outerWidth, yMax = window.outerHeight;
	else
	   var xMax = 640, yMax=500;
	
	var xOffset = (xMax - wide)/2;
	var yOffset = (yMax - high)/3;

	var settings = 'width='+wide+',height='+high+',screenX='+xOffset+',screenY='+yOffset+',top='+yOffset+',left='+xOffset+',resizable=yes,toolbar=no,location=no,directories=no,status=no,menubar=no,scrollbars=yes';
	window.open(url, windowName, settings);
}