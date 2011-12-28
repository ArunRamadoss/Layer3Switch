/*
 *  Author:
 *  Sasikanth.V        <sasikanth@email.com>
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version
 *  2 of the License, or (at your option) any later version.
 */

#include "common_types.h"
#include "cli.h"
#include "ifmgmt.h"

void cli_vrrp_enable (char *args[])
{
	vrrp_create_instance (args[0], cli_get_port (), ip_2_uint32 (args[1], 0));
}

void cli_vrrp_set_priority (char *args[])
{

}

void cli_vrrp_set_adv_timer (char *args[])
{

}

void cli_show_vrrp (void)
{

}

int vrrp_cli_init (void)
{
	install_cmd_handler ("vrrp <vrid> ip <ipaddress>", "Enables VRRP on an interface", cli_vrrp_enable, 
			     "vrrp <INT> ip <IPADDR>", INTERFACE_MODE);

	install_cmd_handler ("vrrp <vrid> priority <level>", "Sets the priority level of the router within a VRRP group",
			     cli_vrrp_set_priority, "vrrp <INT> priority <INT>", INTERFACE_MODE);

	install_cmd_handler ("vrrp <vrid> timers advertise <interval-secs>", 
			      "Configures the interval between successive advertisements by the master virtual router",
			     cli_vrrp_set_adv_timer, "vrrp <INT> timers advertise <INT>", INTERFACE_MODE);

	install_cmd_handler ("show vrrp", "Displays VRRP information", cli_show_vrrp, NULL, USER_EXEC_MODE);
}
