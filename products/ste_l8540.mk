
# Superclass
$(call inherit-product, build/target/product/generic.mk)

# Overrides
PRODUCT_BRAND := ste
PRODUCT_NAME := ste_l8540
PRODUCT_DEVICE := l8540
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

PRODUCT_PACKAGE_OVERLAYS := vendor/st-ericsson/l8540/overlay
PRODUCT_PROPERTY_OVERRIDES := \
 ro.com.google.clientidbase=android-ste

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
                    tc35893-kp.kcm \
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
                    libcn \
                    cn_server \
                    at_core \
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
                    nap-app \
                    copsdaemon \
                    libcops \
                    cops_cmd \
                    cops_data \
                    gen_sipc \
                    libstagefright_soft_ste_aacdec \
                    libstagefright_soft_ste_mp3dec \
                    libstagefright_soft_ste_wmapro_v10 \
                    cspsalist \
                    cspsa_fastparameters \
                    cspsa2q \
                    q2cspsa \
                    STEBluetooth \
                    libnl \
                    overlay.montblanc \
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
                    adm.sqlite-l8540_ab8540_v1 \
                    adm.sqlite-l8540_ab8540_v2 \
                    libril-anm \
                    libaudiopolicy \
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
                    lk.bin \
                    lk_env.bin \
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
                    FmRadioReceiver \
                    FmRadioTransmitter \
                    ste-cg29xx_ctrl \
                    busybox \
                    libhdmi_service \
                    libblt_hw \
                    sgxkm \
                    sgxum \
                    drmserver \
                    libdrmframework \
                    libdrmframework_jni \
                    hwcomposer.montblanc \
                    tee_supplicant \
                    libext2_blkid \
                    libext2_e2p \
                    libext2_com_err \
                    libext2fs \
                    libext2_uuid \
                    e2fsck \
                    mke2fs \
                    make_ext4fs.exe \
                    mmprobed \
                    RXN_IntApp \
                    stedump \
                    local_time.default

# MIS modules for l8540 modem
# need to add lib that are explicitly linked in binary (ex: libmoliU7xx..) and not in makekefile as dependency (ex: libmid..)
PRODUCT_PACKAGES += \
                    Flashless_mid.conf.hsi \
                    Flashless_mid.conf.c2c \
                    Flashless_mid.conf.xmipc \
                    additional-ril-config-readme \
                    ste-ril.cfg \
                    nap_relay \
                    nap_script_connect.sh \
                    nap_script_disconnect.sh \
                    libste-oem-ril \
                    libusb \
                    libat \
                    mid \
                    mid_dbus_cmd \
                    mid_dbus_listener \
                    libmidat.so \
                    libconfig-1.4.7.so \
                    libcap.so \
                    mld \
                    modem_dump_handler \
                    mdh.conf \
                    bridge_relay \
                    rfm-server \
                    caif_socket_broker \
                    com.stericsson.telephony \
                    cmd_relay_switch \
                    crs.conf \
                    9540.cfg \

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
include frameworks/base/data/sounds/AudioPackage2.mk

# Added resources for TextToSpeech utility.
include external/svox/pico/lang/all_pico_languages.mk

# Copy input device configuration files
PRODUCT_COPY_FILES += \
	vendor/st-ericsson/l8540/cyttsp-spi.idc:system/usr/idc/cyttsp-spi.idc

PRODUCT_COPY_FILES += \
        vendor/st-ericsson/l8540/st-ftk.idc:system/usr/idc/st-ftk.idc

# Copy the ste_insmod.sh script to system/bin
PRODUCT_COPY_FILES += \
        vendor/st-ericsson/l8540/ste_insmod.sh:system/bin/ste_insmod.sh

# Copy the blue-up.sh script to system/bin
PRODUCT_COPY_FILES += \
	vendor/st-ericsson/l8540/blue-up.sh:system/bin/blue-up.sh \
	vendor/st-ericsson/l8540/init.rc:root/init.rc \
	vendor/st-ericsson/l8540/init.st-ericsson.rc:root/init.st-ericsson.rc \
	vendor/st-ericsson/l8540/init.st-ericsson.usb.rc:root/init.st-ericsson.usb.rc \
	vendor/st-ericsson/l8540/init.project.rc:root/init.project.rc \
	vendor/st-ericsson/l8540/init.st-ericsson.modem.rc:root/init.st-ericsson.modem.rc \
	vendor/st-ericsson/l8540/init_bd_addr.sh:system/etc/init_bd_addr.sh \
	vendor/st-ericsson/l8540/media_profiles_l8540.xml:system/etc/media_profiles.xml \
	vendor/st-ericsson/l8540/media_codecs_l8540.xml:system/etc/media_codecs.xml

# CG2900 initialization script
PRODUCT_COPY_FILES += \
	vendor/st-ericsson/l8540/cg2900-channel_init.sh:system/bin/cg2900-channel_init.sh

# USB Device ID initialization script
PRODUCT_COPY_FILES += \
	vendor/st-ericsson/l8540/usbid_init.sh:system/bin/usbid_init.sh

# eMMC user storage partition mount script
PRODUCT_COPY_FILES += \
	vendor/st-ericsson/l8540/check_mount_fs.sh:system/bin/check_mount_fs.sh

# copy AGPS configuration files for lbs_core module
PRODUCT_COPY_FILES += \
	vendor/st-ericsson/l8540/LbsConfig.cfg:system/etc/LbsConfig.cfg \
	vendor/st-ericsson/l8540/LbsLogConfig.cfg:system/etc/LbsLogConfig.cfg \
	vendor/st-ericsson/l8540/LbsPgpsConfig.cfg:system/etc/LbsPgpsConfig.cfg \
	vendor/st-ericsson/l8540/cacert.txt:system/etc/cacert.txt \
	vendor/st-ericsson/l8540/LbsPltConfig.cfg:system/etc/LbsPltConfig.cfg

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


# Copy ALSA configuration files to rootfs
# Copy the APN configuration file
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
	vendor/st-ericsson/l8540/apns_l8540.xml:system/etc/apns-conf.xml

# Copy the bootloader splash screen
# If the bootloader splash image output path need is changed, the BOOT_SPLASH_IMAGE_OUTPUT variable
# in BoardConfig.mk (vendor/st-ericsson/uxx00) also needs to be updated
PRODUCT_COPY_FILES += \
        vendor/st-ericsson/l8540/splash_image.bmp:splash.bin

# Copy scripts for automatic startup logging
PRODUCT_COPY_FILES += \
	vendor/st-ericsson/l8540/scripts/ste-reset_autolog.sh:system/bin/ste-reset_autolog.sh \
	vendor/st-ericsson/l8540/scripts/kernel_log.sh:system/bin/kernel_log.sh

# Copy scripts for automatic start/stop MID processes based on runtime mode and modem state
PRODUCT_COPY_FILES += \
	vendor/st-ericsson/l8540/scripts/modem_state_booting.sh:system/bin/modem_state_booting.sh \
	vendor/st-ericsson/l8540/scripts/modem_state_dumping.sh:system/bin/modem_state_dumping.sh \
	vendor/st-ericsson/l8540/scripts/modem_state_off.sh:system/bin/modem_state_off.sh \
	vendor/st-ericsson/l8540/scripts/modem_state_on.sh:system/bin/modem_state_on.sh \
	vendor/st-ericsson/l8540/scripts/modem_state_prepare_off.sh:system/bin/modem_state_prepare_off.sh \
	vendor/st-ericsson/l8540/scripts/ste-lins_off.sh:system/bin/ste-lins_off.sh \
	vendor/st-ericsson/l8540/scripts/ste-lins_on.sh:system/bin/ste-lins_on.sh

# Copy scripts for telling the MID processes that the system is shutting down
PRODUCT_COPY_FILES += \
	vendor/st-ericsson/l8540/scripts/ste-kill-mid.sh:system/bin/ste-kill-mid.sh

###### l8540 Specific ######

# keyboard layouts
PRODUCT_COPY_FILES += \
	vendor/st-ericsson/l8540/STMPE-keypad.kl:system/usr/keylayout/STMPE-keypad.kl \
	vendor/st-ericsson/l8540/STMPE-keypad.kl:system/usr/keylayout/tc35893-kp.kl \
	vendor/st-ericsson/l8540/ux500-ske-keypad.kl:system/usr/keylayout/ux500-ske-keypad.kl \
	vendor/st-ericsson/l8540/ux500-ske-keypad-qwertz.kl:system/usr/keylayout/ux500-ske-keypad-qwertz.kl \
	vendor/st-ericsson/l8540/AB8500_Hs_Button.kl:system/usr/keylayout/AB8500_Hs_Button.kl

# product specific ueventd.rc
PRODUCT_COPY_FILES += \
	vendor/st-ericsson/l8540/ueventd.st-ericsson.rc:root/ueventd.st-ericsson.rc

# RIL initialization script
PRODUCT_COPY_FILES += \
	vendor/st-ericsson/l8540/ste_modem.sh:system/etc/ste_modem.sh

# Test if UIB present script
PRODUCT_COPY_FILES += \
	vendor/st-ericsson/l8540/verifyfb.sh:system/etc/verifyfb.sh

# DBus config
PRODUCT_COPY_FILES += vendor/st-ericsson/l8540/dbus.conf:system/etc/dbus.conf

# Automount SD-Card
PRODUCT_COPY_FILES += \
	vendor/st-ericsson/l8540/vold.fstab:system/etc/vold.fstab

# Modem Workaround - File permissions
PRODUCT_COPY_FILES += \
	vendor/st-ericsson/l8540/set_modem_permission.sh:system/bin/set_modem_permission.sh

# Modem boot condition depending on the board
PRODUCT_COPY_FILES += \
        vendor/st-ericsson/l8540/scripts/ste-start-mid.sh:system/bin/ste-start-mid.sh

# Modem Workaround - to restore q store calibration during init of platform
PRODUCT_COPY_FILES += \
	vendor/st-ericsson/l8540/restore_modem_calibration.sh:system/bin/restore_modem_calibration.sh


# Modem Workaround - for calibration
PRODUCT_COPY_FILES += \
	vendor/st-ericsson/l8540/backup_modem_calibration_in_CSPSA.sh:system/bin/backup_modem_calibration_in_CSPSA.sh

# Windows USB Host Drivers
USB_PC_DRIVERS_SET_DRIVERS := STE-Linux-platform-usb-drivers-windows.zip

# Neven data
PRODUCT_COPY_FILES += \
	external/neven/Embedded/common/data/APIEm/Modules/RFFprec_501.bmd:system/usr/share/bmd/RFFprec_501.bmd


# Bluetooth configs
PRODUCT_COPY_FILES += \
        system/bluetooth/data/qos_role_switch_blacklist.conf:system/etc/bluetooth/qos_role_switch_blacklist.conf \
        system/bluetooth/data/main.conf:system/etc/bluetooth/main.conf
# Copy files for modules
$(call inherit-product-if-exists, vendor/st-ericsson/access_services/modem_adaptations/modem-supervisor/copy_files.mk)
$(call inherit-product-if-exists, vendor/st-ericsson/external/DNSmulticast/avahi/avahi-autoipd/copy_files.mk)
$(call inherit-product-if-exists, vendor/st-ericsson/external/DNSmulticast/avahi/avahi-daemon/copy_files.mk)
$(call inherit-product-if-exists, vendor/st-ericsson/tools/platform/USB-PC-drivers/Windows/copy_files.mk)
$(call inherit-product-if-exists, vendor/st-ericsson/base_utilities/core/utilities/lua/copy_files.mk)
$(call inherit-product-if-exists, vendor/st-ericsson/storage/parameter_storage/cspsa_image/copy_files.mk)
$(call inherit-product-if-exists, vendor/st-ericsson/tools/platform/modem-flashtools/copy_files.mk)

# Google Mobile Services (GMS)
$(call inherit-product-if-exists, vendor/google/products/gms.mk)
