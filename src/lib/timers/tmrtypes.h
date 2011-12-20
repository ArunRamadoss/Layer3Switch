/*
 *  Author:
 *  Sasikanth.V        <sasikanth@email.com>
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version
 *  2 of the License, or (at your option) any later version.
 */


enum {
	TICK = 0,
	SEC,
	MIN,
	HRS
};

enum {
	TICK_TIMERS = 0,
	SECS_TIMERS,
	MIN_TIMERS,
	HRS_TIMERS,
	TIMER_WHEEL,
	WAIT_TIMERS
};

struct active_timers
{
	struct rb_root   root[TIMER_WHEEL]; 
	unsigned int count;
};

struct wait_tmr {
	struct rb_root   root; 
	unsigned int count;
};


typedef struct tm_timer
{
	unsigned int    rmt;
	unsigned int    exp;
	int             wheel;
	int 	        idx;
	int	        flags;
	int 		is_running;
	void           *data;
	void           *apptimer;
 	void           (*time_out_handler)(void *);
}TIMER_T;

typedef struct app_timer {
	struct rb_node rlist; 
	struct list_head elist; 
	TIMER_T	      *timer;
	unsigned int    ctime;
	unsigned int    rt;
}APP_TIMER_T;



#define TIMER_MGR_RUNNING       2
#define TIMER_MGR_SHUTDOWN      1

#define CALCULATE_REM_TIME(ptmr)     (ptmr->duration - clk[TICK])
#define SET_TIMER_MGR_STATE(state)    tmrmgrstate = state

#define QUERY_TIMER_EXPIRY  0x1
#define QUERY_TIMER_INDEX   0x2

#define TIMER_ONCE  	0x1
#define TIMER_REPEAT  	0x2 
#define TIMER_FOREVER   0x4


/*
 TIME FORMAT  32-BIT
-----------------------------------------
| 10 bits   | 6 bits| 6 bits|  10 bits  |
-----------------------------------------
    HRS        MINS    SECS     TICKS
*/

#define TICK_BITS       0xa
#define SECS_BITS       0x6
#define MINS_BITS       0x6
#define HRS_BITS        0xa

#define SEC_BITS_OFFSET TICK_BITS
#define MIN_BITS_OFFSET (SEC_BITS_OFFSET + SECS_BITS)
#define HRS_BITS_OFFSET (MIN_BITS_OFFSET + MINS_BITS)

#define SET_TICK(ct, t)  ct |= (t % tm_get_ticks_per_second())
#define SET_SECS(ct, t)  ct |= ((t % 60) << SEC_BITS_OFFSET)
#define SET_MINS(ct, t)  ct |= ((t % 60) << MIN_BITS_OFFSET)
#define SET_HRS(ct, t)   ct |= (t << HRS_BITS_OFFSET)

#define GET_TICK(ct, t)  (t = (~(~0U << TICK_BITS) & ct))
#define GET_SECS(ct, t)  (t = ((~(~0U << SECS_BITS)) & (ct >> SEC_BITS_OFFSET)))
#define GET_MINS(ct, t)  (t = ((~(~0U << MINS_BITS)) & (ct >> MIN_BITS_OFFSET)))
#define GET_HRS(ct, t)   (t = ((~(~0U << HRS_BITS))  & (ct >> HRS_BITS_OFFSET)))

#define INC_TIMER_COUNT()   ++tmrrq.count         
#define DEC_TIMER_COUNT()   --tmrrq.count         
#define TIMER_COUNT()      tmrrq.count

#define IS_NXT_SEC_HAPPEND     !(clk[TICK] % tm_get_ticks_per_second ())
#define IS_NXT_MIN_HAPPEND     !(clk[SEC] ? (clk[SEC] % 60) : 1)
#define IS_NXT_HR_HAPPEND      !(clk[MIN] ? (clk[MIN] % 60) : 1)

extern struct active_timers  tmrrq;
extern struct list_head expd_tmrs;
extern unsigned int clk[];

int tm_process_tick_and_update_timers (void);
void process_expd_timers (void);
void free_timer_id ();
int find_tmr_slot_and_place (APP_TIMER_T * ptmr);
void timer_del (APP_TIMER_T *n, struct rb_root *root);
void timer_add (APP_TIMER_T *n, struct rb_root *root, int);
