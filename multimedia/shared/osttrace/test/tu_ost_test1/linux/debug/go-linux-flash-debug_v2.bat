@CALL setenv.bat
@IF NOT "%ERRORLEVEL%" == "0" (
@GOTO fatal_error
)


%PERLEXEC% -S ../../../../../launcher/launcher.pl --debug_port T32:E04040005911 --mode=hot --platform u8500_v2-linux --runpath "%MMROOT%/shared/osttrace/unittest/linux/debug"


@GOTO eof


:fatal_error
@SET _EXITCODE=1
@pause


:eof

