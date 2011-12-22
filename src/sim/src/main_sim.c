#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>      
#include <sys/stat.h>
#include <sys/socket.h>
#include <netpacket/packet.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <getopt.h>
#include "common_types.h"



char switch_mac[6] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x00};

#define MAX_PMP_LEN   32

#define MODE_SHIFT  8

#define USER_EXEC_MODE    0x1 << MODE_SHIFT
#define GLOBAL_CONFIG_MODE 0x2 << MODE_SHIFT
#define INTERFACE_MODE    0x4 << MODE_SHIFT

#define ALL_MODE_INDEX     0x80000000 
#define ALL_CONFIG_MODE    0x7FFFFF00


#define PRIV_LEVEL0      0x1
#define PRIV_LEVEL1      PRIV_LEVEL0 << 1
#define PRIV_LEVEL2      PRIV_LEVEL1 << 1
#define PRIV_LEVEL3      PRIV_LEVEL2 << 1
#define PRIV_LEVEL4      PRIV_LEVEL3 << 1
#define PRIV_LEVEL5      PRIV_LEVEL4 << 1
#define PRIV_LEVEL6      PRIV_LEVEL5 << 1
#define PRIV_LEVEL7      PRIV_LEVEL6 << 1

void dump_task_info (void);

int show_uptime ();

execute_system_call (char *arg)
{
	system (arg);
}

extern show_fdb ();

int main (int argc, char **argv)
{
	if (argc < 2) {
		fprintf (stderr, "Usage : ./switch <instance>\n");
		return -1;
	}

	parse_cmdline (argc, argv);

	switch_mac[5] = atoi (argv[1]);

	tmlib_init ();

	cli_init ("Switch");

	spawn_pkt_processing_task ();

	port_init ();

	bridge_init ();

	dhcp_init ();

	set_curr_mode (USER_EXEC_MODE);

        install_cmd_handler ("uptime", "Displays the uptime", show_uptime, NULL, USER_EXEC_MODE);

	start_cli_task ();

	while (1) {
		sleep (-1);
	}
}

int read_port_mac_address (int port, char *p) 
{
	int i = 0;
	while (i < 5) {
		p[i] = switch_mac[i];
		i++;
	}
	p[5] = (uint8_t)port + 1;
	return 0;
}
