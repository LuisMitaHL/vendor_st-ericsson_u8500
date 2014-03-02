@echo off 
cd %~dp0 
..\bin\ostdecoder -D STE_DEFECT_108411\file.dicoxp -t ost_STE_DEFECT_108411_decoded_Dfile.txt    STE_DEFECT_108411\ost_trace.bin 
pause 
