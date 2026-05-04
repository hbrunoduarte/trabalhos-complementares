# Variáveis de compilação
CC = gcc
CFLAGS = -Wall
LIBS = -lglut -lGL -lGLU -lm

# Nomes dos executáveis
TARGET_ROBOT = robot.out

# Regras de Compilação

$(TARGET_ROBOT): robot.c
	$(CC) $(CFLAGS) robot.c $(LIBS) -o $(TARGET_ROBOT)

run_robot: $(TARGET_ROBOT)
	./$(TARGET_ROBOT)

# Limpeza dos arquivos binários
clean:
	rm -f *.out