# Variáveis de compilação
CC = gcc
CFLAGS = -Wall
LIBS = -lglut -lGL -lGLU -lm

# Nomes dos executáveis
TARGET_CASTLE = castle.out
TARGET_SNOWMAN = snowman.out

# Comando padrão: compila tudo
all: $(TARGET_CASTLE) $(TARGET_ROBOT) $(TARGET_SNOWMAN)

# Regras de Compilação
$(TARGET_CASTLE): castle.c
	$(CC) $(CFLAGS) castle.c $(LIBS) -o $(TARGET_CASTLE)

$(TARGET_SNOWMAN): snowman.c
	$(CC) $(CFLAGS) snowman.c $(LIBS) -o $(TARGET_SNOWMAN)

# Comandos para Executar cada um individualmente
run_castle: $(TARGET_CASTLE)
	./$(TARGET_CASTLE)

run_snowman: $(TARGET_SNOWMAN)
	./$(TARGET_SNOWMAN)

# Limpeza dos arquivos binários
clean:
	rm -f *.out