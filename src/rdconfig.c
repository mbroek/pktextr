/*****************************************************************************
 *
 * $Id$
 * Purpose ...............: read configuration
 *
 *****************************************************************************
 * Copyright (C) 2002-2004
 *   
 * Michiel Broek                FIDO:           2:280/2802
 * Beekmansbos 10
 * 1971 BV IJmuiden
 * the Netherlands
 *
 * This file is part of pktextr.
 *
 * This BBS is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2, or (at your option) any
 * later version.
 *
 * MBSE BBS is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with MBSE BBS; see the file COPYING.  If not, write to the Free
 * Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *****************************************************************************/

#include "../config.h"
#include "libs.h"
#include "xutil.h"
#include "rdconfig.h"

#ifndef BUFSIZ
#define BUFSIZ 512
#endif



/* 
 * Global configuration variables 
 */
char		*configname = NULL;



/* 
 * Local 
 */
static char	*k,*v;
static int	linecnt = 0;



/* 
 * External variables 
 */
extern char	*logfile;
extern char	*csvfile;
extern char	*sqlfile;
extern char	*sqltable;


/* 
 * parser prototypes 
 */
static int getstr(char**);
static int getlong(char**);

			

/* 
 * keyword table 
 */
static struct _keytab {
	char *key;
	int (*prc)(char**);
	char** dest;
} keytab[] = {
	{(char *)"logfile",	getstr,		&logfile},
	{(char *)"csvfile",	getstr,		&csvfile},
	{(char *)"sqlfile",	getstr,		&sqlfile},
	{(char *)"sqltable",	getstr,		&sqltable},
	{NULL,			NULL,		NULL}
};



/* 
 * public entry point 
 */
int readconfig(char *config)
{
    int		maxrc = 0, rc, i;
    FILE	*fp;
    char	buf[BUFSIZ], *p;
    struct stat	st;

    configname = config;

    if (stat(configname,&st) != 0) {
	fprintf(stderr, "readconfig: cannot stat file \"%s\" ", configname);
	perror("");
	return 1;
    }

    if ((fp = fopen(configname,"r")) == NULL) {
	fprintf(stderr, "readconfig: cannot open file \"%s\" ", PE_SS(configname));
	perror("");
	return 1;
    }
	
    while (fgets(buf, sizeof(buf)-1, fp)) {
	linecnt++;
	if (*(p = buf + strlen(buf)-1) != '\n') {
	    fprintf(stderr, "%s(%d): %s - line too long\n", PE_SS(configname), linecnt, buf);
	    if (3 > maxrc)
		maxrc = 3;
	    while (fgets(buf, sizeof(buf) -1, fp) && (*(p = buf + strlen(buf)-1) != '\n'));
		continue;
	}

	*p--='\0';
	while ((p >= buf) && isspace(*p)) 
	    *p--='\0';
	k=buf;
	while (*k && isspace(*k)) 
	    k++;
	p=k;
	while (*p && !isspace(*p)) 
	    p++;
	*p++='\0';
	v=p;
	while (*v && isspace(*v)) 
	    v++;

	if ((*k == '\0') || (*k == '#')) {
	    continue;
	}

	for (i = 0; keytab[i].key; i++)
	    if (strcasecmp(k, keytab[i].key) == 0) 
		break;
	
	if (keytab[i].key == NULL) {
	    fprintf(stderr, "%s(%d): %s %s - unknown keyword\n", configname, linecnt, PE_SS(k), PE_SS(v));
	    if (2 > maxrc)
		maxrc = 2;
	} else if ((rc = keytab[i].prc(keytab[i].dest)) > maxrc) 
	    maxrc = rc;
    }
    fclose(fp);

    if (logfile == NULL) {
	fprintf(stderr, "logfile not configured in %s\n", PE_SS(configname));
	return 1;
    }
    
    return maxrc;
}



/* 
 * parsers 
 */


static int getstr(char **dest)
{
    *dest = xstrcpy(v);
    return 0;
}



static int getlong(dest)
char **dest;
{
    if (strspn(v,"0123456789") != strlen(v)) 
	fprintf(stderr, "%s(%d): %s %s - bad numeric\n", configname, linecnt, PE_SS(k), PE_SS(v));
    else 
	*((long*)dest) = atol(v);
    return 0;
}


