#include "common_types.h"
#include "cli.h"
#include "ifmgmt.h"
#include "cparser.h"
#include "cparser_tree.h"

int  cli_set_port_enable (void);
int cli_show_interfaces (int port);
int  cli_set_port_disable (void);
void send_interface_enable_or_disable (int port , int state);

cparser_result_t cparser_cmd_if_enable(cparser_context_t *context)
{
	if (!cli_set_port_enable ())
                return CPARSER_OK;
        return CPARSER_NOT_OK;
}
cparser_result_t cparser_cmd_if_disable(cparser_context_t *context)
{
	if (!cli_set_port_disable ())
                return CPARSER_OK;
        return CPARSER_NOT_OK;
}
#if 0
cparser_result_t cparser_cmd_config_interface_ethernet_portnum(cparser_context_t *context,
    int32_t *portnum_ptr)
{
	char prompt[CPARSER_MAX_PROMPT];
	/* Enter the submode */
        cli_set_port (*portnum_ptr);
        sprintf (prompt, "%s%d%s","(config-if-", *portnum_ptr, ")");
        set_prompt (prompt);
	get_prompt (prompt);
        set_curr_mode (INTERFACE_MODE);
	return cparser_submode_enter(context->parser, NULL, prompt);

}
#endif
cparser_result_t cparser_cmd_if_exit(cparser_context_t *context)
{
	if (!exit_mode ())
	{
		return cparser_submode_exit (context->parser);
	}
	return CPARSER_NOT_OK;
}
cparser_result_t cparser_cmd_interface_ethernet_portnum(cparser_context_t *context,  int32_t *portnum_ptr)
{
	char prompt[CPARSER_MAX_PROMPT];
	/* Enter the submode */
        cli_set_port (*portnum_ptr);
        sprintf (prompt, "%s%d%s","(config-if-", *portnum_ptr, ")");
        set_prompt (prompt);
	get_prompt (prompt);
        set_curr_mode (INTERFACE_MODE);
	return cparser_submode_enter(context->parser, NULL, prompt);

}

cparser_result_t cparser_cmd_show_interface(cparser_context_t *context)
{
	if (!cli_show_interfaces (-1))
                return CPARSER_OK;
        return CPARSER_NOT_OK;
}

int  cli_set_port_enable (void)
{
	int port = cli_get_port ();
	port_cdb[port - 1].ifAdminStatus = IF_UP;
	port_cdb[port - 1].ifOperStatus = IF_UP;
	send_interface_enable_or_disable (port, IF_UP);
	return 0;
}

int cli_set_port_disable (void)
{
	int port = cli_get_port ();
	port_cdb[port - 1].ifAdminStatus = IF_DOWN;
	port_cdb[port - 1].ifOperStatus = IF_DOWN;

	send_interface_enable_or_disable (port, IF_DOWN);

	return 0;
}

int cli_show_interfaces (int port)
{
	int idx = -1;

	const char *state[]  = {"UNKNWN", "UP", "DOWN"};

	printf (" Port    Name     MTU    Type    Admin    Oper   LastChange\n");
	printf (" ----   -----    -----  ------   ------  -----   ----------\n");
	while (++idx < MAX_PORTS) {
		printf (" %-3d    %-6s   %-5d   %-6s  %-4s    %-4s        %-4d\n",
		port_cdb[idx].ifIndex + 1, port_cdb[idx].ifDescr,
		port_cdb[idx].ifMtu, "ETH", state[port_cdb[idx].ifAdminStatus],
		state[port_cdb[idx].ifOperStatus], port_cdb[idx].ifLastChange);
	}

	return 0;
}
