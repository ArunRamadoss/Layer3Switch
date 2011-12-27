/*
 *  Author:
 *  Sasikanth.V        <sasikanth@email.com>
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version
 *  2 of the License, or (at your option) any later version.
 */

#include "common_types.h"
#include "vrrpd.h"

int32_t vrrp_pool_id = -1;

int vrrp_init (void)
{
	vrrp_pool_id = mem_pool_create ("VRRP",  MAX_VRRP_INSTANCE * sizeof(vrrp_rt), 
					 MAX_VRRP_INSTANCE, 0);

	if (vrrp_pool_id < 0) {
		return -1;
	}

	vrrp_cli_init ();

	return 0;
}

static inline void init_vsrv(vrrp_rt *vsrv)
{
	vsrv->oper_state = VRRP_STATE_INIT;
	vsrv->priority	= VRRP_PRIO_DFL;
	vsrv->adver_int	= VRRP_ADVER_DFL*VRRP_TIMER_HZ;
	vsrv->preempt	= VRRP_PREEMPT_DFL;
}


int vrrp_init_instance (int vrid)
{
	vrrp_rt *vsrv = NULL;

	if (vrid < 1 && vrid > MAX_VRRP_INSTANCE)
		return -1;


        vsrv =  alloc_block (vrrp_pool_id);

        if (!vsrv) {
                return -1;
        }

	memset (vsrv, 0, sizeof(*vsrv));

	vsrv->vrid = vrid;

	init_vsrv (vrid);

	return 0;
}
