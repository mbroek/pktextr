#ifndef	_GMTOFFSET_H
#define	_GMTOFFSET_H

/* $Id$ */

long            gmt_offset(time_t);
char            *gmtoffset(time_t);
char            *str_time(time_t);
char            *t_elapsed(time_t, time_t);

#endif
