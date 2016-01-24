#
# Copyright (c) 2013 Claudio Castiglia <ccastiglia@gmail.com>
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * Neither the name of the author nor the names of its contributors
#       may be used to endorse or promote products derived
#       from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
LIB=		libkhook.so
INC=		khook.h

PREFIX?=	/usr/local

LIBDIR?=	${PREFIX}/lib
INCDIR?=	${PREFIX}/include

FULLLIB=	${abspath ${LIBDIR}}/${LIB}.${VERSION}
FULLINC=	${abspath ${INCDIR}}/${INC}
VERSION=	1.0.0

SRCS=		disass.c \
		khook.c

OBJS=		${SRCS:.c=.o}

CPPFLAGS=	-I.

CFLAGS=		-O0 \
		-fPIC \
		-m32 \
		-shared

INSTALL?=	install
LN?=		ln
RM?=		rm

.PHONY: all clean install release

all: ${LIB}

${LIB}: ${OBJS}
	${CC} ${CFLAGS} -o $@ $^

.c.o:
	${CC} ${CPPFLAGS} ${CFLAGS} -c -o $@ $<

install: all
	${INSTALL} -d -m 755 ${LIBDIR} ${INCDIR}
	${INSTALL} -m 755 ${LIB} ${FULLLIB}
	${LN} -sf ${FULLLIB} ${LIBDIR}/${LIB}
	${INSTALL} -m 644 ${INC} ${FULLINC}

clean:
	${RM} -rf ${LIB} ${OBJS}
	${MAKE} ${MAKEARGS} -C example $@

release:
	git archive --format=txz --prefix khook-v${VERSION}/ -o khook-v${VERSION}.txz v${VERSION}

