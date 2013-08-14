#!/bin/sh
#
# $Id: ddns.sh,v 1.1 2007-09-24 09:34:52 winfred Exp $
#
# usage: ddns.sh
#
ddnsenabled=`nvram_get 2860 DDNSEnabled`
#ip=`nvram_get 2860 DDNSIP`
srv=`nvram_get 2860 DDNSProvider`
ddns=`nvram_get 2860 DDNS`
u=`nvram_get 2860 DDNSAccount`
pw=`nvram_get 2860 DDNSPassword`
wan_type=`nvram_get 2860 wanConnectionMode`

killall -q ddns_byx

if [ "$ddnsenabled" = "OFF" ]; then
	exit 0
fi
if [ "$ddns" = "" -o "$u" = "" -o "$pw" = "" ]; then
	exit 0
fi

# debug
echo "srv=$srv"
echo "ddns=$ddns"
echo "u=$u"
echo "pw=$pw"


#get if IP
if [ "$wan_type" = "PPPOE" ]; then
	s1=`ifconfig ppp0 | grep "inet addr"`
	while [ "$s1" = "" ]
	do
		s1=`ifconfig ppp0 | grep "inet addr"`
		sleep 2
	done

	s2=`echo $s1 | cut -f2 -d:`
	if_addr=`echo $s2 | cut -f1 -d " "`

else
	s3=`ifconfig eth2.2 | grep "inet addr"`
	echo "########s3=$s3"
	while [ "$s3" = "" ]
	do
		s3=`ifconfig eth2.2 | grep "inet addr"`
		sleep 2
	done

	s4=`echo $s3 | cut -f2 -d:`
	if_addr=`echo $s4 | cut -f1 -d " "`

fi





if [ "$srv" = "dyndns" ]; then
	ddns_byx -s dyndns $ddns $u $pw $if_addr
elif [ "$srv" = "qdns" ]; then
	ddns_byx -s qdns $ddns $u $pw $if_addr
elif [ "$srv" = "88ip" ]; then
	ddns_byx -s 88ip $ddns $u $pw $if_addr
elif [ "$srv" = "no-ip.com" ]; then
	inadyn -u $u -p $pw -a $ddns --dyndns_system default@$srv &
else
#	echo "$0: unknown DDNS provider: $srv"
	exit 1
fi

