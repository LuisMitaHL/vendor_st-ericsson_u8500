@echo off

setlocal EnableDelayedExpansion
setlocal EnableExtensions

rem -- Variables ----------------------------------------------------------------
rem

rem Script version
set version=0.0.16

rem directory where the script is placed
set scriptdir=%~dp0

rem directory which we run from
set rundir=%CD%

rem Tooldir relative to this script
set tooldir=%scriptdir%\flashkit

rem HW configuration list
set configlist=%scriptdir%\config.list

rem Flashlayout
set flashlayout=%scriptdir%\flashlayout.txt

rem Archive descriptor file
set archivedescr=%scriptdir%\flasharchive.xml

rem loader descriptor file
set loaderdescr=%tooldir%\configs\loader.xml

rem xlaunching descriptor file
set xlaucherdescr=%tooldir%\configs\ramload_normal.xml

rem BIP descriptor file
set bipdescr=%tooldir%\configs\bip.xml

rem bootimages prefix
set bootprefix=.\boot\

rem Modem images prefix
set modemprefix=.\modem_images\

rem loaders prefix
set loaderprefix=.\loaders\

rem Sign package to be used
set signpackage=U8500_STE_R1E
set signpackageauth=U8500_STE_AUTH_R1A

rem local signpackage and key path
set defsignpackegepath=%scriptdir%\signing\signpackages
set defkeypath=%scriptdir%\signing\signkeys

rem image sign config file
set sign_image_config_rnd=%scriptdir%\sw_type_map_no_signing.txt
set sign_image_config_commercial=%scriptdir%\sw_type_map.txt

rem Flasharchive dir
set flasharchive=%rundir%\flasharchive\flashimage.zip

rem Default flash images
set filelist_complete=%scriptdir%\_filelist_complete.txt

rem data for booting itp
set boot_itp_data=06000000

rem data for booting kernel
set boot_kernel_data=03000000

rem device for "extra" output
set device=NUL

rem path for temporary files (today created in users cwd)
set tempdir=!rundir!

rem State flags and other configuration populated in runtime
rem Target, from cmd line
set target_hw=

rem Images defined in configlist
set boot_image=
set dntcert=
set mem_init=
set power_management=
set loader=
set ipl=
set modem=
set xloader=
set tee=
set issw=
set crkc=
set stedntcert=
set loader_bip=
set loader_meminit=
set loader_prcmu=
set loader_issw=
set loader_xloader=
set archive_signed=1

rem Default sign tool is 8500 unless altered by config.list
set sign_tool=sign-tool-cli\sign-tool-u8500.bat

rem Default cspsa data
set default_cspsa=u8500_default_cspsa.bin

rem Cspsa path
set cspsa_path=/flash0/TOC/CSPSA0

rem Flag to check backend status
set backend_running=0

rem Flag to check loader created
set loader_created=0

rem Flag to check if bip is handled already
set biphandled=0

rem Remote signing
set remote_signing=0

rem Permanent authentication
set perm_auth=0

rem Sign as for commercial or RND unit
set commercial_signing=0

rem User has selected a signpackage so those variables hould not be overwritten by hw config
set /a userselectedsignpackage=0

rem variable used to hold the software type of the x-loader image used during the signing
set xloader_sw_type=NA

rem variable used to hold the software version of the x-loader image used during the signing
set xloader_sw_version=0

rem Name of UART port to be triggered
set trigger_uart_port=

rem User specified sw_type_map file
set user_sw_type_map=

rem Set if not U8xxx HW
set non_u8xxx_hw=0

rem backend debug enabled
set debug_enabled=0


rem -- All variables should be above this point --------------------------------------------------------
rem fake to have the start in the bottom of the script

rem parameters needs to be checked before the goto
  if [%1]==[] (
    echo %0: no options specified try "%~0 --help" to see available options
    exit /B 1
  )
goto start %*
goto :EOF

rem Developer Note:
rem all variables in command functions should be prefixed by the corresponding
rem command short-option character and in internal functions with a suitable
rem prefix derived from the function name

rem -- Command functions --------------------------------------------------------
rem
rem -h, --help
rem  Input     : N/A
rem  Output    : N/A
rem  Comment   : Print help text
rem
:print_help (
  echo Usage:
  echo flash-tool -command parameter1,parameter2,...parameterN  -command parameter
  echo.
  echo Commands:
  echo   -h
  echo   --help
  echo     Help screen
  echo.
  echo   -v
  echo   --verbose
  echo     Verbose operation
  echo.
  echo   -V
  echo   --version
  echo     Echo script version
  echo.
  echo   -t HW
  echo   --target-hw HW
  echo     Set target HW to flash. Must always be first command
  echo     Parameters: HW      HW as listed by -l
  echo.
  echo   -l
  echo   --list-targets
  echo     List available HW configurations
  echo     Parameters: None
  echo.
  echo   -i
  echo   --list-images
  echo     List images currently in flashlayout for target
  echo     Parameters: None
  echo.
  echo   -o
  echo   --list-on-target-images
  echo     List devices on ME
  echo     Parameters: None
  echo.
  echo   -C
  echo   --sign-for-commercial
  echo     Sign for commercial device. Default is RnD
  echo     Parameters: None
  echo.
  echo   -p SIGN_PACKAGE_NAME
  echo   --signpackage= SIGN_PACKAGE_NAME
  echo     Set Signpackage. Default is %signpackage%
  echo     Parameters: SIGN_PACKAGE_NAME
  echo.
  echo   -L
  echo   --sign-local
  echo     Use local signing. Default is local signing.
  echo     Parameters: None
  echo.
  echo   -R
  echo   --sign-remote
  echo     Use remote signing. Default is local signing.
  echo     Parameters: None
  echo.
  echo   -N
  echo   --no-signing-archive
  echo     Do not sign the flash archive. Default is that it's signed.
  echo     Note: To be able to flash an unsigned archive, loader support
  echo     is also required. As of today there is no such support in generic
  echo     U8500.
  echo     Parameters: None
  echo.
  echo   -B [ON \ OFF]
  echo   --boot-itp=[ON \ OFF]
  echo     Set BOOT_ITP flag ON or OFF.
  echo     Parameters: [ON \ OFF] Set boot from ITP ON or OFF
  echo.
  echo   -e [IMAGE][,IMAGE 2][,...IMAGE N]
  echo   --erase [IMAGE][,IMAGE 2][,...IMAGE N]
  echo     Erase flash
  echo     Parameters: IMAGE can be any valid flash area.
  echo                 Any number of areas can be given.
  echo                 If no area is given the entire flash, except CSPSA, will
  echo                 be erased.
  echo                 To erase CSPSA it must be given as a separate  parameter.
  echo                 To erase all including CSPSA in one go, run erase twice:
  echo                 ./flasher -e e/flash0/TOC/CSPSA0
  echo.
  echo   -E
  echo   --erase_no_emmc
  echo     Erase flash except the storage emmc partition
  echo     This will erase from 0x0 to 0xafffffff
  echo.
  echo   -w IMAGE=FILENAME[,IMAGE 2=FILENAME 2][,...IMAGE N=FILENAME N]
  echo   --write IMAGE=FILENAME[,IMAGE 2=FILENAME 2][,...IMAGE N=FILENAME N]
  echo     Write IMAGE to ME according to flashlayout.
  echo     Supported images listed by -i
  echo     Parameters: IMAGE     Image to write
  echo                 FILENAME  File to use as IMAGE
  echo.
  echo   -I IMAGE=FILENAME[,IMAGE 2=FILENAME 2][,...IMAGE N=FILENAME N]
  echo   --create-flasharchive IMAGE=FILENAME[,IMAGE 2=FILENAME 2][,...IMAGE N=FILENAME N]
  echo     Create flasharchive for ME according to flashlayout with supplied images.
  echo     Supported images listed by -i
  echo     Parameters: IMAGE     Image to write
  echo                 FILENAME  File to use as IMAGE
  echo.
  echo   -c
  echo   --write-complete
  echo     Write complete image to ME
  echo     Parameters: None
  echo.
  echo   -m
  echo   --write-complete-cspsa
  echo     Write complete image plus default cspsa to ME
  echo     Parameters: None
  echo.
  echo   -F
  echo   --create-complete-flasharchive
  echo     Create a complete flasharchive
  echo     Parameters: None
  echo.
  echo   -T
  echo   --create-complete-flasharchive-cspsa
  echo     Create a complete flasharchive with default cspsa data
  echo     Parameters: None
  echo.
  echo   -x ARCHIVE
  echo   --write-existing ARCHIVE
  echo     Write an existing archive to ME
  echo     Parameters: ARCHIVE   Flash archive to flash including optional path
  echo.
  echo   -d START,LENGTH[,OUTFILE]
  echo   --dump-data START,LENGTH[,OUTFILE]
  echo     Dump memory region.
  echo     Parameters: START     Start address, hex
  echo                 LENGTH    Amount of data, hex
  echo                 [OUTFILE] Optional, out file name. Default is dump.bin
  echo.
  echo   --set-enhanced-area FLASH_AREA,START,LENGTH
  echo     Set enhanced area region.
  echo     Parameters: FLASH_AREA     flash area path, string
  echo                 START          Start address, hex
  echo                 LENGTH         Amount of data, hex
  echo.
  echo   -g [OUTFILE]
  echo   --dump-gd [OUTFILE]
  echo     Dump GD area to ASCII file
  echo     Parameters: [OUTFILE] Optional, out file name. Default is dump.gdf
  echo.
  echo   -u UNIT,DATA
  echo   --write-gd-unit UNIT,DATA
  echo     Write GD parameter
  echo     Parameters: UNIT      GD unit, hex
  echo                 DATA      GD data, hex
  echo.
  echo   -U FILE
  echo   --write_gd_file FILE
  echo     Write multiple gd data from FILE
  echo     Parameters: FILE      GD ASCII data file
  echo.
  echo   -s [IMEI=imei,][SIMLOCK-CODES=codes,][COPS-DATA-FILE=datafile]
  echo   --write-sec-par [IMEI=imei,][SIMLOCK-CODES=codes,][COPS-DATA-FILE=datafile]
  echo     Write security parameters and bind properties.
  echo     Parameters:
  echo     imei     IMEI number, 15 characters
  echo     codes    SIM-lock codes. min 8, max 16 chars per key
  echo              All keys must be given, separeted by ':'
  echo              Network Lock Key:Network Subset Lock Key:Service Provider Lock
  echo              Key:Corporate Lock Key:Flexible ESL Lock Key
  echo     datafile File containing COPS data
  echo.
  echo   -P
  echo   --permanent-authentication
  echo     Use permanent authentication
  echo.
  echo   -f
  echo   --format_gd
  echo     Format gd area
  echo     Parameters: None
  echo.
  echo   -G [FILE]
  echo   --write-gd-image [FILE]
  echo     Write binary image [FILE] to gd area
  echo     Parameters: FILE      binary image to write
  echo.
  echo   -a DATA
  echo   --init-arb-table DATA
  echo     Initilize anti-roll back table
  echo     Parameters: DATA
  echo     Id of a mobile device, can be entered in decimal or hex format,
  echo     if it is in hex format then it has to begin with 0x
  echo.
  echo   -b [MODE]
  echo   --set-bootmode [MODE]
  echo     Wait for next connected ME and send indication to start in special mode
  echo     Parameters: MODE      adl, production or normal_debug
  echo.
  echo   -X [DEBUGENABLED] [FILE]
  echo   --xlaunch [DEBUGENABLED] [FILE]
  echo   Download bootimages plus file specified on command line to ram on next connected ME
  echo   Parameters: [DEBUGENABLED]   0 or 1
  echo               [FILE]           File to run as "normal"
  echo.
  echo   --trigger-uart-port PORT-NAME
  echo     Set name of UART port to be triggered. Must be specified after target.
  echo     Parameters: PORT-NAME      UART port name
  echo.
  echo   --debug
  echo     Enable backend debug log
  echo.
  echo   --dnt FILE
  echo     Write Debug and Test certificate to ME
  echo     Parameters: FILE      Certificate to write
  echo.
  echo   --write-otp FILE
  echo     Write OTP data to ME from input file
  echo     Parameters: FILE      path to and name of input file
  echo.
  echo   --read-otp FILE
  echo     Read OTP data from ME
  echo     Parameters: FILE      path to and name of output file
  echo.
  echo   --sw_type_map FILE
  echo     Path to sw_type_map file
  echo     Parameters: FILE      Path to sw_type_map file
  echo.
  echo   --write-rpmb
  echo     Write RPMB key
  echo.
  echo   --set-hardware-reset=<[ON | OFF]>
  echo     Set HW reset flag
  echo     Parameters: <[ON | OFF]> Set HW reset pin ON or OFF
  echo.
  echo   --set-unenhanced-force=<[ON | OFF]>
  echo     Set forced flashing of enhanced images to unenhanced storage
  echo     Parameters: <[ON | OFF]> Set ON or OFF
  exit /B 0
  goto :EOF
)


rem -v
rem --verbose
rem  Input     : N/A
rem  Output    : N/A
rem  Comment   : Verbose operation
rem
:set_verbose (
  echo.
  echo Set verbose
  set device=CON

  exit /B 0
  goto :EOF
)

rem --trigger-uart-port

:set_trigger_uart_port (
  set trigger_uart_port=%~1
  if NOT "!trigger_uart_port!" == "" (
    echo Setting UART port
  ) else (
    echo No UART port name given
  )

  exit /B 0
  goto :EOF
)

rem --dnt

:set_dnt (
  set dntcert=%~1
  if NOT "!dntcert!" == "" (
    set dntcert=!dntcert:/=\!
    for %%y in ("!dntcert!") do set dntcert=%%~dfy
    echo Setting dnt
  ) else (
    echo No dnt file given
  )

  exit /B 0
  goto :EOF
)

rem -V
rem --version
rem  Input     : N/A
rem  Output    : N/A
rem  Comment   : Print script version
rem
:print_version (
  echo flasher.bat %version%
  exit /B 0
  goto :EOF
)

rem -t<HW>
rem --target-hw=<HW>
rem  Input     : Target HW string as found in config.list
rem  Output    : Set boot images variables according to config.list
rem  Comment   :
rem
:set_target_hw (
  set target_hw=%1
  echo.
  echo Set target %target_hw%
  call :parse_configlist %target_hw%
  if %ERRORLEVEL% NEQ 0 (
    exit /B 1
  )
  echo boot image %boot_image% >>%device%
  echo issw %issw% >>%device%
  echo x-loader %xloader% >>%device%
  echo mem-init %mem_init% >>%device%
  echo power-management %power_management% >>%device%
  echo loader %loader% >>%device%
  echo ipl %ipl% >>%device%
  echo modem %modem% >>%device%
  echo loader boot image %loader_bip% >>%device%
  echo loader issw %loader_issw% >>%device%
  echo loader xloader %loader_xloader% >>%device%
  echo loader meminit %loader_meminit% >>%device%
  echo loader prcmu %loader_prcmu% >>%device%
  echo stedntcert %stedntcert% >>%device%
  echo dntcert %dntcert% >>%device%
  echo tee %tee% >>%device%
  echo sw_type_map %user_sw_type_map% >>%device%

  echo signpackage %signpackage% >>%device%
  echo authentication signpackage %signpackageauth% >>%device%

  rem Check if target hw is U5500
  set tmptarget=!target_hw:5500=tmp!
  if NOT !target_hw! == !tmptarget! (
    set non_u8xxx_hw=1
  )
  rem Check if target hw is U9500_100
  set tmptarget=!target_hw:9500=tmp!
  if NOT !target_hw! == !tmptarget! (
    set non_u8xxx_hw=1
  )
  rem Check if target hw is U9500_40
  set tmptarget=!target_hw:9540=tmp!
  if NOT !target_hw! == !tmptarget! (
    set non_u8xxx_hw=1
    set default_cspsa=u9540_default_cspsa.bin
  )
  rem Check if target hw is L8540
  set tmptarget=!target_hw:8540=tmp!
  if NOT !target_hw! == !tmptarget! (
    set non_u8xxx_hw=1
  )
  exit /B 0
  goto :EOF
)


rem -l
rem --list-targets
rem  Input     : N/A
rem  Output    : Echo HW as found in config.list
rem  Comment   :
rem
:list_targets (
  echo.
  echo Available target HW in %configlist%:

  for /f "usebackq eol=# tokens=1 delims== " %%a in ("!configlist!") do (
    rem echo %%a
    set a=%%a: =%
    rem echo %%a

    set lfirstchar=!a:~0,1!
    if "!lfirstchar!"=="[" (
      set lhwname=!a:~1,-4!
      echo !lhwname!
    )
  )
  set lfirstchar=
  set lhwname=

  exit /B 0
  goto :EOF
)


rem -i
rem --list-images
rem  Input     : N/A
rem  Output    : Echo images as found in flashlayout.txt
rem  Comment   :
rem
:list_images (
  echo.
  echo Available images in %flashlayout%:

  for /f "usebackq eol=# tokens=1,2 delims==: " %%a in ("%flashlayout%") do (
    rem echo %%a %%b

    if "%%a"=="Path" (
      echo %%b
    )
  )
  exit /B 0
  goto :EOF
)

:search_cspsa (
  set tmpcspsapath=%~1
  set tmpcspsapath2=%tmpcspsapath:CSPSA=%
  if NOT %tmpcspsapath%==%tmpcspsapath2% (
    set cspsa_path=%tmpcspsapath%
    set gcreturnvalue=0
  )
  exit /B 0
  goto :EOF
)
rem  Input     : N/A
rem  Output    : Path to CSPSA from flashlayout.txt
rem  Comment   :
rem
:get_cspsa (
  set gcreturnvalue=1
  for /f "usebackq eol=# tokens=1,2 delims==: " %%a in ("%flashlayout%") do (
    if "%%a"=="Path" (
      set gcstring="%%b"
      call :search_cspsa !gcstring!
    )
  )
  if !gcreturnvalue! NEQ 0 (
    echo "Warning: CSPSA path not found in %flashlayout%, default value will be used"
  )
  exit /B 0
  goto :EOF
)



rem -o
rem --list-on-target-images
rem  Input     : N/A
rem  Output    : Echo output from list_devices
:list_me_devices (
    call :common_init
    if %ERRORLEVEL% NEQ 0 (
        exit /B 1
    )
    call :startbackend
    if %ERRORLEVEL% NEQ 0 (
        exit /B 1
    )

    call "%tooldir%\flash-tool-cli\flash-tool.bat" list_devices
    set returnvalue=!ERRORLEVEL!

    if !returnvalue! NEQ 0 (
        echo ...list devices failed...
        call :stopbackend
        exit /B 1
    )

    exit /B 0
    goto :EOF
)

rem -C"
rem --sign-for-commercial"
rem  Input     : N/A
rem  Output    :
rem  Comment   :
rem
:set_commercial_signing (
  echo.
  echo set_commercial_signing
  set commercial_signing=1

  exit /B 0
  goto :EOF
)


rem -p<SIGN_PACKAGE_NAME>"
rem --signpackage=<SIGN_PACKAGE_NAME>"
rem  Input     : N/A
rem Output    :
rem  Comment   :
rem
:set_sign_package (
  echo.
  echo set_sign_package %1
  set signpackage=%1
  set signpackageauth=!signpackage!_AUTH
  set /a userselectedsignpackage=1

  echo signpackage %signpackage% >>%device%
  echo authentication signpackage %signpackageauth% >>%device%

  exit /B 0
  goto :EOF
)


rem -L"
rem --sign-local"
rem  Input     : N/A
rem  Output    :
rem  Comment   :
rem
:set_local_signing (
  echo.
  set remote_signing=0
  echo use local signing

  exit /B 0
  goto :EOF
)


rem -R"
rem --sign-remote"
rem  Input     : N/A
rem  Output    :
rem  Comment   :
rem
:set_remote_signing (
  echo.
  set remote_signing=1
  echo use remote signing

  exit /B 0
  goto :EOF
)


rem -N"
rem --no-signing-archive"
rem  Input     : N/A
rem  Output    :
rem  Comment   :
rem
:set_no_signing_archive (
  echo.
  set archive_signed=0
  echo Do not sign flash archive

  exit /B 0
  goto :EOF
)


rem -B<[ON | OFF]>"
rem -boot-itp=<[ON | OFF]>"
rem  Input     : N/A
rem  Output    :
rem  Comment   :
rem
:set_boot_itp_flag (
  echo.
  echo set_boot_itp_flag %1
  set Bdata=""

  if "%1" == "ON" (
    set Bdata=%boot_itp_data%
  ) else (
    set Bdata=%boot_kernel_data%
  )

  call :common_init
  IF ERRORLEVEL 1 (
    exit /B 1
  )
  call :startbackend
  IF ERRORLEVEL 1 (
    exit /B 1
  )

  call "%tooldir%\flash-tool-cli\flash-tool.bat" write_global_data_unit -storage_id !cspsa_path! -unit_id 0xfffffffc -unit_data !Bdata!
  set returnvalue=!ERRORLEVEL!

  if !returnvalue! NEQ 0 (
    echo ...write gd unit failed...
    call :stopbackend
    exit /B 1
  )

  set Bdata=
  exit /B 0
  goto :EOF
)

rem -e[<IMAGE>][,<IMAGE 2>][,...<IMAGE N>]
rem --erase=[<IMAGE>][,<IMAGE 2>][,...<IMAGE N>]
rem  Input     :
rem  Output    :
rem  Comment   :
rem
:erase (
  call :common_init
  IF ERRORLEVEL 1 (
    echo ... common init failed ...
    exit /B 1
  )

  call :startbackend
  IF ERRORLEVEL 1 (
      exit /B 1
  )

  if "%*" == "" (
    echo Erase all
    call "%tooldir%\flash-tool-cli\flash-tool.bat" erase_area -area_path /flash0 -offset 0x0 -length ALL
    set returnvalue=!ERRORLEVEL!

    if !returnvalue! NEQ 0 (
      echo ...erase failed...
      call :stopbackend
      exit /B 1
    )
  ) else (
    for %%a in (%*) do (
      set eentry=%%a

      echo Erase entry !eentry!

      rem Add trailing '/' to entry to also erase TOC-entry (otherwise only data is erased)
      call "%tooldir%\flash-tool-cli\flash-tool.bat" erase_area -area_path !eentry!/ -offset 0x0 -length ALL
      set returnvalue=!ERRORLEVEL!

      if !returnvalue! NEQ 0 (
        echo ...erase failed...
        call :stopbackend
        exit /B 1
      )
    )

    set eentry=
  )

  exit /B 0
  goto :EOF
)


rem -E
rem --erase_no_emmc
rem  Input     :
rem  Output    :
rem  Comment   :
rem
:erase_no_emmc (
  call :common_init
  IF ERRORLEVEL 1 (
    echo ... common init failed ...
    exit /B 1
  )

  call :startbackend
  IF ERRORLEVEL 1 (
      exit /B 1
  )

  echo Erase all but emmc user storage
  call "%tooldir%\flash-tool-cli\flash-tool.bat" erase_area -area_path /flash0 -offset 0x0 -length 0xafffffff
  set returnvalue=!ERRORLEVEL!

  if !returnvalue! NEQ 0 (
    echo ...erase failed...
    call :stopbackend
    exit /B 1
  )

  exit /B 0
  goto :EOF
)



rem -w<IMAGE>=<FILENAME>[,<IMAGE 2>=<FILENAME 2>][,...<IMAGE N>=<FILENAME N>]
rem --write=<IMAGE>=<FILENAME>[,<IMAGE 2>=<FILENAME 2>][,...<IMAGE N>=<FILENAME N>]
rem  Input     : Image and disk file name pairs
rem  Output    :
rem  Comment   :
rem
:write_images (
  echo.
  echo Write images to ME

  rem not so nice but charachter interpretion prevents testing on %*
  rem as intended, in practice this shouldn't matter as if %~1 is empty
  rem then %* must be empty as well
  if "%~1" == "" (
    echo no images specified
  )

  rem Create temp filelist
  set wtmpfilelist=!tempdir!\tmpimagefilelist
  type NUL >"!wtmpfilelist!"

  rem Always include layout
  echo flashlayout=!flashlayout! >>"!wtmpfilelist!"

  set /a wsameentry=0
  for %%a in (%*) do (
        rem echo params "%%a"

        if !wsameentry! EQU 0 (
            set wareapath=%%a
            set /a wsameentry=1
        ) else (
            for %%t in ("%%~a") do set wfilename=%%~ft
            set /a wsameentry=0

            rem echo wareapath !wareapath! wfilename !wfilename!
            rem now both parts of the information is present
            call :findsourceentry !wareapath! wsourcename
            rem echo !wareapath! !wsourcename!
            if "!wsourcename!" == "" (
                echo Partition !wareapath! doesn't exist. Please point to a correct partition.
                exit /B 1
            ) else (
                echo !wsourcename!=!wfilename! >> "!wtmpfilelist!"
            )
        )
  )
  set wsourcename=
  set wfilename=
  set wareapath=
  set wsameentry=

  echo Flashing images:
  type "!wtmpfilelist!"

  call :common_init
  IF ERRORLEVEL 1 (
    exit /B 1
  )

  rem  Create archive
  call :createarchive "!wtmpfilelist!" "%flasharchive%"
  set returnvalue=!ERRORLEVEL!

  del "!wtmpfilelist!"
  set wtmpfilelist=

  if !returnvalue! NEQ 0 (
    echo ...create archive failed...
    call :stopbackend
    exit /B 1
  )

  rem flash
  call :flash_file "%flasharchive%"
  IF ERRORLEVEL 1 (
    exit /B 1
  )

  exit /B 0
  goto :EOF
)


rem -I<IMAGE>=<FILENAME>[,<IMAGE 2>=<FILENAME 2>][,...<IMAGE N>=<FILENAME N>]
rem --create-flasharchive=<IMAGE>=<FILENAME>[,<IMAGE 2>=<FILENAME 2>][,...<IMAGE N>=<FILENAME N>]
rem  Input     : Image and disk file name pairs
rem  Output    :
rem  Comment   :
rem
:create_flasharchive
(
  echo.
  echo Create flasharchive

  rem not so nice but charachter interpretion prevents testing on %*
  rem as intended, in practice this shouldn't matter as if %~1 is empty
  rem then %* must be empty as well
  if "%~1" == "" (
    echo no images specified
  )

  rem Create temp filelist
  set Itmpfilelist=!tempdir!\tmpimagefilelist
  type NUL >"!Itmpfilelist!"

  rem Always include layout
  echo flashlayout=!flashlayout! >>"!Itmpfilelist!"

  set /a Isameentry=0
  for %%a in (%*) do (
        rem echo params "%%a"

        if !Isameentry! EQU 0 (
            set Iareapath=%%a
            set /a Isameentry=1
        ) else (
            for %%t in ("%%~a") do set Ifilename=%%~ft
            set /a Isameentry=0

            rem echo Iareapath !Iareapath! Ifilename !Ifilename!
            rem now both parts of the information is present
            call :findsourceentry !Iareapath! Isourcename
            rem echo !Iareapath! !Isourcename!

            echo !Isourcename!=!Ifilename! >> "!Itmpfilelist!"
        )
  )
  set Isourcename=
  set Ifilename=
  set Iareapath=
  set Isameentry=

  echo Including images:
  type "!Itmpfilelist!"

  call :common_init
  IF ERRORLEVEL 1 (
    exit /B 1
  )

  rem  Create archive
  call :createarchive "!Itmpfilelist!" "%flasharchive%"
  set returnvalue=!ERRORLEVEL!

  del "!Itmpfilelist!"
  set Itmpfilelist=

  if !returnvalue! NEQ 0 (
    echo ...create archive failed...
    call :stopbackend
    exit /B 1
  )

  exit /B 0
  goto :EOF
)


rem -c
rem --write-complete
rem  Input     : None
rem  Output    :
rem  Comment   :  Write all available images to ME
rem
:write_complete
(
  echo.
  echo [Write all available images to ME]
  echo.

  call :common_init
  IF ERRORLEVEL 1 (
    exit /B 1
  )

  set ctmpfile=!tempdir!\tmpfilelist
  type NUL >!ctmpfile!

  call :handlebip "!ctmpfile!"
  if !ERRORLEVEL! NEQ 0 (
    exit /B 1
  )

  if NOT "%dntcert%" == "" (
    echo dntcert=%dntcert% >>!ctmpfile!
  )
  if NOT "%mem_init%" == "" (
    echo mem_init=%mem_init% >>!ctmpfile!
  )
  if NOT "%power_management%" == "" (
    echo power_management=%power_management% >>!ctmpfile!
  )
  if NOT "%ipl%" == "" (
    echo ipl=%ipl% >>!ctmpfile!
  )
  if NOT "%modem%" == "" (
    echo modem=%modem% >>!ctmpfile!
  )

  rem parse the filelist and expand all relative path to absolute
  rem and write in temporary filelist
  for %%a in ("!filelist_complete!") do set cbasepath=%%~dpa

  pushd "!cbasepath!"
  set cbasepath=

  FOR /F "usebackq eol=# tokens=1,2 delims== " %%a in ("!filelist_complete!") do (
    set centryname=%%a
    set cfilename=%%b
    set cfilename=!cfilename:/=\!

    for %%y in (!cfilename!) do set cfilename=%%~dfy

    echo !centryname!=!cfilename! >>"!ctmpfile!"
  )

  popd

  rem add a dummy for cspsa
  set ccspsadummy=!tempdir!\tmpdummyfile
  type NUL >!ccspsadummy!
  echo cspsa.bin=!ccspsadummy! >>!ctmpfile!

  rem add dummies for ARB
  if !non_u8xxx_hw! EQU 0 (
    set carbdummy=!tempdir!\tmparbdummyfile
    type NUL >!carbdummy!
    echo arb1.bin=!carbdummy! >>!ctmpfile!
    echo arb2.bin=!carbdummy! >>!ctmpfile!
  )

  echo.
  echo Files to flash. From "%configlist%" and "%filelist_complete%"  >>%device%
  type "!ctmpfile!"  >>%device%

  rem  Create archive
  call :createarchive "!ctmpfile!" "%flasharchive%"
  set returnvalue=!ERRORLEVEL!

  del "!ctmpfile!"
  set ctmpfile=
  del "!ccspsadummy!"
  set ccspsadummy=

  if !non_u8xxx_hw! EQU 0 (
    del "!carbdummy!"
    set carbdummy=
  )

  if !returnvalue! NEQ 0 (
    echo ...create archive failed...
    call :stopbackend
    exit /B 1
  )

  rem flash
  call :flash_file "%flasharchive%"
  IF ERRORLEVEL 1 (
    exit /B 1
  )
  echo.
  echo [Done writing all available images to ME]
  echo.
  exit /B 0
  goto :EOF
)

rem -m
rem --write-complete-cspsa
rem  Input     : None
rem  Output    :
rem  Comment   :  Write all available images plus default cspsa data to ME
rem
:write_complete_cspsa
(
  echo.
  echo Write complete cspsa

  call :common_init
  IF ERRORLEVEL 1 (
    exit /B 1
  )

  set mtmpfile=!tempdir!\tmpfilelist
  type NUL >!mtmpfile!

  call :handlebip "!mtmpfile!"
  if !ERRORLEVEL! NEQ 0 (
    exit /B 1
  )

  if NOT "%dntcert%" == "" (
    echo dntcert=%dntcert% >>!mtmpfile!
  )
  if NOT "%mem_init%" == "" (
    echo mem_init=%mem_init% >>!mtmpfile!
  )
  if NOT "%power_management%" == "" (
    echo power_management=%power_management% >>!mtmpfile!
  )
  if NOT "%ipl%" == "" (
    echo ipl=%ipl% >>!mtmpfile!
  )
  if NOT "%modem%" == "" (
    echo modem=%modem% >>!mtmpfile!
  )

  rem parse the filelist and expand all relative path to absolute
  rem and write in temporary filelist
  for %%a in ("!filelist_complete!") do set mbasepath=%%~dpa

  pushd "!mbasepath!"
  set cbasepath=

  FOR /F "usebackq eol=# tokens=1,2 delims== " %%a in ("!filelist_complete!") do (
    set mentryname=%%a
    set mfilename=%%b
    set mfilename=!mfilename:/=\!

    for %%y in (!mfilename!) do set mfilename=%%~dfy

    echo !mentryname!=!mfilename! >>"!mtmpfile!"
  )

  popd

  rem add default cspsa data
  echo cspsa.bin=!default_cspsa! >>!mtmpfile!

  echo.
  echo Files to flash. From "%configlist%" and "%filelist_complete%"
  type "!mtmpfile!"

  rem  Create archive
  call :createarchive "!mtmpfile!" "%flasharchive%" "" "true"
  set returnvalue=!ERRORLEVEL!

  del "!mtmpfile!"
  set mtmpfile=

  if !returnvalue! NEQ 0 (
    echo ...create archive failed...
    call :stopbackend
    exit /B 1
  )

  rem flash
  call :flash_file "%flasharchive%"
  IF ERRORLEVEL 1 (
    exit /B 1
  )

  exit /B 0
  goto :EOF
)


rem -F
rem --create-complete-flasharchive
rem  Input     : None
rem  Output    :
rem  Comment   : Create a flasharchive with all available images
rem
:create_complete_flasharchive
(
  echo.
  echo Create complete flasharchive

  call :common_init
  IF ERRORLEVEL 1 (
    exit /B 1
  )

  set Ftmpfile=!tempdir!\tmpfilelist
  type NUL >!Ftmpfile!

  call :handlebip "!Ftmpfile!"
  if !ERRORLEVEL! NEQ 0 (
    exit /B 1
  )

  if NOT "%dntcert%" == "" (
    echo dntcert=%dntcert% >>!Ftmpfile!
  )
  if NOT "%mem_init%" == "" (
    echo mem_init=%mem_init% >>!Ftmpfile!
  )
  if NOT "%power_management%" == "" (
    echo power_management=%power_management% >>!Ftmpfile!
  )
  if NOT "%ipl%" == "" (
    echo ipl=%ipl% >>!Ftmpfile!
  )
  if NOT "%modem%" == "" (
    echo modem=%modem% >>!Ftmpfile!
  )

  rem parse the filelist and expand all relative path to absolute
  rem and write in temporary filelist
  for %%a in ("!filelist_complete!") do set Fbasepath=%%~dpa

  pushd "!Fbasepath!"
  set cbasepath=

  FOR /F "usebackq eol=# tokens=1,2 delims== " %%a in ("!filelist_complete!") do (
    set Fentryname=%%a
    set Ffilename=%%b
    set Ffilename=!Ffilename:/=\!

    for %%y in (!Ffilename!) do set Ffilename=%%~dfy

    echo !Fentryname!=!Ffilename! >>"!Ftmpfile!"
  )

  popd

  rem add a dummy for cspsa
  set Fcspsadummy=!tempdir!\tmpdummyfile
  type NUL >!Fcspsadummy!
  echo cspsa.bin=!Fcspsadummy! >>!Ftmpfile!

  rem add dummies for ARB
  if !non_u8xxx_hw! EQU 0 (
    set Farbdummy=!tempdir!\tmparbdummyfile
    type NUL >!Farbdummy!
    echo arb1.bin=!Farbdummy! >>!Ftmpfile!
    echo arb2.bin=!Farbdummy! >>!Ftmpfile!
  )

  echo.
  echo Files to include. From "%configlist%" and "%filelist_complete%"
  type "!Ftmpfile!"
  rem  Create archive
  call :createarchive "!Ftmpfile!" "%flasharchive%"
  set returnvalue=!ERRORLEVEL!
  del "!Ftmpfile!"
  set Ftmpfile=
  del "!Fcspsadummy!"
  set Fcspsadummy=

  if !non_u8xxx_hw! EQU 0 (
    del "!Farbdummy!"
    set Farbdummy=
  )

  if !returnvalue! NEQ 0 (
    echo ...create archive failed...
    call :stopbackend
    exit /B 1
  )

  exit /B 0
  goto :EOF
)


rem -T
rem --create-complete-flasharchive-cspsa
rem  Input     : None
rem  Output    :
rem  Comment   : Create a flasharchive with all available images plus default cspsa data
rem
:create_complete_flasharchive_cspsa
(
  echo.
  echo Create complete flasharchive with default cspsa


  call :common_init
  IF ERRORLEVEL 1 (
    exit /B 1
  )

  set Ttmpfile=!tempdir!\tmpfilelist
  type NUL >!Ttmpfile!

  call :handlebip "!Ttmpfile!"
  if !ERRORLEVEL! NEQ 0 (
    exit /B 1
  )

  if NOT "%dntcert%" == "" (
    echo dntcert=%dntcert% >>!Ttmpfile!
  )
  if NOT "%mem_init%" == "" (
    echo mem_init=%mem_init% >>!Ttmpfile!
  )
  if NOT "%power_management%" == "" (
    echo power_management=%power_management% >>!Ttmpfile!
  )
  if NOT "%ipl%" == "" (
    echo ipl=%ipl% >>!Ttmpfile!
  )
  if NOT "%modem%" == "" (
    echo modem=%modem% >>!Ttmpfile!
  )

  rem parse the filelist and expand all relative path to absolute
  rem and write in temporary filelist
  for %%a in ("!filelist_complete!") do set Tbasepath=%%~dpa

  pushd "!Tbasepath!"
  set cbasepath=



  FOR /F "usebackq eol=# tokens=1,2 delims== " %%a in ("!filelist_complete!") do (
    set Tentryname=%%a
    set Tfilename=%%b
    set Tfilename=!Tfilename:/=\!

    for %%y in (!Tfilename!) do set Tfilename=%%~dfy

    echo !Tentryname!=!Tfilename! >>"!Ttmpfile!"
  )

  popd

  rem add default cspsa data
  echo cspsa.bin=!default_cspsa! >>!Ttmpfile!

  echo.
  echo Files to include. From "%configlist%" and "%filelist_complete%"
  type "!Ttmpfile!"

  rem  Create archive
  call :createarchive "!Ttmpfile!" "%flasharchive%" "" "true"
  set returnvalue=!ERRORLEVEL!

  del "!Ttmpfile!"
  set Ttmpfile=


  if !returnvalue! NEQ 0 (
    echo ...create archive failed...
    call :stopbackend
    exit /B 1
  )

  exit /B 0
  goto :EOF
)


rem -x
rem --write-existing
rem  Input     :
rem  Output    :
rem  Comment   :
rem
:write_existing
(
  echo.
  echo write existing "%1"
  for %%a in ("%~1") do set xfile=%%~fa

  call :common_init
  IF ERRORLEVEL 1 (
    exit /B 1
  )

  call :flash_file "!xfile!"
  IF ERRORLEVEL 1 (
    exit /B 1
  )

  set xfile=
  exit /B 0
  goto :EOF
)


rem -d<START>,<LENGTH>[,<OUTFILE>]
rem --dump-data=<START>,<LENGTH>[,<OUTFILE>]
rem  Input     :
rem  Output    :
rem  Comment   :
rem
:dump_data
(
  set ddumpfile=%rundir%\dump.bin
  set dstart=%1
  set dend=%2

  if NOT "%~3" == "" (
    for %%a in ("%~3") do set ddumpfile=%%~fa
  )

  echo Dump start !dstart! length !dend! to !ddumpfile!

  call :common_init
  IF ERRORLEVEL 1 (
    exit /B 1
  )
  call :startbackend
  IF ERRORLEVEL 1 (
    exit /B 1
  )

  call "%tooldir%\flash-tool-cli\flash-tool.bat" dump_area -area_path /flash0 -offset !dstart! -length !dend! -path "!ddumpfile!"
  set returnvalue=!ERRORLEVEL!

  set ddumpfile=
  set dstart=
  set dend=

  if !returnvalue! NEQ 0 (
    echo ...dump failed...
    call :stopbackend
    exit /B 1
  )

  exit /B 0
  goto :EOF
)

rem --set-enhanced-area=<FLASH AREA>,<START>,<LENGTH>
rem  Input     :
rem  Output    :
rem  Comment   :
rem
:set_enhanced_area
(
  set sflasharea=%1
  set sstart=%2
  set ssize=%3

  echo set enhanced area in flash: !sflasharea! start: !sstart!  size: !ssize!

  call :common_init
  IF ERRORLEVEL 1 (
    exit /B 1
  )
  call :startbackend
  IF ERRORLEVEL 1 (
    exit /B 1
  )

  call "%tooldir%\flash-tool-cli\flash-tool.bat" set_enhanced_area -area_path !sflasharea! -offset !sstart! -length !ssize!
  set returnvalue=!ERRORLEVEL!

  if !returnvalue! NEQ 0 (
    echo ...set enhanced area failed...
    call :stopbackend
    exit /B 1
  )

  exit /B 0
  goto :EOF
)


rem -g[<OUTFILE>]
rem --dump-gd=[<OUTFILE>]
rem  Input     :
rem  Output    :
rem  Comment   :
rem
:dump_gd
(
  set ggddumpfile=%rundir%\dump.gdf

  if NOT "%~1" == "" (
    for %%a in ("%~1") do set ggddumpfile=%%~fa
  )

  echo Dump gd to !ggddumpfile!

  call :common_init
  IF ERRORLEVEL 1 (
    exit /B 1
  )
  call :startbackend
  IF ERRORLEVEL 1 (
    exit /B 1
  )

  call "%tooldir%\flash-tool-cli\flash-tool.bat" read_global_data_set -path "!ggddumpfile!" -storage_id !cspsa_path!
  set returnvalue=!ERRORLEVEL!

  set ggddumpfile=

  if !returnvalue! NEQ 0 (
    echo ...dump gd failed...
    call :stopbackend
    exit /B 1
  )

  exit /B 0
  goto :EOF
)

rem -u<UNIT>,<DATA>
rem --write-gd-unit=<UNIT>,<DATA>
rem  Input     :
rem  Output    :
rem  Comment   :
rem
:write_gd_unit
(
  set uunit=%~1
  set udata=%~2

  if "!uunit!" == "" (
    echo No unit specified
    exit /B 1
  )

  if "!udata!" == "" (
    echo No data specified
    exit /B 1
  )

  call :common_init
  IF ERRORLEVEL 1 (
    exit /B 1
  )
  call :startbackend
  IF ERRORLEVEL 1 (
    exit /B 1
  )

  call "%tooldir%\flash-tool-cli\flash-tool.bat" write_global_data_unit -storage_id !cspsa_path! -unit_id !uunit! -unit_data !udata!
  set returnvalue=!ERRORLEVEL!

  set uunit=
  set udata=

  if !returnvalue! NEQ 0 (
    echo ...write gd unit failed...

    call :stopbackend
    exit /B 1
  )

  exit /B 0
  goto :EOF
)


rem -U<FILE>
rem --write_gd_file=<FILE>
rem  Input     :
rem  Output    :
rem  Comment   :
rem
:write_gd_file (
  set Ugdfile=
  if "%~1" == "" (
    echo No file specified
    exit /B 1
  ) else (
    for %%a in ("%~1") do set Ugdfile=%%~fa
  )
  call :common_init
  IF ERRORLEVEL 1 (
    exit /B 1
  )
  call :startbackend
  IF ERRORLEVEL 1 (
    exit /B 1
  )

  call "%tooldir%\flash-tool-cli\flash-tool.bat" write_global_data_set -path "!Ugdfile!" -storage_id !cspsa_path!
  set returnvalue=!ERRORLEVEL!

  set Ugdfile=

  if !returnvalue! NEQ 0 (
    echo ...write gd unit failed...
    call :stopbackend
    exit /B 1
  )

  exit /B 0
  goto :EOF
)


rem -s<IMEI=imei>,<SIMLOCK-CODES=simlockcodes>,<COPS-DATA-FILE=copsdatafile>
rem --write-sec-par=<IMEI=imei>,<SIMLOCK-CODES=simlockcodes>,<COPS-DATA-FILE=copsdatafile>
rem  Input     :
rem  Output    :
rem  Comment   :
rem
:write_sec_par (

  call :common_init
  IF ERRORLEVEL 1 (
      exit /B 1
  )

  call :startbackend
     IF ERRORLEVEL 1 (
         exit /B 1
  )

  :secparloop (
    set skey=%1
    set svalue=%~2

    if "!skey!" == "IMEI" (
      echo Setting IMEI: !svalue!
      call "%tooldir%\flash-tool-cli\flash-tool.bat" set_equipment_property -equipment_property 0x01=!svalue!
      set returnvalue=!ERRORLEVEL!
      goto seccontinue
    )

    if "!skey!" == "SIMLOCK-CODES" (
      for /f "tokens=1,2,3,4,5 delims=:" %%a in ("%svalue%") do (
        set snlk=%%a
        set snsl=%%b
        set sspl=%%c
        set sclk=%%d
        set sfel=%%e
      )

      if "!snlk!" == "" (
        echo Network Lock Key not set, aborting
        set returnvalue=1
      )
      if "!snsl!" == "" (
        echo Network Subset Lock Key not set, aborting
        set returnvalue=1
      )
      if "!sspl!" == "" (
        echo Service Provider Lock Key not set, aborting
        set returnvalue=1
      )
      if "!sclk!" == "" (
        echo Corporate Lock Key not set, aborting
        set returnvalue=1
      )
      if "!sfel!" == "" (
        echo Flexible ESL Lock Key not set, aborting
        set returnvalue=1
      )

      if !returnvalue! NEQ 0 (
        goto seccontinue
      )

      echo Setting SimLock Keys "{Network Lock Key=!snlk!,Network Subset Lock Key=!snsl!,Service Provider Lock Key=!sspl!,Corporate Lock Key=!sclk!,Flexible ESL Lock Key=!sfel!}"
      call "%tooldir%\flash-tool-cli\flash-tool.bat" set_equipment_property -equipment_property 0x10="{Network Lock Key=!snlk!,Network Subset Lock Key=!snsl!,Service Provider Lock Key=!sspl!,Corporate Lock Key=!sclk!,Flexible ESL Lock Key=!sfel!}"
      set returnvalue=!ERRORLEVEL!

      set snlk=
      set snsl=
      set sspl=
      set sclk=
      set sfel=

      goto seccontinue
    )

    if "!skey!" == "COPS-DATA-FILE" (
      for %%a in ("!svalue!") do set sfile=%%~fa
      if EXIST !sfile! (
        echo Setting cops data using file !sfile!
        call "%tooldir%\flash-tool-cli\flash-tool.bat" set_equipment_property -equipment_property 0x100="!sfile!"
        set returnvalue=!ERRORLEVEL!
        goto seccontinue
      ) else (
        echo specified cops-data-file does not exist!
        set returnvalue=1
      )

    )

    set returnvalue=1

    :seccontinue

    if !returnvalue! NEQ 0 (
      echo ..write_sec_par failed...
      call :stopbackend
      exit /B 1
    )

    shift
    shift
    if NOT "%1" == "" goto secparloop
  )

  set skey=
  set svalue=

  call :authentication
  if !ERRORLEVEL! NEQ 0 (
    echo ... write_sec_par: authentication failed! ...
    call :stopbackend
    exit /B 1
  )

  call "%tooldir%\flash-tool-cli\flash-tool.bat" bind_properties
  set returnvalue=!ERRORLEVEL!
  if !returnvalue! NEQ 0 (
    echo ..bind properties failed...
    call :stopbackend
    exit /B 1
  )

  exit /B 0
  goto :EOF
)


:authentication (

  if !perm_auth! EQU 1 (
    call "%tooldir%\flash-tool-cli\flash-tool.bat" permanent_authentication
    set returnvalue=!ERRORLEVEL!
  ) else (
    call :checksignsetup
    IF ERRORLEVEL 1 (
      echo ... authentication: checksignsetup failed! ...
      exit /B 1
    )

    if !remote_signing! EQU 1 (
      call "%tooldir%\flash-tool-cli\flash-tool.bat" set_local_signing -local_signing false
      set returnvalue=!ERRORLEVEL!
    ) else (
      call "%tooldir%\flash-tool-cli\flash-tool.bat" set_local_signing -local_signing true
      set returnvalue=!ERRORLEVEL!
    )

    if !returnvalue! NEQ 0 (
      echo ..failed to setup local signing...
      exit /B 1
    )

    call "%tooldir%\flash-tool-cli\flash-tool.bat" authenticate_certificate -sign_package_name !signpackageauth!
    set returnvalue=!ERRORLEVEL!
  )

  if !returnvalue! NEQ 0 (
    echo ..authenticate failed...
    exit /B 1
  )

  exit /B 0
  goto :EOF
)

rem -P
rem --permanent-authentication
rem  Input     :
rem  Output    :
rem  Comment   :
rem
:set_permanent_authentication (
  echo.
  set perm_auth=1
  echo use permanent_authentication

  exit /B 0
  goto :EOF
)


rem -f
rem --format-gd
rem  Input     :
rem  Output    :
rem  Comment   :
rem
:format_gd (
  echo.
  echo format gd

  call :common_init
  IF ERRORLEVEL 1 (
    exit /B 1
  )
  call :startbackend
  IF ERRORLEVEL 1 (
    exit /B 1
  )

  call "%tooldir%\flash-tool-cli\flash-tool.bat" erase_global_data_set -storage_id !cspsa_path!
  set returnvalue=!ERRORLEVEL!

  if !returnvalue! NEQ 0 (
    echo ..format cspsa failed...
    call :stopbackend
    exit /B 1
  )

  exit /B 0
  goto :EOF
)

rem -G
rem --write-gd-image
rem  Input     : binary image to write
rem  Output    :
rem  Comment   :
rem
:write_gd_image (
  set Gfilename=

  if "%~1" == "" (
    echo No file specified
    exit /B 1
  ) else (
    for %%a in ("%~1") do set Gfilename=%%~fa
  )

  echo.
  echo write gd image "%Gfilename%"

  call :common_init
  IF ERRORLEVEL 1 (
    exit /B 1
  )

  rem this creates the needed xml for this case
  set Gflashgdxml=!tempdir!\flashgddescriptor.xml
  type NUL >"!Gflashgdxml!"

  echo ^<?xml version="1.0"?^>  >>"%Gflashgdxml%"
  echo ^<^^!DOCTYPE target_configuration [  >>"%Gflashgdxml%"
  echo ^<^^!ELEMENT target_configuration                    	(bax:flash_archive?, bax:ram_image?)^>  >>"%Gflashgdxml%"
  echo ^<^^!ELEMENT bax:source                              	(#PCDATA)^>  >>"%Gflashgdxml%"
  echo ^<^^!ELEMENT bax:flash_archive                       	(bax:entry*)^>  >>"%Gflashgdxml%"
  echo ^<^^!ELEMENT bax:ram_image								(bax:entry*)^>  >>"%Gflashgdxml%"
  echo ^<^^!ELEMENT bax:entry                               	(bax:source,bax:attribute*)^>  >>"%Gflashgdxml%"
  echo ^<^^!ELEMENT bax:attribute                         	(#PCDATA)^>  >>"%Gflashgdxml%"
  echo ^<^^!ATTLIST target_configuration      xmlns:bax       CDATA                                   	#FIXED "TBD"^>  >>"%Gflashgdxml%"
  echo ^<^^!ATTLIST bax:entry                 name      		CDATA                         				#REQUIRED^>  >>"%Gflashgdxml%"
  echo ^<^^!ATTLIST bax:entry                 type      		CDATA										#REQUIRED^>  >>"%Gflashgdxml%"
  echo ^<^^!ATTLIST bax:attribute             name      		CDATA                         				#REQUIRED^>  >>"%Gflashgdxml%"
  echo ]^>  >>"%Gflashgdxml%"
  echo ^<target_configuration^>  >>"%Gflashgdxml%"
  echo ^<bax:flash_archive^>  >>"%Gflashgdxml%"
  echo ^<bax:entry name="flashlayout.txt" type="x-empflash/flash-layout"^>  >>"%Gflashgdxml%"
  echo ^<bax:source^>flashlayout^</bax:source^>  >>"%Gflashgdxml%"
  echo ^</bax:entry^>  >>"%Gflashgdxml%"
  echo ^<bax:entry name="cspsa.bin" type="x-empflash/image"^>  >>"%Gflashgdxml%"
  echo ^<bax:source^>cspsa^</bax:source^>  >>"%Gflashgdxml%"
  echo ^<bax:attribute name="Target"^>!cspsa_path!, 0x0 ^</bax:attribute^>  >>"%Gflashgdxml%"
  echo ^<bax:attribute name="TOC-ID"^>CSPSA0^</bax:attribute^>  >>"%Gflashgdxml%"
  echo ^</bax:entry^>  >>"%Gflashgdxml%"
  echo ^</bax:flash_archive^>  >>"%Gflashgdxml%"
  echo ^</target_configuration^>  >>"%Gflashgdxml%"

  rem Create temp filelist
  set Gtmpfile=!tempdir!\tmpgdfilelist.txt
  type NUL >"!Gtmpfile!"

  rem Always include layout
  echo flashlayout=!flashlayout! >> "!Gtmpfile!"
  echo cspsa=!Gfilename! >> "!Gtmpfile!"
  set Gfilename=

  rem  Create archive
  call :createarchive "!Gtmpfile!" "%flasharchive%" "%Gflashgdxml%"
  set returnvalue=!ERRORLEVEL!

  del "!Gtmpfile!"
  set Gtmpfile=

  del "%Gflashgdxml%"
  set Gflashgdxml=

  if !returnvalue! NEQ 0 (
    echo ...create archive failed...
    call :stopbackend
    exit /B 1
  )

  rem flash
  call :flash_file "%flasharchive%"
  IF ERRORLEVEL 1 (
    exit /B 1
  )

  exit /B 0
  goto :EOF
)


rem -b<MODE>
rem --set-bootmode=<MODE>
rem  Input     :
rem  Output    :
rem  Comment   :
rem
:set_special_bootmode (
  if "%1" == "" (
    echo No bootmode specified
    exit /B 1
  )

  set bmode=%1
  set bprofile=

  if "%bmode%" == "adl" (
    set bprofile=STE_DB8500_adlboot
  ) else (
    if "%bmode%" == "production" (
      set bprofile=STE_DB8500_productionboot
    ) else (
        if "%bmode%" == "normal_debug" (
          set bprofile=STE_DB8500_normalboot_bootdebug
        ) else (
          echo invalid parameter
          exit /B 1
        )
  )
  set bmode=
  call :startbackend !bprofile!
  IF ERRORLEVEL 1 (
    exit /B 1
  )

  set bprofile=

  exit /B 0
  goto :EOF
)

rem  -a DATA
rem  --init-arb-table DATA
rem  Initilize anti-roll back table
rem  Parameters: DATA
:arbtable (
  echo.
  echo Initilize anti-roll back table

  if "%1" == "" (
    echo No arbdata specified
    exit /B 1
  ) else (
    set arbdata=%1
  )
  call :common_init
  IF ERRORLEVEL 1 (
    exit /B 1
  )
  call :startbackend
  IF ERRORLEVEL 1 (
    exit /B 1
  )

  call :authentication
  if !ERRORLEVEL! NEQ 0 (
    echo ... arbtable: authentication failed! ...
    call :stopbackend
    exit /B 1
  )

  call "%tooldir%\flash-tool-cli\flash-tool.bat" init_arb_table -arb_data %arbdata%
  set returnvalue=!ERRORLEVEL!
  if !returnvalue! NEQ 0 (
    echo ...initilize anti-roll back table failed...
    call :stopbackend
    exit /B 1
  )

  exit /B 0
  goto :EOF
)


rem -X<MODE>,<FILE>
rem --set-bootmode=<MODE>
rem  Input     :
rem  Output    :
rem  Comment   :
rem
:xlauncher (
  if "%1" == "" (
    echo Debug mode not specified
    exit /B 1
  ) else (
    set Xdebugenabled=%1
  )

  if "%~2" == "" (
    echo File not specified
    exit /B 1
  ) else (
    for %%a in ("%~2") do set xfile=%%~fa
  )

  if !Xdebugenabled! EQU 0 (
    set xprofile=STE_DBX500_ramload_normal
  ) else (
    set xprofile=STE_DBX500_ramload_bootdebug
  )
  set Xdebugenabled=

  set xfilelist=!tempdir!\tmpxlauncherfilelist.txt
  type NUL >"!xfilelist!"

  call :checksignsetup
  if !ERRORLEVEL! NEQ 0 (
    echo ... xlauncher: checksignsetup failed! ...
    exit /B 1
  )

  call :handlebip "!xfilelist!"
  if !ERRORLEVEL! NEQ 0 (
    exit /B 1
  )

  if NOT "%dntcert%" == "" (
    echo dntcert=%dntcert% >>"!xfilelist!"
  )
  if NOT "%mem_init%" == "" (
    echo mem_init=%mem_init% >>"!xfilelist!"
  )
  if NOT "%power_management%" == "" (
    echo power_management=%power_management% >>"!xfilelist!"
  )
  echo normal=!xfile! >>"!xfilelist!"

  set xfile=

  call :sign_images "!xfilelist!"
  set returnvalue=!ERRORLEVEL!
  if !returnvalue! NEQ 0 (
    echo ... xlauncher: sign images failed! ...
    exit /B 1
    goto :EOF
  )
  call "%tooldir%\assemble-tool-cli\assemble-tool.bat" -f -v -t ram_image -c "%xlaucherdescr%" -l "!xfilelist!" "%rundir%/ramload_normal.ldr" >>%device%
  set returnvalue=!ERRORLEVEL!
  del "!xfilelist!"
  set xfilelist=

  if !returnvalue! NEQ 0 (
    echo ...create xlauncher image failed...
    exit /B 1
  )

  call :startbackend %xprofile%
  IF ERRORLEVEL 1 (
    exit /B 1
  )

  set xprofile=

  exit /B 0
  goto :EOF
)

rem -- Internal support functions -----------------------------------------------
rem
rem Developer notes: all internal functions assumes the paths are expanded
rem when the functions are called

rem find source entry for a areapath
rem finds the "sourcename" for an areapath
rem parameters:
rem areapath name
rem name of variable to store the result
:findsourceentry (
  set fseareapath=%1
  set fsevarname=%2

  set fseentrytoken=
  set fseinsideentry=0
  set fsetargetfound=0
  set fsesource=
  set fseresult=

  rem Read archivedescr to match path to a source
  for /f "usebackq eol=# tokens=2,3 delims=<>" %%a in ("!archivedescr!") do (
    rem echo a: %%a b: %%b

    rem find token indication entry start and end
    for /f "tokens=1" %%c in ("%%a") do set fseentrytoken=%%c

    rem echo entrytoken !fseentrytoken!

    rem check for start of entry
    if "!fseentrytoken!" == "bax:entry" (
      set fseinsideentry=1
    )
    rem check for end of entry
    if "!fseentrytoken!" == "/bax:entry" (
      set fseinsideentry=0
    )

    if !fseinsideentry! EQU 1 (
      set fsetargettoken=

      rem echo inside entry %%a %%b

      rem extract target token
      for /f "tokens=2" %%c in ("%%a") do set fsetargettoken=%%c

      rem echo targettoken !fsetargettoken!

      rem target token found
      if "!fsetargettoken!" == "name="Target"" (

      for /f "tokens=1" %%c in ("%%b") do set fsetargetname=%%c
        rem remove unwanted characters
        set fsetargetname=!fsetargetname:~0,-1!

        rem echo target found !fsetargetname!
        rem check input to the found target token
        if "!fsetargetname!" == "!fseareapath!" (
          rem echo correct target
          set fsetargetfound=1
        )
      )

      rem check if this is source
      if "%%a" == "bax:source" (
        rem echo sourcefound %%b
        set fsesource=%%b
      )

      rem only if correct target and the source is found add to the temp filelist
      if !fsetargetfound! EQU 1 (
        if NOT !fsesource! == "" (
          set fsetargetfound=0
          rem this add antry to temp filelist
          set fseresult=!fsesource!
        )
      )
    )
  )
  set fsetargetname=
  set fsetargettoken=
  set fsesource=
  set fsetargetfound=
  set fseinsideentry=
  set fseentrytoken=
  set fseareapath=

  set !fsevarname!=!fseresult!
  set fseresult=
  set fsevarname=

  exit /B 0
  goto :EOF
)

rem common checks and initializations
:common_init (
  rem Check if HW set
  if "%target_hw%" == "" (
    echo HW not selected
    echo The hardware must be selected before this operation using the -t option
    exit /B 1
  )

  rem Assemble loader
  call :createloader
  set returnvalue=!ERRORLEVEL!

  if !returnvalue! NEQ 0 (
    echo ... common_init: create loader failed! ...
    exit /B 1
    goto :EOF
  )

  exit /B 0
  goto :EOF
)

rem creates loader.ldr file
:createloader (
  if !loader_created! EQU 0 (
    set cltmpfile=!tempdir!\tmploaderfilelist.txt
    type NUL >!cltmpfile!

    call :checksignsetup
    if !ERRORLEVEL! NEQ 0 (
      echo ... createloader: checksignsetup failed! ...
      exit /B 1
    )

    call :handlebip "!cltmpfile!" true
    if !ERRORLEVEL! NEQ 0 (
      exit /B 1
    )
    if NOT "%dntcert%" == "" (
      echo dntcert=%dntcert% >>"!cltmpfile!"
    )
    if NOT "%loader_meminit%" == "" (
      echo mem_init=%loader_meminit% >>"!cltmpfile!"
    ) else (
      if NOT "%mem_init%" == "" (
        echo mem_init=%mem_init% >>"!cltmpfile!"
      )
    )
    if NOT "%loader_prcmu%" == "" (
      echo power_management=%loader_prcmu% >>"!cltmpfile!"
    ) else (
      if NOT "%power_management%" == "" (
        echo power_management=%power_management% >>"!cltmpfile!"
      )
    )
    if NOT "%loader%" == "" (
      echo loader=%loader% >>"!cltmpfile!"
    ) else (
      echo No loader specified!
    )
    if NOT "%tee%" == "" (
      echo tee=%tee% >>"!cltmpfile!"
    )

    call :sign_images "!cltmpfile!"
    set returnvalue=!ERRORLEVEL!

    if !returnvalue! NEQ 0 (
      echo ... create loader: sign images failed! ...
      exit /B 1
      goto :EOF
    )

    call "%tooldir%\assemble-tool-cli\assemble-tool.bat" -f -v -t ram_image -c "%loaderdescr%" -l "!cltmpfile!" "%rundir%/loader.ldr" >>%device%
    set returnvalue=!ERRORLEVEL!

    del "!cltmpfile!"
    set cltmpfile=

    if !returnvalue! NEQ 0 (
      echo ...create loader failed...
      exit /B 1
    )

    set loader_created=1
  )

  exit /B 0
  goto :EOF
)

rem creates a flasharchive according to the filelist passed to it
rem parameters
rem filelist
rem name of outputfile
rem (optionally) alternative xml file to use
rem (optionally) argument for flashing default cspsa data
:createarchive (
  set cafilelist=%~1
  set caoutfile=%~2
  set cinclude_default_cspsa=%~4

  set cadescriptor=%archivedescr%

  if NOT "%~3" == "" (
    for %%a in ("%~3") do set cadescriptor=%%~fa
  )

  echo Assemble archive

  call :checksignsetup
  if !ERRORLEVEL! NEQ 0 (
    echo ... create archive: checksignsetup failed! ...
    exit /B 1
  )
  call :sign_images "%cafilelist%"
  set returnvalue=!ERRORLEVEL!

  if !returnvalue! NEQ 0 (
    echo ... create archive: sign images failed! ...
    exit /B 1
    goto :EOF
  )

  for %%a in ("!caoutfile!") do set caoutbasepath=%%~dpa

  mkdir "!caoutbasepath!" >NUL 2>NUL

  if "%cinclude_default_cspsa%"=="true" (
        call "%tooldir%\assemble-tool-cli\assemble-tool.bat" -f -v -r -t flash_archive -c "%cadescriptor%" -l "!cafilelist!" "!caoutbasepath!\contents.zip">> %device%
        set returnvalue=!ERRORLEVEL!
  ) else (
        call "%tooldir%\assemble-tool-cli\assemble-tool.bat" -f -v -t flash_archive -c "%cadescriptor%" -l "!cafilelist!" "!caoutbasepath!\contents.zip">> %device%
        set returnvalue=!ERRORLEVEL!
  )

  set cadescriptor=
  set cafilelist=

  if !returnvalue! NEQ 1 (
        if !archive_signed! NEQ 0 (
            echo Signing flash archive..

            if !remote_signing! EQU 1 (
                call "%tooldir%\%sign_tool%" -b 1047040 -v -p !signpackage! -s flash_archive "!caoutbasepath!\contents.zip" "%caoutfile%"  >>%device%
                set returnvalue=!ERRORLEVEL!
            ) else (
                call "%tooldir%\%sign_tool%" --local -b 1047040 -v -p !signpackage! -s flash_archive "!caoutbasepath!\contents.zip" "%caoutfile%"  >>%device%
                set returnvalue=!ERRORLEVEL!
            )
            echo Done signing flash archive.

            del "!caoutbasepath!\contents.zip"


        ) else (
            if EXIST "!caoutbasepath!\flashimage.zip" (
               del "!caoutbasepath!\flashimage.zip" 
            )
            rename "!caoutbasepath!\contents.zip" "flashimage.zip"
            echo Not signing flash archive.
        )
  )

  set caoutbasepath=
  set caoutfile=

  if !returnvalue! NEQ 1 (
      exit /B 0
  ) else (
      exit /B 1
  )

  goto :EOF
)

rem sign images according to configuration and if it should do "commercial"
rem or "rnd" signing.
rem this overwrites the filelist passed in with a new one conatining the
rem the resulting files.
:sign_images (
  set sifilelist=%~1
  set siconfigfile=%sign_image_config_rnd%
  set sisignedfilelist=signedfilelist.txt

  echo Signing images with sign package, [%signpackage%] ..

  if "!user_sw_type_map!" == "" (
    if !commercial_signing! EQU 1 (
      set siconfigfile=%sign_image_config_commercial%
    )
  ) else (
    set siconfigfile=%user_sw_type_map%
  )

  type NUL >"%sisignedfilelist%"


  FOR /F "usebackq eol=# tokens=1,2 delims==" %%a in ("%sifilelist%") do (
    set sifiletype=%%a
    set sifilename=%%~b
    set sisignentry=""
    set sisw_type=""
    set sifilename_post=!sifilename!

    FOR /F "usebackq eol=# tokens=1,2,3 delims= " %%c in ("%siconfigfile%") do (
      set sisignentry=%%c
      set sisw_type=%%d
      if "%%e" == "" (
        set sisw_version=0
      ) else (
        set sisw_version=%%e
      )

      if "!sifiletype!" == "!sisignentry!" (
        if NOT "!sisw_type!" == "" (
          if NOT "!sisw_type!" == "NA" (

            for %%c in ("!sifilename!") do set sifilebasename=%%~nxc

            set sifilename_post=!rundir!\!sifilebasename!_signed
            set sifilebasename=

            echo signing !sifilename! version !sisw_version! as type '!sisw_type!' to !sifilename_post! >>%device%

            if !remote_signing! EQU 1 (
              call "%tooldir%\%sign_tool%" -b 1047040 -v -p !signpackage! -s !sisw_type! -sv !sisw_version! "!sifilename!" "!sifilename_post!" >>%device%
              set returnvalue=!ERRORLEVEL!
            ) else (
              call "%tooldir%\%sign_tool%" --local -b 1047040 -v -p !signpackage! -s !sisw_type! -sv !sisw_version! "!sifilename!" "!sifilename_post!" >>%device%
              set returnvalue=!ERRORLEVEL!
            )

            if !returnvalue! NEQ 0 (
              exit /B 1
            )
          )
        )
      )
    )

    echo !sifiletype!=!sifilename_post! >>"%sisignedfilelist%"
  )

  move /Y "%sisignedfilelist%" "%sifilelist%"

  set sifilelist=
  set siconfigfile=
  set sisignedfilelist=
  set sifiletype=
  set sifilename=
  set sisignentry=
  set sisw_type=
  set sifilename_post=

  echo Done signing images.
  exit /B 0
  goto :EOF
)

rem handles bips, xloader and issw in the filelist
rem this can handle either a combined bip or a seperate issw and xloader
rem and adds it to the filelist
:handlebip (
  set hbfilelistname=%~1
  set hbforloader=%2
  set hbtmpbip=
  set hbtmpissw=
  set hbtmpxloader=

  if "%hbforloader%"=="true" (
    set hbtmpbip=%boot_image%
    set hbtmpissw=%issw%
    set hbtmpxloader=%xloader%


    set hbtmpbip=%loader_bip%
    set hbtmpissw=%loader_issw%
    set hbtmpxloader=%loader_xloader%

    rem if can be specified a loader issw and a loader xloader but
    rem an empty loader_bip
    if NOT "%loader_issw%" == "" (
      if NOT "%loader_xloader%" == "" (
        set hbtmpissw=%loader_issw%
        set hbtmpxloader=%loader_xloader%
        set hbtmpbip=%loader_bip%
      )
    ) else (
      rem it can also be a loader_bip with empty loader_issw and
      rem loader_xloader
      if NOT "%loader_bip%" == "" (
        set hbtmpissw=%loader_issw%
        set hbtmpxloader=%loader_xloader%
        set hbtmpbip=%loader_bip%
      ) else (
        rem but if all three are empty just use the other variables
        set hbtmpbip=%boot_image%
        set hbtmpissw=%issw%
        set hbtmpxloader=%xloader%
      )
    )

    if !loader_biphandled! EQU 1 (
      echo boot_image=%loader_bip% >>"%hbfilelistname%"

      set hbfilelistname=
      set hbforloader=
      set hbtmpbip=
      set hbtmpissw=
      set hbtmpxloader=
      exit /B 0
      goto :EOF
    )
  ) else (
    set hbtmpbip=%boot_image%
    set hbtmpissw=%issw%
    set hbtmpxloader=%xloader%

    if !biphandled! EQU 1 (
      echo boot_image=%boot_image% >>"%hbfilelistname%"

      set hbfilelistname=
      set hbforloader=
      set hbtmpbip=
      set hbtmpissw=
      set hbtmpxloader=
      exit /B 0
      goto :EOF
    )
  )

  if NOT "!hbtmpissw!" == "" (
    if NOT "!hbtmpxloader!" == "" (
      call :assemblebip hbtmpbip "!hbtmpissw!" "!hbtmpxloader!"
      if !ERRORLEVEL! NEQ 0 (
        exit /B 1
      )
      rem if different bootimages specified for the loader then if only needs
      rem needs to be created once independent if this call was done in the
      rem "create loader" scenario or any other scenario
      rem this is checked before loader_bip is set by this function so
      rem it can be sure that loader_bip was empty to begin with
      if "%loader_bip%" == "" (
        if "%loader_issw%" == "" (
          if "%loader_xloader%" == "" (
            rem if this call was for loader then loader_biphandled should be one
            rem so the "not-loader" variabels can be set as well
            if "%hbforloader%"=="true" (
              set boot_image=!hbtmpbip!
              set biphandled=1
            ) else (
              set loader_bip=!hbtmpbip!
              set loader_biphandled=1
            )
          )
        )
      )

      if "%hbforloader%"=="true" (
        set loader_bip=!hbtmpbip!
        set loader_biphandled=1
      ) else (
        set boot_image=!hbtmpbip!
        set biphandled=1
      )
    ) else (
      echo ...seperate issw or x-loader provided but not both...
      exit /B 1
    )
  ) else (
    if NOT "!hbtmpxloader!" == "" (
      echo ...seperate issw or x-loader provided but not both...
      exit /B 1
    )
  )

  echo boot_image=%hbtmpbip% >>"%hbfilelistname%"

  set hbfilelistname=
  set hbforloader=
  set hbtmpbip=
  set hbtmpissw=
  set hbtmpxloader=
  exit /B 0
  goto :EOF
)

:get_xloader_sw_type (
  set sw_type_list=%sign_image_config_rnd%
  if "!user_sw_type_map!" == "" (
    if !commercial_signing! EQU 1 (
      set sw_type_list=%sign_image_config_commercial%
    )
  ) else (
    set sw_type_list=%user_sw_type_map%
  )
  for /f "usebackq eol=# tokens=1,2,3 delims== " %%a in ("!sw_type_list!") do (
    if "x-loader" == "%%a" (
      set img_sw_type=%%b
      set img_sw_version=%%c
      goto exit_get_xloader_sw_type
    )
  )

:exit_get_xloader_sw_type
  set xloader_sw_type=!img_sw_type!
  if NOT "%img_sw_version%" == "" (
    set xloader_sw_version=%img_sw_version%
  )
goto :EOF
)

rem this assembles a bip and place the filename in variable passed in
:assemblebip (
  set abvarname=%1
  set abissw=%~2
  set abxloader=%~3


  call :checksignsetup
  if !ERRORLEVEL! NEQ 0 (
    echo ... assemblebip: checksignsetup failed! ...
    exit /B 1
  )

  set abassembledbipname=!rundir!\boot_image_assembled.bin

  for %%c in ("!xloader!") do set abxloaderbasename=%%~nxc

  set abxloadersigned=!rundir!\!abxloaderbasename!_signed
  set abxloaderbasename=

  call :get_xloader_sw_type
  if "!xloader_sw_type!" NEQ "NA" (
    echo signing x-loader !abxloader! version !xloader_sw_version! as !abxloadersigned! >>%device%

    if !remote_signing! EQU 1 (
    call "%tooldir%\%sign_tool%" -b 1047040 -v -p !signpackage! -s xloader -sv !xloader_sw_version! "!abxloader!" "!abxloadersigned!" >>%device%
      set returnvalue=!ERRORLEVEL!
    ) else (
    call "%tooldir%\%sign_tool%" --local -b 1047040 -v -p !signpackage! -s xloader -sv !xloader_sw_version! "!abxloader!" "!abxloadersigned!" >>%device%
      set returnvalue=!ERRORLEVEL!
    )

    if !returnvalue! NEQ 0 (
      echo ...signing of xloader failed...
      exit /B 1
    )
  ) else (
    echo X-LOADER will not be signed. Coping "!abxloader!" to "!abxloadersigned!"
    copy "!abxloader!" "!abxloadersigned!"
  )

  set abbipfilelist=tempbipfilelist.txt
  echo issw=!abissw! >"!abbipfilelist!"
  if NOT "%crkc%" == "" (
    echo crkc=!crkc! >> "!abbipfilelist!"
  )
  if NOT "%stedntcert%" == "" (
    echo stedntcert=!stedntcert! >> "!abbipfilelist!"
  )

  echo x-loader=!abxloadersigned! >>"!abbipfilelist!"

  call "%tooldir%\assemble-tool-cli\assemble-tool.bat" -f -v -t ram_image -c "%bipdescr%" -l "!abbipfilelist!" "!abassembledbipname!" >>%device%
  set returnvalue=!ERRORLEVEL!

  del "!abbipfilelist!"
  set hbbipfilelist=

  del "!abxloadersigned!"
  set abxloadersigned=

  set stedntcert=

  if !returnvalue! NEQ 0 (
    echo ...creation of bip failed...
    exit /B 1
  )

  set !abvarname!=!abassembledbipname!

  exit /B 0
  goto :EOF
)


rem flashes the file that is passed in as argument
:flash_file (
  set ffflasharchive=%~1

  call :startbackend
  echo Flashing ..
  IF ERRORLEVEL 1 (
    exit /B 1
  )

  call "%tooldir%\flash-tool-cli\flash-tool.bat" process_file -path "%ffflasharchive%"
  set returnvalue=!ERRORLEVEL!

  set ffflasharchive=

  if !returnvalue! NEQ 1 (
    call "%tooldir%\flash-tool-cli\flash-tool.bat" collect_data -type 2 >>%device%
    set returnvalue=!ERRORLEVEL!
  )

  if !returnvalue! NEQ 0 (
    echo ...flashing failed...
    call :stopbackend
    exit /B 1
  )

  exit /B 0
  goto :EOF
)

rem parses config list to find the hw specific components
rem fills the variables "loader" "ipl" "modem" "boot_image" "mem_init"
rem and "power_management" with the correct files for this hw
:parse_configlist (
  set pchw=%1
  set pcfound=0
  set pchwfound=0
  set pcbasepath=""

  for %%a in ("!configlist!") do set pcbasepath=%%~dpa

  for /f "usebackq eol=# tokens=1,* delims== " %%a in ("!configlist!") do (
    rem echo %%a %%b
    set a=%%a: =%
    set b=%%b: =%
    rem echo %%a %%b

    if "%%a"=="[%pchw%]" (
        set pcfound=1
        set pchwfound=1
    ) else (
        if !pcfound! EQU 1 (
            set pcfirstchar=!a:~0,1!

            if "!pcfirstchar!"=="[" (
                set pcfound=0
            )
            set pcfirstchar=
        )
    )

    pushd !pcbasepath!

    if !pcfound! EQU 1 (
      rem loader and bip must be recreated after this
      set loader_created=0
      set biphandled=0
      set pccfecounter=0

      if "%%a"=="loader" (
        set loader="%%b"
        if not exist !loader! (
          set loader=%loaderprefix%\%%b
        )
        set loader=!loader:/=\!

        for %%y in (!loader!) do (
          set /a pccfecounter+=1
          set loader=%%~dfy
        )
        call :check_file_existance !loader! %%a %%b
      )
      if "%%a"=="ipl" (
        if NOT "%%b"=="nonexistant" (
          set ipl="%%b"
          if not exist !ipl! (
            set ipl=%modemprefix%\%%b
          )
          set ipl=!ipl:/=\!

          for %%y in (!ipl!) do (
            set /a pccfecounter+=1
            set ipl=%%~dfy
          )
          call :check_file_existance !ipl! %%a %%b
        )
      )
      if "%%a"=="modem" (
        if NOT "%%b"=="nonexistant" (
          set modem="%%b"
          if not exist !modem! (
            set modem=%modemprefix%\%%b
          )
          set modem=!modem:/=\!

          for %%y in (!modem!) do (
            set /a pccfecounter+=1
            set modem=%%~dfy
          )
          call :check_file_existance !modem! %%a %%b
        )
      )
      if "%%a"=="boot_image" (
        set boot_image=%bootprefix%\%%b
        set boot_image=!boot_image:/=\!

        for %%y in (!boot_image!) do (
          set /a pccfecounter+=1
          set boot_image=%%~dfy
        )
        call :check_file_existance !boot_image! %%a %%b
      )
      if "%%a"=="mem_init" (
        set mem_init="%%b"
        if not exist !mem_init! (
          set mem_init=%bootprefix%\%%b
        )
        set mem_init=!mem_init:/=\!

        for %%y in (!mem_init!) do (
          set /a pccfecounter+=1
          set mem_init=%%~dfy
        )
        call :check_file_existance !mem_init! %%a %%b
      )
      if "%%a"=="power_management" (
        set power_management="%%b"
        if not exist !power_management! (
          set power_management=%bootprefix%\%%b
        )
        set power_management=!power_management:/=\!

        for %%y in (!power_management!) do (
          set /a pccfecounter+=1
          set power_management=%%~dfy
        )
        call :check_file_existance !power_management! %%a %%b
      )
      if "%%a"=="x-loader" (
        set xloader="%%b.bin"
        if not exist !xloader! (
          set xloader=%bootprefix%\%%b.bin
        )
        set xloader=!xloader:/=\!

        for %%y in (!xloader!) do (
          set /a pccfecounter+=1
          set xloader=%%~dfy
        )
        call :check_file_existance !xloader! %%a %%b
      )
      if "%%a"=="issw" (
        set issw="%%b"
        if not exist !issw! (
          set issw=%bootprefix%\%%b
        )
        set issw=!issw:/=\!

        for %%y in (!issw!) do (
          set /a pccfecounter+=1
          set issw=%%~dfy
        )
        call :check_file_existance !issw! %%a %%b
      )
      if "%%a"=="tee" (
        set tee="%%b"
        if not exist !tee! (
          set tee=%bootprefix%\%%b
        )
        set tee=!tee:/=\!

        for %%y in (!tee!) do (
          set /a pccfecounter+=1
          set tee=%%~dfy
        )
        call :check_file_existance !tee! %%a %%b
      )
      if "%%a"=="loader-bip" (
        set loader_bip="%%b"
        if not exist !loader_bip! (
          set loader_bip=%bootprefix%\%%b
        )
        set loader_bip=!loader_bip:/=\!

        for %%y in (!loader_bip!) do (
          set /a pccfecounter+=1
          set loader_bip=%%~dfy
        )
        call :check_file_existance !loader_bip! %%a %%b
      )
      if "%%a"=="loader-meminit" (
        set loader_meminit="%%b"
        if not exist !loader_meminit! (
          set loader_meminit=%bootprefix%\%%b
        )
        set loader_meminit=!loader_meminit:/=\!

        for %%y in (!loader_meminit!) do (
          set /a pccfecounter+=1
          set loader_meminit=%%~dfy
        )
        call :check_file_existance !loader_meminit! %%a %%b
      )
      if "%%a"=="loader-prcmu" (
        set loader_prcmu="%%b"
        if not exist !loader_prcmu! (
          set loader_prcmu=%bootprefix%\%%b
        )
        set loader_prcmu=!loader_prcmu:/=\!

        for %%y in (!loader_prcmu!) do (
          set /a pccfecounter+=1
          set loader_prcmu=%%~dfy
        )
        call :check_file_existance !loader_prcmu! %%a %%b
      )
      if "%%a"=="loader-issw" (
        set loader_issw="%%b"
        if not exist !loader_issw! (
          set loader_issw=%bootprefix%\%%b
        )
        set loader_issw=!loader_issw:/=\!

        for %%y in (!loader_issw!) do (
          set /a pccfecounter+=1
          set loader_issw=%%~dfy
        )
        call :check_file_existance !loader_issw! %%a %%b
      )
      if "%%a"=="loader-xloader" (
        set loader_xloader="%%b"
        if not exist !loader_xloader! (
          set loader_xloader=%bootprefix%\%%b
        )
        set loader_xloader=!loader_xloader:/=\!

        for %%y in (!loader_xloader!) do (
          set /a pccfecounter+=1
          set loader_xloader=%%~dfy
        )
        call :check_file_existance !loader_xloader! %%a %%b
      )
      if "%%a"=="rootsignpackage" (
        if !userselectedsignpackage! EQU 0 (
          set signpackage=%%b
        )
      )
      if "%%a"=="authsignpackage" (
        if !userselectedsignpackage! EQU 0 (
          set signpackageauth=%%b
        )
      )
      if "%%a"=="crkc" (
        set crkc=%bootprefix%\%%b
        set crkc=!crkc:/=\!
        for %%y in (!crkc!) do (
          set /a pccfecounter+=1
          set crkc=%%~dfy
        )
        call :check_file_existance !crkc! %%a %%b
      )
      if "%%a"=="stedntcert" (
        set stedntcert="%%b"
        if not exist !stedntcert! (
          set stedntcert=%bootprefix%\%%b
        )
        set stedntcert=!stedntcert:/=\!
        for %%y in (!stedntcert!) do (
          set /a pccfecounter+=1
          set stedntcert=%%~dfy
        )
        call :check_file_existance !stedntcert! %%a %%b
      )
      if "%%a"=="sw_type_map" (
        set user_sw_type_map="%%b"
        if not exist !user_sw_type_map! (
          set user_sw_type_map=%bootprefix%\%%b
        )
        set user_sw_type_map=!user_sw_type_map:/=\!
        for %%y in (!user_sw_type_map!) do (
          set /a pccfecounter+=1
          set user_sw_type_map=%%~dfy
        )
        call :check_file_existance !user_sw_type_map! %%a %%b
      )
      if "%%a"=="dntcert" (
        set dntcert="%%b"
        if not exist !dntcert! (
          set dntcert=%bootprefix%\%%b
        )
        set dntcert=!dntcert:/=\!
        for %%y in (!dntcert!) do (
          set /a pccfecounter+=1
          set dntcert=%%~dfy
        )
        call :check_file_existance !dntcert! %%a %%b
      )
      if "%%a"=="mark-hw" (
        if "%%b" == "U5500" (
          set sign_tool=sign-tool-cli\sign-tool-u5500.bat
        ) else (
          set sign_tool=sign-tool-cli\sign-tool-u8500.bat
        )
      )
      if "%%a"=="archive_signed" (
          set archive_signed=%%b
      )
    )
    popd
  )
  if !pchwfound! EQU 0 (
	echo "Hardware %pchw% is not supported"
	exit /B 1
  )
  set pchw=
  set pcfound=
  set pcbasepath=

  exit /B 0
  goto :EOF
)
rem function to check variables in config.list
:check_file_existance (
  set cfeconfigname=%2
  set cfefilename=%3
  set cfefilenamewithpath=%1

  if NOT DEFINED cfefilename (
    echo Warning: No file specified for !cfeconfigname! in the config.list file
    exit /B 2
  )
  if NOT EXIST "!cfefilenamewithpath!" (
    echo Warning:  Could not find file !cfefilenamewithpath! ,please check !cfeconfigname! in the config.list file
    exit /B 1
  )
  if "!pccfecounter!" GEQ "2" (
    echo Warning: you have more than one !cfeconfigname! file, !cfefilenamewithpath! will be used
  )
  set pccfecounter=0
  exit /B 0
  goto :EOF
)
rem --write-otp <FILE>
rem  Input     : Path and name to a otp file
rem  Output    :
rem  Comment   :
rem
:write_otp (
  if "%~1" == "" (
    echo No file specified
    exit /B 1
  ) else (
    set otpfile=%~1
  )

  call :common_init
  IF ERRORLEVEL 1 (
    exit /B 1
  )
  call :startbackend
  IF ERRORLEVEL 1 (
    exit /B 1
  )

  call :authentication
  IF ERRORLEVEL 1 (
    call :stopbackend
    exit /B 1
  )

  call "%tooldir%\flash-tool-cli\flash-tool.bat" burn_otp -path !otpfile!
  set returnvalue=!ERRORLEVEL!

  if !returnvalue! NEQ 0 (
    echo ...write otp data failed...
    call :stopbackend
    exit /B 1
  )

  exit /B 0
  goto :EOF
)


rem --read-otp <FILE>
rem  Input     : Path and name to a otp file
rem  Output    :
rem  Comment   :
rem
:read_otp (
  if "%~1" == "" (
    echo No file specified
    exit /B 1
  ) else (
    set otpfile=%~1
  )

  call :common_init
  IF ERRORLEVEL 1 (
    exit /B 1
  )
  call :startbackend
  IF ERRORLEVEL 1 (
    exit /B 1
  )

  call :authentication
  IF ERRORLEVEL 1 (
    call :stopbackend
    exit /B 1
  )

  call "%tooldir%\flash-tool-cli\flash-tool.bat" read_otp -path "!otpfile!"
  set returnvalue=!ERRORLEVEL!

  if !returnvalue! NEQ 0 (
    echo ...read otp data failed...
    call :stopbackend
    exit /B 1
  )

  exit /B 0
  goto :EOF
)

:set_sw_type_map (
  echo.
  if "%~1" == "" (
    echo No file specified
    exit /B 1
  ) else (
    echo sw_type_map file %~1 selected
    for %%a in ("%~1") do set user_sw_type_map=%%~fa
  )

  exit /B 0
  goto :EOF
)

rem --write-rpmb
rem  Input     :
rem  Output    :
rem  Comment   :
rem
:write_rpmb (
  call :common_init
  IF ERRORLEVEL 1 (
    echo ...write rpmb key failed...
    exit /B 1
  )
  call :startbackend
  IF ERRORLEVEL 1 (
    exit /B 1
  )

  call :authentication
  IF ERRORLEVEL 1 (
	echo ...write rpmb key failed...
	call :stopbackend
	exit /B 1
  )
  
  call "%tooldir%\flash-tool-cli\flash-tool.bat" write_rpmb_key -device_id %1 -commercial %2
  set returnvalue=!ERRORLEVEL!

  if !returnvalue! NEQ 0 (
    echo ...write rpmb key failed...
    call :stopbackend
    exit /B 1
  )

  exit /B 0
  goto :EOF
)

rem --set-unenhanced-force=<[ON | OFF]>"
rem  Input     : N/A
rem  Output    :
rem  Comment   :
rem
:set_unenhanced_force_flag (
  echo.
  echo set_unenhanced_force_flag %1
  set Bdata="1"

  if "%1" == "OFF" (
    set Bdata="0"
  )

  call :common_init
  IF ERRORLEVEL 1 (
    exit /B 1
  )
  call :startbackend
  IF ERRORLEVEL 1 (
    exit /B 1
  )

  call "%tooldir%\flash-tool-cli\flash-tool.bat" select_loader_options -ui_property 0 -ui_value !Bdata!
  set returnvalue=!ERRORLEVEL!

  if !returnvalue! NEQ 0 (
    echo ...set_unenhanced_force_flag failed...
    call :stopbackend
    exit /B 1
  )

  set Bdata=
  exit /B 0
  goto :EOF
)

rem --set-hardware-reset=<[ON | OFF]>"
rem  Input     : N/A
rem  Output    :
rem  Comment   :
rem
:set_hardware_reset_flag (
  echo.
  echo set_hardware_reset_flag %1
  set Bdata="1"

  if "%1" == "OFF" (
    set Bdata="0"
  )

  call :common_init
  IF ERRORLEVEL 1 (
    exit /B 1
  )
  call :startbackend
  IF ERRORLEVEL 1 (
    exit /B 1
  )

  call "%tooldir%\flash-tool-cli\flash-tool.bat" select_loader_options -ui_property 1 -ui_value !Bdata!
  set returnvalue=!ERRORLEVEL!

  if !returnvalue! NEQ 0 (
    echo ...set_hardware_reset_flag failed...
    call :stopbackend
    exit /B 1
  )

  set Bdata=
  exit /B 0
  goto :EOF
)


rem start the backend and also request the user to turn on the hw so it
rem connects to the backend
:startbackend (

  echo.
  echo Waiting for Flash-tool-backend

  if !backend_running! EQU 0 (
    set sbprofile=STE_DBX500_flashloader

    if NOT "%1" == "" (
      set sbprofile=%1
    )

    if !debug_enabled! EQU 1 (
      call start "Flashkit backend" /MIN cmd /k ""%tooldir%\flash-tool-backend\flash-tool-backend.bat" /DEBUG /LOADERS "%rundir%" /CLOSE_ON_EXIT"
    ) else (
      call start "Flashkit backend" /MIN cmd /k ""%tooldir%\flash-tool-backend\flash-tool-backend.bat" /LOADERS "%rundir%" /CLOSE_ON_EXIT"
    )

    :waitbackend
    call "%tooldir%\flash-tool-cli\flash-tool.bat" get_available_profiles -wait y >NUL 2>NUL
    rem Only check for errorlevel indicating that the backend is not started.
    rem so this errorcode should not be considered a failure
    IF ERRORLEVEL 2 (
        goto waitbackend
    )

    echo Flash-tool-backend is up and running.
    rem do we need this?
    call "%tooldir%\flash-tool-cli\flash-tool.bat" set_active_profile -profile_name !sbprofile! >NUL 2>NUL

    set sbprofile=

    if not [!trigger_uart_port!] == [] (
      call "%tooldir%\flash-tool-cli\flash-tool.bat" trigger_uart_port -port_name !trigger_uart_port! >NUL
    )

    set trigger_uart_port=

    echo.
    echo Waiting for next connected equipment..
    echo Connect and reset or power on ME
    echo.
    rem wait for next connected equipment
    call "%tooldir%\flash-tool-cli\flash-tool.bat" get_next_connected_equipment
    if ERRORLEVEL 2 (
      echo ERROR OCCURED. Description:
      echo 64 bit LCD and LCM drivers cannot be found in the native folder. You can:
      echo 1. Use 32 bit java with 32 bit drivers, or
      echo 2. Build 64 bit LCD and LCM drivers and copy to the native folder in flash-tool-backend.
      echo.
    ) else (
      echo Connection established.
      set backend_running=1
    )
  )

  exit /B 0
  goto :EOF
)

rem stops the backend
:stopbackend (
  call "%tooldir%\flash-tool-cli\flash-tool.bat" shutdown_backend
  set backend_running=0

  exit /B 0
  goto :EOF
)

rem if using remote signing it checks that the variables are set and for
rem local signing it sets the variables to the default location
:checksignsetup (
  if %remote_signing% EQU 1 (
    rem Strip environment variable from any whitespace
    set SIGNSERVER=%SIGNSERVER: =%
    if "%SIGNSERVER%"=="" (
      echo The environment variable SIGNSERVER must be set and point to the sign server to use.
      echo For instructions, look in the sign tool documentation
      echo %tooldir%sign-tool-cli\doc\sign-tool-cli-u5500.html
      echo Example:
      echo "set SIGNSERVER=<ip-address>:<port>"

      exit /B 1
    )
  ) else (
    set LOCAL_SIGNPACKAGEROOT=!defsignpackegepath!
    set LOCAL_KEYROOT=!defkeypath!
  )

  exit /B 0
  goto :EOF
)

rem -- Windows specific support functions ---------------------------------------
rem find the next command line switch starting from a specific switch
rem name of variable to place the result (the index to the next switch)
rem the index to start from
:argumentsfindend (
  set afevarname=%1
  set /a afefstart=%2
  rem compensate for extra params
  set /a afefstart+=2

  set /a afeindex=0

  set afeend=-1

  set afetmp=
  set afefirstchar=

  rem echo argumentsfindend start !afefstart!

  for %%a in (%*) do (
    if !afeend! EQU -1 (
      if !afeindex! GTR !afefstart! (
        set afetmp=%%a
        set afefirstchar=!afetmp:~0,1!

        rem echo tmp !afetmp! afeindex !afeindex! firstchar !afefirstchar!
        if "!afefirstchar!" == "-" (
          rem remove compensation
          rem echo end found
          set /a afeend=!afeindex!-2
        ) else (
          rem echo argumentsfindend %%a !afefirstchar!
        )
      )
    )

    set /a afeindex+=1
  )
  set afetmp=
  set afefirstchar=

  if !afeend! EQU -1 (
    set /a afeend=!afeindex!-2
  )

  rem echo argumentsfindend end !afeend!

  set afeindex=
  set afefstart=

  set "!afevarname!=%afeend%"
  set afevarname=

  exit /B 0
  goto :EOF
)

rem extract extra arguments for options with optional arguments
rem parameters:
rem name of variable to place the concatenated string
rem name of variable to place the number of shifts required to move out the arguments
rem the paramatere to look for
rem the number of times to skip when the parameter is found (to handle mutiple same parameters)
:extractarguments (
  rem echo extractarguments: %*

  set eavar=%1
  set eanbrargsvar=%2
  set eaparam=%3
  set eatimetoskip=%4

  set /a eacount=0
  set /a eastart=-1
  set /a eaend=-1
  set /a eaparamfound=0
  set /a eanbrargs=0
  set eatext=

  for %%a in (%*) do (
    set eacurrent=%%a

    rem echo extractarguments eacurrent !eacurrent!
    rem this is to skip extra parameters passed to this function
    rem so this needs to be increased if the number of extra arguments
    rem increase
    if !eacount! GTR 2 (
      rem echo extractarguments test a %%a
      if "%%a" == "!eaparam!" (
        rem echo extractarguments paramfound !eaparamfound! timetoskip !eatimetoskip!
        if !eaparamfound! EQU !eatimetoskip! (
          rem echo extractarguments start found !eacount!
          set /a eastart=!eacount!
        )

        set /a eaparamfound+=1
      )
    )

    if !eaend! EQU -1 (
      if !eastart! NEQ -1 (
        call :argumentsfindend eaend !eastart! %*
        rem echo extractarguments extractarguments !eaend!
      )
    )

    rem echo extractarguments end !eaend! start !eastart! count !eacount! current -!eacurrent!-

    if !eastart! NEQ -1 (
      if !eacount! GTR !eastart! (
        if !eacount! LSS !eaend! (
          rem echo extractarguments New argument found -!eacurrent!-

          set eatext=!eatext!!eacurrent!,

          set /a eanbrargs+=1
          rem echo extractarguments nbrargs !eanbrargs!
          rem echo extractarguments complete text: !eatext!
        )
      )
    )

    set /a eacount+=1
  )
  set eacurrent=
  set eaparamfound=
  set eaend=
  set eastart=
  set eacount=
  set eatimetoskip=
  set eaparam=

  rem echo set result
  rem echo extractarguments result complete text: -!eatext!- nbrargs !eanbrargs!
  set "!eavar!=!eatext!"
  set "!eanbrargsvar!=!eanbrargs!"
  rem echo set result done

  set eavar=
  set eatext=
  set eanbrargsvar=
  set eanbrargs=

  exit /B 0
  goto :EOF
)

rem --debug"
rem  Input     : N/A
rem  Output    :
rem  Comment   :
rem
:backend_debug_enabled (
  echo.
  set debug_enabled=1
  echo backend debug enabled

  exit /B 0
  goto :EOF
)

rem Function to check if 32bit java is available
rem on the system, and if yes, add that path to PATH
:find_32bit_java (

  rem For now we're only checking the default Java 32bit
  rem install path. If user should happen to have a 32bit
  rem java installed on some special own path, as of now,
  rem it won't be detected.
  
  if "%JAVA_HOME%" == "" goto no_java_home
  
  :have_java_home
  set javadir="%JAVA_HOME%"
  goto set_java_32_path
  
  :no_java_home
  set javadir="%ProgramFiles(x86)%\Java\"
  goto set_java_32_path
  
:set_java_32_path (

    if exist %javadir% (

    cd /d %javadir%

    for /d %%a in (jdk*) do (
      if exist %javadir%\%%a\jre\bin\ (
        set jdkpath=%javadir%\%%a\jre\bin\
      )
    )

    if exist "!jdkpath!" (
      set PATH="!jdkpath!";"%PATH%"
      goto done
    ) else (
      for /d %%a in (jre*) do (
        if exist %javadir%\%%a\bin\ (
          set jrepath=%javadir%\%%a\bin\
        )
      )

      if exist "!jrepath!" (
        set PATH="!jrepath!";"%PATH%"
        goto done
      ) else (
        goto no-32bit-java
      )
  )
  )

:no-32bit-java
  echo.
  echo WARNING 32bit Java is required. It does not seem to be installed on your machine
  echo.
:done
  exit /B 0
  goto :EOF
)


rem Function to check which java version is set
rem If 64bit version is set, find_32bit_java will be called.
:check_java (
  java -version 2>&1 | find "64-Bit" >nul:
  if !ERRORLEVEL! NEQ 1 (
    call :find_32bit_java
  )
  exit /B 0
  goto :EOF
)

rem -- Main script execution ----------------------------------------------------
rem
:start
  rem note: checking if no arguments are passed to the script needs
  rem to be checked at the top and therefore is not done here...
  rem look before "goto start"

  rem needs to be seperate for long and short options
  set /a nbrerase=0
  set /a nbrwrite=0
  set /a nbrcreatearchive=0
  set /a nbrsecpar=0
  set /a nbrerasel=0
  set /a nbrwritel=0
  set /a nbrcreatearchivel=0
  set /a nbrsecparl=0

  rem Check if we're running Windows 7
  ver | findstr /i "6\.1\." > nul
  if %ERRORLEVEL% EQU 0 (
    call :check_java
  )
  call :get_cspsa

  cd /d %rundir%

:parseloop

  set /a extrashift=0

  rem echo %1 %2 %3
  rem for improvement:
  rem it is possible to "cut" a number of characters using set targetname=!targetname:~0,-1!

  if "%1" == "-h" (
    call :print_help
    goto continue
  )
  if "%1" == "--help" (
    call :print_help
    goto continue
  )
  if "%1" == "-v" (
    call :set_verbose
    goto continue
  )
  if "%1" == "--verbose" (
    call :set_verbose
    goto continue
  )
  if "%1" == "-V" (
    call :print_version %0
    goto continue
  )
  if "%1" == "--version" (
    call :print_version %0
    goto continue
  )
  if "%1" == "-t" ( rem -t<HW>
    shift
    call :set_target_hw %2
    goto continue
  )
  if "%1" == "--target" (
    shift
    call :set_target_hw %2
    goto continue
  )
  if "%1" == "-l" (
    call :list_targets
    goto continue
  )
  if "%1" == "--list-targets" (
    call :list_targets
    goto continue
  )
  if "%1" == "-i" (
    call :list_images
    goto continue
  )
  if "%1" == "--list-images" (
    call :list_images
    goto continue
  )
  if "%1" == "-o" (
    call :list_me_devices
    goto continue
  )
  if "%1" == "--list-on-target-images" (
    call :list_me_devices
    goto continue
  )
  if "%1" == "-C" (
    call :set_commercial_signing
    goto continue
  )
  if "%1" == "--sign-for-commercial" (
    call :set_commercial_signing
    goto continue
  )
  if "%1" == "-p" (
    shift
    call :set_sign_package %2
    goto continue
  )
  if "%1" == "--signpackage" (
    shift
    call :set_sign_package %2
    goto continue
  )
  if "%1" == "-L" (
    call :set_local_signing
    goto continue
  )
  if "%1" == "--sign-local" (
    call :set_local_signing
    goto continue
  )
  if "%1" == "-R" (
    call :set_remote_signing
    goto continue
  )
  if "%1" == "--sign-remote" (
    call :set_remote_signing
    goto continue
  )
  if "%1" == "-N" (
    call :set_no_signing_archive
    goto continue
  )
  if "%1" == "--no-signing-archive" (
    call :set_no_signing_archive
    goto continue
  )
  if "%1" == "-B" (
    shift
    call :set_boot_itp_flag %2
    goto continue
  )
  if "%1" == "--boot-itp" (
    shift
    call :set_boot_itp_flag %2
    goto continue
  )
  if "%1" == "-e" ( rem -e[<IMAGE>][,<IMAGE 2>][,...<IMAGE N>]
    set arguments=
    call :extractarguments arguments extrashift -e !nbrerase! %*

    call :erase !arguments!

    set /a nbrerase+=1

    goto continue
  )
  if "%1" == "--erase" (
    set arguments=
    call :extractarguments arguments extrashift --erase !nbrerasel! %*

    call :erase !arguments!

    set /a nbrerasel+=1

    goto continue
  )
  if "%1" == "-E" (
    call :erase_no_emmc
    goto continue
  )
  if "%1" == "--erase_no_emmc" (
    call :erase_no_emmc
    goto continue
  )
  if "%1" == "-w" ( rem -w<IMAGE>,<FILENAME>[,<IMAGE 2>,<FILENAME 2>][,...<IMAGE N>,<FILENAME N>]
    set arguments=
    call :extractarguments arguments extrashift -w !nbrwrite! %*

    call :write_images !arguments!

    set /a nbrwrite+=1

    goto continue
  )
  if "%1" == "--write" (
    set arguments=
    call :extractarguments arguments extrashift --write !nbrwritel! %*

    call :write_images !arguments!

    set /a nbrwritel+=1

    goto continue
  )
  if "%1" == "-I" ( rem -I<IMAGE>,<FILENAME>[,<IMAGE 2>,<FILENAME 2>][,...<IMAGE N>,<FILENAME N>]
    set arguments=
    call :extractarguments arguments extrashift -I !nbrcreatearchive! %*

    call :create_flasharchive !arguments!

    set /a nbrcreatearchive+=1

    goto continue
  )
  if "%1" == "--create-flasharchive" (
    set arguments=
    call :extractarguments arguments extrashift --create-flasharchive !nbrcreatearchivel! %*

    call :create_flasharchive !arguments!

    set /a nbrcreatearchivel+=1

    goto continue
  )
  if "%1" == "-c" (
    call :write_complete
    goto continue
  )
  if "%1" == "--write-complete" (
    call :write_complete
    goto continue
  )
  if "%1" == "-m" (
    call :write_complete_cspsa
    goto continue
  )
  if "%1" == "--write-complete-cspsa" (
    call :write_complete_cspsa
    goto continue
  )
  if "%1" == "-F" (
    call :create_complete_flasharchive
    goto continue
  )
  if "%1" == "--create-complete-flasharchive" (
    call :create_complete_flasharchive
    goto continue
  )
  if "%1" == "-T" (
    call :create_complete_flasharchive_cspsa
    goto continue
  )
  if "%1" == "--create-complete-flasharchive-cspsa" (
    call :create_complete_flasharchive_cspsa
    goto continue
  )
  if "%1" == "-x" ( rem <ARCHIVE>
    shift
    call :write_existing %2
    goto continue
  )
  if "%1" == "--write-existing" (
    shift
    call :write_existing %2
    goto continue
  )
  if "%1" == "-d" ( rem -d<START>,<LENGTH>[,<OUTFILE>]
    shift
    shift

    set tmpvar=%4
    set outfile=

    set firstchar=!tmpvar:~0,1!
    if NOT "!firstchar!" == "-" (
      set outfile=%~4
      shift
      set tmpvar=%5
      set firstchar=!tmpvar:~0,1!
      if NOT "!firstchar!" == "-" (
        set outfile=!outfile! %5
        shift
      )
    )
    call :dump_data %2 %3 "!outfile!"
    goto continue
  )
  if "%1" == "--dump-data" (
    shift
    shift

    set tmpvar=%4
    set outfile=

    set firstchar=!tmpvar:~0,1!
    if NOT "!firstchar!" == "-" (
      set outfile=%~4
      shift
      set tmpvar=%5
      set firstchar=!tmpvar:~0,1!
      if NOT "!firstchar!" == "-" (
        set outfile=!outfile! %5
        shift
      )
    )
    call :dump_data %2 %3 "!outfile!"
    goto continue
  )
  if "%1" == "--set-enhanced-area" (rem --set-enhanced-area <FLASH AREA>,<START>,<LENGHT>
    shift
    shift

    call :set_enhanced_area %2 %3 %4
    goto continue
  )
  if "%1" == "-g" ( rem -g[<OUTFILE>]
    set tmpvar=%2
    set outfile=
    set firstchar=!tmpvar:~0,1!
    if NOT "!firstchar!" == "-" (
      set outfile=%~2
      shift
      set tmpvar=%3
      set firstchar=!tmpvar:~0,1!
      if NOT "!firstchar!" == "-" (
        set outfile=!outfile! %3
        shift
      )
    )
    call :dump_gd "!outfile!"
    goto continue
  )
  if "%1" == "--dump-gd" (
    set tmpvar=%2
    set outfile=
    set firstchar=!tmpvar:~0,1!
    if NOT "!firstchar!" == "-" (
      set outfile=%~2
      shift
      set tmpvar=%3
      set firstchar=!tmpvar:~0,1!
      if NOT "!firstchar!" == "-" (
        set outfile=!outfile! %3
        shift
      )
    )
    call :dump_gd "!outfile!"
    goto continue
  )
  if "%1" == "-U" ( rem -U<FILE>
    set tmpvar=%2
    set firstchar=!tmpvar:~0,1!
    if NOT "!firstchar!" == "-" (
      shift
      call :write_gd_file %2
    ) else (
      call :write_gd_file
    )
    goto continue
  )
  if "%1" == "--write_gd_file" (
    set tmpvar=%2
    set firstchar=!tmpvar:~0,1!
    if NOT "!firstchar!" == "-" (
      shift
      call :write_gd_file %2
    ) else (
      call :write_gd_file
    )
    goto continue
  )
  if "%1" == "-s" ( rem -s<IMEI>,<SIMLOCK-CODES>,<COPS-DATA-FILE>
    set arguments=
    call :extractarguments arguments extrashift -s !nbrsecpar! %*

    call :write_sec_par !arguments!

    set /a nbrsecpar+=1

    goto continue
  )
  if "%1" == "--write-sec-par" (
    set arguments=
    call :extractarguments arguments extrashift --write-sec-par !nbrsecparl! %*

    call :write_sec_par !arguments!

    set /a nbrsecparl+=1

    goto continue
  )
  if "%1" == "-P" (
    call :set_permanent_authentication
    goto continue
  )
  if "%1" == "--permanent-authentication" (
    call :set_permanent_authentication
    goto continue
  )
  if "%1" == "-f" (
    call :format_gd
    goto continue
  )
  if "%1" == "--format-gd" (
    call :format_gd
    goto continue
  )
  if "%1" == "-G" ( rem -G<FILE>
    set tmpvar=%2
    set firstchar=!tmpvar:~0,1!
    if NOT "!firstchar!" == "-" (
      shift
      call :write_gd_image %2
    ) else (
      call :write_gd_image
    )
    goto continue
  )
  if "%1" == "--write-gd-image" (
    set tmpvar=%2
    set firstchar=!tmpvar:~0,1!
    if NOT "!firstchar!" == "-" (
      shift
      call :write_gd_image %2
    ) else (
      call :write_gd_image
    )
    goto continue
  )
    if "%1" == "-a" ( rem -a <DATA>
    shift
    call :arbtable %2
    goto continue
  )
  if "%1" == "--init-arb-table" (
    shift
    call :arbtable %2
    goto continue
  )
  if "%1" == "-b" ( rem -b<MODE>
    shift
    call :set_special_bootmode %2
    goto continue
  )
  if "%1" == "--set-bootmode" (
    shift
    call :set_special_bootmode %2
    goto continue
  )
  if "%1" == "-X" ( rem -b<DEBUGENABLED> <FILE>
    set tmpvar=%2
    set firstchar=!tmpvar:~0,1!
    if NOT "!firstchar!" == "-" (
      set tmpvar=%3
      set firstchar=!tmpvar:~0,1!
      if NOT "!firstchar!" == "-" (
        shift
        shift
        call :xlauncher %2 %3
      ) else (
        shift
        call :xlauncher %2
      )
    ) else (
      call :xlauncher
    )
    goto continue
  )
  if "%1" == "--xlaunch" (
    set tmpvar=%2
    set firstchar=!tmpvar:~0,1!
    if NOT "!firstchar!" == "-" (
      set tmpvar=%3
      set firstchar=!tmpvar:~0,1!
      if NOT "!firstchar!" == "-" (
        shift
        shift
        call :xlauncher %2 %3
      ) else (
        shift
        call :xlauncher %2
      )
    ) else (
      call :xlauncher
    )
    goto continue
  )
  if "%1" == "--trigger-uart-port" ( rem --trigger-uart-port <PORT-NAME>
    set tmpvar=%~2
    set firstchar=!tmpvar:~0,1!
    if NOT "!firstchar!" == "-" (
      shift
      call :set_trigger_uart_port "!tmpvar!"
    ) else (
      call :set_trigger_uart_port
    )
    goto continue
  )
  if "%1" == "--dnt" ( rem --dnt <FILE>
    set tmpvar=%~2
    set firstchar=!tmpvar:~0,1!
    if NOT "!firstchar!" == "-" (
      shift
      call :set_dnt "!tmpvar!"
    ) else (
      call :set_dnt
    )
    goto continue
  )
  if "%1" == "--write-otp" (
    set tmpvar=%~2
    set firstchar=!tmpvar:~0,1!
    if NOT "!firstchar!" == "-" (
      shift
      call :write_otp "!tmpvar!"
    ) else (
      call :write_otp
    )
    goto continue
  )
  if "%1" == "--read-otp" (
    set tmpvar=%~2
    set firstchar=!tmpvar:~0,1!
    if NOT "!firstchar!" == "-" (
      shift
      call :read_otp "!tmpvar!"
    ) else (
      call :read_otp
    )
    goto continue
  )
  if "%1" == "--sw_type_map" (
    set tmpvar=%~2
    set firstchar=!tmpvar:~0,1!
    if NOT "!firstchar!" == "-" (
      shift
      call :set_sw_type_map "!tmpvar!"
    ) else (
      call :set_sw_type_map
    )
    goto continue
  )
  if "%1" == "-u" ( rem -u<UNIT>,<DATA>
    set "all_arguments=%*"
    set gd_temp_var="FALSE"
    for %%a in ("!all_arguments: =" "!") do (
      set "gd_input=%%a"
      if !gd_input!=="%2,%3" (
        shift
        shift
        call :write_gd_unit %2 %3
        set gd_temp_var="TRUE"
      ) else (
        if !gd_input!=="%1=%2,%3" (
          shift
          shift
          call :write_gd_unit %2 %3
          set gd_temp_var="TRUE"
        )
      )
    )
    if not !gd_temp_var!=="TRUE" (
      call :write_gd_unit %2
    )
    goto continue
  )
  if "%1" == "--write-gd-unit" (
    set "all_arguments=%*"
    set gd_temp_var="FALSE"
    for %%a in ("!all_arguments: =" "!") do (
      set "gd_input=%%a"
      if !gd_input!=="%2,%3" (
        shift
        shift
        call :write_gd_unit %2 %3
        set gd_temp_var="TRUE"
      ) else (
        if !gd_input!=="%1=%2,%3" (
          shift
          shift
          call :write_gd_unit %2 %3
          set gd_temp_var="TRUE"
        )
      )
    )
    if not !gd_temp_var!=="TRUE" (
      call :write_gd_unit %2
    )
    goto continue
  )
  if "%1" == "--write-rpmb" (
	shift
    call :write_rpmb %2 %3
	shift
    goto continue
  )
  if "%1" == "--set-unenhanced-force" (
    set tmpvar=%2
    set firstchar=!tmpvar:~0,1!
    if NOT "!firstchar!" == "-" (
      shift
      call :set_unenhanced_force_flag %2
    ) else (
      call :set_unenhanced_force_flag
    )
    goto continue
  )
  if "%1" == "--set-hardware-reset" (
    set tmpvar=%2
    set firstchar=!tmpvar:~0,1!
    if NOT "!firstchar!" == "-" (
      shift
      call :set_hardware_reset_flag %2
    ) else (
      call :set_hardware_reset_flag
    )
    goto continue
  )
  if "%1" == "--debug" (
    call :backend_debug_enabled
    goto continue
  )
  echo Unknown parameter %1
  goto terminate

:continue
rem  echo +++ continue %1 %2
  shift
  if !extrashift! NEQ 0 (
    for /L %%t in (1,1,!extrashift!) do (
      shift
    )
    set /a extrashift=0
  )
rem  echo +++ continue shifted %1 %2
  if %ERRORLEVEL% NEQ 0 (
      exit /B 1
  )
  if NOT "%1" == "" goto parseloop

:terminate
if !backend_running! EQU 1 (
   call :stopbackend
)

set loader_created=0
