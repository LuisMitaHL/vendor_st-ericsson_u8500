ERROR=ABORT
WAIT=ON
journal  ON=./logs/journal.txt
log      ON=./logs/logfile.txt
STDIOLOG ON=./logs/stdiolog.txt
connect,reset,halt  "@Cortex-A9_1@RVI"
PAUSE 20
include ./init/init_ddr_ca9.inc 
PAUSE 20
