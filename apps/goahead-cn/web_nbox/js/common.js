function getById(v)
{
	return document.getElementById(v);	
}
function getByName(v)
{
	return document.getElementByName(v);	
}
function blankCheck(v,m)
{	
	if ((v.value.length==0) || (v.value.indexOf(" ") >= 0)){ 
		alert(m+JS_msg58); 
		v.value=v.defaultValue;
		v.focus();
		return 0; 
	}
	return 1;
}
function hostnameCheck(v,m)
{	
	if (v.value.indexOf(" ") >= 0){ 
		alert(m+JS_msg47); 
		v.value=v.defaultValue;
		v.focus();
		return 0; 
	}
	return 1;
}
function ssidCheck(v,m)
{
	if (v.value.length==0){ 
		alert(m+JS_msg58); 
		v.value=v.defaultValue;
		v.focus();
		return 0; 
	}
	return 1;
}
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
function stringCheck2(v,m)
{
	var t = /[;,\"\'\\\~\`\+\|\^\r\n\<\>\(\)\[\]\{\}]{1,}/;
	if (t.test(v.value)){
		alert(m+JS_msg59);
		v.value=v.defaultValue;
		v.focus();
		return 0;
	}
	return 1;
}
function urlCheck(I,m)
{
	var v=I.value;
	for (var i=0; i<v.length; i++) {
    	if ( (v.charAt(i) >= '0' && v.charAt(i) <= '9') ||
			(v.charAt(i) >= 'a' && v.charAt(i) <= 'z') || (v.charAt(i) >= 'A' && v.charAt(i) <= 'Z') || 
			(v.charAt(i) == '-') || (v.charAt(i) == '_') || (v.charAt(i) == '.'))
			continue;
		
		alert(m+JS_msg60);
		I.value=I.defaultValue;
		I.focus();
		return 0;
  	}
  	return 1;
}
function numCheck(v) 
{
	var t = /[^0-9]{1,}/;
	if (t.test(v.value)){
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
function numCheckNullMsg(v,m) 
{
	if (!blankCheck(v,m)) return 0;
	var t = /[^0-9]{1,}/;
	if (t.test(v.value)){
		alert(m+JS_msg61);
		v.value=v.defaultValue;
		v.focus();
		return 0;
	}
	return 1;
}
function rangeCheck(v,a,b,m) 
{
   if (!numCheckMsg(v,m)) return 0;          
   if ((parseInt(v.value)<a)||(parseInt(v.value)>b)){	
      	alert(m+JS_msg62+a+"-"+b+"!") ;
      	v.value=v.defaultValue;
		v.focus();
      	return 0;
   } 
   return 1;
}
function rangeCheckNullMsg(v,a,b,m) 
{
   if (!blankCheck(v,m)) return 0;
   if (!numCheckMsg(v,m)) return 0;          
   if ((parseInt(v.value)<a)||(parseInt(v.value)>b)){	
      	alert(m+JS_msg62+a+"-"+b+"!") ;
      	v.value=v.defaultValue;
		v.focus();
      	return 0;
   } 
   return 1;
}
function portCheck(v) 
{
   if (!numCheck(v)) return 0;          
   if ((parseInt(v.value)<1)||(parseInt(v.value)>65535)){	
      	v.value=v.defaultValue;
		v.focus();
      	return 0;
   } 
   return 1;
}
function portCheckMsg(v,m) 
{
   if (!numCheckMsg(v,m)) return 0;          
   if ((parseInt(v.value)<1)||(parseInt(v.value)>65535)){	
      	alert(m+JS_msg63) ;
      	v.value=v.defaultValue;
		v.focus();
      	return 0;
   } 
   return 1;
}
function portCheckNullMsg(v,m) 
{
   if (!blankCheck(v,m)) return 0;
   if (!numCheckMsg(v,m)) return 0;          
   if ((parseInt(v.value)<1)||(parseInt(v.value)>65535)){	
      	alert(m+JS_msg63) ;
      	v.value=v.defaultValue;
		v.focus();
      	return 0;
   } 
   return 1;
}
function portRangeCheck(a,b) 
{
   if (parseInt(a) > parseInt(b)) return 0;
   return 1;
}
function hexCheck(v,m) 
{
	var t = /[^0-9a-fA-F]{1,}/;
	if (t.test(v.value)){
		alert(m+JS_msg64) ;
		v.value=v.defaultValue;
		v.focus();
		return 0;
	}
	return 1;
}
function macCheck(I,m)
{
	var t = /[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}/;
	if ((I.value.length != 17)||(!t.test(I.value))){
		alert(JS_msg65+m+"!");
		I.value = I.defaultValue;
		I.focus();
		return 0;
	}
	if (I.value.toUpperCase()=="FF:FF:FF:FF:FF:FF"){
		alert(m+JS_msg66);
		I.value = I.defaultValue;
		I.focus();
		return 0;
	}	
	for (var i=0; i<I.value.length; i++){
		if ((I.value.charAt(1)&0x01)||
			(I.value.charAt(1).toUpperCase()=='B')||
			(I.value.charAt(1).toUpperCase()=='D')||
			(I.value.charAt(1).toUpperCase()=='F')){
			alert(m+JS_msg67);
			I.value = I.defaultValue;
			I.focus();
			return 0;
		}
	}	
	return 1;
}
function trimCheck(str)
{
	var v=str.replace(/(^\s*)|(\s*$)/g, "");
	//alert(v+"@@@");
	return v;
}
function ipCheck(I,m,subnet)
{
	ip = new Array();
	ip=trimCheck(I.value).split(".");
	
	if (ip.length!=4){	
		alert(JS_msg65+m+"!"); 
		I.value = I.defaultValue;
		I.focus(); 
		return 0; 
	}
    for (var j=0; j<4; j++){
        d = ip[j];
        if ((d<=255) && (d>=0)){
			if (j!=3 || subnet==1)	continue;
			else {	if ((d!=255) && (d!=0)) continue;	}
        }	
        alert(JS_msg65+m+"!");
		I.value = I.defaultValue;
		I.focus();
        return 0;
    }
    return 1;
}
function maskCheck(I,m)
{
	var mn = new Array();
	if (I.length==4)
		for (i=0;i<4;i++) mn[i]=I[i].value;
	else{
		mn=I.value.split('.');
		if (mn.length!=4){ 
			alert(JS_msg65+m+"!"); I.value = I.defaultValue;
			I.focus(); 
			return 0; 
		}
	}

	var t = /[^0-9]{1,}/;
	for (var i=0;i<4;i++){
		if (t.test(mn[i])||mn[i]>255){ 
			alert(JS_msg65+m+"!"); 
			I.value = I.defaultValue;
			I.focus(); 
			return 0; 
		}
	}

	var v=(mn[0]<<24)|(mn[1]<<16)|(mn[2]<<8)|(mn[3]);
	var f=0;	  
	for (k=0;k<32;k++){
		if ((v>>k)&1) f=1;
		else if (f==1){ 
			alert(JS_msg65+m+"!"); 
			I.value = I.defaultValue;
			I.focus(); 
			return 0; 
		}
	}
	if (f==0){ 
		alert(JS_msg65+m+"!");
		I.value = I.defaultValue;
		I.focus(); 
		return 0; 
	}
	return 1;	
}
function ipv6AddrCheck(I, len, m)
{
	var ip_item = new Array();
	ip_item = I.value.split(":");
	for (var i=0; i<ip_item.length; i++) {
		if (parseInt(ip_item[i], 16) == "NaN") {
			alert(m+JS_msg68);
			I.value = I.defaultValue;
			I.focus();
			return 0;
		}
	}
	return 1;
}
function ipAndUrlCheckMsg(I,m)
{
	if (!urlCheck(I,m) && !ipCheck(I,m,0)) return 0;
	return 1;
}
function commentCheck(value)
{
	var len = 0;
	/*
	if (value.length > 10){
		alert(JS_msg190);
		return 0;
	}
	*/
	for (var i=0; i<=value.length; i++){	
		if( (/[\u4e00-\u9fa5]+/).test(value.charAt(i)) ){
			len += 2;
			continue;
		}
		len++;
	}	
	if (len > 11){
		alert(JS_msg185);
	    return 0;
	}
	return 1;
}
function atoi(str, num)
{
    i=1;
    if(num!=1){
        while (i!=num && str.length!=0){
            if(str.charAt(0)=='.') i++;
            str = str.substring(1);
        }
        if(i!=num) return -1;
    }
    for(i=0; i<str.length; i++){
        if(str.charAt(i)=='.'){ str=str.substring(0, i); break; }
    }
    if(str.length==0) return -1;
    return parseInt(str, 10);
}
function subnetCheck(ip, mask, client)
{
  	ip_d = atoi(ip, 1);
  	mask_d = atoi(mask, 1);
  	client_d = atoi(client, 1);
  	if ( (ip_d & mask_d) != (client_d & mask_d ) )	return 0;

  	ip_d = atoi(ip, 2);
  	mask_d = atoi(mask, 2);
  	client_d = atoi(client, 2);
  	if ( (ip_d & mask_d) != (client_d & mask_d ) )	return 0;

  	ip_d = atoi(ip, 3);
  	mask_d = atoi(mask, 3);
  	client_d = atoi(client, 3);
  	if ( (ip_d & mask_d) != (client_d & mask_d ) )	return 0;

  	ip_d = atoi(ip, 4);
  	mask_d = atoi(mask, 4);
  	client_d = atoi(client, 4);
  	if ( (ip_d & mask_d) != (client_d & mask_d ) )	return 0;
  	return 1;
}
function clientRangeCheck(start,end)
{
	var start_d, end_d;
	start_d = atoi(start,4);
	start_d += atoi(start,3)*256;
	start_d += atoi(start,2)*256;
	start_d += atoi(start,1)*256;
	
	end_d = atoi(end,4);
	end_d += atoi(end,3)*256;
	end_d += atoi(end,2)*256;
	end_d += atoi(end,1)*256;
	
	if ( start_d > end_d )	return 0;
	return 1;
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
function skip () { this.blur(); }
function disableTextField (field) 
{
  	if (document.all || document.getElementById)
    	field.disabled = true;
  	else {
    	field.oldOnFocus = field.onfocus;
    	field.onfocus = skip;
  	}
}
function enableTextField (field) 
{
  	if (document.all || document.getElementById)
    	field.disabled = false;
  	else
    	field.onfocus = field.oldOnFocus;
}
function disableButton (button) 
{
  	if (document.all || document.getElementById)
    	button.disabled = true;
  	else if (button) {
		button.oldOnClick = button.onclick;
		button.onclick = null;
		button.oldValue = button.value;
		button.value = 'DISABLED';
  	}
}
function enableButton (button) 
{
  	if (document.all || document.getElementById)
    	button.disabled = false;
  	else if (button) {
    	button.onclick = button.oldOnClick;
    	button.value = button.oldValue;
  	}
}
//------------------------------------------------------------------
function Cfg(i,n,v)
{
	this.i=i;
    this.n=n;
    this.v=this.o=v;
}
var CA = new Array();
function addCfg(n,i,v)
{
	CA.length++;
    CA[CA.length-1]= new Cfg(i,n,v);
}
function idxOfCfg(kk)
{
    if (kk=='undefined') { alert("Undefined"); return -1; }
    for (var i=0; i< CA.length ;i++){
        if ( CA[i].n != 'undefined' && CA[i].n==kk ) return i;
    }
    return -1;
}
function getCfg(n)
{
	var idx=idxOfCfg(n)
	if (idx >=0) return CA[idx].v;
	else return "";
}
function cfg2Form(f)
{
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