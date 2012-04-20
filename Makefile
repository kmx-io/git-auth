##
## git-auth  -  restrict git commands
## Copyright 2012 Thomas de Grivel <billitch@gmail.com>
##
## Permission to use, copy, modify, and distribute this software for any
## purpose with or without fee is hereby granted, provided that the above
## copyright notice and this permission notice appear in all copies.
##
## THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
## WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
## MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
## ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
## WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
## ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
## OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
##

PROG =	git-auth

BINDIR ?= /usr/local/bin

DEBUG ?= -g -DDEBUG
WARNINGS ?= yes

CDIAGFLAGS ?= -W -Wall -Werror

SRCS =	git-auth.c \
	rule.c rule.h \
	symbol.c symbol.h \
	symtable.c symtable.h \
	sympackage.c sympackage.h

VER = 0.1

build: ${PROG}

DISTDIR = ${PROG}-${VER}
DIST = ${DISTDIR}.tar.gz
dist: ${DIST}

DISTFILES = ${SRCS} ${PROG}.1 Makefile

${DIST}: ${DISTFILES}
	mkdir ${PROG}-${VER}
	cd ${.CURDIR} && cp ${DISTFILES} ${.OBJDIR}/${PROG}-${VER}
	tar czf $@.tmp ${PROG}-${VER}
	rm -rf ${PROG}-${VER}
	mv $@.tmp $@

cleandir: cleandist

cleandist:
	rm -rf ${DISTDIR}

.PHONY: cleandist

CLEANFILES += ${DIST}

.include <bsd.prog.mk>
