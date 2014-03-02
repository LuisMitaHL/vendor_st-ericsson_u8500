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

@IF "%MYWEEK%" GEQ "w1040" (
	@IF "%COMPUTERNAME%" == "GNB300843" (
		@set T32SYS=C:\T32_W1040
		@set T32CONFIG=C:\T32_W1040\config.t32,
		@set PATH=%PATH%;C:\T32_W1040;
	) ELSE (
		@set T32SYS=C:\T32
		@set T32CONFIG=C:\T32\config.t32,
		@set PATH=%PATH%;C:\T32;
	)
)


rem %T32SYS%\t32marm.exe -c ./config_arm_usb.t32
%T32SYS%\t32rem.exe localhost port=20000 kill


@GOTO eof


:fatal_error
@SET _EXITCODE=1


:eof

