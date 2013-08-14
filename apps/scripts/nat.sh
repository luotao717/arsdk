#!/bin/sh
#
# $Id: nat.sh,v 1.1 2007-09-26 01:33:21 winfred Exp $
#
# usage: nat.sh
#

. /sbin/global.sh

lan_ip=`nvram_get 2860 lan_ipaddr`
nat_en=`nvram_get 2860 natEnabled`


echo 1 > /proc/sys/net/ipv4/ip_forward

if [ "$nat_en" = "1" ]; then
	echo 1500 > /proc/sys/net/ipv4/netfilter/ip_conntrack_max
	echo 30 > /proc/sys/net/ipv4/netfilter/ip_conntrack_udp_timeout
	echo 5 > /proc/sys/net/ipv4/netfilter/ip_conntrack_tcp_timeout_time_wait
	echo 60 > /proc/sys/net/ipv4/netfilter/ip_conntrack_tcp_timeout_fin_wait 
	echo 450 > /proc/sys/net/ipv4/netfilter/ip_conntrack_tcp_timeout_established
#	echo 180 > /proc/sys/net/ipv4/netfilter/ip_conntrack_tcp_timeout_established
	if [ "$wanmode" = "PPPOE" -o "$wanmode" = "L2TP" -o "$wanmode" = "PPTP" ]; then
		wan_if="ppp0"
	else
		wan_if="br1"
	fi
	iptables -t nat -A POSTROUTING -s $lan_ip/8 -o $wan_if -j MASQUERADE
#	iptables -t nat -A POSTROUTING -o $wan_if -j MASQUERADE
fi

