#!/bin/sh

# place the commands to run after the pppd dial up successfully.

#if [ -x /bin/qos_run ]; then
#	echo "/bin/qos_run"
#	/bin/qos_run
#fi

#if [ -x /sbin/ddns.sh ]; then
#	echo "/sbin/ddns.sh"
#	/sbin/ddns.sh
#fi

#if [ -x /sbin/config-udhcpd.sh ]; then
#	echo "/sbin/config-udhcpd.sh $DNS1 $DNS2"
#	/sbin/config-udhcpd.sh -d $DNS1 $DNS2
#fi

# notify goahead that WAN IP has been acquired/updated.
killall -SIGTSTP goahead
