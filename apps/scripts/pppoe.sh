#!/bin/sh

if [ ! -n "$5" ]; then
  echo "insufficient arguments!"
  echo "Usage: $0 <user> <password> <eth_name> <mtu> <mru>"
  exit 0
fi

PPPOE_USER_NAME="$1"
PPPOE_PASSWORD="$2"
PPPOE_IF="$3"
PPPOE_MTU="$4"
PPPOE_MRU="$5"


PIDFILE="/etc/ppp/ppoe.pid"
PPPOE="/usr/sbin/pppoe"
PPPD="/usr/sbin/pppd"
PPPOE_PIDFILE="${PIDFILE}.pppoe"
PPPD_PIDFILE="${PIDFILE}.pppd"

ETH="$PPPOE_IF"
PPPOE_TIMEOUT="80"
PPPOE_SYNC=""
CLAMPMSS="-m 1412"
ACNAME=""
SERVICENAMEOPT=""
PPPOE_EXTRA=""
DEFAULTROUTE=defaultroute

PLUGIN_OPTS=""
PEERDNS=""
USER="$1"
LCP_INTERVAL="15"
LCP_FAILURE="3"
PPPD_EXTRA=""
PPPOE_EXTRA=""
DEMAND=""
PPPD_SYNC=""

PPPOE_CMD="$PPPOE -p $PPPOE_PIDFILE -I $ETH -T $PPPOE_TIMEOUT -U $PPPOE_SYNC $CLAMPMSS $ACNAME $SERVICENAMEOPT $PPPOE_EXTRA"
PPP_STD_OPTIONS="$PLUGIN_OPTS noipdefault noauth usepeerdns default-asyncmap $DEFAULTROUTE hide-password nodetach $PEERDNS mtu $PPPOE_MTU mru $PPPOE_MRU \
 noaccomp debug nodeflate nopcomp novj novjccomp user $PPPOE_USER_NAME lcp-echo-interval $LCP_INTERVAL lcp-echo-failure $LCP_FAILURE $PPPD_EXTRA"

mkdir -p /tmp/ppp
cp /etc/pppbak/* /tmp/ppp/
mkdir -p /var/local/tmp/ppp

$PPPD pty "$PPPOE_CMD" \
	    $PPP_STD_OPTIONS \
	    $DEMAND \
	    $PPPD_SYNC &
echo "$!" > $PPPD_PIDFILE



echo "$PPPOE_USER_NAME * $PPPOE_PASSWORD *" > /var/local/tmp/ppp/pap-secrets
echo "$PPPOE_USER_NAME * $PPPOE_PASSWORD *" > /var/local/tmp/ppp/chap-secrets
