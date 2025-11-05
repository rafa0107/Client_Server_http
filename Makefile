# Nome dos binários
CLIENT = meu_navegador
SERVER = meu_servidor

# Compilador e flags
CC = gcc
CFLAGS = -Wall -Wextra -O2

# Arquivos fonte
CLIENT_SRC = client.c
SERVER_SRC = server.c

all: $(CLIENT) $(SERVER)

$(CLIENT): $(CLIENT_SRC)
	$(CC) $(CFLAGS) -o $(CLIENT) $(CLIENT_SRC)

$(SERVER): $(SERVER_SRC)
	$(CC) $(CFLAGS) -o $(SERVER) $(SERVER_SRC)

clean:
	rm -f $(CLIENT) $(SERVER)

