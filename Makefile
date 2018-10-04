GCC=gcc
GCCFLAGS=-Wall -std=c99 -pedantic

TARGET=run
SOURCES=$(wildcard *.c)
OBJECTS=$(SOURCES:%.c=$(OUTPUT)/%.o)
OUTPUT=build

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(GCC) $(GCCFLAGS) -o $(OUTPUT)/$(TARGET) $(OBJECTS)

$(OUTPUT)/%.o: %.c
	$(GCC) $(GCCFLAGS) -I./include -c $< -o $@

clean:
	rm -rf build/*

.PHONY: all clean
