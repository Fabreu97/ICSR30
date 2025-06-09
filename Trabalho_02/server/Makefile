COMPILER = g++
FLAGS = -Wall
SRCS = server.cpp main.cpp

all:
	g++ $(FLAGS) -c server.cpp -o server.o
	g++ $(FLAGS) -c main.cpp -o main.o
	g++ server.o main.o -o main
	./main || rm -rf *.o main


clean:
	rm -rf *.o main