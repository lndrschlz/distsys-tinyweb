# Makefile für Webserver Laboruebungen
CFLAGS=-Wall -Werror -Wunreachable-code -Wswitch-default -std=gnu99 -pedantic -O2
CC=gcc

.PHONY: all
all: file-o-res #echos

.PHONY: file-o-res
file-o-res:  file-o-res.o
	gcc -o file-o-res file-o-res.o libsockets.a

.PHONY: echos
echos: echos.o handleclient.o
	gcc -o echos echos.o handleclient.o libsockets.a

%.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@

#echos.o: 	
#	gcc -o echos.o -c echos.c

#handleclient.o:
#	gcc -o handleclient.o -c handleclient.c

.PHONY: clean
clean:
	rm echos *.o file-o-res echos
