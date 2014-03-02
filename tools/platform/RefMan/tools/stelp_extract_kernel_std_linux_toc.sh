#!/bin/sh
# © Copyright ST-Ericsson 2010. All rights reserved.

KERNELPATH=$1
PROJECTROOTDIR=$2
OUTDIR=$3
WERROR=$4
FINDSTRING="ST-Ericsson\|STMicroelectronics"
APITOCXSD=../templates/API_Toc.xsd
KERNEL_VERSION=2.6.x

if [ -z "$KERNELPATH" ]
then
  SHOWPARAMS=yes
fi

if [ -z "$OUTDIR" ]
then
  SHOWPARAMS=yes
fi

if [ -z "$PROJECTROOTDIR" ]
then
  SHOWPARAMS=yes
fi

if [ -n "$SHOWPARAMS" ]
then
    echo $0: Usage:
    echo "  $0 path_to_kernel path_to_projectrootdir path_to_outdirectory [--ignore-errors]"
    echo
    echo "  Extracts the free-text documentation from the linux kernel and produces"
    echo "  a standard-api-style toc-locations.xml file, containing it."
    echo "  An docbook index.xml file is also produced, containing references"
	echo "  to all the files."
    echo "  Source codes, images and shell scripts are excluded."
    echo "  Due to the RefMan tools dependency on git, path_to_outdirectory must"
    echo "  be a location relative the path_to_projectrootdir such that it could"
    echo "  be mistaken for being a git - i.e. it cannot be located in the out folder,"
    echo "  unless it is specified to be found recursively."
    echo "  Thus, all documentation in the Documentation folder of the kernel"
    echo "  will appear to the Java program as being documents belonging to a"
    echo "  standardized api, which is located in a different git than the implementation."

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

# File to exclude
EXCLUDEFILES="Makefile\|.*\.c\|.*\.h\|.*\.sh\|.*\.gif\|.*\.svg\|.*\.jpg\|\.gitignore"

# List fo all files and directories in the Documentation directory
STD_LINUX_ALL=`ls -1 $KERNELPATH/Documentation/ | grep -v $EXCLUDEFILES`

# File containing a list of directories in the Documentation directory
TEMP_DIR_LIST=$OUTDIR/stelp_extract_kernel_toc_dir_list.txt
#echo $TEMP_DIR_LIST TEMP_DIR_LIST

# File containing a list of files in a directory
TEMP_FILE_LIST=$OUTDIR/stelp_extract_kernel_toc_file_list.txt
#echo $TEMP_FILE_LIST TEMP_FILE_LIST

# Name of the toc-locations.xml file to generate
TOC_LOCATIONS_NAME=toc-locations.xml
#echo $TOC_LOCATIONS_NAME TOC_LOCATIONS_NAME

# Name of the docbook file to generate
DOCBOOK_NAME=std-linux-kernel-documentation-index.xml
#echo $DOCBOOK_NAME DOCBOOK_NAME

# Full path to the toc-locations.xml file to generate
STD_LINUX_TOC_LOCATIONS=$OUTDIR/$TOC_LOCATIONS_NAME
#echo $STD_LINUX_TOC_LOCATIONS STD_LINUX_TOC_LOCATIONS

# Full path to the docbook file to generate
STD_LINUX_DOCBOOK=$OUTDIR/Documentation/$DOCBOOK_NAME
#echo $STD_LINUX_DOCBOOK STD_LINUX_DOCBOOK

# KERNELDIR: KERNELPATH converted to a path relative to PROJECTROOTDIR:
KERNELABSDIR=`cd $KERNELPATH;pwd`
KERNELDIR=`basename ${KERNELABSDIR#$PROJECTROOTDIR}`
#echo $KERNELABSDIR KERNELABSDIR
#echo $KERNELDIR KERNELDIR

if [ ! -d $PROJECTROOTDIR/$KERNELDIR ]
then
  KERNELDIR=linux
fi

# DOCUMENTATIONDIR: OUTDIR converted to a path relative to PROJECTROOTDIR:
DOCUMENTATIONABSDIR=`cd $OUTDIR;pwd`
DOCUMENTATIONDIR=`basename ${DOCUMENTATIONABSDIR#$PROJECTROOTDIR}`
#echo $DOCUMENTATIONABSDIR DOCUMENTATIONABSDIR
#echo $DOCUMENTATIONDIR DOCUMENTATIONDIR


#
# Ensure directories and temp files are ok
#
mkdir -p $OUTDIR/Documentation
if [ -f $TEMP_DIR_LIST ]
then
  rm -f $TEMP_DIR_LIST
fi
echo > $TEMP_DIR_LIST

if [ -f $TEMP_FILE_LIST ]
then
  rm -f $TEMP_FILE_LIST
fi
echo > $TEMP_FILE_LIST
rm -f $STD_LINUX_DOCBOOK
rm -f $STD_LINUX_TOC_LOCATIONS

#
# Generate the temp files
#

for FILENAME in $STD_LINUX_ALL
do
  if [ -d $KERNELPATH/Documentation/$FILENAME ]
  then
    echo $FILENAME >> $TEMP_DIR_LIST
  else
    echo $FILENAME >> $TEMP_FILE_LIST
  fi
done

# List fo all documents in the Documentation directory
STD_LINUX_DOCS=`cat $TEMP_FILE_LIST`

#
# Generate the doc-book and toc-locations.xml file
#

# doc-book header

echo >>$STD_LINUX_DOCBOOK '<?xml version="1.0" encoding="UTF-8"?>'
echo >>$STD_LINUX_DOCBOOK '<!DOCTYPE book PUBLIC "-//OASIS//DTD DocBook XML V4.5//EN"'
echo >>$STD_LINUX_DOCBOOK '"http://www.oasis-open.org/docbook/xml/4.5/docbookx.dtd">'
echo >>$STD_LINUX_DOCBOOK '<book id="std-linux-kernel-documentation">'
echo >>$STD_LINUX_DOCBOOK '  <bookinfo>'
echo >>$STD_LINUX_DOCBOOK '    <title>Linux Kernel Documentation</title>'
echo >>$STD_LINUX_DOCBOOK '    <copyright>'
echo >>$STD_LINUX_DOCBOOK '     <year>2010</year>'
echo >>$STD_LINUX_DOCBOOK '     <holder>ST-Ericsson AB</holder>'
echo >>$STD_LINUX_DOCBOOK '    </copyright>'
echo >>$STD_LINUX_DOCBOOK '    <legalnotice>'
echo >>$STD_LINUX_DOCBOOK '      <para>'
echo >>$STD_LINUX_DOCBOOK '        This documentation is free software; you can redistribute'
echo >>$STD_LINUX_DOCBOOK '        it and/or modify it under the terms of the GNU General Public'
echo >>$STD_LINUX_DOCBOOK '        License as published by the Free Software Foundation; either'
echo >>$STD_LINUX_DOCBOOK '        version 2 of the License, or (at your option) any later'
echo >>$STD_LINUX_DOCBOOK '        version.'
echo >>$STD_LINUX_DOCBOOK '      </para>'
echo >>$STD_LINUX_DOCBOOK '      <para>'
echo >>$STD_LINUX_DOCBOOK '        This program is distributed in the hope that it will be'
echo >>$STD_LINUX_DOCBOOK '        useful, but WITHOUT ANY WARRANTY; without even the implied'
echo >>$STD_LINUX_DOCBOOK '        warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.'
echo >>$STD_LINUX_DOCBOOK '        See the GNU General Public License for more details.'
echo >>$STD_LINUX_DOCBOOK '      </para>'
echo >>$STD_LINUX_DOCBOOK '      <para>'
echo >>$STD_LINUX_DOCBOOK '        You should have received a copy of the GNU General Public'
echo >>$STD_LINUX_DOCBOOK '        License along with this program; if not, write to the Free'
echo >>$STD_LINUX_DOCBOOK '        Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,'
echo >>$STD_LINUX_DOCBOOK '        MA 02111-1307 USA'
echo >>$STD_LINUX_DOCBOOK '      </para>'
echo >>$STD_LINUX_DOCBOOK '      <para>'
echo >>$STD_LINUX_DOCBOOK '        For more details see the file COPYING in the source'
echo >>$STD_LINUX_DOCBOOK '        distribution of Linux.'
echo >>$STD_LINUX_DOCBOOK '      </para>'
echo >>$STD_LINUX_DOCBOOK '    </legalnotice>'
echo >>$STD_LINUX_DOCBOOK '  </bookinfo>'
echo >>$STD_LINUX_DOCBOOK '  <toc></toc>'
echo >>$STD_LINUX_DOCBOOK '  <chapter id="intro">'
echo >>$STD_LINUX_DOCBOOK '    <title>Introduction</title>'
echo >>$STD_LINUX_DOCBOOK '    <para>'
echo >>$STD_LINUX_DOCBOOK '      This documentation is automatically extracted'
echo >>$STD_LINUX_DOCBOOK '      from the Documentation directory in the Linux Kernel.'
echo >>$STD_LINUX_DOCBOOK '    </para>'
echo >>$STD_LINUX_DOCBOOK '  </chapter>'
echo >>$STD_LINUX_DOCBOOK '  <chapter id="gettingstarted">'
echo >>$STD_LINUX_DOCBOOK '    <title>Getting Started</title>'
echo >>$STD_LINUX_DOCBOOK '    <para>'
echo >>$STD_LINUX_DOCBOOK '      See the "Getting Started with Android and Linux Environments, USER GUIDE".'
echo >>$STD_LINUX_DOCBOOK '    </para>'
echo >>$STD_LINUX_DOCBOOK '  </chapter>'
echo >>$STD_LINUX_DOCBOOK '  <chapter id="concepts">'
echo >>$STD_LINUX_DOCBOOK '    <title>Concepts</title>'
echo >>$STD_LINUX_DOCBOOK '      <section id="concepts-Documentation">'
echo >>$STD_LINUX_DOCBOOK '        <title>Documentation</title>'
echo >>$STD_LINUX_DOCBOOK '        <para>'
echo >>$STD_LINUX_DOCBOOK '          <itemizedlist>'

#
# toc-locations header
#

echo >>$STD_LINUX_TOC_LOCATIONS '<?xml version="1.0" encoding="utf-8"?>'
echo >>$STD_LINUX_TOC_LOCATIONS '<toc xmlns="http://www.stericsson.com/refman/API_Toc.xsd" label="API">'
echo >>$STD_LINUX_TOC_LOCATIONS '  <topic label="Linux standard functions">'
echo >>$STD_LINUX_TOC_LOCATIONS '    <topic label="Kernel space">'
echo >>$STD_LINUX_TOC_LOCATIONS '      <standardapi title="Linux Kernel Documentation"'
echo >>$STD_LINUX_TOC_LOCATIONS '        modulename="std-linux-kernel-documentation"'
echo >>$STD_LINUX_TOC_LOCATIONS '        modulepath="Documentation"'
echo >>$STD_LINUX_TOC_LOCATIONS '        implementationpath="'$KERNELDIR'"'
echo >>$STD_LINUX_TOC_LOCATIONS '        documentation="Documentation/'$DOCBOOK_NAME'">'

#
# now iterate everything, first files in docbook...
#

for FILENAME in $STD_LINUX_DOCS
do
  echo >>$STD_LINUX_DOCBOOK '            <listitem><para><ulink url="'$FILENAME'">'$FILENAME'</ulink></para></listitem>'
done

#
# docbook separateor between files and sections of directories
#

echo >>$STD_LINUX_DOCBOOK '          </itemizedlist>'
echo >>$STD_LINUX_DOCBOOK '        </para>'
echo >>$STD_LINUX_DOCBOOK '      </section>'

#
# now iterate every directory, for docbook and toc-locations...
#

for DIRNAME in `cat $TEMP_DIR_LIST`
do
  if [ ! "$DIRNAME" = "DocBook" ]
  then
    echo >> $STD_LINUX_TOC_LOCATIONS '        <includedirectory name="'$DIRNAME'" destination="'$DIRNAME'"/>'

    STD_LINUX_DOCS_IN_DIR=`cd $KERNELPATH/Documentation/$DIRNAME; ls -1 | grep -v $EXCLUDEFILES`
    DOC_OK="no"
    for FILENAME in $STD_LINUX_DOCS_IN_DIR
    do
      if [ "$DOC_OK" = "no" ]
      then
        echo >>$STD_LINUX_DOCBOOK '      <section id="concepts-Documentation-'$DIRNAME'">'
        echo >>$STD_LINUX_DOCBOOK '        <title>Documentation/'$DIRNAME'</title>'
        echo >>$STD_LINUX_DOCBOOK '        <para>'
        echo >>$STD_LINUX_DOCBOOK '          <itemizedlist>'
      fi

      if [ -f $KERNELPATH/Documentation/$DIRNAME/$FILENAME ]
      then
        echo >> $STD_LINUX_DOCBOOK '              <listitem><para><ulink url="'$DIRNAME/$FILENAME'">'$FILENAME'</ulink></para></listitem>'
	  else
	    ALLFILES=`cd $KERNELPATH/Documentation/$DIRNAME; find $FILENAME -iname "*" | grep -v $EXCLUDEFILES`
		for AFILE in $ALLFILES
		do
		  if [ -d $KERNELPATH/Documentation/$DIRNAME/$AFILE ]
		  then
		    echo >> $STD_LINUX_DOCBOOK '              <listitem><para><emphasis role="strong">'$AFILE'</emphasis></para></listitem>'
		  else
            echo >> $STD_LINUX_DOCBOOK '              <listitem><para><ulink url="'$DIRNAME/$AFILE'">'$AFILE'</ulink></para></listitem>'
          fi
		done
      fi
      DOC_OK="yes"
    done
    if [ "$DOC_OK" = "yes" ]
    then
      echo >>$STD_LINUX_DOCBOOK '          </itemizedlist>'
      echo >>$STD_LINUX_DOCBOOK '        </para>'
      echo >>$STD_LINUX_DOCBOOK '      </section>'
    fi

  else
    echo >> $STD_LINUX_TOC_LOCATIONS '        <!-- DocBook directory is not handled by this script -->'
  fi
done

#
# output files to toc-locations
#

for FILENAME in $STD_LINUX_DOCS
do
  echo >> $STD_LINUX_TOC_LOCATIONS '        <includefile name="'$FILENAME'" destination=""/>'
done

#
# toc-locations footer
#

echo >>$STD_LINUX_TOC_LOCATIONS '      </standardapi>'
echo >>$STD_LINUX_TOC_LOCATIONS '    </topic>'
echo >>$STD_LINUX_TOC_LOCATIONS '  </topic>'
echo >>$STD_LINUX_TOC_LOCATIONS '</toc>'

#
# Docbook footer
#

echo >>$STD_LINUX_DOCBOOK '  </chapter>'

echo >>$STD_LINUX_DOCBOOK '  <chapter id="conformance-description">'
echo >>$STD_LINUX_DOCBOOK '    <title>Conformance Description</title>'
echo >>$STD_LINUX_DOCBOOK '     <section id="conformance-description-intro">'
echo >>$STD_LINUX_DOCBOOK '       <title>Introduction</title>'

echo >>$STD_LINUX_DOCBOOK '       <para>'
echo >>$STD_LINUX_DOCBOOK '	     See the'
echo >>$STD_LINUX_DOCBOOK '	     "Implementation Conformance Statement, ST-Ericsson Mobile Platform, TEST REPORT"'
echo >>$STD_LINUX_DOCBOOK '         for details on conformity to the Linux Kernel.'
echo >>$STD_LINUX_DOCBOOK '       </para>'
echo >>$STD_LINUX_DOCBOOK '     </section>'

echo >>$STD_LINUX_DOCBOOK '     <section id="conformance-description-references">'
echo >>$STD_LINUX_DOCBOOK '       <title>References</title>'
echo >>$STD_LINUX_DOCBOOK '          <para>'
echo >>$STD_LINUX_DOCBOOK '            <informaltable>'
echo >>$STD_LINUX_DOCBOOK '                <tgroup cols="2">'
echo >>$STD_LINUX_DOCBOOK '                  <thead>'
echo >>$STD_LINUX_DOCBOOK '                    <row>'
echo >>$STD_LINUX_DOCBOOK '                      <entry>Title</entry>'
echo >>$STD_LINUX_DOCBOOK '                      <entry>Web Link</entry>'
echo >>$STD_LINUX_DOCBOOK '                    </row>'
echo >>$STD_LINUX_DOCBOOK '                  </thead>'
echo >>$STD_LINUX_DOCBOOK '                  <tbody>'
echo >>$STD_LINUX_DOCBOOK '					<row>'
echo >>$STD_LINUX_DOCBOOK '                      <entry>The Linux Kernel Archives</entry>'

echo >>$STD_LINUX_DOCBOOK '                      <entry><ulink'
echo >>$STD_LINUX_DOCBOOK '                        url="http://www.kernel.org/">www.kernel.org'
echo >>$STD_LINUX_DOCBOOK '                      </ulink></entry>'
echo >>$STD_LINUX_DOCBOOK '                    </row>'

echo >>$STD_LINUX_DOCBOOK '                  </tbody>'
echo >>$STD_LINUX_DOCBOOK '                </tgroup>'
echo >>$STD_LINUX_DOCBOOK '            </informaltable>'
echo >>$STD_LINUX_DOCBOOK '          </para>'
echo >>$STD_LINUX_DOCBOOK '        </section>'
echo >>$STD_LINUX_DOCBOOK '        <section id="conformance-description-function-conformance">'
echo >>$STD_LINUX_DOCBOOK '            <title>Function Conformance</title>'
echo >>$STD_LINUX_DOCBOOK '            <informaltable>'
echo >>$STD_LINUX_DOCBOOK '                <tgroup cols="3">'
echo >>$STD_LINUX_DOCBOOK '                  <thead>'
echo >>$STD_LINUX_DOCBOOK '                    <row>'
echo >>$STD_LINUX_DOCBOOK '                      <entry>Functions</entry>'
echo >>$STD_LINUX_DOCBOOK '                      <entry>Header File</entry>'
echo >>$STD_LINUX_DOCBOOK '                      <entry>Description/Conformance</entry>'
echo >>$STD_LINUX_DOCBOOK '                    </row>'
echo >>$STD_LINUX_DOCBOOK '                  </thead>'
echo >>$STD_LINUX_DOCBOOK '                  <tbody>'
echo >>$STD_LINUX_DOCBOOK '                    <row>'
echo >>$STD_LINUX_DOCBOOK '                      <entry>All functions</entry>'
echo >>$STD_LINUX_DOCBOOK '                      <entry>linux/*.h</entry>'
echo >>$STD_LINUX_DOCBOOK '                      <entry>See the TEST REPORT above for details.</entry>'
echo >>$STD_LINUX_DOCBOOK '                    </row>'

echo >>$STD_LINUX_DOCBOOK '                  </tbody>'
echo >>$STD_LINUX_DOCBOOK '                </tgroup>'
echo >>$STD_LINUX_DOCBOOK '            </informaltable>'
echo >>$STD_LINUX_DOCBOOK '            <para>'
echo >>$STD_LINUX_DOCBOOK '              See Implementation Description for implementation-defined behaviour.'
echo >>$STD_LINUX_DOCBOOK '            </para>'
echo >>$STD_LINUX_DOCBOOK '        </section>'
echo >>$STD_LINUX_DOCBOOK '  </chapter>'

echo >>$STD_LINUX_DOCBOOK '  <chapter id="implementation-description">'
echo >>$STD_LINUX_DOCBOOK '    <title>Implementation Description</title>'

echo >>$STD_LINUX_DOCBOOK '    <section id="implementation-description-details">'
echo >>$STD_LINUX_DOCBOOK '      <title>Implementation details</title>'

echo >>$STD_LINUX_DOCBOOK '      <para>'
echo >>$STD_LINUX_DOCBOOK '	    ST-Ericsson provides the reference implementation for the'
echo >>$STD_LINUX_DOCBOOK '	    ST-Ericsson Linux Platforms.'
echo >>$STD_LINUX_DOCBOOK '	  </para>'

echo >>$STD_LINUX_DOCBOOK '      <informaltable>'
echo >>$STD_LINUX_DOCBOOK '        <tgroup cols="3">'
echo >>$STD_LINUX_DOCBOOK '          <thead>'
echo >>$STD_LINUX_DOCBOOK '            <row>'
echo >>$STD_LINUX_DOCBOOK '              <entry>Functions</entry>'
echo >>$STD_LINUX_DOCBOOK '              <entry>Header File</entry>'
echo >>$STD_LINUX_DOCBOOK '              <entry>Description/Conformance</entry>'
echo >>$STD_LINUX_DOCBOOK '            </row>'
echo >>$STD_LINUX_DOCBOOK '          </thead>'
echo >>$STD_LINUX_DOCBOOK '          <tbody>'
echo >>$STD_LINUX_DOCBOOK '            <row>'
echo >>$STD_LINUX_DOCBOOK '              <entry>All functions</entry>'
echo >>$STD_LINUX_DOCBOOK '              <entry>linux/*.h</entry>'
echo >>$STD_LINUX_DOCBOOK '              <entry>For details, see elsewhere in this manual.</entry>'
echo >>$STD_LINUX_DOCBOOK '            </row>'
echo >>$STD_LINUX_DOCBOOK '          </tbody>'
echo >>$STD_LINUX_DOCBOOK '         </tgroup>'
echo >>$STD_LINUX_DOCBOOK '       </informaltable>'
echo >>$STD_LINUX_DOCBOOK '    </section>'
echo >>$STD_LINUX_DOCBOOK '     <section id="implementation-description-supported-versions">'
echo >>$STD_LINUX_DOCBOOK '      <title>Supported versions</title>'
echo >>$STD_LINUX_DOCBOOK '      <para>'
echo >>$STD_LINUX_DOCBOOK '	       The platform implements support according to the'
echo >>$STD_LINUX_DOCBOOK '        Linux Kernel '$KERNEL_VERSION '.'
echo >>$STD_LINUX_DOCBOOK '      </para>'
echo >>$STD_LINUX_DOCBOOK '    </section>'
echo >>$STD_LINUX_DOCBOOK '  </chapter>'

echo >>$STD_LINUX_DOCBOOK ' <chapter id="specifications">'
echo >>$STD_LINUX_DOCBOOK '  <title>Specifications</title>'
echo >>$STD_LINUX_DOCBOOK '  <para>'
echo >>$STD_LINUX_DOCBOOK '    For details, see elsewhere in this manual, and the'
echo >>$STD_LINUX_DOCBOOK '    "Software architecture overview, ST-Ericsson platforms, DESCRIPTION".'
echo >>$STD_LINUX_DOCBOOK '  </para>'
echo >>$STD_LINUX_DOCBOOK ' </chapter>'



echo >>$STD_LINUX_DOCBOOK '  <chapter id="bugs">'
echo >>$STD_LINUX_DOCBOOK '    <title>Known Bugs And Assumptions</title>'
echo >>$STD_LINUX_DOCBOOK '    <para>'
echo >>$STD_LINUX_DOCBOOK '      See the "Implementation Conformance Statement, ST-Ericsson Mobile Platform, TEST REPORT"'
echo >>$STD_LINUX_DOCBOOK '      for details on conformity to the Linux Kernel.'
echo >>$STD_LINUX_DOCBOOK '    </para>'
echo >>$STD_LINUX_DOCBOOK '  </chapter>'
echo >>$STD_LINUX_DOCBOOK '</book>'

