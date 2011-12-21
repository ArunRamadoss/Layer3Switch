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
#include "rbtree.h"
#include "list.h"
#include "cmd.h"

extern char read_input ();
int add_input_to_the_cmd(char c,char *tmp);
int write_input_on_screen(char c);
void write_string (char *str);

struct rb_root   cmd_root; 

static char prmpt[MAX_PMP_LEN + 1]; 
static char hostname[MAX_PMP_LEN];
static struct termios oldt, newt;
int is_help = 0;
static int gfd = 1;
static int current_priv_level = PRIV_LEVEL5;
int clitskid = 0;
static void init_signals (void);
static void spawn_cli_thread (void);
static void *cmdinterface(void *unused);
static void init_tty_prompt (void);
void handle_segfault (int );

int create_cmdline_interface (char *prmt)
{
	/* init the tty promt properties*/
	init_tty_prompt ();
	/*signal handlers for SIGINT, SIGSEGV, SIGUSR1, SIGUSR2*/
	init_signals();
	/*XXX:set the tty prompt - we take control from shell 
	  don't forget to call reset_tty () once your job is done*/
	set_tty();

	set_prompt(prmt, strlen(prmt));

	create_sync_lock (&cmd_root.lock);
	/*finally kick-start the shell thread*/
	spawn_cli_thread ();
	return 0;
}

static void install_signal_handler (int signo, void (*handler)(int))
{
	signal (signo, handler);
}

static void init_signals (void)
{
	install_signal_handler (SIGSEGV, handle_segfault);
}

static void spawn_cli_thread (void)
{
	task_create ("CLI", 10, 3, 20000, cmdinterface, NULL, NULL, &clitskid);
}

static void *cmdinterface(void *unused)
{
	char c;
	char cmd[MAX_CMD_NAME];

	print_prompt ();

	memset (cmd, 0, MAX_CMD_NAME);

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

int install_cmd_handler (char *cmd, char *help, int (*handler) (void *), void *arg, int priv_mode)
{
	cmdnode_t *new = NULL;
	cmd_t *p = NULL;
	if (!(new = (cmdnode_t *)tm_malloc (sizeof(cmdnode_t)))) {
		write_string(" -ERR- MALLOC: ");
		return -1;
	}
	memcpy (new->cmd, cmd, MAX_CMD_NAME);
	if (help)
		memcpy (new->helpstring, help, MAX_HELP_STRING);
	new->handler = handler;
	INIT_LIST_HEAD (&new->np);
	new->arg = arg;
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

int set_prompt (char *pmt, int len)
{
	if (len > MAX_PMP_LEN || !pmt) {
		return -1;
	}
	sprintf (prmpt, "%s%s ", pmt, "#");
	return 0;
}

int  get_prompt (char *pmt)
{
	if (!pmt)
		return -1;
	memcpy (pmt, prmpt, MAX_PMP_LEN);
	return 0;
}

int add_input_to_the_cmd(char c,char *tmp)
{
	switch(c) {
		case '\n': 
			if(tmp[0] == '\0') {
				return 1;
			} 
			else  {
				strncat(tmp, "\0", 1);
				parse_and_execute(tmp);
			}
			memset(tmp, 0, MAX_CMD_NAME);
			break;
		default: strncat(tmp, &c, 1);
			 return 0;
			 break;
	}
	return 1;
}

print_prompt ()
{
	write(gfd, prmpt, strlen(prmpt));
}

int write_input_on_screen(char c)
{
	write(gfd,&c,1);
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
	return current_priv_level;
}

void set_curr_priv_level (int level)
{
	current_priv_level = level;
}

void install_commands (cmdnode_t *p, int count)
{
}
