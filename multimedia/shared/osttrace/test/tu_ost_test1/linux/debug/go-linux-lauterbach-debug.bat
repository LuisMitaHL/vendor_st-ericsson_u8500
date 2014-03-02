@CALL setenv.bat
@IF NOT "%ERRORLEVEL%" == "0" (
@GOTO fatal_error
)


@rem *******************************
@rem Start Launcher with trace configuration
@rem *******************************
%PERLEXEC% -S ../../../../../launcher/launcher.pl --debug_port T32:E04040005911 --mode=dbg --platform href_v1-linux --runpath "%MMROOT%/shared/osttrace/test/tu_ost_test1/linux/debug"
@IF NOT "%ERRORLEVEL%" == "0" (
@ECHO Laucher failed !!
@GOTO fatal_error
)

@GOTO eof


:fatal_error
@SET _EXITCODE=1
@pause


:eof

