ROOT_DIR:=../$(ROOT_DIR)
include $(ROOT_DIR)/.config.mk
TAT_PERLLIB:=$(ROOT_DIR)/../tat/perllibs
TAT_PERL:=perl -I$(TAT_PERLLIB)
