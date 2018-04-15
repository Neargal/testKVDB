CC = g++-6

CFLAGS = -c -Wall -std=c++11

LIBS = -lzmq

all: testKVDB

testKVDB: main.o zmqServerBD.o queuePUB.o baseData.o listSortedTTL.o listTable.o hashTable.o linkedList.o
	$(CC) main.o zmqServerBD.o queuePUB.o baseData.o listSortedTTL.o listTable.o hashTable.o linkedList.o -o testKVDB $(LIBS)

main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp

zmqServerBD.o: zmqServerBD.cpp
	$(CC) $(CFLAGS) zmqServerBD.cpp

queuePUB.o: queuePUB.cpp
	$(CC) $(CFLAGS) queuePUB.cpp

baseData.o: baseData.cpp
	$(CC) $(CFLAGS) baseData.cpp

listSortedTTL.o: listSortedTTL.cpp
	$(CC) $(CFLAGS) listSortedTTL.cpp

listTable.o: listTable.cpp
	$(CC) $(CFLAGS) listTable.cpp

hashTable.o: hashTable.cpp
	$(CC) $(CFLAGS) hashTable.cpp

linkedList.o: linkedList.cpp
	$(CC) $(CFLAGS) linkedList.cpp

clean:
	rm -rf *o testKVDB
