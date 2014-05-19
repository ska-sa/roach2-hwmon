#KATCP ?= ../katcp

#include ../Makefile.inc

#INC = -I$(KATCP)
#LIB = -L$(KATCP) -lkatcp
#BUILD = unknown-0.1

CC = $(CROSS_COMPILE)gcc
CFLAGS = -Wall
EXE = r2hwmond

all: $(EXE)

r2hwmond: main.c fork-parent.c
	$(CC) $(CFLAGS) -o $@ $^
#	$(CC) -DBUILD=\"$(BUILD)\" $(CFLAGS) $(INC) -o $@ $^ $(LIB)

clean: 
	$(RM) $(EXE)

install: $(EXE)
	$(ECHO) "not installing examples"

