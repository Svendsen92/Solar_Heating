CFLAGS = -Wall -Wno-psabi -std=c++14 -Iinclude -c -o

LIB_FLAGS = -lpthread -lwiringPi

all:
	g++ $(CFLAGS) objFiles/main.o src/main.cpp
	g++ $(CFLAGS) objFiles/tcp_ip.o src/tcp_ip.cpp
	g++ $(CFLAGS) objFiles/timing.o src/timing.cpp
	g++ $(CFLAGS) objFiles/csv_logger.o src/csv_logger.cpp
	g++ $(CFLAGS) objFiles/activities.o src/activities.cpp
	g++ $(CFLAGS) objFiles/sharedStruct.o src/sharedStruct.cpp
	g++ $(CFLAGS) objFiles/appControl.o src/appControl.cpp
	g++ $(CFLAGS) objFiles/flowControl.o src/flowControl.cpp
	g++ $(CFLAGS) objFiles/solarControl.o src/solarControl.cpp
	@echo ""
	g++ -Wall -std=c++14 -o main objFiles/main.o objFiles/tcp_ip.o objFiles/timing.o objFiles/csv_logger.o objFiles/activities.o objFiles/sharedStruct.o objFiles/appControl.o objFiles/flowControl.o objFiles/solarControl.o  $(LIB_FLAGS)
	@echo "\nThe project has been successfully compiled"

clean:
	rm main
	rm objFiles/main.o
	rm objFiles/tcp_ip.o
	rm objFiles/timing.o
	rm objFiles/csv_logger.o
	rm objFiles/activities.o
	rm objFiles/sharedStruct.o
	rm objFiles/appControl.o
	rm objFiles/flowControl.o
	rm objFiles/solarControl.o
