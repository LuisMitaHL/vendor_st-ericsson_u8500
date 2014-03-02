#
# Copyright (C) ST-Ericsson SA 2010
#

# Use build flags to find as many problems as possible already at compile time.
CFLAGS_BASE  := -g -std=c99
CFLAGS_LOW  := -Wno-missing-field-initializers -Wno-unused-parameter -Wold-style-definition -Wpacked -Wstrict-aliasing=2 -Wundef -pedantic -Wmissing-braces -Wparentheses -Wno-variadic-macros -Winline
CFLAGS_MEDIUM := -Waggregate-return -Wlarger-than-65500 -Wredundant-decls -fno-common #-Wswitch-enum can't use until AT enumeration solved
CFLAGS_HIGH  := -Wall -W -Wbad-function-cast -Wcast-align -Wdeclaration-after-statement -Werror-implicit-function-declaration -Wextra -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat=2 -Winit-self -Wmissing-declarations -Wmissing-format-attribute -Wmissing-include-dirs -Wmissing-noreturn -Wmissing-prototypes -Wnested-externs -Wpointer-arith -Wshadow -Wstrict-prototypes -Wswitch-default -Wunsafe-loop-optimizations -Wwrite-strings #-Wunreachable-code
# does not build with Linux_SW_v0.9 LOCAL_CFLAGS := $(CFLAGS_BASE) $(CFLAGS_LOW) $(CFLAGS_MEDIUM) $(CFLAGS_HIGH)
#LOCAL_CFLAGS := -v (Use -v to check flags sent to compiler)
LOCAL_CFLAGS := -D_GNU_SOURCE -DATC_IN_ANDROID -DAT_SERVICE -DCFG_USE_ANDROID_LOG

ifeq ($(ATC_ENABLE_DEBUG),true)
LOCAL_CFLAGS += -DATC_ENABLE_DEBUG
endif

ifeq ($(ATC_ENABLE_FEATURE_READABILITY),true)
LOCAL_CFLAGS += -DENABLE_READABILITY
endif

ifeq ($(EXE_USE_AUDIO_SERVICE),true)
LOCAL_CFLAGS += -DEXE_USE_AUDIO_SERVICE
endif

ifeq ($(EXE_USE_CN_SERVICE),true)
LOCAL_CFLAGS += -DEXE_USE_CN_SERVICE
endif

ifeq ($(EXE_USE_COPS_SERVICE),true)
LOCAL_CFLAGS += -DEXE_USE_COPS_SERVICE
endif

ifeq ($(EXE_USE_PSCC_SERVICE),true)
LOCAL_CFLAGS += -DEXE_USE_PSCC_SERVICE
endif

ifeq ($(EXE_USE_SIM_SERVICE),true)
LOCAL_CFLAGS += -DEXE_USE_SIM_SERVICE
endif

ifeq ($(EXE_USE_SIMPB_SERVICE),true)
LOCAL_CFLAGS += -DEXE_USE_SIMPB_SERVICE
endif

ifeq ($(EXE_USE_SMS_SERVICE),true)
LOCAL_CFLAGS += -DEXE_USE_SMS_SERVICE
endif

ifeq ($(EXE_USE_CSPSA_SERVICE),true)
LOCAL_CFLAGS += -DEXE_USE_CSPSA_SERVICE
endif

ifeq ($(EXE_USE_ATC_CUSTOMER_EXTENSIONS),true)
LOCAL_CFLAGS += -DEXE_USE_ATC_CUSTOMER_EXTENSIONS
include $(ACCESS_SERVICES_PATH)/at/at_core_extension/inc_Compile_flags.mk
endif
