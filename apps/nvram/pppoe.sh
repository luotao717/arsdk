#!/bin/sh

PPPOE_FILE=/etc/options.pppoe

if [ ! -n "$5" ]; then
  echo "insufficient arguments!"
  echo "Usage: $0 <user> <password> <eth_name> <mtu> <mru>"
  exit 0
fi
#if [ ! -n "$3" ]; then
#  echo "insufficient arguments!"
#  echo "Usage: $0 <user> <password> <eth_name>"
#  exit 0
#fi
# changed by 2.4(use this script in 2.3)

PPPOE_USER_NAME="$1"
PPPOE_PASSWORD="$2"
PPPOE_IF="$3"
PPPOE_MTU="$4"
PPPOE_MRU="$5"
echo "noauth" > $PPPOE_FILE
echo "user '$PPPOE_USER_NAME'" >> $PPPOE_FILE
echo "password '$PPPOE_PASSWORD'" >> $PPPOE_FILE
#nvram_pppoe_option在配置文件中输入用户名密码
#nvram_pppoe_option /etc/options.pppoe
echo "nomppe" >> $PPPOE_FILE
echo "hide-password" >> $PPPOE_FILE
echo "noipdefault" >> $PPPOE_FILE
echo "defaultroute" >> $PPPOE_FILE
echo "nodetach" >> $PPPOE_FILE
echo "usepeerdns" >> $PPPOE_FILE
ServiceName=`nvram_get 2860 wan_pppoe_server_name`
pppoe_specific=`nvram_get 2860 pppoe_specific`
pppConnectType=`nvram_get 2860 pppConnectType`
if [ "$pppConnectType" = "1" ]; then
	echo "pppoe need type"
	pppIdleTime=`nvram_get 2860 pppIdleTime`
	pppIdleTime=`expr $pppIdleTime \* 60`
	echo "demand" >> $PPPOE_FILE
	echo "idle $pppIdleTime" >> $PPPOE_FILE
elif [ "$pppConnectType" = "0" ]; then
	echo "pppoe keep-alive type"
	echo "persist" >> $PPPOE_FILE
	echo "holdoff 60" >> $PPPOE_FILE
	if [ "$pppoe_specific" = "2" ]; then
		echo "maxfail 2" >> $PPPOE_FILE
	else
		echo "maxfail 0" >> $PPPOE_FILE
	fi

#	echo "holdoff $PPPOE_OPTIME" >> $PPPOE_FILE
else
	echo "maxfail 2" >> $PPPOE_FILE
fi
#//moved by 2.4 but we still use this
echo "mru $PPPOE_MRU" >> $PPPOE_FILE  
echo "mtu $PPPOE_MTU" >> $PPPOE_FILE
echo "ipcp-accept-remote" >> $PPPOE_FILE 
echo "ipcp-accept-local" >> $PPPOE_FILE 
echo "lcp-echo-failure 3" >> $PPPOE_FILE
echo "lcp-echo-interval 40" >> $PPPOE_FILE
#echo "lcp-max-configure 5" >> $PPPOE_FILE
#echo "lcp-max-failure 5" >> $PPPOE_FILE
echo "ktune" >> $PPPOE_FILE
echo "default-asyncmap nopcomp noaccomp" >> $PPPOE_FILE
echo "novj nobsdcomp nodeflate" >> $PPPOE_FILE
if [ -n "$ServiceName" ]; then
  echo "plugin /etc/ppp/plugins/rp-pppoe.so rp_pppoe_service $ServiceName $PPPOE_IF" >> $PPPOE_FILE
else
  echo "plugin /etc/ppp/plugins/rp-pppoe.so $PPPOE_IF" >> $PPPOE_FILE
fi


