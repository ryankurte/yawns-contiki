CONTIKI_PROJECT = test
all: $(CONTIKI_PROJECT)

CONTIKI = ./contiki

TARGETDIRS += targets
TARGET=yawns

PROJECT_SOURCEFILES += yawns-radio.c

#APP=shell

CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\" -DNETSTACK_CONF_RADIO=yawns_driver
CFLAGS += -DUIP_CONF_IPV6_RPL=1 -DNETSTACK_CONF_WITH_IPV6=1

#CFLAGS += -DDEBUG=DEBUG_FULL
#CFLAGS += -DRPL_BORDER_ROUTER=1

CFLAGS += $(CLIFLAGS)

include $(CONTIKI)/Makefile.include

server:
	make clean && make CLIFLAGS="-DRPL_BORDER_ROUTER=1" && mv test.yawns server.yawns

client:
	make clean && make CLIFLAGS="" && mv test.yawns client.yawns

run: server client
	owns-sim -c simple.yml
	