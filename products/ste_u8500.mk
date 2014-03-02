

# Superclass
$(call inherit-product, build/target/product/generic.mk)

# Overrides
PRODUCT_BRAND := ste
PRODUCT_NAME := ste_u8500
PRODUCT_DEVICE := u8500
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

PRODUCT_PACKAGE_OVERLAYS := vendor/st-ericsson/u8500/overlay
PRODUCT_PROPERTY_OVERRIDES := \
 ro.com.google.clientidbase=android-ste

# Enable AAC 5.1 channel output
PRODUCT_PROPERTY_OVERRIDES += \
 media.aac_51_output_enabled=1 \
 qemu.hw.mainkeys=1

## Product configration flags ##

# Enable features for signing verification needed on production fused hw
ENABLE_FEATURE_SIGNATURE_VERIFICATION ?= false

## end Product configuration flags ##

# Add packages to build.
# Module names for lights, sensors, gps, gralloc, overlay, copybit, audio_policy, and
# audio.primary should always be hard coded to match TARGET_BOARD_PLATFORM in
# vendor/st-ericsson/u8500/BoardConfig.mk
PRODUCT_PACKAGES += STEBluetooth \
                    LiveWallpapersPicker \
                    LiveWallpapers \
                    MagicSmokeWallpapers \
                    VisualizationWallpapers \
                    libRS \
                    librs_jni \
                    lights.montblanc \
                    copybit.montblanc \
                    STMPE-keypad.kcm \
                    tc3589x-keypad.kcm \
                    ux500-ske-keypad.kcm \
                    ux500-ske-keypad-qwertz.kcm \
                    cat \
                    libapdu \
                    libpc \
                    libsim \
                    libsimcom \
                    simd \
                    libsimpb \
                    simpbd \
                    tapdu \
                    tcat \
                    tuicc \
                    uicc \
                    libmsl \
                    libaccsutil_log \
                        telephony_log_levels \
                    libaccsutil_security \
                    libaccsutil_char_conv \
                    libaccsutil_predial_checks \
                    libcn \
                    cn_server \
                        plmn.operator.list \
                        plmn.latam.list \
                    at_core \
                        system_id.cfg \
                        manuf_id.cfg \
                    ste_mad \
                        model_id.cfg \
                    libisimessage \
                    libisimessage.so \
                    libmalcs \
                    libmalcs.so \
                    libmalgpds \
                    libmalgpds.so \
                    libmalgss \
                    libmalgss.so \
                    libmalmce \
                    libmalmce.so \
                    libmalmis \
                    libmalmis.so \
                    libmalnet \
                    libmalnet.so \
                    libmalnvd \
                    libmalnvd.so \
                    libmalpipe \
                    libmalpipe.so \
                    libmalsms \
                    libmalsms.so \
                    libmaluicc \
                    libmaluicc.so \
                    libmalutil \
                    libmalrf \
                    libmalrf.so \
                    libmalftd \
                    libmalftd.so \
                    com.stericsson.ril.oem \
                    com.stericsson.ril.oem.xml \
                    STErilOemService \
                    libphonet \
                    libphonet.so \
                    libmal \
                    libsms_server \
                    sms_server \
                    sms_test_harness \
                    sms_stub \
                    lbsd \
                    libclientgps \
                    gps.montblanc \
                    liblcsclient \
                    libamilclient \
                    lbsstelp \
                    liblbstestengine \
                    libstecom \
                    libmpl \
                    libnlcom \
                    libpscc \
                    psccd \
                        stepscc_conf \
                    libsterc \
                    stercd \
                        sterc_script_connect \
                        sterc_script_disconnect \
                        sterc_conf \
                        sterc_script_connect_alt \
                        sterc_conf_alt \
                        sterc_script_connect_dun \
                        sterc_script_disconnect_dun \
                        rt_tables \
                    ppp_sterc \
                    ip \
                    copsdaemon \
                    libcops \
                    cops_cmd \
                    cops_data \
                    gen_sipc \
                    libstagefright_soft_ste_aacdec \
                    libstagefright_soft_ste_mp3dec \
                    libstagefright_soft_ste_wmapro_v10 \
                    nwm2gdf \
                    gdflist \
                    gdf2pff \
                    pff2cspsa \
                    cspsalist \
                    cspsa2nwm \
                    nwm2cspsa \
                    cspsa2qstore \
                    STEBluetooth \
                    libnl \
                    gralloc.montblanc \
                    overlay.montblanc \
                    hwcomposer.montblanc \
                    msa \
                    libste_adm \
                    admsrv \
                    tuning_server \
                    ste-adm-test \
                    libasound_module_pcm_steiop \
                    adm-asound.conf \
                    preload_adm.txt \
                    adm.sqlite \
                    adm.sqlite-u8500_ab8500 \
                    adm.sqlite-u8520_ab8505_v1 \
                    adm.sqlite-u8520_ab8505_v2 \
                    adm.sqlite-u9540_ab9540_v1 \
                    audio_policy.montblanc \
                    audio.primary.montblanc \
                    audio.a2dp.default \
                    libasound \
                    libasf \
                    crda \
                    intersect \
                    regdbdump \
                    regulatory.bin \
                    85-regulatory.rules \
                    cspsa-server \
                    libcspsa \
                    cspsa.conf \
                    cspsa-cmd \
                    libcspsa-core \
                    libcspsa-ll-file \
                    sensors.montblanc \
                    power.montblanc \
                    camera.montblanc \
                    libu300-ril \
                        ril_config \
                    libu300-parser \
                    libcalcdigest \
                    libverify \
                    libtee \
                    sap \
                    libsap_pts \
                    libsbc \
                    libasound_module_pcm_bluetooth \
                    libasound_module_ctl_bluetooth \
                    wfa_ca \
                    wfa_dut \
                    libwfa \
                    libwfa_ca \
                    libwfa_dut \
                    Stk \
                    libmalmon \
                    libmalmon.so \
                    libmalmte \
                    libmalmte.so \
                    libmlr \
                    modem_log_relay \
                    trace_auto.conf \
                    modem-supervisor \
                    autologcat \
                    autologcat.conf \
                    libthermal \
                    ThermalService \
                    rtc_calibration \
                    ATService \
                    libstagefrighthw \
                    uImage \
                    u-boot.bin \
                    u-boot-env.bin \
                    st-ericsson-multimedia-package \
                    cw1200_wlan.ko \
                    modemfs.img \
                    kernel-firmware \
                    smcl_ta \
                    COPS_DATA_0.csd \
                    cops_ta \
                    libarchive \
                    libarchive_fe \
                    bsdtar \
                    chargemode \
                    factoryreset \
                    hdmi_service_st \
                    hdmid \
                    battery_params \
                    ste-cg29xx_ctrl \
                    busybox \
                    cspsa_image \
                    libhdmi_service \
                    libblt_hw \
                    drmserver \
                    libdrmframework \
                    libdrmframework_jni \
                    Camera \
                    mmprobed \
                    RXN_IntApp \
                    stedump \
                    audio.usb.default \
                    make_ext4fs \
                    libtinyalsa \
                    tinyplay \
                    tinycap \
                    tinymix \
                    libsrec_jni

#Include the libaudiopolicy for legacy Audio HAL
ifneq ($(BOARD_USES_C_AUDIO_HAL), true)
	PRODUCT_PACKAGES += libaudiopolicy
endif

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

# Copy input device configurations
PRODUCT_COPY_FILES += \
    vendor/st-ericsson/u8500/synaptics_rmi4_i2c.idc:system/usr/idc/synaptics_rmi4_i2c.idc \
    vendor/st-ericsson/u8500/bu21013_ts.idc:system/usr/idc/bu21013_ts.idc \
    vendor/st-ericsson/u8500/cyttsp-spi.idc:system/usr/idc/cyttsp-spi.idc \
    vendor/st-ericsson/u8500/tp_ft5306.idc:system/usr/idc/tp_ft5306.idc


# Copy the blue-up.sh script to system/bin
PRODUCT_COPY_FILES += \
	vendor/st-ericsson/u8500/blue-up.sh:system/bin/blue-up.sh \
	vendor/st-ericsson/u8500/init.rc:root/init.rc \
	vendor/st-ericsson/u8500/init.st-ericsson.rc:root/init.st-ericsson.rc \
	vendor/st-ericsson/u8500/init.st-ericsson.usb.rc:root/init.st-ericsson.usb.rc \
	vendor/st-ericsson/u8500/init_bd_addr.sh:system/etc/init_bd_addr.sh \
	vendor/st-ericsson/u8500/media_profiles_u8500.xml:system/etc/media_profiles.xml \
	vendor/st-ericsson/u8500/media_codecs_u8500.xml:system/etc/media_codecs.xml

# CG2900 initialization script
PRODUCT_COPY_FILES += \
	vendor/st-ericsson/u8500/cg2900-channel_init.sh:system/bin/cg2900-channel_init.sh

# USB Device ID initialization script
PRODUCT_COPY_FILES += \
	vendor/st-ericsson/u8500/usbid_init.sh:system/bin/usbid_init.sh

# copy AGPS configuration files for lbs_core module
PRODUCT_COPY_FILES += \
	vendor/st-ericsson/u8500/LbsConfig.cfg:system/etc/LbsConfig.cfg \
	vendor/st-ericsson/u8500/LbsLogConfig.cfg:system/etc/LbsLogConfig.cfg \
	vendor/st-ericsson/u8500/LbsPgpsConfig.cfg:system/etc/LbsPgpsConfig.cfg \
	vendor/st-ericsson/u8500/cacert.txt:system/etc/cacert.txt \
	vendor/st-ericsson/u8500/LbsPltConfig.cfg:system/etc/LbsPltConfig.cfg

# Added platform feature permissions
PRODUCT_COPY_FILES += \
        frameworks/native/data/etc/android.hardware.touchscreen.multitouch.distinct.xml:system/etc/permissions/android.hardware.touchscreen.multitouch.distinct.xml \
        frameworks/native/data/etc/handheld_core_hardware.xml:system/etc/permissions/handheld_core_hardware.xml \
        frameworks/native/data/etc/android.hardware.camera.autofocus.xml:system/etc/permissions/android.hardware.camera.autofocus.xml \
        frameworks/native/data/etc/android.hardware.camera.flash-autofocus.xml:system/etc/permissions/android.hardware.camera.flash-autofocus.xml \
        frameworks/native/data/etc/android.hardware.camera.front.xml:system/etc/permissions/android.hardware.camera.front.xml \
        frameworks/native/data/etc/android.hardware.sensor.gyroscope.xml:system/etc/permissions/android.hardware.sensor.gyroscope.xml \
        frameworks/native/data/etc/android.hardware.sensor.light.xml:system/etc/permissions/android.hardware.sensor.light.xml \
        frameworks/native/data/etc/android.hardware.sensor.proximity.xml:system/etc/permissions/android.hardware.sensor.proximity.xml \
        frameworks/native/data/etc/android.hardware.sensor.barometer.xml:system/etc/permissions/android.hardware.sensor.barometer.xml \
        frameworks/native/data/etc/android.hardware.telephony.gsm.xml:system/etc/permissions/android.hardware.telephony.gsm.xml \
        frameworks/native/data/etc/android.hardware.location.gps.xml:system/etc/permissions/android.hardware.location.gps.xml \
        frameworks/native/data/etc/android.hardware.wifi.xml:system/etc/permissions/android.hardware.wifi.xml \
        frameworks/native/data/etc/android.hardware.wifi.direct.xml:system/etc/permissions/android.hardware.wifi.direct.xml \
        frameworks/native/data/etc/android.hardware.usb.accessory.xml:system/etc/permissions/android.hardware.usb.accessory.xml \
        frameworks/native/data/etc/android.hardware.usb.host.xml:system/etc/permissions/android.hardware.usb.host.xml \
        packages/wallpapers/LivePicker/android.software.live_wallpaper.xml:system/etc/permissions/android.software.live_wallpaper.xml \
        frameworks/base/data/etc/com.stericsson.hardware.fm.receiver.xml:system/etc/permissions/com.stericsson.hardware.fm.receiver.xml \
        frameworks/base/data/etc/com.stericsson.hardware.fm.transmitter.xml:system/etc/permissions/com.stericsson.hardware.fm.transmitter.xml

# Copy the APN configuration file
PRODUCT_COPY_FILES += \
        vendor/st-ericsson/u8500/apns_u8500.xml:system/etc/apns-conf.xml

# Copy the U-Boot splash screen
# If the U-boot splash image output path need is changed, the UBOOT_SPLASH_IMAGE_OUTPUT variable
# in BoardConfig.mk (vendor/st-ericsson/uxx00) also needs to be updated
PRODUCT_COPY_FILES += \
   vendor/snda/products/logos/boot-snda.bmp:splash.bin

# keyboard layouts
PRODUCT_COPY_FILES += \
	vendor/st-ericsson/u8500/STMPE-keypad.kl:system/usr/keylayout/STMPE-keypad.kl \
	vendor/st-ericsson/u8500/tc3589x-keypad.kl:system/usr/keylayout/tc3589x-keypad.kl \
	vendor/st-ericsson/u8500/ux500-ske-keypad.kl:system/usr/keylayout/ux500-ske-keypad.kl \
	vendor/st-ericsson/u8500/ux500-ske-keypad-qwertz.kl:system/usr/keylayout/ux500-ske-keypad-qwertz.kl \
	vendor/st-ericsson/u8500/AB8500_Hs_Button.kl:system/usr/keylayout/AB8500_Hs_Button.kl

# product specific ueventd.rc
PRODUCT_COPY_FILES += \
	vendor/st-ericsson/u8500/ueventd.st-ericsson.rc:root/ueventd.st-ericsson.rc

# RIL initialization script
PRODUCT_COPY_FILES += \
	vendor/st-ericsson/u8500/ste_modem.sh:system/etc/ste_modem.sh

# ZRAM config
PRODUCT_COPY_FILES += \
	vendor/st-ericsson/u8500/zram_config.sh:system/etc/zram_config.sh
# DBus config
PRODUCT_COPY_FILES += vendor/st-ericsson/u8500/dbus.conf:system/etc/dbus.conf

# Automount SD-Card
PRODUCT_COPY_FILES += \
	vendor/st-ericsson/u8500/vold.fstab:system/etc/vold.fstab

# Bluetooth configs
PRODUCT_COPY_FILES += \
	system/bluetooth/data/qos_role_switch_blacklist.conf:system/etc/bluetooth/qos_role_switch_blacklist.conf \
	system/bluetooth/data/main.conf:system/etc/bluetooth/main.conf

# AVAHI configuration
AVAHI_ENABLE_FEATURE_DAEMON_CONF := false
AVAHI_ENABLE_FEATURE_AUTOIP_ACTION := false

# Windows USB Host Drivers
USB_PC_DRIVERS_SET_DRIVERS := STE-Linux-platform-usb-drivers-windows.zip

# Neven data
PRODUCT_COPY_FILES += \
	external/neven/Embedded/common/data/APIEm/Modules/RFFprec_501.bmd:system/usr/share/bmd/RFFprec_501.bmd

# Copy files for modules
$(call inherit-product-if-exists, vendor/st-ericsson/access_services/modem_adaptations/modem-supervisor/copy_files.mk)
$(call inherit-product-if-exists, vendor/st-ericsson/external/DNSmulticast/avahi/avahi-autoipd/copy_files.mk)
$(call inherit-product-if-exists, vendor/st-ericsson/external/DNSmulticast/avahi/avahi-daemon/copy_files.mk)
$(call inherit-product-if-exists, vendor/st-ericsson/tools/platform/USB-PC-drivers/Windows/copy_files.mk)
$(call inherit-product-if-exists, vendor/st-ericsson/base_utilities/core/utilities/lua/copy_files.mk)

# Google Mobile Services (GMS)
$(call inherit-product-if-exists, vendor/google/products/gms.mk)
