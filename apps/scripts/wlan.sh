#!/bin/sh
brctl addbr br1
ifconfig br1 up
brctl addbr br2
ifconfig br2 up

RadioOff=`nvram_get 2860 RadioOff`
RadioOff2=`nvram_get 2860 RadioOff2`

WirelessMode=`nvram_get 2860 WirelessMode`
SSID1=`nvram_get 2860 SSID1`
SSID2=`nvram_get 2860 SSID2`

HideSSID=`nvram_get 2860 HideSSID`
HT_EXTCHA=`nvram_get 2860 HT_EXTCHA`
NoForwarding=`nvram_get 2860 NoForwarding`
NoForwardingBTNBSSID=`nvram_get 2860 NoForwardingBTNBSSID`
Channel=`nvram_get 2860 Channel`
AuthModeAll=`nvram_get 2860 AuthMode`
WPAPSK1=`nvram_get 2860 WPAPSK1`
WPAPSK2=`nvram_get 2860 WPAPSK2`

EncrypTypeAll=`nvram_get 2860 EncrypType`
DefaultKeyIDAll=`nvram_get 2860 DefaultKeyID`
Key1TypeAll=`nvram_get 2860 Key1Type`
Connect2wifiports=`nvram_get 2860 Connect2wifiports`
Connect1vlanEn=`nvram_get 2860 Connect1vlanEn`
Key1Str1=`nvram_get 2860 Key1Str1`
Key1Str2=`nvram_get 2860 Key1Str2`


AuthMode=`echo "$AuthModeAll"|awk -F ';' '{print $1}'`
AuthMode2=`echo "$AuthModeAll"|awk -F ';' '{print $2}'`


EncrypType=`echo "$EncrypTypeAll"|awk -F ';' '{print $1}'`
EncrypType2=`echo "$EncrypTypeAll"|awk -F ';' '{print $2}'`


DefaultKeyID=`echo "$DefaultKeyIDAll"|awk -F ';' '{print $1}'`
DefaultKeyID2=`echo "$DefaultKeyIDAll"|awk -F ';' '{print $2}'`


Key1Type=`echo "$Key1TypeAll"|awk -F ';' '{print $1}'`
Key1Type2=`echo "$Key1TypeAll"|awk -F ';' '{print $2}'`

HideSSID0=`echo "$HideSSID"|awk -F ';' '{print $1}'`
HideSSID1=`echo "$HideSSID"|awk -F ';' '{print $2}'`

HT_BW=`nvram_get 2860 HT_BW`
HT_BSSCoexistence=`nvram_get 2860 HT_BSSCoexistence`
countryconde=`nvram_get 2860 CountryCode`

#cfg wlan up
#apdown
#apup

if [ x$RadioOff = x"0" -o x$RadioOff2 = x"0" -o x$RadioOff3 = x"0" -o x$RadioOff4 = x"0" ]; then
    insmod /lib/modules/2.6.31/net/asf.ko
    insmod /lib/modules/2.6.31/net/adf.ko
    insmod /lib/modules/2.6.31/net/ath_hal.ko
    insmod /lib/modules/2.6.31/net/ath_rate_atheros.ko
    insmod /lib/modules/2.6.31/net/ath_dev.ko
    insmod /lib/modules/2.6.31/net/umac.ko
#    iwpriv wifi0 setCountryID $ATH_countrycode
fi


iwpriv wifi0 setCountry $countryconde
if [ x$RadioOff = x"0" ]; then
wlanconfig ath0 create wlandev wifi0 wlanmode ap
fi
if [ x$RadioOff2 = x"0" ]; then
wlanconfig ath1 create wlandev wifi0 wlanmode ap
fi

started=`ifconfig ath0 2>/dev/null`
echo $started
# 判断无线是否启动
if [ x$RadioOff = x"0" ]; then


# 设置无线模式
if [ x$WirelessMode = x"1" ];then
iwpriv ath0 mode 11B
elif [ x$WirelessMode = x"4" ];then
iwpriv ath0 mode 11G
elif [ x$WirelessMode = x"0" ];then
iwpriv ath0 mode 11G
elif [ x$WirelessMode = x"9" -o x$WirelessMode = x"6" ];then
	if [ x$HT_BW = x"0" ];then
		iwpriv ath0 mode 11NGHT20
  elif [ x$HT_BW = x"1" ];then
			if [ $Channel -gt 7 ];then
				iwpriv ath0 mode 11NGHT40MINUS
			else
				iwpriv ath0 mode 11NGHT40PLUS
			fi
	fi
fi

# 设置无线SSID
if [ "$SSID1" != "" ];then
iwconfig ath0 essid "$SSID1"
echo $SSID1
fi

# 设置SSID是否隐藏
iwpriv ath0 hide_ssid $HideSSID0

# 设置扩展频段
#iwpriv ath0 chextoffset $HT_EXTCHA

# 设置频段信道
iwconfig ath0 channel $Channel

WPAn=0;
if [ x$AuthMode = x"WPAPSKWPA2PSK" ]; then
WPAn=3;
elif [ x$AuthMode = x"WPA2PSK" ]; then
WPAn=2;
elif [ x$AuthMode = x"WPAPSK" ]; then
WPAn=1;
fi

vlanbridgename=br1
if [ x$Connect1vlanEn = x"1" ]; then
vlanbridgename=br2;
elif [ x$AuthMode = x"0" ]; then
vlanbridgename=br1;
fi

if [ x$EncrypType = x"WEP" ]; then
# 设置WEP加密
#killall -q hostapd
iwconfig ath0 key off
if [ x$Key1Type = x"0" ]; then
iwconfig ath0 key [1] `nvram_get 2860 "Key"${DefaultKeyID}"Str1"`
elif [ x$Key1Type = x"1" ]; then
echo $Key1Str1
iwconfig ath0 key [1] s:$Key1Str1
fi

elif [ $WPAn -gt 0 ]; then
# 设置WPA加密
#killall -q hostapd

mkdir -p /tmp/wlan/hostapd

if [ x$EncrypType = x"TKIPAES" ]; then
PAIRWISE="TKIP CCMP"
elif [ x$EncrypType = x"TKIP" ]; then
PAIRWISE=TKIP
elif [ x$EncrypType = x"AES" ]; then
PAIRWISE=CCMP
fi

ath0brname=br0
echo "$Connect2wifiports" | grep -q "0"
if [ $? -eq 0 ]; then
ath0brname=$vlanbridgename
fi
echo "interface=ath0
bridge=$ath0brname
ctrl_interface=/tmp/wlan/hostapd
ctrl_interface_group=0
ssid=$SSID1
dtim_period=2
max_num_sta=255
macaddr_acl=0
auth_algs=1
ignore_broadcast_ssid=$HideSSID0
wme_enabled=0
ieee8021x=0
eapol_version=2
eapol_key_index_workaround=0
eap_server=1
wpa=$WPAn
wpa_passphrase=$WPAPSK1
wpa_key_mgmt=WPA-PSK
wpa_pairwise=$PAIRWISE
wpa_group_rekey=0
wpa_strict_rekey=1" > /tmp/hostapd.conf
hostapd -Bdd /tmp/hostapd.conf

elif [ x$AuthMode = x"OPEN" -a x$EncrypType = x"NONE" ]; then
#killall -q hostapd
iwconfig ath0 key off
echo $EncrypType
fi

#if [ x$started = x ]; then
#ifconfig ath0 up
#fi


elif [ x$RadioOff = x"1" -a x$started != "x" ];then
ifconfig ath0 down
fi

if [ x$RadioOff = x"1" ];then
ifconfig ath0 down
fi

#######ath1





# 判断无线是否启动
if [ x$RadioOff2 = x"0" ]; then

echo ath1start1



# 设置无线SSID
if [ "$SSID2" != "" ];then
echo $SSID2
iwconfig ath1 essid "$SSID2"
fi
# 设置SSID是否隐藏
iwpriv ath1 hide_ssid $HideSSID1

# 设置频段信道
iwconfig ath1 channel $Channel


WPAn=0;
if [ x$AuthMode2 = x"WPAPSKWPA2PSK" ]; then
WPAn=3;
elif [ x$AuthMode2 = x"WPA2PSK" ]; then
WPAn=2;
elif [ x$AuthMode2 = x"WPAPSK" ]; then
WPAn=1;
fi

if [ x$EncrypType2 = x"WEP" ]; then
# 设置WEP加密
#killall -q hostapd
iwconfig ath1 key off
if [ x$Key1Type2 = x"0" ]; then
iwconfig ath1 key [2] `nvram_get 2860 "Key"${DefaultKeyID2}"Str2"`
elif [ x$Key1Type2 = x"1" ]; then
echo $Key1Str2
iwconfig ath1 key [2] s:$Key1Str2
fi

elif [ $WPAn -gt 0 ]; then
# 设置WPA加密
#killall -q hostapd
mkdir -p /tmp/wlan/hostapd2

if [ x$EncrypType2 = x"TKIPAES" ]; then
PAIRWISE="TKIP CCMP"
elif [ x$EncrypType2 = x"TKIP" ]; then
PAIRWISE=TKIP
elif [ x$EncrypType2 = x"AES" ]; then
PAIRWISE=CCMP
fi

ath1brname=br0
echo "$Connect2wifiports" | grep -q "1"
if [ $? -eq 0 ]; then
ath1brname=$vlanbridgename
fi
echo "interface=ath1
bridge=$ath1brname
ctrl_interface=/tmp/wlan/hostapd2
ctrl_interface_group=0
ssid=$SSID2
dtim_period=2
max_num_sta=255
macaddr_acl=0
auth_algs=1
ignore_broadcast_ssid=$HideSSID1
wme_enabled=0
ieee8021x=0
eapol_version=2
eapol_key_index_workaround=0
eap_server=1
wpa=$WPAn
wpa_passphrase=$WPAPSK2
wpa_key_mgmt=WPA-PSK
wpa_pairwise=$PAIRWISE
wpa_group_rekey=0
wpa_strict_rekey=1" > /tmp/hostapd2.conf
hostapd -Bdd /tmp/hostapd2.conf

elif [ x$AuthMode2 = x"OPEN" -a x$EncrypType2 = x"NONE" ]; then
#killall -q hostapd
iwconfig ath1 key off
fi

#if [ x$started = x ]; then
#ifconfig ath1 up
#fi


elif [ x$RadioOff2 = x"1" -a x$started != "x" ];then
ifconfig ath1 down
fi



if [ x$RadioOff = x"0" ]; then
ifconfig ath0 up
fi
ifconfig ath1 up

sleep 5

if [ x$HT_BSSCoexistence=x"0" ];then
	iwpriv ath0 disablecoext 1
fi

iwpriv ath1 ap_bridge 0

