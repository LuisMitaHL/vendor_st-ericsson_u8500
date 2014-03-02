# -*- Mode: Makefile -*-
# vim:syntax=make:
ifeq ($(findstring mmdsp,$(MMPROCESSOR)),mmdsp) # mmdsp intrinsics
   CPPFLAGS+=  -DMMDSP -DBUILD_FIXED -DUSE_24BITS_LIBBITSTREAM# BUILD_FIXED mean not float implementation name can be changed
   ifeq ($(MMCOMPILER),flexcc) # ac compilation
      #CPPFLAGS+= -D__flexcc2__ automatically defined by compiler
      ifndef NMFCONFC
	CFLAGS = -g -O3 -Wrestrict
      endif
   else # for x86_mmdsp
      CFLAGS=-g	
      CPPFLAGS+= -D_REENTRANT -DUSE_LONG_LONG_BIT_TRUE -DBIT_TRUE # all these flags should be removed, need mmdsptools update. Please remove them from code!!
   endif
else # all except mmdsp
   CPPFLAGS+= -I$(MM_HEADER_DIR)/audiolibs/fake_dsptools
   CFLAGS=-g -O3 
# stl support in Android environment: fetch stlport for now
# FIXME: maybe find another better stl location
  ifeq ($(STE_SOFTWARE),android)
    ifneq ($(findstring rvct,$(MMCOMPILER)),rvct)
      CPPFLAGS += -I$(ANDROID_BSP_ROOT)/external/stlport/stlport
      CPPFLAGS += -I$(libstdc++_root)/..
    endif
  endif

   ifeq ($(findstring cortexA9,$(MMPROCESSOR)),cortexA9) # ca9 with neon
      ifeq ($(findstring rvct,$(MMCOMPILER)),rvct)  # current care for mop-linux and mop-android compilations
         CPPFLAGS+= -DARM -DNEON -DARM_DATA_PRELOAD -DARM_INST_PRELOAD
         CPPFLAGS+= -I$(RVCT40INC)
         #CFLAGS   = -O3 -Otime --vectorize  --min_array_alignment=8
      endif
	   ifeq ($(MMPROCESSOR),x86_cortexA9)
              CPPFLAGS+= -DARM -DNEON -DSTM_ARM_NEON_MODELS -DSTM_ARM_MODELS
      endif
   else # no neon 
      ifeq ($(findstring rvct,$(MMCOMPILER)),rvct) # This -DARM is for all arm codes running on rvct (arm11/cortexA9.noneon)      
         CPPFLAGS+= -DARM
          ifeq ($(MMPROCESSOR),arm11) # RVDS_2_2 is specific flag to be enabled for cpu arm11 on rvds 2.2 
                CPPFLAGS+= -DRVDS_2_2 
           endif
      endif
   endif
endif

CPPFLAGS += -DNDEBUG

# Starts here

LIB_STANDALONE=
MEM_CFG_STANDALONE=
SUPPORT_CORTEXA9_STANDALONE=false
ifeq ($(MMPROCESSOR),cortexA9)
ifeq ($(MMCOMPILER),gnu)
SUPPORT_CORTEXA9_STANDALONE=true
endif
ifeq ($(MMCOMPILER),rvct4gnu)
SUPPORT_CORTEXA9_STANDALONE=true
endif
ifeq ($(MMCOMPILER),rvct)
SUPPORT_CORTEXA9_STANDALONE=true
endif
endif

ifeq ($(SUPPORT_CORTEXA9_STANDALONE),true)
COMPILER_DIR=cortexA9-x-rvct-x
ifeq ($(findstring gnu,$(MMCOMPILER)),gnu) 
COMPILER_DIR=cortexA9-x-rvct4gnu-x
endif
     ifeq ($(PPROFILER_ENV),true)
          CPPFLAGS+=-I$(MM_TOOLS_DIR)/audio/ARM/perf/include/ -DPPROFILER -UARM_CYCLES
          ifeq ($(MMCOMPILER),gnu)
          CFLAGS+= -finstrument-functions
          else
          CFLAGS+= --gnu-instrument
          endif
          LIBPROFILER=$(MM_TOOLS_DIR)/audio/ARM/perf/target/lib/$(COMPILER_DIR)/libtperf.a
          LIB_STANDALONE+=$(LIBPROFILER)
    else
          CPPFLAGS+=-I$(MM_TOOLS_DIR)/audio/ARM/pmu/include -UPPROFILER -DARM_CYCLES
          LIBARMPMU=$(MM_TOOLS_DIR)/audio/ARM/pmu/lib/$(COMPILER_DIR)/libpmu.a
          LIB_STANDALONE+=$(LIBARMPMU) 
    endif
    ifeq ($(USE_LINUX_ENV),true)
       # binary will run on Linux directly           
    else
          ifeq ($(MMCOMPILER),rvct4gnu)
             FIXED_LDEXEFLAGS=
             LD_EXE=armlink  #overwrite
          endif
          CPPFLAGS+=-UPPROFILER -UARM_CYCLES    
          LIBINITARM=$(MM_TOOLS_DIR)/audio/ARM/init/cortexA9/bin/$(COMPILER_DIR)/cortexinit
          LIB_STANDALONE+=$(LIBINITARM)
          MEMORYCFG=--scatter=$(MM_TOOLS_DIR)/audio/ARM/init/$(MMPROCESSOR)/scatter.scat
          MEM_CFG_STANDALONE+=--entry=0x2000
         MEM_CFG_STANDALONE+=$(MEMORYCFG)
    endif
endif

