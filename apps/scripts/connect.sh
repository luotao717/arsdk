#!/bin/sh
#eval `flash get DNS_MODE`
#eval `flash get WAN_DHCP`
#eval `flash get PPP_MTU_SIZE`
#eval `flash get PPTP_MTU_SIZE`
RESOLV=/etc_ro/resolv.conf
PIDFILE=/var/run/dnrd.pid
#CONNECTFILE=/etc/ppp/connectfile
wan_mode=`nvram_get 2860 wanConnectionMode`
#echo "pass" > $CONNECTFILE

if [ "$wan_mode" = "PPTP" ]; then
  ptpgw0=`ifconfig ppp0 | grep -i "P-t-P:"`
  ptpgw1=`echo $ptpgw0 | cut -f3 -d:`
  ptpgw=`echo $ptpgw1 | cut -f1 -d " "`
  route add -net default gw $ptpgw dev ppp0
fi

#if [ $DNS_MODE != 1 ]; then
  if [ -r "$RESOLV" ] ; then
    if [ -f $PIDFILE ]; then
      PID=`cat $PIDFILE`
      kill -9 $PID 
      rm -f $PIDFILE
    fi
    line=0
    cat $RESOLV | grep nameserver > /var/ddfile 
    line=`cat /var/ddfile | wc -l`
    num=1
    while [ $num -le $line ];
    do
      pat0=` head -n $num /var/ddfile | tail -n 1`
      pat1=`echo $pat0 | cut -f2 -d " "`
      DNS="$DNS -s $pat1"
      num=`expr $num + 1`
    done
    num=1
    lanip=`nvram_get 2860 lan_ipaddr`
    while [ $num -le 5 ];
    do
      dnrd $DNS -a $lanip -c 100:300 -m off
      if [ -f $PIDFILE ]; then
        break
      else
        sleep 1
        num=`expr $num + 1`
      fi
    done
  fi
ddns.sh&
#fi
#if [ $WAN_DHCP = 4 ]; then
#  ifconfig ppp0 mtu $PPTP_MTU_SIZE txqueuelen 25
#else
#  ifconfig ppp0 mtu $PPP_MTU_SIZE txqueuelen 25
#fi
#upnp.sh
#if [ -f /bin/vpn.sh ]; then
#      echo 'Setup VPN'
#      vpn.sh all
#fi
