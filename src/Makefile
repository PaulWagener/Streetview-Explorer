PROG = streetview-explorer
SRCS += Application.cpp
SRCS += EditLocationsDialog.cpp
SRCS += Explorer.cpp
SRCS += GLCanvas.cpp
SRCS += MainFrame.cpp
SRCS += Panorama.cpp
SRCS += Player.cpp
SRCS += PreferencesDialog.cpp
SRCS += Settings.cpp
SRCS += Utm.cpp
SRCS += base64.cpp
SRCS += common.cpp
SRCS += download.cpp
SRCS += statustext.cpp

# X11 and package dirs.
X11BASE ?= /usr/X11R6
LOCALBASE ?= /usr/local

# Install parameters.
BINDIR ?= ${LOCALBASE}/bin
BINOWN ?= root
BINGRP ?= bin
BINMODE ?= 555
INSTALL_COPY ?= -c
INSTALL_STRIP ?= -s

RM ?= rm -f
CPPFLAGS += -I${X11BASE}/include -I${LOCALBASE}/include
CPPFLAGS += `wx-config --cxxflags`
LDADD += -L${X11BASE}/lib -L${LOCALBASE}/lib
LDADD += -lGL -lGLU -lcurl
LDADD += `wx-config --libs gl`
OBJS = ${SRCS:.cpp=.o}

.SUFFIXES: .cpp .o

all: ${PROG}

.cpp.o:
	${CXX} ${CPPFLAGS} ${CXXFLAGS} -o $@ -c $<

depend:
	@files="${SRCS}"; \
	if [ "$$files" != "  " ]; then \
	  echo mkdep -a ${MKDEP} ${CXXFLAGS:M-[ID]*} ${CPPFLAGS} $$files; \
	  mkdep -a ${MKDEP} ${CXXFLAGS:M-[ID]*} ${CPPFLAGS} $$files; \
	fi

${PROG}: ${OBJS}
	c++ -o ${PROG} ${OBJS} ${LDADD}

clean:
	${RM} ${PROG} ${OBJS}

install:
	${INSTALL} ${INSTALL_COPY} ${INSTALL_STRIP} -o ${BINOWN} -g ${BINGRP} \
	    -m ${BINMODE} ${PROG} ${DESTDIR}${BINDIR}/${PROG}
