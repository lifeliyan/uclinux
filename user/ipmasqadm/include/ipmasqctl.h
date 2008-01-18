#ifndef _IPMASQCTL_H
#define _IPMASQCTL_H


/*
 * 	$Id: ipmasqctl.h 1012 2005-07-25 04:03:16Z magicyang $
 *
 * 	This file "opens" setsockopt() options name space for
 *	user-level masq operations
 */

#include <net/if.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netinet/udp.h>
#include <netinet/tcp.h>

#include <linux/types.h>
#include <asm/types.h>

/*
 *	Nice glibc2 kLuDge
 */
#if defined(__GLIBC__) || (__GLIBC__ > 2)
#define _LINUX_IN_H
#define _LINUX_IP_H
#define _LINUX_ICMP_H
#define _LINUX_UDP_H
#define _LINUX_TCP_H
#define _LINUX_BYTEORDER_GENERIC_H
#endif

#include <linux/ip_fw.h>

#endif
