#include "stp_info.h"
#include "cli.h"

extern struct list_head stp_instance_head;

void show_spanning_tree  (void)
{
	struct stp_instance *pstp_inst = NULL;

	list_for_each_entry(pstp_inst, &stp_instance_head, next) {

		if (pstp_inst->stp_enabled) {

			char *mac = NULL;
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
				char *state[] = {"DISABLED", "LISTENING", "LEARNING", 
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

void spanning_tree_enable (void)
{
	vlan_spanning_tree_enable_on_vlan (cli_get_vlan_id (), MODE_STP);
}
void spanning_tree_disable (void)
{
	vlan_spanning_tree_disable_on_vlan (cli_get_vlan_id (), MODE_STP);
}
void set_spanning_bridge_priority (char *args[])
{
	uint16_t prio = (uint16_t) atoi(args[0]);

 	stp_set_bridge_priority (prio, cli_get_vlan_id ());
}

void set_spanning_bridge_hello_time (char *args[])
{
	int hello = atoi(args[0]);

 	stp_set_bridge_hello_time (hello, cli_get_vlan_id ());
}

void set_spanning_bridge_fdly (char *args[])
{
	int fdly = atoi(args[0]);

 	stp_set_bridge_forward_delay (fdly, cli_get_vlan_id ());
}

void set_spanning_bridge_max_age (char *args[])
{
	int max_age = atoi(args[0]);

 	stp_set_bridge_max_age (max_age, cli_get_vlan_id ());
}

int stp_cli_init_cmd (void)
{
	install_cmd_handler ("spanning-tree", "Enables Spanning Tree", 
			     spanning_tree_enable, NULL, GLOBAL_CONFIG_MODE | VLAN_CONFIG_MODE);

	install_cmd_handler ("no spanning-tree", "Disables Spanning Tree", 
                              spanning_tree_disable, NULL, GLOBAL_CONFIG_MODE | VLAN_CONFIG_MODE);

	install_cmd_handler ("spanning-tree priority <prio>", 
                              "Sets Spanning Priority <0-65535>", 
                              set_spanning_bridge_priority, "spanning-tree priority <INT>", GLOBAL_CONFIG_MODE
                              | VLAN_CONFIG_MODE);

	install_cmd_handler ("spanning-tree hello-time <secs>", 
                              "Sets Spanning Hello time <1-10 secs>", 
                              set_spanning_bridge_hello_time, "spanning-tree hello-time <INT>", 
			      GLOBAL_CONFIG_MODE | VLAN_CONFIG_MODE);

	install_cmd_handler ("spanning-tree forward-delay <secs>", 
                              "Sets Spanning foward delay  <4-30 secs>", 
                              set_spanning_bridge_fdly, "spanning-tree forward-delay <INT>", 
			      GLOBAL_CONFIG_MODE | VLAN_CONFIG_MODE);

	install_cmd_handler ("spanning-tree max-age <secs>", 
                              "Sets Spanning max age <6-40 secs>", 
                              set_spanning_bridge_max_age, "spanning-tree max-age <INT>", 
			      GLOBAL_CONFIG_MODE | VLAN_CONFIG_MODE);

	install_cmd_handler ("show spanning-tree", "shows Spanning Tree", 
                              show_spanning_tree, NULL, USER_EXEC_MODE);
}
