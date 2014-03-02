# Change this flag to true to enable test
MODEM_SUPERVISOR_TEST:=false

ifeq ($(MODEM_SUPERVISOR_TEST), true)

#####################################################################
# Shell scripts for unitary tests                                   #
#####################################################################
PRODUCT_COPY_FILES += \
	$(LOCAL_PATH)/test/ts_msup.sh:root/system/bin/ts_msup.sh \
	$(LOCAL_PATH)/test/phonetTestApp:root/system/bin/phonetTestApp \
	$(LOCAL_PATH)/test/shmnetlnktestapp:root/system/bin/shmnetlnktestapp \
	$(LOCAL_PATH)/test/init.rc:root/system/bin/init.rc
endif
