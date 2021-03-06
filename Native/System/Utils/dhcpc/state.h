// dhcpc : A DHCP client for Syllable
// (C)opyright 2002-2003,2007 Kristian Van Der Vliet
//
// This is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#ifndef __F_DHCP_STATE_H__
#define __F_DHCP_STATE_H__

#include <dhcp.h>

enum state{
	STATE_NONE,
	STATE_INIT,
	STATE_SELECTING,
	STATE_REQUESTING,
	STATE_BOUND,
	STATE_RENEWING,
	STATE_REBINDING,
	STATE_SHUTDOWN
};

int change_state( DHCPSessionInfo_s *info, int new_state );
int get_state( DHCPSessionInfo_s *info );

#endif		// __F_DHCP_STATE_H__

