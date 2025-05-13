COMPILER    = gcc
CFLAGS      = -Wall -Iinclude

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
	-rm -f ./bin/*
	@echo "Diretórios criados com sucesso!"

object_files_create:
	gcc $(CFLAGS) -c src/packet.c -o obj/packet.o
	gcc $(CFLAGS) -c src/main.c -o obj/main.o

$(TARGET): create_dirs object_files_create
	$(COMPILER) ./obj/main.o ./obj/packet.o -o $(TARGET)
	mv $(TARGET) ./bin/

run:
	./bin/$(TARGET)


# Limpar arquivos gerados
clean:
	-mv ./src/* ./
	-mv ./include/* ./
	rm -rf $(DIRS)