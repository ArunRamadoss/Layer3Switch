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
#if 0
    uint16_t            prio = (uint16_t) atoi (args[0]);

    rstp_set_bridge_priority (prio);
#endif
}

void
set_spanning_8021w_bridge_hello_time (char *args[])
{
    int                 hello = atoi (args[0]);
#if 0
    rstp_set_bridge_hello_time (hello);
#endif
}

int
rstp_cli_init_cmd ()
{
    install_cmd_handler ("spanning-tree rstp", "Enables rstp Spanning Tree",
                         spanning_8021w_tree_enable, NULL,
                         GLOBAL_CONFIG_MODE | VLAN_CONFIG_MODE);

    install_cmd_handler ("no spanning-tree rstp", "Disables rstp Spanning Tree",
                         spanning_8021w_tree_disable, NULL,
                         GLOBAL_CONFIG_MODE | VLAN_CONFIG_MODE);

    install_cmd_handler ("spanning-tree rstp priority <prio>",
                         "Sets rstp Spanning Priority <0-65535>",
                         set_spanning_8021w_bridge_priority,
                         "spanning-tree rstp priority <INT>",
                         GLOBAL_CONFIG_MODE | VLAN_CONFIG_MODE);

    install_cmd_handler ("spanning-tree rstp hello-time <secs>",
                         "Sets rapid Spanning Hello time <1-10 secs>",
                         set_spanning_8021w_bridge_hello_time,
                         "spanning-tree rstp hello-time <INT>",
                         GLOBAL_CONFIG_MODE | VLAN_CONFIG_MODE);
    install_cmd_handler ("show rstp", "shows rstp Spanning Tree",
                         show_spanning_tree_8021w, NULL, USER_EXEC_MODE);
}
