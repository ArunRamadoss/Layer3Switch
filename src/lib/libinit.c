/*
 *  Author:
 *  Sasikanth.V        <sasikanth@email.com>
 *  $Id: tmlibinit.c,v 1.3 2011/01/26 20:14:19 Sasi Exp $
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version
 *  2 of the License, or (at your option) any later version.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/times.h>

int tmlib_init (void);
int mem_init (void);
int  msg_Q_init (void);
int init_timer_mgr (void);

int tmlib_init (void)
{
	mem_init ();

	msg_Q_init ();

	init_timer_mgr ();

	return 0;
}
