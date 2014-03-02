@CALL setenv.bat
@IF NOT "%ERRORLEVEL%" == "0" (
@GOTO fatal_error
)


@rem *******************************
@rem Start Launcher with hot attach configuration
@rem *******************************
%PERLEXEC% -S ../../../../../launcher/launcher.pl --debug_port T32:E04040005911 --mode=hot --platform href_v1-linux --runpath "%MMROOT%/shared/osttrace/test/tu_ost_test1/linux/debug"


@GOTO eof


:fatal_error
@SET _EXITCODE=1
@pause


:eof

