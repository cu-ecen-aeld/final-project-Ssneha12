PIR_SENSOR_SITE=$(BR2_EXTERNAL_MCHP_PATH)/package/pir-sensor
PIR_SENSOR_SITE_METHOD=local
PIR_SENSOR_VERSION =1.0
PIR_SENSOR_LICENSE = GPL-2.0

$(eval $(kernel-module))
$(eval $(generic-package))
