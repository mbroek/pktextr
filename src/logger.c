/*****************************************************************************
 *
 * $Id$
 * Purpose ...............: Logging
 *
 *****************************************************************************
 * Copyright (C) 2002
 *   
 * Michiel Broek		FIDO:		2:280/2802
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
#include "crc.h"
#include "xutil.h"
#include "logger.h"



unsigned long	lcrc = 0, tcrc = 1;	/* CRC values of logstring	*/
int		lcnt = 0;		/* Log messages counter		*/
char		*logfile = NULL;


/*
 * Prototypes
 */
char *date(void);



static char *mon[] = {
	    (char *)"Jan",(char *)"Feb",(char *)"Mar", (char *)"Apr",(char *)"May",(char *)"Jun",
	    (char *)"Jul",(char *)"Aug",(char *)"Sep", (char *)"Oct",(char *)"Nov",(char *)"Dec"
};


char *date(void)
{
    struct  tm      ptm;
    time_t          now;
    static  char    buf[20];

    now = time(NULL);
    ptm = *localtime(&now);
    sprintf(buf,"%02d-%s-%04d %02d:%02d:%02d", ptm.tm_mday, mon[ptm.tm_mon], ptm.tm_year+1900,
		ptm.tm_hour, ptm.tm_min, ptm.tm_sec);
    return(buf);
}


void Syslog(int level, const char *format, ...)
{
    char    *outstr;
    va_list va_ptr;
    int	    i;
    FILE    *fp;

    outstr = calloc(10240, sizeof(char));
    va_start(va_ptr, format);
    vsprintf(outstr, format, va_ptr);
    va_end(va_ptr);

    for (i = 0; i < strlen(outstr); i++)
	if (outstr[i] == '\r' || outstr[i] == '\n')
	    outstr[i] = ' ';

    fp = fopen(logfile, "a");
    if (fp == NULL) {
	printf("Cannot open logfile \"%s\"\n", logfile);
	return;
    }

    tcrc = StringCRC32(outstr);
    if (tcrc == lcrc) {
	lcnt++;
	fclose(fp);
	return;
    } else {
	lcrc = tcrc;
	if (lcnt) {
	    lcnt++;
	    fprintf(fp, "%c %s pktextr[%d] Last message repeated %d times", level, date(), getpid(), lcnt);
	}
	lcnt = 0;
    }

    fprintf(fp, "%c %s pktextr[%d] ", level, date(), getpid());
    fprintf(fp, *outstr == '$' ? outstr+1 : outstr);
    if (*outstr == '$')
	fprintf(fp, ": %s\n", strerror(errno));
    else
	fprintf(fp, "\n");

    fflush(fp);
    fclose(fp);
    free(outstr);
}


