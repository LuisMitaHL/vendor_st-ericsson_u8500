# -*- Mode: Makefile -*-
# vim:syntax=make:

# -------------
# Note: To use this makefile you have to define:
# TESTLISTS list of testlist
# DATABASE name of test database
# -------------
ifndef TESTLISTS
  ifndef INLST
    $(error TESTLISTS not defined)
  else
    ifndef OUTLST
      $(error TESTLISTS not defined)
    endif
  endif
else
INLST=$(TESTLISTS)
OUTLST=$(TESTLISTS)
endif

ifndef DATABASE
$(error DATABASE not defined)
endif

ifeq ($(LSF_QUEUE),)
LSF_QUEUE   = short
endif
ifeq ($(LSF_PROJECT),)
LSF_PROJECT = 8815
endif
ifeq ($(LSF_TYPE),)
LSF_TYPE    = rh40
endif

ifneq ($(LSF),)
LSF_OPTIONS=--lsf --parallel=100
endif

ifeq ($(TEST_TAG),)
TEST_TAG=$(PLATFORM)
endif


export LSF_QUEUE
export LSF_PROJECT
export LSF_TYPE


test: $(DATABASE)_$(TEST_TAG)
	@echo Test done

$(DATABASE)_$(TEST_TAG):
	testengine $(TESTFLAGS) $(addprefix -i , $(INLST)) $(addprefix -o , $(OUTLST)) -t $(TEST_TAG) -d $(DATABASE) $(LSF_OPTIONS)

cleantest:
	cleantests --database $(DATABASE) --tag $(TEST_TAG) $(addprefix -i , $(OUTLST))

