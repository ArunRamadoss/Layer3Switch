#include "common_types.h"

int
rstp_init (void)
{
    rstp_cli_init_cmd ();
    return 0;
}

#if 0
int
rstp_is_root_bridge (const STPM_T * br)
{
    return !memcmp (&br->bridge_id, &br->designated_root, 8);
}
#endif
