##################################
############ MAKEFILE ############
##################################

# Adiciona a flag -g para incluir símbolos de depuração
CFLAGS = -g
LIBS = -lssl -lcrypto

all:
	g++ $(CFLAGS) -c ../utils/utils.cpp -o ../utils/utils.o
	g++ $(CFLAGS) -c ../utils/packet.cpp -o ../utils/packet.o
	g++ $(CFLAGS) -c server.cpp -o server.o
	g++ $(CFLAGS) -c main.cpp -o main.o
	g++ $(CFLAGS) ../utils/packet.o ../utils/utils.o server.o main.o -o main $(LIBS)

run:
	./main

clean:
	rm -rf *.o main ../utils/*.o