/* NetworkManager -- Network link manager
 *
 * Dan Williams <dcbw@redhat.com>
 * Per Øyvind Karlsen <peroyvind@mandriva.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 * (C) Copyright 2004 Red Hat, Inc.
 * (C) Copyright 2008 Per Øyvind Karlsen
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>

#include "NetworkManagerGeneric.h"
#include "NetworkManagerSystem.h"
#include "NetworkManagerUtils.h"
#include "nm-device.h"
#include "nm-device-802-3-ethernet.h"
#include "nm-device-802-11-wireless.h"
#include "nm-utils.h"
#include "shvar.h"

/*
 * nm_system_init
 *
 * Initializes the distribution-specific system backend
 *
 */
void nm_system_init (void)
{
	nm_generic_init ();
}


/*
 * nm_system_device_flush_ip4_routes
 *
 * Flush all routes associated with a network device
 *
 */
void nm_system_device_flush_ip4_routes (NMDevice *dev)
{
	nm_generic_device_flush_ip4_routes (dev);
}


/*
 * nm_system_device_flush_ip4_routes_with_iface
 *
 * Flush all routes associated with a network device
 *
 */
void nm_system_device_flush_ip4_routes_with_iface (const char *iface)
{
	nm_generic_device_flush_ip4_routes_with_iface (iface);
}

/*
 * nm_system_device_has_active_routes
 *
 * Find out whether the specified device has any routes in the routing
 * table.
 *
 */
gboolean nm_system_device_has_active_routes (NMDevice *dev)
{
	return (FALSE);
}


/*
 * nm_system_device_flush_ip4_addresses
 *
 * Flush all network addresses associated with a network device
 *
 */
void nm_system_device_flush_ip4_addresses (NMDevice *dev)
{
	nm_generic_device_flush_ip4_addresses (dev);
}


/*
 * nm_system_device_flush_ip4_addresses_with_iface
 *
 * Flush all network addresses associated with a network device
 *
 */
void nm_system_device_flush_ip4_addresses_with_iface (const char *iface)
{
	nm_generic_device_flush_ip4_addresses_with_iface (iface);
}


#if 0
	/* Alert other computers of our new address */
	temp_addr.s_addr = addr;
	buf = g_strdup_printf ("/sbin/arping -q -A -c 1 -I %s %s", iface, inet_ntoa (temp_addr));
	nm_spawn_process (buf);
	g_free (buf);
	g_usleep (G_USEC_PER_SEC * 2);
	buf = g_strdup_printf ("/sbin/arping -q -U -c 1 -I %s %s", iface, inet_ntoa (temp_addr));
	nm_spawn_process (buf);
	g_free (buf);
#endif


/*
 * nm_system_enable_loopback
 *
 * Bring up the loopback interface
 *
 */
void nm_system_enable_loopback (void)
{
	nm_generic_enable_loopback ();
}


/*
 * nm_system_flush_loopback_routes
 *
 * Flush all routes associated with the loopback device, because it
 * sometimes gets the first route for ZeroConf/Link-Local traffic.
 *
 */
void nm_system_flush_loopback_routes (void)
{
	nm_generic_flush_loopback_routes ();
}


/*
 * nm_system_flush_arp_cache
 *
 * Flush all entries in the arp cache.
 *
 */
void nm_system_flush_arp_cache (void)
{
	nm_generic_flush_arp_cache ();
}


/*
 * nm_system_kill_all_dhcp_daemons
 *
 * Kill all DHCP daemons currently running, done at startup.
 *
 */
void nm_system_kill_all_dhcp_daemons (void)
{
}


/*
 * nm_system_update_dns
 *
 * Invalidate the nscd host cache, if it exists, since
 * we changed resolv.conf.
 *
 */
void nm_system_update_dns (void)
{
	if (g_file_test ("/usr/sbin/nscd", G_FILE_TEST_EXISTS | G_FILE_TEST_IS_EXECUTABLE | G_FILE_TEST_IS_REGULAR)) {
		nm_spawn_process ("/etc/init.d/nscd condrestart");
		nm_spawn_process ("/usr/sbin/nscd -i hosts");
	}
}


/*
 * nm_system_restart_mdns_responder
 *
 * Restart the multicast DNS responder so that it knows about new
 * network interfaces and IP addresses.
 *
 */
void nm_system_restart_mdns_responder (void)
{
#if defined(MDNS_PROVIDER_AVAHI)
	nm_info ("Restarting avahi-daemon");
	if (g_file_test ("/var/run/avahi-daemon/pid", G_FILE_TEST_EXISTS))
	{
		nm_spawn_process ("/etc/init.d/avahi-daemon restart");
	}
#elif defined(MDNS_PROVIDER_HOWL)
	if (g_file_text ("/var/run/mDNSResponder.pid", G_FILE_TEST_EXISTS))
	{
		nm_info ("Restarting mDNSResponder");
		nm_spawn_process ("/etc/init.d/mDNSResponder stop");
		nm_spawn_process ("/etc/init.d/mDNSResponder zap");
		nm_spawn_process ("/etc/init.d/mDNSResponder start");
	}
#elif defined(MDNS_PROVIDER_BONJOUR)
	if (g_file_test ("/var/run/mDNSResponderPosix.pid", G_FILE_TEST_EXISTS))
	{
		nm_info ("Restarting mDNSResponderPosix");
		nm_spawn_process ("/etc/init.d/mDNSResponderPosix restart");
	}
#else
	g_printerr("No mDNSResponder support enabled");
	g_assert_not_reached();
#endif
}

/*
 * nm_system_activate_nis
 *
 * set up the nis domain and write a yp.conf
 *
 */
void nm_system_activate_nis (NMIP4Config *config)
{
}

/*
 * nm_system_shutdown_nis
 *
 * shutdown ypbind
 *
 */
void nm_system_shutdown_nis (void)
{
}

/*
 * nm_system_set_hostname
 *
 * set the hostname
 *
 */
void nm_system_set_hostname (NMIP4Config *config)
{
}


/*
 * nm_system_should_modify_resolv_conf
 *
 * Can NM update resolv.conf, or is it locked down?
 */
gboolean nm_system_should_modify_resolv_conf (void)
{
	return TRUE;
}

