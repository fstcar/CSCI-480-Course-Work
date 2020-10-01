# Robert Oury
# CSCI 480
# 10/10/19

CC = g++
CFLAGS = -std=c++11 -Wall
OBJECTS = assign4.o process.cc

all: Assign4

assign4.o: assign4.cc
	$(CC) $(CFLAGS) -c assign4.cc

process.o: process.cc
	$(CC) $(CFLAGS) -c process.cc

Assign4: $(OBJECTS)
	$(CC) $(OBJECTS) -o Assign4

clean:
	rm -f *.o Assign4