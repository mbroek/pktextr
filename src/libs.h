/*****************************************************************************
 *
 * $Id$
 * Purpose ...............: Libraries include list
 *
 *****************************************************************************
 * Copyright (C) 2004
 *   
 * Michiel Broek		FIDO:	2:280/2802
 * Beekmansbos 10
 * 1971 BV IJmuiden
 * the Netherlands
 *
 * This file is part of pktstat.
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


#ifndef _LIBS_H
#define _LIBS_H

#ifndef _GNU_SOURCE
#define	_GNU_SOURCE 1
#endif

#define	TRUE 1
#define	FALSE 0


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>   
#include <stdarg.h>
#include <sys/ioctl.h>
#ifdef	HAVE_SYS_VFS_H
#include <sys/vfs.h>
#endif
#include <setjmp.h>

#endif
