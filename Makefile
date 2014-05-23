# Makefile for roach 2 hardware monitor

SENSORLIB ?= ../lm-sensors/
KATCPLIB ?= ../katcp_devel/katcp/

INC = -I$(SENSORLIB) -I$(KATCPLIB)
LIB = -L$(SENSORLIB)lib -L$(KATCPLIB) -lsensors -lkatcp

CC = $(CROSS_COMPILE)gcc
CFLAGS = -Wall -ggdb -O0
EXE = r2hwmond

SOURCE = main.c fork-parent.c sensorlib.c chips.c log.c sense.c alarm.c

all: $(EXE)

r2hwmond:
	$(CC) $(CFLAGS) -o $@ $(INC) $(SOURCE) $(LIB)

clean: 
	$(RM) $(EXE)
	
static: 
	$(CC) $(CFLAGS) -o $(EXE) $(INC) $(SOURCE) -static $(LIB) -lm

install: $(EXE)
	$(ECHO) "Attempt to copy to server..."
	scp $^ $(USER)@$(SERVER):/home/nfs/test/.

