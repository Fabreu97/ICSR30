##################################
############ MAKEFILE ############
##################################
all:
	g++ -c client.cpp -o client.o
	g++ -c main.cpp -o main.o
	g++ client.o main.o -o main

run:
	./main

clean:
	rm -rf *.o main