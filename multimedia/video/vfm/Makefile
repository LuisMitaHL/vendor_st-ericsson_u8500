# vim:syntax=make:
# -*- Mode: Makefile -*-

#-------------------------------
# Default Config
#-------------------------------
#MMSYSTEM=x
#MMBOARD=x
include $(MMROOT)/shared/makefiles/Make.config


#----------------------------
#     Entry for Make.rules
#
#VPATH    : Search Path for prerequisites.(see default value in Make.rules)
#CFLAGS   : Extra flags to give to the C compiler. (see default value in Make.rules)g
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
#----------------------------

ifneq ($(MYMMSYSTEM),symbian)
CPPFLAGS += -I$(MM_IDT_DIR)
endif

CPPFLAGS += -I$(MMROOT)/video/components/common/nmf/arm/inc
CPPFLAGS += -I$(MMROOT)/video/vfm

// CPPFLAGS += -DPACKET_VIDEO_SUPPORT
CPPFLAGS += -D_CACHE_OPT_

ifeq ($(findstring 5500,$(PLATFORM)),5500)
    CPPFLAGS += -I$(MMROOT)/shared/omxil
    CPPFLAGS += -D__ndk5500_a0__ 
endif
LIBOBJ   = \
		VFM_Component \
                VFM_ParamAndConfig \
		VFM_Port \
		VFM_Memory \
		VFM_DDepUtility \
		VFM_NmfHost_ProcessingComponent

ifneq ($(PLATFORM),x86-linux)
ifneq ($(findstring 5500,$(PLATFORM)),5500)
LIBOBJ	+=	VFM_NmfMpc_ProcessingComponent 
endif
endif

ifneq ($(PLATFORM),x86-linux)
ifeq ($(findstring 5500,$(PLATFORM)),5500)
#CPPFLAGS += -D__ARM_NEON_SWAP__
#LIBOBJ += VFM_swap_vect
#VPATH += src/NEON
endif
endif

ifdef DEBUG
CPPFLAGS += -DDEBUG -D__DEBUG
LIBNAME   = st_vfm_debug
INSTALL_LIB = libst_vfm_debug.a
OBJ_EXT =odebug 
else
CPPFLAGS += -DRELEASE -D__RELEASE
LIBNAME   = st_vfm
INSTALL_LIB = libst_vfm.a
endif

INSTALL_HEADERDIR= include

include make.ost_conf
# from \bridge_sw\bridge\montblanc\multimedia\video\uid\inc\st_video_uid.hrh
# and \epoc32\include\assp\nomadik\nomadikuid.hrh
# KStUidVideoVFMDll = KStUidVideoDefine031 = 0x20024D92 = 537021842
OST_COMPONENT_ID = 537021842
OST_DICO_NAME = $(OST_COMPONENT_NAME)
CPPFLAGS+= -I$(OST_COMPONENT_INC)/autogentraces

# install idt files too
IDT_HEADERS = $(notdir $(wildcard $(addsuffix /*.idt,$(INSTALL_HEADERDIR))))
PATCHED_HEADERS += $(addprefix $(INSTALL_HEADER_DIR)/, $(INSTALL_HEADER) $(IDT_HEADERS))

GROUP_PATH = $(MMROOT)/video/vfm
GROUP_NAME = $(SYMBIAN_VARIANT_NAME)/group
SYMBIAN_PRJNAME = st_vfm
SYMBIAN_EXTRA_MMP_FLAG += "SMPSAFE"
SYMBIAN_EXTRA_MMP_FLAG += "DEFFILE ../../eabi/st_vfm.def"
SYMBIAN_SYSTEMINCLUDE +=/epoc32/include/assp/nomadik/nmf/share
SYMBIAN_SYSTEMINCLUDE +=/epoc32/include/osi/shared
SYMBIAN_SYSTEMINCLUDE +=/epoc32/include/osi/shared/nmf
SYMBIAN_SYSTEMINCLUDE +=/epoc32/include/osi/shared/host
SYMBIAN_SYSTEMINCLUDE +=/epoc32/include/osi/video
SYMBIAN_SYSTEMLIB += omxilenswrapper.lib
SYMBIAN_SYSTEMLIB += st_ens.lib
SYMBIAN_SYSTEMLIB += omxilmmhwbuffer.lib
SYMBIAN_SYSTEMLIB += hostee.lib
SYMBIAN_SYSTEMLIB += cm_user_api.lib
SYMBIAN_SYSTEMLIB += euser.lib
SYMBIAN_SYSTEMLIB += stuutils.lib
SYMBIAN_SYSTEMLIB += omxilosalservices_dev.lib
SYMBIAN_SYSTEMLIB += ost_wrapper.lib

SYMBIAN_SYSTEMSTATICLIB += omxilosalservices.lib
SYMBIAN_SYSTEMSTATICLIB += omxilensservices.lib
video_EXPORTDIR = include
SYMBIAN_ALL = symb-prj

ifeq ($(MYMMSYSTEM),symbian)
todo: lib $(SYMBIAN_ALL)
else
todo: lib
endif

#-------------------------------
# Generic Rules
#-------------------------------
include $(MMROOT)/shared/makefiles/Make.rules
include $(MMROOT)/video/makefiles/Make.symbian.rules

#-------------------------------
# local Rules
#-------------------------------

