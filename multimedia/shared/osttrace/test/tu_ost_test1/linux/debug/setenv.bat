@rem *******************************
@rem Configure tools environement
@rem *******************************
@rem
@rem At this point MMROOT not defined, will be defined by perl script
@rem
@set TRACE_TOOLSET=D:\Tools\System-Trace-110-02
@set PERL5LIB=%MMROOT%/shared/launcher;%MMROOT%/shared/launcher/perllib;%MMROOT%/shared/osttools/ostdec


@rem ******************************************************************
@rem Check which week to select proper Lauterbach installation
@rem ******************************************************************
@For /F "usebackq" %%i IN (`perl -e "use Cwd; my $winroot = getcwd(); $winroot =~ /(.+)\/multimedia\/(.+)/; my $week = $1; $week =~/(w\d\d\d\d)/; print $1"`) DO @set MYWEEK=%%i
@echo MYWEEK=%MYWEEK%

@set T32SYS=C:\T32
@set T32CONFIG=C:\T32\config.t32,
@set PATH=%PATH%;C:\T32;


@rem *******************************
@rem Insure that perl is in in the path
@rem *******************************
@rem
@IF "%CLEARCASE_SERVER%" == "" (
	@echo Hum, perl not reachable through CLEARCASE_SERVER variable, try local perl...
	@perl -v > NUL
	@IF NOT "%ERRORLEVEL%" == "0" (
		@ECHO PERL not found 
		@GOTO fatal_error
	)
	@set PERLEXEC=perl
) ELSE (
	@set PERLEXEC="\\%CLEARCASE_SERVER%\perl\bin\perl"
)


@rem ******************************************************************
@rem Compute MMROOT variable in case no correctly set by build process
@rem ******************************************************************
@rem @For /F "usebackq" %%i IN (`%PERLEXEC% -e "use Cwd; print getcwd() =~ /(.+)\/multimedia\/(.+)/"`) DO @set MMROOT=%%i
@For /F "usebackq" %%i IN (`perl -e "use Cwd; my $winroot = getcwd(); $winroot =~ /(.+)\/multimedia\/(.+)/; print $1"`) DO @set MMROOT=%%i
@set MMROOT=%MMROOT%/multimedia
@echo MMROOT=%MMROOT%


@GOTO eof


:fatal_error
@SET _EXITCODE=1


:eof
@ECHO Environment configuration Sucessfull !!

