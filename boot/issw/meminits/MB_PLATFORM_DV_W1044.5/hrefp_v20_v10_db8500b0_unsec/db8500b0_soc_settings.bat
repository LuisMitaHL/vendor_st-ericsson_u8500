armcc --apcs /ropi -g -DHREF db8500b0_soc_settings_toshiba_400mhz.c -o db8500b0_soc_settings_toshiba_400mhz.axf
fromelf --bin db8500b0_soc_settings_toshiba_400mhz.axf --output hrefp_v20_v10_db8500b0_unsec_toshiba_400mhz.bin

pause

exit