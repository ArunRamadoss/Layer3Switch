#include "cli.h"
#include "common_types.h"

void
show_spanning_tree_8021w (void)
{
#if 0
    STPM_T             *stp_instance_head = rstp8021w_stpm_get_the_list ();
    STPM_T             *pstp_inst = NULL;

    for (pstp_inst = stp_instance_head; pstp_inst; pstp_inst = pstp_inst->next)
    {

        if (pstp_inst->admin_state)
        {

            char               *mac = NULL;

            int                 is_root = rstp_is_root_bridge (pstp_inst);

            printf ("\n  Rapid Spanning tree enabled protocol ieee on\n");
            printf ("  -------------------------------------- \n\n");

            printf ("  VLAN  : %d\n\n", pstp_inst->vlan_id);

            printf ("  Root ID\n\tPriority    %d\n",
                    pstp_inst->designated_root.prio);

            mac = pstp_inst->designated_root.addr;

            printf ("\tAddress     %02x:%02x:%02x:%02x:%02x:%02x\n",
                    mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

            if (is_root)
            {
                printf ("\tThis bridge is the root\n");
            }

            printf
                ("\tHello Time  %d sec  Max Age %d sec  Forward Delay %d sec\n\n",
                 pstp_inst->root_times.HelloTime, pstp_inst->root_times.MaxAge,
                 pstp_inst->root_times.ForwardDelay);

            printf ("  Bridge ID\n\tPriority    %d\n",
                    pstp_inst->bridge_id.prio);

            mac = pstp_inst->bridge_id.addr;

            printf ("\tAddress     %02x:%02x:%02x:%02x:%02x:%02x\n",
                    mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

            printf
                ("\tHello Time  %d sec  Max Age %d sec  Forward Delay %d sec\n\n",
                 pstp_inst->bridge_times.HelloTime,
                 pstp_inst->bridge_times.MaxAge,
                 pstp_inst->bridge_times.ForwardDelay);

            if (!is_root)
            {
                printf ("\n\tRoot Port : %d\n", pstp_inst->root_port);
            }

            if (pstp_inst->ports)
            {
                struct rstp_port_entry *p = NULL;
                char               *state[] =
                    { "DISABLED", "ALTERNATE", "BACKUP",
                    "ROOTPORT", "DESIGNATED"
                };
                printf
                    ("\nPort   Cost     State      Bridge Id    \n");
                printf
                    ("----   -----   ------   -----------------  \n");
                for (p = pstp_inst->ports; p; p = p->next)
                {
                    mac = p->owner->bridge_id.addr;
                    printf
                        ("%2d   %4d   %10s   %02x:%02x:%02x:%02x:%02x:%02x\n",
                         p->port_index, 20000, state[p->role], mac[0], mac[1],
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
#endif
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
