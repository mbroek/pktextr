#ifndef	_RDCONFIG_H
#define	_RDCONFIG_H

/* $Id$ */

#define PE_SS(x) (x)?(x):"(null)"

int readconfig(char *conffile);

#endif
