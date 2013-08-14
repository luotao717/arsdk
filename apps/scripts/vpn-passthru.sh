#!/bin/sh

l2tp_pt=`nvram_get 2860 l2tpPassThru`
ipsec_pt=`nvram_get 2860 ipsecPassThru`
pptp_pt=`nvram_get 2860 pptpPassThru`

# note: they must be removed in order
rmmod ip_nat_proto_gre
rmmod ip_nat_pptp
rmmod ip_conntrack_pptp
rmmod ip_conntrack_proto_gre


if [ "$pptp_pt" = "1" -o "$l2tp_pt" = "1" -o "$ipsec_pt" = "1" ]; then
	insmod -q ip_conntrack_proto_gre
	insmod -q ip_nat_proto_gre

	if [ "$pptp_pt" = "1" ]; then
		insmod -q ip_conntrack_pptp
		insmod -q ip_nat_pptp
	fi
fi

