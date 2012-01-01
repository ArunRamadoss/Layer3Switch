/*
 *  Author:
 *  Sasikanth.V        <sasikanth@email.com>
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version
 *  2 of the License, or (at your option) any later version.
 */

//CPU uage calucation code Last modified: 04/04/11 12:34:52(CEST) by Fabian Holler

#include <stdio.h>
#include "common_types.h"

extern struct list_head      tsk_hd;

static TIMER_ID cpu_timer;

struct pstat_temp
{
	long unsigned int utime;
	long unsigned int stime;
	long unsigned int tcpu;
	long int cstime;
	long int cutime;
};
int init_task_cpu_usage_moniter_timer (void);
void track_cpu_usage (void *);
int  show_cpu_usage (void);
void calc_cpu_usage (struct pstat* curr, struct pstat* lst, float* ucpu_usage, float* scpu_usage, float *tcpu);
int get_usage(const pid_t pid, struct pstat_temp* result);

void track_cpu_usage (void *unused)
{
	register struct list_head *node = NULL;
	register tmtask_t  *tskinfo = NULL;

	list_for_each (node, (&tsk_hd))
	{
		struct pstat_temp current;

		memset (&current, 0, sizeof(current));

		tskinfo = (tmtask_t *) node;

		get_usage (tskinfo->tsk_pid, &current);

		tskinfo->cpu_stats.utime = current.utime + current.cutime;
		tskinfo->cpu_stats.stime = current.stime + current.cstime;
		tskinfo->cpu_stats.tcpu = current.tcpu;
	}
	mod_timer (cpu_timer, 1);
}


int init_task_cpu_usage_moniter_timer (void)
{
	setup_timer (&cpu_timer, track_cpu_usage, NULL);

	mod_timer (cpu_timer, 5);

	return 0;
}

int show_cpu_usage (void)
{
	register struct list_head *node = NULL;
	register tmtask_t  *tskinfo = NULL;

	printf
		("\n  Task Name         CPU User          System \n");
	printf
		("\r  ---------        ---------         --------\n");


	list_for_each (node, (&tsk_hd))
	{
		float user_usage = 0.0, system_usage = 0.0, tcpu = 0.0;
		struct pstat_temp current;
		struct pstat curr;;

		memset (&current, 0, sizeof(current));

		tskinfo = (tmtask_t *) node;

		get_usage (tskinfo->tsk_pid, &current);

		curr.utime = current.utime + current.cutime;
		curr.stime = current.stime + current.cstime;
		curr.tcpu = current.tcpu;

		calc_cpu_usage (&curr, &tskinfo->cpu_stats, &user_usage, &system_usage , & tcpu) ;

		printf ("%10s     %10.1f       %10.1f\n", tskinfo->task_name, user_usage * 2, system_usage);
	}
	return 0;
}

int get_usage(const pid_t pid, struct pstat_temp* result)
{
	char pid_s[20];
	char stat_filepath[30] = "/proc/self/task/";
	int i =0;
	FILE *fpstat = NULL;
	FILE *fstat = NULL;
	long unsigned int cpu_time[10];

	memset(cpu_time, 0, sizeof(cpu_time));

	snprintf(pid_s, sizeof(pid_s), "%d", pid);
	strncat(stat_filepath, pid_s, sizeof(stat_filepath) - strlen(stat_filepath) -1);
	strncat(stat_filepath, "/stat", sizeof(stat_filepath) - strlen(stat_filepath) -1);

	fpstat = fopen(stat_filepath, "r");

	if(!fpstat){
		return -1;
	}

	fstat = fopen("/proc/stat", "r");
	if(!fstat){
		fclose(fstat);
		return -1;
	}
	bzero(result, sizeof(struct pstat));

	if(fscanf(fpstat, "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %lu %lu %ld %ld", 
		  &result->utime, &result->stime, &result->cutime, &result->cstime) == EOF){
		fclose(fpstat);
		fclose(fstat);
		return -1;
	}
	fclose(fpstat);

	//read+calc cpu total time from /proc/stat, on linux 2.6.35-23 x86_64 the cpu row has 10values could differ on different architectures :/
	if(fscanf(fstat, "%*s %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu", 
		         &cpu_time[0], &cpu_time[1], &cpu_time[2], &cpu_time[3], &cpu_time[4], &cpu_time[5], &cpu_time[6], 
			&cpu_time[7], &cpu_time[8], &cpu_time[9]) == EOF){
		fclose(fstat);
		return -1;
	}
	fclose(fstat);

	for(i=0; i < 10;i++){
		result->tcpu += cpu_time[i];
	}

	return 0;
}

/*
 * calculates the actual CPU usage(curr - lst) in percent
 * curr, lst: both last measured get_usage() results
 * ucpu_usage, scpu_usage: result parameters: user and sys cpu usage in %
 */
void calc_cpu_usage (struct pstat* curr, struct pstat* lst, float* ucpu_usage, float* scpu_usage, float *tcpu)
{
	*ucpu_usage = ((100 * (curr->utime - lst->utime )) / (float)((curr->tcpu - lst->tcpu)));
	*scpu_usage = ((100 * (curr->stime - lst->stime))  / (float)((curr->tcpu - lst->tcpu)));

	*tcpu = ((100 * ((curr->utime + curr->stime) -(lst->utime + lst->stime))) / (float)((curr->tcpu - lst->tcpu)));
}

