/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

SMS ltp instructions

--------
-to run-
--------
-start sms server
cd /usr/sbin
./sms_stub&
./sms_server&

-run ltp
cd /opt/ltp
./runltp -o sms_out -f sms_command

-logs and results files are here:
/opt/ltp/output/sms_out
/opt/ltp/results/LTP_x

N.B. Logs from test code, which is running within LTP, are saved to file and will not appear in
stdout.
