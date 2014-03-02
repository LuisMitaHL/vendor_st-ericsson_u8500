#!/bin/bash

compute_diff=0
compute_mmdsp=1
compute_rvct=1

script_name=`basename $0`

function my-help()
{
    echo "Usage: $script_name [OPTIONS]"
    echo
    echo "  OPTIONS"
    echo "    --help     : this help"
    echo "    --rvct     : process RVCT binaries only"
    echo "    --mmdsp    : process MMDSP binaries only"
    echo "    --diff     : generate the diff on the binaries"
}

## Process arguments
while true; do
    if [ -z $1 ] ; then break ; fi
    case $1 in
        --mmdsp)
            compute_rvct=0
            ;;
        --rvct)
            compute_mmdsp=0
            ;;
        --diff)
            compute_diff=1
            ;;
        --help)
            my-help
            exit 0
            ;;
        *)
            echo "Error: unknown option '$1'"
            my-help
            exit 1
            ;;
    esac
    shift
done

################################################################################

## Default diff args
if [ -z $DIFF_ARGS ] ; then
    #DIFF_ARGS="-Naur"
    DIFF_ARGS="-y -W 150"
fi

## multimedia directory
MM_DIR=`dirname $0`
MM_DIR=`cd $MM_DIR/../.. ; pwd`

echo "Cleaning up MM source tree : $MM_DIR"

## Find the gits in multimedia
MM_GITS=`find $MM_DIR -maxdepth 6 -name ".git" | sort | sed 's,/.git,,g'`

dir=" "
print_dirname=0

MMDSP_OBJDUMP=vendor/st-ericsson/tools/host/mmdsp/bin/Linux/mmdsp-objdump
ARM_FROMELF=vendor/st-ericsson/tools/host/rvct/Programs/4.0/650/linux-pentium/fromelf

if [ ! -e $MMDSP_OBJDUMP ] ; then
    echo "Error: failed to find $MMDSP_OBJDUMP"
    echo "       script must be executed from ANDROID_BUILD_TOP"
fi

if [ ! -e $ARM_FROMELF ] ; then
    echo "Error: failed to find $ARM_FROMELF"
    echo "       script must be executed from ANDROID_BUILD_TOP"
fi

MMDSP_OBJDUMP=`pwd`/$MMDSP_OBJDUMP
ARM_FROMELF=`pwd`/$ARM_FROMELF

ARM_FROMELF_ARGS="--nodebug --text -c -t --ignore_section=.comment"

################################################################################

function maybe-print-dirname() {
    if [ $print_dirname -eq 1 ] ; then
        echo "################################################################################"
        echo "## Under "`echo $dir | sed "s,$ANDROID_BUILD_TOP/,,"`
    fi
    print_dirname=0
}

################################################################################
# main

function diff-elf4nmf() {
    elf4nmf_file=$1
    elf_file=`echo $elf4nmf_file | sed 's,\.elf4nmf$,\.elf,g'`

    if [ "$elf_file" = "$elf4nmf_file" ] ; then
        ## Not an elf4nmf file
        return
    fi

    diff_file=`echo $elf4nmf_file | sed 's,\.elf4nmf$,\.elf.diff,g'`
    diff_4nmf=`echo $elf4nmf_file | sed 's,\.elf4nmf$,\.elf4nmf.diff,g'`

    head_elf=`mktemp`
    head_elf_objdump=`mktemp`
    curr_elf_objdump=`mktemp`

    head_4nmf=`mktemp`
    head_4nmf_hex=`mktemp`
    curr_4nmf_hex=`mktemp`

    ## Extract head version to tmp file
    git archive --format=tar HEAD $elf_file | tar -xO $elf_file > $head_elf
    git archive --format=tar HEAD $elf4nmf_file | tar -xO $elf4nmf_file > $head_4nmf

    ## Disassemble ELF versions
    $MMDSP_OBJDUMP -D $head_elf > $head_elf_objdump
    $MMDSP_OBJDUMP -D $elf_file > $curr_elf_objdump

    ## Remove offsets to reduce diff
    sed -r -i -e 's,^ *....:,    :,g' -e 's,[0-9a-f]{16},,g' $head_elf_objdump
    sed -r -i -e 's,^ *....:,    :,g' -e 's,[0-9a-f]{16},,g' $curr_elf_objdump

    diff $DIFF_ARGS $head_elf_objdump $curr_elf_objdump > $diff_file
    echo "        elf diff to HEAD : $dir/$diff_file"

    ## Hexdump on the elf4nmf
    hexdump -C $elf4nmf_file > $curr_4nmf_hex
    hexdump -C $head_4nmf > $head_4nmf_hex
    diff $DIFF_ARGS $curr_4nmf_hex $head_4nmf_hex > $diff_4nmf
    echo "    elf4nmf diff to HEAD : $dir/$diff_4nmf"

    ## Cleanup
    rm -f $head_elf $head_elf_objdump $curr_elf_objdump $head_4nmf $head_4nmf_hex $curr_4nmf_hex 
}

function process-files-in-dir() {
    dir=$1
    print_dirname=1

    cd $dir

    ################################################################################
    # MMDSP
    if [ "$compute_mmdsp" = "1" ] ; then
        modified_mpc=`git status | grep 'modified:' | grep 'repository/mmdsp' | sed -r 's,\s\s*, ,g' | cut -d ' ' -f 3`
        modified_elf4nmf=`echo $modified_mpc | sed 's,[^ ]*\.elf ,,g' | sed 's,[^ ]*\.elf$,,g'`
        ## changed_elf: elf files changed because elf4nmf have changed
        changed_elf=`echo $modified_elf4nmf | sed 's,\.elf4nmf,\.elf,g'`
        modified_elf=`echo $modified_mpc | sed 's,[^ ]*\.elf4nmf,,g'`
        ## elf_to_revert: elf files to be reverted because elf4nmf have not changed
        elf_to_revert=$modified_elf

        for f in $changed_elf ; do
            elf_to_revert=`echo $elf_to_revert | sed "s,$f,,g"`
        done

        for f in $elf_to_revert ; do
            maybe-print-dirname
            echo "  Reverting unchanged    : $dir/$f"
            git checkout $f
        done

        for f in $modified_elf4nmf ; do
            maybe-print-dirname
            echo "  Keeping changed        : $dir/$f"
            if [ "$compute_diff" = "1" ] ; then
                diff-elf4nmf $f
            fi
        done
    fi

    ################################################################################
    # RVCT
    if [ "$compute_rvct" = "1" ] ; then
        modified_rvct=`git status | grep 'modified:' | grep -- '-rvct4gnu-' | sed -r 's,\s\s*, ,g' | cut -d ' ' -f 3`
        head=`mktemp`
        head_fromelf=`mktemp`
        curr_fromelf=`mktemp`
        tmp=`mktemp`

        for f in $modified_rvct ; do
            maybe-print-dirname

            # Extract HEAD version
            git archive --format=tar HEAD $f | tar -xO $f > $head
            # Strip both versions
            $ARM_FROMELF $ARM_FROMELF_ARGS $head > $head_fromelf 2>/dev/null
            $ARM_FROMELF $ARM_FROMELF_ARGS $f > $curr_fromelf 2>/dev/null

            # Remove information known to be different without impacting behavior
            sed -i -e 's,File Name:.*,,g' -e 's,Section header offset:.*,,g' $head_fromelf $curr_fromelf
            for e in $head_fromelf $curr_fromelf ; do
                awk 'BEGIN { i=1 ; c=0 ; s=0 } /.*\.shstrtab.*/ { c=1 } /.*\.strtab.*/ { c=1 } /.*\.comment.*/ { c=1 } /\*\* Section/ { s=1 } ; { if (c && s) { i = 0 } else { if (s) { i=1 } } ; if (i) { print $0; } } ; c=0 ; s=0 ' $e > $tmp
                mv $tmp $e
            done

            # Binary diff the 2 version
            diff $DIFF_ARGS $head_fromelf $curr_fromelf > $f.diff
            if [ "$?" = "1" ] ; then
                echo "  Keeping changed        : $dir/$f"
                echo "            diff to HEAD : $dir/$f.diff"
            else
                echo "  Reverting unchanged    : $dir/$f"
                git checkout $f
                rm $f.diff
            fi
        done
        # Cleanup
        rm -f $head $head_fromelf $curr_fromelf $tmp
    fi

    cd - > /dev/null
}

################################################################################
# main

for d in $MM_GITS ; do
    process-files-in-dir $d
done

