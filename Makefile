CC=gcc
CFLAGS=-Wall -Werror -g -I.
LFLAGS=-lGLEW -lGL -lglfw -lm
TARGET=main

sources = $(wildcard *.c)
objects = $(sources:.c=.o)

$(TARGET): $(objects)
	$(CC) -o $@ $^ $(LFLAGS)

.PHONY: clean
clean:
	rm -f $(objects) $(TARGET)
