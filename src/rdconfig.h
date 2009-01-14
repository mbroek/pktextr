#ifndef	_RDCONFIG_H
#define	_RDCONFIG_H

/* $Id$ */

#define PE_SS(x) (x)?(x):"(null)"
#define SQ_SS(x) (x)?(x):"NULL"

int readconfig(char *conffile);

#endif
