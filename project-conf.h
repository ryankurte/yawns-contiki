
#ifndef PROJECT_TEST_CONF_H_
#define PROJECT_TEST_CONF_H_


#define NETSTACK_CONF_WITH_IPV6         1
#define UIP_CONF_ROUTER                 1
#define UIP_CONF_IPV6_RPL               1
#define HARD_CODED_ADDRESS      "bbbb::10"

#define UIP_CONF_ND6_SEND_RA            0
#define UIP_CONF_ND6_REACHABLE_TIME     600000
#define UIP_CONF_ND6_RETRANS_TIMER      10000

#if RPL_BORDER_ROUTER

#undef UIP_CONF_ROUTER
#define UIP_CONF_ROUTER             1

//#undef UIP_FALLBACK_INTERFACE
//#define UIP_FALLBACK_INTERFACE rpl_interface

#undef HARD_CODED_ADDRESS
#define HARD_CODED_ADDRESS            "bbbb::1"

#endif

#endif
