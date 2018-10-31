CC=g++

daemon: daemon.cpp
	$(CC) -o daemon daemon.cpp notify.h notify.cpp
clean:
	rm -f daemon
	rm -f *.o
