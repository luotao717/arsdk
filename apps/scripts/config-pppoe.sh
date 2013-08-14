#!/bin/sh
#
# $Id: config-pppoe.sh,v 1.3 2008-01-14 09:37:12 winfred Exp $
#
# usage: config-pppoe.sh <user> <password> <wan_if_name <mtu> <mru>>
#

usage()
{
	echo "Usage:"
	echo "  $0 <user> <password> <wan_if_name> <mtu> <mru>"
	exit 1
}

if [ "$5" = "" ]; then
	echo "$0: insufficient arguments"
	usage $0
fi

DNRD_PID=/var/run/dnrd.pid
killall -9 dnrd
rm -f /tmp/connectPass
#syslogd -m 0
pppoe.sh $1 $2 $3 $4 $5
if [ ! -f $DNRD_PID ]; then
	dnrd -s 168.95.1.1
fi
#pppoe.sh $1 $2 $3 by 2.4
pppConnectType=`nvram_get 2860 pppConnectType`
if [ "$pppConnectType" = "2" ]; then
	echo "pppoe manul type"
else
	echo "pppoe continue type"
fi
#iptables -A FORWARD -p tcp --tcp-flags SYN,RST SYN -j TCPMSS --clamp-mss-to-pmtu


