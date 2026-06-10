# Variáveis de compilação
CC = gcc
CFLAGS = -Wall
LIBS = -lGLEW -lglfw -lGL -lGLU -lm

# Encontra magicamente TODOS os arquivos .c no diretório atual
SRCS = $(wildcard *.c)

# Nome do executável
TARGET_SOLAR = solar.out

# Regras de Compilação
$(TARGET_SOLAR): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) $(LIBS) -o $(TARGET_SOLAR)

run: $(TARGET_SOLAR)
	./$(TARGET_SOLAR)

# Limpeza dos arquivos binários
clean:
	rm -f *.out