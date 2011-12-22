#include "common_types.h"
#include "cli.h"


void cli_dhcp_acquire_ip_address (void *arg[])
{
	int port = cli_get_port ();

#if 0

	dhcp_acquire_ip_address (port);
#endif
}

void cli_dhcp_client_hostname (void *arg[])
{
}

void cli_dhcp_client_lease (void *arg[])
{
}


int dhcp_cli_init (void)
{
	install_cmd_handler ("ip address dhcp", "Acquires an IP address on an interface from DHCP. ", 
			     cli_dhcp_acquire_ip_address , NULL,  INTERFACE_MODE);

	install_cmd_handler ("ip dhcp client hostname <STRING>", "Specifies or modifies the host name sent in the DHCP message", 
                              cli_dhcp_client_hostname, NULL, INTERFACE_MODE);

	install_cmd_handler ("ip dhcp client lease  <INT> [<INT>] [<INT>]", "Configures the duration of the lease for an IP address", 
                              cli_dhcp_client_lease, NULL, INTERFACE_MODE);
}

int dhcp_init (void)
{
	dhcp_cli_init ();
}
