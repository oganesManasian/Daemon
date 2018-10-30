CC=g++

daemon: daemon.cpp
	$(CC) -o daemon daemon.cpp notify.h notify.cpp config.h config.cpp
clean:
	rm -f daemon
	rm -f *.o
