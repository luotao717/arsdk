#!/bin/sh

PPPOE_FILE=/etc/ppp/peers/adsl

if [ ! -n "$4" ]; then
  echo "insufficient arguments!"
  echo "Usage: $0 <user> <password> <eth_name> <opmode> <mtu>"
  exit 0
fi

PPPOE_USER_NAME="$1"
PPPOE_PASSWORD="$2"
PPPOE_IF="$3"
PPPOE_OPMODE="$4"
PPPOE_IDLETIME="$5"

echo "plugin /bin/rp-pppoe.so $PPPOE_IF" > $PPPOE_FILE
echo "lcp-echo-failure 3" >> $PPPOE_FILE
echo "lcp-echo-interval 15" >> $PPPOE_FILE
echo "nopcomp" >> $PPPOE_FILE
echo "novjccomp" >> $PPPOE_FILE
echo "noccp" >> $PPPOE_FILE
echo "defaultroute" >> $PPPOE_FILE
echo "usepeerdns" >> $PPPOE_FILE
echo "debug" >> $PPPOE_FILE

pppd call adsl user $PPPOE_USER_NAME password $PPPOE_PASSWORD
#echo "user '$PPPOE_USER_NAME'" >> $PPPOE_FILE
#echo "password '$PPPOE_PASSWORD'" >> $PPPOE_FILE
#echo "hide-password" >> $PPPOE_FILE
#echo "noipdefault" >> $PPPOE_FILE

#echo "nodetach" >> $PPPOE_FILE

#if [ $PPPOE_OPMODE == "KeepAlive" ]; then
#	echo "persist" >> $PPPOE_FILE
#elif [ $PPPOE_OPMODE == "OnDemand" ]; then
#	PPPOE_IDLETIME=`expr $PPPOE_IDLETIME \* 60`
#	echo "demand" >> $PPPOE_FILE
#	echo "idle $PPPOE_IDLETIME" >> $PPPOE_FILE
#fi
#echo "ipcp-accept-remote" >> $PPPOE_FILE 
#echo "ipcp-accept-local" >> $PPPOE_FILE 
#echo "ipv6 ," >> $PPPOE_FILE
#echo "ipv6cp-accept-local" >> $PPPOE_FILE 
#echo "ipv6cp-use-persistent" >> $PPPOE_FILE 

#echo "ktune" >> $PPPOE_FILE
#echo "default-asyncmap nopcomp noaccomp" >> $PPPOE_FILE
#echo "novj nobsdcomp nodeflate" >> $PPPOE_FILE
