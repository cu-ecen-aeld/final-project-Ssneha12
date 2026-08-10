#!/bin/sh

case "$1" in 
	start)
		echo "Loading PIR Sensor module"
		modprobe pir_sensor
		;;
	stop)
		echo "Unloading PIR Sensor module"
		rmmod pir_sensor
		;;
	*)
		echo "Usage:$0 {start|stop}"
		exit 1
		;;
esac

exit 0
