.PHONY: all clean install uninstall

VPATH = ./:./
# vpath %.h ../headers
TARGET = testKVDB
PREFIX = /usr/local/bin
CC = g++
CFLAGS = -c -Wall -std=c++17 -MD
LIBS = -lzmq
SOURCE = $(wildcard *.cpp *.c)
HEADERS = $(wildcard *.h)
OBJECTS = $(patsubst %.cpp,%.o,$(SOURCE))

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@$(CC) $^ -o $(TARGET) $(LIBS)

%.o: %.cpp $(HEADERS)
	@echo "["$(CC)"] "$^" -> "$@
	@$(CC) $(CFLAGS) $<

clean:
	@rm -rf *.o *.d $(TARGET)

install: $(TARGET)
	@install $(TARGET) $(PREFIX)

uninstall:
	@rm -rf $(PREFIX)/$(TARGET)