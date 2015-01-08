CC=gcc
CFLAGS=-c -Wall -lSDL2  -lSDL2_image -lm -g
LDFLAGS=-lSDL2  -lSDL2_image -lm -g
SOURCES=main.c wav.c vinyl.c point.c surface.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=vinyldc

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC)  $(OBJECTS) $(LDFLAGS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)
