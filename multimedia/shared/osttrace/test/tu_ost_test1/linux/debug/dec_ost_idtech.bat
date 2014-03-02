title decode_ost

@rem *******************************
@rem Insure that perl is in in the path
@rem *******************************
@rem
@IF "%CLEARCASE_SERVER%" == "" (
	@echo Hum, perl not reachable through CLEARCASE_SERVER variable, try local perl...
	@perl --v > NUL
	@IF NOT "%ERRORLEVEL%" == "0" (
		@ECHO PERL not found 
		@GOTO fatal_error
	)
	@set PERLEXEC=perl
) ELSE (
	@set PERLEXEC="\\%CLEARCASE_SERVER%\perl\bin\perl"
)

@rem ******************************************************************
@rem Put in a varible the list of dico to use since current version does not support list in a file
@rem ******************************************************************
@call dico_list.txt.cmd
%PERLEXEC% -S ../../../../../osttools/ostdec/ostdec.pl --input=dicodir=../../../../../dictionaries --defines=dispmode=idtech --input=file=ost_trace.bin --input=type=combiprobe --output=ost_trace_ste.txt
..\..\..\..\..\osttools\ostdecoder\win32\bin\ostdecoder -r -D ../../../../../launcher/ostdico_list.txt -t ost_trace_idtech.txt ost_trace.bin
@IF NOT "%ERRORLEVEL%" == "0" (
	@ECHO OST decoder execution issue
	@GOTO fatal_error
)

@GOTO eof


:fatal_error
@SET _EXITCODE=1
pause


:eof
@rem ../../../../osttools/ostdecoder/win32/bin/ostdecoder -p -d P:\work_ost_w1036\multimedia\shared\dictionaries\ST_OSTTEST_COMMON_host_arm.xml -d P:\work_ost_w1036\multimedia\shared\dictionaries\ST_OSTTEST_COMMON_nmf_arm.xml -d P:\work_ost_w1036\multimedia\shared\dictionaries\ST_OSTTEST_COMMON_nmf_dsp.xml -d P:\work_ost_w1036\multimedia\shared\dictionaries\ST_OSTTEST_host_arm.xml --output=ost_trace.txt ost_trace.bin

