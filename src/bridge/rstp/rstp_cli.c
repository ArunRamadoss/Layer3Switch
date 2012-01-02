#include "cli.h"
#include "bridgestp.h"
#include "cparser.h"


int set_spanning_8021w_port_prio (uint32_t prio, int portnum);
int set_spanning_8021w_port_path_cost (uint32_t path_cost, int portnum);
int show_spanning_tree_8021w (void);
int spanning_8021w_tree_enable (void);
int spanning_8021w_tree_disable (void);

extern struct list_head bstp_list;

cparser_result_t cparser_cmd_show_rstp(cparser_context_t *context)
{
	if (!show_spanning_tree_8021w ())
		return CPARSER_OK;
	return CPARSER_NOT_OK;
}
cparser_result_t cparser_cmd_config_spanning_tree_rstp (cparser_context_t *context)
{
	if (!spanning_8021w_tree_enable ())
		return CPARSER_OK;
	return CPARSER_NOT_OK;
}
cparser_result_t cparser_cmd_config_spanning_tree_rstp_priority_priority(cparser_context_t *context, int32_t *priority_ptr)
{
	if (!rstp_set_bridge_priority (*priority_ptr, cli_get_vlan_id ()))
		return CPARSER_OK;
	return CPARSER_NOT_OK;
}

cparser_result_t cparser_cmd_config_spanning_tree_rstp_hello_time_htimesecs_forward_delay_fdlysecs_max_age_maxagesecs (
    cparser_context_t *context,
    int32_t *htimesecs_ptr,
    int32_t *fdlysecs_ptr,
    int32_t *maxagesecs_ptr)
{
	if (!fdlysecs_ptr && !maxagesecs_ptr) 
	{
		if (!rstp_set_bridge_hello_time (*htimesecs_ptr, cli_get_vlan_id ()))
			return CPARSER_OK;
		return CPARSER_NOT_OK;
	}

        if (*htimesecs_ptr < BSTP_MIN_HELLO_TIME || *htimesecs_ptr > BSTP_MAX_HELLO_TIME)
        {
                printf ("Invaild Rapid Spanning tree Hello time. Valid range %d-%d\n",
                        BSTP_MIN_HELLO_TIME, BSTP_MAX_HELLO_TIME);
		return CPARSER_NOT_OK;
        }

	if (fdlysecs_ptr) {
		if (*fdlysecs_ptr < BSTP_MIN_FORWARD_DELAY || *fdlysecs_ptr > BSTP_MAX_FORWARD_DELAY) {
			printf ("Invaild Rapid Spanning tree Forward Delay. Valid range %d-%d\n",
				BSTP_MIN_FORWARD_DELAY, BSTP_MAX_FORWARD_DELAY);
			return CPARSER_NOT_OK;
		}
	}
	
	if (maxagesecs_ptr) {
		if (*maxagesecs_ptr < BSTP_MIN_MAX_AGE || *maxagesecs_ptr > BSTP_MAX_MAX_AGE) {
			printf ("Invaild Rapid Spanning tree max age. Valid range %d-%d\n",
				BSTP_MIN_MAX_AGE, BSTP_MAX_MAX_AGE);
			return CPARSER_NOT_OK;
		}
	}

	if (!rstp_set_bridge_times ((fdlysecs_ptr? *fdlysecs_ptr : -1), (maxagesecs_ptr? *maxagesecs_ptr : -1),
				  (htimesecs_ptr? *htimesecs_ptr : -1), cli_get_vlan_id()))
		return CPARSER_NOT_OK;
	return CPARSER_NOT_OK;
}

cparser_result_t cparser_cmd_config_spanning_tree_rstp_forward_delay_fdlysecs_max_age_maxagesecs_hello_time_htimesecs(
    cparser_context_t *context,
    int32_t *fdlysecs_ptr,
    int32_t *maxagesecs_ptr,
    int32_t *htimesecs_ptr)
{
	if (!htimesecs_ptr && !maxagesecs_ptr) 
	{
		if (!rstp_set_bridge_fdly (*fdlysecs_ptr, cli_get_vlan_id ()))
			return CPARSER_OK;
		return CPARSER_NOT_OK;
	}

	if (*fdlysecs_ptr < BSTP_MIN_FORWARD_DELAY || *fdlysecs_ptr > BSTP_MAX_FORWARD_DELAY) {
		printf ("Invaild Rapid Spanning tree Forward Delay. Valid range %d-%d\n",
			BSTP_MIN_FORWARD_DELAY, BSTP_MAX_FORWARD_DELAY);
		return CPARSER_NOT_OK;
	}


	if (htimesecs_ptr) {
		if (*htimesecs_ptr < BSTP_MIN_HELLO_TIME || *htimesecs_ptr > BSTP_MAX_HELLO_TIME)
		{
			printf ("Invaild Rapid Spanning tree Hello time. Valid range %d-%d\n",
					BSTP_MIN_HELLO_TIME, BSTP_MAX_HELLO_TIME);
			return CPARSER_NOT_OK;
		}
	}

	
	if (maxagesecs_ptr) {
		if (*maxagesecs_ptr < BSTP_MIN_MAX_AGE || *maxagesecs_ptr > BSTP_MAX_MAX_AGE) {
			printf ("Invaild Rapid Spanning tree max age. Valid range %d-%d\n",
				BSTP_MIN_MAX_AGE, BSTP_MAX_MAX_AGE);
			return CPARSER_NOT_OK;
		}
	}

	if (!rstp_set_bridge_times ((fdlysecs_ptr? *fdlysecs_ptr : -1), (maxagesecs_ptr? *maxagesecs_ptr : -1),
				  (htimesecs_ptr? *htimesecs_ptr : -1), cli_get_vlan_id()))
		return CPARSER_NOT_OK;
	return CPARSER_NOT_OK;
}
cparser_result_t cparser_cmd_config_spanning_tree_rstp_max_age_maxagesecs_forward_delay_fdlysecs_hello_time_htimesecs(
    cparser_context_t *context,
    int32_t *maxagesecs_ptr,
    int32_t *fdlysecs_ptr,
    int32_t *htimesecs_ptr)
{
        if (!fdlysecs_ptr && !htimesecs_ptr)
        {
                if (!rstp_set_bridge_max_age (*maxagesecs_ptr, cli_get_vlan_id ()))
                        return CPARSER_OK;
                return CPARSER_NOT_OK;
        }

	if (*maxagesecs_ptr < BSTP_MIN_MAX_AGE || *maxagesecs_ptr > BSTP_MAX_MAX_AGE)         {
		printf ("Invaild Rapid Spanning tree max age. Valid range %d-%d\n",
				BSTP_MIN_MAX_AGE, BSTP_MAX_MAX_AGE);
		return CPARSER_NOT_OK;
	}

        if (fdlysecs_ptr) {
                if (*fdlysecs_ptr < BSTP_MIN_FORWARD_DELAY || *fdlysecs_ptr > BSTP_MAX_FORWARD_DELAY) {
                        printf ("Invaild Rapid Spanning tree Forward Delay. Valid range %d-%d\n",
                                BSTP_MIN_FORWARD_DELAY, BSTP_MAX_FORWARD_DELAY);
                        return CPARSER_NOT_OK;
                }
        }

        if (htimesecs_ptr) {
                if (*htimesecs_ptr < BSTP_MIN_HELLO_TIME || *htimesecs_ptr > BSTP_MAX_HELLO_TIME)
                {
                        printf ("Invaild Rapid Spanning tree Hello time. Valid range %d-%d\n",
                                        BSTP_MIN_HELLO_TIME, BSTP_MAX_HELLO_TIME);
                        return CPARSER_NOT_OK;
                }
        }

	if (!rstp_set_bridge_times ((fdlysecs_ptr? *fdlysecs_ptr : -1), (maxagesecs_ptr? *maxagesecs_ptr : -1),
				  (htimesecs_ptr? *htimesecs_ptr : -1), cli_get_vlan_id()))
		return CPARSER_NOT_OK;
	return CPARSER_NOT_OK;
}

cparser_result_t cparser_cmd_config_spanning_tree_rstp_ethernet_portnum_path_cost_cost(cparser_context_t *context,
    int32_t *portnum_ptr,
    int32_t *cost_ptr)
{
	if (!set_spanning_8021w_port_path_cost (*cost_ptr, *portnum_ptr))
		return CPARSER_OK;
	return CPARSER_NOT_OK;
}

cparser_result_t cparser_cmd_config_spanning_tree_rstp_ethernet_portnum_priority_priority(cparser_context_t *context,
    int32_t *portnum_ptr,
    int32_t *priority_ptr)
{
	if (!set_spanning_bridge_port_prio (*priority_ptr, *portnum_ptr))
		return CPARSER_OK;
	return CPARSER_NOT_OK;
}

cparser_result_t cparser_cmd_config_no_spanning_tree_rstp(cparser_context_t *context)
{
	if (!spanning_8021w_tree_disable ())
		return CPARSER_OK;
	return CPARSER_NOT_OK;
}

cparser_result_t cparser_cmd_config_spanning_tree_rstp_ethernet_portnum_admin_edge_port(cparser_context_t *context,
    int32_t *portnum_ptr)
{
	struct bstp_state *pinst =  rstp_get_this_bridge_entry (cli_get_vlan_id ());
	struct bstp_port *p = NULL;

	if (!pinst)
	{
		printf ("Rapid spanning not enabled\n");
		return CPARSER_NOT_OK;
	}

	if (!(p = rstp_get_port_entry (pinst, *portnum_ptr)))
	{
		printf ("Invalid Port Number\n");
		return CPARSER_NOT_OK;
	}

	if (bstp_set_edge (p, 1))
		return CPARSER_NOT_OK;

	return CPARSER_OK;
}

cparser_result_t cparser_cmd_config_spanning_tree_rstp_ethernet_portnum_admin_pt2pt(cparser_context_t *context,
    int32_t *portnum_ptr)
{
	struct bstp_state *pinst =  rstp_get_this_bridge_entry (cli_get_vlan_id ());
	struct bstp_port *p = NULL;

	if (!pinst)
	{
		printf ("Rapid spanning not enabled\n");
		return CPARSER_NOT_OK;
	}

	if (!(p = rstp_get_port_entry (pinst, *portnum_ptr)))
	{
		printf ("Invalid Port Number\n");
		return CPARSER_NOT_OK;
	}

	if (bstp_set_ptp (p, 1))
		return CPARSER_NOT_OK;

	return CPARSER_OK;
}



int show_spanning_tree_8021w (void)
{
	struct bstp_state             *pstp_inst = NULL;

	list_for_each_entry(pstp_inst, &bstp_list, bs_list) {

		if (pstp_inst->bs_running)
		{
			uint8_t              mac[6];
			int                 is_root = rstp_is_root_bridge (pstp_inst);

			printf ("\n  Rapid Spanning tree enabled protocol ieee on\n");
			printf ("  ------------------------------------------ \n\n");

			printf ("  VLAN  : %d\n\n", pstp_inst->vlan_id);

			printf ("  Root ID\n\tPriority    %d\n",
					pstp_inst->bs_root_pv.pv_root_id >> 48);

			PV2ADDR(pstp_inst->bs_root_pv.pv_root_id, mac);

			printf ("\tAddress     %02x:%02x:%02x:%02x:%02x:%02x\n",
					mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

			if (is_root)
			{
				printf ("\tThis bridge is the root\n");
			}

			printf
				("\tHello Time  %d sec  Max Age %d sec  Forward Delay %d sec\n\n",
				 pstp_inst->bs_root_htime, pstp_inst->bs_root_max_age,
				 pstp_inst->bs_root_fdelay);

			printf ("  Bridge ID\n\tPriority    %d\n", pstp_inst->bs_bridge_pv.pv_root_id >> 48);

			PV2ADDR(pstp_inst->bs_bridge_pv.pv_root_id, mac);

			printf ("\tAddress     %02x:%02x:%02x:%02x:%02x:%02x\n",
					mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

			printf
				("\tHello Time  %d sec  Max Age %d sec  Forward Delay %d sec\n\n",
				 pstp_inst->bs_bridge_htime,
				 pstp_inst->bs_bridge_max_age,
				 pstp_inst->bs_bridge_fdelay);

#if 0
			if (!is_root)
			{
				printf ("\n\tRoot Port : %d\n", pstp_inst->root_port);
			}
#endif
			if (!list_empty(&pstp_inst->bs_bplist))
			{
				struct bstp_port *p = NULL;
				char     *role[] = { "DISABLED", "ROOT", "DESIGNATED",
					  	     "ALTERNATE", "BACKUP"};
				char     *state[] = { "DISABLED", "LISTENING", "LEARNING","FORWARDING","BLOCKING","DISCARDING"};

				printf
				     ("\nPort     Cost       Role        State           Bridge Id    \n");
				printf
					("----   ------     --------    ----------     -----------------  \n");
				list_for_each_entry(p, &pstp_inst->bs_bplist, bp_next) {
					PV2ADDR(p->bp_desg_pv.pv_dbridge_id, mac);
					printf
						("%2d   %8d   %10s    %10s     %02x:%02x:%02x:%02x:%02x:%02x\n",
						 p->bp_ifp, 20000, role[p->bp_role], state[p->bp_state],mac[0], mac[1],
						 mac[2], mac[3], mac[4], mac[5]);
				}
			}
		}
		else
		{
			printf ("\n Rapid Spanning tree not enabled on");
			printf (" VLAN  : %d\n\n", pstp_inst->vlan_id);
		}
	}

	return 0;
}

int spanning_8021w_tree_enable (void)
{
    return vlan_spanning_tree_enable_on_vlan (cli_get_vlan_id (), MODE_RSTP);
}

int  spanning_8021w_tree_disable (void)
{
    return  vlan_spanning_tree_disable_on_vlan (cli_get_vlan_id (), MODE_RSTP);
}

int set_spanning_8021w_port_path_cost (uint32_t path_cost, int portnum)
{
	struct bstp_state *pinst =  rstp_get_this_bridge_entry (cli_get_vlan_id ());
	struct bstp_port *p = NULL;

	if (!pinst)
	{
		printf ("Rapid spanning not enabled\n");
		return -1;
	}

	if (!(p = rstp_get_port_entry (pinst, portnum)))
	{
		printf ("Invalid Port Number\n");
		return -1;
	}

	if (path_cost > BSTP_MAX_PATH_COST)
	{
		printf ("Invaild Rapid spanning tree port path-cost. Valid range 0-%d\n", 
			BSTP_MAX_PATH_COST);
		return -1;
	}

	return bstp_set_path_cost (p, path_cost);
}

int set_spanning_8021w_port_prio (uint32_t prio, int portnum)
{
	struct bstp_state *pinst =  rstp_get_this_bridge_entry (cli_get_vlan_id ());
	struct bstp_port *p = NULL;

	if (!pinst)
	{
		printf ("Rapid spanning not enabled\n");
		return;
	}

	if (!(p = rstp_get_port_entry (pinst, portnum)))
	{
		printf ("Invalid Port Number\n");
		return -1;
	}

	if (prio > BSTP_MAX_PORT_PRIORITY)
	{
		printf ("Invaild spanning tree port priority. Valid Range 0-240\n");
		return;
	}

	return bstp_set_port_priority (p, prio);
}
