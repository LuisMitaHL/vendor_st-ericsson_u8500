#!/bin/sh
# © Copyright ST-Ericsson 2010. All rights reserved.

KERNELPATH=$1
REFMAN_OUT_DIR=$2
OUTDIR=$3
WERROR=$4
FINDSTRING="ST-Ericsson\|STMicroelectronics"
APITOCXSD=../templates/API_Toc.xsd

if [ -z "$KERNELPATH" ]
then
  SHOWPARAMS=yes
fi

if [ -z "$OUTDIR" ]
then
  SHOWPARAMS=yes
fi

if [ -n "$SHOWPARAMS" ]
then
    echo $0: Usage:
    echo "  $0 path_to_kernel path_to_outdirectory [--ignore-errors]"
    echo
    echo "  Obtains the meta-data for all kernel DocBook .tmpl files"
    echo "  that contains the string " '"'$FINDSTRING'"' "as the <holder> tag."
    echo "  The <subjectterm> tag in the .tmpl file defines which part of the"
    echo "  reference manual the module shall be contained within."
    echo "  The $APITOCXSD file defines the possible contents in the"
    echo "  <subjectterm> tag:"
    if [ -f "$APITOCXSD" ]
    then
       grep enumeration $APITOCXSD | grep -v "User space" | grep -v "Kernel space"
    fi
    echo
    echo "  All other .tmpl files are considered linux standard."
    exit 1
fi

if [ "$WERROR" = "--ignore-errors" ]
then
  ABORT_ON_ERRORS=
else
  ABORT_ON_ERRORS=yes
fi

if [ ! -d "$OUTDIR" ]
then
  echo $0: $OUTDIR: Not a directory
  exit 1
fi

# Parameters are ok.

FILELIST=`grep -l $FINDSTRING $KERNELPATH/Documentation/DocBook/*.tmpl`
NON_STE_LIST=`grep -L $FINDSTRING $KERNELPATH/Documentation/DocBook/*.tmpl`

for FILENAME in $FILELIST
do
  THEBASENAME=${FILENAME%.tmpl}
  THESHORTNAME=${THEBASENAME#$KERNELPATH/Documentation/DocBook/}
  TMPNAME=$THEBASENAME.tmp
  XMLNAME=$THEBASENAME.xml
  TOCLOCATION=$THEBASENAME-toc-locations.xml

  java -cp $REFMAN_OUT_DIR/RefMan.jar com.stericsson.RefMan.TemplateComparator.KernelTmplVerifier $FILENAME
  exitValue=$?

  if [ -n "$ABORT_ON_ERRORS" ]
  then
    if [ $exitValue != "0" ]
    then
	  exit 1
    fi
  fi

  if [ -f $XMLNAME ]
  then

    # Extract the bookinfo, removing any comments tag:
	# using oneliner, first strip comments, then extract bookinfo only
    sed < $FILENAME  > $TMPNAME.10 's,<!--\(.*\)-->,,;/<\!--/,/-->/d'
    INFONAME=$TMPNAME.11
    sed < $TMPNAME.10 > $INFONAME -n '/bookinfo/,/\/bookinfo/p'

    #Extract title tag, from bookinfo
    TITLE=`sed < $INFONAME -n -e 's,<title>\(.*\)</title>,\1,p' | head -1 | sed 's/^[ \t]*//;s/[ \t]*$//;s/[ \r]*$//' `
    if [ -z "$TITLE" ]
    then
      # Attempt to extract title from articleinfo instead
      INFONAME=$TMPNAME.12
      sed < $TMPNAME.10 > $INFONAME -n '/articleinfo/,/\/articleinfo/p'
      TITLE=`sed < $INFONAME -n -e 's,<title>\(.*\)</title>,\1,p' | head -1 | sed 's/^[ \t]*//;s/[ \t]*$//;s/[ \r]*$//' `
      if [ -z "$TITLE" ]
      then
        echo $FILENAME: 'error: <title> tags missing or empty!' 1>&2
        if [ -n "$ABORT_ON_ERRORS" ]
        then
          exit 1
        else
          TITLE='Undefined title'
        fi
      fi
    fi

    #Extract holder tag
    HOLDER=`sed < $INFONAME -n -e 's,<holder>\(.*\)</holder>,\1,p' | head -1 | sed 's/^[ \t]*//;s/[ \t]*$//;s/[ \r]*$//' `
    #echo Obtaining meta data for '"'$TITLE'"', found holder '"'$HOLDER'"'
	
    #Extract subjectterm tag
    AREA=`sed < $INFONAME -n -e 's,<subjectterm>\(.*\)</subjectterm>,\1,p' | head -1 | sed 's/^[ \t]*//;s/[ \t]*$//;s/[ \r]*$//' `
    if [ -z "$AREA" ]
    then
      echo $FILENAME: 'error: <subjectterm> tags missing or empty!' 1>&2
      if [ -n "$ABORT_ON_ERRORS" ]
      then
        echo "  The $APITOCXSD file defines the possible contents in the"   1>&2
        echo "  <subjectterm> tag:" 1>&2
        if [ -f "$APITOCXSD" ]
        then
         grep enumeration $APITOCXSD | grep -v "User space" | grep -v "Kernel space"
        fi
        exit 1
      else
        AREA='Undefined area'
      fi
    fi

    echo >  $TOCLOCATION '<?xml version="1.0" encoding="utf-8"?>'
    echo >> $TOCLOCATION '<toc xmlns="http://www.stericsson.com/refman/API_Toc.xsd" label="API">'
    echo >> $TOCLOCATION '  <topic label="'$AREA'">'
    echo >> $TOCLOCATION '    <topic label="Kernel space">'
    echo >> $TOCLOCATION '      <topic label="'$TITLE'" href="'html/$THESHORTNAME/index.html'"/>'
    echo >> $TOCLOCATION '    </topic>'
    echo >> $TOCLOCATION '  </topic>'
    echo >> $TOCLOCATION '</toc>'
    mv -f $TOCLOCATION $OUTDIR
    rm -f $TMPNAME.*
  else
    echo $FILENAME ': warning: Has not been compiled - please check Documentation/DocBook/Makefile'
  fi
done

#
# Extract non-ST Ericsson documents
#
for FILENAME in $NON_STE_LIST
do
  THEBASENAME=${FILENAME%.tmpl}
  THESHORTNAME=${THEBASENAME#$KERNELPATH/Documentation/DocBook/}
  TMPNAME=$THEBASENAME.tmp
  XMLNAME=$THEBASENAME.xml
  TOCLOCATION=$THEBASENAME-toc-locations.xml

  if [ -f $XMLNAME ]
  then

    # Extract the bookinfo, removing any comments tag:
	# using oneliner, first strip comments, then extract bookinfo only
    sed < $FILENAME  > $TMPNAME.10 's,<!--\(.*\)-->,,;/<\!--/,/-->/d'
    INFONAME=$TMPNAME.11
    sed < $TMPNAME.10 > $INFONAME -n '/bookinfo/,/\/bookinfo/p'

    #Extract title tag, from bookinfo
    TITLE=`sed < $INFONAME -n -e 's,<title>\(.*\)</title>,\1,p' | head -1 | sed 's/^[ \t]*//;s/[ \t]*$//;s/[ \r]*$//' `
    if [ -z "$TITLE" ]
    then
      echo $FILENAME: 'warning: <title> tags missing or empty, attempting articleinfo!'
      # Attempt to extract title from articleinfo instead
      INFONAME=$TMPNAME.12
      sed < $TMPNAME.10 > $INFONAME -n '/articleinfo/,/\/articleinfo/p'
      TITLE=`sed < $INFONAME -n -e 's,<title>\(.*\)</title>,\1,p' | head -1 | sed 's/^[ \t]*//;s/[ \t]*$//;s/[ \r]*$//' `
      if [ -z "$TITLE" ]
      then
        echo $FILENAME: 'error: <title> tags missing or empty!'
        if [ -n "$ABORT_ON_ERRORS" ]
        then
          exit 1
        else
          TITLE=$THESHORTNAME
        fi
      fi
    fi

    echo >  $TOCLOCATION '<?xml version="1.0" encoding="utf-8"?>'
    echo >> $TOCLOCATION '<toc xmlns="http://www.stericsson.com/refman/API_Toc.xsd" label="API">'
    echo >> $TOCLOCATION '  <topic label="Linux standard functions">'
    echo >> $TOCLOCATION '    <topic label="Kernel space">'
    echo >> $TOCLOCATION '      <topic label="'$TITLE'" href="'html/$THESHORTNAME/index.html'"/>'
    echo >> $TOCLOCATION '    </topic>'
    echo >> $TOCLOCATION '  </topic>'
    echo >> $TOCLOCATION '</toc>'
    mv -f $TOCLOCATION $OUTDIR
    rm -f $TMPNAME.*
  else
    echo $FILENAME ': warning: Has not been compiled - please check Documentation/DocBook/Makefile'
  fi
done

