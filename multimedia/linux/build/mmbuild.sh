#
# Wrapper for lbp & android build command
# Allows to build multimedia components unitarily with a simple command
#
# Author: sonia.petetin@stericsson.com
#
# LBP:
#    go to the folder of the component you want to build, then type:
#    lmm
#    or
#    use lm with a multimedia component name:
#    lm component
#
# ANDROID:
#    go to the folder of the component you want to build, then type:
#    amm
#    or
#    use am with a multimedia component name:
#    am component


ammusage() {

cat <<EOF

amm:
  runs android build command for the multimedia component in current dir
  if called from multimedia/linux/build, builds the whole multimedia stack
  if current dir is not in multimedia tree: exit with error

usage:

  amm [options] [flags]

options:

  --help|-h             : prints usage
  --build|-b            : select target for build (default)
  --clean|-c            : select target for clean
  --target=|-t <target> : select specific target
  --dryrun              : prints build command, no execution

example:
  ## Build the component and its dependencies without rebuilding the prebuilt parts kept in source tree
  amm
  ## Build the component but not its dependencies without rebuilding the prebuilt parts kept in source tree
  amm BUILD_DEPS=false
  ## Build the component and its dependencies including rebuilding the prebuild parts kept in source tree
  ## also build the MMDSP alternate platforms binaries if any
  amm FORCEBUILD=src
  ## Build the component but not its dependencies including rebuilding the prebuild parts kept in source tree
  ## also build the MMDSP alternate platforms binaries if any
  amm BUILD_DEPS=false FORCEBUILD=src
  ## Clean the component and its dependencies but not the prebuild parts kept in source tree
  amm --clean
  ## Clean the component and its dependencies INCLUDING the prebuild parts kept in source tree
  amm --clean FORCEBUILD=src
  ## Clean the component but not its dependencies but not the prebuild parts kept in source tree
  amm --clean BUILD_DEPS=false
  ## Clean the component but not its dependencies INCLUDING the prebuild parts kept in source tree
  amm --clean BUILD_DEPS=false FORCEBUILD=src
  ## Build the test target
  amm --target=test

EOF
}

amusage() {

cat <<EOF

amm:
  runs android build command for the multimedia component given in parameter
  if component is a path under multimedia tree, we use it as it is
  otherwise, we try to detect it:
      - first trial: we try to find its entry in linux/build/Dirs.XXXX.mk. WARNING: we take only the first match!
      - second trial: we try to find this component by running a find with limited depth level in MM tree. WARNING: we take only the first match!
  to build the whole android tree: run am from top of environment without specifying any component
  to build the whole multimedia tree: run am with component "mm"

usage:

  amm [component] [options] [flags]

options:

  --help|-h             : prints usage
  --build|-b            : select target for build (default)
  --clean|-c            : select target for clean
  --target=|-t <target> : select specific target
  --dryrun              : prints build command, no execution

example:
  am g1dec
  am g1dec FORCEBUILD=src
  am g1dec --clean
  am g1dec --target=test

EOF
}

lmmusage() {

cat <<EOF

lmm:
  runs lbp build command for the multimedia component in current dir
  if called from multimedia/linux/build, builds the whole multimedia stack
  if called from top, builds the whole lbp content
  otherwise, if current dir is not in multimedia tree: exit with error

usage:

  lmm [options] [flags]

options:

  --help|-h                 : prints usage
  --build|-b                : select target for build (default)
  --clean|-c                : select target for clean
  --ramdisk|-r              : generate ramdisk
  --kernel|-k               : build linux kernel
  --kernel-modules|--kmo|-m : build kernel modules
  --target=|-t <TARGET>     : use target passed in TARGET
  --dryrun|-n               : prints build command, no execution

example:
  ## Build the component and its dependencies without rebuilding the prebuilt parts kept in source tree
  lmm
  ## Build the component but not its dependencies without rebuilding the prebuilt parts kept in source tree
  lmm BUILD_DEPS=false
  ## Build the component and its dependencies including rebuilding the prebuild parts kept in source tree
  ## also build the MMDSP alternate platforms binaries if any
  lmm FORCEBUILD=src
  ## Build the component but not its dependencies including rebuilding the prebuild parts kept in source tree
  ## also build the MMDSP alternate platforms binaries if any
  lmm BUILD_DEPS=false FORCEBUILD=src
  ## Clean the component but not the prebuild parts kept in source tree
  lmm --clean
  ## Clean the component INCLUDING the prebuild parts kept in source tree
  lmm --clean FORCEBUILD=src
  ## Build the test target
  lmm --target=test

EOF
}

lmusage() {

cat <<EOF

lm:
  runs lbp build command for the multimedia component given in parameter
  if component is a path under multimedia tree, we use it as it is
  otherwise, we try to detect it:
      - first trial: we try to find its entry in linux/build/Dirs.XXXX.mk. WARNING: we take only the first match!
      - second trial: we try to find this component by running a find with limited depth level in MM tree. WARNING: we take only the first match!

usage:

  lm component [options] FLAGS

options:

  --help|-h                 : prints usage
  --build|-b                : select target for build (default)
  --clean|-c                : select target for clean
  --ramdisk|-r              : generate ramdisk
  --kernel|-k               : build linux kernel
  --kernel-modules|--kmo|-m : build kernel modules
  --target=|-t <TARGET>     : use target passed in TARGET
  --dryrun|-n               : prints build command, no execution

example:
  lm g1dec
  lm g1dec --clean
  lm g1dec --target=test
  lm g1dec FORCEBUILD=src

EOF
}

xmmusage() {

cat <<EOF

xmm:
  runs x86 build command for the multimedia component in current dir
  if called from multimedia/linux/build, builds the whole multimedia stack
  if called from top, builds the whole lbp content
  otherwise, if current dir is not in multimedia tree: exit with error

usage:

  xmm [options] [flags]

options:

  --help|-h                 : prints usage
  --build|-b                : select target for build (default)
  --clean|-c                : select target for clean
  --target=|-t <TARGET>     : use target passed in TARGET
  --dryrun|-n               : prints build command, no execution

example:
  xmm
  xmm --clean
  xmm --target=test

EOF
}

xmusage() {

cat <<EOF

xm:
  runs x86 build command for the multimedia component given in parameter
  if component is a path under multimedia tree, we use it as it is
  otherwise, we try to detect it:
      - first trial: we try to find its entry in linux/build/Dirs.XXXX.mk. WARNING: we take only the first match!
      - second trial: we try to find this component by running a find with limited depth level in MM tree. WARNING: we take only the first match!

usage:

  xm component [options] FLAGS

options:

  --help|-h                 : prints usage
  --build|-b                : select target for build (default)
  --clean|-c                : select target for clean
  --target=|-t <TARGET>     : use target passed in TARGET
  --dryrun|-n               : prints build command, no execution

example:
  xm audiolibs
  xm audiolibs --clean
  xm audiolibs --target=test

EOF
}



getmmtop () {
    # Get multimedia folder from current directory
    # We don't simply return $MMROOT
    # This allows to check if we're really in multimedia folder

    local TOP=`echo $PWD | sed -n 's:\(.*multimedia\)\/.*:\1:p'`
    echo "$TOP"
}

getmmtoprel () {
    # Get relative path from environment top directory to multimedia folder
    # Use LBP_BUILD_TOP instead of ANDROID_BUILD_TOP, since LBP_BUILD_TOP is exported during sourcing of mmbuild.sh
    # Hence it's always defined

    local dir=`echo $MMROOT | sed -n 's:.*'$LBP_BUILD_TOP'/\(.*\):\1:p'`
    echo "$dir"
}

getmmrel () {
    # Get relative path from multimedia folder to path given in argument

    local dir="$1"
    dir=`echo $dir | sed -n 's:.*multimedia/\(.*\):\1:p'`
    echo "$dir"
}

getmmtarget () {

    # Get make target according to build type (eg android or lbp)
    # and requested target (eg build or clean)

    local build_type="$1"
    local arg="$2"
    local buildall="$3"
    local target

    if [ "$build_type" = "lbp" -a "$arg" = "clean" -a $buildall -eq 1 ] ; then
        target="lbp-clean"
    elif [ "$build_type" = "lbp" -a "$arg" = "clean" ] ; then
        target="mm-clean"
    elif [ "$build_type" = "lbp" -a "$arg" = "build" -a $buildall -eq 1 ] ; then
        target="lbp-all"
    elif [ "$build_type" = "lbp" -a "$arg" = "build" ] ; then
        target="mm-all"
    elif [ "$build_type" = "lbp" -a "$arg" = "all" -a $buildall -eq 1 ] ; then
        target="lbp-all"
    elif [ "$build_type" = "lbp" -a "$arg" = "all" ] ; then
        target="mm-all"
    elif [ "$build_type" = "lbp" -a "$arg" = "ramdisk" ] ; then
        target="lbp-ramdisk"
    elif [ "$build_type" = "lbp" -a "$arg" = "kernel" ] ; then
        target="lbp-kernel"
    elif [ "$build_type" = "lbp" -a "$arg" = "kernel-modules" ] ; then
        target="lbp-kernel-modules"
    elif [ "$build_type" = "lbp" -a ! "$arg" = "" ] ; then
        target="DO=$arg $arg"
    elif [ "$build_type" = "android" -a "$arg" = "clean" -a $buildall -eq 1 ] ; then
        target="clean"
    elif [ "$build_type" = "android" -a "$arg" = "clean" ] ; then
        target="MM_DO=clean-multimedia"
    elif [ "$build_type" = "android" -a $buildall -eq 1 ] ; then
        target=""
    elif [ "$build_type" = "android" -a "$arg" = "build" ] ; then
        target="build-multimedia"
    elif [ "$build_type" = "android" -a ! "$arg" = "" ] ; then
        target="DO=$arg $arg"
    elif [ "$build_type" = "lbp" ] ; then
        target="mm-all"
    elif [ "$build_type" = "x86" -a "$arg" = "clean" -a $buildall -eq 1 ] ; then
        target="mm-clean"
    elif [ "$build_type" = "x86" -a "$arg" = "clean" ] ; then
        target="mm-clean"
    elif [ "$build_type" = "x86" -a "$arg" = "all" ] ; then
        target="mm-all"
    elif [ "$build_type" = "x86" -a ! "$arg" = "" ] ; then
        target="DO=$arg $arg"
    elif [ "$build_type" = "x86" ] ; then
        target="mm-all"
    else
        target="build-multimedia"
    fi
    echo "$target"
}

getcmpdir () {
    # Return relative path of multimedia component from multimedia top dir
    # Search component in Dirs.xxx.mk file or in multimedia tree

    local cmp=$1
    local cmpdir
    local TOP=$LBP_BUILD_TOP

    # Environment may be android or lbp, so we need to try several variables
    local platform=`echo ${TARGET_PRODUCT:4}`
    if [ -z "$platform" ]; then
      platform=$STE_PLATFORM
    fi

    # Search component in Dirs.xxx.mk
    local dirfile=$MMROOT/linux/build/Dirs.$platform.mk
    if [ ! -f $dirfile ]; then
	# Unable to find mm central dir file
	return
    fi
    cmpdir=`\grep -m 1 $cmp $dirfile`
    # Remove trailing " \" or other character after a space
    local cmpdir2=`echo $cmpdir | sed -n 's:\([\w/]*'$cmp'\)\([ ].*\):\1:p'`
    [ ! -z "$cmpdir2" ] && cmpdir=$cmpdir2
    # Remove characters before component path: eg in "XXXXXX path/component" => keep "path/component"
    cmpdir2=`echo $cmpdir | sed -n 's:\(.*[ ]\)\([a-zA-Z0-9_/]*'$cmp'\):\2:p'`
    [ ! -z "$cmpdir2" ] && cmpdir=$cmpdir2

    # Test the path, to be sure we didn't get some bad information from Dir file
    if [ ! -z "$cmpdir" -a -d $MMROOT/$cmpdir ]; then
	echo "$cmpdir"
	return
    fi
    # If not found, search component in source tree
    if [ ! "$cmpdir" ]; then
	cmpdir=`find $MMROOT -maxdepth 3 -name $cmp -type d | head -1`
	cmpdir=$(getmmrel $cmpdir)
    fi
    echo "$cmpdir"
}

am () {
    _mmandroid_main am $@
}

amm () {
    _mmandroid_main amm $@
}

lm () {
    _mmlbp_main lm $@
}

lmm () {
    _mmlbp_main lmm $@
}

xm () {
    _mmx86_main xm $@
}

xmm () {
    _mmx86_main xmm $@
}


_mmandroid_main () {

    # Main script for Android build. Uses script name as parameter: amm or am
    local script=$1
    shift

    local target="build-multimedia"
    local dryrun=0
    local MMBUILD=$MMROOT/linux/build

    local cmp=$1
    local isdash=`expr match "$cmp" '-.*'`
    local buildall=0
    local buildmm=0
    local cmpdir=

    if [ "$script" = "am" ]; then
        # am: get component from parameters
	if [ "${isdash}" != "0" ]; then
	    if [ ! -d $(getmmtoprel)/linux/build ]; then
		echo "Error: you must specify component name/path first"
		[ "$script" = "am" ] && amusage
		[ "$script" = "amm" ] && ammusage
		return
	    else
		buildall=1
		OPTIND=1
	    fi
	else
	    OPTIND=2
	fi
    else
	if [ -f product/common/ux500.mak ]; then
	    buildall=1
	    target=""
	fi
	OPTIND=1
    fi

    # Parse options and rebuild the unkown ones to pass them to make
    local OPT
    local UNKNOWN_ARG=""
    local UNKNOWN_ARG_PRE=" -"
    while getopts ":bcxnht:-:" OPT
    do
        # Get long options with or without value
	[ $OPT = "-" ] && case "${OPTARG%%=*}" in
            build)   OPT="b" ;;
            clean)   OPT="c" ;;
            dryrun)  OPT="n" ; echo "short for dryrun" ;;
            help)    OPT="h" ;;
            target)  OPT="t" ; OPTARG="${OPTARG#*=}" ;;
            *) UNKNOWN_ARG_PRE=" --";;
	esac
        # Get short options with or without value
	case $OPT in
            b) target=$(getmmtarget android build $buildall) ;;
            c) target=$(getmmtarget android clean $buildall) ;;
            h) [ "$script" = "am" ] && amusage; [ "$script" = "amm" ] && ammusage; return ;;
            n) echo "dry-run activated" ; dryrun=1 ;;
            t) target=$(getmmtarget android $OPTARG $buildall) ;;
            t) target="DO=$OPTARG $OPTARG";;
            *) UNKNOWN_ARG="${UNKNOWN_ARG}${UNKNOWN_ARG_PRE}${OPTARG}" ; if [ "$UNKNOWN_ARG_PRE" = " --" ] ; then UNKNOWN_ARG_PRE=" -" ; else UNKNOWN_ARG_PRE="" ; fi ;;
	esac
    done

    # Get flags from arguments, expected after options
    if [ $OPTIND -eq 2 ]; then
	shift
    else
	let optshift=$OPTIND-1
	shift $optshift
    fi
    local MAKE_ARGS=$(echo -n "$@" | awk -v RS=" " -v ORS=" " '/^[^-].*$/')
    local cmd=

    MAKE_ARGS="$UNKNOWN_ARG $MAKE_ARGS"

    [ "$script" = "amm" -a -f build/core/envsetup.mk -a -f Makefile ] && buildall=1
    [ "$script" = "amm" -a "$PWD" = "$MMBUILD" -a -f Android.mk ] && buildmm=1
    [ "$script" = "am" -a "$cmp" = "mm" ] && buildmm=1

    if [ $buildall -eq 1 ]; then
	[ "$target" = "clean" ] && echo "Clean everything from top"
	[ "$target" = "" ] && echo "Build everything from top"
	cmd="make $target $MAKE_ARGS"
        echo "  $cmd" && [ "$dryrun" -eq "0" ] && eval $cmd
	return
    fi

    MMT=$(getmmtop)
    if [ ! "$MMT" -a "$script" = "amm" ]; then
        echo "Not in multimedia folder. Run this script under multimedia tree." && return 1
    fi

    T=$(gettop)
    if [ ! "$T" ]; then
        echo "Couldn't locate the top of the tree.  Try setting TOP."
        return 1
    fi

    # For amm, detect component path from current dir
    if [ "$script" = "amm" -a $buildmm -eq 0 ]; then
	cmpdir=$(getmmrel $PWD)
	echo "Component dir=$cmpdir"
    fi

    # For am, detect component path from argument
    if [ "$script" = "am" -a $buildmm -eq 0 ]; then
	if [ -d $MMROOT/$cmp ]; then
	    echo "Component parameter $cmp is a correct path in MM tree. Let's use it as it is"
	    cmpdir=$cmp
	else
	    cmpdir=$(getcmpdir $cmp)
	fi
	if [ ! "$cmpdir" ]; then
	    echo "Error: component $cmp not found!"
	    echo "Check your environment or call amm in revelevant folder instead of using this this script"
	    return
	fi
	echo "Component dir=$cmpdir"
    fi

    # Find mm android makefile
    local M
    if [ -f $MMBUILD/Android.mk ]; then
        M=$MMBUILD/Android.mk
        M=`echo $M|sed 's:'$T'/::'`
    else
        echo "Unable to find MM android makefile."
        return 1
    fi

    # Run build command for selected multimedia stuff
    if [ $buildmm -eq 1 ]; then
	if [[ "$target" == *clean* ]]; then
	    echo "Clean whole multimedia tree"
	else
	    echo "Build whole multimedia tree"
	fi
        cmd="ONE_SHOT_MAKEFILE=$M make -C $T all_modules $MAKE_ARGS $target"
    else
        cmd="DIRS_TO_BUILD=\"$cmpdir\" ONE_SHOT_MAKEFILE=$M make -C $T all_modules $MAKE_ARGS $target"
    fi
    echo "  $cmd" && [ "$dryrun" -eq "0" ] && eval $cmd

}

_mmlbp_main () {

    # Main script for LBP build. Uses script name as parameter: lmm or lm
    local script=$1
    shift

    local target="mm-all"
    local dryrun=0
    MMT=$(getmmtop)
    local MMBUILD=$MMROOT/linux/build

    local cmp=$1
    local isdash=`expr match "$cmp" '-.*'`
    local buildall=0
    local buildmm=0
    local buildkernel=0
    local buildramdisk=0

    # For lm: get component name from parameters
    if [ "$script" = "lm" ]; then
	if [ "${isdash}" != "0" ]; then
	    if [ ! -d $(getmmtoprel)/linux/build ]; then
		echo "Error: you must specify component name/path first"
                [ "$script" = "lm" ] && lmusage
		[ "$script" = "lmm" ] && lmmusage
		return
	    else
		buildall=1
		target=lbp-all
		OPTIND=1
	    fi
	else
	    OPTIND=2
	fi
    else
	if [ -f product/common/ux500.mak ]; then
	    buildall=1
	    target=lbp-all
	fi
	OPTIND=1
    fi

    # Parse options and rebuild the unkown ones to pass them to make
    local OPT
    local UNKNOWN_ARG=""
    local UNKNOWN_ARG_PRE=" -"
    while getopts ":bcxnhkmrt:-:" OPT
    do
        # Get long options with or without value
	[ $OPT = "-" ] && case "${OPTARG%%=*}" in
            build)   OPT="b" ;;
            clean)   OPT="c" ;;
            dryrun)  OPT="n" ;;
            help)    OPT="h" ;;
            kernel)  OPT="k" ;;
            kernel-modules)  OPT="m" ;;
            kmo)     OPT="m" ;;
            ramdisk) OPT="r" ;;
            target)  OPT="t" ; OPTARG="${OPTARG#*=}" ;;
            *) UNKNOWN_ARG_PRE=" --";;
	esac
        # Get short options with or without value
	case $OPT in
            b) target=$(getmmtarget lbp build $buildall) ;;
            c) target=$(getmmtarget lbp clean $buildall) ;;
            h) [ "$script" = "lm" ] && lmusage; [ "$script" = "lmm" ] && lmmusage; return ;;
            k) buildall=0; target=$(getmmtarget lbp kernel $buildall) ; buildkernel=1 ;;
            m) buildall=0; target=$(getmmtarget lbp kernel-modules $buildall) ; buildkernel=1 ;;
            n) echo "dry-run activated" ; dryrun=1 ;;
            t) target=$(getmmtarget lbp $OPTARG $buildall) ;;
            r) buildall=0; target=$(getmmtarget lbp ramdisk $buildall) ; buildramdisk=1 ;;
            *) UNKNOWN_ARG="${UNKNOWN_ARG}${UNKNOWN_ARG_PRE}${OPTARG}" ; if [ "$UNKNOWN_ARG_PRE" = " --" ] ; then UNKNOWN_ARG_PRE=" -" ; else UNKNOWN_ARG_PRE="" ; fi ;;
	esac
    done

    # Get flags from arguments, expected after options
    if [ $OPTIND -eq 2 ]; then
	shift
    else
	let optshift=$OPTIND-1
	shift $optshift
    fi

    local MAKE_ARGS=$(echo -n "$@" | awk -v RS=" " -v ORS=" " '/^[^-].*$/')
    local arg=
    local cmd=
    local cmpdir=

    MAKE_ARGS="$UNKNOWN_ARG $MAKE_ARGS"

    # Allow STE_PLATFORM to be provided as make argument
    for arg in $MAKE_ARGS ; do
        key=`echo $arg | cut -d '=' -f 1`
        if [ "$key" = "STE_PLATFORM" ] ; then
            STE_PLATFORM=`echo $arg | cut -d '=' -f 2`
        fi
    done

    if [ -z "$STE_PLATFORM" ]; then
	local platform=`echo ${TARGET_PRODUCT:4}`
	if [ ! -z "$platform" ]; then
	    echo "found TARGET_PRODUCT=$TARGET_PRODUCT, set STE_PLATFORM to $platform"
	    export STE_PLATFORM=$platform
	else
            echo "Error: STE_PLATFORM must be set in your environment, supported values are:"
            echo -n "  "
            cd $MMROOT/linux/build && \ls Dirs.*.mk | \sed 's,Dirs\.,  ,g' | \sed 's,\.mk,,g' | \awk '{ printf "%s  ", $1 }' && cd - > /dev/null
            echo
            return
	fi
    fi

    [ "$script" = "lmm" -a -f product/common/ux500.mak -a $buildkernel -ne 1 ] && buildall=1
    [ "$script" = "lmm" -a "$PWD" = "$MMBUILD" ] && buildmm=1
    [ "$script" = "lm" -a "$cmp" = "mm" ] && buildmm=1

    if [ $buildall -eq 1 -o $buildkernel -eq 1 -o $buildramdisk -eq 1 ]; then
	if [ $buildall -eq 1 ]; then
	    if [[ "$target" == *clean* ]]; then
		echo "Clean everything from top"
	    else
		echo "Build everything from top"
	    fi
	fi
	[ $buildkernel -eq 1 ] && echo "Build kernel"
	[ $buildramdisk -eq 1 ] && echo "Build ramdisk"
        cmd="make -C $MMBUILD -f lbp.mk $target $MAKE_ARGS"
        echo "  $cmd" && [ "$dryrun" -eq "0" ] && eval $cmd
	return
    fi

    if [ ! "$MMT" -a "$script" = "lmm" ]; then
	echo "Not in multimedia folder. Run this script under multimedia tree."
	return
    fi

    # For lmm, detect component path from current dir
    if [ "$script" = "lmm" -a $buildmm -eq 0 ]; then
	cmpdir=$(getmmrel $PWD)
	echo "Component dir=$cmpdir"
    fi

    # For lm, detect component path from argument
    if [ "$script" = "lm" -a $buildmm -eq 0 ]; then
	if [ -d $MMROOT/$cmp ]; then
	    echo "Component parameter $cmp is a correct path in MM tree. Let's use it as it is"
	    cmpdir=$cmp
	else
	    cmpdir=$(getcmpdir $cmp)
	fi
	if [ ! "$cmpdir" ]; then
	    echo "Error: component $cmp not found!"
	    echo "Check your environment or call lmm in revelevant folder instead of using this this script"
	    return
	fi
	echo "Component dir=$cmpdir"
    fi

    # Run build command for selected multimedia stuff
    if [ $buildmm -eq 1 ]; then
	if [[ "$target" == *clean* ]]; then
	    echo "Clean whole multimedia tree"
	else
	    echo "Build whole multimedia tree"
	fi
        cmd="make -C $MMBUILD -f lbp.mk $MAKE_ARGS $target"
    else
        cmd="make -C $MMBUILD -f lbp.mk DIRS_TO_BUILD=\"$cmpdir\" $MAKE_ARGS $target"
    fi
    echo "  $cmd" && [ "$dryrun" -eq "0" ] && eval $cmd

}

_mmx86_main () {

  # Main script for X86 build. Uses script name as parameter: xmm or xm
  local script=$1
  shift

  local target="mm-all"
  local dryrun=0
  MMT=$(getmmtop)
  local MMBUILD=$MMROOT/linux/build

  local cmp=$1
  local isdash=`expr match "$cmp" '-.*'`
  local buildall=0
  local buildmm=0

  # For xm: get component name from parameters
  if [ "$script" = "xm" ]; then
    if [ "${isdash}" != "0" ]; then
      if [ ! -d $(getmmtoprel)/linux/build ]; then
        echo "Error: you must specify component name/path first"
        [ "$script" = "xm" ] && xmusage
        [ "$script" = "xmm" ] && xmmusage
        return
      else
        buildall=1
        target=mm-all
        OPTIND=1
      fi
    else
      OPTIND=2
    fi
  else
    if [ -f product/common/ux500.mak ]; then
	buildall=1
	target=mm-all
    fi
    OPTIND=1
  fi

  # Parse options and rebuild the unkown ones to pass them to make
  local OPT
  local UNKNOWN_ARG=""
  local UNKNOWN_ARG_PRE=" -"
  while getopts ":bcxnht:-:" OPT
  do
    # Get long options with or without value
    [ $OPT = "-" ] && case "${OPTARG%%=*}" in
          build)   OPT="b" ;;
          clean)   OPT="c" ;;
          dryrun)  OPT="n" ;;
          help)    OPT="h" ;;
          target)  OPT="t" ; OPTARG="${OPTARG#*=}" ;;
          *) UNKNOWN_ARG_PRE=" --";;
     esac

     # Get short options with or without value
     case $OPT in
          b) target=$(getmmtarget x86 build $buildall) ;;
          c) target=$(getmmtarget x86 clean $buildall) ;;
          h) [ "$script" = "xm" ] && xmusage; [ "$script" = "xmm" ] && xmmusage; return ;;
          n) echo "dry-run activated" ; dryrun=1 ;;
          t) target=$(getmmtarget x86 $OPTARG $buildall) ;;
          *) UNKNOWN_ARG="${UNKNOWN_ARG}${UNKNOWN_ARG_PRE}${OPTARG}" ; if [ "$UNKNOWN_ARG_PRE" = " --" ] ; then UNKNOWN_ARG_PRE=" -" ; else UNKNOWN_ARG_PRE="" ; fi ;;
      esac
    done

    # Get flags from arguments, expected after options
    if [ $OPTIND -eq 2 ]; then
      shift
    else
      let optshift=$OPTIND-1
      shift $optshift
    fi

    local MAKE_ARGS=$(echo -n "$@" | awk -v RS=" " -v ORS=" " '/^[^-].*$/')
    local arg=
    local cmd=
    local cmpdir=

    MAKE_ARGS="$UNKNOWN_ARG $MAKE_ARGS"

    # Allow STE_PLATFORM to be provided as make argument
    for arg in $MAKE_ARGS ; do
      key=`echo $arg | cut -d '=' -f 1`
      if [ "$key" = "STE_PLATFORM" ] ; then
        STE_PLATFORM=`echo $arg | cut -d '=' -f 2`
      fi
    done

    if [ -z "$STE_PLATFORM" ]; then
      local platform=`echo ${TARGET_PRODUCT:4}`
      if [ ! -z "$platform" ]; then
        echo "found TARGET_PRODUCT=$TARGET_PRODUCT, set STE_PLATFORM to $platform"
        export STE_PLATFORM=$platform
      else
        echo "Error: STE_PLATFORM must be set in your environment, supported values are:"
        echo -n "  "
        cd $MMROOT/linux/build && \ls Dirs.*.mk | sed 's,Dirs\.,  ,g' | sed 's,\.mk,,g' | awk '{ printf "%s  ", $1 }' && cd - > /dev/null
        echo
        return
      fi
    fi

    [ "$script" = "xmm" -a "$PWD" = "$MMBUILD" ] && buildmm=1
    [ "$script" = "xm" -a "$cmp" = "mm" ] && buildmm=1

    if [ $buildall -eq 1 ]; then
      if [[ "$target" == *clean* ]]; then
          echo "Clean everything from top"
      else
	  echo "Build everything from top"
      fi

      cmd="make -C $MMBUILD -f x86.mk $target $MAKE_ARGS"
      echo "  $cmd" && [ "$dryrun" -eq "0" ] && eval $cmd
      return
    fi

    if [ ! "$MMT" -a "$script" = "xmm" ]; then
      echo "Not in multimedia folder. Run this script under multimedia tree."
      return
    fi

    # For xmm, detect component path from current dir
    if [ "$script" = "xmm" -a $buildmm -eq 0 ]; then
      cmpdir=$(getmmrel $PWD)
      echo "Component dir=$cmpdir"
    fi

    # For xm, detect component path from argument
    if [ "$script" = "xm" -a $buildmm -eq 0 ]; then
      if [ -d $MMROOT/$cmp ]; then
        echo "Component parameter $cmp is a correct path in MM tree. Let's use it as it is"
        cmpdir=$cmp
      else
        cmpdir=$(getcmpdir $cmp)
      fi
      if [ ! "$cmpdir" ]; then
        echo "Error: component $cmp not found!"
        echo "Check your environment or call lmm in revelevant folder instead of using this this script"
        return
      fi
      echo "Component dir=$cmpdir"
    fi

    # Run build command for selected multimedia stuff
    if [ $buildmm -eq 1 ]; then
      if [[ "$target" == *clean* ]]; then
	  echo "Clean whole multimedia tree"
      else
	  echo "Build whole multimedia tree"
      fi

      cmd="make -C $MMBUILD -f x86.mk $MAKE_ARGS $target"
    else
      cmd="make -C $MMBUILD -f x86.mk DIRS_TO_BUILD=\"$cmpdir\" $MAKE_ARGS $target"
    fi
    echo "  $cmd" && [ "$dryrun" -eq "0" ] && eval $cmd

}

function clean-git() {
    git clean -xdf && git reset --hard
}

check_packing_usage() {

cat <<EOF

check_packing:
 perform the following tasks to test component packing
  - build for packing: mm -j8 MM_BUILD_FOR_PACKAGING=true VERBOSE=3
  - update of restricted-binary file with component name and optional build entities
  - packing for the component given in parameters: linux/build/preparePackaging.sh
  - clean up of MM build artifacts:
    - removal of mmbuidint,mmbuildout directories
    - clean-up of build artifacts inside file tree for components
      packed as source (not listed in restricted-binary)
  - post build packing: mm -j8 MM_BUILD_FOR_PACKAGING=true VERBOSE=3
 Build logs and result from packing log are generated inside linux/build directory

********************************************************************************
  The script perform several time git reset --hard meaning that all un-stashed
  on un-commited changes will be LOST !!!!
********************************************************************************

usage:

 check_packing [component] [build/packing entities]

example:
  check_packing imaging/ext_sia_hw_components_8500 arm-proxy-bin arm-nmf-bin mmdsp-nmf-bin
EOF
}

function clean_up_mm_tree() {

  if [ "$1" == "" ] ; then
    log_file="/dev/stdout"
  else
    log_file=$1
  fi

  if [ "$2" == "" ] ; then
    bin_file=""
  else
    bin_file="$2"
  fi

  echo "Removing mmbuildint"
  \rm -rvf $ANDROID_PRODUCT_OUT/obj/mmbuildint > $log_file 2>&1
  echo "Removing mmbuildout"
  \rm -rvf $ANDROID_PRODUCT_OUT/obj/mmbuildout > $log_file 2>&1

  echo "Removing build artifacts from MM file tree with log in $log_file"
  for d in `find "$MMROOT" -maxdepth 6 -name ".git" | sort` ; do
    dir=`dirname $d`
    component_dir=`echo $dir | sed "s,\(.*\)\/multimedia\/\(.*\),\2,g"`
    if [ -z $bin_file ] ; then
      match=""
    else
      match=`\grep $component_dir $bin_file`
    fi
    if [ "$match" == "" ] ; then
      cd $dir
      echo "########## Under: $dir" >> $log_file
      clean-git >> $log_file 2>&1
      if [ $? -ne 0 ] ; then
        cd - > /dev/null
        return 1
      fi
      cd - > /dev/null
    fi
  done
}

function clean_mm_prebuilt() {
  MMROOT=$ANDROID_BUILD_TOP/vendor/st-ericsson/multimedia
  find $MMROOT -wholename "*/nmf_repository/*" -exec rm -rv \{} \;
  find $MMROOT -wholename "*/lib/*-rvct4gnu-*/*" -exec rm -rv \{} \;
  ## Restore library not having source under GIT
  cd $MMROOT/imaging/face_detector ; git checkout STI_facedetect/lib/cortexA9-x-rvct4gnu-x/libsti_face_detector.a ; cd - > /dev/null
}

function check_packing() {

  if [ "$1" == "--help" ] ; then
    check_packing_usage
    return
  fi

  if [ "$1" == "" ] ; then
    echo "using existing packing file in $MMROOT/linux/build/restricted-binary"
  else
    git=$MMROOT/$1
    component=$1
    if [ ! -d $git ] ; then
      echo "git: $git does not exist"
      return 1
    fi
    shift
    entities=$@

    echo "updating existing packing file with: $component $entities"
    echo $component $entities > $MMROOT/linux/build/restricted-binary
  fi

  if [ -z "CHECK_FULL_MM_PACKING" ] ; then
    echo "Forcing restricted-not_delivered to be empty"
    echo "" > $MMROOT/linux/build/restricted-not_delivered
  fi

  clean_up_mm_tree $ANDROID_BUILD_TOP/check_packing-initial-clean-git_log.txt

  log_file="$ANDROID_BUILD_TOP/check_packing-build-default_log.txt"
  echo "Building MM with log in $log_file"
  cd $MMROOT/linux/build
  mm -j8 VERBOSE=3 > $log_file 2>&1
  if [ $? -ne 0 ] ; then
    cd - > /dev/null
    echo 'Building MM with with default option failed !!'
    return 1
  fi
  cd - > /dev/null

  # List all generated file inside mmbuildout with standard build
  file_list_normal_build="$ANDROID_BUILD_TOP/check_packing-file_list_source_build.txt"
  echo "Generating list of files in mmbuildout with standard build"
  find -L $ANDROID_PRODUCT_OUT/obj/mmbuildout ! \( -name '*.c' -or -name '*.cpp' \) 2>&1 | sort > $file_list_normal_build 

  clean_up_mm_tree $ANDROID_BUILD_TOP/check_packing-build-default-clean-git_log.txt

  log_file="$ANDROID_BUILD_TOP/check_packing-build-prepacking_log.txt"
  echo "Building MM for packing with log in $log_file"
  cd $MMROOT/linux/build
  mm -j8 MM_BUILD_FOR_PACKAGING=true VERBOSE=3 > $log_file 2>&1
  if [ $? -ne 0 ] ; then
    cd - > /dev/null
    echo 'Building MM for packing failed !!'
    return 1
  fi
  cd - > /dev/null

  log_file="$ANDROID_BUILD_TOP/check_packing-packing_log.txt"
  echo "Packing with log in $log_file"
  cd $ANDROID_BUILD_TOP/vendor/st-ericsson
  $MMROOT/linux/build/preparePackaging.sh > $log_file 2>&1
  if [ $? -ne 0 ] ; then
    cd - > /dev/null
    echo 'Packing failed !!'
    return 1
  fi
  cd - > /dev/null

  log_file="$ANDROID_BUILD_TOP/check_packing-clean-source-git_log.txt"
  echo "Cleaning post packing (only source gits) with log in $log_file"
  clean_up_mm_tree $log_file $MMROOT/linux/build/restricted-binary
  if [ $? -ne 0 ] ; then
    echo 'Cleaning of sources after packing failed !!'
    return 1
  fi

  log_file="$ANDROID_BUILD_TOP/check_packing-build-postpacking_log.txt"
  echo "Building MM after packing with log in $log_file"
  cd $MMROOT/linux/build
  mm -j8 MM_PACKAGE=restricted VERBOSE=3 > $log_file 2>&1
  if [ $? -ne 0 ] ; then
    cd - > /dev/null
    echo 'Building packed MM failed !!'
    return 1
  fi
  cd - > /dev/null

  # List all generated file inside mmbuildout with packed build
  file_list_packed_build="$ANDROID_BUILD_TOP/check_packing-file_list_packed_build.txt"
  echo "Generating list of files in mmbuildout with packed build"
  find -L $ANDROID_PRODUCT_OUT/obj/mmbuildout ! \( -name '*.c' -or -name '*.cpp' \) 2>&1 | sort > $file_list_packed_build

  # Compare the installed trees content
  log_file="$ANDROID_BUILD_TOP/check_packing-diff-out.txt"
  echo "Generating diff of out trees in $log_file"
  diff $file_list_normal_build $file_list_packed_build > $log_file 2>&1
  if [ $? -ne 0 ] ; then
    echo 'WARNING Packing: List of files in mmbuildout are not the same when comparing standard build and packed build'
  else
    echo 'Sucessfull :)'
  fi

  if [ -z "CHECK_FULL_MM_PACKING" ] ; then
    echo "Restoring deleted GITs"
    for d in `cat $MMROOT/linux/build/restricted-not_delivered` ; do
      p=`grep $d\" .repo/manifest.xml | sed -e 's,[^"]*"\([^"]*\)*.*,\1,'`
      cmd="repo sync -l $p"
      echo $cmd ; eval $cmd
    done
  fi
}

if [ ! -f build/envsetup.sh ] ; then
    echo "Error: please source this file from top level of source tree"
else
    # Export useful variables
    export MMROOT=$( cd "$( dirname "$BASH_SOURCE" )/../.." && pwd )
    export LBP_BUILD_TOP=`\pwd`
    if [ ! -z "$STE_PLATFORM" ] ; then
        export LBP_PRODUCT_OUT=$LBP_BUILD_TOP/out/$STE_PLATFORM
    fi
fi
