#include "stp_info.h"
#include "cli.h"
#include "cparser.h"

/******************************************************/
int spanning_tree_enable (void);
int spanning_tree_disable (void);
int set_spanning_bridge_port_path_cost (uint32_t path_cost, uint32_t portnum);
int set_spanning_bridge_port_prio (uint32_t prio, uint32_t portnum);
int  show_spanning_tree  (void);
int vlan_spanning_tree_enable_on_vlan (int vlan_id, int mode);
int vlan_spanning_tree_disable_on_vlan (int vlan_id, int mode);
struct stp_instance * get_this_bridge_entry (uint16_t vlan_id);
/******************************************************/

extern struct list_head stp_instance_head;

cparser_result_t cparser_cmd_show_spanning_tree(cparser_context_t *context)
{
	if (!show_spanning_tree ())
		return CPARSER_OK;
	return CPARSER_NOT_OK;
}
cparser_result_t cparser_cmd_config_spanning_tree(cparser_context_t *context)
{
	if (!spanning_tree_enable ())
		return CPARSER_OK;
	return CPARSER_NOT_OK;
}
cparser_result_t cparser_cmd_config_spanning_tree_priority_priority(cparser_context_t *context, int32_t *priority_ptr)
{
	if (!stp_set_bridge_priority (*priority_ptr, cli_get_vlan_id ()))
		return CPARSER_OK;
	return CPARSER_NOT_OK;
}

cparser_result_t cparser_cmd_config_spanning_tree_hello_time_htimesecs_forward_delay_fdlysecs_max_age_maxagesecs (
    cparser_context_t *context,
    int32_t *htimesecs_ptr,
    int32_t *fdlysecs_ptr,
    int32_t *maxagesecs_ptr)
{
	if (!fdlysecs_ptr && !maxagesecs_ptr) 
	{
		if (!stp_set_bridge_hello_time (*htimesecs_ptr, cli_get_vlan_id ()))
			return CPARSER_OK;
		return CPARSER_NOT_OK;
	}

        if (*htimesecs_ptr < STP_MIN_HELLO_TIME || *htimesecs_ptr > STP_MAX_HELLO_TIME)
        {
                printf ("Invaild Spanning tree Hello time. Valid range %d-%d\n",
                        STP_MIN_HELLO_TIME, STP_MAX_HELLO_TIME);
		return CPARSER_NOT_OK;
        }

	if (fdlysecs_ptr) {
		if (*fdlysecs_ptr < STP_MIN_FORWARD_DELAY || *fdlysecs_ptr > STP_MAX_FORWARD_DELAY) {
			printf ("Invaild Spanning tree Forward Delay. Valid range %d-%d\n",
				STP_MIN_FORWARD_DELAY, STP_MAX_FORWARD_DELAY);
			return CPARSER_NOT_OK;
		}
	}
	
	if (maxagesecs_ptr) {
		if (*maxagesecs_ptr < STP_MIN_MAX_AGE || *maxagesecs_ptr > STP_MAX_MAX_AGE)         {
			printf ("Invaild Spanning tree max age. Valid range %d-%d\n",
				STP_MIN_MAX_AGE, STP_MAX_MAX_AGE);
			return CPARSER_NOT_OK;
		}
	}

	if (!stp_set_bridge_times ((fdlysecs_ptr? *fdlysecs_ptr : -1), (maxagesecs_ptr? *maxagesecs_ptr : -1),
				  (htimesecs_ptr? *htimesecs_ptr : -1), cli_get_vlan_id()))
		return CPARSER_NOT_OK;
	return CPARSER_NOT_OK;
}

cparser_result_t cparser_cmd_config_spanning_tree_forward_delay_fdlysecs_max_age_maxagesecs_hello_time_htimesecs(
    cparser_context_t *context,
    int32_t *fdlysecs_ptr,
    int32_t *maxagesecs_ptr,
    int32_t *htimesecs_ptr)
{
	if (!htimesecs_ptr && !maxagesecs_ptr) 
	{
		if (!stp_set_bridge_forward_delay (*fdlysecs_ptr, cli_get_vlan_id ()))
			return CPARSER_OK;
		return CPARSER_NOT_OK;
	}

	if (*fdlysecs_ptr < STP_MIN_FORWARD_DELAY || *fdlysecs_ptr > STP_MAX_FORWARD_DELAY) {
		printf ("Invaild Spanning tree Forward Delay. Valid range %d-%d\n",
			STP_MIN_FORWARD_DELAY, STP_MAX_FORWARD_DELAY);
		return CPARSER_NOT_OK;
	}


	if (htimesecs_ptr) {
		if (*htimesecs_ptr < STP_MIN_HELLO_TIME || *htimesecs_ptr > STP_MAX_HELLO_TIME)
		{
			printf ("Invaild Spanning tree Hello time. Valid range %d-%d\n",
					STP_MIN_HELLO_TIME, STP_MAX_HELLO_TIME);
			return CPARSER_NOT_OK;
		}
	}

	
	if (maxagesecs_ptr) {
		if (*maxagesecs_ptr < STP_MIN_MAX_AGE || *maxagesecs_ptr > STP_MAX_MAX_AGE)         {
			printf ("Invaild Spanning tree max age. Valid range %d-%d\n",
				STP_MIN_MAX_AGE, STP_MAX_MAX_AGE);
			return CPARSER_NOT_OK;
		}
	}

	if (!stp_set_bridge_times ((fdlysecs_ptr? *fdlysecs_ptr : -1), (maxagesecs_ptr? *maxagesecs_ptr : -1),
				  (htimesecs_ptr? *htimesecs_ptr : -1), cli_get_vlan_id()))
		return CPARSER_NOT_OK;
	return CPARSER_NOT_OK;
}
cparser_result_t cparser_cmd_config_spanning_tree_max_age_maxagesecs_forward_delay_fdlysecs_hello_time_htimesecs(
    cparser_context_t *context,
    int32_t *maxagesecs_ptr,
    int32_t *fdlysecs_ptr,
    int32_t *htimesecs_ptr)
{
        if (!fdlysecs_ptr && !htimesecs_ptr)
        {
                if (!stp_set_bridge_max_age (*maxagesecs_ptr, cli_get_vlan_id ()))
                        return CPARSER_OK;
                return CPARSER_NOT_OK;
        }

	if (*maxagesecs_ptr < STP_MIN_MAX_AGE || *maxagesecs_ptr > STP_MAX_MAX_AGE)         {
		printf ("Invaild Spanning tree max age. Valid range %d-%d\n",
				STP_MIN_MAX_AGE, STP_MAX_MAX_AGE);
		return CPARSER_NOT_OK;
	}

        if (fdlysecs_ptr) {
                if (*fdlysecs_ptr < STP_MIN_FORWARD_DELAY || *fdlysecs_ptr > STP_MAX_FORWARD_DELAY) {
                        printf ("Invaild Spanning tree Forward Delay. Valid range %d-%d\n",
                                STP_MIN_FORWARD_DELAY, STP_MAX_FORWARD_DELAY);
                        return CPARSER_NOT_OK;
                }
        }

        if (htimesecs_ptr) {
                if (*htimesecs_ptr < STP_MIN_HELLO_TIME || *htimesecs_ptr > STP_MAX_HELLO_TIME)
                {
                        printf ("Invaild Spanning tree Hello time. Valid range %d-%d\n",
                                        STP_MIN_HELLO_TIME, STP_MAX_HELLO_TIME);
                        return CPARSER_NOT_OK;
                }
        }

	if (!stp_set_bridge_times ((fdlysecs_ptr? *fdlysecs_ptr : -1), (maxagesecs_ptr? *maxagesecs_ptr : -1),
				  (htimesecs_ptr? *htimesecs_ptr : -1), cli_get_vlan_id()))
		return CPARSER_NOT_OK;
	return CPARSER_NOT_OK;
}
cparser_result_t cparser_cmd_config_spanning_tree_ethernet_portnum_path_cost_cost(cparser_context_t *context,
    int32_t *portnum_ptr,
    int32_t *cost_ptr)
{
	if (!set_spanning_bridge_port_path_cost (*cost_ptr, *portnum_ptr))
		return CPARSER_OK;
	return CPARSER_NOT_OK;
}
cparser_result_t cparser_cmd_config_spanning_tree_ethernet_portnum_priority_priority(cparser_context_t *context,
    int32_t *portnum_ptr,
    int32_t *priority_ptr)
{
	if (!set_spanning_bridge_port_prio (*priority_ptr, *portnum_ptr))
		return CPARSER_OK;
	return CPARSER_NOT_OK;
}
cparser_result_t cparser_cmd_config_no_spanning_tree(cparser_context_t *context)
{
	if (!spanning_tree_disable ())
		return CPARSER_OK;
	return CPARSER_NOT_OK;
}

int  show_spanning_tree  (void)
{
	struct stp_instance *pstp_inst = NULL;

	list_for_each_entry(pstp_inst, &stp_instance_head, next) {

		if (pstp_inst->stp_enabled) {

			uint8_t *mac = NULL;
			struct stp_port_entry *p =  NULL;

			int is_root = stp_is_root_bridge (pstp_inst);

			printf ("\n  Spanning tree enabled protocol ieee on\n");
			printf ("  -------------------------------------- \n\n");

			printf ("  VLAN  : %d\n\n", pstp_inst->vlan_id);

			printf ("  Root ID\n\tPriority    %d\n", pstp_inst->designated_root.prio);

			mac = pstp_inst->designated_root.addr;

			printf ("\tAddress     %02x:%02x:%02x:%02x:%02x:%02x\n", 
				mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);

			if (is_root)  {
				printf ("\tThis bridge is the root\n");
			}

			printf ("\tHello Time  %d sec  Max Age %d sec  Forward Delay %d sec\n\n",
					pstp_inst->hello_time, pstp_inst->max_age, pstp_inst->forward_delay);

			printf ("  Bridge ID\n\tPriority    %d\n",pstp_inst->bridge_id.prio);

			mac = pstp_inst->bridge_id.addr;

			printf ("\tAddress     %02x:%02x:%02x:%02x:%02x:%02x\n", 
					mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
			printf ("\tHello Time  %d sec  Max Age %d sec  Forward Delay %d sec\n",
				pstp_inst->bridge_hello_time, pstp_inst->bridge_max_age, 
				pstp_inst->bridge_forward_delay);

			if (!is_root) {
				printf ("\n\tRoot Port : %d\n", pstp_inst->root_port);
			}

			if (!list_empty (&pstp_inst->port_list)) {
				const char *state[] = {"DISABLED", "LISTENING", "LEARNING", 
					               "FORWARDING", "BLOCKING"};
				printf ("\nPort   Cost     State      Bridge Id         Prio \n");
				printf ("----   -----   ------   -----------------    ------\n");
				list_for_each_entry(p, &pstp_inst->port_list, list) {
					mac = p->designated_bridge.addr;
					printf ("%2d   %4d   %10s   %02x:%02x:%02x:%02x:%02x:%02x  %4d\n",
						p->port_no, p->path_cost, state[p->state],
						mac[0],mac[1],mac[2],mac[3],mac[4],mac[5],p->priority);
				}
			} 	
		}
		else {
			printf ("\n  Spanning tree not enabled on");
			printf (" VLAN  : %d\n\n", pstp_inst->vlan_id);
		}
	}
}

int spanning_tree_enable (void)
{
	vlan_spanning_tree_enable_on_vlan (cli_get_vlan_id (), MODE_STP);
}
int spanning_tree_disable (void)
{
	vlan_spanning_tree_disable_on_vlan (cli_get_vlan_id (), MODE_STP);
}
int set_spanning_bridge_port_path_cost (uint32_t path_cost, uint32_t portnum)
{
	struct stp_instance *br = get_this_bridge_entry (cli_get_vlan_id ());
	struct stp_port_entry *p = NULL;

	if (!br)
	{
		printf ("Spanning-tree not enabled\n");
		return -1;
	}

	if (!(p = stp_get_port_info (br, portnum)))
	{
		printf ("Invalid Port Number\n");
		return -1;
	}

	if (path_cost < STP_MIN_PATH_COST || path_cost > STP_MAX_PATH_COST)
	{
		printf ("Invaild spanning tree port path-cost. Valid range %d-%d\n", 
			STP_MIN_PATH_COST, STP_MAX_PATH_COST);
		return -1;
	}

	stp_set_path_cost (p, path_cost);

	return 0;
}

int set_spanning_bridge_port_prio (uint32_t prio, uint32_t portnum)
{
	struct stp_instance *br = get_this_bridge_entry (cli_get_vlan_id ());
	struct stp_port_entry *p = NULL;

	if (!br)
	{
		printf ("Spanning-tree not enabled\n");
		return -1;
	}

	if (!(p = stp_get_port_info (br, portnum)))
	{
		printf ("Invalid Port Number\n");
		return -1;
	}

	if (prio > STP_MAX_PORT_PRIORITY)
	{
		printf ("Invaild spanning tree port priority. Valid Range 0-240\n");
		return -1;
	}

	stp_set_port_priority (p, prio);

	return 0;
}
