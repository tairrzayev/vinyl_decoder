PROJECT=vinyldc
CC=gcc
CFLAGS=-c -Wall -Isrc -g
LDFLAGS=-lSDL2 -lSDL2_image -lm
SOURCES=src/main.c src/wav.c src/vinyl.c src/point.c src/surface.c
OBJECTS=$(SOURCES:.c=.o)
RM=rm -f

all: $(PROJECT)

$(PROJECT): $(OBJECTS) 
	$(CC) $^ $(LDFLAGS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	$(RM) $(OBJECTS) $(PROJECT)
