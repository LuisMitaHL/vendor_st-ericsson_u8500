@ECHO OFF

REM ============================================================
REM Generate_Symbian_Report
REM 
REM This batch file allow user to generate HTML file corresponding to his out file
REM 
REM The batch file takes 4 arguments
REM _ the path to the testlist : default a testlist will be generated
REM _ the path to the out files : mandatory
REM _ the type of platform : mandatory 
REM _ the release name
REM
REM ============================================================


ECHO.
ECHO. WARNING : make sure cygwin_setup have been launch once before, else errors will occurs
ECHO. Take care of your testlist
ECHO.

REM ============================================================
REM Set Path to used folder
REM ============================================================
 
PUSHD "%~dp0"
SETLOCAL
SET PERL5LIB=
SET DRIVE=//gnx5579.gnb.st.com/multimedia_shared$
SET MMTestDrive=%DRIVE%/common/TOOLS/Linux/internals/mmtest
SET PATH=%DRIVE%/common/TOOLS/win32/externals/Cygwin/bin;%MMTestDrive%;%PATH%

SET PERLLIB=%DRIVE%/common/TOOLS/Linux/internals/mmtest/perllibs

SET MMROOT="xxx"

REM ============================================================
REM PARSE COMMAND LINE OPTION
REM ============================================================

ECHO.
SET _ARG=
SET TESTLIST=
SET OUT_FOLDER=
SET PLATFORM=
SET TESTLIST_FOLDER=
SET WEEK=


:loop_parse_options

SET _ARG=%1
SHIFT

IF "%_ARG%" == "" GOTO end_of_parse_options

IF "%_ARG%" == "--help"              GOTO help
IF "%_ARG%" == "-h"                  GOTO help
IF "%_ARG%" == "/?"                  GOTO help
IF "%_ARG%" == "?"                   GOTO help

IF "%_ARG%" == "TESTLIST"            GOTO option_set_testlist
IF "%_ARG%" == "--testlist"          GOTO option_set_testlist
IF "%_ARG%" == "-t"                  GOTO option_set_testlist
                  
IF "%_ARG%" == "OUT_FOLDER"          GOTO option_set_out_folder
IF "%_ARG%" == "--out_folder"        GOTO option_set_out_folder
IF "%_ARG%" == "-o"                  GOTO option_set_out_folder

IF "%_ARG%" == "PLATFORM"            GOTO option_set_platform         
IF "%_ARG%" == "--platform"          GOTO option_set_platform         
IF "%_ARG%" == "-p"                  GOTO option_set_platform

IF "%_ARG%" == "WEEK"                GOTO option_set_week         
IF "%_ARG%" == "--week"              GOTO option_set_week        
IF "%_ARG%" == "-w"                  GOTO option_set_week

GOTO loop_parse_options
 
 
REM Define a macro
:option_set_testlist
SET TESTLIST=%1
SHIFT
GOTO loop_parse_options

:option_set_out_folder
SET OUT_FOLDER=%1
SHIFT
GOTO loop_parse_options

:option_set_platform
SET PLATFORM=%1
SHIFT
GOTO loop_parse_options

:option_set_week
SET WEEK=%1
SHIFT
GOTO loop_parse_options

 
REM ------------------------------------------------------------
REM HELP
REM ------------------------------------------------------------

:help

ECHO.
ECHO.
ECHO   Usage: Generate_Symbian_Report_GNB  TESTLIST path_to_your_test_list 
ECHO.                                      OUT_FOLDER path_to_your_out_folder 
ECHO.                                      PLATFORM name_of_your_platform
ECHO.                                      WEEK release tested
ECHO.
ECHO.
ECHO.  This script will generate the html file for Symbian user
ECHO.  HTML will be generated depending on your .out file and your testlist
ECHO.
ECHO.  OUT_FOLDER and PLATFORM arguments are mandatory 
ECHO.  If TESTLIST is empty, a default testlist will be generated, containing every .out find in the OUT_FOLDER
ECHO.  If HTML_FOLDER is empty, the report will be placed into the OUT_FOLDER
ECHO.
ECHO.  If one of the others arguments is empty, execution will be abord                                                           
ECHO.
ECHO. Options:
ECHO. -----------------
ECHO.
ECHO.   TESTLIST,    --testlist,   -t   : the whole path to the testlist you want to use
ECHO.       
ECHO.   OUT_FOLDER,  --out_folder, -o   : the whole path to the folder containing your .out files
ECHO.       
ECHO.   PLATFORM,    --platform,   -p   : the name of the platform you are working on
ECHO.
ECHO.   WEEK,        --week,       -w   : the name of release tested
ECHO.
ECHO.                --help,       -h   : display this screen       
ECHO.
GOTO eof                                                                     


:end_of_parse_options

SET _ARG=

REM ------------------------------------------------------------
REM Verify argument
REM ------------------------------------------------------------

IF "%OUT_FOLDER%"=="" (
  ECHO. Missing OUT_FOLDER to use
  ECHO.
  GOTO help
)

IF "%PLATFORM%"=="" (
  ECHO. Missing PLATFORM to use
  ECHO.
  GOTO help
)

IF "%TESTLIST%"=="" (
  ECHO. Missing TESLIST to use 
  ECHO. Default testlist will be created
  perl TestListGenerator.pl --path=%OUT_FOLDER% %OUT_FOLDER%/*.out
  SET TESTLIST=%OUT_FOLDER%\testlist.txt
  ECHO.
)
IF "%WEEK%"=="" (
  ECHO. Missing WEEK to use 
  ECHO. Default release name will be created
  SET WEEK="Wxxx"
  ECHO.
)
                                                                         
REM ============================================================
                      
:Generator

ECHO. Launch create_report_ite.pl -tag %WEEK%-%PLATFORM%-Symbian -path %OUT_FOLDER% -testlist %TESTLIST% -platform %PLATFORM%
perl create_report_ite.pl -tag %WEEK%-%PLATFORM%-Symbian -path %OUT_FOLDER% -testlist %TESTLIST% -platform %PLATFORM%
rem perl create_report_ite.pl -tag %WEEK% -path %OUT_FOLDER% -testlist %TESTLIST% -platform %PLATFORM% -verbose


ECHO. End of html file generation 
rem CD /d %CURRENT_FOLDER%

GOTO eof

REM ============================================================
REM FATAL ERROR
REM ============================================================

:fatal_error

ECHO. Execution aborted due to fatal error
SET _EXITCODE=1

GOTO eof

REM ============================================================
REM END OF SCRIPT
REM ============================================================

:eof
SET TESTLIST=
SET OUT_FOLDER=
SET PLATFORM=  
SET WEEK=  

POPD "%~dp0" 
 
EXIT /B 
