/*
 *  Author:
 *  Sasikanth.V        <sasikanth@email.com>
 *
 *  $Id: lock.c,v 1.5 2011/01/26 20:14:19 Sasi Exp $
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version
 *  2 of the License, or (at your option) any later version.
*/

#include <stdio.h>
#include <setjmp.h>
#include <time.h>
#include <sys/time.h>
#include <signal.h>
#include <errno.h>
#include "list.h"
#include "sched_sfs.h"

int create_sync_lock (sync_lock_t *slock)
{
	if (!slock)
		return -1;
	if (sem_init(slock, 0, 1) < 0) {
		perror ("SEM_INIT: ");
		return -1;
	}
	return 0;
}

int sync_lock (sync_lock_t *slock)
{
	while (sem_wait (slock) < 0)  {
		/*signal interrupts*/
		if (errno == EINTR) {
			continue;
		}
	}

	return 0;
}
int sync_unlock (sync_lock_t *slock)
{
	if (sem_post (slock) < 0) {
		perror ("SEM_POST: ");
		return -1;
	}
	return 0;
}
