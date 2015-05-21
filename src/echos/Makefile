# Makefile für Beispielprogramm
# ECHOS.C

all: echos

echos: echos.o handleclient.o
	gcc -o echos echos.o handleclient.o

echos.o: 
	gcc -o echos.o -c echos.c

handleclient.o:
	gcc -o handleclient.o -c handleclient.c

clean:
	rm echos echos.o handleclient.o
