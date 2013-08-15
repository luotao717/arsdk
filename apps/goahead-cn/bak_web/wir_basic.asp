<html>
<head>
<title>WR922</title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<link href="images/kingnet.css" type="text/css" rel="stylesheet">
<SCRIPT src="helpScript.js"></SCRIPT>
<script language="JavaScript" type="text/javascript">
var PhyMode  = '<% getCfgZero(1, "WirelessMode"); %>';
var broadcastssidEnable  = '<% getCfgZero(1, "HideSSID"); %>';
var apisolated = '<% getCfgZero(1, "NoForwarding"); %>';
var mbssidapisolated = '<% getCfgZero(1, "NoForwardingBTNBSSID"); %>';
var channel_index  = '<% getWlanChannel(); %>';
var wdsMode  = '<% getCfgZero(1, "WdsEnable"); %>';
var wdsList  = '<% getCfgGeneral(1, "WdsList"); %>';
var wdsPhyMode  = '<% getCfgZero(1, "WdsPhyMode"); %>';
var wdsEncrypType  = '<% getWlanWdsEncType(); %>';
var wdsEncrypKey  = '<% getCfgGeneral(1, "WdsKey"); %>';
var countrycode = '<% getCfgGeneral(1, "CountryCode"); %>';
var ht_mode = '<% getCfgZero(1, "HT_OpMode"); %>';
var ht_bw = '<% getCfgZero(1, "HT_BW"); %>';
var ht_gi = '<% getCfgZero(1, "HT_GI"); %>';
var ht_stbc = '<% getCfgZero(1, "HT_STBC"); %>';
var ht_mcs = '<% getCfgZero(1, "HT_MCS"); %>';
var ht_htc = '<% getCfgZero(1, "HT_HTC"); %>';
var ht_rdg = '<% getCfgZero(1, "HT_RDG"); %>';
//var ht_linkadapt = '<% getCfgZero(1, "HT_LinkAdapt"); %>';
var ht_extcha = '<% getCfgZero(1, "HT_EXTCHA"); %>';
var ht_amsdu = '<% getCfgZero(1, "HT_AMSDU"); %>';
var ht_autoba = '<% getCfgZero(1, "HT_AutoBA"); %>';
var ht_badecline = '<% getCfgZero(1, "HT_BADecline"); %>';
var ht_f_40mhz = '<% getCfgZero(1, "HT_40MHZ_INTOLERANT"); %>';
//var wifi_optimum = '<!--#include ssi=getWlanWiFiTest()-->';
//var apcli_include = '<!--#include ssi=getWlanApCliInclude()-->';
//var wifi_optimum = '0';
var apcli_include = '<% getWlanApcliBuilt(); %>';
var tx_stream_idx = '<% getCfgZero(1, "HT_TxStream"); %>';
var rx_stream_idx = '<% getCfgZero(1, "HT_RxStream"); %>';

ChannelList_24G = new Array(14);
ChannelList_24G[0] = "2412MHz (Channel 1)";
ChannelList_24G[1] = "2417MHz (Channel 2)";
ChannelList_24G[2] = "2422MHz (Channel 3)";
ChannelList_24G[3] = "2427MHz (Channel 4)";
ChannelList_24G[4] = "2432MHz (Channel 5)";
ChannelList_24G[5] = "2437MHz (Channel 6)";
ChannelList_24G[6] = "2442MHz (Channel 7)";
ChannelList_24G[7] = "2447MHz (Channel 8)";
ChannelList_24G[8] = "2452MHz (Channel 9)";
ChannelList_24G[9] = "2457MHz (Channel 10)";
ChannelList_24G[10] = "2462MHz (Channel 11)";
ChannelList_24G[11] = "2467MHz (Channel 12)";
ChannelList_24G[12] = "2472MHz (Channel 13)";
ChannelList_24G[13] = "2484MHz (Channel 14)";

ChannelList_5G = new Array(33);
ChannelList_5G[0] = "5180MHz (Channel 36)";
ChannelList_5G[1] = "5200MHz (Channel 40)";
ChannelList_5G[2] = "5220MHz (Channel 44)";
ChannelList_5G[3] = "5240MHz (Channel 48)";
ChannelList_5G[4] = "5260MHz (Channel 52)";
ChannelList_5G[5] = "5280MHz (Channel 56)";
ChannelList_5G[6] = "5300MHz (Channel 60)";
ChannelList_5G[7] = "5320MHz (Channel 64)";
ChannelList_5G[16] = "5500MHz (Channel 100)";
ChannelList_5G[17] = "5520MHz (Channel 104)";
ChannelList_5G[18] = "5540MHz (Channel 108)";
ChannelList_5G[19] = "5560MHz (Channel 112)";
ChannelList_5G[20] = "5580MHz (Channel 116)";
ChannelList_5G[21] = "5600MHz (Channel 120)";
ChannelList_5G[22] = "5620MHz (Channel 124)";
ChannelList_5G[23] = "5640MHz (Channel 128)";
ChannelList_5G[24] = "5660MHz (Channel 132)";
ChannelList_5G[25] = "5680MHz (Channel 136)";
ChannelList_5G[26] = "5700MHz (Channel 140)";
ChannelList_5G[28] = "5745MHz (Channel 149)";
ChannelList_5G[29] = "5765MHz (Channel 153)";
ChannelList_5G[30] = "5785MHz (Channel 157)";
ChannelList_5G[31] = "5805MHz (Channel 161)";
ChannelList_5G[32] = "5825MHz (Channel 165)";

HT5GExtCh = new Array(22);
HT5GExtCh[0] = new Array(1, "5200MHz (Channel 40)"); // channel 36's extension channel
HT5GExtCh[1] = new Array(0, "5180MHz (Channel 36)"); // channel 40's extension channel
HT5GExtCh[2] = new Array(1, "5240MHz (Channel 48)"); // channel 44's extension channel
HT5GExtCh[3] = new Array(0, "5220MHz (Channel 44)"); // channel 48's extension channel
HT5GExtCh[4] = new Array(1, "5280MHz (Channel 56)"); // channel 52's extension channel
HT5GExtCh[5] = new Array(0, "5260MHz (Channel 52)"); // channel 56's extension channel
HT5GExtCh[6] = new Array(1, "5320MHz (Channel 64)"); // channel 60's extension channel
HT5GExtCh[7] = new Array(0, "5300MHz (Channel 60)"); // channel 64's extension channel
HT5GExtCh[8] = new Array(1, "5520MHz (Channel 104)"); // channel 100's extension channel
HT5GExtCh[9] = new Array(0, "5500MHz (Channel 100)"); // channel 104's extension channel
HT5GExtCh[10] = new Array(1, "5560MHz (Channel 112)"); // channel 108's extension channel
HT5GExtCh[11] = new Array(0, "5540MHz (Channel 108)"); // channel 112's extension channel
HT5GExtCh[12] = new Array(1, "5600MHz (Channel 120)"); // channel 116's extension channel
HT5GExtCh[13] = new Array(0, "5580MHz (Channel 116)"); // channel 120's extension channel
HT5GExtCh[14] = new Array(1, "5640MHz (Channel 128)"); // channel 124's extension channel
HT5GExtCh[15] = new Array(0, "5620MHz (Channel 124)"); // channel 128's extension channel
HT5GExtCh[16] = new Array(1, "5680MHz (Channel 136)"); // channel 132's extension channel
HT5GExtCh[17] = new Array(0, "5660MHz (Channel 132)"); // channel 136's extension channel
HT5GExtCh[18] = new Array(1, "5765MHz (Channel 153)"); // channel 149's extension channel
HT5GExtCh[19] = new Array(0, "5745MHz (Channel 149)"); // channel 153's extension channel
HT5GExtCh[20] = new Array(1, "5805MHz (Channel 161)"); // channel 157's extension channel
HT5GExtCh[21] = new Array(0, "5785MHz (Channel 157)"); // channel 161's extension channel

function style_display_on()
{
	if (window.ActiveXObject)
	{ // IE
		return "block";
	}
	else if (window.XMLHttpRequest)
	{ // Mozilla, Safari,...
		return "table-row";
	}
}

function insertChannelOption(vChannel, band)
{
	var y = document.createElement('option');

	if (1*band == 24)
	{
		y.text = ChannelList_24G[1*vChannel - 1];
		y.value = 1*vChannel;
	}
	else if (1*band == 5)
	{
		y.value = 1*vChannel;
		if (1*vChannel <= 140)
			y.text = ChannelList_5G[((1*vChannel) - 36) / 4];
		else
			y.text = ChannelList_5G[((1*vChannel) - 36 - 1) / 4];
	}

	if (1*band == 24)
		var x=document.getElementById("sz11gChannel");
	else if (1*band == 5)
		var x=document.getElementById("sz11aChannel");

	try
	{
		x.add(y,null); // standards compliant
	}
	catch(ex)
	{
		x.add(y); // IE only
	}
}

function CreateExtChannelOption(vChannel)
{
	var y = document.createElement('option');

	y.text = ChannelList_24G[1*vChannel - 1];
//	y.value = 1*vChannel;
	y.value = 1;

	var x = document.getElementById("n_extcha");

	try
	{
		x.add(y,null); // standards compliant
	}
	catch(ex)
	{
		x.add(y); // IE only
	}
}

function insertExtChannelOption()
{
	var wmode = document.wireless_basic.wirelessmode.options.selectedIndex;
	var option_length; 
	var CurrentCh;

	if ((1*wmode == 3))
	//if ((1*wmode == 4) || (1*wmode == 5))
	{
		var x = document.getElementById("n_extcha");
		var length = document.wireless_basic.n_extcha.options.length;

		if (length > 1)
		{
			x.selectedIndex = 1;
			x.remove(x.selectedIndex);
		}

		if (1*wmode == 4)
		{
			CurrentCh = document.wireless_basic.sz11aChannel.value;

			if ((1*CurrentCh >= 36) && (1*CurrentCh <= 64))
			{
				CurrentCh = 1*CurrentCh;
				CurrentCh /= 4;
				CurrentCh -= 9;

				x.options[0].text = HT5GExtCh[CurrentCh][1];
				x.options[0].value = HT5GExtCh[CurrentCh][0];
			}
			else if ((1*CurrentCh >= 100) && (1*CurrentCh <= 136))
			{
				CurrentCh = 1*CurrentCh;
				CurrentCh /= 4;
				CurrentCh -= 17;

				x.options[0].text = HT5GExtCh[CurrentCh][1];
				x.options[0].value = HT5GExtCh[CurrentCh][0];
			}
			else if ((1*CurrentCh >= 149) && (1*CurrentCh <= 161))
			{
				CurrentCh = 1*CurrentCh;
				CurrentCh -= 1;
				CurrentCh /= 4;
				CurrentCh -= 19;

				x.options[0].text = HT5GExtCh[CurrentCh][1];
				x.options[0].value = HT5GExtCh[CurrentCh][0];
			}
			else
			{
				x.options[0].text = "自动选择";
				x.options[0].value = 0;
			}
		}
		else if (1*wmode == 3)
		{
			CurrentCh = document.wireless_basic.sz11gChannel.value;
			option_length = document.wireless_basic.sz11gChannel.options.length;

			if ((CurrentCh >=1) && (CurrentCh <= 4))
			{
				x.options[0].text = ChannelList_24G[1*CurrentCh + 4 - 1];
				x.options[0].value = 1*CurrentCh + 4;
			}
			else if ((CurrentCh >= 5) && (CurrentCh <= 7))
			{
				x.options[0].text = ChannelList_24G[1*CurrentCh - 4 - 1];
				x.options[0].value = 0; //1*CurrentCh - 4;
				CurrentCh = 1*CurrentCh;
				CurrentCh += 4;
				CreateExtChannelOption(CurrentCh);
			}
			else if ((CurrentCh >= 8) && (CurrentCh <= 9))
			{
				x.options[0].text = ChannelList_24G[1*CurrentCh - 4 - 1];
				x.options[0].value = 0; //1*CurrentCh - 4;

				if (option_length >=14)
				{
					CurrentCh = 1*CurrentCh;
					CurrentCh += 4;
					CreateExtChannelOption(CurrentCh);
				}
			}
			else if (CurrentCh == 10)
			{
				x.options[0].text = ChannelList_24G[1*CurrentCh - 4 - 1];
				x.options[0].value = 0; //1*CurrentCh - 4;

				if (option_length > 14)
				{
					CurrentCh = 1*CurrentCh;
					CurrentCh += 4;
					CreateExtChannelOption(CurrentCh);
				}
			}
			else if (CurrentCh >= 11)
			{
				x.options[0].text = ChannelList_24G[1*CurrentCh - 4 - 1];
				x.options[0].value = 0; //1*CurrentCh - 4;
			}
			else
			{
				x.options[0].text = "自动选择";
				x.options[0].value = 0;
			}
		}
	}
}

function ChannelOnChange()
{
	if (document.wireless_basic.n_bandwidth[1].checked == true)
	{
		var w_mode = document.wireless_basic.wirelessmode.options.selectedIndex;

		if (1*w_mode == 4)
		{
			if (document.wireless_basic.n_bandwidth[1].checked == true)
			{
				document.getElementById("extension_channel").style.visibility = "visible";
				document.getElementById("extension_channel").style.display = style_display_on();
				document.wireless_basic.n_extcha.disabled = false;
			}

			if (document.wireless_basic.sz11aChannel.options.selectedIndex == 0)
			{
				document.getElementById("extension_channel").style.visibility = "hidden";
				document.getElementById("extension_channel").style.display = "none";
				document.wireless_basic.n_extcha.disabled = true;
			}
		}
		else if (1*w_mode == 5)
		{
			if (document.wireless_basic.n_bandwidth[1].checked == true)
			{
				document.getElementById("extension_channel").style.visibility = "visible";
				document.getElementById("extension_channel").style.display = style_display_on();
				document.wireless_basic.n_extcha.disabled = false;
			}

			if (document.wireless_basic.sz11gChannel.options.selectedIndex == 0)
			{
				document.getElementById("extension_channel").style.visibility = "hidden";
				document.getElementById("extension_channel").style.display = "none";
				document.wireless_basic.n_extcha.disabled = true;
			}
		}
	}

	insertExtChannelOption();
}

function Channel_BandWidth_onClick()
{
	var w_mode = document.wireless_basic.wirelessmode.options.selectedIndex;

	if (document.wireless_basic.n_bandwidth[0].checked == true)
	{
		document.getElementById("extension_channel").style.visibility = "hidden";
		document.getElementById("extension_channel").style.display = "none";
		document.wireless_basic.n_extcha.disabled = true;
		if (1*w_mode == 4)
			Check5GBandChannelException();
	}
	else
	{
		document.getElementById("extension_channel").style.visibility = "visible";
		document.getElementById("extension_channel").style.display = style_display_on();
		document.wireless_basic.n_extcha.disabled = false;

		if (1*w_mode == 4)
		{
			Check5GBandChannelException();

			if (document.wireless_basic.sz11aChannel.options.selectedIndex == 0)
			{
				document.getElementById("extension_channel").style.visibility = "hidden";
				document.getElementById("extension_channel").style.display = "none";
				document.wireless_basic.n_extcha.disabled = true;
			}
		}
	}
}

function Check5GBandChannelException()
{
	var w_mode = document.wireless_basic.wirelessmode.options.selectedIndex;

	if (1*w_mode == 4)
	{
		var x = document.getElementById("sz11aChannel")
		var current_length = document.wireless_basic.sz11aChannel.options.length;
		var current_index = document.wireless_basic.sz11aChannel.options.selectedIndex;
		var current_channel = document.wireless_basic.sz11aChannel.value;
		 
		if (1*current_index == 0)
		{
			if (1*channel_index != 0)
				current_index = 1;
		}

		for (ch_idx = current_length - 1; ch_idx > 0; ch_idx--)
		{
			x.remove(ch_idx);
		}

		if (document.wireless_basic.n_bandwidth[1].checked == true)
		{
			if ((countrycode == 'NONE') || (countrycode == 'FR') || (countrycode == 'US') ||
				(countrycode == 'IE') || (countrycode == 'JP') || (countrycode == 'HK'))
			{
				for(ch = 36; ch <= 48; ch+=4)
					insertChannelOption(ch, 5);
			}

			if ((countrycode == 'NONE') || (countrycode == 'FR') || (countrycode == 'US') ||
				(countrycode == 'IE') || (countrycode == 'TW') || (countrycode == 'HK'))
			{
				for(ch = 52; ch <= 64; ch+=4)
					insertChannelOption(ch, 5);
			}

			if (countrycode == 'NONE')
			{
				for(ch = 100; ch <= 136; ch+=4)
					insertChannelOption(ch, 5);
			}

			if ((countrycode == 'NONE') || (countrycode == 'US') || (countrycode == 'TW') ||
				(countrycode == 'CN') || (countrycode == 'HK'))
			{
				for(ch = 149; ch <= 161; ch+=4)
					insertChannelOption(ch, 5);
			}

			if ((1*current_channel == 140) || (1*current_channel == 165))
			{
				document.wireless_basic.sz11aChannel.options.selectedIndex = (1*current_index) -1;
			}
			else
			{
				document.wireless_basic.sz11aChannel.options.selectedIndex = (1*current_index);
			}
		}
		else
		{
			if ((countrycode == 'NONE') || (countrycode == 'FR') || (countrycode == 'US') ||
				(countrycode == 'IE') || (countrycode == 'JP') || (countrycode == 'HK'))
			{
				for(ch = 36; ch <= 48; ch+=4)
					insertChannelOption(ch, 5);
			}

			if ((countrycode == 'NONE') || (countrycode == 'FR') || (countrycode == 'US') ||
				(countrycode == 'IE') || (countrycode == 'TW') || (countrycode == 'HK'))
			{
				for(ch = 52; ch <= 64; ch+=4)
					insertChannelOption(ch, 5);
			}

			if (countrycode == 'NONE')
			{
				for(ch = 100; ch <= 140; ch+=4)
					insertChannelOption(ch, 5);
			}

			if ((countrycode == 'NONE') || (countrycode == 'US') || (countrycode == 'TW') ||
				(countrycode == 'CN') || (countrycode == 'HK'))
			{
				for(ch = 149; ch <= 161; ch+=4)
					insertChannelOption(ch, 5);
			}

			if ((countrycode == 'NONE') || (countrycode == 'US') ||
				(countrycode == 'CN') || (countrycode == 'HK'))
			{
					insertChannelOption(165, 5);
			}

			document.wireless_basic.sz11aChannel.options.selectedIndex = (1*current_index);
		}
	}
	else if (1*w_mode == 3)
	{
		var x = document.getElementById("sz11aChannel")
		var current_length = document.wireless_basic.sz11aChannel.options.length;
		var current_index = document.wireless_basic.sz11aChannel.options.selectedIndex;

		for (ch_idx = current_length - 1; ch_idx > 0; ch_idx--)
		{
			x.remove(ch_idx);
		}

		if ((countrycode == 'NONE') || (countrycode == 'FR') || (countrycode == 'US') ||
			(countrycode == 'IE') || (countrycode == 'JP') || (countrycode == 'HK'))
		{
			for(ch = 36; ch <= 48; ch+=4)
				insertChannelOption(ch, 5);
		}

		if ((countrycode == 'NONE') || (countrycode == 'FR') || (countrycode == 'US') ||
			(countrycode == 'IE') || (countrycode == 'TW') || (countrycode == 'HK'))
		{
			for(ch = 52; ch <= 64; ch+=4)
				insertChannelOption(ch, 5);
		}

		if (countrycode == 'NONE')
		{
			for(ch = 100; ch <= 140; ch+=4)
				insertChannelOption(ch, 5);
		}

		if ((countrycode == 'NONE') || (countrycode == 'US') || (countrycode == 'TW') ||
			(countrycode == 'CN') || (countrycode == 'HK'))
		{
			for(ch = 149; ch <= 161; ch+=4)
				insertChannelOption(ch, 5);
		}

		if ((countrycode == 'NONE') || (countrycode == 'US') ||
			(countrycode == 'CN') || (countrycode == 'HK'))
		{

				insertChannelOption(165, 5);
		}

		document.wireless_basic.sz11aChannel.options.selectedIndex = (1*current_index);
	}
}

function initValue()
{
	var ssidArray;
	var wdslistArray;
	var broadcastssidArray;
	var channel_11a_index;
	var current_channel_length;
	var radio_off = '<% getCfgZero(1, "RadioOff"); %>';

	if (countrycode == '')
		countrycode = 'NONE';

	document.getElementById("div_11a_channel").style.visibility = "hidden";
	document.getElementById("div_11a_channel").style.display = "none";
	document.wireless_basic.sz11aChannel.disabled = true;
	document.getElementById("div_11b_channel").style.visibility = "hidden";
	document.getElementById("div_11b_channel").style.display = "none";
	document.wireless_basic.sz11bChannel.disabled = true;
	document.getElementById("div_11g_channel").style.visibility = "hidden";
	document.getElementById("div_11g_channel").style.display = "none";
	document.wireless_basic.sz11gChannel.disabled = true;
	document.getElementById("div_11n").style.visibility = "hidden";
	document.getElementById("div_11n").style.display = "none";	
	document.wireless_basic.n_mode.disabled = true;
	document.wireless_basic.n_bandwidth.disabled = true;
	document.wireless_basic.n_rdg.disabled = true;
	document.wireless_basic.n_gi.disabled = true;
	document.wireless_basic.n_mcs.disabled = true;
	//document.getElementById("div_11n_plugfest").style.display = "none";
	//document.wireless_basic.f_40mhz.disabled = true;

	PhyMode = 1*PhyMode;

	if (PhyMode >= 8)
	{
		if (window.ActiveXObject) // IE
			document.getElementById("div_11n").style.display = "block";
		else if (window.XMLHttpRequest)  // Mozilla, Safari,...
			document.getElementById("div_11n").style.display = "table";
		document.wireless_basic.n_mode.disabled = false;
		document.wireless_basic.n_bandwidth.disabled = false;
		document.wireless_basic.n_rdg.disabled = false;
		document.wireless_basic.n_gi.disabled = false;
		document.wireless_basic.n_mcs.disabled = false;
		//document.getElementById("div_11n_plugfest").style.display = "block";
		//document.wireless_basic.f_40mhz.disabled = false;
	}

	if ((PhyMode == 0) || (PhyMode == 4) || (PhyMode == 9))
	{
		if (PhyMode == 0)
			document.wireless_basic.wirelessmode.options.selectedIndex = 0;
		else if (PhyMode == 4)
			document.wireless_basic.wirelessmode.options.selectedIndex = 2;
		else if (PhyMode == 9)
			document.wireless_basic.wirelessmode.options.selectedIndex = 3;

		document.getElementById("div_11g_channel").style.visibility = "visible";
		document.getElementById("div_11g_channel").style.display = style_display_on();
		document.wireless_basic.sz11gChannel.disabled = false;
	}
	else if (PhyMode == 1)
	{
		document.wireless_basic.wirelessmode.options.selectedIndex = 1;
		document.getElementById("div_11b_channel").style.visibility = "visible";
		document.getElementById("div_11b_channel").style.display = style_display_on();
		document.wireless_basic.sz11bChannel.disabled = false;
	}
	else if ((PhyMode == 2) || (PhyMode == 8))
	{
		if (PhyMode == 2)
			document.wireless_basic.wirelessmode.options.selectedIndex = 3;
		else if (PhyMode == 8)
			document.wireless_basic.wirelessmode.options.selectedIndex = 4;
		document.getElementById("div_11a_channel").style.visibility = "visible";
		document.getElementById("div_11a_channel").style.display = style_display_on();
		document.wireless_basic.sz11aChannel.disabled = false;
	}

	broadcastssidArray = broadcastssidEnable.split(";");

	if (1*broadcastssidArray[0] == 0)
		document.wireless_basic.broadcastssid[0].checked = true;
	else
		document.wireless_basic.broadcastssid[1].checked = true;

	if (1*ht_bw == 0)
	{
		document.wireless_basic.n_bandwidth[0].checked = true;
		document.getElementById("extension_channel").style.visibility = "hidden";
		document.getElementById("extension_channel").style.display = "none";
		document.wireless_basic.n_extcha.disabled = true;
	}
	else
	{
		document.wireless_basic.n_bandwidth[1].checked = true;
		document.getElementById("extension_channel").style.visibility = "visible";
		document.getElementById("extension_channel").style.display = style_display_on();
		document.wireless_basic.n_extcha.disabled = false;
	}

	channel_index = 1*channel_index;

	if ((PhyMode == 0) || (PhyMode == 4) || (PhyMode == 9))
	{
		document.wireless_basic.sz11gChannel.options.selectedIndex = channel_index;

		current_channel_length = document.wireless_basic.sz11gChannel.options.length;

		if ((channel_index + 1) > current_channel_length)
			document.wireless_basic.sz11gChannel.options.selectedIndex = 0;
	}
	else if (PhyMode == 1)
	{
		document.wireless_basic.sz11bChannel.options.selectedIndex = channel_index;

		current_channel_length = document.wireless_basic.sz11bChannel.options.length;

		if ((channel_index + 1) > current_channel_length)
			document.wireless_basic.sz11bChannel.options.selectedIndex = 0;
	}
	else if ((PhyMode == 2) || (PhyMode == 8))
	{
		if (countrycode == 'NONE')
		{
			if (channel_index <= 64)
			{
				channel_11a_index = channel_index;
				channel_11a_index = channel_11a_index / 4;
				if (channel_11a_index != 0)
					channel_11a_index = channel_11a_index - 8;
			}
			else if ((channel_index >= 100) && (channel_index <= 140))
			{
				channel_11a_index = channel_index;
				channel_11a_index = channel_11a_index / 4;
				channel_11a_index = channel_11a_index - 16;
			}
			else if (channel_index >= 149)
			{
				channel_11a_index = channel_index - 1;
				channel_11a_index = channel_11a_index / 4;
				channel_11a_index = channel_11a_index - 17;

				if (document.wireless_basic.n_bandwidth[1].checked == true)
				{
					channel_11a_index = channel_11a_index - 1;
				}
			}
			else
			{
				channel_11a_index = 0;
			}
		}
		else if ((countrycode == 'US') || (countrycode == 'HK') || (countrycode == 'FR') || (countrycode == 'IE'))
		{
			if (channel_index <= 64)
			{
				channel_11a_index = channel_index;
				channel_11a_index = channel_11a_index / 4;
				if (channel_11a_index != 0)
					channel_11a_index = channel_11a_index - 8;
			}
			else if (channel_index >= 149)
			{
				channel_11a_index = channel_index - 1;
				channel_11a_index = channel_11a_index / 4;
				channel_11a_index = channel_11a_index - 28;
			}
			else
			{
				channel_11a_index = 0;
			}
		}
		else if (countrycode == 'JP')
		{
			if (channel_index <= 48)
			{
				channel_11a_index = channel_index;
				channel_11a_index = channel_11a_index / 4;
				if (channel_11a_index != 0)
					channel_11a_index = channel_11a_index - 8;
			}
			else
			{
				channel_11a_index = 0;
			}
		}
		else if (countrycode == 'TW')
		{
			if (channel_index <= 64)
			{
				channel_11a_index = channel_index;
				channel_11a_index = channel_11a_index / 4;
				if (channel_11a_index != 0)
					channel_11a_index = channel_11a_index - 12;
			}
			else if (channel_index >= 149)
			{
				channel_11a_index = channel_index - 1;
				channel_11a_index = channel_11a_index / 4;
				channel_11a_index = channel_11a_index - 32;
			}
			else
			{
				channel_11a_index = 0;
			}
		}
		else if (countrycode == 'CN')
		{
			if (channel_index >= 149)
			{
				channel_11a_index = channel_index - 1;
				channel_11a_index = channel_11a_index / 4;
				channel_11a_index = channel_11a_index - 36;
			}
			else
			{
				channel_11a_index = 0;
			}
		}
		else
		{
			channel_11a_index = 0;
		}

		Check5GBandChannelException();

		if (channel_index > 0)
			document.wireless_basic.sz11aChannel.options.selectedIndex = channel_11a_index;
		else
			document.wireless_basic.sz11aChannel.options.selectedIndex = channel_index;
	}

	if (wdsList != "")
	{
		wdslistArray = wdsList.split(";");
		for(i = 1; i <= wdslistArray.length; i++)
			eval("document.wireless_basic.wds_"+i).value = wdslistArray[i - 1];
/*
		for (i = wdslistArray.length + 1; i <= 4; i++)
			eval("document.wireless_basic.wds_"+i).value = "00:00:00:00:00:00";
	}
	else
	{
		for(i = 1; i <= 4; i++)
			eval("document.wireless_basic.wds_"+i).value = "00:00:00:00:00:00";
*/
	}

	wdsMode = 1*wdsMode;

	if (wdsMode == 0)
		document.wireless_basic.wds_mode.options.selectedIndex = 0;
	else if (wdsMode == 4)
		document.wireless_basic.wds_mode.options.selectedIndex = 1;
	else if (wdsMode == 2)
		document.wireless_basic.wds_mode.options.selectedIndex = 2;
	else if (wdsMode == 3)
		document.wireless_basic.wds_mode.options.selectedIndex = 3;

	document.wireless_basic.wds_phy_mode.options.selectedIndex = 1*wdsPhyMode;
	document.wireless_basic.wds_encryp_type.options.selectedIndex = 1*wdsEncrypType;
	document.wireless_basic.wds_encryp_key.value = wdsEncrypKey;

	WdsModeOnChange();

	insertExtChannelOption();

	if (1*ht_mode == 0)
	{
		document.wireless_basic.n_mode[0].checked = true;
	}
	else if (1*ht_mode == 1)
	{
		document.wireless_basic.n_mode[1].checked = true;
	}

	else if (1*ht_mode == 2)
	{
		document.wireless_basic.n_mode[2].checked = true;
	}

	if (1*ht_gi == 0)
	{
		document.wireless_basic.n_gi[0].checked = true;
	}
	else if (1*ht_gi == 1)
	{
		document.wireless_basic.n_gi[1].checked = true;
	}
	else if (1*ht_gi == 2)
	{
		document.wireless_basic.n_gi[2].checked = true;
	}

	if (1*ht_mcs <= 15)
		document.wireless_basic.n_mcs.options.selectedIndex = ht_mcs;
	else if (1*ht_mcs == 32)
		document.wireless_basic.n_mcs.options.selectedIndex = 16;
	else if (1*ht_mcs == 33)
		document.wireless_basic.n_mcs.options.selectedIndex = 17;

	if (1*ht_rdg == 0)
		document.wireless_basic.n_rdg[0].checked = true;
	else
		document.wireless_basic.n_rdg[1].checked = true;

	var option_length = document.wireless_basic.n_extcha.options.length;

	if (1*ht_extcha == 0)
	{
		if (option_length > 1)
			document.wireless_basic.n_extcha.options.selectedIndex = 0;
	}
	else if (1*ht_extcha == 1)
	{
		if (option_length > 1)
			document.wireless_basic.n_extcha.options.selectedIndex = 1;
	}
	else
	{
		document.wireless_basic.n_extcha.options.selectedIndex = 0;
	}

	if (1*PhyMode == 8)
	{
		if (document.wireless_basic.sz11aChannel.options.selectedIndex == 0)
		{
			document.getElementById("extension_channel").style.visibility = "hidden";
			document.getElementById("extension_channel").style.display = "none";
			document.wireless_basic.n_extcha.disabled = true;
		}
	}
	else if (1*PhyMode == 9)
	{
		if (document.wireless_basic.sz11gChannel.options.selectedIndex == 0)
		{
			document.getElementById("extension_channel").style.visibility = "hidden";
			document.getElementById("extension_channel").style.display = "none";
			document.wireless_basic.n_extcha.disabled = true;
		}
	}

	if (1*ht_amsdu == 0)
		document.wireless_basic.n_amsdu[0].checked = true;
	else
		document.wireless_basic.n_amsdu[1].checked = true;

	if (1*ht_autoba == 0)
		document.wireless_basic.n_autoba[0].checked = true;
	else
		document.wireless_basic.n_autoba[1].checked = true;

	if (1*ht_badecline == 0)
		document.wireless_basic.n_badecline[0].checked = true;
	else
		document.wireless_basic.n_badecline[1].checked = true;

	//if (1*ht_f_40mhz == 0)
		//document.wireless_basic.f_40mhz[0].checked = true;
	//else
		//document.wireless_basic.f_40mhz[1].checked = true;

	/*
	if (1*wifi_optimum == 0)
		document.wireless_basic.wifi_opt[0].checked = true;
	else
		document.wireless_basic.wifi_opt[1].checked = true;
	*/

	if (1*apcli_include == 1)
	{
		document.wireless_basic.mssid_7.disabled = true;
	}

	document.wireless_basic.rx_stream.options.selectedIndex = rx_stream_idx - 1;
	document.wireless_basic.tx_stream.options.selectedIndex = tx_stream_idx - 1;

	if (1*radio_off != 1)
	{
		document.wireless_basic.radioButton.value = "按钮 关闭";
		document.getElementById("status0").style.visibility = "visible";
		document.getElementById("status0").style.display = style_display_on();
		document.getElementById("status1").style.visibility = "hidden";
	    document.getElementById("status1").style.display = "none";
	}
	else
	{
		document.wireless_basic.radioButton.value = "按钮 启动";
		document.getElementById("status1").style.visibility = "visible";
		document.getElementById("status1").style.display = style_display_on();
		document.getElementById("status0").style.visibility = "hidden";
	    document.getElementById("status0").style.display = "none";
	}
	
	if (apisolated == "1")
		document.wireless_basic.apisolated[0].checked = true;
	else
		document.wireless_basic.apisolated[1].checked = true;
	if (mbssidapisolated == "1")
		document.wireless_basic.mbssidapisolated[0].checked = true;
	else
		document.wireless_basic.mbssidapisolated[1].checked = true;
}

function wirelessModeChange()
{
	var wmode;
   
	document.getElementById("div_11a_channel").style.visibility = "hidden";
	document.getElementById("div_11a_channel").style.display = "none";
	document.wireless_basic.sz11aChannel.disabled = true;
	document.getElementById("div_11b_channel").style.visibility = "hidden";
	document.getElementById("div_11b_channel").style.display = "none";
	document.wireless_basic.sz11bChannel.disabled = true;
	document.getElementById("div_11g_channel").style.visibility = "hidden";
	document.getElementById("div_11g_channel").style.display = "none";
	document.wireless_basic.sz11gChannel.disabled = true;
	document.getElementById("div_11n").style.visibility = "hidden";
	document.getElementById("div_11n").style.display = "none";
	document.wireless_basic.n_mode.disabled = true;
	document.wireless_basic.n_bandwidth.disabled = true;
	document.wireless_basic.n_rdg.disabled = true;
	document.wireless_basic.n_gi.disabled = true;
	document.wireless_basic.n_mcs.disabled = true;
	//document.getElementById("div_11n_plugfest").style.display = "none";
	//document.wireless_basic.f_40mhz.disabled = true;

	wmode = document.wireless_basic.wirelessmode.options.selectedIndex;

	wmode = 1*wmode;
	if (wmode == 0)
	{
		document.wireless_basic.wirelessmode.options.selectedIndex = 0;
		document.getElementById("div_11g_channel").style.visibility = "visible";
		document.getElementById("div_11g_channel").style.display = style_display_on();
		document.wireless_basic.sz11gChannel.disabled = false;
	}
	else if (wmode == 1)
	{
		document.wireless_basic.wirelessmode.options.selectedIndex = 1;
		document.getElementById("div_11b_channel").style.visibility = "visible";
		document.getElementById("div_11b_channel").style.display = style_display_on();
		document.wireless_basic.sz11bChannel.disabled = false;
	}
	else if (wmode == 2)
	{
		document.wireless_basic.wirelessmode.options.selectedIndex = 2;
		document.getElementById("div_11g_channel").style.visibility = "visible";
		document.getElementById("div_11g_channel").style.display = style_display_on();
		document.wireless_basic.sz11gChannel.disabled = false;
	}
	//else if (wmode == 3)
	else if (wmode == 11)
	{
		document.wireless_basic.wirelessmode.options.selectedIndex = 3;
		document.getElementById("div_11a_channel").style.visibility = "visible";
		document.getElementById("div_11a_channel").style.display = style_display_on();
		document.wireless_basic.sz11aChannel.disabled = false;

		Check5GBandChannelException();
	}
	//else if (wmode == 4)
	else if (wmode == 12)
	{
		document.wireless_basic.wirelessmode.options.selectedIndex = 4;
		document.getElementById("div_11a_channel").style.visibility = "visible";
		document.getElementById("div_11a_channel").style.display = style_display_on();
		document.wireless_basic.sz11aChannel.disabled = false;
		if (window.ActiveXObject) // IE
			document.getElementById("div_11n").style.display = "block";
		else if (window.XMLHttpRequest)  // Mozilla, Safari,...
			document.getElementById("div_11n").style.display = "table";
		document.wireless_basic.n_mode.disabled = false;
		document.wireless_basic.n_bandwidth.disabled = false;
		document.wireless_basic.n_rdg.disabled = false;
		document.wireless_basic.n_gi.disabled = false;
		document.wireless_basic.n_mcs.disabled = false;
		//document.getElementById("div_11n_plugfest").style.display = "block";
		//document.wireless_basic.f_40mhz.disabled = false;

		Check5GBandChannelException();

		if (document.wireless_basic.sz11aChannel.options.selectedIndex == 0)
		{
			document.getElementById("extension_channel").style.visibility = "hidden";
			document.getElementById("extension_channel").style.display = "none";
			document.wireless_basic.n_extcha.disabled = true;
		}

		insertExtChannelOption();
	}
	//else if (wmode == 5)
	else if (wmode == 3)
	{
		document.wireless_basic.wirelessmode.options.selectedIndex = 3;
		document.getElementById("div_11g_channel").style.visibility = "visible";
		document.getElementById("div_11g_channel").style.display = style_display_on();
		document.wireless_basic.sz11gChannel.disabled = false;
		if (window.ActiveXObject) // IE
			document.getElementById("div_11n").style.display = "block";
		else if (window.XMLHttpRequest)  // Mozilla, Safari,...
			document.getElementById("div_11n").style.display = "table";
		document.wireless_basic.n_mode.disabled = false;
		document.wireless_basic.n_bandwidth.disabled = false;
		document.wireless_basic.n_rdg.disabled = false;
		document.wireless_basic.n_gi.disabled = false;
		document.wireless_basic.n_mcs.disabled = false;
		//document.getElementById("div_11n_plugfest").style.display = "block";
		//document.wireless_basic.f_40mhz.disabled = false;

		if (document.wireless_basic.sz11gChannel.options.selectedIndex == 0)
		{
			document.getElementById("extension_channel").style.visibility = "hidden";
			document.getElementById("extension_channel").style.display = "none";
			document.wireless_basic.n_extcha.disabled = true;
		}

		insertExtChannelOption();
	}
	

	//WDS Phy Mode
	if (wmode != 1)
		document.wireless_basic.wds_phy_mode.options[1] = new Option("OFDM", "OFDM");
	if (wmode >= 4)
	{
		document.wireless_basic.wds_phy_mode.options[2] = new Option("HTMIX", "HTMIX");
		document.wireless_basic.wds_phy_mode.options[3] = new Option("GREENFIELD", "GREENFIELD");
	}
}

function WdsModeOnChange()
{
	document.getElementById("div_wds_phy_mode").style.visibility = "hidden";
	document.getElementById("div_wds_phy_mode").style.display = "none";
	document.wireless_basic.wds_phy_mode.disabled = true;
	document.getElementById("div_wds_encryp_type").style.visibility = "hidden";
	document.getElementById("div_wds_encryp_type").style.display = "none";
	document.wireless_basic.wds_encryp_type.disabled = true;
	document.getElementById("div_wds_encryp_key").style.visibility = "hidden";
	document.getElementById("div_wds_encryp_key").style.display = "none";
	document.wireless_basic.wds_encryp_key.disabled = true;
	document.getElementById("wds_mac_list_1").style.visibility = "hidden";
	document.getElementById("wds_mac_list_1").style.display = "none";
	document.wireless_basic.wds_1.disabled = true;
	document.getElementById("wds_mac_list_2").style.visibility = "hidden";
	document.getElementById("wds_mac_list_2").style.display = "none";
	document.wireless_basic.wds_2.disabled = true;
	document.getElementById("wds_mac_list_3").style.visibility = "hidden";
	document.getElementById("wds_mac_list_3").style.display = "none";
	document.wireless_basic.wds_3.disabled = true;
	document.getElementById("wds_mac_list_4").style.visibility = "hidden";
	document.getElementById("wds_mac_list_4").style.display = "none";
	document.wireless_basic.wds_4.disabled = true;

	if (document.wireless_basic.wds_mode.options.selectedIndex >= 1)
	{
		document.getElementById("div_wds_phy_mode").style.visibility = "visible";
		document.getElementById("div_wds_phy_mode").style.display = style_display_on();
		document.wireless_basic.wds_phy_mode.disabled = false;
		document.getElementById("div_wds_encryp_type").style.visibility = "visible";
		document.getElementById("div_wds_encryp_type").style.display = style_display_on();
		document.wireless_basic.wds_encryp_type.disabled = false;

		if (document.wireless_basic.wds_encryp_type.options.selectedIndex >= 2)
		{
			document.getElementById("div_wds_encryp_key").style.visibility = "visible";
			document.getElementById("div_wds_encryp_key").style.display = style_display_on();
			document.wireless_basic.wds_encryp_key.disabled = false;
		}
	}

	if (document.wireless_basic.wds_mode.options.selectedIndex >= 2)
	{
		document.getElementById("wds_mac_list_1").style.visibility = "visible";
		document.getElementById("wds_mac_list_1").style.display = style_display_on();
		document.wireless_basic.wds_1.disabled = false;
		document.getElementById("wds_mac_list_2").style.visibility = "visible";
		document.getElementById("wds_mac_list_2").style.display = style_display_on();
		document.wireless_basic.wds_2.disabled = false;
		document.getElementById("wds_mac_list_3").style.visibility = "visible";
		document.getElementById("wds_mac_list_3").style.display = style_display_on();
		document.wireless_basic.wds_3.disabled = false;
		document.getElementById("wds_mac_list_4").style.visibility = "visible";
		document.getElementById("wds_mac_list_4").style.display = style_display_on();
		document.wireless_basic.wds_4.disabled = false;
	}
}

function WdsSecurityOnChange()
{
	document.getElementById("div_wds_encryp_key").style.visibility = "hidden";
	document.getElementById("div_wds_encryp_key").style.display = "none";
	document.wireless_basic.wds_encryp_key.disabled = true;

	if (document.wireless_basic.wds_encryp_type.options.selectedIndex >= 1)
	{
		document.getElementById("div_wds_encryp_key").style.visibility = "visible";
		document.getElementById("div_wds_encryp_key").style.display = style_display_on();
		document.wireless_basic.wds_encryp_key.disabled = false;
	}
}

function CheckValue()
{
	var wireless_mode;
	var submit_ssid_num;
	var channel_11a_index;
	var check_wds_mode;
	var all_wds_list;

	if (document.wireless_basic.ssid.value == "")
	{
		alert("请输?SSID!");
		document.wireless_basic.ssid.focus();
		document.wireless_basic.ssid.select();
		return false;
	}

	submit_ssid_num = 1;

	for (i = 1; i < 8; i++)
	{
		if (eval("document.wireless_basic.mssid_"+i).value != "")
		{
			if (i == 7)
			{
				if (1*apcli_include == 0)
					submit_ssid_num++;
			}
			else
				submit_ssid_num++;
		}
	}

	document.wireless_basic.bssid_num.value = submit_ssid_num;

	all_wds_list = '';
	if (document.wireless_basic.wds_mode.options.selectedIndex >= 2)
	{
		var re = /[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}/;
		for (i = 1; i <= 4; i++)
		{
			if (eval("document.wireless_basic.wds_"+i).value == "")
				continue;
			if (!re.test(eval("document.wireless_basic.wds_"+i).value)) {
				alert("请输入正确的远程WPS AP MAC地址! (XX:XX:XX:XX:XX:XX)");
				return false;
			}
			else {
				all_wds_list += eval("document.wireless_basic.wds_"+i).value;
				all_wds_list += ';';
			}
		}
		if (all_wds_list == "")
		{
			alert("WDS 远程 AP MAC 地址为空 !!!");
			document.wireless_basic.wds_1.focus();
			document.wireless_basic.wds_1.select(); 
			return false;
		}
		else
		{
			document.wireless_basic.wds_list.value = all_wds_list;
			document.wireless_basic.wds_1.disabled = true;
			document.wireless_basic.wds_2.disabled = true;
			document.wireless_basic.wds_3.disabled = true;
			document.wireless_basic.wds_4.disabled = true;
		}
		//document.wireless_basic.rebootAP.value = 1;
	}
	syncWithHost();
	return true;
}

function RadioStatusChange(rs)
{
	if (rs == 1) {
		document.wireless_basic.radioButton.value = "按钮 关闭";
		document.wireless_basic.radiohiddenButton.value = 0;
	}
	else {
		document.wireless_basic.radioButton.value = "按钮 启动";
		document.wireless_basic.radiohiddenButton.value = 1;
	}
}

function doit()
{
	if (document.wireless_basic.radioButton.value.indexOf('关闭') >= 0) 
		RadioStatusChange(1); 
	else 
		RadioStatusChange(0); 
	
	document.wireless_basic.submit();
}

function syncWithHost()
{
	var currentTime = new Date();

	var seconds = currentTime.getSeconds();
	var minutes = currentTime.getMinutes();
	var hours = currentTime.getHours();
	var month = currentTime.getMonth() + 1;
	var day = currentTime.getDate();
	var year = currentTime.getFullYear();

	var seconds_str = " ";
	var minutes_str = " ";
	var hours_str = " ";
	var month_str = " ";
	var day_str = " ";
	var year_str = " ";

	if(seconds < 10)
		seconds_str = "0" + seconds;
	else
		seconds_str = ""+seconds;

	if(minutes < 10)
		minutes_str = "0" + minutes;
	else
		minutes_str = ""+minutes;

	if(hours < 10)
		hours_str = "0" + hours;
	else
		hours_str = ""+hours;

	if(month < 10)
		month_str = "0" + month;
	else
		month_str = ""+month;

	if(day < 10)
		day_str = "0" + day;
	else
		day_str = day;

	var tmp = month_str + day_str + hours_str + minutes_str + year + " ";
	document.wireless_basic.synctime.value = tmp;
}
</script>
</head>
<body onLoad="initValue()">
<form method=post name=wireless_basic action="/goform/wirelessBasic" onSubmit="return CheckValue()">
<input type="hidden" name="bssid_num" value="1">
<table width="90%" border="0" cellpadding="0" cellspacing="0" align="center">
  <tr>
    <td style="padding-top:30px;">
		<table width="100%" border="0" cellpadding="0" cellspacing="0">
        	<tr>
            	<td class="nav">当前路径:KN-WR922 &gt;&gt;无线设置 &gt;&gt;基础设置</td>
          </tr>
		  	  <tr>
            	<td>&nbsp;</td>
          </tr>
          <tr>
            	<td class="space" style="padding-left:10px; padding-right:10px;">&nbsp;&nbsp;&nbsp;本页面提供了基础的无线设置，一般情况下可保持默认配置。详细信息请点<input type="button" class="button5" value="帮助" onclick=popHelp('help.htm#wiressbasic')></td>
          </tr>
		  <tr>
        	<td>&nbsp;</td>
      	  </tr>
			<tr>
            	<td class="titlebg">无线网络</td>
          	</tr>
          	<tr>
            	<td>
			  		<table class="space" width="100%" border="0" cellspacing="0" cellpadding="0">
						<tr id="status0">
                  			<td width="25%" class="contentpadding">无线状态</td>
                 			<td width="75%">开启</td>
                		</tr>
			  			<tr id="status1">
                  			<td width="25%" class="contentpadding">无线状态</td>
            				<td width="75%" id="status1">关闭</td>
                		</tr>
			  			<!--<tr>
                  			<td width="25%" class="contentpadding" id="basicRadioButton">无线开?</td>
                  			<td width="75%" ><input type="button" class="button" name="radioButton" value="RADIO ON" onClick="if (this.value.indexOf('关闭') >= 0) RadioStatusChange(1); else RadioStatusChange(0); document.wireless_basic.submit();" /><input type=hidden name=radiohiddenButton value="2"></td>
						</tr>-->
						<tr>
                  			<td width="25%" class="contentpadding" id="basicRadioButton">无线开关</td>
                  			<td width="75%" ><input type="button" class="button3" name="radioButton" value="RADIO ON" onClick="doit();" /><input type=hidden name=radiohiddenButton value="2"></td>
						</tr>
                		<tr>
                  			<td width="25%" class="contentpadding" id="basicNetMode">网络模式:</td>
                  			<td width="75%"><select name="wirelessmode" id="wirelessmode" size="1" onChange="wirelessModeChange()">
                                    <option value=0>11b/g</option>
                                    <option value=1>11b</option>
                                    <option value=4>11g</option>
									<!--
                                    <option value=2>11a</option>
									
                                    <option value=8>11a/n</option>
									-->
                                    <option value=9>11b/g/n</option></select>
                  			（一般保持默认的11b/g/n 模式即可）</td>
                		</tr>
						<tr>
						  <td width="25%" class="contentpadding" id="basicSSID">网络名称（SSID）</td>
						  <td><input type=text name=ssid size=20 maxlength=32 value="<% getCfgGeneral(1, "SSID1"); %>">
						  （必填）</td>
						</tr>
                        <!--
						<tr>
						  <td width="25%" class="contentpadding">SSID 1:</td>
						  <td><input type=text name=mssid_1 size=20 maxlength=32 value="<% getCfgGeneral(1, "SSID2"); %>">
					      （选填）</td>
						</tr>
						<tr>
						  <td width="25%" class="contentpadding">SSID 2:</td>
						  <td><input type=text name=mssid_2 size=20 maxlength=32 value="<% getCfgGeneral(1, "SSID3"); %>">
					      （选填）</td>
						</tr>
						<tr>
						  <td width="25%" class="contentpadding">SSID 3:</td>
						  <td><input type=text name=mssid_3 size=20 maxlength=32 value="<% getCfgGeneral(1, "SSID4"); %>">
					      （选填）</td>
						</tr>
						<tr>
						  <td width="25%" class="contentpadding">SSID 4:</td>
						  <td><input type=text name=mssid_4 size=20 maxlength=32 value="<% getCfgGeneral(1, "SSID5"); %>">
					      （选填）</td>
						</tr>
						<tr>
						  <td width="25%" class="contentpadding">SSID 5:</td>
						  <td><input type=text name=mssid_5 size=20 maxlength=32 value="<% getCfgGeneral(1, "SSID6"); %>">
					      （选填）</td>
						</tr>
						<tr>
						  <td width="25%" class="contentpadding">SSID 6:</td>
						  <td><input type=text name=mssid_6 size=20 maxlength=32 value="<% getCfgGeneral(1, "SSID7"); %>">
					      （选填）</td>
						</tr>
						<tr>
						  <td class="contentpadding">SSID 7:</td>
						  <td><input type=text name=mssid_7 size=20 maxlength=32 value="<% getCfgGeneral(1, "SSID8"); %>">
					      （选填）</td>
						</tr>  -->
						<tr>
						  <td width="25%" class="contentpadding" id="basicBroadcastSSID">广播网络名称（SSID）</td>
						  <td><input type=radio name=broadcastssid value="1" checked>启用&nbsp;<input type=radio name=broadcastssid value="0">禁用</td>
						</tr>
						<tr>
						  <td width="25%" class="contentpadding" id="basicBSSID">BSSID:</td>
						  <td><% getWlanCurrentMac(); %></td>
						</tr>
						<tr>
						<td class="contentpadding" id="basicApIsolated">AP Isolation:</td>
   						<td>
      						<input type=radio name=apisolated value="1"><font id="basicApIsolatedEnable">启用</font>
     						 <input type=radio name=apisolated value="0" checked><font id="basicApIsolatedDisable">禁用</font>（本AP内隔离）
   						 </td>
  						</tr>
  						<tr> 
  						  <td class="contentpadding" id="basicMBSSIDApIsolated">MBSSID AP Isolation:</td>
   						  <td>
   						   <input type=radio name=mbssidapisolated value="1"><font id="basicMBSSIDApIsolatedEnable">启用</font>
     						 <input type=radio name=mbssidapisolated value="0" checked><font id="basicMBSSIDApIsolatedDisable">禁用</font>（本AP外隔离）
   						 </td>
						 </tr>
						
						<tr id="div_11a_channel" name="div_11a_channel" style="visibility:visible; display:none">
						  <td width="25%" class="contentpadding">频段:</td>
						  <td><select id="sz11aChannel" name="sz11aChannel" size="1" onChange="ChannelOnChange()">
								<option value=0 id="basicFreqAAuto">自动选择</option>
									   <% getWlan11aChannels(); %>
							  </select></td>
						</tr>
						<tr id="div_11b_channel" name="div_11b_channel" style="visibility:visible; display:none">
						  <td width="25%" class="contentpadding">频段:</td>
						  <td><select id="sz11bChannel" name="sz11bChannel" size="1" onChange="ChannelOnChange()">
								<option value=0 id="basicFreqBAuto">自动选择</option>
									  <% getWlan11bChannels(); %>
							  </select></td>
						</tr>
						<tr id="div_11g_channel" name="div_11g_channel" style="visibility:visible; display:none">
						  <td width="25%" class="contentpadding">频段:</td>
						  <td><select id="sz11gChannel" name="sz11gChannel" size="1" onChange="ChannelOnChange()">
								<option value=0 id="basicFreqGAuto">自动选择</option>
							<% getWlan11gChannels(); %>
						  </select></td>
						</tr>
						<tr name="extension_channel" id="extension_channel">
						  <td class="contentpadding" id="basicHTExtChannel">扩展频段:</td>
						  <td><select id="n_extcha" name="n_extcha" size="1">
						  	<option value=1 selected>2412MHz (Channel 1)</option></select></td>
						</tr>
              		</table>
				</td>
          	</tr>
			<tr style="display:none; visibility:hidden">
            	<td class="titlebg" id="basicWDSTitle">无线分布系统(WDS)</td>
          	</tr>
		 	<tr style="display:none; visibility:hidden">
            	<td>
			  		<table class="space" width="100%" border="0" cellspacing="0" cellpadding="0">
						<tr>
						  <td width="25%" class="contentpadding" id="basicWDSMode">WDS 模式:</td>
						  <td width="75%"><select name="wds_mode" id="wds_mode" size="1" onChange="WdsModeOnChange()">
									  <option value=0 SELECTED id="basicWDSDisable">禁用</option>
									  <option value=4>懒人模式</option>
									  <option value=2>桥接模式</option>
									  <option value=3>中继模式</option>
									 </select></td>
						</tr>
						<tr id="div_wds_phy_mode" name="div_wds_phy_mode" style="visibility:visible; display:none"> 
						  <td class="contentpadding" id="basicWDSPhyMode">Phy 模式:</td>
						  <td><select name="wds_phy_mode" id="wds_phy_mode" size="1">
								<option value=CCK selected>CCK</option>
								<!-- other modes are added by javascript -->
			  						</select></td>
						</tr>
						<tr id="div_wds_encryp_type" name="div_wds_encryp_type" style="visibility:visible; display:none"> 
						  <td class="contentpadding" id="basicWDSEncrypType">加密方式:</td>
						  <td><select name="wds_encryp_type" id="wds_encryp_type" size="1" onChange="WdsSecurityOnChange()">
									  <option value="NONE" selected>None</option>
									  <option value="WEP">WEP</option>
									  <option value="TKIP">TKIP</option>
									  <option value="AES">AES</option>
								  </select></td>
						</tr>
						<tr id="div_wds_encryp_key" name="div_wds_encryp_key" style="visibility:visible; display:none">
						  <td class="contentpadding" id="basicWDSEncrypKey">密钥:</td>
						  <td><input type=text name=wds_encryp_key size=28 maxlength=64 value=""></td>
						</tr>
						<tr id="wds_mac_list_1" name="wds_mac_list_1" style="visibility:visible; display:none">
						  <td class="contentpadding" id="basicWDSAPMacAddr">AP MAC 地址:</td>
						  <td><input type=text name=wds_1 size=20 maxlength=17 value=""></td>
						</tr>
						<tr id="wds_mac_list_2" name="wds_mac_list_2" style="visibility:visible; display:none">
						  <td class="contentpadding" id="basicWDSAPMacAddr">AP MAC 地址:</td>
						  <td><input type=text name=wds_2 size=20 maxlength=17 value=""></td>
						</tr>
						<tr id="wds_mac_list_3" name="wds_mac_list_3" style="visibility:visible; display:none">
						  <td class="contentpadding" id="basicWDSAPMacAddr">AP MAC 地址:</td>
						  <td><input type=text name=wds_3 size=20 maxlength=17 value=""></td>
						</tr>
						<tr id="wds_mac_list_4" name="wds_mac_list_4" style="visibility:visible; display:none">
						  <td class="contentpadding" id="basicWDSAPMacAddr">AP MAC 地址:</td>
						  <td><input type=text name=wds_4 size=20 maxlength=17 value=""></td>
						</tr><input type="hidden" name="wds_list" value="1">
              		</table>
				</td>
          	</tr>
			<tr style="display:none; visibility:hidden">
				<td class="titlebg" id="basicHTPhyMode">HT 实体模式</td>
			</tr>
		  	<tr>
            	<td><!--<table width="98%" id="div_11n" name="div_11n" border="0" cellspacing="0" cellpadding="0" style="display:none">-->
			  		<table class="space" width="100%" id="div_11n" name="div_11n" border="0" cellspacing="0" cellpadding="0">
						<tr style="display:none; visibility:hidden">
						  <td width="22%" class="contentpadding" id="basicHTOPMode">运作模式:</td>
						  <td width="78%"><input type=radio name=n_mode value="0" checked>混合模式&nbsp;<input type=radio name=n_mode value="1">Green Field</td>
						</tr>
						<tr style="display:none; visibility:hidden">
						  <td class="contentpadding" id="basicHTChannelBW">频段带宽:</td>
						  <td><input type=radio name=n_bandwidth value="0" onClick="Channel_BandWidth_onClick()" checked>20&nbsp;<input type=radio name=n_bandwidth value="1" onClick="Channel_BandWidth_onClick()">20/40</td>
						</tr>
						<tr style="display:none; visibility:hidden">
						  <td class="contentpadding" id="basicHTGI">保护间隔:</td>
						  <td><input type=radio name=n_gi value="0" checked>&nbsp;<input type=radio name=n_gi value="1">自动</td>
						</tr>
						<tr style="display:none; visibility:hidden">
						  <td class="contentpadding">MCS:</td>
						  <td><select name="n_mcs" size="1">
											 <option value = 0>0</option>
											 <option value = 1>1</option>
											 <option value = 2>2</option>
											 <option value = 3>3</option>
											 <option value = 4>4</option>
											 <option value = 5>5</option>
											 <option value = 6>6</option>
											 <option value = 7>7</option>
											 <option value = 8>8</option>
											 <option value = 9>9</option>
											 <option value = 10>10</option>
											 <option value = 11>11</option>
											 <option value = 12>12</option>
											 <option value = 13>13</option>
											 <option value = 14>14</option>
											 <option value = 15>15</option>
											 <option value = 32>32</option>
											 <option value = 33 selected id="basicHTAutoMCS">自动</option></select></td>
						</tr>
						<tr style="display:none; visibility:hidden">
						  <td class="contentpadding" id="basicHTRDG">反转方向权限(RDG):</td>
						  <td><input type=radio name=n_rdg value="0" checked>禁用&nbsp;<input type=radio name=n_rdg value="1">启用</td>
						</tr>
		  				
						<tr style="display:none; visibility:hidden">
						  <td class="contentpadding" id="basicHTAMSDU">聚合MAC业务数据单元(A-MSDU):</td>
						  <td><input type=radio name=n_amsdu value="0" checked>禁用&nbsp;<input type=radio name=n_amsdu value="1">启用</td>
						</tr>
						<tr style="display:none; visibility:hidden">
						  <td class="contentpadding" id="basicHTAddBA">自动单一区块确认:</td>
						  <td><input type=radio name=n_autoba value="0" checked>禁用&nbsp;<input type=radio name=n_autoba value="1">启用</td>
						</tr>
						<tr style="display:none; visibility:hidden">
						  <td class="contentpadding" id="basicHTDelBA">拒绝单一区块确认要求:</td>
						  <td><input type=radio name=n_badecline value="0" checked>禁用&nbsp;<input type=radio name=n_badecline value="1">启用</td>
						</tr>
					</table>
				</td>
          	</tr>
			<tr style="display:none; visibility:hidden">
				<td class="titlebg" id="basicOther">其他设置</td>
			</tr>
          	<tr style="display:none; visibility:hidden">
            	<td>
			  		<table class="space" width="100%" id="div_11n_plugfest" name="div_11n_plugfest" border="0" cellspacing="0" cellpadding="0">
					  <!--
					  <tr>
						<td width="45%" bgcolor="#E8F8FF" nowrap>40 Mhz Intolerant</td>
						<td ><font color="#003366" face=arial><b>
						  <input type=radio name=f_40mhz value="0" checked>Diable&nbsp;
						  <input type=radio name=f_40mhz value="1">Enable
						</b></font></td>
					  </tr>
					  <tr>
						<td>WiFi Optimum</td>
						<td>
						  <input type=radio name=wifi_opt value="0" checked>Diable&nbsp;
						  <input type=radio name=wifi_opt value="1">Enable
						</td>
					  </tr>
					  -->
						<tr>
                  			<td width="25%" class="contentpadding" id="basicHTTxStream">高吞吐量传送数据流:</td>
                  			<td width="75%"><select name="tx_stream" size="1">
                            	<option value = 1>1</option>
	                            <option value = 2>2</option></select></td>
						</tr>
						<tr>
						  <td class="contentpadding" id="basicHTRxStream">高吞吐量接收数据流:</td>
						  <td><select name="rx_stream" size="1">
											<option value = 1>1</option>
											<option value = 2>2</option></select></td>
						</tr>
              		</table>
				</td>
          	</tr>
		  	<tr>
            	<td>&nbsp;</td>
          	</tr>
          	<tr>
            	<td class="contentpadding"><input type="hidden" value="" name="synctime"><input type="hidden" value="/wir_basic.asp" name="submit-url"><input type="submit" class="button" value="提交"/>&nbsp;&nbsp;<input type="reset" class="button" value="取消" onClick="window.location.reload()"></td>
            </tr>
		    <tr>
           	   <td>&nbsp;</td>
            </tr>
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