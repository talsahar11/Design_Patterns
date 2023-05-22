all: react_server

react_server: react_server.c st_reactor.so
	gcc -c react_server.c -o react_server.o
	gcc react_server.o -o react_server -L. ./st_reactor.so -pthread
	rm react_server.o

st_reactor.so: st_reactor.c
	gcc -shared -fPIC -Wl,--soname=st_reactor.so -o st_reactor.so st_reactor.c

clean:
	rm st_reactor.so react_server