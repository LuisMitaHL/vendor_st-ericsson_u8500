#!/bin/sh
# © Copyright ST-Ericsson 2010. All rights reserved.

RESULT_XMLTO=`xmlto --version`
if [ ! -n "$RESULT_XMLTO" ]
then
	echo ""
	echo "**********************************************************************"
	echo "**********************************************************************"
	echo "**                                                                  **"
	echo "**  WARN Xmlto does not exist, it will not be possible to generate  **"
	echo "**  WARN the referene manual.                                       **"
	echo "**  WARN Please install Xmlto on your computer.                     **"
	echo "**                                                                  **"
	echo "**********************************************************************"
	echo "**********************************************************************"
	echo ""
fi

RESULT_DOXYGEN=`doxygen --version`
if [ ! -n "$RESULT_DOXYGEN" ]
then
	echo ""
	echo "************************************************************************"
	echo "************************************************************************"
	echo "**                                                                    **"
	echo "**  WARN Doxygen does not exist, it will not be possible to generate  **"
	echo "**  WARN the referene manual.                                         **"
	echo "**  WARN Please install Doxygen on your computer.                     **"
	echo "**                                                                    **"
	echo "************************************************************************"
	echo "************************************************************************"
	echo ""
fi