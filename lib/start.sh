#!/bin/bash

basepath=$(cd `dirname $0`; pwd)
USER=`echo $basepath | cut -d \/ -f 3`
echo "user is $USER"
USERHOME=/home/$USER
BIN_PATH=$USERHOME/dss_bin
PKT_PATH=$USERHOME/dss_pkt
MAIN_PROCESS=pano360
UDISK_SERVER=udiskserver
UPGRADE_SERVER=upgrade_fw
WEIGHT_FOLDER=weight
JETSON_CLOCKS_SCRIPT=jetson_clocks.sh

echo "$USER" | nohup sudo -S mount /dev/sda1 /home/nvidia/calib/

if [ ! -d $BIN_PATH ];then
echo "$BIN_PATH not exist, now mkdir it."
mkdir $BIN_PATH
fi

if [ ! -d $PKT_PATH ];then
	echo "$PKT_PATH not exist，now mkdir it."
	mkdir $PKT_PATH
else
	chmod +x  $PKT_PATH/lib*.so
	echo "$USER" | nohup sudo -S mv $PKT_PATH/lib*.so  /usr/lib/
	echo "mv lib done."

	if [ -f $PKT_PATH/$UPGRADE_SERVER ];then
		echo "$USER" | nohup sudo -S mv $PKT_PATH/$UPGRADE_SERVER  $USERHOME/
		echo "mv $UPGRADE_SERVER done."
	else
		echo "no $UPGRADE_SERVER to move"
	fi

	if [ -f $PKT_PATH/$UDISK_SERVER ];then
		echo "$USER" | nohup sudo -S mv $PKT_PATH/$UDISK_SERVER  $USERHOME/
		echo "mv $UDISK_SERVER done."
	else
		echo "no $UDISK_SERVER to move"
	fi

	if [ -d $PKT_PATH/$WEIGHT_FOLDER ];then
		echo "$USER" | nohup sudo -S mv $PKT_PATH/$WEIGHT_FOLDER  $USERHOME/
		echo "mv $WEIGHT_FOLDER done."
	else
		echo "no $WEIGHT_FOLDER to move"
	fi

	echo "$USER" | nohup sudo -S mv $PKT_PATH/*  $BIN_PATH/
	echo "mv other file done."
fi

cd $BIN_PATH
echo "$USER" | nohup sudo -S insmod load_fpga.ko > /dev/null 2>&1 &
sleep 12
echo "load fpga done."

if test $( pgrep -f $UPGRADE_SERVER | wc -l ) -eq 0
then
echo "$UPGRADE_SERVER previous stat is closed."
else 
echo "$UPGRADE_SERVER is running, now kill it."
echo "$USER" | nohup sudo -S killall -9 $UPGRADE_SERVER 
fi 

if test $( pgrep -f $UDISK_SERVER | wc -l ) -eq 0
then
echo "$UDISK_SERVER previous stat is closed."
else 
echo "$UDISK_SERVER is running, now kill it."
echo "$USER" | nohup sudo -S killall -9 $UDISK_SERVER 
fi 

#if test $( pgrep -f $SUB_PROCESS | wc -l ) -eq 0
#then
#echo "$SUB_PROCESS previous stat is closed."
#else 
#echo "$SUB_PROCESS is running, now kill it."
#echo "$USER" | nohup sudo -S killall -9 $SUB_PROCESS 
#fi 

if test $( pgrep -f $MAIN_PROCESS | wc -l ) -eq 0
then
echo "$MAIN_PROCESS previous stat is closed."
else
echo "$MAIN_PROCESS is running, now kill it."
echo "$USER" | nohup sudo -S killall -9 $MAIN_PROCESS
fi 

echo "$USER" | nohup sudo -S ipcrm -Q 0x00010002
echo "$USER" | nohup sudo -S ipcrm -Q 0x01010002

echo "$USER" | nohup sudo -S chmod +x $BIN_PATH/setresol.sh
$BIN_PATH/setresol.sh

cd $USERHOME
echo "$USER" | nohup sudo -S chmod 777 $UPGRADE_SERVER
echo "$USER" | nohup sudo -S ./$UPGRADE_SERVER > /dev/null 2>&1 &

cd $USERHOME
echo "$USER" | nohup sudo -S chmod 777 $UDISK_SERVER
echo "$USER" | nohup sudo -S ./$UDISK_SERVER > /dev/null 2>&1 &

cd $BIN_PATH
#echo "$USER" | nohup sudo -S chmod 777 $SUB_PROCESS
echo "$USER" | nohup sudo -S chmod 777 $MAIN_PROCESS

#echo "$USER" | nohup sudo -S ./$SUB_PROCESS > /dev/null 2>&1 &
#sleep 1
echo "$USER" | nohup sudo -S ./$MAIN_PROCESS > /dev/null 2>&1 &

sleep 10
cd $USERHOME
echo "$USER" | nohup sudo -S ./$JETSON_CLOCKS_SCRIPT > /dev/null 2>&1 &
echo "jetson_clocks.sh is running."

