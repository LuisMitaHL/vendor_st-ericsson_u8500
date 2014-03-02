@CALL setenv.bat
@IF NOT "%ERRORLEVEL%" == "0" (
@GOTO fatal_error
)

C:\T32\t32marm.exe -c ./config_arm.t32, trace.cmm

@GOTO eof


:fatal_error
@SET _EXITCODE=1
@pause


:eof


