##################################
############ MAKEFILE ############
##################################

LIBS = -lssl -lcrypto

all:
	g++ -c ../utils/utils.cpp -o ../utils/utils.o
	g++ -c ../utils/packet.cpp -o ../utils/packet.o
	g++ -c server.cpp -o server.o
	g++ -c main.cpp -o main.o
	g++ ../utils/packet.o ../utils/utils.o server.o main.o -o main $(LIBS)

run:
	./main

clean:
	rm -rf *.o main ../utils/*.o