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
#include "rbtree.h"
#include "list.h"
#include "cli.h"
#include "cmd.h"

#define MAX_CLI_SESSION  8

#define MAX_MODE_GROUP  24

#define DEFAULT_USER_NAME  "guest"

static void init_signals (void);
static void spawn_cli_thread (void);
static void *cmdinterface(void *unused);
static void init_tty_prompt (void);
void handle_segfault (int );


struct rb_root   cmd_root; 
int is_help = 0;
struct cli this_cli[MAX_CLI_SESSION];
int this_session = -1;
int clitskid = 0;

static int gfd = 1;
static struct termios oldt, newt;
int login_sucessfull = 0;

int cli_init (char *prmt) 
{
	/* init the tty promt properties*/
	init_tty_prompt ();
	/*signal handlers for SIGINT, SIGSEGV, SIGUSR1, SIGUSR2*/
	init_signals();
	/*XXX:set the tty prompt - we take control from shell 
	  don't forget to call reset_tty () once your job is done*/
	set_tty();

	this_session = 0;

	cli_set_vlan_id (1);

	create_sync_lock (&cmd_root.lock);

	memcpy (this_cli[this_session].hostname, prmt, strlen(prmt));

	set_prompt_internal (this_cli[this_session].hostname, 
			strlen(this_cli[this_session].hostname));

	user_db_init ();

	strcpy (this_cli[this_session].username, DEFAULT_USER_NAME);

	cli_init_cmds ();

	return 0;
}

int start_cli_task (void)
{
	/*finally kick-start the shell thread*/
	spawn_cli_thread ();

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

static void spawn_cli_thread (void)
{
	task_create ("CLI", 10, 3, 32000, cmdinterface, NULL, NULL, &clitskid);
}

static void *cmdinterface(void *unused)
{
	char c;
	char cmd[MAX_CMD_NAME];

	memset (cmd, 0, MAX_CMD_NAME);

	show_login_prompt ();

	print_prompt ();

	while(1) {
		c = read_input ();

		if (!handle_cntrl_char (c, &cmd)) {
			continue;	
		}
		else {
			write_input_on_screen(c);
			if(add_input_to_the_cmd(c,cmd)) {
				print_prompt ();
			}
		}
	}
	return;
}

int handle_cntrl_char (int c, char *cmd)
{
	if(IS_BACK_SPACE(c)) {
		if(cmd[0] == '\0')
			return 0;
		write (gfd,"\b \b",sizeof("\b \b"));
		cmd[strlen(cmd) - 1] = '\0';
		return 0;
	}
	else if(c == 0x1b ) {
		c = read_input();
		c = read_input();
		if(c == 65) {
			return 0;
		}
		else if(c == 66) {
		}
		return 0;
	}
	else if(IS_HELP_CHAR(c)) {
		write_input_on_screen (c);
		is_help = 1;
		handle_help(cmd, 0);
		is_help = 0;
		goto cmd_print;
	}
	else if(IS_TAB(c)) {
		handle_tab (cmd);  
		goto cmd_print;
	}
	else {
		return 1;
	}
cmd_print:
	print_prompt ();
	write_string (cmd);
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


static int init_tty_noncan_echo_disable (int fd, struct termios *terminal)
{
	/* following TTY settings for SERIAL-MODE COMMUNICATIONS*/

	if (ioctl (fd, TCGETA, terminal) == -1) {
		return -1;
	}
	terminal->c_lflag &= ~(ECHO);    /* disable echo */
	if (ioctl (fd, TCSETA, terminal) == -1) {
		return -1;
	}
	if (tcgetattr (fd, terminal) == -1) {
		return -1;
	}
	terminal->c_lflag &= ~(ICANON | ECHO);    /* Non-Canonical mode and 
						     Disable Echo */
	terminal->c_oflag |= OFDEL;    /* Fill Delete character */
	terminal->c_cc[VMIN] = 1;
	return 0;
}

static void init_tty_prompt (void)
{
	save_tty (&oldt);
	init_tty_noncan_echo_disable (gfd, &newt);
}


int set_tty ()
{
	if (tcsetattr (gfd, TCSANOW, &newt) == -1){
		return -1;
	}
	return 0;
}

int save_tty (struct termios *tty)
{
	if (tcgetattr (STDIN_FILENO, tty) == -1) {
		return -1;
	}
	return 0;   
}

int reset_tty(void)
{
	if (tcsetattr (STDIN_FILENO, TCSANOW, &oldt) == -1){
		return -1;
	}
	return 0;   
}

int install_cmd_handler (char *cmd, char *help, int (*handler) (void *), char *syntax, int priv_mode)
{
	cmdnode_t *new = NULL;
	cmd_t *p = NULL;
	if (!(new = (cmdnode_t *)tm_malloc (sizeof(cmdnode_t)))) {
		perror(" -ERR- MALLOC: ");
		return -1;
	}
	memcpy (new->cmd, cmd, MAX_CMD_NAME);
	if (help)
		memcpy (new->helpstring, help, MAX_HELP_STRING);
	new->handler = handler;
	INIT_LIST_HEAD (&new->np);
	if (syntax)
		memcpy (new->syntax, syntax, MAX_CMD_NAME);
	new->priv_mode = priv_mode;

	p = cmd_tree_walk (&cmd_root, cmd[0]);

	if (!p) {
		cmd_t *new = (cmd_t *)tm_malloc (sizeof(cmd_t));
		new->start_char = (unsigned char)cmd[0];
		INIT_LIST_HEAD (&new->head);
		cmd_add (new, &cmd_root);
		p = new;
	}
	list_add (&new->np, &p->head);

	return 0;
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

int add_input_to_the_cmd(char c,char *tmp)
{
	int rval = 0;
	switch(c) {
		case '\n': 
			if(tmp[0]) 
			{
				strncat(tmp, "\0", 1);
				parse_and_execute(tmp);
				fflush (stdout);
				memset(tmp, 0, MAX_CMD_NAME);

			}
			rval = 1;
			break;
		default: strncat(tmp, &c, 1);
			 break;
	}
	return rval;
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
}

void write_string (char *str)
{
	write (gfd, str, strlen(str));
	fflush (stdout);
}


void handle_segfault (int signo)
{
	write_string ("ooppppssssssss ....! System crashed ..."
			"Am going to sleep for 100 secs debug the image\n");
	sleep (100);
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
}

void set_curr_priv_level (int level)
{
	this_cli[this_session].current_priv_level = level;
}

void install_commands (cmdnode_t *p, int count)
{
}

int set_prompt (char *prmpt_new)
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
