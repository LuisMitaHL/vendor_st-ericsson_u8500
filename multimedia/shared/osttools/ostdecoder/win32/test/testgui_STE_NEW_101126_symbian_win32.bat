@echo off 
cd %~dp0 
..\ostviewer.bat --ostdecoder -h -a -D STE_NEW_101126\symbian\dicoList.txt STE_NEW_101126\symbian\st_osttest_carbide_fido_ost_symbian.bin 
pause 
