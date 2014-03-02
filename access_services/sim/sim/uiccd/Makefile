######################### -*- Mode: Makefile-Gmake -*- ########################
## Copyright (C) 2010, ST-Ericsson
## 
## File name       : Makefile
## Description     : builds uicc library
## 
## Author          : Steve Critchlow <steve.critchlow@stericsson.com>
## 
###############################################################################


MODEM_DIR := ../../sim_msl/libmsl
CN_DIR := ../../../call_network/call_network

CC := gcc
CFLAGS   := -g -Wall -Wextra -std=c99 -pthread -m32 -ftest-coverage -fprofile-arcs -DHOST_FS_PATH='"$(HOST_FS_PATH)"'
INCDIRS  := -I../simd -I../libsimcom -I../libuicc -I../libsim -I../libapdu \
	    -I../include -I../cat -I$(MODEM_DIR) -I$(CN_DIR)/libcn/include -I./ -I./include
CPPFLAGS := $(INCDIRS)
LDFLAGS  :=
LDLIBS   := -lgcov

SRC := src/uiccd.c \
	   src/uiccd_msg.c \
	   src/uicc_sig.c \
       src/uicc_event_state_change_handler.c \
       src/uicc_channel_close_handler.c \
       src/uicc_channel_open_handler.c \
       src/uicc_channel_send_handler.c \
       src/uicc_file_ecc_handler.c \
       src/uicc_file_fdn_read_handler.c \
       src/uicc_file_icon_read_handler.c \
       src/uicc_file_plmn_common.c \
       src/uicc_file_plmn_read_handler.c \
       src/uicc_file_plmn_update_handler.c \
       src/uicc_file_rat_read_handler.c \
       src/uicc_other_sap_handler.c \
       src/uicc_other_sim_reset_handler.c \
       src/uicc_other_deprecated.c \
       src/uicc_other_app_status_handler.c \
       src/uicc_other_card_status_handler.c \
       src/uicc_other_get_sim_state_handler.c \
       src/uicc_other_app_info_handler.c \
       src/uicc_other_sim_status_handler.c \
       src/uicc_other_sim_power_on_off_handler.c \
       src/uicc_other_modem_silent_reset.c \
       src/uicc_file_sim_file_read_generic_handler.c \
       src/uicc_file_smsc_common.c \
       src/uicc_file_smsc_get_record_max_handler.c \
       src/uicc_file_smsc_read_handler.c \
       src/uicc_file_smsc_restore_from_record_handler.c \
       src/uicc_file_smsc_save_to_record_handler.c \
       src/uicc_file_smsc_update_handler.c \
       src/uicc_file_subscriber_number_read_handler.c \
       src/uicc_file_read_sim_file_record_handler.c \
       src/uicc_file_sim_file_get_format_handler.c \
       src/uicc_file_get_file_information_handler.c \
       src/uicc_file_read_sim_file_binary_handler.c \
       src/uicc_file_update_sim_file_record_handler.c \
       src/uicc_file_update_sim_file_binary_handler.c \
       src/uicc_file_service_table_get_handler.c \
       src/uicc_file_service_table_update_handler.c \
       src/uicc_file_service_availability_handler.c \
       src/uicc_file_service_table_common.c \
       src/uicc_pin_verify_handler.c \
       src/uicc_pin_disable_handler.c \
       src/uicc_pin_enable_handler.c \
       src/uicc_pin_info_handler.c \
       src/uicc_pin_change_handler.c \
       src/uicc_pin_unblock_handler.c \
       src/uicc_pin_caching_handler.c \
       src/uicc_state_machine.c

OBJ := $(SRC:.c=.o)
HDR := include/uiccd.h \
	   include/uiccd_msg.h \
       include/uicc_handlers.h \
       include/uicc_plmn_common.h \
       include/uicc_smsc_common.h \
       include/uicc_file_service_table_common.h \
       include/uicc_file_paths.h \
       include/uicc_other_sap_common.h \
       include/uicc_state_machine.h

.PHONY: all clean splint instrument_funcs

all: libuiccd.a

libuiccd.a: $(OBJ)
	rm -f libuiccd.a
	$(AR) crv libuiccd.a $(OBJ)

clean:
	rm -f libuiccd.a $(OBJ) *~ core

splint:
	splint $(INCDIRS) $(HDR) $(SRC)

instrument_funcs: CFLAGS += -finstrument-functions -finstrument-functions-exclude-function-list=main
instrument_funcs: all
