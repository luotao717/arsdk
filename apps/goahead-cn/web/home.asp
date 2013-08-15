<html>
<head>
<title><% getTitle(); %></title>
<meta http-equiv="Content-Type" content="text/html; charset=<% getCharset(); %>" />
<link href="images/kingnet.css" type="text/css" rel="stylesheet">
</head>

<frameset rows="100,*" cols="*" frameborder="no" border="0" framespacing="0">
  <frame src="top1.asp" name="topFrame" scrolling="No" noresize="noresize" id="topFrame" title="topFrame" />
  <frameset cols="172,32,*" frameborder="no" border="0" framespacing="0">
  	<frame src="<% ShowLeftItem(); %>" name="leftFrame" scrolling="auto" noresize="noresize" id="leftFrame" title="leftFrame" />
    <frame src="arc.html" name="arcFrame" scrolling="No" noresize="noresize" id="arcFrame" title="arcFrame" />
	<frame src="adm/status.asp" name="mainFrame" id="mainFrame" title="mainFrame" />	  
</frameset></frameset>
<noframes><body>
</body>

</noframes></html>
