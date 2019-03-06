#!/bin/bash

# Set QTDIR env var, if not already done
: "${QTDIR:=/Users/mrhyde/Qt/5.12.0/clang_64}"

echo QTDIR is ${QTDIR}


THE_BUNDLE_NAME="Tsunami"

THE_BUNDLE_APP=${THE_BUNDLE_NAME}.app


THE_DEPLOYCMD=${QTDIR}/bin/macdeployqt

# Set THE_BUILD_DIR with the dir where QtCreator put the built object (=folder containing built Tsunami.app)
# If not set, the current path will be used
# THE_BUILD_DIR=build-Tsunami-Desktop_Qt_5_12_0_clang_64bit-Release
THE_BUILD_DIR=$1

THE_ADDITIONALLIB_DIR=$2

if [ -z ${THE_BUILD_DIR} ]; then
        THE_BUILD_DIR=`pwd`
fi

THE_BUNDLE_APP_PATH=${THE_BUILD_DIR}\/${THE_BUNDLE_APP}

if [ ! -d ${THE_BUNDLE_APP_PATH} ]; then
	echo Bundle dir ${THE_BUNDLE_APP} not found in ${THE_BUILD_DIR}, cannot continue
	exit 1
fi

THE_BUNDLE_EXE=${THE_BUNDLE_APP_PATH}/Contents/MacOS/Tsunami

if [ ! -f ${THE_BUNDLE_EXE} ]; then
	echo File ${THE_BUNDLE_EXE} not found, cannot continue
	exit 1
fi



THE_VERSION=`grep "DISTNAME      =" Makefile | sed 's/DISTNAME      = Tsunami//g'`

echo Version is ${THE_VERSION}

NAME_OF_DMG=${THE_BUNDLE_NAME}-${THE_VERSION}

echo checking for debug code...
HAS_DEBUGGING=`echo ${THE_BUILD_DIR} | grep -i "\-debug$"`
if [ "x${HAS_DEBUGGING}" != "x" ]; then
        echo "    built with Debugging!"
        NAME_OF_ZIP+="_Debug"
fi

echo checking for i386 arch...
# Check if binary includes i386 
HAS_I386=`file ${THE_BUNDLE_EXE} | grep i386`
if [ "x${HAS_I386}" != "x" ]; then
        echo "    built for i386!"
        NAME_OF_DMG+="_i386"
fi


echo checking for x86_64 arch...
# Check if binary includes x86_64 
HAS_X86_64=`file ${THE_BUNDLE_EXE} | grep x86_64`
if [ "x${HAS_X86_64}" != "x" ]; then
        echo "    built for x86_64!"
        NAME_OF_DMG+="_x64"
fi

echo checking for min OSX version...
# Check min version
MIN_OSX_VER=`otool -l ${THE_BUNDLE_EXE} | grep _MIN_ -A2 | grep version | sed 's/^.*version[ *]//g'`
if [ "x${MIN_OSX_VER}" != "x" ]; then
        echo "    built for OSX >= ${MIN_OSX_VER}!"
        NAME_OF_DMG+="_OSX_${MIN_OSX_VER}"
fi


NAME_OF_DMG+=".dmg"

ORIGINAL_DMG=${THE_BUNDLE_NAME}.dmg


THE_TIMESTAMP=`date "+%Y%m%d_%H%M%S"`
BACKUPNAME=${NAME_OF_ZIP}_${THE_TIMESTAMP}

if [ -f ${ORIGINAL_DMG} ]; then
	BACKUPNAME=${ORIGINAL_DMG}_${THE_TIMESTAMP}
	echo A previous version of DMG ${ORIGINAL_DMG} has been found and it will be renamed as ${BACKUPNAME}
	mv -f ${ORIGINAL_DMG} ${BACKUP_NAME} 
fi

echo Creating ${ORIGINAL_DMG} from ${THE_BUNDLE_APP} ...
if [ -n ${THE_ADDITIONALLIB_DIR} ]; then
	${THE_DEPLOYCMD} ${THE_BUNDLE_APP} -libpath="${THE_ADDITIONALLIB_DIR}" -dmg -always-overwrite
else
	${THE_DEPLOYCMD} ${THE_BUNDLE_APP} -dmg -always-overwrite
fi

if [ ! -f ${ORIGINAL_DMG} ]; then
	echo Failed to create bundle dmg/${ORIGINAL_DMG} not found, cannot continue
	exit 1
fi

# Be sure to not overwrite previous packets
if [ -f ${NAME_OF_DMG} ]; then
	BACKUPNAME=${NAME_OF_DMG}_${THE_TIMESTAMP}
	echo A previous version of DMG ${NAME_OF_DMG} has been found and it will be renamed as ${BACKUPNAME}
	mv -f ${NAME_OF_DMG} ${BACKUP_NAME} 
fi

echo renaming ${ORIGINAL_DMG} to ${NAME_OF_DMG} ...
mv -f ${ORIGINAL_DMG} ${NAME_OF_DMG}
echo Done!



