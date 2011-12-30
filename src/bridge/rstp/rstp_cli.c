#include "cli.h"
#include "bridgestp.h"

extern struct list_head bstp_list;

void
show_spanning_tree_8021w (void)
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
}

void
spanning_8021w_tree_enable (void)
{
    vlan_spanning_tree_enable_on_vlan (cli_get_vlan_id (), MODE_RSTP);
}

void
spanning_8021w_tree_disable (void)
{
    vlan_spanning_tree_disable_on_vlan (cli_get_vlan_id (), MODE_RSTP);
}

void
set_spanning_8021w_bridge_priority (char *args[])
{
    uint16_t            prio = (uint16_t) atoi (args[0]);

    rstp_set_bridge_priority (prio,cli_get_vlan_id ());
}

void
set_spanning_8021w_bridge_hello_time (char *args[])
{
    int                 hello = atoi (args[0]);

    rstp_set_bridge_hello_time (hello, cli_get_vlan_id ());
}

void
set_spanning_8021w_bridge_fwd_delay (char *args[])
{
    int                 fdly = atoi (args[0]);

    rstp_set_bridge_fdly (fdly, cli_get_vlan_id ());
}

void
set_spanning_8021w_bridge_max_age (char *args[])
{
    int                 max_age = atoi (args[0]);

    rstp_set_bridge_max_age (max_age, cli_get_vlan_id ());
}

void set_spanning_8021w_port_path_cost (char *args[])
{
	struct bstp_state *pinst =  rstp_get_this_bridge_entry (cli_get_vlan_id ());
	struct bstp_port *p = NULL;
	uint32_t path_cost = atoi (args[1]);

	if (!pinst)
	{
		printf ("Rapid spanning not enabled\n");
		return;
	}

	if (!(p = rstp_get_port_entry (pinst, atoi (args[0]))))
	{
		printf ("Invalid Port Number\n");
	}

	if (path_cost > BSTP_MAX_PATH_COST)
	{
		printf ("Invaild Rapid spanning tree port path-cost. Valid range 0-%d\n", 
			BSTP_MAX_PATH_COST);
		return;
	}

	bstp_set_path_cost (p, path_cost);
}

void set_spanning_8021w_port_prio (char *args[])
{
	struct bstp_state *pinst =  rstp_get_this_bridge_entry (cli_get_vlan_id ());
	struct bstp_port *p = NULL;
	uint32_t prio = atoi (args[1]);

	if (!pinst)
	{
		printf ("Rapid spanning not enabled\n");
		return;
	}

	if (!(p = rstp_get_port_entry (pinst, atoi (args[0]))))
	{
		printf ("Invalid Port Number\n");
	}

	if (prio > BSTP_MAX_PORT_PRIORITY)
	{
		printf ("Invaild spanning tree port priority. Valid Range 0-240\n");
		return;
	}

	bstp_set_port_priority (p, prio);
}


void set_spanning_8021w_port_admin_edge (char *args[])
{
	struct bstp_state *pinst =  rstp_get_this_bridge_entry (cli_get_vlan_id ());
	struct bstp_port *p = NULL;

	if (!pinst)
	{
		printf ("Rapid spanning not enabled\n");
		return;
	}

	if (!(p = rstp_get_port_entry (pinst, atoi (args[0]))))
	{
		printf ("Invalid Port Number\n");
	}

	bstp_set_edge (p, 1);
}

void set_spanning_8021w_port_admin_p2p (char *args[])
{
	struct bstp_state *pinst =  rstp_get_this_bridge_entry (cli_get_vlan_id ());
	struct bstp_port *p = NULL;

	if (!pinst)
	{
		printf ("Rapid spanning not enabled\n");
		return;
	}

	if (!(p = rstp_get_port_entry (pinst, atoi (args[0]))))
	{
		printf ("Invalid Port Number\n");
	}

	bstp_set_ptp (p, 1);
}



int
rstp_cli_init_cmd ()
{
    install_cmd_handler ("spanning-tree rstp", "Enables Rapid Spanning Tree",
                         spanning_8021w_tree_enable, NULL,
                         GLOBAL_CONFIG_MODE | VLAN_CONFIG_MODE);

    install_cmd_handler ("no spanning-tree rstp", "Disables Rapid Spanning Tree",
                         spanning_8021w_tree_disable, NULL,
                         GLOBAL_CONFIG_MODE | VLAN_CONFIG_MODE);

    install_cmd_handler ("spanning-tree rstp priority <prio>",
                         "Sets Rapid Spanning Priority <0-65535>",
                         set_spanning_8021w_bridge_priority,
                         "spanning-tree rstp priority <INT>",
                         GLOBAL_CONFIG_MODE | VLAN_CONFIG_MODE);

    install_cmd_handler ("spanning-tree rstp hello-time <secs>",
                         "Sets Rapid Spanning Hello time <1-10 secs>",
                         set_spanning_8021w_bridge_hello_time,
                         "spanning-tree rstp hello-time <INT>",
                         GLOBAL_CONFIG_MODE | VLAN_CONFIG_MODE);

    install_cmd_handler ("spanning-tree rstp forward-delay <secs>",
                         "Sets Rapid Spanning forward delay <4-30 secs>",
                         set_spanning_8021w_bridge_fwd_delay,
                         "spanning-tree rstp forward-delay <INT>",
                         GLOBAL_CONFIG_MODE | VLAN_CONFIG_MODE);

    install_cmd_handler ("spanning-tree rstp max-age <secs>",
                         "Sets Rapid Spanning max age <6-40 secs>",
                         set_spanning_8021w_bridge_max_age,
                         "spanning-tree rstp max-age <INT>",
                         GLOBAL_CONFIG_MODE | VLAN_CONFIG_MODE);

    install_cmd_handler ("spanning-tree rstp ethernet <port> path-cost <cost>", 
                         "Sets Rapid Spanning ports path cost <0 – 200000000>", 
                          set_spanning_8021w_port_path_cost, "spanning-tree rstp ethernet <INT> path-cost <INT>", 
			  GLOBAL_CONFIG_MODE | VLAN_CONFIG_MODE);

    install_cmd_handler ("spanning-tree rstp ethernet <port> priority <prio>", 
                         "Sets Rapid Spanning port priority <0 – 255>", 
                          set_spanning_8021w_port_prio, "spanning-tree rstp ethernet <INT> priority <INT>", 
 	                  GLOBAL_CONFIG_MODE | VLAN_CONFIG_MODE);

    install_cmd_handler ("spanning-tree rstp ethernet <port> admin-edge-port", 
                         "Sets Rapid Spanning port as edge port", 
                          set_spanning_8021w_port_admin_edge, "spanning-tree rstp ethernet <INT> admin-edge-port", 
 	                  GLOBAL_CONFIG_MODE | VLAN_CONFIG_MODE);

    install_cmd_handler ("spanning-tree rstp ethernet <port> admin-pt2pt", 
                         "Sets Rapid Spanning port link as point 2 point", 
                          set_spanning_8021w_port_admin_p2p, "spanning-tree rstp ethernet <INT> admin-pt2pt", 
 	                  GLOBAL_CONFIG_MODE | VLAN_CONFIG_MODE);

    install_cmd_handler ("show rstp", "shows rstp Spanning Tree",
                         show_spanning_tree_8021w, NULL, USER_EXEC_MODE);
}
