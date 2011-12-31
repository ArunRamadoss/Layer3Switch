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
}__attribute__ ((__packed__));

typedef struct cmds {
	struct rb_node rlist; 
	int start_char; /*key*/
	struct list_head  head;
}cmd_t;


typedef struct cmd_node {
	char cmd[MAX_CMD_NAME];
	char helpstring[MAX_HELP_STRING];
	unsigned int priv_mode; /*8 bits for priv, 24 bits for mode*/
	char syntax[MAX_CMD_NAME];
	int (*handler) (char *args[]);
	struct list_head  np;
}cmdnode_t;


cmd_t * cmd_tree_walk (struct rb_root  *cmd_root, unsigned int key);
extern char read_input ();
int add_input_to_the_cmd(char c,char *tmp);
int write_input_on_screen(char c);
void write_string (const char *str);
