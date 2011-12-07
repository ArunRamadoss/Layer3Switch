#include "common_types.h"
#include "cli.h"
#include "ifmgmt.h"

extern int change_to_interface_mode (char **);

void cli_set_port_enable (void *arg[])
{
	int port = cli_get_port ();
	(void)arg;
	port_cdb[port - 1].ifAdminStatus = IF_UP;
	port_cdb[port - 1].ifOperStatus = IF_UP;

	send_interface_enable_or_disable (port, IF_UP);
}

void cli_set_port_disable (void *arg[])
{
	int port = cli_get_port ();
	(void)arg;
	port_cdb[port - 1].ifAdminStatus = IF_DOWN;
	port_cdb[port - 1].ifOperStatus = IF_DOWN;

	send_interface_enable_or_disable (port, IF_DOWN);
}

void cli_show_interfaces (void *arg[])
{
	int idx = -1;

	char *state[]  = {"UNKNWN", "UP", "DOWN"};

	printf (" Port    Name     MTU    Type    Admin    Oper   LastChange\n");
	printf (" ----   -----    -----  ------   ------  -----   ----------\n");
	while (++idx < MAX_PORTS) {
		printf (" %-3d    %-6s   %-5d   %-6s  %-4s    %-4s        %-4d\n",
		port_cdb[idx].ifIndex + 1, port_cdb[idx].ifDescr,
		port_cdb[idx].ifMtu, "ETH", state[port_cdb[idx].ifAdminStatus],
		state[port_cdb[idx].ifOperStatus], port_cdb[idx].ifLastChange);
	}
}

void port_cli_cmds_init (void)
{
	install_cmd_handler ("interface ethernet <port>", "Interface mode",change_to_interface_mode , 
                             "interface ethernet <INT>", GLOBAL_CONFIG_MODE);

	install_cmd_handler ("disable", "Disables the port", 
   			      cli_set_port_disable, NULL, INTERFACE_MODE);

	install_cmd_handler ("enable", "Enables the port", 
   			      cli_set_port_enable, NULL, INTERFACE_MODE);
	install_cmd_handler ("show interface", "Displays interface", 
   			      cli_show_interfaces, NULL, USER_EXEC_MODE);
}
