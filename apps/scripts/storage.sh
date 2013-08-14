#!/bin/sh
#
# $Id: storage.sh,v 1.23 2008-07-01 02:52:11 chhung Exp $
#
# usage: storage.sh
#
#
PART1=""
#PART1="/var"
for part in a b c d e f g h i j k l m n o p q r s t u v w x y z
do
	for index in 1 2 3 4 5 6 7 8 9
	do
		if [ -e "/media/sd$part$index" ]; then
			PART1="/media/sd$part$index"
			break;
		fi
	done
	if [ "$PART1" != "" ]; then
		break;
	fi
done

setUser()
{
	if [ ! -e "$PART1/home/" ]; then
		mkdir "$PART1/home/"
	fi
	for index in 1 2 3 4 5 6 7 8
	do
		user=`nvram_get 2860 "User$index"`
		base=500
		id=`expr $base + $index`
		if [ "$user" ]; then
			echo "$user::$id:$id:$user:$PART1/home/$user:/bin/sh" >> /etc/passwd
			echo "$user:x:$id:$user" >> /etc/group
			if [ ! -e "$PART1/home/$user" ]; then
				mkdir "$PART1/home/$user"
			fi
			chmod 777 "$PART1/home/$user"
		fi
	done
}

setFtp()
{
	ftpport=`nvram_get 2860 FtpPort`
	maxuser=`nvram_get 2860 FtpMaxUsers`
	loginT=`nvram_get 2860 FtpLoginTimeout`
	stayT=`nvram_get 2860 FtpStayTimeout`
	stupid-ftpd-common.sh "$ftpport" "$maxuser" "$loginT" "$stayT"
	echo "stupid-ftpd-common.sh "$ftpport" "$maxuser" "$loginT" "$stayT""
	admID=`nvram_get 2860 Login`
	admPW=`nvram_get 2860 Password`
	stupid-ftpd-user.sh "$admID" "$admPW" / 3 A
	echo "stupid-ftpd-user.sh "$admID" "$admPW" / 3 A"
	anonymous=`nvram_get 2860 FtpAnonymous`
	if [ "$anonymous" = "1" ]; then
		stupid-ftpd-user.sh anonymous "*" /tmp 3 D	
		echo "stupid-ftpd-user.sh anonymous "*" /tmp 3 D"
	fi
	if [ -e "$PART1" ]; then
		for index in 1 2 3 4 5 6 7 8
		do
			user=`nvram_get 2860 "User$index"`
			ftpuser=`nvram_get 2860 "FtpUser$index"`
			if [ "$user" -a "$ftpuser" = "1" ]; then
				pw=`nvram_get 2860 "UserPasswd$index"`
				max=`nvram_get 2860 "FtpMaxLogins$index"`
				mode=`nvram_get 2860 "FtpMode$index"`
				stupid-ftpd-user.sh "$user" "$pw" "$PART1/$user" "$max" "$mode"
				echo "stupid-ftpd-user.sh "$user" "$pw" "$PART1/$user" "$max" "$mode""
			fi
		done
	fi
}

setSmb()
{
	smbnetbios=`nvram_get 2860 SmbNetBIOS`
	smbwg=`nvram_get 2860 HostName`
	samba.sh "$smbnetbios" "$smbwg" 
	echo "samba.sh "$smbnetbios" "$smbwg""
	admID=`nvram_get 2860 Login`
	admPW=`nvram_get 2860 Password`
	smbpasswd -a "$admID" "$admPW"
	echo "smbpasswd -a "$admID" "$admPW""
	allusers="$admID"
	if [ -e "$PART1" ]; then
		for index in 1 2 3 4 5 6 7 8
		do
			user=`nvram_get 2860 "User$index"`
			smbuser=`nvram_get 2860 "SmbUser$index"`
			if [ "$user" -a "$smbuser" = "1" ]; then
				pw=`nvram_get 2860 "UserPasswd$index"`
				smbpasswd -a "$user" "$pw"
				echo "smbpasswd -a "$user" "$pw""
				allusers="$allusers $user"
			fi
		done
	fi
	if [ ! -e "$PART1/public" ]; then
		mkdir "$PART1/public"
	fi
	chmod 777 "$PART1/public"
	samba_add_dir.sh Public "$PART1/public" "$allusers"
	echo "samba_add_dir.sh Public "$PART1/public" "$allusers""
}

case $1 in
	"admin")
		admID=`nvram_get 2860 Login`
		admPW=`nvram_get 2860 Password`
		echo "$admID::0:0:Adminstrator:/:/bin/sh" > /etc/passwd
		echo "$admID:x:0:$admID" > /etc/group
		chpasswd.sh $admID $admPW
		chmod 777 /tmp
		if [ -e "$PART1" ]; then
			setUser
		fi
		;;
	"adddir")
		if [ -n "$2" ]; then
			if [ ! -e "$2" ]; then
				mkdir "$2"
				chmod 777 "$2"
			fi
		fi
		;;
	"deldir")
		if [ -n "$2" ]; then
			rm -rf "$2"
		fi
		;;
	"reparted")
		fdisk -D /dev/sda
		echo "fdisk -D /dev/sda"
		sleep 1
		if [ "$2" -gt "0" ]; then
			fdisk /dev/sda -n $2 -p 1
			echo "fdisk /dev/sda -n $2 -p 1"
		fi
		sleep 1
		if [ "$3" -gt "0" ]; then
			fdisk /dev/sda -n $3 -p 2
			echo "fdisk /dev/sda -n $3 -p 2"
		fi
		sleep 1
		if [ "$4" -gt "0" ]; then
			fdisk /dev/sda -n $4 -p 3
			echo "fdisk /dev/sda -n $4 -p 3"
		fi
		sleep 1
		if [ "$5" -gt "0" ]; then
			fdisk /dev/sda -n $5 -p 4
			echo "fdisk /dev/sda -n $5 -p 4"
		fi
		reboot
		;;	
	"ftp")
		killall -q stupid-ftpd
		ftpenabled=`nvram_get 2860 FtpEnabled`
		if [ "$ftpenabled" = "1" ]; then
			setFtp
			stupid-ftpd
			echo "stupid-ftpd"
		fi
		;;
	"samba")
		if [ "$2" = "set" ]; then
			killall -q nmbd
			killall -q smbd
			setSmb
		elif [ "$2" = "start" ]; then
			smbenabled=`nvram_get 2860 SmbEnabled`
			if [ "$smbenabled" = "1" ]; then
				nmbd
				echo "nmbd"
				smbd
				echo "smbd"
			fi
		elif [ "$2" = "stop" ]; then
			killall -q nmbd
			killall -q smbd
		fi
		;;
	"media")
		killall -q ushare
		media_enabled=`nvram_get 2860 mediaSrvEnabled`
		media_name=`nvram_get 2860 mediaSrvName`
		if [ "$media_enabled" = "1" ]; then
			ushare.sh $media_name "$2" "$3" "$4" "$5"
			echo "ushare.sh $media_name "$2" "$3" "$4" "$5""
			ushare -D
			echo "ushare -D"
		fi
		;;
esac
