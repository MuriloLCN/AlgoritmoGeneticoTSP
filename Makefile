# Nome do compilador
CC = cc

# Flags de compilação
# CFLAGS = -Wall -O2
CFLAGS = 

# Tenta usar -pthread, senão tenta -lpthread
# Você pode forçar manualmente caso necessário
PTHREAD_FLAGS = -pthread

# Bibliotecas extras
LIBS = $(PTHREAD_FLAGS) -lm

# Arquivos fonte
SRCS = main-final.c leitura_arquivo.c

# Arquivo de cabeçalho
HEADERS = leitura_arquivo.h

# Nome do executável
TARGET = tsp

# Regra padrão
all: $(TARGET)

# Como gerar o executável
$(TARGET): $(SRCS) $(HEADERS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET) $(LIBS)

# Limpeza de arquivos gerados
clean:
	rm -f $(TARGET) *.o

.PHONY: all clean