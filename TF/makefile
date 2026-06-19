# Variáveis de compilação
CC = gcc
CFLAGS = -w -Wall
LIBS = -lGLEW -lglfw -lGL -lGLU -lcglm -lm

SRCS = $(wildcard *.c)

# Nome do executável
TARGET_SOLAR = main.out

run: $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) $(LIBS) -o $(TARGET_SOLAR) && ./$(TARGET_SOLAR)

# Limpeza dos arquivos binários
clean:
	rm -f *.out