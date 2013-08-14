#!/bin/sh
wan_if=eth2.2
user=`nvram_get 2860 wan_pppoe_user`
pw=`nvram_get 2860 wan_pppoe_pass`
mtu=`nvram_get 2860 wan_pppoe_mtu`
mru=`nvram_get 2860 wan_pppoe_mru`
pppoe_specific=`nvram_get 2860 pppoe_specific`
#pppoe.sh $u $pw $wan_if $mtu $mru
DNRD_PID=/var/run/dnrd.pid
killall -9 pppd
killall -9 dnrd
rm -f /tmp/connectPass
#syslogd -m 0
if [ "$pppoe_specific" = "2" ]; then
	u=`nvram_get 2860 wan_pppoe_Encrypt_user`
	ncxkjs $u
fi

pppoe.sh $user $pw $wan_if $mtu $mru
if [ ! -f $DNRD_PID ]; then
	dnrd -s 168.95.1.1
fi
echo "pppoe connect server manul"
