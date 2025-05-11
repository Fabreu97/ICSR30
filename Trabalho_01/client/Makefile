
COMPILER 	= gcc
CFLAGS   	= -Wall -O2 -Iinclude
SRCS     	= main.c
OBJS     	= $(SRCS:.c=.o)
INCLUDES 	= 
TARGET   	= main

DIRS = "bin" "obj" "src" "include"

# Regra padrão (alvo padrão do make)
all: $(TARGET)

# Como gerar o executável
$(TARGET): create_dirs
	$(COMPILER) ./obj/$(OBJS) -o $(TARGET)
	mv $(TARGET) ./bin/

create_dirs:
	mkdir -p $(DIRS)
	-mv *.c ./src/
	-mv $(INCLUDES) ./include/
	@echo "Diretórios criados com sucesso!"
	@make obj/$(OBJS)

# Como gerar arquivos .o a partir de .c
obj/%.o: src/%.c include/$(INCLUDES)
	$(COMPILER) $(CFLAGS) -c $< -o $@
	-mv $< ./src/
	-mv $@ ./obj/

src/%.c:
	$(COMPILER) $(CFLAGS) -c $< -o $@
	-mv $@ ./obj/

run:
	./bin/main

# Limpar arquivos gerados
clean:
	-mv ./src/* ./
	-mv ./include/* ./
	rm -rf $(DIRS)