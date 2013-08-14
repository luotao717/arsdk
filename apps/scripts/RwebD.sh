#!/bin/sh

if [ $1 = 'option' ]; then
{
echo "0" >/tmp/webstatus
while [ true ];do

  sleep 10
# 	 echo "hahahahah1111"

	xx=`ps aux | grep goahead | grep -v grep`
	if [ "$xx" = "" ]; then 
    echo "the web error ,restart!"
		goahead zzz&	
	fi

   t1=`ps | grep "goahead" | grep "R"`
   if [ "$t1" = "" ]; then 
#    echo "the web status not R ,return!"
    echo "0" >/tmp/webstatus
    sleep 30
    continue
   fi
#   echo "haha2"
#   echo $t1
   if [ "$t1" != "" ]; then 
   	y1=`cat /tmp/webstatus`
   	if [ "$y1" = "0" ]; then 
#    	echo "the web status R ,first time!"
    	echo "1" >/tmp/webstatus
    	sleep 10
    	continue
    fi
    if [ "$y1" = "1" ]; then 
#    	echo "the web status R ,second time!"
    	echo "2" >/tmp/webstatus
    	sleep 5
    	continue
    fi
     if [ "$y1" = "2" ]; then 
#    	echo "the web status R ,third time!"
    	echo "3" >/tmp/webstatus
    	sleep 5
    	continue
    fi
     if [ "$y1" = "3" ]; then 
    	echo "the web status R ,forth time! so kill and restart it"
    	echo "0" >/tmp/webstatus
    	killall goahead
    	goahead zzz&
    	sleep 30
    fi
   fi
done
}&
fi

