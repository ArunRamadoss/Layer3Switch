#define MAX_PMP_LEN   64

#define MODE_SHIFT 8

#define USER_EXEC_MODE      0x1 << MODE_SHIFT
#define GLOBAL_CONFIG_MODE  0x2 << MODE_SHIFT
#define INTERFACE_MODE      0x4 << MODE_SHIFT
#define VLAN_CONFIG_MODE    0x8 << MODE_SHIFT
#define ALL_CONFIG_MODE     0x7FFFFF00

#define PRIV_LEVEL0       0x1
#define PRIV_LEVEL1      (0x1 << 1)
#define PRIV_LEVEL2      (0x1 << 2)
#define PRIV_LEVEL3      (0x1 << 3)
#define PRIV_LEVEL4      (0x1 << 4)
#define PRIV_LEVEL5      (0x1 << 5)
#define PRIV_LEVEL6      (0x1 << 6)
#define PRIV_LEVEL7      (0x1 << 7)


int  install_cmd_handler (const char *,const char *, void (*handler) (char *[]),const char *, int); 
int cli_get_vlan_id ();
int cli_get_port ();
int set_prompt (const char *prmpt_new);
int cli_set_port (int port);
int set_curr_mode (int mode);
int  get_prompt (char *pmt);
int exit_mode ();
