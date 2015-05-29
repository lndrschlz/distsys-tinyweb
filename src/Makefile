# Makefile für Webserver Laboruebungen
CFLAGS=-Wall -Werror -Wunreachable-code -Wswitch-default -std=gnu99 -pedantic -O2 -g
CC=gcc
LIBPATH=../lib/libsockets

.PHONY: all 
all: lib file-o-res file-o-req echos 

lib:
	make -C $(LIBPATH)

file-o-req:	file-o-req.o
	$(CC) -o file-o-req file-o-req.o $(LIBPATH)/libsockets.a

file-o-res:  file-o-res.o
	$(CC) -o file-o-res file-o-res.o $(LIBPATH)/libsockets.a

echos: echos.o handleclient.o
	$(CC) -o echos echos.o handleclient.o $(LIBPATH)/libsockets.a

%.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@ -I $(LIBPATH)

.PHONY: clean
clean:
	-rm echos
	-rm file-o-res
	-rm file-o-req
	-rm *.o 
