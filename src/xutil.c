/*****************************************************************************
 *
 * $Id$
 * Purpose ...............: memory utils.
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


char *xmalloc(size_t size)
{
    char *tmp;

    tmp = malloc(size);
    if (!tmp) 
	abort();
	
    return tmp;
}



char *xstrcpy(char *src)
{
    char    *tmp;

    if (src == NULL) 
	return(NULL);
    tmp = xmalloc(strlen(src)+1);
    strcpy(tmp,src);
    return tmp;
}



char *xstrcat(char *src, char *add)
{
    char    *tmp;
    size_t  size=0;

    if ((add == NULL) || (strlen(add) == 0)) 
	return src;
    if (src) 
	size=strlen(src);
    size+=strlen(add);
    tmp=xmalloc(size+1);
    *tmp='\0';
    if (src) {
	strcpy(tmp,src);
	free(src);
    }
    strcat(tmp,add);
    return tmp;
}

