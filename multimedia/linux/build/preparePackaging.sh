#!/bin/bash
#-------------------------------------------------------------------------------
#                        (C) STEricsson
#    Reproduction and Communication of this document is strictly prohibited 
#          unless specifically authorized in writing by STMicroelectronics.
#-------------------------------------------------------------------------------

echo "Executing: $0 $*"

local_dir=`dirname $0`
compute_size=0
backup=0
DRY_RUN=0

MULTIMEDIA_DIR=multimedia
PACKAGE_NAME=restricted
MULTIMEDIA_PATH=vendor/st-ericsson/multimedia
DEFAULT_ENTITIES_BINARY="arm-proxy-bin,arm-nmf-bin,arm-proc-bin,mmdsp-nmf-bin,mmdsp-proc-bin,xp70-proc-bin"

## Check if some arguments are provided
if [ $# = 2 ] ; then
    MULTIMEDIA_PATH=$1
    PACKAGE_NAME=$2
    MULTIMEDIA_DIR=$MULTIMEDIA_PATH
elif [ $# = 3 ] ; then
    MULTIMEDIA_PATH=$1
    PACKAGE_NAME=$2
    DRY_RUN=$3
    MULTIMEDIA_DIR=$MULTIMEDIA_PATH
else
    if [ $# != 0 ] ; then
        echo "Error: invalid number of arguments"
        echo "Usage: preparePackaging.sh [MULTIMEDIA_PATH PACKAGE_NAME [DRY_RUN]]"
        echo "   Take either no arguments or 2/3 in the given order"
        echo "   Where MULTIMEDIA_PATH is pointing to root of multimedia forest"
        echo "   Where PACKAGE_NAME is the name of the package files prefix to read from"
        echo "   Where DRY_RUN 0/1 allow to perform dry run"
        exit 1
    fi
fi

if [ ! -d $MULTIMEDIA_DIR ] ; then
    echo "Error: $MULTIMEDIA_DIR is not a directory"
    exit 1
fi

## Get ABS path for MULTIMEDIA_DIR
cd $MULTIMEDIA_DIR ; MULTIMEDIA_DIR=`pwd -P` ; cd - > /dev/null

echo "Will prepare for multimedia packing the following:"
echo "  MULTIMEDIA_DIRECTORY=$MULTIMEDIA_DIR"
echo "  DRY_RUN=$DRY_RUN"
echo "  $PACKAGE_NAME-not_delivered : listing directories not to be delivered"
echo "  $PACKAGE_NAME-binary        : listing directories to deliver in binary format"

function execute()
{
  if [ $DRY_RUN -ne 1 ] ; then
    bash -c "$1"
  else
    echo "$1"
  fi

  if [ $? -ne 0 ] ; then
    exit 1
  fi

  return $?

}

function configure_rodos()
{
  # ------------------------------------------------------------
  # Initialize perl and default paths
  # ------------------------------------------------------------

  _RODOS_ROOT_PATH=$local_dir/../../../tools/internal/rodos

  # Check whether perl exists and is in the path
  if [ -z "`which perl 2>/dev/null`" ]; then
    echo
    echo "*** ERROR: perl is required to run rodos tool. You need to add perl in your path ***"
    exit 1;
  fi

  export RODOS_PERL=`which perl`

  # ------------------------------------------------------------
  # Add rodos in the path
  # ------------------------------------------------------------

 _RODOS_INSTALL_PATH=${_RODOS_ROOT_PATH}/bin
  export PATH=${_RODOS_INSTALL_PATH}:${PATH}

  # Check whether rodos is now ok
  rodos --version >/dev/null 2>&1
  if [ $? -ne 0 ] ; then
    echo "*** ERROR: rodos not found ***"
    exit 1
  else
    echo "Info: rodos path: `which rodos`"
  fi

}

################################################################################
# Helper removal function
function remove()
{
  if [ -e $1 ] || [ -L $1 ] ; then
    echo "removing $1"
    ok=0
    if [ -d $1 ] ; then
      if [ $backup = 1 ] ; then
        execute "mkdir -p `dirname backup/$1`"
        execute "tar -c -C `dirname $1` `basename $1` | tar -x -C `dirname backup/$1`"
      fi
      execute "rm -rf $1"
      ok=1
    fi
    if [ -f $1 ] ; then
      if [ $backup = 1 ] ; then
        execute "mkdir -p `dirname backup/$1`"
        execute "cp -f $1 `dirname backup/$1`"
      fi
      execute "rm -f $1"
      ok=1
    fi
    if [ -L $1 ] ; then
      if [ $backup = 1 ] ; then
        execute "mkdir -p `dirname backup/$1`"
        execute "cp -fa $1 `dirname backup/$1`"
      fi
      execute "rm -f $1"
      ok=1
    fi
    if [ $ok -ne 1 ] ; then
      echo "Failed to remove $1"
    fi
  fi
}

function remove_list()
{
  while [ $# != 0 ] ; do
    remove $1
    shift
  done
}

# Default filtering scheme (component not having mmdsp,rvct and xp70 code)
function package_binary_component()
{
  component_name=$1

  ## Clean what should not even be delivered to SI
  LIST="$LIST `find -L $MULTIMEDIA_DIR/$component_name -name '*-rvct-*'`"
  LIST="$LIST `find -L $MULTIMEDIA_DIR/$component_name -name 'streams'`"
  LIST="$LIST `find -L $MULTIMEDIA_DIR/$component_name -name 'mmte_script'`"
  LIST="$LIST `find -L $MULTIMEDIA_DIR/$component_name -name 'scripts'`"
  LIST="$LIST `find -L $MULTIMEDIA_DIR/$component_name -wholename '*nmf_repository/x86*'`"
  LIST="$LIST `find -L $MULTIMEDIA_DIR/$component_name -wholename '*lib/x86*'`"
  LIST="$LIST `find -L $MULTIMEDIA_DIR/$component_name -wholename '*/group/*'`"

  ## Support for V1 not needed any more
  LIST="$LIST `find -L $MULTIMEDIA_DIR/$component_name -wholename '*/nmf_repository/mmdsp_8500/*'`"

  ## Clean video tmp build internal
  LIST="$LIST `find -L $MULTIMEDIA_DIR/$component_name -wholename '*/libtmp/*'`"
  LIST="$LIST `find -L $MULTIMEDIA_DIR/$component_name -wholename '*/libtmp_hdtv/*'`"

  for i in $LIST ; do
    remove $i
  done

  FIND_ARGS="-wholename */nmf_repository/generated/*"
  FIND_ARGS="$FIND_ARGS -o -wholename */autogentraces/*.h"
  FIND_ARGS="$FIND_ARGS -o -wholename */autogentraces/*.done"
  FIND_ARGS="$FIND_ARGS -o -name component.desc"

  PATTERN_LIST="*.c *.html *.cpp *.conf *.bak *~"
  for p in $PATTERN_LIST ; do
    FIND_ARGS="$FIND_ARGS -o -type f -name $p"
  done

  ## remove source files but keep idt and itf files as may be used by components in src
  LIST=`find -L $MULTIMEDIA_DIR/$component_name $FIND_ARGS`
  remove_list $LIST

  # Clean intermediate of build process on all components
  remove_list `find -L $MULTIMEDIA_DIR/$component_name -type d -name _nmf_tmpdir`
  remove_list `find -L $MULTIMEDIA_DIR/$component_name -type d -name dep -o -type d -name obj`
}



# Perform filtering thanks to packaging.xml
function package_binary_component_with_packaging_file()
{

  component_name=$1
  entities=$2

  echo "Info: Component: $component_name Entities: $entities"

  if [ "$entities" != "" ] ; then
    echo "Info: Component: $component_name Aply user defined packaging rules: $entities"
    if [ $DRY_RUN -ne 1 ] ; then
      execute "rodos listpkcontent --pktype=$entities --apply-filter --check $MULTIMEDIA_DIR/$component_name"
    else
      rodos listpkcontent --pktype=$entities --check $MULTIMEDIA_DIR/$component_name
    fi
  else
    echo "Info: Component: $component_name Applying default packaging rules: $DEFAULT_ENTITIES_BINARY"
    if [ $DRY_RUN -ne 1 ] ; then
      execute "rodos listpkcontent --pktype=$DEFAULT_ENTITIES_BINARY --apply-filter --check $MULTIMEDIA_DIR/$component_name"
    else
      rodos listpkcontent --pktype=$DEFAULT_ENTITIES_BINARY --check $MULTIMEDIA_DIR/$component_name
    fi
  fi
}

################################################################################
# Removed completely the directories which are not to be delivered
# ( keep the directory and .git in it but remove everything else )
echo "*********************************************"
echo "Info: Perform filtering for:  $PACKAGE_NAME-not_delivered"
echo "*********************************************"
for d in `cat $local_dir/$PACKAGE_NAME-not_delivered | sed "s,^$MULTIMEDIA_PATH/,,g"`; do
    if [ -d $MULTIMEDIA_DIR/$d ] ; then
	remove $MULTIMEDIA_DIR/$d
    fi
done

################################################################################
# Clean result from install process
echo "*********************************************"
echo "Info: Perform filtering from install process"
echo "*********************************************"
for d in audio imaging linux shared video ; do
    for s in include bin lib idt nmf_repository rootfs install ; do
	if [ -d $MULTIMEDIA_DIR/$d/$s ] ; then
	    remove $MULTIMEDIA_DIR/$d/$s
	fi
    done
done

################################################################################
# Manage mali special case
echo "*********************************************"
echo "Info: Perform filtering for MALI: $PACKAGE_NAME-binary "
echo "*********************************************"
HAS_MALI=`cat $local_dir/$PACKAGE_NAME-binary | sed "s,^$MULTIMEDIA_PATH/,,g" | grep linux/mali400`
if [ "$HAS_MALI" = "linux/mali400" ] ; then
    remove $MULTIMEDIA_DIR/linux/mali400/driver
    remove $MULTIMEDIA_DIR/linux/mali400/.gitignore
fi

################################################################################
# Add rodos in the path
##############################################################################

echo "*********************************************"
echo "Info: Add rodos in the path to support MM packing"
echo "*********************************************"
configure_rodos

################################################################################
# Clean binary packages
##############################################################################
echo "*********************************************"
echo "Info: Perform filtering for:$PACKAGE_NAME-binary "
echo "*********************************************"

while read d ; do

  if [ "`echo $d | grep '#'`" != "" ] ; then
    continue
  fi
  if [ "$d" = "" ] ; then
    continue
  fi

  component_name=`echo $d | awk '{print $1}' | sed "s,^$MULTIMEDIA_PATH/,,g"`
  entities=`echo $d | awk '{for(i=2;i<=NF;i++) if (i==NF) {printf "%s",$i} else {printf "%s,", $i}}'`

  if [ -d $MULTIMEDIA_DIR/$component_name ] ; then
    # Use filtering from rodos with packaging.xml file for component having mmdsp,rvct and xp70 code
    if [ -f "$MULTIMEDIA_DIR/$component_name/component/component.xml" ] ; then
      if [ ! -f "$MULTIMEDIA_DIR/$component_name/component/packaging.xml" ] ; then
        echo "Error: Packaging.xml not found for component $component_name"
        exit 1;
      else
        package_binary_component_with_packaging_file $component_name $entities
      fi
    elif [ -f "$MULTIMEDIA_DIR/$component_name/component/packaging.xml" ] ; then
      package_binary_component_with_packaging_file $component_name $entities
    else
      package_binary_component $component_name
    fi
    echo "gen_mk_rules.pl $component_name"
    perl $MULTIMEDIA_DIR/linux/build/gen_mk_rules.pl $component_name $entities > $MULTIMEDIA_DIR/$component_name/BuildSpec.mk
  fi
done < $local_dir/$PACKAGE_NAME-binary


# Clean result of gathering in linux_ramdisk
echo "*********************************************"
echo "Info: Clean result of gathering in linux_ramdisk"
echo "*********************************************"
remove_list $MULTIMEDIA_DIR/linux/ramdisk/*-android_*


################################################################################
# Remove all empty directories
echo "*********************************************"
echo "Info: Remove all empty directories"
echo "*********************************************"
execute "find $MULTIMEDIA_DIR -depth -type d -a -empty -a -not -wholename \"*/\.git/*\" -a -not -wholename \"*/\.svn/*\" -exec rmdir -v {} \;"

# And restore a directory that must exist, even though it is empty,  
# as make recurses into it  
execute "mkdir -p $MULTIMEDIA_DIR/video/components/h264enc/arm_nmf/proxynmf"

