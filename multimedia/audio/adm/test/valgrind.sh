#!/bin/sh
# Assumes ./admd is already running in the background
# valgrind -v --leak-check=full --show-reachable=yes --leak-resolution=high --undef-value-errors=yes ./admd

./ste-adm-test play /bin/ls HSetOut
./ste-adm-test play /bin/ls Speaker
./ste-adm-test play /bin/ls Earpiece
./ste-adm-test record /dev/null HSetIn
./ste-adm-test record /dev/null Mic
./ste-adm-test voicecall
./ste-adm-test play /bin/ls Earpiece
./ste-adm-test record /dev/null HSetIn
./ste-adm-test mute_upstream 0
./ste-adm-test mute_upstream 1
./ste-adm-test vc_status
./ste-adm-test vctx_volume -100
./ste-adm-test vcrx_volume -100
./ste-adm-test map Speaker REF_Earpiece
./ste-adm-test map Speaker REF_Speaker
./ste-adm-test play /bin/ls Speaker

./ste-adm-test msg TERMINATE

