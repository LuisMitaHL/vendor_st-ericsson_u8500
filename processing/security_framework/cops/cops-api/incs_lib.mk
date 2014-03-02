
ifdef COPS_API

CPPFLAGS+= -I$(COPS_API)/include

ifndef COPS_IN_LOADERS
LDADD+= -L$(COPS_API) -Wl,-rpath=$(COPS_API) -lcops
endif

endif #COPS_API

