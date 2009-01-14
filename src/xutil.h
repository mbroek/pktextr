#ifndef	_XUTIL_H
#define	_XUTIL_H

/* $Id$ */


#ifndef size_t
#define size_t unsigned int
#endif

char *xmalloc(size_t);
char *xstrcpy(char *);
char *xstrcat(char *,char *);

#endif
