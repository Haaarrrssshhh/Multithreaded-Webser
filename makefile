CC = gcc
CFLAGS = -Wall -pthread -g
SOURCES = src/main.c src/server.c
EXECUTABLE = bin/server

all: $(EXECUTABLE)

$(EXECUTABLE): $(SOURCES)
	@mkdir -p bin
	$(CC) $(CFLAGS) -o $(EXECUTABLE) $(SOURCES)

clean:
	rm -f $(EXECUTABLE)
