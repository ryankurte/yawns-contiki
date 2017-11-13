/*
 * Copyright (c) 2005, Swedish Institute of Computer Science
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 */

#ifndef CONTIKI_CONF_H_
#define CONTIKI_CONF_H_

#include <inttypes.h>
#ifndef WIN32_LEAN_AND_MEAN
#include <sys/select.h>
#endif

struct select_callback {
  int  (* set_fd)(fd_set *fdr, fd_set *fdw);
  void (* handle_fd)(fd_set *fdr, fd_set *fdw);
};
int select_set_callback(int fd, const struct select_callback *callback);

#define CC_CONF_REGISTER_ARGS          1
#define CC_CONF_FUNCTION_POINTER_ARGS  1
#define CC_CONF_VA_ARGS                1
/*#define CC_CONF_INLINE                 inline*/

#ifndef EEPROM_CONF_SIZE
#define EEPROM_CONF_SIZE				1024
#endif

#define CCIF
#define CLIF

#define NETSTACK_CONF_RADIO   yawns_driver

typedef unsigned short uip_stats_t;

#if NETSTACK_CONF_WITH_IPV6
/* Minimal-net gets a 6 byte ethernet MAC assigned in uip.c, currently {0x00,0x06,0x98,0x00,0x02,0x32}
 * This gets converted to a link layer address of [fe80::206:98ff:fe00:232]
 * You could change the uip.c address when running multiple instances, however HARD_CODED_ADDRESS
 * can also specify a different MAC address if any of the last three bytes are non-zero. It can also
 * specify a prefix if any of the first four longs are nonzero. RPL builds use this to pass a prefix
 * to the border router and also to ensure it has a different link-layer address from the RPL end node.
 * HARD_CODED_ADDRESS can also be changed in /platform/minimal-net/contiki-main.c so as not to force complete
 * rebuilds when making multiple instances of a minimal-net client.
*/
//#define HARD_CODED_ADDRESS      "::10" //assign link-layer address fe80::ff:fe00:10, wait for RA or RPL prefix
//#define HARD_CODED_ADDRESS      "fdfd::" //assign prefix; address becomes fdfd::206:98ff:fe00:232
//#define HARD_CODED_ADDRESS      "fdfd::10" //assign prefix and ipv6 address fdfd::ff:fe00:10

/* The status.shtml page shows addresses, neighbors, and routes on ipv6 builds. Use this define to enable
 * the needed routines in httpd-cgi.c on the webserver6 build. The status page is present in
 * /apps/webserver/httpd-fs/ but not in the default /apps/webserver/httpd-fsdata.c file.
 * To include it run the PERL script /../../tools/makefsdata from the /apps/webserver/ directory.
 * NB: Webserver builds on all platforms will use the current httpd-fsdata.c file. The added 160 bytes
 * could overflow memory on the smaller platforms.
 */
#define WEBSERVER_CONF_STATUSPAGE   1

/* RPL currently works only on Windows. *nix would require converting the tun interface to two pcap tees. */
//#define RPL_BORDER_ROUTER           0
#endif

#if UIP_CONF_IPV6_RPL
/* RPL motes use the uip.c link layer address or optionally the harded coded address (but without the prefix!)
 * Different instances can be made by changing the link layer portion of HARD_CODED_ADDRESS in contiki-main.c
 * Rename them to e.g. webserver6.10, webserver6.11, ...
 * They should all attach to a minimal-net rpl border that uses the same primary interface.
 * For multihop testing, configure intermediate notes as routers.
 */
#define HARD_CODED_ADDRESS      "bbbb::10"  //the prefix is ignored for a rpl node
#define UIP_CONF_ROUTER                 0
#define UIP_CONF_ND6_SEND_RA            0
#define UIP_CONF_ND6_REACHABLE_TIME     600000
#define UIP_CONF_ND6_RETRANS_TIMER      10000

#if RPL_BORDER_ROUTER
/* RPL border router accepts packets from the host through the fallback and directs them to
 * the primary interface. Thus the fallback and rpl dag prefix must be the same. The prefix of
 * the primary interface does not matter!
 * Rename this build to e.g. webrouter. Then on Windows create two loopback interfaces, bbbb:: and fdfd::
 * Attach the RPL end nodes to fdfd:: and the webrouter to fdfd:: with bbbb:: as the fallback.
 * Direct browser to bbbb::ff:fe00:1/status.html, bbbb::ff:fe00:10/status.html, bbbb::ff:fe00:20/status.html.
 * The traffic will go through the bbbb:: interface to the router, then out the fdfd:: interface to the end
 * nodes. The end nodes must be explicitly added as neighbors to avoid a blocking NS
 * netsh interface ipv6 add neighbor bbbb::ff:fe00:10 33-33-ff-33-44-10 interface=16 (# of the bbbb interface)
 * netsh interface ipv6 add neighbor bbbb::ff:fe00:20 33-33-ff-33-44-20 interface=16 (# of the bbbb interface)
 *
 * Instead of using the fdfd:: loopback it is also possible to attach the border router to another interface,
 * e.g. the jackdaw RNDIS <->  repeater. Then RPL will configure on the radio network and the RF motes will
 * be reached through bbbb::<mote link layer address>.
 * Possibly minimal-net RPL motes could also be added to this interface?
 *
 */
#undef UIP_CONF_ROUTER
#define UIP_CONF_ROUTER             1
//#define RPL_CONF_STATS              0
//#define UIP_CONF_BUFFER_SIZE	 1300
#undef UIP_FALLBACK_INTERFACE
#define UIP_FALLBACK_INTERFACE rpl_interface
//#define WPCAP_FALLBACK_ADDRESS     "bbbb::1"  //bbbb::1 is the default fallback prefix
#undef HARD_CODED_ADDRESS
#define HARD_CODED_ADDRESS            "bbbb::1" //bbbb::ff:fe00:1 is the RPL border router default
//#define UIP_CONF_ND6_SEND_RA		0
//#define UIP_CONF_ND6_REACHABLE_TIME 600000
//#define UIP_CONF_ND6_RETRANS_TIMER  10000
#endif

#endif

#define UIP_CONF_LLH_LEN              14
#define UIP_CONF_MAX_LISTENPORTS      40
#define UIP_CONF_MAX_CONNECTIONS      40
#define UIP_CONF_BYTE_ORDER           UIP_LITTLE_ENDIAN
#define UIP_CONF_TCP_SPLIT            0
#define UIP_CONF_IP_FORWARD           0
#define UIP_CONF_LOGGING              0
#define UIP_CONF_UDP_CHECKSUMS        1

/* Not used but avoids compile errors while sicslowpan.c is being developed */
#define SICSLOWPAN_CONF_COMPRESSION       SICSLOWPAN_COMPRESSION_HC06

#define NETSTACK_CONF_LINUXRADIO_DEV "wpan0"

#define UIP_CONF_UDP                  1
#define UIP_CONF_TCP                  1

#if NETSTACK_CONF_WITH_IPV6
#define UIP_CONF_IPV6_QUEUE_PKT       1
#define UIP_CONF_IPV6_CHECKS          1
#define UIP_CONF_IPV6_REASSEMBLY      1
//#define UIP_CONF_NETIF_MAX_ADDRESSES  5
#define NBR_TABLE_CONF_MAX_NEIGHBORS     100
#define UIP_CONF_DS6_DEFRT_NBU   2
#define UIP_CONF_DS6_PREFIX_NBU  5
#define UIP_CONF_MAX_ROUTES   100
#define UIP_CONF_DS6_ADDR_NBU    10
#define UIP_CONF_DS6_MADDR_NBU   0
#define UIP_CONF_DS6_AADDR_NBU   0
#endif /* NETSTACK_CONF_WITH_IPV6 */



#include "ctk-config.h"

typedef unsigned long clock_time_t;

#define CLOCK_CONF_SECOND 1000

#define LOG_CONF_ENABLED 1

#define PROGRAM_HANDLER_CONF_MAX_NUMDSCS 10
#define PROGRAM_HANDLER_CONF_QUIT_MENU   1

#define EMAIL_CONF_WIDTH  78
#define EMAIL_CONF_HEIGHT 17
#ifndef PLATFORM_BUILD
#define EMAIL_CONF_ERASE   0
#endif

#define IRC_CONF_WIDTH         78
#define IRC_CONF_HEIGHT        17
#define IRC_CONF_SYSTEM_STRING "*nix"

#define SHELL_CONF_WITH_PROGRAM_HANDLER 1

#define SHELL_GUI_CONF_XSIZE 78
#define SHELL_GUI_CONF_YSIZE 17

#ifdef PLATFORM_BUILD
#define TELNETD_CONF_GUI 1
#endif /* PLATFORM_BUILD */

#ifdef PLATFORM_BUILD
#define WWW_CONF_WEBPAGE_WIDTH  78
#define WWW_CONF_WEBPAGE_HEIGHT 17
#endif /* PLATFORM_BUILD */

/* Not part of C99 but actually present */
int strcasecmp(const char*, const char*);

/* include the project config */
/* PROJECT_CONF_H might be defined in the project Makefile */
#ifdef PROJECT_CONF_H
#include PROJECT_CONF_H
#endif /* PROJECT_CONF_H */

#endif /* CONTIKI_CONF_H_ */
