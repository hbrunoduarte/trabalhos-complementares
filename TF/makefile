# Variáveis de compilação
CC = gcc
CFLAGS = -Wall
LIBS = -lGLEW -lglfw -lGL -lGLU -lm

# Nomes dos executáveis
TARGET_SOLAR = solar.out

# Regras de Compilação

$(TARGET_SOLAR): solar.c
	$(CC) $(CFLAGS) solar.c vector.c terra.c sol.c $(LIBS) -o $(TARGET_SOLAR)

run: $(TARGET_SOLAR)
	./$(TARGET_SOLAR)

# Limpeza dos arquivos binários
clean:
	rm -f *.out