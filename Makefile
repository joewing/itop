#############################################################################
# Makefile for itop
#############################################################################

CC = gcc
CFLAGS = -O2 -g -Wall
LFLAGS = 

OBJECTS = load.o username.o process.o misc.o command.o help.o cpu.o \
	memory.o swap.o main.o
EXE = itop

.SUFFIXES: .c .o .h

all: $(EXE)

$(EXE): $(OBJECTS)
	$(CC) -o $(EXE) $(OBJECTS) $(LFLAGS)

.c.o:
	$(CC) -c $(CFLAGS) $*.c

clean:
	rm -f $(EXE) $(OBJECTS) core

