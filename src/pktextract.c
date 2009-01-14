/*****************************************************************************
 *
 * $Id$
 * Purpose ...............: .pkt data extract
 *
 *****************************************************************************
 * Copyright (C) 2002-2008
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
#include "rdconfig.h"
#include "aread.h"
#include "rfcdate.h"
#include "printable.h"
#include "pktextract.h"


// Extracted fields:
// From node
// Date
// Too
// From
// Subject
// Area
// Pid
// Tid
// Chrs


/*
 * Prototypes
 */
void Progname(void);
void Help(void);
void die(int);
char *sqlescape(char *);


char		*pktfile = NULL;
char		*csvfile = NULL;
char		*sqlfile = NULL;
char		*sqltable = NULL;


void die(int onsig)
{
    signal(onsig, SIG_IGN);

    Syslog(' ', "PKTEXTRACT finished");
    exit(onsig);
}



void Progname(void)
{
    /*
     * Print copyright notices.
     */
    printf("PKTEXTRACT: v%s Mail packet data extracter\n", VERSION);
    printf("            %s\n\n", COPYRIGHT);
}



void Help(void)
{
    Progname();
}



char *sqlescape(char *inp)
{
    static char	buf[256];
    int		i, j = 0;

    memset(&buf, 0, sizeof(buf));
    for (i = 0; i < strlen(inp); i++) {
	if (inp[i] == '\'') {
	    buf[j] = '\\';
	    j++;
	}
	buf[j] = inp[i];
	j++;
    }
    return buf;
}



int main(int argc, char **argv)
{
    int		    i, tmp, result, flags, cost, line, echo;
    struct stat	    st;
    FILE	    *fp, *op;
    char	    buf[2048], *l, *r, *p, *subj = NULL, *fname = NULL, *tname = NULL, *area = NULL, *pid = NULL, *tid = NULL;
    char	    *orig = NULL, *chrs = NULL, *sql;
    time_t	    mdate = 0L;
    unsigned char   pbuffer[0x3a], mbuffer[0x0e];
    int		    f_zone, f_net, f_node, f_point = 0;
    int		    t_zone, t_net, t_node, t_point = 0;
    int		    capword, prodx, major, minor = 0;
    long            year, month, day, hour, min, sec;
    int		    netmail = 0, echomail = 0;

    /*
     *  Catch all the signals we can, and ignore the rest. Note that SIGKILL can't be ignored but that's live.
     */
    for (i = 0; i < NSIG; i++) {
        if ((i == SIGHUP) || (i == SIGINT) || (i == SIGBUS) || (i == SIGILL) || (i == SIGSEGV) || (i == SIGTERM))
            signal(i, (void (*))die);
	else if (i == SIGCHLD)
	    signal(i, SIG_DFL);
        else if ((i != SIGKILL) && (i != SIGSTOP))
            signal(i, SIG_IGN);
    }

    if (argc != 3) {
	Help();
	printf("Need 2 parameters\n");
	exit(1);
    }

    /*
     * Attempt to read the configuration
     */
    if (readconfig(argv[1])) {
	printf("Configuration error in %s\n", argv[1]);
	exit(1);
    }

    Syslog(' ', " ");
    Syslog(' ', "PKTEXTRACT v%s", VERSION);

    pktfile = argv[2];
    if (stat(pktfile, &st) != 0) {
	Syslog('!', "$Can't stat file \"%s\"", PE_SS(pktfile));
	die(100);
    }
    
    if ((fp = fopen(pktfile, "r")) == NULL) {
	Syslog('!', "$Can't open file \"%s\"", PE_SS(pktfile));
	die(100);
    }

    /*
     * Read type 2+ packet header, see FSC-0039 version 4 and FTS-0001
     */
    if (fread(pbuffer, 1, 0x3a, fp) != 0x3a) {
	Syslog('!', "$Could not read pkt header \"%s\"", PE_SS(pktfile));
	fclose(fp);
	die(100);
    }
    if ((pbuffer[0x12] + (pbuffer[0x13] << 8)) != 2) {
	Syslog('!', "Not a type 2 packet \"%s\"", PE_SS(pktfile));
	fclose(fp);
	die(100);
    }

    f_node = (pbuffer[0x01] << 8) + pbuffer[0x00];
    t_node = (pbuffer[0x03] << 8) + pbuffer[0x02];
    f_net  = (pbuffer[0x15] << 8) + pbuffer[0x14];
    t_net  = (pbuffer[0x17] << 8) + pbuffer[0x16];
    f_zone = (pbuffer[0x23] << 8) + pbuffer[0x22];
    t_zone = (pbuffer[0x25] << 8) + pbuffer[0x24];


    year    = (pbuffer[0x05] << 8) + pbuffer[0x04];
    month   = (pbuffer[0x07] << 8) + pbuffer[0x06] + 1;
    day     = (pbuffer[0x09] << 8) + pbuffer[0x08];
    hour    = (pbuffer[0x0b] << 8) + pbuffer[0x0a];
    min     = (pbuffer[0x0d] << 8) + pbuffer[0x0c];
    sec     = (pbuffer[0x0f] << 8) + pbuffer[0x0e];
    prodx   =  pbuffer[0x18];
    major   =  pbuffer[0x19];

    capword = (pbuffer[0x2d] << 8) + pbuffer[0x2c];
    if (capword != ((pbuffer[0x28] << 8) + pbuffer[0x29]))
	capword = 0;

    if (capword & 0x0001) {
	/*
	 * FSC-0039 packet type 2+
	 */
	prodx    = prodx + (pbuffer[0x2a] << 8);
	minor    = pbuffer[0x2b];
	f_zone   = pbuffer[0x2e] + (pbuffer[0x2f] << 8);
	t_zone   = pbuffer[0x30] + (pbuffer[0x31] << 8);
	f_point  = pbuffer[0x32] + (pbuffer[0x33] << 8);
	t_point  = pbuffer[0x34] + (pbuffer[0x35] << 8);
    }

    Syslog('+', "Packet   : %s type %s", pktfile, (capword & 0x0001) ? "2+":"stone-age");
    Syslog('+', "From     : %d:%d/%d.%d to  %d:%d/%d.%d", f_zone, f_net, f_node, f_point, t_zone, t_net, t_node, t_point);
    Syslog('+', "Dated    : %02u-%02u-%u %02u:%02u:%02u", day, month, year, hour, min, sec);

    /*
     * Read messages from the packet
     */
    for (;;) {
	result = fread(&mbuffer, 1, sizeof(mbuffer), fp);
	if (result == 0) {
	    Syslog('+', "Zero bytes message, assume end of pkt");
	    break;
	}

	/*
	 * Check if a valid message follows
	 */
	tmp = (mbuffer[0x01] << 8) + mbuffer[0x00];
	if (tmp == 0) {
	    if (result == 2)
		break;
	    else {
		Syslog('!', "Junk after logical end of packet");
		break;
	    }
	}
	if (tmp != 2) {
	    Syslog('!', "bad message type: 0x%04x",tmp);
	    break;
	}
	
	if (result != 0x0e) {
	    Syslog('!', "Unexpected end of packet");
	    break;
	}

	/*
	 * Valid message header block
	 */
	f_zone = f_net = f_node = f_point = 0;
	t_zone = t_net = t_node = t_point = 0;
	f_node = (mbuffer[0x03] << 8) + mbuffer[0x02];
	t_node = (mbuffer[0x05] << 8) + mbuffer[0x04];
	f_net  = (mbuffer[0x07] << 8) + mbuffer[0x06];
	t_net  = (mbuffer[0x09] << 8) + mbuffer[0x08];
	flags  = (mbuffer[0x0b] << 8) + mbuffer[0x0a];
	cost   = (mbuffer[0x0d] << 8) + mbuffer[0x0c];
	
        /*
	 * Read the DateTime, toUserName, fromUserName and subject fields
	 * from the packed message. The stringlength is +1 for the right
	 * check. This is different then in ifmail's original code.
	 */
        if (aread(buf, sizeof(buf)-1, fp)) {
	    if (strlen(buf) > 20)
		Syslog('!', "date too long (%d) \"%s\"", strlen(buf), printable(buf, 0));
	    mdate = parsefdate(buf, NULL);
	    if (aread(buf, sizeof(buf)-1, fp)) {
		Syslog('!', "date not null-terminated: \"%s\"",buf);
		break;
	    }
	}

        if (aread(buf, sizeof(buf)-1, fp)) {
	    if (strlen(buf) > 36)
	        Syslog('!', "to name too long (%d) \"%s\"", strlen(buf), printable(buf, 0));
            tname = xstrcpy(buf);
            if (aread(buf, sizeof(buf)-1, fp)) {
                if (*(p = tname + strlen(tname)-1) == '\n')
		    *p = '\0';
                Syslog('!', "to name not null-terminated: \"%s\"",buf);
                break;
            }
	}

        if (aread(buf, sizeof(buf)-1, fp)) {
            if (strlen(buf) > 36)
	        Syslog('!', "from name too long (%d) \"%s\"", strlen(buf), printable(buf, 0));
            fname = xstrcpy(buf);
            if (aread(buf, sizeof(buf)-1, fp)) {
                if (*(p = fname + strlen(fname)-1) == '\n') 
	            *p = '\0';
                Syslog('!', "from name not null-terminated: \"%s\"",buf);
                break;
            }
        }

        if (aread(buf, sizeof(buf)-1, fp)) {
            if (strlen(buf) > 72)
	        Syslog('!', "subject too long (%d) \"%s\"", strlen(buf), printable(buf, 0));
            subj = xstrcpy(buf);
            if (aread(buf, sizeof(buf)-1, fp)) {
		if (*(p = subj + strlen(subj)-1) == '\n') 
	            *p = '\0';
                subj = xstrcat(subj,(char *)"\\n");
                subj = xstrcat(subj,buf);
                Syslog('!', "subj not null-terminated: \"%s\"",buf);
                return 3;
            }
        }

        if (feof(fp) || ferror(fp)) {
            Syslog('!', "Could not read message header, aborting");
	    break;
	}

       /*
	* Read the text from the .pkt file
	*/
	line = 0;
	echo = FALSE;
        while (aread(buf, sizeof(buf)-1, fp)) {
	    line++;
	    for (i = 0; i < strlen(buf); i++) {
		if (*(buf + 1) == '\0')
		    break;
		if (*(buf + i) == '\n')
		    *(buf + i) = '\0';
	    }
	    if (line == 1) {
		if (strncmp(buf, (char *)"AREA:", 5) == 0) {
		    echo = TRUE;
		    echomail++;
		    area = xstrcpy(buf + 5);
		} else {
		    netmail++;
		}
	    }
	    if (strncmp(buf, (char *)"\001PID: ", 6) == 0) {
		pid = xstrcpy(buf + 6);
	    }
	    if (strncmp(buf, (char *)"\001TID: ", 6) == 0) {
		tid = xstrcpy(buf + 6);
	    }
	    if (strncmp(buf, (char *)"\001CHRS: ", 7) == 0) {
		chrs = xstrcpy(buf + 7);
	    } else if (strncmp(buf, (char *)"\001CHARSET: ", 10) == 0) {
		chrs = xstrcpy(buf +10);
	    }
	    if (!strncmp(buf, " * Origin:",10)) {
		p = buf + 10;
		while (*p == ' ')
		    p++;
		if ((l=strrchr(p,'(')) && (r=strrchr(p,')')) && (l < r)) {
		    *l = '\0';
		    *r = '\0';
		    l++;
		    orig = xstrcpy(l);
		}
	    }
	}

	/*
	 * All data collected, write results.
	 */
	if (echo) {
	    if (csvfile) {
		if ((op = fopen(csvfile, "a"))) {
		    fprintf(op, "\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\"\r\n",
			PE_SS(orig), rfcdate(mdate), tname, fname, subj, area, PE_SS(pid), PE_SS(tid), PE_SS(chrs));
		    fclose(op);
		}
	    }
	    if (sqlfile && sqltable) {
		if ((op = fopen(sqlfile, "a"))) {
		    sql = calloc(PATH_MAX, sizeof(char));
		    sprintf(sql, "INSERT INTO %s SET ", sqltable);
		    if (orig) {
			sprintf(sql + strlen(sql), "origin='%s', ", orig);
		    }
		    sprintf(sql + strlen(sql), "date='%s'", rfcdate(mdate));
		    sprintf(sql + strlen(sql), ", f_to='%s'", sqlescape(tname));
		    sprintf(sql + strlen(sql), ", f_from='%s'", sqlescape(fname));
		    if (subj) {
			sprintf(sql + strlen(sql), ", subject='%s'", sqlescape(subj));
		    }
		    if (area) {
			sprintf(sql + strlen(sql), ", area='%s'", area);
		    }
		    if (pid) {
			sprintf(sql + strlen(sql), ", pid='%s'", sqlescape(pid));
		    }
		    if (tid) {
			sprintf(sql + strlen(sql), ", tid='%s'", sqlescape(tid));
		    }
		    if (chrs) {
			sprintf(sql + strlen(sql), ", charset='%s'", sqlescape(chrs));
		    }
		    sprintf(sql + strlen(sql), ";\n");

		    fprintf(op, sql);
		    free(sql);
		    fclose(op);
		}
	    }
	}

	/*
	 * Cleanup for next message
	 */
	if (subj)
	    free(subj);
	subj = NULL;
	if (fname)
	    free(fname);
	fname = NULL;
	if (tname)
	    free(tname);
	tname = NULL;
	if (area)
	    free(area);
	area = NULL;
	if (pid)
	    free(pid);
	pid = NULL;
	if (tid)
	    free(tid);
	tid = NULL;
	if (orig)
	    free(orig);
	orig = NULL;
	if (chrs)
	    free(chrs);
	chrs = NULL;
    }

    Syslog('+', "Messages : netmail %d, echomail %d", netmail, echomail);
    die(0);
    return 0;
}



