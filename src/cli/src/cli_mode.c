#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <termios.h>
#include <unistd.h>
#include <termio.h>
#include "defs.h"
#include "cli.h"

int change_to_interface_mode (char **args);
int change_config_mode (char **args);
int change_vlan_mode (char **args);
int end_mode (char **args);
extern int show_users (void);
int process_logout();
int process_lock (void);
extern void show_cpu_usage (void);

struct modes {
	int mode;
	int (*mode_change) (char **args);
};

struct modes mode_p[] = { {GLOBAL_CONFIG_MODE, change_config_mode},
			  {INTERFACE_MODE ,    change_to_interface_mode},
			  {VLAN_CONFIG_MODE, change_vlan_mode},
			  {USER_EXEC_MODE,  end_mode}
			};

void do_mode_change (int mode)
{
	int i = sizeof (mode_p)/ sizeof(struct modes);
	int j = 0;

	while (j < i) {
		if (mode_p[j].mode == mode) {
			mode_p[j].mode_change (NULL);
			return;
		}
		j++;
	}
	return;
}

int change_vlan_mode (char **args)
{
	char prmpt[MAX_PMP_LEN];
	int vlan_id = 0;
	memset (prmpt, 0, sizeof (prmpt));
	if (args) {
		vlan_id = atoi (args[0]);
		cli_set_vlan_id (atoi(args[0]));
	}
	else {
		vlan_id = cli_get_vlan_id ();
	}
	sprintf (prmpt, "%s%d%s","(config-vlan-",vlan_id, ")");
	set_prompt (prmpt);
	set_curr_mode (VLAN_CONFIG_MODE);
}

int change_to_interface_mode (char **args)
{
	char prmpt[MAX_PMP_LEN];
	int port = 0;
	memset (prmpt, 0, sizeof (prmpt));
	if (args) {
		port = atoi (args[0]);
		cli_set_port (atoi(args[0]));
	}
	else {
		port = cli_get_port ();
	}
	sprintf (prmpt, "%s%d%s","(config-if-",port, ")");
	set_prompt (prmpt);
	set_curr_mode (INTERFACE_MODE);
}

int change_config_mode (char **args)
{
	set_prompt ("(config)");
	set_curr_mode (GLOBAL_CONFIG_MODE);
}

int end_mode (char **args)
{
	set_prompt ("");
	set_curr_mode (USER_EXEC_MODE);
}

int do_exit ()
{
	reset_tty ();
	exit (1);
}

int exit_mode ()
{
	int mode = get_curr_mode ();

	switch (mode) {
		case GLOBAL_CONFIG_MODE:
			do_mode_change (USER_EXEC_MODE);
			break;
		case INTERFACE_MODE:
			do_mode_change (GLOBAL_CONFIG_MODE);
			break;
		case VLAN_CONFIG_MODE:
			do_mode_change (GLOBAL_CONFIG_MODE);
			break;
		case USER_EXEC_MODE:
			break;
	}
}

int add_user (char **args)
{
	create_user (args[0], args[1], atoi(args[2]));
}

int del_user (char **args)
{
	user_del (args[0]);
}


int cli_init_cmds (void)
{
	install_cmd_handler ("exit", "Exit the application", exit_mode, NULL, ALL_CONFIG_MODE);
	install_cmd_handler ("lock", "Locks the Screen", process_lock, NULL, USER_EXEC_MODE);
	install_cmd_handler ("logout", "logout from current session", process_logout, NULL, USER_EXEC_MODE);
	install_cmd_handler ("show users", "Displays user info", show_users, NULL, USER_EXEC_MODE);
	install_cmd_handler ("user del <username>", "Delete user", del_user,"user del <STR>", GLOBAL_CONFIG_MODE);
	install_cmd_handler ("user add <username> <password> <level>", 
                             "Add new user info", add_user, "user add <STR> <STR> <INT>", GLOBAL_CONFIG_MODE);
	install_cmd_handler ("quit", "quit the application", do_exit, NULL, ALL_CONFIG_MODE);
	install_cmd_handler ("end", "Changes to the exec mode", end_mode, NULL, ALL_CONFIG_MODE);
	install_cmd_handler ("configure terminal", "configuration mode", change_config_mode, NULL, USER_EXEC_MODE);
	install_cmd_handler ("show task-cpu", "Displays the CPU usage of the task", show_cpu_usage, NULL, USER_EXEC_MODE);
}
