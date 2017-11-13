CONTIKI_PROJECT = hello-world
all: $(CONTIKI_PROJECT)

CONTIKI = ../contiki

TARGETDIRS += targets
PROJECT_SOURCEFILES += yawns-radio.c

TARGET=yawns-net
#APP=shell

DEFINES += NETSTACK_CONF_WITH_IPV6 UIP_CONF_IPV6_RPL RPL_BORDER_ROUTER

TARGET_LIBFILES += -lncurses -lowns -lzmq -lczmq -lprotobuf -lprotobuf-c -lpthread

include $(CONTIKI)/Makefile.include


