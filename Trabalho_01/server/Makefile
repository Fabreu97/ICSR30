COMPILER    = gcc
FLAGS      	= -Wall -Iinclude -std=c17
SRC			= ./src/utils.c ./src/packet.c ./src/main.c
OBJ			= ./obj/utils.c ./obj/packet.o ./obj/main.o
TARGET      = main

DIRS        = bin obj src include

# Regra padrão (alvo padrão do make)
all: $(TARGET)

create_dirs:
	mkdir -p $(DIRS)
	-cp ../utils/*.h ./include/
	-cp ../utils/*.c ./src/
	-mv *.c ./src/
	-mv *.h ./include/
	@echo "Diretórios criados com sucesso!"



object_files_create:
	gcc $(FLAGS) -c src/utils.c -o obj/utils.o
	gcc $(FLAGS) -c src/packet.c -o obj/packet.o
	gcc $(FLAGS) -c src/main.c -o obj/main.o

%.o: %.c
	$(COMPILER) $(FLAGS) -c $< -o $@


$(TARGET): create_dirs object_files_create
	$(COMPILER) ./obj/main.o ./obj/packet.o ./obj/utils.o -o $(TARGET)
	mv $(TARGET) ./bin/

run:
	./bin/$(TARGET)


# Limpar arquivos gerados
clean:
	-mv ./src/* ./
	-mv ./include/* ./
	rm -rf $(DIRS)