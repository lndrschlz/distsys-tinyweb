# Makefile für Webserver Laboruebungen
CFLAGS=-Wall -Werror -Wunreachable-code -Wswitch-default -std=gnu99 -pedantic -O2
CC=gcc

.PHONY: all
all: file-o-res file-o-req echos

.PHONY: file-o-req
file-o-req:	file-o-req.o
	$(CC) -o file-o-req file-o-req.o libsockets.a

.PHONY: file-o-res
file-o-res:  file-o-res.o
	$(CC) -o file-o-res file-o-res.o libsockets.a

.PHONY: echos
echos: echos.o handleclient.o
	$(CC) -o echos echos.o handleclient.o libsockets.a

%.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@

.PHONY: clean
clean:
	-rm echos
	-rm file-o-res
	-rm file-o-req
	-rm *.o 
