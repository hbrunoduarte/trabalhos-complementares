# Variáveis de compilação
CC = gcc
CFLAGS = -Wall
LIBS = -lglut -lGL -lGLU -lm

# Nomes dos executáveis
TARGET_LIVRO = livro.out

# Regras de Compilação

$(TARGET_LIVRO): livro.c
	$(CC) $(CFLAGS) livro.c $(LIBS) -o $(TARGET_LIVRO)

run_livro: $(TARGET_LIVRO)
	./$(TARGET_LIVRO)

# Limpeza dos arquivos binários
clean:
	rm -f *.out
