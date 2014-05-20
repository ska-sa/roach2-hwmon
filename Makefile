#KATCP ?= ../katcp

#include ../Makefile.inc

#INC = -I$(KATCP)
#LIB = -L$(KATCP) -lkatcp
#BUILD = unknown-0.1


SENSORLIB ?= ../lm-sensors/

INC = -I$(SENSORLIB)
LIB = -L$(SENSORLIB)/lib -lsensors

CC = $(CROSS_COMPILE)gcc
CFLAGS = -Wall -ggdb -O0
EXE = r2hwmond

all: $(EXE)

r2hwmond: main.c fork-parent.c sensorlib.c
	$(CC) $(CFLAGS) -o $@ $(INC) $^ $(LIB)
#	$(CC) -DBUILD=\"$(BUILD)\" $(CFLAGS) $(INC) -o $@ $^ $(LIB)

clean: 
	$(RM) $(EXE)

install: $(EXE)
	$(ECHO) "not installing examples"

