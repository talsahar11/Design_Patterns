
#export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:.

all: server

server: Server.c reactor.so
	gcc -c Server.c -o server.o
	gcc server.o -o server -L. -lreactor -pthread

reactor.so: reactor.c
	gcc -shared -fPIC -o libreactor.so reactor.c
