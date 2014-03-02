
# Superclass
$(call inherit-product, build/target/product/generic.mk)

# Overrides
PRODUCT_BRAND := ste
PRODUCT_NAME := ste_u9500_100
PRODUCT_DEVICE := u9500_100
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

PRODUCT_PACKAGE_OVERLAYS := vendor/st-ericsson/u9500_100/overlay
PRODUCT_PROPERTY_OVERRIDES := \
 ro.com.google.clientidbase=android-ste \
 ro.com.google.gmsversion=2.3_r5

## Product configration flags ##

# Enable features for signing verification needed on production fused hw
ENABLE_FEATURE_SIGNATURE_VERIFICATION ?= false

## end Product configuration flags ##

# Call script to create/modify u9500_defconfig
UPDATE_KERNEL_DEFCONFIG_FILE:= $(shell test -f vendor/st-ericsson/$(PRODUCT_DEVICE)/patch_kernel_defconfig.sh && sh vendor/st-ericsson/$(PRODUCT_DEVICE)/patch_kernel_defconfig.sh)

# Add packages to build.
# Module names for lights, sensors, gps, gralloc, overlay, copybit, audio_policy, and
# audio.primary should always be hard coded to match TARGET_BOARD_PLATFORM in
# vendor/st-ericsson/u9500_100/BoardConfig.mk
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
                    model_id.cfg \
                    libisimessage \
                    libmalcs \
                    libmalgpds \
                    libmalgss \
                    libmalmce \
                    libmalmis \
                    libmalnet \
                    libmalnvd \
                    libmalpipe \
                    libmalsms \
                    libmaluicc \
                    libphonet \
                    libmal \
                    libsms_server \
                    sms_server \
                    sms_test_harness \
                    sms_stub \
                    lbsd \
                    libclientgps \
                    gps.montblanc \
                    libamilatclient \
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
                    libstagefright_soft_ste_aacdec \
                    libstagefright_soft_ste_mp3dec \
                    nwm2gdf \
                    gdflist \
                    gdf2pff \
                    pff2cspsa \
                    cspsalist \
                    cspsa2nwm \
                    cspsa2qstore \
                    STEBluetooth \
                    libnl \
                    gralloc.montblanc \
                    overlay.montblanc \
                    msa \
                    libril-anm \
                    libaudiopolicy \
                    audio_policy.montblanc \
                    audio.primary.montblanc \
                    libasound \
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
                    camera.montblanc \
                    libste-ril \
                    libu300-parser \
                    libcalcdigest \
                    libverify \
                    libtee \
                    sap \
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
                    libmalmte \
                    libmlr \
                    modem_log_relay \
                    mlr_files \
                    modem-supervisor \
                    libthermal \
                    ThermalService \
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
                    wpa_supplicant.conf \
                    drmserver \
                    libdrmframework \
                    libdrmframework_jni \
                    mmprobed


# MIS modules for u9500 modem
PRODUCT_PACKAGES += \
                    Flashed_mid.conf \
                    Flashless_mid.conf \
                    libusb \
                    libconfig \
                    libmid \
                    libmoliU33x \
                    libmoliU5xx \
                    libmoliU7xx \
                    libat \
                    mid \
                    mid_dbus_cmd \
                    mid_dbus_listener \
                    libmoliU33x.so \
                    libmoliU5xx.so \
                    libmoliU7xx.so \
                    libat.so \
                    libconfig.so \
                    libusb.so \
                    libapr-1.so \
                    ldiscd \
                    mld \
                    bridge_relay \
                    rfm-server \

#                    FmRadioReceiver \
#                    FmRadioTransmitter \


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
    vendor/st-ericsson/u9500_100/synaptics_rmi4_i2c.idc:system/usr/idc/synaptics_rmi4_i2c.idc \
    vendor/st-ericsson/u9500_100/bu21013_ts.idc:system/usr/idc/bu21013_ts.idc \
    vendor/st-ericsson/u9500_100/cyttsp-spi.idc:system/usr/idc/cyttsp-spi.idc

# Copy the blue-up.sh script to system/bin
PRODUCT_COPY_FILES += \
	vendor/st-ericsson/u9500_100/blue-up.sh:system/bin/blue-up.sh \
	vendor/st-ericsson/u9500_100/init.rc:root/init.rc \
	vendor/st-ericsson/u9500_100/init_bd_addr.sh:system/etc/init_bd_addr.sh \
	vendor/st-ericsson/u9500_100/media_profiles_u8500.xml:system/etc/media_profiles.xml

# CG2900 initialization script
PRODUCT_COPY_FILES += \
	vendor/st-ericsson/u9500_100/cg2900-channel_init.sh:system/bin/cg2900-channel_init.sh

# USB Device ID initialization script
PRODUCT_COPY_FILES += \
	vendor/st-ericsson/u9500_100/usbid_init.sh:system/bin/usbid_init.sh

# copy AGPS configuration files for lbs_core module
PRODUCT_COPY_FILES += \
	vendor/st-ericsson/u9500_100/LbsConfig.cfg:system/etc/LbsConfig.cfg \
	vendor/st-ericsson/u9500_100/LbsLogConfig.cfg:system/etc/LbsLogConfig.cfg \
	vendor/st-ericsson/u9500_100/LbsPgpsConfig.cfg:system/etc/LbsPgpsConfig.cfg \
	vendor/st-ericsson/u9500_100/cacert.txt:system/etc/cacert.txt \
	vendor/st-ericsson/u9500_100/LbsPltConfig.cfg:system/etc/LbsPltConfig.cfg

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
        vendor/st-ericsson/u9500_100/apns_u9500_100.xml:system/etc/apns-conf.xml


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
	external/alsa-lib/src/conf/pcm/surround71.conf:system/usr/share/alsa/pcm/surround71.conf \


# Copy the U-Boot splash screen
# If the U-boot splash image output path need is changed, the UBOOT_SPLASH_IMAGE_OUTPUT variable
# in BoardConfig.mk (vendor/st-ericsson/uxx00) also needs to be updated
PRODUCT_COPY_FILES += \
	vendor/st-ericsson/boot/u-boot/tools/logos/stericsson.bmp:splash.bin

###### u9500_100 Specific ######

# keyboard layouts
PRODUCT_COPY_FILES += \
	vendor/st-ericsson/u9500_100/STMPE-keypad.kl:system/usr/keylayout/STMPE-keypad.kl \
	vendor/st-ericsson/u9500_100/tc3589x-keypad.kl:system/usr/keylayout/tc3589x-keypad.kl \
	vendor/st-ericsson/u9500_100/ux500-ske-keypad.kl:system/usr/keylayout/ux500-ske-keypad.kl \
	vendor/st-ericsson/u9500_100/ux500-ske-keypad-qwertz.kl:system/usr/keylayout/ux500-ske-keypad-qwertz.kl \
	vendor/st-ericsson/u9500_100/AB8500_Hs_Button.kl:system/usr/keylayout/AB8500_Hs_Button.kl

# product specific ueventd.rc
PRODUCT_COPY_FILES += \
	vendor/st-ericsson/u9500_100/init.u9500_100.rc:root/init.u9500_100.rc \
	vendor/st-ericsson/u9500_100/ueventd.st-ericsson.rc:root/ueventd.st-ericsson.rc

# RIL initialization script
PRODUCT_COPY_FILES += \
	vendor/st-ericsson/u9500_100/ste_modem.sh:system/etc/ste_modem.sh

# DBus config
PRODUCT_COPY_FILES += vendor/st-ericsson/u9500_100/dbus.conf:system/etc/dbus.conf

# Automount SD-Card
PRODUCT_COPY_FILES += \
	vendor/st-ericsson/u9500_100/vold.fstab:system/etc/vold.fstab

# BT QoS role switch blacklist
PRODUCT_COPY_FILES += \
	system/bluetooth/data/qos_role_switch_blacklist.conf:system/etc/bluetooth/qos_role_switch_blacklist.conf

# Windows USB Host Drivers
USB_PC_DRIVERS_SET_DRIVERS := STE-Linux-platform-usb-drivers-windows.zip

# Neven data
PRODUCT_COPY_FILES += \
	external/neven/Embedded/common/data/APIEm/Modules/RFFprec_501.bmd:system/usr/share/bmd/RFFprec_501.bmd

# Copy files for modules
$(call inherit-product-if-exists, vendor/st-ericsson/tools/platform/USB-PC-drivers/Windows/copy_files.mk)
$(call inherit-product-if-exists, vendor/st-ericsson/base_utilities/core/utilities/lua/copy_files.mk)
$(call inherit-product-if-exists, vendor/st-ericsson/storage/parameter_storage/cspsa_image/copy_files.mk)

# HSI flush workaround
PRODUCT_COPY_FILES += \
	vendor/st-ericsson/u9500_100/flush_hsi/flush_hsi:system/bin/flush_hsi

# Modem Workaround - File permissions
PRODUCT_COPY_FILES += \
	vendor/st-ericsson/u9500_100/set_modem_permission.sh:system/bin/set_modem_permission.sh

#UIB detection
PRODUCT_COPY_FILES += \
	vendor/st-ericsson/u9500_100/uib_detect.sh:system/bin/uib_detect.sh
