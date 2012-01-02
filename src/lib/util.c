/*
 *  Author:
 *  Sasikanth.V        <sasikanth@email.com>
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version
 *  2 of the License, or (at your option) any later version.
 */

#include "common_types.h"

uint32_t ip_2_uint32 (uint8_t *ipaddress, int byte_order)
{
        uint32_t  byte[4];

        memset (byte, 0, sizeof(byte));

        sscanf ((char *)ipaddress, "%u.%u.%u.%u", &byte[0], &byte[1], &byte[2], &byte[3]);

        if (byte_order) /*Network*/
               return (byte[0] << 24) | (byte[1] << 16) | (byte[2] << 8) | (byte[3]);
        else /*Host*/
               return (byte[3] << 24) | (byte[2] << 16) | (byte[1] << 8) | (byte[0]);
}

void uint32_2_ipstring (uint32_t ipAddress, uint8_t *addr)
{
	int i = 0;
	for (i=0; i < 4; i++) {
		addr[i] = (ipAddress >> (i * 8) ) & 0xFF;
	}
}


