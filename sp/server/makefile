CC = gcc
CFLAGS = -lpthread -g

BIN = server.exe
OBJ = server.o 

$(BIN): $(OBJ)
		$(CC) $(OBJ) -o $(BIN) $(CFLAGS)

server.o: server.c
		gcc -c server.c $(CFLAGS)