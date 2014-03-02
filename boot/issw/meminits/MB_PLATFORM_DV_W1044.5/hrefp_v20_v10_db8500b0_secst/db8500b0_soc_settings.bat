armcc --apcs /ropi -g -DHREF db8500b0_soc_settings_toshiba_400mhz.c -o db8500b0_soc_settings_toshiba_400mhz.axf
fromelf --bin db8500b0_soc_settings_toshiba_400mhz.axf --output hrefp_v20_v10_db8500b0_secst_toshiba_400mhz.bin

armcc --apcs /ropi -g -DHREF db8500b0_soc_settings_toshiba_400mhz_1ghz.c -o db8500b0_soc_settings_toshiba_400mhz_1ghz.axf
fromelf --bin db8500b0_soc_settings_toshiba_400mhz_1ghz.axf --output hrefp_v20_v10_db8500b0_secst_toshiba_400mhz_1ghz.bin

pause

exit