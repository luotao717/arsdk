#!/bin/sh

PPPD_FILE=/tmp/pppd
pppoe_specific=`nvram_get 2860 pppoe_specific`
u=`nvram_get 2860 wan_pppoe_Encrypt_user`
pw=`nvram_get 2860 wan_pppoe_pass`
mtu=`nvram_get 2860 wan_pppoe_mtu`
mru=`nvram_get 2860 wan_pppoe_mru`
while [ "$pppoe_specific" = "2" ]
do
        if [ ! -f "$PPPD_FILE" ]; then
        echo "PPPD IS STOP,run pppd"
        ncxkjs $u
        config-pppoe.sh user $pw eth2.2 $mtu $mru
        fi
        sleep 3
        pppoe_specific=`nvram_get 2860 pppoe_specific`
done
