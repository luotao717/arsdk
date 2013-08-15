<html>
<! Copyright (c) Kingnet Electronical Corp., 2003. All Rights Reserved. ->
<head>
<meta http-equiv="Content-Type" content="text/html">
<title>WAN Interface Setup </title>
<LINK href="css/global.css" type=text/css rel=stylesheet>
<form action=/goform/formOnekeySetup method=POST name="tcpip">
<table border="0" width=480>
    <tr>
       <td width="30%"><font size=2><b>IP Address:</b></td>
       <td width="70%"><font size=2>
        <input type="text" name="staticIp" size="18" maxlength="15" value="">
		
	        </td>
    </tr>
    <tr>
      <td width="30%"><font size=2><b>Subnet Mask:</b></td>
      <td width="70%"><font size=2><input type="text" name="staticNetmask" size="18" maxlength="15" value=""></td>
    </tr>
    <tr>
      <td width="30%"><font size=2><b>Default Gateway:</b></td>
      <td width="70%"><font size=2><input type="text" name="staticGateway" size="18" maxlength="15" value=""></td>
	  
    </tr>
    <tr>
      <td width="30%"><font size=2><b>DNS1:</b></td>
      <td width="70%"><font size=2><input type="text" name="staticPriDns" size="18" maxlength="15" value=""></td>
	  
    </tr>
	<tr><td><input type="text" name="connectionType" size="18" maxlength="15" value="wantype">
	  <input type="text" name="radiohiddenButton" size="18" maxlength="15" value="radioswith">
		<input type="text" name="mssid" size="18" maxlength="15" value="ssid">
		<input type="text" name="sz11gChannel" size="18" maxlength="15" value="channel">
		<input type="text" name="security_mode" size="18" maxlength="15" value="SHARED">
		<input type="text" name="wep_key_1" size="18" maxlength="15" value="12345">
		<input type="submit" value=" Apply " name="save" class="actButton"></td></tr>
  </table>  
  </form>