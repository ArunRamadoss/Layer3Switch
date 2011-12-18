#define MAX_CMD_NAME        128
#define MAX_HELP_STRING     128
#define MAX_PMP_LEN   32

#define IS_HELP_CHAR(c)   (c == '?')
#define IS_BACK_SPACE(c)  (c == 0x7f)
#define IS_TAB(c)         (c == '\t')
#define PAGE_UP     73
#define HOME        71
#define END         79
#define PAGE_DOWN   81
#define UP_ARROW    72
#define LEFT_ARROW  75
#define DOWN_ARROW  80
#define RIGHT_ARROW 77
#define F1          59
#define F2          60
#define F3          61
#define F4          62
#define F5          63
#define F6          64
#define F7          65
#define F8          66
#define F9          67
#define F10         68

#define PRIV_LEVEL0     (0x1 << 23)
#define PRIV_LEVEL1      PRIV_LEVEL0 << 1
#define PRIV_LEVEL2      PRIV_LEVEL1 << 1
#define PRIV_LEVEL3      PRIV_LEVEL2 << 1
#define PRIV_LEVEL4      PRIV_LEVEL3 << 1
#define PRIV_LEVEL5      PRIV_LEVEL4 << 1
#define PRIV_LEVEL6      PRIV_LEVEL5 << 1
#define PRIV_LEVEL7      PRIV_LEVEL6 << 1


struct cli {
	int session;
	int priv_level;
};

typedef struct cmds {
	struct rb_node rlist; 
	int start_char; /*key*/
	struct list_head  head;
}cmd_t;


typedef struct cmd_node {
	char cmd[MAX_CMD_NAME];
	char helpstring[MAX_HELP_STRING];
	unsigned int priv_mode; /*8 bits for priv, 24 bits for mode*/
	void *arg;
	int (*handler) (char *, ...);
	struct list_head  np;
}cmdnode_t;


cmd_t * cmd_tree_walk (struct rb_root  *cmd_root, unsigned int key);
