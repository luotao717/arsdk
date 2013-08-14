#!/bin/sh
#
# $Id: config-igmpproxy.sh,v 1.3.4.1 2008-08-01 03:56:20 steven Exp $
#
# usage: config-igmpproxy.sh <wan_if_name> <lan_if_name>
#

. /sbin/global.sh

usage()
{
	echo "Usage:"
	echo "  $0 <wan_if_name> <lan_if_name>"
	exit 1
}

if [ "$2" = "" ]; then
	echo "$0: insufficient arguments"
	usage $0
fi

igmpproxy.sh $wan_if $lan_if ppp0
igmpproxy

