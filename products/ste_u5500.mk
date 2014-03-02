
# Superclass
$(call inherit-product, build/target/product/generic.mk)

# Overrides
PRODUCT_BRAND := ste
PRODUCT_NAME := ste_u5500
PRODUCT_DEVICE := u5500
PRODUCT_MANUFACTURER := ste
PRODUCT_LOCALES := \
	en_US \
	en_GB \
	en_CA \
	en_AU \
	en_NZ \
	en_SG \
	ja_JP \
	fr_FR \
	fr_BE \
	fr_CA \
	fr_CH \
	it_IT \
	it_CH \
	es_ES \
	de_DE \
	de_AT \
	de_CH \
	de_LI \
	nl_NL \
	nl_BE \
	cs_CZ \
	pl_PL \
	zh_CN \
	zh_TW \
	ru_RU \
	ko_KR \
	hdpi

PRODUCT_PACKAGE_OVERLAYS := vendor/st-ericsson/u5500/overlay
PRODUCT_PROPERTY_OVERRIDES := \
	 ro.com.google.clientidbase=android-ste \
	 ro.com.google.gmsversion=2.3_r5

## Product configration flags ##

# Enable features for signing verification needed on production fused hw
ENABLE_FEATURE_SIGNATURE_VERIFICATION ?= false

## end Product configuration flags ##

# Add packages to build.
# Module names for lights, sensors, gps, gralloc, overlay , copybit, audio_policy and audio.primary should always be
# hard coded to match TARGET_BOARD_PLATFORM in vendor/st-ericsson/u5500/BoardConfig.mk
PRODUCT_PACKAGES += STEBluetooth \
	LiveWallpapersPicker \
	LiveWallpapers \
	MagicSmokeWallpapers \
	VisualizationWallpapers \
	libRS \
	librs_jni \
	lbsd \
	libclientgps \
	gps.fairbanks \
	libamilatclient \
	lbsstelp \
	liblbstestengine \
	pff2cspsa \
	cspsalist \
	cspsa2map \
	map2cspsa \
	map2q \
	q2map \
	libnl \
	crda \
	regdbdump \
	regulatory.bin \
	85-regulatory.rules \
	cspsa-server \
	libcspsa \
	cspsa.conf \
	cspsa-cmd \
	libcspsa-core \
	libcspsa-ll-file \
	libasound_module_pcm_bluetooth \
	rfm-server \
	wfa_ca \
	wfa_dut \
	libwfa \
	libwfa_ca \
	libwfa_dut \
	Stk \
	uImage \
	u-boot.bin \
	u-boot-env.bin \
	cw1200_wlan.ko \
	kernel-firmware \
	smcl_ta \
	COPS_DATA_0.csd \
	gralloc.fairbanks \
	copybit.fairbanks \
	overlay.fairbanks \
	libste_adm \
	admsrv \
	tuning_server \
	ste-adm-test \
	libasound_module_pcm_steiop \
	adm-asound.conf \
	adm.sqlite \
	adm.sqlite-u8500_ab8500 \
	adm.sqlite-u8520_ab8505_v1 \
	adm.sqlite-u8520_ab8505_v2 \
	adm.sqlite-u9540_ab9540_v1 \
	cops_ta \
	copsdaemon \
	libcops \
	cops_cmd \
	mloader \
	modemfs.img \
	atrelay \
	libste-ril \
	libat \
	libstagefrighthw \
	st-ericsson-multimedia-package \
	lights.fairbanks \
	sensors.fairbanks \
	camera.fairbanks \
	maja_mid_android.conf \
	itp_start_tunnel.sh \
	itp_stop_tunnel.sh \
	libusb \
	libconfig \
	libconfig.so \
	libmid \
	libmidat \
	libmidat.so \
	mid \
	mid_dbus_cmd \
	mid_dbus_listener \
	mld \
	modem_dump_handler \
	mdh.conf \
	stop_console \
	libapr-1.so \
	ashd \
	fdcd \
	iptables-1.4.10 \
	stercd \
	    sterc_script_connect \
	    sterc_script_disconnect \
	    sterc_conf \
	    sterc_script_connect_alt \
	    sterc_conf_alt \
	    rt_tables \
	chargemode \
	FmRadioReceiver \
	FmRadioTransmitter \
	startup \
	ste-cg29xx_ctrl \
	hdmi_service_st \
	hdmid \
	busybox \
	BooksPhone \
	Camera \
	GenieWidget \
	Gmail \
	GoogleBackupTransport \
	GoogleCalendarSyncAdapter \
	GoogleContactsSyncAdapter \
	GoogleFeedback \
	GooglePartnerSetup \
	GoogleServicesFramework \
	GoogleQuickSearchBox \
	Maps \
	MediaUploader \
	NetworkLocation \
	SetupWizard \
	Street \
	Talk \
	Talk2 \
	Vending \
	MarketUpdater \
	VoiceSearch \
	YouTube \
	com.google.android.maps.jar \
	com.google.android.maps.xml \
	google_generic_update.txt \
	libtalk_jni \
	libvoicesearch \
	cspsa_image \
	libhdmi_service \
	libblt_hw \
	thermald \
	libthermal \
	libthermalutils \
	thermal_sensors.xml \
	wpa_supplicant.conf \
	audio_policy.fairbanks \
	audio.primary.fairbanks \
	libstagefright_soft_ste_aacdec \
	libstagefright_soft_ste_mp3dec \
	drmserver \
	libdrmframework \
	libdrmframework_jni \
	bsdtar \
	fwdClient


# FM Radio library name is dependent on whether it is RX+TX/RX/TX
# set one of these to true to get RX or TX only
FMRADIO_CG2900_SET_TX_ONLY := false
FMRADIO_CG2900_SET_RX_ONLY := false

ifeq ($(FMRADIO_CG2900_SET_RX_ONLY), true)
	PRODUCT_PACKAGES += libfmradio.cg2900_rx
else
ifeq ($(FMRADIO_CG2900_SET_TX_ONLY), true)
	PRODUCT_PACKAGES += libfmradio.cg2900_tx
else
	PRODUCT_PACKAGES += libfmradio.cg2900
endif
endif

# Pick up some sounds
include frameworks/base/data/sounds/OriginalAudio.mk

# Added resources for TextToSpeech utility.
include external/svox/pico/lang/all_pico_languages.mk

# Copy input device configuration files
PRODUCT_COPY_FILES += \
	vendor/st-ericsson/u5500/cyttsp-spi.idc:system/usr/idc/cyttsp-spi.idc

# Copy the blue-up.sh script to system/bin
PRODUCT_COPY_FILES += \
	vendor/st-ericsson/u5500/blue-up.sh:system/bin/blue-up.sh \
        vendor/st-ericsson/u5500/init.rc:root/init.rc \
	vendor/st-ericsson/u5500/init_bd_addr.sh:system/etc/init_bd_addr.sh \
	vendor/st-ericsson/u5500/media_profiles_u5500.xml:system/etc/media_profiles.xml

# CG2900 initialization script
PRODUCT_COPY_FILES += \
	vendor/st-ericsson/u5500/cg2900-channel_init.sh:system/bin/cg2900-channel_init.sh

# USB Device ID initialization script
PRODUCT_COPY_FILES += \
	vendor/st-ericsson/u5500/usbid_init.sh:system/bin/usbid_init.sh

# copy AGPS configuration files for lbs_core module (comment these since the files do not exist in the u5500 folder yet)
PRODUCT_COPY_FILES += \
	vendor/st-ericsson/u5500/LbsConfig.cfg:system/etc/LbsConfig.cfg \
	vendor/st-ericsson/u5500/LbsLogConfig.cfg:system/etc/LbsLogConfig.cfg \
	vendor/st-ericsson/u5500/cacert.txt:system/etc/cacert.txt \
	vendor/st-ericsson/u5500/LbsPltConfig.cfg:system/etc/LbsPltConfig.cfg

# Added platform feature permissions
PRODUCT_COPY_FILES += \
        frameworks/base/data/etc/android.hardware.touchscreen.multitouch.distinct.xml:system/etc/permissions/android.hardware.touchscreen.multitouch.distinct.xml \
        frameworks/base/data/etc/handheld_core_hardware.xml:system/etc/permissions/handheld_core_hardware.xml \
        frameworks/base/data/etc/android.hardware.camera.autofocus.xml:system/etc/permissions/android.hardware.camera.autofocus.xml \
        frameworks/base/data/etc/android.hardware.camera.flash-autofocus.xml:system/etc/permissions/android.hardware.camera.flash-autofocus.xml \
        frameworks/base/data/etc/android.hardware.camera.front.xml:system/etc/permissions/android.hardware.camera.front.xml \
        frameworks/base/data/etc/android.hardware.sensor.gyroscope.xml:system/etc/permissions/android.hardware.sensor.gyroscope.xml \
        frameworks/base/data/etc/android.hardware.sensor.light.xml:system/etc/permissions/android.hardware.sensor.light.xml \
        frameworks/base/data/etc/android.hardware.sensor.proximity.xml:system/etc/permissions/android.hardware.sensor.proximity.xml \
        frameworks/base/data/etc/android.hardware.sensor.barometer.xml:system/etc/permissions/android.hardware.sensor.barometer.xml \
        frameworks/base/data/etc/android.hardware.telephony.gsm.xml:system/etc/permissions/android.hardware.telephony.gsm.xml \
        frameworks/base/data/etc/android.hardware.location.gps.xml:system/etc/permissions/android.hardware.location.gps.xml \
        frameworks/base/data/etc/android.hardware.wifi.xml:system/etc/permissions/android.hardware.wifi.xml \
        frameworks/base/data/etc/android.hardware.wifi.direct.xml:system/etc/permissions/android.hardware.wifi.direct.xml \
        frameworks/base/data/etc/android.hardware.usb.accessory.xml:system/etc/permissions/android.hardware.usb.accessory.xml \
        frameworks/base/data/etc/android.hardware.usb.host.xml:system/etc/permissions/android.hardware.usb.host.xml \
	 packages/wallpapers/LivePicker/android.software.live_wallpaper.xml:system/etc/permissions/android.software.live_wallpaper.xml \
        frameworks/base/data/etc/com.stericsson.hardware.fm.receiver.xml:system/etc/permissions/com.stericsson.hardware.fm.receiver.xml \
        frameworks/base/data/etc/com.stericsson.hardware.fm.transmitter.xml:system/etc/permissions/com.stericsson.hardware.fm.transmitter.xml

# Copy the APN configuration file
PRODUCT_COPY_FILES += \
        vendor/st-ericsson/u5500/apns_u5500.xml:system/etc/apns-conf.xml

# Copy ALSA configuration files to rootfs
PRODUCT_COPY_FILES += \
	external/alsa-lib/src/conf/alsa.conf:system/usr/share/alsa/alsa.conf \
	external/alsa-lib/src/conf/cards/aliases.conf:system/usr/share/alsa/cards/aliases.conf \
	external/alsa-lib/src/conf/pcm/center_lfe.conf:system/usr/share/alsa/pcm/center_lfe.conf \
	external/alsa-lib/src/conf/pcm/default.conf:system/usr/share/alsa/pcm/default.conf \
	external/alsa-lib/src/conf/pcm/dmix.conf:system/usr/share/alsa/pcm/dmix.conf \
	external/alsa-lib/src/conf/pcm/dpl.conf:system/usr/share/alsa/pcm/dpl.conf \
	external/alsa-lib/src/conf/pcm/dsnoop.conf:system/usr/share/alsa/pcm/dsnoop.conf \
	external/alsa-lib/src/conf/pcm/front.conf:system/usr/share/alsa/pcm/front.conf \
	external/alsa-lib/src/conf/pcm/iec958.conf:system/usr/share/alsa/pcm/iec958.conf \
	external/alsa-lib/src/conf/pcm/modem.conf:system/usr/share/alsa/pcm/modem.conf \
	external/alsa-lib/src/conf/pcm/rear.conf:system/usr/share/alsa/pcm/rear.conf \
	external/alsa-lib/src/conf/pcm/side.conf:system/usr/share/alsa/pcm/side.conf \
	external/alsa-lib/src/conf/pcm/surround40.conf:system/usr/share/alsa/pcm/surround40.conf \
	external/alsa-lib/src/conf/pcm/surround41.conf:system/usr/share/alsa/pcm/surround41.conf \
	external/alsa-lib/src/conf/pcm/surround50.conf:system/usr/share/alsa/pcm/surround50.conf \
	external/alsa-lib/src/conf/pcm/surround51.conf:system/usr/share/alsa/pcm/surround51.conf \
	external/alsa-lib/src/conf/pcm/surround71.conf:system/usr/share/alsa/pcm/surround71.conf

# Copy the U-Boot splash screen
# If the U-boot splash image output path need is changed, the UBOOT_SPLASH_IMAGE_OUTPUT variable
# in BoardConfig.mk (vendor/st-ericsson/uxx00) also needs to be updated
PRODUCT_COPY_FILES += \
        vendor/st-ericsson/boot/u-boot/tools/logos/stericsson.bmp:splash.bin

# keyboard layouts
PRODUCT_COPY_FILES += \
	vendor/st-ericsson/u5500/STMPE-keypad.kl:system/usr/keylayout/STMPE-keypad.kl \
	vendor/st-ericsson/u5500/STMPE-keypad.kl:system/usr/keylayout/tc35893-kp.kl \
	vendor/st-ericsson/u5500/db5500-keypad.kl:system/usr/keylayout/db5500-keypad.kl \
	vendor/st-ericsson/u5500/ux500-ske-keypad.kl:system/usr/keylayout/ux500-ske-keypad.kl \
	vendor/st-ericsson/u5500/ux500-ske-keypad-qwertz.kl:system/usr/keylayout/ux500-ske-keypad-qwertz.kl \
	vendor/st-ericsson/u5500/AB8500_Hs_Button.kl:system/usr/keylayout/AB8500_Hs_Button.kl

# board specific init.rc
PRODUCT_COPY_FILES += \
	$(LOCAL_PATH)/init.rc:root/init.rc

# modem initialization script
PRODUCT_COPY_FILES += \
	vendor/st-ericsson/u5500/ste_modem.sh:system/etc/ste_modem.sh \
	vendor/st-ericsson/u5500/cspsa_gdfs_sync.sh:system/etc/cspsa_gdfs_sync.sh

# Copy the DBDT script file
PRODUCT_COPY_FILES += \
        vendor/st-ericsson/u5500/ste_dbdt.sh:system/etc/ste_dbdt.sh

# Copy the Shutdown MID script
PRODUCT_COPY_FILES += \
        vendor/st-ericsson/u5500/ste-kill-mid.sh:system/etc/ste-kill-mid.sh

# Automount SD-Card
PRODUCT_COPY_FILES += \
	vendor/st-ericsson/u5500/vold.fstab:system/etc/vold.fstab

# Bluetooth configs
PRODUCT_COPY_FILES += \
        system/bluetooth/data/qos_role_switch_blacklist.conf:system/etc/bluetooth/qos_role_switch_blacklist.conf \
        system/bluetooth/data/main.conf:system/etc/bluetooth/main.conf

# Windows USB Host Drivers
USB_PC_DRIVERS_SET_DRIVERS := STE-Linux-platform-usb-drivers-windows.zip

# Neven data
PRODUCT_COPY_FILES += \
	external/neven/Embedded/common/data/APIEm/Modules/RFFprec_501.bmd:system/usr/share/bmd/RFFprec_501.bmd

# Copy files for modules
$(call inherit-product-if-exists, vendor/st-ericsson/tools/platform/USB-PC-drivers/Windows/copy_files.mk)
$(call inherit-product-if-exists, vendor/st-ericsson/base_utilities/core/utilities/lua/copy_files.mk)
