/***********************************************************
 * Author : TechMinds
 *
 * GPLv2 licen
 ***********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <termios.h>
#include <unistd.h>
#include <termio.h>
#include "defs.h"
#include "cparser.h"
#include "cparser_tree.h"

#define MAX_CLI_SESSION  8

#define MAX_MODE_GROUP  24

#define DEFAULT_USER_NAME  "guest"

struct cli {
	int session;
	int priv_level;
	int in;
	int out;
	int port_number;
        int current_priv_level;
	int current_mode;
	int port_no;
	int vlanid;
	char prmpt[MAX_PMP_LEN + 1]; 
	char hostname[MAX_PMP_LEN];
	char prmpt_prev[MAX_PMP_LEN];
	char username[MAX_USER_NAME];
	cparser_t parser;
}__attribute__ ((__packed__));


static void init_signals (void);
static void spawn_cli_thread (int);
static void *cmdinterface(void *unused);
static void init_tty_prompt (void);
void handle_segfault (int );
int start_cli_task (void);
int cli_init (const char *prmt);

struct cli this_cli[MAX_CLI_SESSION];
int this_session = -1;
int clitskid = 0;
static int gfd = 1;
int login_sucessfull = 0;

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


int cli_init (const char *prmt) 
{
	this_session = 0;
        cli_set_vlan_id (1);
	cli_session_init (prmt, 0);
}
int cli_session_init (const char *prmt, int this_session) 
{

	memset(&this_cli[this_session].parser, 0, sizeof(this_cli[this_session].parser));
        memcpy (this_cli[this_session].hostname, prmt, strlen(prmt));

        set_prompt_internal (this_cli[this_session].hostname,
                        strlen(this_cli[this_session].hostname));
        strcpy (this_cli[this_session].username, DEFAULT_USER_NAME);
	this_cli[this_session].parser.cfg.root = &cparser_root;
	this_cli[this_session].parser.cfg.ch_complete = '\t';
	this_cli[this_session].parser.cfg.ch_erase = '\b';
	this_cli[this_session].parser.cfg.ch_help = '?';
	this_cli[this_session].parser.cfg.flags = 0;
	get_prompt (this_cli[this_session].parser.cfg.prompt);
	cparser_io_config(&this_cli[this_session].parser);
	this_cli[this_session].parser.cfg.fd = STDOUT_FILENO;
        user_db_init ();
	return 0;
}

int start_cli_task (void)
{
	set_curr_mode (USER_EXEC_MODE);

	/*finally kick-start the shell thread*/
	spawn_cli_thread (this_session);

	return 0;
}

void install_signal_handler (int signo, void (*handler)(int))
{
	signal (signo, handler);
}

static void init_signals (void)
{
	install_signal_handler (SIGSEGV, handle_segfault);
}

static void spawn_cli_thread (int this_session)
{
	task_create ("CLI", 10, 3, 32000, cmdinterface, NULL, this_session, &clitskid);
}

static void *cmdinterface(void *unused)
{
	int session = (int)unused;
	cparser_result_t rc;

	rc = cparser_init(&this_cli[session].parser.cfg, &this_cli[session].parser);

	cparser_run(&this_cli[session].parser);
}

int set_prompt_internal (char *pmt, int len)
{
	if (len > MAX_PMP_LEN || !pmt) {
		return -1;
	}
	sprintf (this_cli[this_session].prmpt, "%s%s ", pmt, "#");
	return 0;
}

int  get_prompt (char *pmt)
{
	if (!pmt)
		return -1;
	memcpy (pmt, this_cli[this_session].prmpt, MAX_PMP_LEN);
	return 0;
}

print_prompt ()
{
	write(gfd, this_cli[this_session].prmpt, 
			strlen(this_cli[this_session].prmpt));
	fflush (stdout);
}

int write_input_on_screen(char c)
{
	write(gfd,&c,1);
	fflush (stdout);
	return 0;
}

void write_string (const char *str)
{
	write (gfd, str, strlen(str));
	fflush (stdout);
	return 0;
}


void handle_segfault (int signo)
{
	write_string ("ooppppssssssss ....! System crashed ..."
			"Am going to sleep for 10 secs debug the image\n");
	sleep (10);
	exit (0);
}

int get_curr_priv_level (void)
{
	return this_cli[this_session].current_priv_level;
}

int get_curr_mode ()
{
	return this_cli[this_session].current_mode;
}

int set_curr_mode (int mode)
{
	this_cli[this_session].current_mode = mode;
	return 0;
}

void set_curr_priv_level (int level)
{
	this_cli[this_session].current_priv_level = level;
}

int set_prompt (const char *prmpt_new)
{
	char gprompt[MAX_PMP_LEN];

	char sprompt[MAX_PMP_LEN];

	memset (sprompt, 0, MAX_PMP_LEN);
	memset (gprompt, 0, MAX_PMP_LEN);

	sprintf (sprompt, "%s%s", this_cli[this_session].hostname, 
		 prmpt_new);
	set_prompt_internal(sprompt, strlen (sprompt));

	return 0;
}

int cli_set_port (int port)
{
	this_cli[this_session].port_no = port;
}

int cli_get_vlan_id ()
{
	return this_cli[this_session].vlanid;
}

int cli_set_vlan_id (int vlan_id)
{
	this_cli[this_session].vlanid = vlan_id;
}
int cli_get_port ()
{
	return this_cli[this_session].port_no;
}

int get_current_user_name (char *user)
{
	strcpy (user, this_cli[this_session].username);
	return 0;
}
int set_current_user_name (char *user)
{
	strcpy (this_cli[this_session].username, user);
	return 0;
}

char read_input ()
{
        char c = 0;
        while (read (fileno(stdin), &c, 1) == -1) {
                continue;
        }
        return c;
}

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
	return 0;
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
	return 0;
}

int change_config_mode (char **args)
{
	set_prompt ("(config)");
	set_curr_mode (GLOBAL_CONFIG_MODE);
	return 0;
}

int end_mode (char **args)
{
	set_prompt ("");
	set_curr_mode (USER_EXEC_MODE);
	return 0;
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
		default:
			return -1;
        }
	return 0;
}
