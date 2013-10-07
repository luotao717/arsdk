#!/bin/sh

RadioOff=`nvram_get 2860 RadioOff`
RadioOff2=`nvram_get 2860 RadioOff2`
RadioOff3=`nvram_get 2860 RadioOff3`
RadioOff4=`nvram_get 2860 RadioOff3`
WirelessMode=`nvram_get 2860 WirelessMode`
SSID1=`nvram_get 2860 SSID1`
SSID2=`nvram_get 2860 SSID2`
SSID3=`nvram_get 2860 SSID3`
SSID4=`nvram_get 2860 SSID4`
HideSSID=`nvram_get 2860 HideSSID`
HT_EXTCHA=`nvram_get 2860 HT_EXTCHA`
NoForwarding=`nvram_get 2860 NoForwarding`
NoForwardingBTNBSSID=`nvram_get 2860 NoForwardingBTNBSSID`
Channel=`nvram_get 2860 Channel`
AuthModeAll=`nvram_get 2860 AuthMode`
WPAPSK1=`nvram_get 2860 WPAPSK1`
WPAPSK2=`nvram_get 2860 WPAPSK2`
WPAPSK3=`nvram_get 2860 WPAPSK3`
WPAPSK4=`nvram_get 2860 WPAPSK4`
EncrypTypeAll=`nvram_get 2860 EncrypType`
DefaultKeyIDAll=`nvram_get 2860 DefaultKeyID`
Key1TypeAll=`nvram_get 2860 Key1Type`
Connect2wifiports=`nvram_get 2860 Connect2wifiports`
Connect1vlanEn=`nvram_get 2860 Connect1vlanEn`
Key1Str1=`nvram_get 2860 Key1Str1`
Key1Str2=`nvram_get 2860 Key1Str2`
Key1Str3=`nvram_get 2860 Key1Str3`
Key1Str4=`nvram_get 2860 Key1Str4`

AuthMode=`echo "$AuthModeAll"|awk -F ';' '{print $1}'`
AuthMode2=`echo "$AuthModeAll"|awk -F ';' '{print $2}'`
AuthMode3=`echo "$AuthModeAll"|awk -F ';' '{print $3}'`
AuthMode4=`echo "$AuthModeAll"|awk -F ';' '{print $4}'`

EncrypType=`echo "$EncrypTypeAll"|awk -F ';' '{print $1}'`
EncrypType2=`echo "$EncrypTypeAll"|awk -F ';' '{print $2}'`
EncrypType3=`echo "$EncrypTypeAll"|awk -F ';' '{print $3}'`
EncrypType4=`echo "$EncrypTypeAll"|awk -F ';' '{print $4}'`

DefaultKeyID=`echo "$DefaultKeyIDAll"|awk -F ';' '{print $1}'`
DefaultKeyID2=`echo "$DefaultKeyIDAll"|awk -F ';' '{print $2}'`
DefaultKeyID3=`echo "$DefaultKeyIDAll"|awk -F ';' '{print $3}'`
DefaultKeyID4=`echo "$DefaultKeyIDAll"|awk -F ';' '{print $4}'`

Key1Type=`echo "$Key1TypeAll"|awk -F ';' '{print $1}'`
Key1Type2=`echo "$Key1TypeAll"|awk -F ';' '{print $2}'`
Key1Type3=`echo "$Key1TypeAll"|awk -F ';' '{print $3}'`
Key1Type4=`echo "$Key1TypeAll"|awk -F ';' '{print $4}'`


HideSSID0=`echo "$HideSSID"|awk -F ';' '{print $1}'`
HideSSID1=`echo "$HideSSID"|awk -F ';' '{print $2}'`
HideSSID2=`echo "$HideSSID"|awk -F ';' '{print $3}'`
HideSSID3=`echo "$HideSSID"|awk -F ';' '{print $4}'`


HT_BW=`nvram_get 2860 HT_BW`
HT_BSSCoexistence=`nvram_get 2860 HT_BSSCoexistence`
countryconde=`nvram_get 2860 CountryCode`

iwpriv wifi0 setCountry $countryconde

started=`ifconfig ath0 2>/dev/null`
echo $started
# �ж������Ƿ�����
if [ x$RadioOff = x"0" ]; then


# ��������ģʽ
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

# ��������SSID
if [ "$SSID1" != "" ];then
iwconfig ath0 essid "$SSID1"
echo $SSID1
fi

# ����SSID�Ƿ�����
iwpriv ath0 hide_ssid $HideSSID0

# ������չƵ��
#iwpriv ath0 chextoffset $HT_EXTCHA

# ����Ƶ���ŵ�
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

killall hostapd

if [ x$EncrypType = x"WEP" ]; then
# ����WEP����
#killall -q hostapd
iwconfig ath0 key off
if [ x$Key1Type = x"0" ]; then
iwconfig ath0 key [1] `nvram_get 2860 "Key"${DefaultKeyID}"Str1"`
elif [ x$Key1Type = x"1" ]; then
echo $Key1Str1
iwconfig ath0 key [1] s:$Key1Str1
fi

elif [ $WPAn -gt 0 ]; then
# ����WPA����
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

ifconfig ath0 up

elif [ x$RadioOff = x"1" -a x$started != "x" ];then
ifconfig ath0 down
fi

if [ x$RadioOff = x"1" ];then
ifconfig ath0 down
fi

#if [ x$RadioOff = x"0" ]; then
#ifconfig ath0 up
#fi

#######ath1



#started=`ifconfig ath1 2>/dev/null`

# �ж������Ƿ�����
if [ x$RadioOff2 = x"0" ]; then

# ��������SSID
if [ "$SSID2" != "" ];then
echo $SSID2
iwconfig ath1 essid "$SSID2"
fi
# ����SSID�Ƿ�����
iwpriv ath1 hide_ssid $HideSSID1

WPAn=0;
if [ x$AuthMode2 = x"WPAPSKWPA2PSK" ]; then
WPAn=3;
elif [ x$AuthMode2 = x"WPA2PSK" ]; then
WPAn=2;
elif [ x$AuthMode2 = x"WPAPSK" ]; then
WPAn=1;
fi

#pid=`ps -ef | grep "hostapd -Bdd /tmp/hostapd2.conf" | grep -v grep | cut -c 0-5`

if [ x$EncrypType2 = x"WEP" ]; then
# ����WEP����
#killall -q hostapd
iwconfig ath1 key off
if [ x$Key1Type2 = x"0" ]; then
iwconfig ath1 key [2] `nvram_get 2860 "Key"${DefaultKeyID2}"Str2"`
elif [ x$Key1Type2 = x"1" ]; then
echo $Key1Str2
iwconfig ath1 key [2] s:$Key1Str2
fi

elif [ $WPAn -gt 0 ]; then
# ����WPA����
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

ifconfig ath1 up

elif [ x$RadioOff2 = x"1" ];then
ifconfig ath1 down
fi

#######ath2



#started=`ifconfig ath2 2>/dev/null`



# �ж������Ƿ�����
if [ x$RadioOff3 = x"0" ]; then




# ��������SSID
if [ "$SSID3" != "" ];then
echo $SSID3
iwconfig ath2 essid "$SSID3"
fi

# ����SSID�Ƿ�����
iwpriv ath2 hide_ssid $HideSSID2

WPAn=0;
if [ x$AuthMode3 = x"WPAPSKWPA2PSK" ]; then
WPAn=3;
elif [ x$AuthMode3 = x"WPA2PSK" ]; then
WPAn=2;
elif [ x$AuthMode3 = x"WPAPSK" ]; then
WPAn=1;
fi

if [ x$EncrypType3 = x"WEP" ]; then
# ����WEP����
#killall -q hostapd
iwconfig ath2 key off
if [ x$Key1Type3 = x"0" ]; then
iwconfig ath2 key [3] `nvram_get 2860 "Key"${DefaultKeyID3}"Str3"`
elif [ x$Key1Type3 = x"1" ]; then
echo $Key1Str3
iwconfig ath2 key [3] s:$Key1Str3
fi

elif [ $WPAn -gt 0 ]; then
# ����WPA����
#killall -q hostapd

mkdir -p /tmp/wlan/hostapd3

if [ x$EncrypType3 = x"TKIPAES" ]; then
PAIRWISE="TKIP CCMP"
elif [ x$EncrypType3 = x"TKIP" ]; then
PAIRWISE=TKIP
elif [ x$EncrypType3 = x"AES" ]; then
PAIRWISE=CCMP
fi

ath2brname=br0
echo "$Connect2wifiports" | grep -q "2"
if [ $? -eq 0 ]; then
ath2brname=$vlanbridgename
fi
echo "interface=ath2
bridge=$ath2brname
ctrl_interface=/tmp/wlan/hostapd3
ctrl_interface_group=0
ssid=$SSID3
dtim_period=2
max_num_sta=255
macaddr_acl=0
auth_algs=1
ignore_broadcast_ssid=$HideSSID2
wme_enabled=0
ieee8021x=0
eapol_version=2
eapol_key_index_workaround=0
eap_server=1
wpa=$WPAn
wpa_passphrase=$WPAPSK3
wpa_key_mgmt=WPA-PSK
wpa_pairwise=$PAIRWISE
wpa_group_rekey=0
wpa_strict_rekey=1" > /tmp/hostapd3.conf
hostapd -Bdd /tmp/hostapd3.conf

elif [ x$AuthMode3 = x"OPEN" -a x$EncrypType3 = x"NONE" ]; then
#killall -q hostapd
iwconfig ath2 key off
fi

#if [ x$started = x ]; then
#ifconfig ath2 up
#fi

ifconfig ath2 up

elif [ x$RadioOff3 = x"1" ];then
ifconfig ath2 down
fi






#######ath3

#started=`ifconfig ath3 2>/dev/null`

# �ж������Ƿ�����
if [ x$RadioOff4 = x"0" ]; then




# ��������SSID
if [ "$SSID4" != "" ];then
echo $SSID4
iwconfig ath3 essid "$SSID4"
fi

# ����SSID�Ƿ�����
iwpriv ath3 hide_ssid $HideSSID3


WPAn=0;
if [ x$AuthMode4 = x"WPAPSKWPA2PSK" ]; then
WPAn=3;
elif [ x$AuthMode4 = x"WPA2PSK" ]; then
WPAn=2;
elif [ x$AuthMode4 = x"WPAPSK" ]; then
WPAn=1;
fi

if [ x$EncrypType4 = x"WEP" ]; then
# ����WEP����
#killall -q hostapd
iwconfig ath3 key off
if [ x$Key1Type4 = x"0" ]; then
iwconfig ath3 key [4] `nvram_get 2860 "Key"${DefaultKeyID4}"Str4"`
elif [ x$Key1Type4 = x"1" ]; then
echo $Key1Str4
iwconfig ath3 key [4] s:$Key1Str4
fi

elif [ $WPAn -gt 0 ]; then
# ����WPA����
#killall -q hostapd

mkdir -p /tmp/wlan/hostapd4

if [ x$EncrypType4 = x"TKIPAES" ]; then
PAIRWISE="TKIP CCMP"
elif [ x$EncrypType4 = x"TKIP" ]; then
PAIRWISE=TKIP
elif [ x$EncrypType4 = x"AES" ]; then
PAIRWISE=CCMP
fi

ath3brname=br0
echo "$Connect2wifiports" | grep -q "3"
if [ $? -eq 0 ]; then
ath3brname=$vlanbridgename
fi
echo "interface=ath3
bridge=$ath3brname
ctrl_interface=/tmp/wlan/hostapd4
ctrl_interface_group=0
ssid=$SSID4
dtim_period=2
max_num_sta=255
macaddr_acl=0
auth_algs=1
ignore_broadcast_ssid=$HideSSID3
wme_enabled=0
ieee8021x=0
eapol_version=2
eapol_key_index_workaround=0
eap_server=1
wpa=$WPAn
wpa_passphrase=$WPAPSK4
wpa_key_mgmt=WPA-PSK
wpa_pairwise=$PAIRWISE
wpa_group_rekey=0
wpa_strict_rekey=1" > /tmp/hostapd4.conf
hostapd -Bdd /tmp/hostapd4.conf

elif [ x$AuthMode4 = x"OPEN" -a x$EncrypType4 = x"NONE" ]; then
#killall -q hostapd
iwconfig ath3 key off
fi

#if [ x$started = x ]; then
#ifconfig ath3 up
#fi
ifconfig ath3 up

elif [ x$RadioOff4 = x"1" ];then
ifconfig ath3 down
fi

#ifconfig ath1 up
#ifconfig ath2 up
#ifconfig ath3 up

if [ x$HT_BSSCoexistence=x"0" ];then
	iwpriv ath0 disablecoext 1
fi