<html>
<head>
<title></title>
<meta http-equiv="Content-Type" content="text/html; charset=<% getCharset(); %>"> 
<link href="<% getStyle(); %>" rel="stylesheet" type="text/css">
<script language="javascript" src="js/language_<% getCfgZero(1, "LanguageType"); %>.js"></script>
<script type="text/javascript">
function getNavigator()      
{      
    var navigatorType = navigator.userAgent;
	if (navigatorType.indexOf("MSIE")>0)  return "MSIE";//IE       
	if (navigatorType.indexOf("Firefox")>0)  return "Firefox";//Firefox       
	if (navigatorType.indexOf("Safari")>0)  return "Safari";//Safan       
	if (navigatorType.indexOf("Chrome")>0)  return "Chrome";//Camino       
	if (navigatorType.indexOf("Gecko")>0)  return "Gecko";//Gecko 
	if (navigatorType.indexOf("Opera")>0)  return "Opera";//Opera 
	if (navigatorType.indexOf("AppWebKit")>0)  return "AppWebKit";//AppWebKit  
}

var http_request = false;
function changeLanguage(value)
{
    http_request = false;
	var navigatorType = getNavigator(); 
    if (navigatorType == "MSIE"){   
        http_request = new ActiveXObject("Microsoft.XMLHTTP");  //IE
    }
	else{   
        http_request = new XMLHttpRequest();   
    }   
    http_request.onreadystatechange = doChange;
	http_request.open('POST', '/goform/setLanguage', true);
	http_request.send('langType='+value);
}

function doChange()
{
    if (http_request.readyState == 4){
		if (http_request.status == 200){
			top.location.reload();
		} else {
			//alert(JS_msg6);
		}
	}
}

function updateLanguage(val)
{
	changeLanguage(val);
}

function Load_Setting()
{
	var default_language = "<% getLangBuilt(); %>";
	var langType = "<% getCfgZero(1, "LanguageType"); %>";
	
	if (default_language == "1")
		document.getElementById("lang").style.display = "";
	else
		document.getElementById("lang").style.display = "none";
	
	if (langType == "cn")	
		document.getElementById("cn").className = 'title_link_select';
	else
		document.getElementById("en").className = 'title_link_select';
}
</script>
</head>
<body onLoad="Load_Setting()">
<form name="langCfg">
<input type="hidden" name="langType">
<input type="hidden" name="submit-url" value="/title.asp">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <tr>
    <td class="title_down_left"><% getProductModel(); %></td>
    <td class="title_down_center">&nbsp;</td>
    <td class="title_down_right" align="right"><span id="lang"><b><script>dw(MM_language)</script>:</b>&nbsp;&nbsp;<a href="javascript:updateLanguage('cn')" id="cn" class="title_link"><script>dw(MM_chinese)</script></a>&nbsp;&nbsp;|&nbsp;&nbsp;<a href="javascript:updateLanguage('en')" id="en" class="title_link"><script>dw(MM_english)</script></a></span>&nbsp;&nbsp;&nbsp;&nbsp;</td>
  </tr>
</table>
</form>
</body>
</html>
