# Makefile für Beispielprogramm
# ECHOS.C

all: file-o-res echos

file-o-res: 
	gcc -o file-o-res file-o-res.c libsockets.a

echos: echos.o handleclient.o
	gcc -o echos echos.o handleclient.o libsockets.a

echos.o: 	
	gcc -o echos.o -c echos.c

handleclient.o:
	gcc -o handleclient.o -c handleclient.c

clean:
	rm echos *.o file-o-res
