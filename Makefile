# Makefile for roach 2 hardware monitor

SENSORLIB ?= ../lm-sensors/
KATCPLIB ?= ../katcp_devel/katcp/

INC = -I$(SENSORLIB) -I$(KATCPLIB)
LIB = -L$(SENSORLIB)lib -L$(KATCPLIB) -lsensors -lkatcp

CC = $(CROSS_COMPILE)gcc
CFLAGS = -Wall -DKATCP_USE_FLOATS
EXE = r2hwmond
DBG_EXE = r2hwmondd
SOURCE = main.c fork-parent.c sensorlib.c chips.c log.c sense.c alarm.c
ECHO = echo

SERVER = dbelab00

all: $(EXE)

r2hwmond:
	$(CC) $(CFLAGS) -O2 -o $@ $(INC) $(SOURCE) $(LIB)

debug: 
	$(CC) $(CFLAGS) -ggdb -o $(DBG_EXE) $(INC) $(SOURCE) $(LIB)

clean: 
	$(RM) $(EXE)
	$(RM) $(DBG_EXE)
	
static: 
	$(CC) $(CFLAGS) -02 -o $(EXE) $(INC) $(SOURCE) -static $(LIB) -lm

install: $(EXE)
	$(ECHO) "Attempting to copy $(EXE) to server..."
	scp $(EXE) $(USER)@$(SERVER):/home/nfs/test/.

