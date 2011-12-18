#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rbtree.h"
#include "list.h"
#include "cmd.h"

#define  MAX_TOKENS 32

/*XXX: remove the repeated code*/

extern struct rb_root   cmd_root; 

enum {
	ERR_INVALID_COMMAND = 0,
	ERR_COMMAND_CONFLICTS
};

char *cli_error[] = {
	"    Invalid Command\n",
	"    Command Conflicts!\n"
};

char * check_alias (char *cmd);
void print_cmd_helpstring (cmdnode_t *cmd);
cmdnode_t * search_command (char *cmd, char * (*args)[10], int *error_code);
static inline void build_token (char *cmd, char * (*)[10]);
void execute_cmd (cmdnode_t *cmd, char *args[]);
void do_auto_complete (char *cmd, char *line);

extern int is_help;

int parse_and_execute (char *line)
{
	char *cmd = NULL;
	char * args [MAX_TOKENS];
	cmdnode_t *cmd_node = NULL;
	int error_code = -1;

	if (!(cmd = check_alias (line))) {
		cmd = line;
	}

	if (!(cmd_node = search_command (cmd, &args, &error_code))) {
		write_string (cli_error[error_code]);
		return -1;
	}

	if (!(get_curr_priv_level () & cmd_node->priv_mode)) {
		write_string (cli_error[ERR_INVALID_COMMAND]);
		return 0;
	}
	execute_cmd (cmd_node, args);

	return 0;
}

void execute_cmd (cmdnode_t *cmd, char *args[])
{
	cmd->handler (args);
}

char * check_alias (char *cmd)
{
	return NULL;
}

cmdnode_t *command_lookup (char *command, char * (*args)[10], int *error_code) 
{
	unsigned int key = command[0];
	struct list_head  *pnode = NULL;
	struct list_head  *head = NULL;
	cmdnode_t *match = NULL, *cmd = NULL;
	int nref = 0;
	char * token[MAX_TOKENS];
	char tmp[MAX_CMD_NAME];
	cmd_t  *p = cmd_tree_walk (&cmd_root, key);

	if (!p) {
		*error_code = ERR_INVALID_COMMAND;
		return NULL;
	}

	memset (token, 0, sizeof(token));
	memset (tmp, 0, sizeof(tmp));

	head = &p->head;

	memcpy (tmp, command, strlen(command) + 1);

	build_token (command, &token);

	list_for_each (pnode, head) {

		cmd= list_entry (pnode, cmdnode_t, np);

		if (match_command (cmd, token, args)) {
			++nref;
			match = cmd;
		}
	}
	if (nref == 1) {
		if (match->priv_mode & get_curr_priv_level ())
			return match;
		else {
			*error_code = ERR_INVALID_COMMAND;
			return NULL;
		}
	}
	else if (nref > 1) {
		handle_help (tmp, 0);
		*error_code = ERR_COMMAND_CONFLICTS;
	} else  if (!nref){
		*error_code = ERR_INVALID_COMMAND;
	}
	return NULL;
}

cmdnode_t * search_command (char *cmd, char * (*args)[10], int *error_code)
{
	cmdnode_t *p = NULL;

	if (!(p = command_lookup (cmd, args, error_code))) {
		return NULL;
	}
	return p;
} 

int match_command (cmdnode_t *cmd, char *token[], char * (*args)[10])
{
	char *cmd_tokens[MAX_TOKENS];
	char *next = NULL;
	char parse_cmd[MAX_CMD_NAME];
	int i = 0;
	int j = 0;

	if (!cmd || !token) 
		return 0;

	memset (cmd_tokens, 0, sizeof(cmd_tokens));

	memset (parse_cmd, 0, MAX_CMD_NAME);

	memcpy (parse_cmd, cmd->cmd, strlen(cmd->cmd) + 1);

	next = strtok (parse_cmd, " ");

	while (next && (i < MAX_TOKENS)) {
		cmd_tokens[i] = next;
		i++;
		next = strtok (NULL, " ");
	}

	i = 0;

	while (cmd_tokens[i] && token[i]) {
#define IS_SPECIAL_CHAR(ch)  ((ch < 65 && ch > 90) && (ch < 97 && ch > 122))
		if ((*cmd_tokens[i]) == '<') {
			if (args)
				(*args)[j++] = token[i];
			i++;
			if (!validate_input_param (token[i], cmd_tokens[i]))
				return 0;
			continue;
		} else if (!strncmp (cmd_tokens[i], token[i], 
					strlen (token[i]))) {
			i++;
		} else
			return 0;
	}

	if (!cmd_tokens[i] && token[i])
		return 0;
	else if (cmd_tokens[i] && !is_help) 
		return 0;
	else 
		return 1;
}

int validate_input_param (char *itoken, char *otoken)
{
	return 1;
}

static inline void build_token (char *cmd, char * (*token)[10])
{
	char *next = NULL;
	int i = 0;
	if (!cmd || !token) 
		return;

	next = strtok (cmd, " ");

	while (next && (i < MAX_TOKENS)) {
		(*token)[i] = next;
		i++;
		next = strtok (NULL, " ");
	}
	return;
}

cmd_t * cmd_tree_walk (struct rb_root  *cmd_root, unsigned int key)
{
	struct rb_node **p = &cmd_root->rb_node;
	struct rb_node *parent = NULL;
	cmd_t * cmd = NULL;
	unsigned int cmp = 0;

	while (*p) {

		parent = *p;

		cmd = rb_entry (parent, cmd_t, rlist);

		cmp = cmd->start_char;
		if (key < cmp)
			p = &(*p)->rb_left;
		else if (key > cmp) 
			p = &(*p)->rb_right;
		else 
			return cmd;
	}
	return NULL;
}

void cmd_del (cmd_t *n, struct rb_root *root)
{
	rb_erase (&n->rlist, root);
}

void cmd_add (cmd_t *n, struct rb_root *root)
{
	unsigned int key = n->start_char;
	struct rb_node **link = &root->rb_node;
	struct rb_node *parent = NULL;
	cmd_t  *x = NULL;

	while (*link) {

		parent = *link;

		x = rb_entry(parent, cmd_t, rlist);

		if (key < x->start_char)
			link = &(*link)->rb_left;
		else
			link = &(*link)->rb_right;
	}

	rb_link_node(&n->rlist, parent, link);
	rb_insert_color(&n->rlist, root);
}

int  handle_help (char *line, int is_tab)
{
	char * tokens[MAX_TOKENS];
	char *command = line;
	unsigned int key = line[0];
	struct list_head  *pnode = NULL;
	struct list_head  *head = NULL;
	cmdnode_t *cmd_node = NULL;
	cmd_t  *p = NULL;
	char parse_cmd[MAX_CMD_NAME];
	int flag = 0;
	cmdnode_t * match = NULL;
	int iref = 0;

	memset (tokens, 0, sizeof(tokens));

	if (!(command = check_alias (line))) {
		command = line;
	}
	memcpy (parse_cmd, command,  strlen(command) + 1);

	build_token (parse_cmd, &tokens);

	p = cmd_tree_walk (&cmd_root, key);

	if (p && tokens[0]) {
		flag = 1;
	} else if ((p = rb_first (&cmd_root)) && !tokens[0]) {
		is_tab = 0;
		flag = 0;
	} else {
		write_string ("\n");
		return 0;
	}

	if (is_help)
		write_string ("\n");

	do {
		head = &p->head;

		list_for_each (pnode, head) {

			cmdnode_t *cmd= list_entry (pnode, cmdnode_t, np);

			if (!tokens[0] || match_command (cmd, tokens, NULL)) {
				if (get_curr_priv_level () & cmd->priv_mode) {
					if (is_tab) {
						iref++;
						if (iref == 1) {
							match = cmd;
							continue;
						}
						if (iref == 2) {
							print_cmd_helpstring (match);
							is_tab = 0;
						}

					}
					print_cmd_helpstring (cmd);
				}
			}
		}

		if (iref == 1)
			goto autocomplete;
		if (flag)	
			return 0;
	} while (p = rb_next (&p->rlist));

	return 0;
autocomplete:
	memcpy (parse_cmd, match->cmd,  strlen(match->cmd) + 1);
	do_auto_complete (parse_cmd, line);
	return 0;
}

void do_auto_complete (char *cmd, char *line)
{
	int i = 0;
	int syn = 0;

	for  (; cmd[i] ; i++) {
		if (cmd[i] == '<') {
			syn = 1;
			continue;
		} else if (cmd[i] == '>') {
			syn = 0;
			continue;
		}
		line[i] = cmd[i];
	}
}

void print_cmd_helpstring (cmdnode_t *cmd)
{
	printf ("    %-30s %-30s\n",cmd->cmd, cmd->helpstring);
}

int handle_tab (char *line)
{
	is_help = 1;
	handle_help (line, 1);
	is_help = 0;
}
