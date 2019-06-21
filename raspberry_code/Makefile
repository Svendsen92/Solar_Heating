CFLAGS = -Wall -std=c++14 -Iinclude -c -o

all:
	g++ $(CFLAGS) objFiles/main.o src/main.cpp 
	g++ $(CFLAGS) objFiles/tcp_ip.o src/tcp_ip.cpp
	g++ $(CFLAGS) objFiles/activities.o src/activities.cpp
	g++ -Wall -std=c++14 -o main objFiles/main.o objFiles/tcp_ip.o objFiles/activities.o -lpthread -lwiringPi
	echo "\nThe project has been successfully compiled"

clean:
	rm main
	rm objFiles/main.o
	rm objFiles/tcp_ip.o
	re objFiles/activities.o