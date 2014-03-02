include $(MM_MAKEFILES_DIR)/SharedConfig.mk

#----------------------------
#     Entry for Make.rules
#
#VPATH    : Search Path for prerequisites.(see default value in Make.rules)
#CFLAGS   : Extra flags to give to the C compiler. (see default value in Make.rules)
#CXXFLAGS : Extra flags to give to the C++ compiler. (see default value in Make.rules)
#CPPFLAGS : Extra flags to give to the C preprocessor.(see default value in Make.rules)
#
#DEP_DEP  : Dependencies of dependency files. 
#           (for example generated header file before generating *.d)
#TOBECLEAN: List of files you want to delete on realclean target.
#
#LIBNAME  : Name of the library without the prefix lib.
#LIBOBJ   : List of object file to put in the lib
#
#EXENAME  : Name of executable
#EXEOBJ   : List of object file needed to build executable
#         For multiple executable, suffix these to variable with _1, _2 ...  
#
#LOCALLIBS: list of local libraries to be linked with executable
#
#OSILIBS
#
#RECOMPILED_PROJECT: list of project that must be recompiled (not for 8810 8815)
#
#
#----------------------------
ifeq ($(MYMMSYSTEM),linux)
  MMFLAVOUR = linux
else ifeq ($(MYMMSYSTEM),android)
  MMFLAVOUR = linux
endif

VPATH+= $(AUDIO_REPOSITORY_MPC)/host $(SHARED_REPOSITORY_MPC)/host

EXEOBJ= main FileBuffer $(COMMON_OBJ) 
EXENAME=cs3

INSTALL_BIN=cs3

ifeq ($(MMFLAVOUR),linux)
  MEMORYCFG=
  NMF_SUFFIX_NAME=$(subst stn,,$(NMF_CHIP_REP))
#  NMF_LIB_PATH=$(MMROOT)/shared/nmf/linux/lib/$(MYMMBOARD)-$(MYMMSYSTEM)
#  COMMON_LIBS += $(NMF_LIB_PATH)/liblos.a $(NMF_LIB_PATH)/libnmf.a $
  COMMON_LIBS += liblos.a libnmf.a libnmfee.a libnmftimer.a
  CPPFLAGS += -DNO_OSILIB
  POST_BUILD   =
endif

# UGLY patch to force $(AUDIO_REPOSITORY_MPC) BEFORE $(SHARED_REPOSITORY_MPC)
CPPFLAGS := -I$(AUDIO_REPOSITORY_MPC) $(CPPFLAGS) 
CPPFLAGS += -I$(AUDIO_REPOSITORY_HOST) -I$(COMPONENT_TOP_DIR)/linux/audiomodem_driver/inc

COMMON_LIBS+= $(NMFLIBS) $(OSILIBS) $(DEBUGLIBS)
COMMON_LIBS+= $(MM_AUDIO_NMF_REPO_DIR)/$(MMPROCESSOR)-x-$(MMCOMPILER)-x/cscall/nmf/host/composite/cscall.a
COMMON_LIBS+= $(MM_AUDIO_NMF_REPO_DIR)/$(MMPROCESSOR)-x-$(MMCOMPILER)-x/amr/nmfil/host/composite/decalgo.a
COMMON_LIBS+= $(MM_AUDIO_NMF_REPO_DIR)/$(MMPROCESSOR)-x-$(MMCOMPILER)-x/amr/nmfil/host/composite/encalgo.a
COMMON_LIBS+= libst_ensfsm_host.a
COMMON_LIBS+= libst_ens.a
COMMON_LIBS+= libosttrace.a libtrace.a

ADDITIONAL_LIBS=libamrcommon.a libamrdec.a libamrdec24.a  libamrenc.a libamrenc24.a
ADDITIONAL_LIBS+=libamrwbcommon.a libamrwbdec.a libamrwbdec24.a  libamrwbenc.a libamrwbenc24.a 
ADDITIONAL_LIBS+=libbitstream.a libbtrue.a 
ADDITIONAL_LIBS+=libgsmhr.a libgsmhrdec24.a libgsmhrenc24.a 
ADDITIONAL_LIBS+=libgsmfr.a libgsmfrdec24.a libgsmfrenc24.a
ADDITIONAL_LIBS+=libresampling.a

LIBS += $(COMMON_LIBS) ${ADDITIONAL_LIBS}

todo: lib exe


#-------------------------------
# Generic Rules
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedRules.mk


.SECONDARY:
