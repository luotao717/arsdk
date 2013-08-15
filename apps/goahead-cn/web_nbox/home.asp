<html>
<head>
<title><% getTitle(); %></title>
<meta http-equiv="Content-Type" content="text/html; charset=<% getCharset(); %>"> 
<link href="<% getStyle(); %>" rel="stylesheet" type="text/css">
<link rel="shortcut icon" href="<% getFavicon(); %>">
</head>

<frameset cols="*,1002,*" frameborder="no" border="0" framespacing="0">
  <frame src="empty.htm" name="empty1" scrolling="No" noresize="noresize" />
  <frameset rows="66,*" cols="*" frameborder="NO" border="0" framespacing="0">
		<frame src="top.asp" name="top" scrolling="NO" noresize>
		<frameset cols="224,*" frameborder="NO" border="0" framespacing="0">
			<frame src="left.asp" name="menu" scrolling="NO" noresize>
			<frame src="adm/status.asp" name="view" style="border-right:1px solid #eceded" />
		</frameset>
	</frameset>
  <frame src="empty.htm" name="empty2" scrolling="No" noresize="noresize" />
</frameset>
<noframes><body>
</body>
</noframes></html>
