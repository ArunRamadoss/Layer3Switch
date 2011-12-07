#include "common_types.h"
#include "cli.h"

extern int show_vlan (void);

int set_vlan (char **args)
{
	if (!vlan_create_vlan (atoi (args[0]))) {
		change_vlan_mode (args);
		return 0;
	}
	return -1;
}

int set_tagged__port_to_vlan (char **args)
{
	if (vlan_add_port (cli_get_vlan_id (), atoi(args[0]), TAGGED) < 0) {
		printf ("Unabled Add the tagged port\n");
		return -1;
	}
	vlan_del_port (VLAN_DEFAULT_VLAN_ID, atoi(args[0]), TAGGED);
	return 0;
}

int vlan_cli_init_cmd (void)
{
#if 0
	install_cmd_handler ("vlan <vlan-id>", 
                             "set the vlan id 1-4096", 
                              set_vlan, "vlan <INT>", GLOBAL_CONFIG_MODE);

	install_cmd_handler ("tagged port <port_no>", 
                             "tagged port for vlan", 
                              set_tagged__port_to_vlan, "tagged port <INT>", VLAN_CONFIG_MODE);

	install_cmd_handler ("show vlan", "shows vlan info", 
                              show_vlan, NULL, USER_EXEC_MODE);
#endif
}

