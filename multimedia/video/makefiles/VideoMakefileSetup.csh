echo "************************************************************************************************"
echo " Note that you can avoid sourcing this file if you wish to compile everything (as in 'all')"
echo "    and you don't have in your env any variable NO_STD_BUILD NO_NMF_BUILD NO_OMX_BUILD defined"
echo "************************************************************************************************"
echo " "  

if ("$1" != "fw" && "$1" != "nmf" && "$1" != "omx" && "$1" != "all") then
echo "Build type must be specified as argument (vsourceme <build type>)"
echo "fw  => Use this type if you only wish to develop standalone FW (without NMF)"
echo "nmf => Use this type if you work at ARM nmf/testappli level (this skips OMX proxy compil)"
echo "omx => Use this if you work at proxy level and you want to avoid compilation of non-NMF FW"
echo "all => build ALL nmf and non-nmf binaries, this is the default option"
echo " "
exit
endif

if ( ! $?MMROOT ) then
    echo Please setenv MMROOT variable to your multimedia development directory 
else
   # source $MM_MAKEFILES_DIR/SharedMakefileSetup.csh
   # source ../videotools/videotools
   # Avoid videotools as depedency ! 
     if ("$1" == "fw") then
        echo Standalone firmware, disables NMF/OMX build 
        unsetenv NO_STD_BUILD
        setenv NO_NMF_BUILD 1
        setenv NO_OMX_BUILD 1
    else if ("$1" == "nmf") then
        echo "NMF development settings (disable OMX and standalone build)" 
        unsetenv NO_NMF_BUILD
        setenv NO_STD_BUILD 1
        setenv NO_OMX_BUILD 1
    else if ("$1" == "omx") then
        echo "omx proxy settings (disable standalone build)"
        unsetenv NO_NMF_BUILD
        setenv NO_STD_BUILD 1
        unsetenv NO_OMX_BUILD
    else if ("$1" == "all") then
        echo "Build everything from standalone FW up to OMX proxy (default option)"
        unsetenv NO_NMF_BUILD
        unsetenv NO_STD_BUILD
        unsetenv NO_OMX_BUILD
    else
        echo "Unknown build type $1"
    endif

    # TLM
    setenv SVA_TLM_DIR_ndk8500_ed_ttv /prj/multimedia_shared/common/TOOLS/Linux/internals/TLM/TTV/8820.B0.1.9.47
    setenv SVA_TLM_DIR_ndk8500_a0_ttv /prj/multimedia_shared/common/TOOLS/Linux/internals/TLM/TTV/8500.A0.1.0.65
    setenv SVA_TLM_DIR_ndk8500_ed_svp /prj/multimedia_shared/common/TOOLS/Linux/internals/TLM/SVP/NVP_8820_1.4.666-RedHatEnterpriseWS_3-x86_64-gcc_3.4.6

    # symbian checks
    alias constraint_mmgrep find . -name \".svn\" -prune -o -type f -name \"\!:2\" -exec grep -w -H -n \!:1 \\\{\} \\\;
    alias constraint_grep grep -w \"\!:1\" \"\"\!:2\"\"
    
endif
