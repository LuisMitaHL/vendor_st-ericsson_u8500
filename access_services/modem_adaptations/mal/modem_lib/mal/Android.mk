BASE_PATH := $(call my-dir)

DIRS := \
	$(BASE_PATH)/libphonet/Android.mk \
	$(BASE_PATH)/libisimessage/Android.mk \
	$(BASE_PATH)/libmalmon/Android.mk \
	$(BASE_PATH)/libmalmte/Android.mk \
	$(BASE_PATH)/libmalmce/Android.mk \
	$(BASE_PATH)/libmalnet/Android.mk \
	$(BASE_PATH)/libmalgss/Android.mk \
	$(BASE_PATH)/libmalmis/Android.mk \
	$(BASE_PATH)/libmalnvd/Android.mk \
	$(BASE_PATH)/libmalat/Android.mk \
	$(BASE_PATH)/libmalpipe/Android.mk \
	$(BASE_PATH)/libmalgpds/Android.mk \
	$(BASE_PATH)/libmalcs/Android.mk \
	$(BASE_PATH)/libmalsms/Android.mk \
	$(BASE_PATH)/libmaluicc/Android.mk \
	$(BASE_PATH)/libshmnetlnk/Android.mk \
	$(BASE_PATH)/libmalrf/Android.mk \
	$(BASE_PATH)/mal_util/Android.mk \
	$(BASE_PATH)/libmalftd/Android.mk

include $(DIRS)
