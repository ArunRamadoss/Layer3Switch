#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "defs.h"
#include "rbtree.h"
#include "list.h"
#include "cmd.h"

#define  MAX_TOKENS 32

enum {
	EXACT_MATCH = 0x1,
	MATCH = 0x2,
	NO_MATCH = 0x0
};
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
static inline int build_token (char *cmd, char * (*)[10]);
void execute_cmd (cmdnode_t *cmd, char *args[]);
int  do_auto_complete (char *cmd, char *line, int len, int , int);

extern int is_help;

int parse_and_execute (char *line)
{
	char *cmd = NULL;
	char * args [MAX_TOKENS];
	cmdnode_t *cmd_node = NULL;
	int error_code = -1;
	int mode = 0;

	if (!(cmd = check_alias (line))) {
		cmd = line;
	}

	if (!(cmd_node = search_command (cmd, &args, &error_code))) {
		write_string (cli_error[error_code]);
		return -1;
	}

	mode = (cmd_node->priv_mode & 0xFFFFFF00);

	if (!(mode & get_curr_mode ())) {
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
	int rval = 0;
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

		rval = match_command (cmd, token, args);
		if (rval == EXACT_MATCH) {
			nref = 1;
			match = cmd;
			break;
		} else if (rval == MATCH){
			++nref;
			match = cmd;
		} else 
			continue;
	}
	if (nref == 1) {
		int mode = (match->priv_mode & 0xFFFFFF00);
		if (mode & get_curr_mode ())
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
	int xctm = -1;

	if (!cmd || !token) 
		return 0;

	memset (cmd_tokens, 0, sizeof(cmd_tokens));

	memset (parse_cmd, 0, MAX_CMD_NAME);

	if (strlen(cmd->syntax))
		memcpy (parse_cmd, cmd->syntax, strlen(cmd->syntax) + 1);
	else
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
			if (!validate_input_param (token[i], cmd_tokens[i] + 1))
				return 0;
			i++;
			continue;
		} else if (!strncmp (cmd_tokens[i], token[i], 
					strlen (token[i]))) {
			i++;
		} else
			return 0;
	}

	if (!cmd_tokens[i] && token[i])
		return NO_MATCH;
	else if (cmd_tokens[i] && !is_help) 
		return NO_MATCH;
	else if (strlen (cmd_tokens[i-1]) == strlen (token[i-1]))
		return EXACT_MATCH;
	else 
		return MATCH;
}

int validate_input_param (char *itoken, char *otoken)
{
	if (!strncmp (otoken, "INT", sizeof("INT"))){
		while (*itoken) {
			if (isalpha (*itoken))
				return 0;
			itoken++;
		}
	}
	if (!strncmp (otoken, "STR", sizeof("STR"))){
		return 1;
	}
	return 1;
}

static inline int build_token (char *cmd, char * (*token)[10])
{
	char *next = NULL;
	int i = 0;
	if (!cmd || !token) 
		return -1;

	next = strtok (cmd, " ");

	while (next && (i < MAX_TOKENS)) {
		(*token)[i] = next;
		i++;
		next = strtok (NULL, " ");
	}
	if (i)
		return i;
	else
		return -1;
}

cmd_t * cmd_tree_walk (struct rb_root  *cmd_root, unsigned int key)
{
	struct rb_node **p = NULL;
	struct rb_node *parent = NULL;
	cmd_t * cmd = NULL;
	unsigned int cmp = 0;

	sync_lock (&cmd_root->lock);

	p = &cmd_root->rb_node;

	while (*p) {

		parent = *p;

		cmd = rb_entry (parent, cmd_t, rlist);

		cmp = cmd->start_char;
		if (key < cmp)
			p = &(*p)->rb_left;
		else if (key > cmp) 
			p = &(*p)->rb_right;
		else {
			sync_unlock (&cmd_root->lock);
			return cmd;
		}
	}

	sync_unlock (&cmd_root->lock);
	return NULL;
}

void cmd_del (cmd_t *n, struct rb_root *root)
{
	sync_lock (&root->lock);
	rb_erase (&n->rlist, root);
	sync_unlock (&root->lock);
}

void cmd_add (cmd_t *n, struct rb_root *root)
{
	unsigned int key = n->start_char;
	struct rb_node **link = NULL;
	struct rb_node *parent = NULL;
	cmd_t  *x = NULL;

	sync_lock (&root->lock);

	link = &root->rb_node;

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

	sync_unlock (&root->lock);
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
	int mode = 0;
	int last_token = 0;
	int rval = -1;

	memset (tokens, 0, sizeof(tokens));

	if (!(command = check_alias (line))) {
		command = line;
	}
	memcpy (parse_cmd, command,  strlen(command) + 1);

	last_token = build_token (parse_cmd, &tokens) - 1;

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

			if (!tokens[0] || (rval = match_command (cmd, tokens, NULL))) {
				if (rval == NO_MATCH)
					continue;
				mode = (cmd->priv_mode & 0xFFFFFF00);
				if (mode & get_curr_mode ()) {
					if (is_tab) {
						iref++;
						if (rval == EXACT_MATCH) {
							match = cmd;
							goto autocomplete;
						}
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
	rval = do_auto_complete (match->cmd, line, strlen(tokens[last_token]), last_token, iref);
	if (rval && iref == 1)
		print_cmd_helpstring (match);
	return 0;
}

int  do_auto_complete (char *cmd, char *line, int len, int last_t, int nref)
{
	char * tokens[MAX_TOKENS];
	char parse_cmd[MAX_CMD_NAME];
	int offset = 0;

	memset (parse_cmd, 0, MAX_CMD_NAME);
	memset (tokens, 0, sizeof(tokens));

	memcpy (parse_cmd, cmd,  strlen(cmd) + 1);

	build_token (parse_cmd, &tokens);

	offset = strlen(line) - len - 1;
	if ((!strncmp (line + offset, tokens[last_t], strlen(tokens[last_t]))) 
             || (*tokens[last_t] == '<')) {
		last_t++;
		offset = strlen (line);
	}
	else  
		offset = strlen (line) - len;

	if (tokens[last_t] && *tokens[last_t] != '<') {
		strcpy (line + offset, tokens[last_t]);
		strcat  (line, " ");
	}
	return 1;
}

void print_cmd_helpstring (cmdnode_t *cmd)
{
	printf ("   %-48s   %-64s\n",cmd->cmd, cmd->helpstring);
}

int handle_tab (char *line)
{
	is_help = 1;
	handle_help (line, 1);
	is_help = 0;
}
