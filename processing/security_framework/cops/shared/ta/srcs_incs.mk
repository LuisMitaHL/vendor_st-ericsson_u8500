
ifdef COPS_SHARED

VPATH += $(COPS_SHARED)
CPPFLAGS += -I$(COPS_SHARED)
ifndef COPS_TAPP_EMUL
CPPFLAGS += -I../../bass_app/teec/
ifeq ($(COPS_USE_GP_TEEV0_17), true)
CFLAGS   += -DCOPS_USE_GP_TEEV0_17
endif
endif

SRCS += cops_data_parameter.c
SRCS += cops_data_sipc.c
SRCS += cops_sipc_message.c
SRCS += cops_tapp_sipc.c

SRCS += cops_data_imei.c
SRCS +=  cops_data_lock.c

ifdef COPS_TAPP_LOCAL
SRCS += cops_gstate.c
endif

endif #COPS_SHARED
