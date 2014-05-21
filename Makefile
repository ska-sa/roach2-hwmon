#KATCP ?= ../katcp

#include ../Makefile.inc

#INC = -I$(KATCP)
#LIB = -L$(KATCP) -lkatcp
#BUILD = unknown-0.1

SENSORLIB ?= ../lm-sensors/
KATCPLIB ?= ../katcp_devel/katcp/

INC = -I$(SENSORLIB) -I$(KATCPLIB)
LIB = -L$(SENSORLIB)lib -L$(KATCPLIB) -lsensors -lkatcp

CC = $(CROSS_COMPILE)gcc
CFLAGS = -Wall -ggdb -O0
EXE = r2hwmond

all: $(EXE)

r2hwmond: main.c fork-parent.c sensorlib.c chips.c log.c
	$(CC) $(CFLAGS) -o $@ $(INC) $^ $(LIB)
#	$(CC) -DBUILD=\"$(BUILD)\" $(CFLAGS) $(INC) -o $@ $^ $(LIB)

clean: 
	$(RM) $(EXE)

install: $(EXE)
	$(ECHO) "Not installing examples."

