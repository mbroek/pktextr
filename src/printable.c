/*****************************************************************************
 *
 * $Id$
 * Purpose ...............: print cleaned-up string
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
#include "printable.h"


static char	*pbuff = NULL;



char *printable(char *s, int l)
{
    int     len;
    char    *p;

    if (pbuff) 
	free(pbuff);
    pbuff=NULL;

    if (s == NULL) 
	return (char *)"(null)";

    if (l > 0) 
	len=l;
    else if (l == 0) 
	len=strlen(s);
    else {
	len=strlen(s);
	if (len > -l) 
	    len=-l;
    }

    pbuff=(char*)xmalloc(len*4+1);
    p=pbuff;
    while (len--) {
	if (*(unsigned char*)s >= ' ') 
	    *p++=*s;
	else switch (*s) {
	    case '\\': *p++='\\'; *p++='\\'; break;
	    case '\r': *p++='\\'; *p++='r'; break;
	    case '\n': *p++='\\'; *p++='n'; break;
	    case '\t': *p++='\\'; *p++='t'; break;
	    case '\b': *p++='\\'; *p++='b'; break;
	    default:   sprintf(p,"\\%03o",*s); p+=4; break;
	}
	s++;
    }
    *p='\0';
    return pbuff;
}



char *printablec(char c)
{
    return printable(&c,1);
}



