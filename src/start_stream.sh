#!/bin/sh

#Configuring Media controller pipeline
  
media-ctl -d /dev/media0 --set-v4l2 '"imx219 1-0010":0[fmt:SRGGB10_1X10/640x480]'
media-ctl -d /dev/media0 --set-v4l2 '"dw-csi.0":0[fmt:SRGGB10_1X10/640x480]'
media-ctl -d /dev/media0 --set-v4l2 '"csi2dc":0[fmt:SRGGB10_1X10/640x480]'
media-ctl -d /dev/media0 --set-v4l2 '"microchip_isc_scaler":0[fmt:SRGGB10_1X10/640x480]'

#GStreamer Pipeline 
gst-launch-1.0 \
            v4l2src device=/dev/video0 \
                ! video/x-raw,width=640,height=480,format=RGB16 \
                    ! videoconvert \
                        ! x264enc tune=zerolatency speed-preset=ultrafast bitrate=5000 \
                            ! rtph264pay pt=96 \
                                ! udpsink host=192.168.10.22 port=5000

#Load custom PIR driver and check interrupt generation
modprobe pir_sensor
