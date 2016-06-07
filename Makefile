CC=gcc
CFLAGS=-g -I.
LFLAGS=-lGL -lglfw -lm
TARGET=main

sources = $(wildcard *.c)
objects = $(sources:.c=.o)

$(TARGET): $(objects)
	$(CC) -o $@ $^ $(LFLAGS)

.PHONY: clean
clean:
	rm -f $(objects) $(TARGET)
