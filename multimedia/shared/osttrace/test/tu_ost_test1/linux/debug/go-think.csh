
#!/bin/tcsh
#set OPT=/StripPART \"multimedia/\" /SOURCEPATH \"%MMROOT%\"
set OPT=

perl -S ../../../launcher/launcher.pl --binary ${MMROOT}/shared/examples/test_ost_2/host/bin/cortexA9-think-rvct-href_v1/test_ost_2.axf --debug_port T32:E04040005911 --mode 'dbg/bkpt=$Super$$main' --platform href_v1 --config LOAD_ARGS="${OPT}" --config DBG_USER="${MMROOT}/shared/examples/test_ost_2/debug/trace.cmm" --runpath "${MMROOT}/shared/examples/test_ost_2/debug"


