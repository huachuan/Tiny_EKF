#
# Makefile for TinyEKF GPS example
#
# Copyright (C) 2015 Simon D. Levy
#
# MIT License

SRC = ../../src

runs: server
	./server
runc: client
	./client

server: server.c $(SRC)/tiny_ekf.c $(SRC)/tiny_ekf.h
	g++ -Wall -I. -I$(SRC) -o server server.cpp gps_ekf.c $(SRC)/tiny_ekf.c -lm
client: client.c gpsdata.h
	gcc -Wall -I. client.c -o client

clean:
	rm -f server client gpa_ekf *.o *~ ekf.csv
