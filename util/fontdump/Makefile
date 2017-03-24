CC = gcc
CFLAGS = `freetype-config --cflags` -Wall
LDFLAGS = `freetype-config --libs` -lpng
TARGET = dumpfont

all:
	$(CC) $(CFLAGS) -o $(TARGET) dumpfont.c $(LDFLAGS)

clean:
	rm $(TARGET)
