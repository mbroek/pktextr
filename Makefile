# Top-level makefile for pktextr package
# $Id$

include Makefile.global

OTHER		= AUTHORS ChangeLog COPYING FILE_ID.DIZ.in \
		  INSTALL Makefile Makefile.global.in NEWS \
		  README TODO UPGRADE aclocal.m4 \
		  config.h.in configure configure.in config-example
TARFILE		= ${PACKAGE}-${VERSION}.tar.gz

###############################################################################


all depend:
		@for d in ${SUBDIRS}; do (cd $$d && ${MAKE} $@) || exit; done; \

install:
		@if [ -z ${PREFIX} ] ; then \
			echo; echo "PREFIX is not set!"; echo; exit 3; \
		fi
		@if [ ! -d ${PREFIX}/bin ] ; then \
			mkdir ${PREFIX}/bin ; \
			${CHOWN} ${OWNER}.${GROUP} ${PREFIX}/bin ; \
		fi
		@if [ ! -d ${PREFIX}/etc ] ; then \
			mkdir ${PREFIX}/etc ; \
			${CHOWN} ${OWNER}.${GROUP} ${PREFIX}/etc ; \
		fi
		@chmod 0775 ${PREFIX}/etc
		@if [ ! -d ${PREFIX}/log ] ; then \
			mkdir ${PREFIX}/log ; \
			${CHOWN} ${OWNER}.${GROUP} ${PREFIX}/log ; \
		fi
		@chmod 0775 ${PREFIX}/log
		@if [ ! -d ${PREFIX}/var/pktextr ] ; then \
			mkdir ${PREFIX}/var/pktextr ; \
			${CHOWN} ${OWNER}.${GROUP} ${PREFIX}/var/pktextr ; \
			chmod 0750 ${PREFIX}/var/pktextr ; \
		fi
		@chmod 0770 ${PREFIX}/var
		for d in ${SUBDIRS}; do (cd $$d && ${MAKE} $@) || exit; done

dist tar:	${TARFILE}

clean:
		rm -f .filelist core ${TARFILE}
		for d in ${SUBDIRS}; do (cd $$d && ${MAKE} $@) || exit; done;

${TARFILE}:	.filelist
		cd ..; rm -f ${TARFILE}; \
		${TAR} cvTf ./${PACKAGE}-${VERSION}/.filelist - | gzip >${TARFILE}

.filelist filelist:
		(for f in ${OTHER} ;do echo ${PACKAGE}-${VERSION}/$$f; done) >.filelist
		for d in ${SUBDIRS}; do (cd $$d && ${MAKE} filelist && cat filelist >>../.filelist) || exit; done;

