
COMPILER = gcc
CFLAGS   = -Wall
SRCS     = main.c
OBJS     = $(SRCS:.c=.o)
TARGET   = main

DIRS = "bin" "obj" "src" "include"

# Regra padrão (alvo padrão do make)
all: $(TARGET)

# Como gerar o executável
$(TARGET): create_dirs $(OBJS)
	$(COMPILER) ./obj/$(OBJS) -o $(TARGET)
	mv $(TARGET) ./bin/

create_dirs:
	mkdir -p $(DIRS)
	-mv $(SRCS) ./src/
	-mv $(SRCS:.c=.h) ./include/
	echo "Diretórios criados com sucesso!"
# Como gerar arquivos .o a partir de .c
%.o: src/%.c
	$(COMPILER) $(CFLAGS) -c $< -o $@
	-mv $@ ./obj/
# Limpar arquivos gerados
clean:
	-mv ./src/* ./
	-mv ./include/* ./
	rm -rf $(DIRS)