# Makefile para Projeto ACD2
CC = gcc
CFLAGS = -Wall -Wextra -g -std=c99
TARGET = projeto

# Lista de arquivos - AJUSTE ESTA LINDA COM OS ARQUIVOS QUE VOCÊ TEM!
SOURCES = projeto_main.c dataset.c abb.c relatorio_ordenado.c matriz.c a_avl.c
HEADERS = projeto_acd2.h
OBJECTS = $(SOURCES:.c=.o)

# Regra principal
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS) -lm

# Regra para compilar cada arquivo .c
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Limpeza
clean:
	rm -f $(OBJECTS) $(TARGET)

# Executar
run: $(TARGET)
	./$(TARGET)

.PHONY: clean run