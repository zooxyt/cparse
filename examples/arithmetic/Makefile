CC = gcc
CFLAGS = -Wall -Wextra -g
INCLUDES = -I/usr/include/cparse/
LDFLAGS = -L/usr/lib/ -lcparse
RM = rm -rf
SOURCES = $(wildcard *.c)
TARGET = test
CP = cp
RM = rm

default:
	$(CC) $(CFLAGS) $(INCLUDES) $(SOURCES) -o $(TARGET) $(LDFLAGS) 

clean:
	$(RM) $(TARGET)

install:
	$(CP) $(TARGET) /usr/bin/

uninstall:
	$(RM) /usr/bin/$(TARGET)

