##########################################################
# Copyright (C) ST-Ericsson SA 2010. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.
##########################################################

#lbsd make file

LOCAL_PATH:= $(call my-dir)
SAVED_AGPS_PATH := $(LOCAL_PATH)

ifeq ($(LBS_SET_GNSSCHIP_VER),CG2910)
    LOCAL_ENABLE_GLONASS := true
    LOCAL_ENABLE_QZSS    := true
    LOCAL_ENABLE_CG2910  := true
    LOCAL_ENABLE_EE      := true
endif



ifeq ($(TARGET_SIMULATOR),true)
    $(warning "Building Simulator. No work here for me" )
else

# +++++++++++++++++++++++++++ BEGIN : Platform Specific Config +++++++++++++++++++++++++++

ifeq ($(LBS_SET_MODEMCLIENT),CAIF)
# Used for U9500 Platform.
    $(warning "LBS_SET_MODEMCLIENT==CAIF" )
    LOCAL_ENABLE_SUPL := true
    LOCAL_ENABLE_CP := true
    LOCAL_ENABLE_FREQ_AID := false
    LOCAL_ENABLE_FTA_AID := false
    LOCAL_ENABLE_SBEE := false
    LOCAL_ENABLE_CAIF_SOCKET := true
    LOCAL_ENABLE_CELL_INFO := true

else
# Used for U8500 Platform.

# Enable / Disable below mentioned switches even in the lbs_mal_client\liblcsclient\Android.mk & \lbs_amil_client\Android.mk file.
    LOCAL_ENABLE_SUPL := true
    LOCAL_ENABLE_CP := true
    LOCAL_ENABLE_FREQ_AID := true

    LOCAL_ENABLE_FTA_AID := true

    LOCAL_ENABLE_SBEE := false

ifeq ($(LBS_SET_MODEMCLIENT),MAL)
ifeq ($(STE_USE_MOD_APE), 1)
    LOCAL_AMIL_LCS_SOLUTION := false
else
    LOCAL_AMIL_LCS_SOLUTION := true
endif
else
    LOCAL_AMIL_LCS_SOLUTION := false
endif

endif
ifeq ($(LBS_SET_NDK_BUILD),true)
    LOCAL_SET_NDK_BUILD := true
endif
ifeq (hats,$(findstring hats, $(TARGET_PRODUCT)))
    $(warning "Compiling lbsd for HATS" )
    LOCAL_SET_HATS_BUILD := true
endif
    LOCAL_GENERIC_SOLUTION := true
    LOCAL_AGPS_CFLAGS := -DGPS_OVER_HCI


    LOCAL_PATH:= $(SAVED_AGPS_PATH)
include $(CLEAR_VARS)
# --------------------------- END  :  Platform Specific Config ---------------------------
# +++++++++++++++++++++++++++ BEGIN : NDK PREBUILT LIBRARIES   +++++++++++++++++++++++++++
ifeq ($(LOCAL_SET_NDK_BUILD),true)

    LIBGPSNDK_MK := $(SAVED_AGPS_PATH)/libgps/libgps_ndk.mk
include $(LIBGPSNDK_MK)


ifeq ($(LOCAL_ENABLE_SBEE),true)
    RXN_SECURITYNDK_MK := $(SAVED_AGPS_PATH)/../../lbs_extended_eph/RxnLib/RXN_security_ndk.mk
include $(RXN_SECURITYNDK_MK)
    RXN_PGPS_APINDK_MK := $(SAVED_AGPS_PATH)/../../lbs_extended_eph/RxnLib/RXN_PGPS_API_ndk.mk
include $(RXN_PGPS_APINDK_MK)
endif

ifeq ($(LOCAL_AMIL_LCS_SOLUTION),true)

    LIBLCSCLIENTNDK_MK := $(SAVED_AGPS_PATH)/../../lbs_external/mil_lib/liblcsclient_ndk.mk
include $(LIBLCSCLIENTNDK_MK)
    LIBPHONETNDK_MK := $(SAVED_AGPS_PATH)/../../lbs_external/mil_lib/libphonet_ndk.mk
include $(LIBPHONETNDK_MK)
    LIBISIMESSAGENDK_MK := $(SAVED_AGPS_PATH)/../../lbs_external/mil_lib/libisimessage_ndk.mk
include $(LIBISIMESSAGENDK_MK)
    LIBSMS_SERVERNDK_MK := $(SAVED_AGPS_PATH)/../../lbs_external/mil_lib/libsms_server_ndk.mk
include $(LIBSMS_SERVERNDK_MK)
    LIBCNNDK_MK := $(SAVED_AGPS_PATH)/../../lbs_external/mil_lib/libcn_ndk.mk
include $(LIBCNNDK_MK)
    LIBMALUTILNDK_MK := $(SAVED_AGPS_PATH)/../../lbs_external/mil_lib/libmalutil_ndk.mk
include $(LIBMALUTILNDK_MK)
    LIBSHMNETLNKNDK_MK := $(SAVED_AGPS_PATH)/../../lbs_external/mil_lib/libshmnetlnk_ndk.mk
include $(LIBSHMNETLNKNDK_MK)

endif
ifeq ($(LOCAL_ENABLE_SUPL),true)

    LIBSIMNDK_MK := $(SAVED_AGPS_PATH)/../../lbs_external/mil_lib/libsim_ndk.mk
include $(LIBSIMNDK_MK)
    UICCNDK_MK := $(SAVED_AGPS_PATH)/../../lbs_external/mil_lib/uicc_ndk.mk
include $(UICCNDK_MK)
    LIBSIMCOMNDK_MK := $(SAVED_AGPS_PATH)/../../lbs_external/mil_lib/libsimcom_ndk.mk
include $(LIBSIMCOMNDK_MK)

endif

LOCAL_PATH:= $(SAVED_AGPS_PATH)

# --------------------------- END  :  NDK PREBUILT LIBRARIES ---------------------------
else

ifeq ($(LOCAL_ENABLE_SBEE),true)
    LOCAL_PREBUILT_LIBS    :=  ../../lbs_extended_eph/RxnLib/RXN_security.a ../../lbs_extended_eph/RxnLib/RXN_PGPS_API.a
endif
endif

ifeq ($(LOCAL_ENABLE_CG2910),true)
    LOCAL_PREBUILT_LIBS    +=  libgps/CG2910/lib/Android_gcc432_ARMv5te/libgps.a
else
    LOCAL_PREBUILT_LIBS    +=  libgps/CG2900/lib/Android_gcc432_ARMv5te/libgps.a
endif


    LOCAL_MODULE_TAGS := optional



include $(BUILD_MULTI_PREBUILT)

include $(CLEAR_VARS)
# ---------------- Autonomous Build -------------------
ifeq ($(LOCAL_GENERIC_SOLUTION),true)
    LOCAL_CFLAGS += -DAGPS_GENERIC_SOLUTION_FTR

    LOCAL_CFLAGS += -UCMCC_LOGGING_ENABLE
else
    LOCAL_CFLAGS += -DAMIL_FTR
endif
#LOCAL_CFLAGS += -DCMCC_LOGGING_ENABLE
#LOCAL_CFLAGS += -DSUPL_CMCC_LOGGING_ENABLE

    LOCAL_CFLAGS += -DAGPS_LINUX_FTR
ifeq ($(LOCAL_SET_NDK_BUILD),true)
    LOCAL_CFLAGS += -UAGPS_ANDR_WAKEUP_FTR -DHAVE_TM_GMTOFF
else
    LOCAL_CFLAGS += -DAGPS_ANDR_WAKEUP_FTR
endif
ifeq ($(LOCAL_SET_HATS_BUILD),true)
    LOCAL_CFLAGS += -DARM_LINUX_AGPS_FTR
    LOCAL_CFLAGS += -DAGPS_LINUX_BARE_FTR
endif

    LOCAL_CFLAGS += -UHEAP_MEMORY_DEBUG

    LOCAL_CFLAGS += -DDEBUG_LOG
    LOCAL_CFLAGS += -UDEBUG_LOG_LEV2
    LOCAL_CFLAGS += -DAGPS_DONOT_USE_OS_QUEUE
    LOCAL_CFLAGS += -UHALGPS_DONOT_USE_RTC
    LOCAL_CFLAGS += -UHALGPS_HCI_LOGGING
    LOCAL_CFLAGS += -DAGPS_TEST_MODE_FTR
    LOCAL_CFLAGS += -DATT_LOGGING_ENABLE
    LOCAL_CFLAGS += -DDO_NOT_USE_ALMANAC
LOCAL_CFLAGS += $(LOCAL_AGPS_CFLAGS)

ifeq ($(LOCAL_ENABLE_SBEE),true)
    LOCAL_CFLAGS += -DAGPS_SBEE_FTR
    LOCAL_CFLAGS += -UCGPS_STRAIGHT_TO_OFF_FTR
    LOCAL_CFLAGS += -UAGPS_HISTORICAL_DATA_FTR
endif

ifeq ($(LOCAL_AMIL_LCS_SOLUTION),true)
    LOCAL_CFLAGS += -DAMIL_LCS_FTR
endif
ifeq ($(LOCAL_ENABLE_CAIF_SOCKET),true)
    LOCAL_CFLAGS += -DAMIL_LCS_FTR
endif

ifeq ($(LOCAL_ENABLE_MSR),true)
    LOCAL_CFLAGS += -DAMIL_MSR
endif
ifeq ($(LOCAL_ENABLE_GLONASS),true)
    LOCAL_CFLAGS += -DLBS_GLONASS
endif

ifeq ($(LOCAL_ENABLE_QZSS),true)
    LOCAL_CFLAGS += -DLBS_QZSS
endif

ifeq ($(LOCAL_ENABLE_CG2910),true)
    LOCAL_CFLAGS += -DLBS_CG2910
endif

    LOCAL_SRC_FILES +=  agpsmain/srce/agpsserver.c \
                        agpscodec/srce/agpscodec.c \
                        agpsmain/srce/agpsmain.c \
                        libagpsosa/srce/agpsosa.c \
                        libagpsosa/srce/agpsfsm.c \
                        libagpsosa/srce/lbsosatrace.c

ifeq ($(LOCAL_GENERIC_SOLUTION),false)
    LOCAL_SRC_FILES +=  amil/srce/AMIL_ATCommandComposer.c \
                        amil/srce/AMIL_ATMessageParser.c \
                        amil/srce/AMIL_Interface.c \
                        amil/srce/AMIL_MessageHandler.c \
                        amil/srce/AMIL_MessagePipeHandler.c
endif
ifeq ($(LOCAL_GENERIC_SOLUTION),true)
    LOCAL_SRC_FILES +=  halgps/srce/halgps0main.c \
                        halgps/srce/halgps1lib.c \
                        halgps/srce/halgps2uart.c \
                        halgps/srce/halgps3time.c \
                        halgps/srce/halgps6spi.c \
                        halgps/srce/halgps6spiLinux.c \
                        halgps/srce/halgps8xosgps.c \
                        halgps/srce/halgps9hci.c \
                        halgps/srce/halgps20gsa.c \
                        halgps/srce/halgps21l1rx.c \
                        halgps/srce/halgps10hciLinux.c \
                        cgps/srce/cgps0main.c \
                        cgps/srce/cgps1lib.c \
                        cgps/srce/cgps2patch.c \
                        cgps/srce/cgps3demo.c \
                        cgps/srce/cgps4utils.c \
                        cgps/srce/cgps5supl.c \
                        cgps/srce/cgps6tat.c \
                        cgps/srce/cgps7cp.c \
                        cgps/srce/cgps8gsa.c \
                        cgps/srce/cgps9testMode.c \
                        cgps/srce/cgps10HistoricalCell.c \
                        cgps/srce/cgps11ee.c \
                        gns/srce/gns.c \
                        lbscfg/srce/lbscfg_log.c \
                        lbscfg/srce/lbscfg_platform.c \
                        lbscfg/srce/lbscfg_user.c


ifeq ($(LOCAL_ENABLE_SBEE),true)
    LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../lbs_extended_eph/libsbee/incl
else
ifeq ($(LOCAL_ENABLE_CG2910),true)
        LOCAL_SRC_FILES +=  libgps/CG2910/srce/GN_Test_Stubs.c
endif
endif


endif

    LOCAL_C_INCLUDES += \
                        $(LOCAL_PATH)/libagpsosa/incl \
                        $(LOCAL_PATH)/agpsmain/incl \
                        $(LOCAL_PATH)/agpscodec/incl \
    $(LOCAL_PATH)/include
ifeq ($(LOCAL_GENERIC_SOLUTION),false)
    LOCAL_C_INCLUDES += \
                        $(LOCAL_PATH)/../../lbs_amil_client/incl
endif
ifeq ($(LOCAL_GENERIC_SOLUTION),true)
    LOCAL_C_INCLUDES += \
                        $(LOCAL_PATH)/halgps/incl \
                        $(LOCAL_PATH)/cgps/incl \
                        $(LOCAL_PATH)/lsim/incl \
                        $(LOCAL_PATH)/gns/incl \
                        $(LOCAL_PATH)/gnsi/incl \
                        $(LOCAL_PATH)/lbscfg/incl \
                        $(LOCAL_PATH)/../../lbs_amil_client/incl \
                        $(LOCAL_PATH)/../../../../access_services/sms/sms/include \
                        $(LOCAL_PATH)/../../../../access_services/sim/sim/include \
                        $(LOCAL_PATH)/../../../../access_services/call_network/call_network/libcn/include \
                        $(LOCAL_PATH)/../lbsclient/incl
                        endif
ifeq ($(LOCAL_ENABLE_CG2910),true)
    LOCAL_C_INCLUDES += $(LOCAL_PATH)/libgps/CG2910/incl
else
    LOCAL_C_INCLUDES += $(LOCAL_PATH)/libgps/CG2900/incl
endif


# ---------------- AGPS Common Build -------------------

ifeq ($(LOCAL_ENABLE_SUPL),true)
    LOCAL_SRC_FILES += lsim/srce/lsim0fsm.c
else
ifeq ($(LOCAL_ENABLE_CP),true)
    LOCAL_SRC_FILES += lsim/srce/lsim0fsm.c
else
ifeq ($(LOCAL_ENABLE_FREQ_AID),true)
    LOCAL_SRC_FILES += lsim/srce/lsim0fsm.c
endif
endif
endif



# ---------------- Extended Ephemeris build -------------------

ifeq ($(LOCAL_ENABLE_EE),true)
    $(warning "Compiling with Extended Ephemeris Enabled" )
    LOCAL_CFLAGS += -DAGPS_EE_FTR
    LOCAL_SRC_FILES += lsim/srce/lsim8ee.c \
                        gns/srce/gnsEE.c
else
    ifeq ($(LOCAL_ENABLE_CG2910),true)
        LOCAL_SRC_FILES +=  libgps/CG2910/srce/GN_EE_api_stubs.c
    else
        LOCAL_SRC_FILES +=  libgps/CG2900/srce/GN_EE_api_stubs.c
    endif

endif





# ---------------- Frequency aiding Build -------------------

ifeq ($(LOCAL_ENABLE_FREQ_AID),true)
    $(warning "Compiling with Frequency Aiding Enabled" )
    LOCAL_CFLAGS += -DGPS_FREQ_AID_FTR
    LOCAL_SRC_FILES += lsim/srce/lsim5cc.c  \
                       lsim/srce/lsim6ccgns.c
endif


# ---------------- Cell Info Build -------------------

ifeq ($(LOCAL_ENABLE_CELL_INFO),true)
    $(warning "Compiling with CEll Info Enabled" )
    LOCAL_CFLAGS += -DGNS_CELL_INFO_FTR
endif


# ---------------- CP Build -------------------

#if CP is enabled in this build
ifeq ($(LOCAL_ENABLE_CP),true)
    $(warning "Compiling with CP Enabled" )
    LOCAL_CFLAGS += -DAGPS_FTR -DDGPS_SLEEP_TIMEOUT_FTR=0 -DNO_REQ_NAV_REF_FTR -DCGPS_TEMP_HANDLES_CP_REPORTING_AMNT_FTR

ifeq ($(LOCAL_GENERIC_SOLUTION),true)
    LOCAL_SRC_FILES +=  lsim/srce/lsim2cp.c \
                        lsim/srce/lsim4cpgns.c

                        LOCAL_C_INCLUDES += $(LOCAL_PATH)/lsim/incl \
    $(LOCAL_PATH)/../../lbs_amil_client/incl
ifeq ($(LOCAL_ENABLE_CAIF_SOCKET),true)
    LOCAL_CFLAGS += -UAGPS_CP_SUSPEND_ON_AID_REQ
    LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../lbs_mfl_client/libamilatclient/incl
else
    LOCAL_CFLAGS += -DAGPS_CP_SUSPEND_ON_AID_REQ
endif

endif
#  FINE TIME aiding Build
ifeq ($(LOCAL_ENABLE_FTA_AID),true)
    $(warning "Compiling with Finetime Aiding Enabled" )
    LOCAL_CFLAGS += -DAGPS_TIME_SYNCH_FTR
    LOCAL_SRC_FILES += lsim/srce/lsim7fta.c
endif
else
    $(warning "Compiling with CP Disabled" )
endif


# ---------------- SUPL Build -------------------

#if SUPL is enabled in this build
ifeq ($(LOCAL_ENABLE_SUPL),true)
    $(warning "Compiling with SUPL Enabled" )
    LOCAL_CFLAGS += -DAGPS_UP_FTR
ifeq ($(LOCAL_GENERIC_SOLUTION),true)
    LOCAL_SRC_FILES +=   lsim/srce/lsim1up.c \
                         lsim/srce/lsim3upgns.c \

    LOCAL_CFLAGS += -UAGPS_INJECT_MOBILE_INFO_TEST

    LOCAL_SRC_FILES +=\
           ../../asn1/src/BIT_STRING.c        \
           ../../asn1/src/NULL.c            \
           ../../asn1/src/asn_SEQUENCE_OF.c    \
           ../../asn1/src/constr_CHOICE.c        \
           ../../asn1/src/der_encoder.c        \
           ../../asn1/src/xer_encoder.c        \
           ../../asn1/src/BOOLEAN.c        \
           ../../asn1/src/NativeEnumerated.c    \
           ../../asn1/src/asn_SET_OF.c        \
           ../../asn1/src/constr_SEQUENCE.c    \
           ../../asn1/src/per_decoder.c        \
           ../../asn1/src/xer_support.c        \
           ../../asn1/src/ENUMERATED.c        \
           ../../asn1/src/NativeInteger.c        \
           ../../asn1/src/asn_codecs_prim.c    \
           ../../asn1/src/constr_SEQUENCE_OF.c    \
           ../../asn1/src/per_encoder.c        \
           ../../asn1/src/GeneralizedTime.c    \
           ../../asn1/src/OCTET_STRING.c        \
           ../../asn1/src/ber_decoder.c        \
           ../../asn1/src/constr_SET_OF.c        \
           ../../asn1/src/per_opentype.c        \
           ../../asn1/src/IA5String.c        \
           ../../asn1/src/UTCTime.c        \
           ../../asn1/src/ber_tlv_length.c    \
           ../../asn1/src/constr_TYPE.c        \
           ../../asn1/src/per_support.c        \
           ../../asn1/src/INTEGER.c        \
           ../../asn1/src/VisibleString.c        \
           ../../asn1/src/ber_tlv_tag.c        \
           ../../asn1/src/constraints.c        \
           ../../asn1/src/xer_decoder.c        \
           \
           supl/asn1c_helper/asn1_helper/srce/rrlp_asn1.c    \
           supl/asn1c_helper/asn1_helper/srce/supl_asn1.c    \
           \
           supl/asn1c_helper/rrlp_helper/rrlp_helper.c        \
           supl/asn1c_helper/supl_helper/supl_helper.c        \
           supl/GAD_helper/GAD_helper.c            \
           \
           supl/rrlp_handler/GN_RRLP_api.c        \
           supl/rrlp_handler/rrlp_client.c        \
           supl/rrlp_handler/rrlp_handler.c        \
           supl/rrlp_handler/rrlp_instance.c        \
           supl/rrlp_handler/rrlp_messages.c        \
           \
           supl/supl_client/assist_gps_dummy.c        \
           supl/supl_client/supl_client.c        \
           \
           supl/supl_module/GN_RRLP_api_supl.c        \
           supl/supl_module/GN_SUPL_api.c        \
           supl/supl_module/supl_config.c        \
           supl/supl_module/supl_instance.c        \
           supl/supl_module/supl_log.c            \
           supl/supl_module/supl_messages.c        \
           supl/supl_module/supl_module.c        \
           supl/supl_module/supl_user_timers.c        \
           \
          ../../asn1/rrlp_asn1/srce/RRLP_Accuracy.c                            \
          ../../asn1/rrlp_asn1/srce/RRLP_AccuracyOpt.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_AcquisAssist.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_AcquisElement.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_Add-GPS-AssistData.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_Add-GPS-ControlHeader.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_AddionalAngleFields.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_AddionalDopplerFields.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_AdditionalAssistanceData.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_AdditionalDopplerFields.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_AlertFlag.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_Almanac-ECEFsbasAlmanacSet.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_Almanac-GlonassAlmanacSet.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_Almanac-KeplerianSet.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_Almanac-MidiAlmanacSet.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_Almanac-NAVKeplerianSet.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_Almanac-ReducedKeplerianSet.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_Almanac.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_AlmanacElement.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_AntiSpoofFlag.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_AssistBTSData-R98-ExpOTD.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_AssistBTSData.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_AssistanceData.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_AssistanceNeeded.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_AssistanceSupported.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_BCCHCarrier.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_BSIC.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_BSICAndCarrier.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_BTSPosition.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_BadSignalElement.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_BitNumber.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_CNAVclockModel.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_CalcAssistanceBTS.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_CellID.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_CellIDAndLAC.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_CommonGANSSAssistance.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_ControlHeader.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_DGANSSExtensionSgnElement.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_DGANSSExtensionSgnTypeElement.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_DGANSSSgnElement.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_DGPSCorrections.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_DGPSCorrectionsValidityPeriod.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_DGPSExtensionSatElement.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_EOTDQuality.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_EnvironmentCharacter.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_EphemerisSubframe1Reserved.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_ErrorCodes.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_ExpOTDUncertainty.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_ExpectedOTD.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_Ext-GeographicalInformation.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_Extended-reference.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_ExtensionContainer.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_FineRTD.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_FixType.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_FrameDrift.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_FrameNumber.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GANSS-AssistData.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GANSS-ControlHeader.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GANSS-ID1-element.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GANSS-ID1.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GANSS-ID3-element.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GANSS-ID3.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GANSS-MsrElement.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GANSS-MsrSetElement.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GANSS-SgnElement.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GANSS-SgnTypeElement.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GANSSAddIonosphericModel.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GANSSAddUTCModel.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GANSSAdditionalAssistanceChoices.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GANSSAdditionalAssistanceChoicesForOneGANSS.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GANSSAlmanacElement.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GANSSAlmanacModel-R10-Ext.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GANSSAlmanacModel.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GANSSAssistance.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GANSSAssistanceData.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GANSSAssistanceForOneGANSS.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GANSSAssistanceSet.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GANSSAuxiliaryInformation.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GANSSClockModel.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GANSSCommonAssistData.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GANSSDataBit.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GANSSDataBitAssist.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GANSSDataBitsSgnElement.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GANSSDeltaElementList.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GANSSDeltaEpochHeader.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GANSSDiffCorrections.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GANSSDiffCorrectionsValidityPeriod.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GANSSEarthOrientParam.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GANSSEphemerisDeltaBitSizes.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GANSSEphemerisDeltaEpoch.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GANSSEphemerisDeltaMatrix.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GANSSEphemerisDeltaScales.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GANSSEphemerisExtension.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GANSSEphemerisExtensionCheck.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GANSSEphemerisExtensionHeader.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GANSSEphemerisExtensionTime.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GANSSGenericAssistDataElement.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GANSSIonoStormFlags.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GANSSIonosphereModel.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GANSSIonosphericModel.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GANSSLocationInfo.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GANSSMeasureInfo.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GANSSModelID.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GANSSNavModel.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GANSSOrbitModel.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GANSSPositionMethod.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GANSSPositionMethods.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GANSSPositioningMethod.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GANSSPositioningMethodTypes.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GANSSRealTimeIntegrity.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GANSSRefLocation.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GANSSRefMeasurement-R10-Ext-Element.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GANSSRefMeasurementAssist-R10-Ext.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GANSSRefMeasurementAssist.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GANSSRefMeasurementElement.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GANSSRefTimeInfo.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GANSSReferenceOrbit.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GANSSReferenceTime-R10-Ext.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GANSSReferenceTime.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GANSSSatEventsInfo.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GANSSSatelliteElement.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GANSSSignalID.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GANSSSignals.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GANSSTOD-GSMTimeAssociation.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GANSSTOD.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GANSSTODUncertainty.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GANSSTODm.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GANSSTimeModelElement-R10-Ext.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GANSSTimeModelElement.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GANSSUTCModel.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GLONASSclockModel.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GPS-AssistData.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GPS-MeasureInfo.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GPS-MsrElement.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GPS-MsrSetElement.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GPSAcquisAssist-R10-Ext-Element.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GPSAcquisAssist-R10-Ext.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GPSAlmanac-R10-Ext.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GPSAssistance.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GPSAssistanceData.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GPSClockModel.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GPSDeltaElementList.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GPSDeltaEpochHeader.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GPSEphemerisDeltaBitSizes.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GPSEphemerisDeltaEpoch.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GPSEphemerisDeltaMatrix.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GPSEphemerisDeltaScales.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GPSEphemerisExtension.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GPSEphemerisExtensionCheck.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GPSEphemerisExtensionHeader.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GPSEphemerisExtensionTime.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GPSReferenceOrbit.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GPSReferenceTime-R10-Ext.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GPSReferenceTimeUncertainty.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GPSSatEventsInfo.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GPSTOW23b.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GPSTOW24b.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GPSTOWAssist.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GPSTOWAssistElement.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GPSTime.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GPSTimeAssistanceMeasurements.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GPSWeek.c                                \
          ../../asn1/rrlp_asn1/srce/RRLP_GSMTime.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_GanssDataBitsElement.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_IonosphericModel.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_LAC.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_LocErrorReason.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_LocationError.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_LocationInfo.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_MeasureResponseTime.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_MethodType.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_ModuloTimeSlot.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_MoreAssDataToBeSent.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_MpathIndic.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_MsrAssistBTS-R98-ExpOTD.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_MsrAssistBTS.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_MsrAssistData-R98-ExpOTD.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_MsrAssistData.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_MsrPosition-Req.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_MsrPosition-Rsp.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_MultiFrameCarrier.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_MultiFrameOffset.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_MultipleMeasurementSets.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_MultipleSets.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_NAVclockModel.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_NavModel-CNAVKeplerianSet.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_NavModel-GLONASSecef.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_NavModel-KeplerianSet.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_NavModel-NAVKeplerianSet.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_NavModel-SBASecef.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_NavModelElement.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_NavigationModel.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_NeighborIdentity.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_NonGANSSPositionMethods.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_NumOfMeasurements.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_OTD-FirstSetMsrs.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_OTD-MeasureInfo-5-Ext.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_OTD-MeasureInfo-R98-Ext.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_OTD-MeasureInfo.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_OTD-Measurement.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_OTD-MeasurementWithID.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_OTD-MsrElementFirst-R98-Ext.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_OTD-MsrElementFirst.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_OTD-MsrElementRest.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_OTD-MsrsOfOtherSets.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_OTDValue.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_PDU.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_PosCapabilities.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_PosCapability-Req.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_PosCapability-Rsp.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_PositionData.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_PositionInstruct.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_PositionMethod.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_ProtocolError.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_RRLP-Component.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_RefLocation.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_RefQuality.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_ReferenceAssistData.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_ReferenceFrame.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_ReferenceIdentity.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_ReferenceIdentityType.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_ReferenceNavModel.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_ReferenceRelation.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_ReferenceTime.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_ReferenceWGS84.c                        \
          ../../asn1/rrlp_asn1/srce/RRLP_Rel-5-MsrPosition-Rsp-Extension.c        \
          ../../asn1/rrlp_asn1/srce/RRLP_Rel-5-ProtocolError-Extension.c        \
          ../../asn1/rrlp_asn1/srce/RRLP_Rel-7-MsrPosition-Rsp-Extension.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_Rel-98-MsrPosition-Rsp-Extension.c    \
          ../../asn1/rrlp_asn1/srce/RRLP_Rel5-AssistanceData-Extension.c        \
          ../../asn1/rrlp_asn1/srce/RRLP_Rel5-MsrPosition-Req-Extension.c        \
          ../../asn1/rrlp_asn1/srce/RRLP_Rel7-AssistanceData-Extension.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_Rel7-MsrPosition-Req-Extension.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_Rel98-AssistanceData-Extension.c        \
          ../../asn1/rrlp_asn1/srce/RRLP_Rel98-Ext-ExpOTD.c                    \
          ../../asn1/rrlp_asn1/srce/RRLP_Rel98-MsrPosition-Req-Extension.c        \
          ../../asn1/rrlp_asn1/srce/RRLP_RelDistance.c                            \
          ../../asn1/rrlp_asn1/srce/RRLP_RelativeAlt.c                            \
          ../../asn1/rrlp_asn1/srce/RRLP_RequestIndex.c                        \
          ../../asn1/rrlp_asn1/srce/RRLP_RequiredResponseTime.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_RoughRTD.c                            \
          ../../asn1/rrlp_asn1/srce/RRLP_SBASID.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_SBASclockModel.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_SLR-Arg-PCS-Extensions.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_SLR-ArgExtensionContainer.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_SVID.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_SatElement.c                            \
          ../../asn1/rrlp_asn1/srce/RRLP_SatStatus.c                            \
          ../../asn1/rrlp_asn1/srce/RRLP_SatelliteID.c                            \
          ../../asn1/rrlp_asn1/srce/RRLP_Seq-OfGANSSDataBitsSgn.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_SeqOf-BadSatelliteSet.c                \
          ../../asn1/rrlp_asn1/srce/RRLP_SeqOf-GANSSDataBits.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_SeqOfAcquisElement.c                    \
          ../../asn1/rrlp_asn1/srce/RRLP_SeqOfAlmanacElement.c                    \
          ../../asn1/rrlp_asn1/srce/RRLP_SeqOfBadSignalElement.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_SeqOfDGANSSExtensionSgnElement.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_SeqOfDGANSSSgnElement.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_SeqOfGANSS-MsrElement.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_SeqOfGANSS-MsrSetElement.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_SeqOfGANSS-SgnElement.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_SeqOfGANSS-SgnTypeElement.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_SeqOfGANSSAlmanacElement.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_SeqOfGANSSGenericAssistDataElement.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_SeqOfGANSSRefMeasurementElement.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_SeqOfGANSSRefOrbit.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_SeqOfGANSSSatelliteElement.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_SeqOfGANSSTimeModel-R10-Ext.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_SeqOfGANSSTimeModel.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_SeqOfGPS-MsrElement.c                    \
          ../../asn1/rrlp_asn1/srce/RRLP_SeqOfGPS-MsrSetElement.c                \
          ../../asn1/rrlp_asn1/srce/RRLP_SeqOfGPSRefOrbit.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_SeqOfGanssDataBitsElement.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_SeqOfMsrAssistBTS-R98-ExpOTD.c        \
          ../../asn1/rrlp_asn1/srce/RRLP_SeqOfMsrAssistBTS.c                    \
          ../../asn1/rrlp_asn1/srce/RRLP_SeqOfNavModelElement.c                \
          ../../asn1/rrlp_asn1/srce/RRLP_SeqOfOTD-FirstSetMsrs-R98-Ext.c        \
          ../../asn1/rrlp_asn1/srce/RRLP_SeqOfOTD-FirstSetMsrs.c                \
          ../../asn1/rrlp_asn1/srce/RRLP_SeqOfOTD-MsrElementRest.c                \
          ../../asn1/rrlp_asn1/srce/RRLP_SeqOfOTD-MsrsOfOtherSets.c            \
          ../../asn1/rrlp_asn1/srce/RRLP_SeqOfReferenceIdentityType.c            \
          ../../asn1/rrlp_asn1/srce/RRLP_SeqOfSatElement.c                        \
          ../../asn1/rrlp_asn1/srce/RRLP_SeqOfSgnTypeElement.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_SeqOfStandardClockModelElement.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_SeqOfSystemInfoAssistBTS-R98-ExpOTD.c    \
          ../../asn1/rrlp_asn1/srce/RRLP_SeqOfSystemInfoAssistBTS.c            \
          ../../asn1/rrlp_asn1/srce/RRLP_SgnTypeElement.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_SpecificGANSSAssistance.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_StandardClockModelElement.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_StdResolution.c                        \
          ../../asn1/rrlp_asn1/srce/RRLP_SystemInfoAssistBTS-R98-ExpOTD.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_SystemInfoAssistBTS.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_SystemInfoAssistData-R98-ExpOTD.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_SystemInfoAssistData.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_SystemInfoIndex.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_TA0.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_TA1.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_TA2.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_TLMReservedBits.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_TLMWord.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_TOA-MeasurementsOfRef.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_TimeRelation.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_TimeSlot.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_TimeSlotScheme.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_UTCModel.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_UTCmodelSet2.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_UTCmodelSet3.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_UTCmodelSet4.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_UlPseudoSegInd.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_UncompressedEphemeris.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_UseMultipleSets.c  \
          ../../asn1/rrlp_asn1/srce/RRLP_VelocityEstimate.c  \
\
          ../../asn1/lpp_asn1/srce/LPP_Abort.c             \
          ../../asn1/lpp_asn1/srce/LPP_Abort-r9-IEs.c       \
          ../../asn1/lpp_asn1/srce/LPP_AccessTypes.c        \
          ../../asn1/lpp_asn1/srce/LPP_Acknowledgement.c        \
          ../../asn1/lpp_asn1/srce/LPP_AdditionalInformation.c       \
          ../../asn1/lpp_asn1/srce/LPP_A-GNSS-Error.c           \
          ../../asn1/lpp_asn1/srce/LPP_A-GNSS-ProvideAssistanceData.c \
          ../../asn1/lpp_asn1/srce/LPP_A-GNSS-ProvideCapabilities.c     \
          ../../asn1/lpp_asn1/srce/LPP_A-GNSS-ProvideLocationInformation.c  \
          ../../asn1/lpp_asn1/srce/LPP_A-GNSS-RequestAssistanceData.c   \
          ../../asn1/lpp_asn1/srce/LPP_A-GNSS-RequestCapabilities.c    \
          ../../asn1/lpp_asn1/srce/LPP_A-GNSS-RequestLocationInformation.c  \
          ../../asn1/lpp_asn1/srce/LPP_AlmanacECEF-SBAS-AlmanacSet.c    \
          ../../asn1/lpp_asn1/srce/LPP_AlmanacGLONASS-AlmanacSet.c    \
          ../../asn1/lpp_asn1/srce/LPP_AlmanacKeplerianSet.c    \
          ../../asn1/lpp_asn1/srce/LPP_AlmanacMidiAlmanacSet.c  \
          ../../asn1/lpp_asn1/srce/LPP_AlmanacNAV-KeplerianSet.c   \
          ../../asn1/lpp_asn1/srce/LPP_AlmanacReducedKeplerianSet.c  \
          ../../asn1/lpp_asn1/srce/LPP_ARFCN-ValueEUTRA.c   \
          ../../asn1/lpp_asn1/srce/LPP_ARFCN-ValueUTRA.c  \
          ../../asn1/lpp_asn1/srce/LPP_AssistanceDataSupportList.c  \
          ../../asn1/lpp_asn1/srce/LPP_BadSignalElement.c   \
          ../../asn1/lpp_asn1/srce/LPP_CellGlobalIdEUTRA-AndUTRA.c   \
          ../../asn1/lpp_asn1/srce/LPP_CellGlobalIdGERAN.c  \
          ../../asn1/lpp_asn1/srce/LPP_CNAV-ClockModel.c    \
          ../../asn1/lpp_asn1/srce/LPP_CommonIEsAbort.c     \
          ../../asn1/lpp_asn1/srce/LPP_CommonIEsError.c     \
          ../../asn1/lpp_asn1/srce/LPP_CommonIEsProvideAssistanceData.c    \
          ../../asn1/lpp_asn1/srce/LPP_CommonIEsProvideCapabilities.c      \
          ../../asn1/lpp_asn1/srce/LPP_CommonIEsProvideLocationInformation.c     \
          ../../asn1/lpp_asn1/srce/LPP_CommonIEsRequestAssistanceData.c    \
          ../../asn1/lpp_asn1/srce/LPP_CommonIEsRequestCapabilities.c      \
          ../../asn1/lpp_asn1/srce/LPP_CommonIEsRequestLocationInformation.c  \
          ../../asn1/lpp_asn1/srce/LPP_DGNSS-CorrectionsElement.c    \
          ../../asn1/lpp_asn1/srce/LPP_DGNSS-SatList.c         \
          ../../asn1/lpp_asn1/srce/LPP_DGNSS-SgnTypeElement.c     \
          ../../asn1/lpp_asn1/srce/LPP_DGNSS-SgnTypeList.c     \
          ../../asn1/lpp_asn1/srce/LPP_ECGI.c         \
          ../../asn1/lpp_asn1/srce/LPP_ECID-Error.c         \
          ../../asn1/lpp_asn1/srce/LPP_ECID-LocationServerErrorCauses.c    \
          ../../asn1/lpp_asn1/srce/LPP_ECID-ProvideCapabilities.c    \
          ../../asn1/lpp_asn1/srce/LPP_ECID-ProvideLocationInformation.c      \
          ../../asn1/lpp_asn1/srce/LPP_ECID-RequestCapabilities.c    \
          ../../asn1/lpp_asn1/srce/LPP_ECID-RequestLocationInformation.c      \
          ../../asn1/lpp_asn1/srce/LPP_ECID-SignalMeasurementInformation.c \
          ../../asn1/lpp_asn1/srce/LPP_ECID-TargetDeviceErrorCauses.c      \
          ../../asn1/lpp_asn1/srce/LPP_EllipsoidArc.c          \
          ../../asn1/lpp_asn1/srce/LPP_Ellipsoid-Point.c          \
          ../../asn1/lpp_asn1/srce/LPP_EllipsoidPointWithAltitude.c     \
          ../../asn1/lpp_asn1/srce/LPP_EllipsoidPointWithAltitudeAndUncertaintyEllipsoid.c   \
          ../../asn1/lpp_asn1/srce/LPP_Ellipsoid-PointWithUncertaintyCircle.c \
          ../../asn1/lpp_asn1/srce/LPP_EllipsoidPointWithUncertaintyEllipse.c \
          ../../asn1/lpp_asn1/srce/LPP_Environment.c           \
          ../../asn1/lpp_asn1/srce/LPP_EPDU.c               \
          ../../asn1/lpp_asn1/srce/LPP_EPDU-Body.c          \
          ../../asn1/lpp_asn1/srce/LPP_EPDU-ID.c               \
          ../../asn1/lpp_asn1/srce/LPP_EPDU-Identifier.c          \
          ../../asn1/lpp_asn1/srce/LPP_EPDU-Name.c          \
          ../../asn1/lpp_asn1/srce/LPP_EPDU-Sequence.c            \
          ../../asn1/lpp_asn1/srce/LPP_Error.c          \
          ../../asn1/lpp_asn1/srce/LPP_Error-r9-IEs.c          \
          ../../asn1/lpp_asn1/srce/LPP_GLONASS-ClockModel.c       \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-AcquisitionAssistance.c     \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-AcquisitionAssistanceReq.c     \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-AcquisitionAssistanceSupport.c \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-AcquisitionAssistElement.c     \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-AcquisitionAssistList.c     \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-Almanac.c          \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-AlmanacElement.c         \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-AlmanacList.c         \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-AlmanacReq.c          \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-AlmanacSupport.c         \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-AuxiliaryInformation.c      \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-AuxiliaryInformationReq.c      \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-AuxiliaryInformationSupport.c  \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-BadSignalList.c       \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-ClockModel.c          \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-CommonAssistanceDataSupport.c  \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-CommonAssistData.c       \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-CommonAssistDataReq.c    \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-DataBitAssistance.c         \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-DataBitAssistanceReq.c      \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-DataBitAssistanceSupport.c     \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-DataBitsReqSatElement.c     \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-DataBitsReqSatList.c        \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-DataBitsSatElement.c        \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-DataBitsSatList.c        \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-DataBitsSgnElement.c        \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-DataBitsSgnList.c        \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-DifferentialCorrections.c      \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-DifferentialCorrectionsReq.c      \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-DifferentialCorrectionsSupport.c  \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-EarthOrientationParameters.c      \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-EarthOrientationParametersReq.c   \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-EarthOrientationParametersSupport.c  \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-GenericAssistanceDataSupport.c \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-GenericAssistData.c         \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-GenericAssistDataElement.c     \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-GenericAssistDataReq.c      \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-GenericAssistDataReqElement.c  \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-GenericAssistDataSupportElement.c \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-ID.c               \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-ID-Bitmap.c           \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-ID-GLONASS.c          \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-ID-GLONASS-SatElement.c     \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-ID-GPS.c           \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-ID-GPS-SatElement.c         \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-IonosphericModel.c       \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-IonosphericModelReq.c    \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-IonosphericModelSupport.c      \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-LocationInformation.c    \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-LocationServerErrorCauses.c    \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-MeasurementForOneGNSS.c     \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-MeasurementList.c        \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-NavigationModel.c        \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-NavigationModelReq.c        \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-NavigationModelSupport.c    \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-NavModelSatelliteElement.c     \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-NavModelSatelliteList.c     \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-OrbitModel.c          \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-PositioningInstructions.c      \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-RealTimeIntegrity.c         \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-RealTimeIntegrityReq.c      \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-RealTimeIntegritySupport.c     \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-ReferenceLocation.c         \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-ReferenceLocationReq.c      \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-ReferenceLocationSupport.c     \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-ReferenceTime.c       \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-ReferenceTimeForOneCell.c      \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-ReferenceTimeReq.c       \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-ReferenceTimeSupport.c      \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-SatMeasElement.c         \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-SatMeasList.c         \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-SgnMeasElement.c         \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-SgnMeasList.c         \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-SignalID.c            \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-SignalIDs.c           \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-SignalMeasurementInformation.c \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-SupportElement.c         \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-SupportList.c         \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-SystemTime.c          \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-TargetDeviceErrorCauses.c      \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-TimeModelElement.c       \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-TimeModelElementReq.c    \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-TimeModelList.c       \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-TimeModelListReq.c       \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-TimeModelListSupport.c      \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-UTC-Model.c           \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-UTC-ModelReq.c        \
          ../../asn1/lpp_asn1/srce/LPP_GNSS-UTC-ModelSupport.c       \
          ../../asn1/lpp_asn1/srce/LPP_GPS-TOW-Assist.c           \
          ../../asn1/lpp_asn1/srce/LPP_GPS-TOW-AssistElement.c       \
          ../../asn1/lpp_asn1/srce/LPP_HorizontalAccuracy.c       \
          ../../asn1/lpp_asn1/srce/LPP_HorizontalVelocity.c       \
          ../../asn1/lpp_asn1/srce/LPP_HorizontalVelocityWithUncertainty.c \
          ../../asn1/lpp_asn1/srce/LPP_HorizontalWithVerticalVelocity.c    \
          ../../asn1/lpp_asn1/srce/LPP_HorizontalWithVerticalVelocityAndUncertainty.c  \
          ../../asn1/lpp_asn1/srce/LPP_Initiator.c          \
          ../../asn1/lpp_asn1/srce/LPP_KlobucharModelParameter.c        \
          ../../asn1/lpp_asn1/srce/LPP_LocationCoordinates.c         \
          ../../asn1/lpp_asn1/srce/LPP_LocationCoordinateTypes.c        \
          ../../asn1/lpp_asn1/srce/LPP_LocationError.c            \
          ../../asn1/lpp_asn1/srce/LPP_LocationFailureCause.c        \
          ../../asn1/lpp_asn1/srce/LPP_LocationInformationType.c        \
          ../../asn1/lpp_asn1/srce/LPP_MeasuredResultsElement.c         \
          ../../asn1/lpp_asn1/srce/LPP_MeasuredResultsList.c         \
          ../../asn1/lpp_asn1/srce/LPP_MeasurementReferenceTime.c    \
          ../../asn1/lpp_asn1/srce/LPP_Message.c               \
          ../../asn1/lpp_asn1/srce/LPP_MessageBody.c           \
          ../../asn1/lpp_asn1/srce/LPP_NAV-ClockModel.c           \
          ../../asn1/lpp_asn1/srce/LPP_NavModelCNAV-KeplerianSet.c      \
          ../../asn1/lpp_asn1/srce/LPP_NavModel-GLONASS-ECEF.c       \
          ../../asn1/lpp_asn1/srce/LPP_NavModelKeplerianSet.c        \
          ../../asn1/lpp_asn1/srce/LPP_NavModelNAV-KeplerianSet.c    \
          ../../asn1/lpp_asn1/srce/LPP_NavModel-SBAS-ECEF.c       \
          ../../asn1/lpp_asn1/srce/LPP_NeighbourMeasurementElement.c    \
          ../../asn1/lpp_asn1/srce/LPP_NeighbourMeasurementList.c    \
          ../../asn1/lpp_asn1/srce/LPP_NeQuickModelParameter.c       \
          ../../asn1/lpp_asn1/srce/LPP_NetworkTime.c           \
          ../../asn1/lpp_asn1/srce/LPP_OTDOA-Error.c           \
          ../../asn1/lpp_asn1/srce/LPP_OTDOA-LocationServerErrorCauses.c      \
          ../../asn1/lpp_asn1/srce/LPP_OTDOA-MeasQuality.c        \
          ../../asn1/lpp_asn1/srce/LPP_OTDOA-NeighbourCellInfoElement.c    \
          ../../asn1/lpp_asn1/srce/LPP_OTDOA-NeighbourCellInfoList.c    \
          ../../asn1/lpp_asn1/srce/LPP_OTDOA-NeighbourFreqInfo.c        \
          ../../asn1/lpp_asn1/srce/LPP_OTDOA-ProvideAssistanceData.c    \
          ../../asn1/lpp_asn1/srce/LPP_OTDOA-ProvideCapabilities.c      \
          ../../asn1/lpp_asn1/srce/LPP_OTDOA-ProvideLocationInformation.c  \
          ../../asn1/lpp_asn1/srce/LPP_OTDOA-ReferenceCellInfo.c        \
          ../../asn1/lpp_asn1/srce/LPP_OTDOA-RequestAssistanceData.c    \
          ../../asn1/lpp_asn1/srce/LPP_OTDOA-RequestCapabilities.c      \
          ../../asn1/lpp_asn1/srce/LPP_OTDOA-RequestLocationInformation.c  \
          ../../asn1/lpp_asn1/srce/LPP_OTDOA-SignalMeasurementInformation.c   \
          ../../asn1/lpp_asn1/srce/LPP_OTDOA-TargetDeviceErrorCauses.c     \
          ../../asn1/lpp_asn1/srce/LPP_PeriodicalReportingCriteria.c    \
          ../../asn1/lpp_asn1/srce/LPP_Polygon.c               \
          ../../asn1/lpp_asn1/srce/LPP_PolygonPoints.c            \
          ../../asn1/lpp_asn1/srce/LPP_PositioningModes.c         \
          ../../asn1/lpp_asn1/srce/LPP_ProvideAssistanceData.c       \
          ../../asn1/lpp_asn1/srce/LPP_ProvideAssistanceData-r9-IEs.c      \
          ../../asn1/lpp_asn1/srce/LPP_ProvideCapabilities.c         \
          ../../asn1/lpp_asn1/srce/LPP_ProvideCapabilities-r9-IEs.c     \
          ../../asn1/lpp_asn1/srce/LPP_ProvideLocationInformation.c     \
          ../../asn1/lpp_asn1/srce/LPP_ProvideLocationInformation-r9-IEs.c \
          ../../asn1/lpp_asn1/srce/LPP_PRS-Info.c           \
          ../../asn1/lpp_asn1/srce/LPP_QoS.c           \
          ../../asn1/lpp_asn1/srce/LPP_ReportingDuration.c        \
          ../../asn1/lpp_asn1/srce/LPP_ReqNavListInfo.c           \
          ../../asn1/lpp_asn1/srce/LPP_RequestAssistanceData.c       \
          ../../asn1/lpp_asn1/srce/LPP_RequestAssistanceData-r9-IEs.c      \
          ../../asn1/lpp_asn1/srce/LPP_RequestCapabilities.c         \
          ../../asn1/lpp_asn1/srce/LPP_RequestCapabilities-r9-IEs.c     \
          ../../asn1/lpp_asn1/srce/LPP_RequestLocationInformation.c     \
          ../../asn1/lpp_asn1/srce/LPP_RequestLocationInformation-r9-IEs.c \
          ../../asn1/lpp_asn1/srce/LPP_ResponseTime.c          \
          ../../asn1/lpp_asn1/srce/LPP_SatListRelatedDataElement.c      \
          ../../asn1/lpp_asn1/srce/LPP_SatListRelatedDataList.c         \
          ../../asn1/lpp_asn1/srce/LPP_SBAS-ClockModel.c          \
          ../../asn1/lpp_asn1/srce/LPP_SBAS-ID.c               \
          ../../asn1/lpp_asn1/srce/LPP_SBAS-IDs.c           \
          ../../asn1/lpp_asn1/srce/LPP_SequenceNumber.c           \
          ../../asn1/lpp_asn1/srce/LPP_StandardClockModelElement.c      \
          ../../asn1/lpp_asn1/srce/LPP_StandardClockModelList.c         \
          ../../asn1/lpp_asn1/srce/LPP_StoredNavListInfo.c        \
          ../../asn1/lpp_asn1/srce/LPP_SupportedBandEUTRA.c       \
          ../../asn1/lpp_asn1/srce/LPP_SV-ID.c              \
          ../../asn1/lpp_asn1/srce/LPP_TransactionID.c            \
          ../../asn1/lpp_asn1/srce/LPP_TransactionNumber.c        \
          ../../asn1/lpp_asn1/srce/LPP_TriggeredReportingCriteria.c     \
          ../../asn1/lpp_asn1/srce/LPP_UTC-ModelSet1.c            \
          ../../asn1/lpp_asn1/srce/LPP_UTC-ModelSet2.c            \
          ../../asn1/lpp_asn1/srce/LPP_UTC-ModelSet3.c            \
          ../../asn1/lpp_asn1/srce/LPP_UTC-ModelSet4.c            \
          ../../asn1/lpp_asn1/srce/LPP_Velocity.c           \
          ../../asn1/lpp_asn1/srce/LPP_VelocityTypes.c            \
          ../../asn1/lpp_asn1/srce/LPP_VerticalAccuracy.c         \
            \
          ../../asn1/supl_asn1/srce/SUPL_ULP_RSRQ-Range.c                        \
          ../../asn1/supl_asn1/srce/SUPL_ULP_RTD.c                                \
          ../../asn1/supl_asn1/srce/SUPL_ULP_RTDUnits.c                        \
          ../../asn1/supl_asn1/srce/SUPL_ULP_RelativeTime.c                    \
          ../../asn1/supl_asn1/srce/SUPL_ULP_RepModeOptions.c                    \
          ../../asn1/supl_asn1/srce/SUPL_ULP_RepModeSupport.c                    \
          ../../asn1/supl_asn1/srce/SUPL_ULP_RepeatedReportingParams.c            \
          ../../asn1/supl_asn1/srce/SUPL_ULP_ReportData.c                        \
          ../../asn1/supl_asn1/srce/SUPL_ULP_ReportDataList.c                    \
          ../../asn1/supl_asn1/srce/SUPL_ULP_ReportedLocation.c                \
          ../../asn1/supl_asn1/srce/SUPL_ULP_ReportingCap.c                    \
          ../../asn1/supl_asn1/srce/SUPL_ULP_ReportingCriteria.c                \
          ../../asn1/supl_asn1/srce/SUPL_ULP_ReportingMode.c                    \
          ../../asn1/supl_asn1/srce/SUPL_ULP_ReqDataBitAssistanceList.c        \
          ../../asn1/supl_asn1/srce/SUPL_ULP_RequestedAssistData.c                \
          ../../asn1/supl_asn1/srce/SUPL_ULP_ResultCode.c                        \
          ../../asn1/supl_asn1/srce/SUPL_ULP_SET-GANSSReferenceTime.c            \
          ../../asn1/supl_asn1/srce/SUPL_ULP_SETAuthKey.c                        \
          ../../asn1/supl_asn1/srce/SUPL_ULP_SETCapabilities.c                    \
          ../../asn1/supl_asn1/srce/SUPL_ULP_SETId.c                            \
          ../../asn1/supl_asn1/srce/SUPL_ULP_SLPAddress.c                        \
          ../../asn1/supl_asn1/srce/SUPL_ULP_SLPMode.c                            \
          ../../asn1/supl_asn1/srce/SUPL_ULP_SPCSETKey.c                        \
          ../../asn1/supl_asn1/srce/SUPL_ULP_SPCSETKeylifetime.c                \
          ../../asn1/supl_asn1/srce/SUPL_ULP_SPCTID.c                            \
          ../../asn1/supl_asn1/srce/SUPL_ULP_SUPLAUTHREQ.c                        \
          ../../asn1/supl_asn1/srce/SUPL_ULP_SUPLAUTHRESP.c                    \
          ../../asn1/supl_asn1/srce/SUPL_ULP_SUPLEND.c                            \
          ../../asn1/supl_asn1/srce/SUPL_ULP_SUPLINIT.c                        \
          ../../asn1/supl_asn1/srce/SUPL_ULP_SUPLPOS.c                            \
          ../../asn1/supl_asn1/srce/SUPL_ULP_SUPLPOSINIT.c                        \
          ../../asn1/supl_asn1/srce/SUPL_ULP_SUPLRESPONSE.c                    \
          ../../asn1/supl_asn1/srce/SUPL_ULP_SUPLSTART.c                        \
          ../../asn1/supl_asn1/srce/SUPL_ULP_SatelliteInfo.c                    \
          ../../asn1/supl_asn1/srce/SUPL_ULP_SatelliteInfoElement.c            \
          ../../asn1/supl_asn1/srce/SUPL_ULP_SatellitesListRelatedData.c        \
          ../../asn1/supl_asn1/srce/SUPL_ULP_SatellitesListRelatedDataList.c    \
          ../../asn1/supl_asn1/srce/SUPL_ULP_ServiceCapabilities.c                \
          ../../asn1/supl_asn1/srce/SUPL_ULP_ServicesSupported.c                \
          ../../asn1/supl_asn1/srce/SUPL_ULP_SessionCapabilities.c                \
          ../../asn1/supl_asn1/srce/SUPL_ULP_SessionID.c                        \
          ../../asn1/supl_asn1/srce/SUPL_ULP_SessionInformation.c                \
          ../../asn1/supl_asn1/srce/SUPL_ULP_SessionList.c                        \
          ../../asn1/supl_asn1/srce/SUPL_ULP_SetSessionID.c                    \
          ../../asn1/supl_asn1/srce/SUPL_ULP_SlpSessionID.c                    \
          ../../asn1/supl_asn1/srce/SUPL_ULP_Status.c                            \
          ../../asn1/supl_asn1/srce/SUPL_ULP_StatusCode.c                        \
          ../../asn1/supl_asn1/srce/SUPL_ULP_Supported3GPP2PosProtocolVersion.c\
          ../../asn1/supl_asn1/srce/SUPL_ULP_SupportedNetworkInformation.c        \
          ../../asn1/supl_asn1/srce/SUPL_ULP_SupportedWCDMAInfo.c                \
          ../../asn1/supl_asn1/srce/SUPL_ULP_SupportedWLANApData.c                \
          ../../asn1/supl_asn1/srce/SUPL_ULP_SupportedWLANApsChannel11a.c        \
          ../../asn1/supl_asn1/srce/SUPL_ULP_SupportedWLANApsChannel11bg.c        \
          ../../asn1/supl_asn1/srce/SUPL_ULP_SupportedWLANApsList.c            \
          ../../asn1/supl_asn1/srce/SUPL_ULP_SupportedWLANInfo.c                \
          ../../asn1/supl_asn1/srce/SUPL_ULP_TAResolution.c                    \
          ../../asn1/supl_asn1/srce/SUPL_ULP_TGSN.c                            \
          ../../asn1/supl_asn1/srce/SUPL_ULP_ThirdParty.c                        \
          ../../asn1/supl_asn1/srce/SUPL_ULP_ThirdPartyID.c                    \
          ../../asn1/supl_asn1/srce/SUPL_ULP_TimeStamp.c                        \
          ../../asn1/supl_asn1/srce/SUPL_ULP_TimeWindow.c                        \
          ../../asn1/supl_asn1/srce/SUPL_ULP_TimeslotISCP-List.c                \
          ../../asn1/supl_asn1/srce/SUPL_ULP_TimeslotISCP.c                    \
          ../../asn1/supl_asn1/srce/SUPL_ULP_TimingAdvance.c                    \
          ../../asn1/supl_asn1/srce/SUPL_ULP_TrackingAreaCode.c                \
          ../../asn1/supl_asn1/srce/SUPL_ULP_TriggerParams.c                    \
          ../../asn1/supl_asn1/srce/SUPL_ULP_TriggerType.c                        \
          ../../asn1/supl_asn1/srce/SUPL_ULP_UARFCN.c                            \
          ../../asn1/supl_asn1/srce/SUPL_ULP_ULP-PDU.c                            \
          ../../asn1/supl_asn1/srce/SUPL_ULP_UMBAreaId.c                        \
          ../../asn1/supl_asn1/srce/SUPL_ULP_UTRA-CarrierRSSI.c                \
          ../../asn1/supl_asn1/srce/SUPL_ULP_UTRAN-GANSSReferenceTime.c        \
          ../../asn1/supl_asn1/srce/SUPL_ULP_UTRAN-GANSSReferenceTimeAssistance.c        \
          ../../asn1/supl_asn1/srce/SUPL_ULP_UTRAN-GANSSReferenceTimeResult.c    \
          ../../asn1/supl_asn1/srce/SUPL_ULP_UTRAN-GPSReferenceTime.c            \
          ../../asn1/supl_asn1/srce/SUPL_ULP_UTRAN-GPSReferenceTimeAssistance.c\
          ../../asn1/supl_asn1/srce/SUPL_ULP_UTRAN-GPSReferenceTimeResult.c    \
          ../../asn1/supl_asn1/srce/SUPL_ULP_UTRANGANSSDriftRate.c                \
          ../../asn1/supl_asn1/srce/SUPL_ULP_UTRANGPSDriftRate.c                \
          ../../asn1/supl_asn1/srce/SUPL_ULP_UlpMessage.c                        \
          ../../asn1/supl_asn1/srce/SUPL_ULP_UmbCellInformation.c                \
          ../../asn1/supl_asn1/srce/SUPL_ULP_Velocity.c                        \
          ../../asn1/supl_asn1/srce/SUPL_ULP_Ver.c                                \
          ../../asn1/supl_asn1/srce/SUPL_ULP_Ver2-CellInfo-extension.c            \
          ../../asn1/supl_asn1/srce/SUPL_ULP_Ver2-Notification-extension.c        \
          ../../asn1/supl_asn1/srce/SUPL_ULP_Ver2-PosPayLoad-extension.c        \
          ../../asn1/supl_asn1/srce/SUPL_ULP_Ver2-PosProtocol-extension.c        \
          ../../asn1/supl_asn1/srce/SUPL_ULP_Ver2-PosTechnology-extension.c    \
          ../../asn1/supl_asn1/srce/SUPL_ULP_Ver2-RequestedAssistData-extension.c        \
          ../../asn1/supl_asn1/srce/SUPL_ULP_Ver2-SETCapabilities-extension.c    \
          ../../asn1/supl_asn1/srce/SUPL_ULP_Ver2-SUPL-END-extension.c            \
          ../../asn1/supl_asn1/srce/SUPL_ULP_Ver2-SUPL-INIT-extension.c        \
          ../../asn1/supl_asn1/srce/SUPL_ULP_Ver2-SUPL-POS-INIT-extension.c    \
          ../../asn1/supl_asn1/srce/SUPL_ULP_Ver2-SUPL-POS-extension.c            \
          ../../asn1/supl_asn1/srce/SUPL_ULP_Ver2-SUPL-RESPONSE-extension.c    \
          ../../asn1/supl_asn1/srce/SUPL_ULP_Ver2-SUPL-START-extension.c        \
          ../../asn1/supl_asn1/srce/SUPL_ULP_Ver2-SUPLNOTIFY.c                    \
          ../../asn1/supl_asn1/srce/SUPL_ULP_Ver2-SUPLNOTIFYRESPONSE.c            \
          ../../asn1/supl_asn1/srce/SUPL_ULP_Ver2-SUPLREPORT.c                    \
          ../../asn1/supl_asn1/srce/SUPL_ULP_Ver2-SUPLSETINIT.c                \
          ../../asn1/supl_asn1/srce/SUPL_ULP_Ver2-SUPLTRIGGEREDRESPONSE.c        \
          ../../asn1/supl_asn1/srce/SUPL_ULP_Ver2-SUPLTRIGGEREDSTART.c            \
          ../../asn1/supl_asn1/srce/SUPL_ULP_Ver2-SUPLTRIGGEREDSTOP.c            \
          ../../asn1/supl_asn1/srce/SUPL_ULP_Version.c                            \
          ../../asn1/supl_asn1/srce/SUPL_ULP_WCDMAAreaId.c                        \
          ../../asn1/supl_asn1/srce/SUPL_ULP_WLANAreaId.c                        \
          ../../asn1/supl_asn1/srce/SUPL_ULP_WcdmaCellInformation.c            \
          ../../asn1/supl_asn1/srce/SUPL_ULP_WimaxAreaId.c                        \
          ../../asn1/supl_asn1/srce/SUPL_ULP_WimaxBSInformation.c                \
          ../../asn1/supl_asn1/srce/SUPL_ULP_WimaxBsID.c                        \
          ../../asn1/supl_asn1/srce/SUPL_ULP_WimaxNMR.c                        \
          ../../asn1/supl_asn1/srce/SUPL_ULP_WimaxNMRList.c                    \
          ../../asn1/supl_asn1/srce/SUPL_ULP_WimaxRTD.c                        \
          ../../asn1/supl_asn1/srce/SUPL_ULP_WlanAPInformation.c                \
          ../../asn1/supl_asn1/srce/SUPL_ULP_RSRP-Range.c                        \
          ../../asn1/supl_asn1/srce/SUPL_ULP_AllowedReportingType.c            \
          ../../asn1/supl_asn1/srce/SUPL_ULP_AltitudeInfo.c                    \
          ../../asn1/supl_asn1/srce/SUPL_ULP_ApplicationID.c                    \
          ../../asn1/supl_asn1/srce/SUPL_ULP_AreaEventParams.c                    \
          ../../asn1/supl_asn1/srce/SUPL_ULP_AreaEventType.c                    \
          ../../asn1/supl_asn1/srce/SUPL_ULP_AreaId.c                            \
          ../../asn1/supl_asn1/srce/SUPL_ULP_AreaIdList.c                        \
          ../../asn1/supl_asn1/srce/SUPL_ULP_AreaIdSet.c                        \
          ../../asn1/supl_asn1/srce/SUPL_ULP_AreaIdSetType.c                    \
          ../../asn1/supl_asn1/srce/SUPL_ULP_BasicProtectionParams.c            \
          ../../asn1/supl_asn1/srce/SUPL_ULP_BatchRepCap.c                        \
          ../../asn1/supl_asn1/srce/SUPL_ULP_BatchRepConditions.c                \
          ../../asn1/supl_asn1/srce/SUPL_ULP_BatchRepType.c                    \
          ../../asn1/supl_asn1/srce/SUPL_ULP_CDMAAreaId.c                        \
          ../../asn1/supl_asn1/srce/SUPL_ULP_CPICH-Ec-N0.c                        \
          ../../asn1/supl_asn1/srce/SUPL_ULP_CPICH-RSCP.c                        \
          ../../asn1/supl_asn1/srce/SUPL_ULP_CauseCode.c                        \
          ../../asn1/supl_asn1/srce/SUPL_ULP_CdmaCellInformation.c                \
          ../../asn1/supl_asn1/srce/SUPL_ULP_CellGlobalIdEUTRA.c                \
          ../../asn1/supl_asn1/srce/SUPL_ULP_CellIdentity.c                    \
          ../../asn1/supl_asn1/srce/SUPL_ULP_CellInfo.c                        \
          ../../asn1/supl_asn1/srce/SUPL_ULP_CellMeasuredResults.c                \
          ../../asn1/supl_asn1/srce/SUPL_ULP_CellMeasuredResultsList.c            \
          ../../asn1/supl_asn1/srce/SUPL_ULP_CellParametersID.c                \
          ../../asn1/supl_asn1/srce/SUPL_ULP_ChipRate.c                        \
          ../../asn1/supl_asn1/srce/SUPL_ULP_CircularArea.c                    \
          ../../asn1/supl_asn1/srce/SUPL_ULP_Coordinate.c                        \
          ../../asn1/supl_asn1/srce/SUPL_ULP_DGANSS-Sig-Id-Req.c                \
          ../../asn1/supl_asn1/srce/SUPL_ULP_EllipticalArea.c                    \
          ../../asn1/supl_asn1/srce/SUPL_ULP_EncodingType.c                    \
          ../../asn1/supl_asn1/srce/SUPL_ULP_EventTriggerCapabilities.c        \
          ../../asn1/supl_asn1/srce/SUPL_ULP_ExtendedEphCheck.c                \
          ../../asn1/supl_asn1/srce/SUPL_ULP_ExtendedEphemeris.c                \
          ../../asn1/supl_asn1/srce/SUPL_ULP_FQDN.c                            \
          ../../asn1/supl_asn1/srce/SUPL_ULP_FormatIndicator.c                    \
          ../../asn1/supl_asn1/srce/SUPL_ULP_FrequencyInfo.c                    \
          ../../asn1/supl_asn1/srce/SUPL_ULP_FrequencyInfoFDD.c                \
          ../../asn1/supl_asn1/srce/SUPL_ULP_FrequencyInfoTDD.c                \
          ../../asn1/supl_asn1/srce/SUPL_ULP_GANSSPositionMethod.c                \
          ../../asn1/supl_asn1/srce/SUPL_ULP_GANSSPositionMethods.c            \
          ../../asn1/supl_asn1/srce/SUPL_ULP_GANSSPositioningMethodTypes.c        \
          ../../asn1/supl_asn1/srce/SUPL_ULP_GANSSSignals.c                    \
          ../../asn1/supl_asn1/srce/SUPL_ULP_GANSSSignalsDescription.c            \
          ../../asn1/supl_asn1/srce/SUPL_ULP_GANSSextEphTime.c                    \
          ../../asn1/supl_asn1/srce/SUPL_ULP_GANSSsignalsInfo.c                \
          ../../asn1/supl_asn1/srce/SUPL_ULP_GNSSPosTechnology.c                \
          ../../asn1/supl_asn1/srce/SUPL_ULP_GPSTime.c                            \
          ../../asn1/supl_asn1/srce/SUPL_ULP_GSMAreaId.c                        \
          ../../asn1/supl_asn1/srce/SUPL_ULP_GanssAdditionalDataChoices.c        \
          ../../asn1/supl_asn1/srce/SUPL_ULP_GanssDataBits.c                    \
          ../../asn1/supl_asn1/srce/SUPL_ULP_GanssExtendedEphCheck.c            \
          ../../asn1/supl_asn1/srce/SUPL_ULP_GanssNavigationModelData.c        \
          ../../asn1/supl_asn1/srce/SUPL_ULP_GanssReqGenericData.c                \
          ../../asn1/supl_asn1/srce/SUPL_ULP_GanssRequestedCommonAssistanceDataList.c    \
          ../../asn1/supl_asn1/srce/SUPL_ULP_GanssRequestedGenericAssistanceDataList.c    \
          ../../asn1/supl_asn1/srce/SUPL_ULP_GeoAreaIndex.c                    \
          ../../asn1/supl_asn1/srce/SUPL_ULP_GeoAreaMappingList.c                \
          ../../asn1/supl_asn1/srce/SUPL_ULP_GeoAreaShapesSupported.c            \
          ../../asn1/supl_asn1/srce/SUPL_ULP_GeographicTargetArea.c            \
          ../../asn1/supl_asn1/srce/SUPL_ULP_GeographicTargetAreaList.c        \
          ../../asn1/supl_asn1/srce/SUPL_ULP_GsmCellInformation.c                \
          ../../asn1/supl_asn1/srce/SUPL_ULP_HRPDAreaId.c                        \
          ../../asn1/supl_asn1/srce/SUPL_ULP_HistoricReporting.c                \
          ../../asn1/supl_asn1/srce/SUPL_ULP_Horandveruncert.c                    \
          ../../asn1/supl_asn1/srce/SUPL_ULP_Horandvervel.c                    \
          ../../asn1/supl_asn1/srce/SUPL_ULP_Horvel.c                            \
          ../../asn1/supl_asn1/srce/SUPL_ULP_Horveluncert.c                    \
          ../../asn1/supl_asn1/srce/SUPL_ULP_HrpdCellInformation.c                \
          ../../asn1/supl_asn1/srce/SUPL_ULP_IPAddress.c                        \
          ../../asn1/supl_asn1/srce/SUPL_ULP_KeyIdentity.c                        \
          ../../asn1/supl_asn1/srce/SUPL_ULP_KeyIdentity4.c                    \
          ../../asn1/supl_asn1/srce/SUPL_ULP_LTEAreaId.c                        \
          ../../asn1/supl_asn1/srce/SUPL_ULP_LocationData.c                    \
          ../../asn1/supl_asn1/srce/SUPL_ULP_LocationEncodingDescriptor.c        \
          ../../asn1/supl_asn1/srce/SUPL_ULP_LocationId.c                        \
          ../../asn1/supl_asn1/srce/SUPL_ULP_LocationIdData.c                    \
          ../../asn1/supl_asn1/srce/SUPL_ULP_LteCellInformation.c                \
          ../../asn1/supl_asn1/srce/SUPL_ULP_MAC.c                                \
          ../../asn1/supl_asn1/srce/SUPL_ULP_MCC-MNC-Digit.c                    \
          ../../asn1/supl_asn1/srce/SUPL_ULP_MCC.c                                \
          ../../asn1/supl_asn1/srce/SUPL_ULP_MNC.c                                \
          ../../asn1/supl_asn1/srce/SUPL_ULP_MeasResultEUTRA.c                    \
          ../../asn1/supl_asn1/srce/SUPL_ULP_MeasResultListEUTRA.c                \
          ../../asn1/supl_asn1/srce/SUPL_ULP_MeasuredResults.c                    \
          ../../asn1/supl_asn1/srce/SUPL_ULP_MeasuredResultsList.c                \
          ../../asn1/supl_asn1/srce/SUPL_ULP_MultipleLocationIds.c                \
          ../../asn1/supl_asn1/srce/SUPL_ULP_NMR.c                                \
          ../../asn1/supl_asn1/srce/SUPL_ULP_NMRelement.c                        \
          ../../asn1/supl_asn1/srce/SUPL_ULP_NavigationModel.c                    \
          ../../asn1/supl_asn1/srce/SUPL_ULP_Notification.c                    \
          ../../asn1/supl_asn1/srce/SUPL_ULP_NotificationMode.c                \
          ../../asn1/supl_asn1/srce/SUPL_ULP_NotificationResponse.c            \
          ../../asn1/supl_asn1/srce/SUPL_ULP_NotificationType.c                \
          ../../asn1/supl_asn1/srce/SUPL_ULP_PLMN-Identity.c                    \
          ../../asn1/supl_asn1/srce/SUPL_ULP_Pathloss.c                        \
          ../../asn1/supl_asn1/srce/SUPL_ULP_PeriodicParams.c                    \
          ../../asn1/supl_asn1/srce/SUPL_ULP_PhysCellId.c                        \
          ../../asn1/supl_asn1/srce/SUPL_ULP_PolygonArea.c                        \
          ../../asn1/supl_asn1/srce/SUPL_ULP_PolygonDescription.c                \
          ../../asn1/supl_asn1/srce/SUPL_ULP_PosMethod.c                        \
          ../../asn1/supl_asn1/srce/SUPL_ULP_PosPayLoad.c                        \
          ../../asn1/supl_asn1/srce/SUPL_ULP_PosProtocol.c                        \
          ../../asn1/supl_asn1/srce/SUPL_ULP_PosProtocolVersion3GPP.c            \
          ../../asn1/supl_asn1/srce/SUPL_ULP_PosProtocolVersion3GPP2.c            \
          ../../asn1/supl_asn1/srce/SUPL_ULP_PosTechnology.c                    \
          ../../asn1/supl_asn1/srce/SUPL_ULP_Position.c                        \
          ../../asn1/supl_asn1/srce/SUPL_ULP_PositionData.c                    \
          ../../asn1/supl_asn1/srce/SUPL_ULP_PositionEstimate.c                \
          ../../asn1/supl_asn1/srce/SUPL_ULP_PrefMethod.c                        \
          ../../asn1/supl_asn1/srce/SUPL_ULP_PrimaryCCPCH-RSCP.c                \
          ../../asn1/supl_asn1/srce/SUPL_ULP_PrimaryCPICH-Info.c                \
          ../../asn1/supl_asn1/srce/SUPL_ULP_ProtLevel.c                        \
          ../../asn1/supl_asn1/srce/SUPL_ULP_ProtectionLevel.c                    \
          ../../asn1/supl_asn1/srce/SUPL_ULP_QoP.c                    \
           \
           supl/os_abstraction/pdu_defs.c                            \
           supl/supl_hash/supl_hmac.c




    LOCAL_C_INCLUDES +=\
            $(LOCAL_PATH)/supl/incl                      \
            $(LOCAL_PATH)/supl/asn1_interface            \
            $(LOCAL_PATH)/../../asn1/include            \
           $(LOCAL_PATH)/supl/asn1c_helper/asn1_helper/incl   \
           $(LOCAL_PATH)/../../asn1/rrlp_asn1/incl            \
           $(LOCAL_PATH)/../../asn1/supl_asn1/incl            \
           $(LOCAL_PATH)/../../asn1/lpp_asn1/incl             \
            $(LOCAL_PATH)/supl/os_abstraction            \
            $(LOCAL_PATH)/supl/GAD_helper                \
            $(LOCAL_PATH)/supl/rrlp_handler                \
            $(LOCAL_PATH)/supl/supl_module                \
            $(LOCAL_PATH)/supl/supl_hash



    LOCAL_C_INCLUDES += $(LOCAL_PATH)/lsim/incl


else
    LOCAL_STATIC_LIBRARIES := libgps/CG2900/lib/Android_gcc432_ARMv5te/libgps
endif
else
    $(warning "Compiling with SUPL Disabled" )

endif


#-----------------INCLUDE LIBRARIES -------------------


                      LOCAL_STATIC_LIBRARIES := libgps libpower


ifeq ($(LOCAL_ENABLE_SBEE),true)
    LOCAL_STATIC_LIBRARIES += libsbee RXN_security RXN_PGPS_API
endif

ifeq ($(LOCAL_AMIL_LCS_SOLUTION),true)
ifeq ($(LOCAL_ENABLE_CP),true)
    LOCAL_STATIC_LIBRARIES += libamilclient liblcsclient
else
ifeq ($(LOCAL_ENABLE_FREQ_AID),true)
    LOCAL_STATIC_LIBRARIES += libamilclient liblcsclient
else
ifeq ($(LOCAL_ENABLE_SUPL),true)
    LOCAL_STATIC_LIBRARIES += libamilclient
endif
endif
endif
ifeq ($(LOCAL_ENABLE_SUPL),true)
    LOCAL_STATIC_LIBRARIES += libsim uicc libsimcom
endif
endif

ifeq ($(LOCAL_ENABLE_CAIF_SOCKET),true)
ifeq ($(LOCAL_ENABLE_CP),true)
    LOCAL_STATIC_LIBRARIES += libamilatclient
endif
endif





ifeq ($(LOCAL_AMIL_LCS_SOLUTION),true)
LOCAL_SHARED_LIBRARIES := libc libutils libphonet libisimessage  libsms_server libcn libshmnetlnk libmalutil
else
LOCAL_SHARED_LIBRARIES := libc libutils
ifeq ($(LOCAL_ENABLE_CAIF_SOCKET),true)
ifeq ($(LBS_ENABLE_SILENT_REBOOT_SUPPORT),true)
    LOCAL_SHARED_LIBRARIES += libdbus
endif
endif
endif
# ---------------- BUILD_EXECUTABLE -------------------
ifeq ($(LOCAL_SET_NDK_BUILD),true)
    LOCAL_LDLIBS := -llog
endif

    LOCAL_PRELINK_MODULE := false
    LOCAL_MODULE:= lbsd
    LOCAL_MODULE_TAGS := optional


ifeq ($(LOCAL_SET_NDK_BUILD),true)
    define cmd-build-executable
$(eval linkargs := $(call host-path,$@.linkargs))
    @ echo 'Link arguments : ' $(linkargs)
    $(hide) echo \
        $(PRIVATE_LDSCRIPT_X) \
        -Wl,--gc-sections \
        -Wl,-z,nocopyreloc \
        --sysroot=$(call host-path,$(PRIVATE_SYSROOT)) \
        > $(linkargs)
        $(hide) $(foreach object, $(call host-path, $(PRIVATE_LINKER_OBJECTS_AND_LIBRARIES)), echo '$(object)' >> $(linkargs); )
        $(hide) $(foreach object, $(call host-path, $(PRIVATE_LDFLAGS)), echo '$(object)' >> $(linkargs); )
        $(hide) $(foreach object, $(call host-path, $(PRIVATE_LDLIBS)), echo '$(object)' >> $(linkargs); )
        $(hide) echo '-o' >> $(linkargs);
    $(hide) $(foreach object, $(call host-path, $(call host-path,$@)), echo '$(object)' >> $(linkargs); )
$(hide) $(PRIVATE_CXX) @$(linkargs)
endef
endif


include $(BUILD_EXECUTABLE)



endif








