#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=
AS=as

# Macros
CND_PLATFORM=GNU-Linux-x86
CND_CONF=Release
CND_DISTDIR=dist

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=build/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/1360937237/Settings.o \
	${OBJECTDIR}/_ext/1360937237/MainFrame.o \
	${OBJECTDIR}/_ext/1360937237/Application.o \
	${OBJECTDIR}/_ext/1360937237/GLCanvas.o \
	${OBJECTDIR}/_ext/1360937237/Player.o \
	${OBJECTDIR}/_ext/1360937237/download.o \
	${OBJECTDIR}/_ext/1360937237/Utm.o \
	${OBJECTDIR}/_ext/1360937237/statustext.o \
	${OBJECTDIR}/_ext/1360937237/PreferencesDialog.o \
	${OBJECTDIR}/_ext/1360937237/base64.o \
	${OBJECTDIR}/_ext/1360937237/Panorama.o \
	${OBJECTDIR}/_ext/1360937237/Explorer.o \
	${OBJECTDIR}/_ext/1360937237/common.o \
	${OBJECTDIR}/_ext/1360937237/EditLocationsDialog.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=`wx-config --cppflags` 
CXXFLAGS=`wx-config --cppflags` 

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=`wx-config --libs --gl-libs`  

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-Release.mk dist/Release/GNU-Linux-x86/StreetViewExplorer

dist/Release/GNU-Linux-x86/StreetViewExplorer: ${OBJECTFILES}
	${MKDIR} -p dist/Release/GNU-Linux-x86
	${LINK.cc} -lcurl -ljpeg -lGLEW -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/StreetViewExplorer ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/_ext/1360937237/Settings.o: ../src/Settings.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.cc) -O2 -I/usr/local/include/wx-2.9 -I/usr/local/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/Settings.o ../src/Settings.cpp

${OBJECTDIR}/_ext/1360937237/MainFrame.o: ../src/MainFrame.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.cc) -O2 -I/usr/local/include/wx-2.9 -I/usr/local/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/MainFrame.o ../src/MainFrame.cpp

${OBJECTDIR}/_ext/1360937237/Application.o: ../src/Application.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.cc) -O2 -I/usr/local/include/wx-2.9 -I/usr/local/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/Application.o ../src/Application.cpp

${OBJECTDIR}/_ext/1360937237/GLCanvas.o: ../src/GLCanvas.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.cc) -O2 -I/usr/local/include/wx-2.9 -I/usr/local/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/GLCanvas.o ../src/GLCanvas.cpp

${OBJECTDIR}/_ext/1360937237/Player.o: ../src/Player.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.cc) -O2 -I/usr/local/include/wx-2.9 -I/usr/local/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/Player.o ../src/Player.cpp

${OBJECTDIR}/_ext/1360937237/download.o: ../src/download.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.cc) -O2 -I/usr/local/include/wx-2.9 -I/usr/local/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/download.o ../src/download.cpp

${OBJECTDIR}/_ext/1360937237/Utm.o: ../src/Utm.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.cc) -O2 -I/usr/local/include/wx-2.9 -I/usr/local/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/Utm.o ../src/Utm.cpp

${OBJECTDIR}/_ext/1360937237/statustext.o: ../src/statustext.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.cc) -O2 -I/usr/local/include/wx-2.9 -I/usr/local/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/statustext.o ../src/statustext.cpp

${OBJECTDIR}/_ext/1360937237/PreferencesDialog.o: ../src/PreferencesDialog.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.cc) -O2 -I/usr/local/include/wx-2.9 -I/usr/local/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/PreferencesDialog.o ../src/PreferencesDialog.cpp

${OBJECTDIR}/_ext/1360937237/base64.o: ../src/base64.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.cc) -O2 -I/usr/local/include/wx-2.9 -I/usr/local/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/base64.o ../src/base64.cpp

${OBJECTDIR}/_ext/1360937237/Panorama.o: ../src/Panorama.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.cc) -O2 -I/usr/local/include/wx-2.9 -I/usr/local/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/Panorama.o ../src/Panorama.cpp

${OBJECTDIR}/_ext/1360937237/Explorer.o: ../src/Explorer.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.cc) -O2 -I/usr/local/include/wx-2.9 -I/usr/local/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/Explorer.o ../src/Explorer.cpp

${OBJECTDIR}/_ext/1360937237/common.o: ../src/common.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.cc) -O2 -I/usr/local/include/wx-2.9 -I/usr/local/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/common.o ../src/common.cpp

${OBJECTDIR}/_ext/1360937237/EditLocationsDialog.o: ../src/EditLocationsDialog.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360937237
	${RM} $@.d
	$(COMPILE.cc) -O2 -I/usr/local/include/wx-2.9 -I/usr/local/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1360937237/EditLocationsDialog.o ../src/EditLocationsDialog.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/Release
	${RM} dist/Release/GNU-Linux-x86/StreetViewExplorer

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
