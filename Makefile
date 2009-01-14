# Top-level makefile for pktextr package
# $Id$

include Makefile.global

OTHER		= AUTHORS ChangeLog COPYING FILE_ID.DIZ.in \
		  INSTALL Makefile Makefile.global.in NEWS \
		  README TODO UPGRADE aclocal.m4 \
		  config.h.in configure configure.in config-example
TARFILE		= ${PACKAGE}-${VERSION}.tar.bz2

###############################################################################


all depend:
		@for d in ${SUBDIRS}; do (cd $$d && ${MAKE} $@) || exit; done; \

install:
		@if [ -z ${PREFIX} ] ; then \
			echo; echo "PREFIX is not set!"; echo; exit 3; \
		fi
		@${INSTALL} -d -o ${OWNER} -g ${GROUP} -m 0755 ${PREFIX}
		@${INSTALL} -d -o ${OWNER} -g ${GROUP} -m 0770 ${PREFIX}/bin
		@${INSTALL} -d -o ${OWNER} -g ${GROUP} -m 0770 ${PREFIX}/etc
		@${INSTALL} -d -o ${OWNER} -g ${GROUP} -m 0770 ${PREFIX}/log		
		for d in ${SUBDIRS}; do (cd $$d && ${MAKE} $@) || exit; done

dist tar:	${TARFILE}

clean:
		rm -f .filelist core ${TARFILE} config.h FILE_ID.DIZ
		for d in ${SUBDIRS}; do (cd $$d && ${MAKE} $@) || exit; done;

${TARFILE}:	.filelist
		cd ..; ln -s ${PACKAGE} ${PACKAGE}-${VERSION} ; rm -f ${TARFILE}; \
		${TAR} cvTf ./${PACKAGE}-${VERSION}/.filelist - | bzip2 >${TARFILE} ; \
		rm -f ${PACKAGE}-${VERSION}

.filelist filelist:
		(for f in ${OTHER} ;do echo ${PACKAGE}-${VERSION}/$$f; done) >.filelist
		for d in ${SUBDIRS}; do (cd $$d && ${MAKE} filelist && cat filelist >>../.filelist) || exit; done;

