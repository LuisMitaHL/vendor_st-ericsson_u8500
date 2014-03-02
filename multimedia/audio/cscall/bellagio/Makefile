################################################################################
#                        (C) ST-Ericsson
#    Reproduction and Communication of this document is strictly prohibited 
#          unless specifically authorized in writing by ST-Ericsson.
################################################################################


include $(MM_MAKEFILES_DIR)/SharedConfig.mk
# We are generating an OMX component .so - request to be installed in correct place
IS_OMX_COMPONENT_LIB:=true
include $(MM_MAKEFILES_DIR)/STEOMXLoaderConfig.mk

CPPFLAGS+=-I../common -I../proxy

SOLIBNAMES+=ste_cscall
LIBOBJ_ste_cscall = ste_omx_ens_component

AUDIO_LIBS = st_cscall_host gsmhr gsmfr gsmhrdec24 gsmfrdec24 gsmfrenc24 gsmhrenc24 st_GSMHRDecoder st_GSMFRDecoder st_GSMFREncoder st_GSMHREncoder vector

AUDIO_NMF_HOST_COMPONENTS = cscall.nmf.host.composite.cscall cscall.nmf.host.composite.cscallLoopULDLAlg gsmfr.nmfil.host.composite.decalgo gsmfr.nmfil.host.composite.encalgo gsmhr.nmfil.host.composite.decalgo gsmhr.nmfil.host.composite.encalgo
SOLIBS_ste_cscall += $(AUDIO_NMF_HOST_COMPONENTS_LIBS)

EXTRALIBS_ste_cscall= $(AUDIO_LIBS)
FLAGS_ste_cscall = $(LDFLAGS_STE_ENS_OMX_COMPONENTS) -lste_ens_audio_common -lnmftimer \
	  	    -lste_dec_amr -lste_enc_amr -lste_dec_amrwb -lste_enc_amrwb -lste_ens_audio_samplerateconv

# Link with mmprobe if feature enabled in BoardConfig.mk
ifeq ($(MMPROBE_ENABLE_FEATURE_MMPROBE),true)
FLAGS_ste_cscall += -lmmprobe
endif

ifneq ($(MMCOMPILER),android)
FLAGS_ste_cscall += -lpthread 
endif

CPPFLAGS += $(EXTRA_CPPFLAGS)
 
ifeq ($(MMCOMPILER),android)
ifneq (CSCALL_ENABLE_FEATURE_CAIF,$(findstring CSCALL_ENABLE_FEATURE_CAIF,$(CPPFLAGS)))
FLAGS_ste_cscall += -L$(ANDROID_ROOTFS_DIR)/obj/SHARED_LIBRARIES/libshmnetlnk_intermediates/LINKED/
FLAGS_ste_cscall += -lshmnetlnk
endif
endif

ifeq (u5500,$(filter u5500%,$(STE_PLATFORM)))
FLAGS_ste_cscall += -L$(ANDROID_ROOTFS_DIR)/obj/STATIC_LIBRARIES/libsamc_client_intermediates/ \
                   -L$(ANDROID_ROOTFS_DIR)/obj/SHARED_LIBRARIES/libmfl_intermediates/LINKED/ \
                   -L$(ANDROID_ROOTFS_DIR)/obj/SHARED_LIBRARIES/libmfl_samc_intermediates/LINKED/ \
                   -lsamc_client -lmfl -lmfl_samc
endif

INSTALL_SOLIB = ste_cscall


todo: solib 

include $(MM_MAKEFILES_DIR)/SharedRules.mk

