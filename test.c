/*
 * Copyright (c) 2006, Swedish Institute of Computer Science.
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

/**
 * \file
 *         A very simple Contiki application showing how Contiki programs look
 * \author
 *         Adam Dunkels <adam@sics.se>
 */

#include "contiki.h"
#include "contiki-net.h"

#include "net/ip/uip.h"

#include <stdio.h> /* For printf() */


#if RPL_BORDER_ROUTER
#include "net/rpl/rpl.h"

static uint16_t dag_id[] = {0x1111, 0x1100, 0, 0, 0, 0, 0, 0x0011};

void sprint_ip6(uip_ip6addr_t addr)
{
    unsigned char i = 0;
    unsigned char zerocnt = 0;
    unsigned char numprinted = 0;
    unsigned char notskipped = 0;
    char thestring[40];
    char *result = thestring;

    *result++ = '[';
    while(numprinted < 8) {
        if((addr.u16[i] == 0) && (zerocnt == 0)) {
            while(addr.u16[zerocnt + i] == 0) {
                zerocnt++;
            }
            if(zerocnt == 1 && notskipped) {
                *result++ = '0';
                numprinted++;
                notskipped = 1;
                continue;
            }
            i += zerocnt;
            numprinted += zerocnt;
        } else {
            result += sprintf(result, "%x", (unsigned int)(uip_ntohs(addr.u16[i])));
            i++;
            numprinted++;
        }
        if(numprinted != 8) {
            *result++ = ':';
        }
    }
    *result++=']';
    *result=0;
    printf("%s", thestring);
}

PROCESS(border_router_process, "RPL Border Router");
AUTOSTART_PROCESSES(&border_router_process);
PROCESS_THREAD(border_router_process, ev, data)
{

    PROCESS_BEGIN();

    printf("Hello, server\n");

    PROCESS_PAUSE();

    {
        rpl_dag_t *dag;
        char buf[sizeof(dag_id)];
        memcpy(buf,dag_id,sizeof(dag_id));
        dag = rpl_set_root(0, (uip_ip6addr_t *)buf);

        /* Assign separate addresses to the uip stack and the host network
            interface, but with the same prefix E.g. bbbb::ff:fe00:200 to
            the stack and bbbb::1 to the host *fallback* network interface
            Otherwise the host will trap packets intended for the stack,
            just as the stack will trap packets intended for the host
            $ifconfig usb0 -arp on Ubuntu to skip the neighbor
            solicitations. Add explicit neighbors on other OSs */

        if(dag != NULL) {
            printf("Created a new RPL dag\n");


            int i;
            uip_ip6addr_t ipaddr;
#ifdef HARD_CODED_ADDRESS
            uiplib_ipaddrconv(HARD_CODED_ADDRESS, &ipaddr);
#else
            uip_ip6addr(&ipaddr, 0xbbbb, 0, 0, 0, 0, 0, 0, 0x1);
#endif
            uip_ds6_set_addr_iid(&ipaddr, &uip_lladdr);
            uip_ds6_addr_add(&ipaddr, 0, ADDR_AUTOCONF);
            rpl_set_prefix(dag, &ipaddr, 64);

            for(i = 0; i < UIP_DS6_ADDR_NB; i++) {
                if(uip_ds6_if.addr_list[i].isused) {
                    printf("IPV6 Address: ");
                    sprint_ip6(uip_ds6_if.addr_list[i].ipaddr);
                    printf("\n");
                }
            }
        }
    }
    /* The border router runs with a 100% duty cycle in order to ensure high
       packet reception rates. */
    /* NETSTACK_MAC.off(1); */

    while(1) {
        PROCESS_YIELD();
        /* Local and global dag repair can be done from ? */
        /*  rpl_set_prefix(rpl_get_dag(RPL_ANY_INSTANCE), &ipaddr, 64);
        rpl_repair_dag(rpl_get_dag(RPL_ANY_INSTANCE)); */
    }

    PROCESS_END();
  }
  
# else

  /*---------------------------------------------------------------------------*/
PROCESS(hello_world_process, "Hello world process");
AUTOSTART_PROCESSES(&hello_world_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(hello_world_process, ev, data)
{
    PROCESS_BEGIN();

    printf("Hello, client\n");

    PROCESS_END();
}
/*---------------------------------------------------------------------------*/


#endif /* RPL_BORDER_ROUTER */
