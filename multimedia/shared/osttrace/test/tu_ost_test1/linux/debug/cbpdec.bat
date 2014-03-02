title decode_cbpdec

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


%PERLEXEC% -S ../../../../../osttools/ostdec/cbpdec.pl --input=file=ost_trace.bin --input=type=combiprobe --output=cbpdec.txt
@IF NOT "%ERRORLEVEL%" == "0" (
	@ECHO PERL decoder failed 
	@GOTO fatal_error
)
@GOTO eof


:fatal_error
@SET _EXITCODE=1
pause


:eof
