CC = gcc
CFLAGS = -Wall -Wextra -pthread

TARGET = findeq
SOURCES = findeq.c
OBJECTS = $(SOURCES:.c=.o)

default: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)
