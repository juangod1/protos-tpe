GCC=gcc
GCCFLAGS=-Wall -std=c99 -pedantic

TARGET=run
SOURCES=$(wildcard *.c)
OBJECTS=$(SOURCES:%.c=$(OUTPUT)/%.o)
OUTPUT=build

DIRECTORY=build/

all: | $(DIRECTORY) $(TARGET)

$(DIRECTORY):
	@echo "Folder $(directory) does not exist, one will be created."
	mkdir $@;

$(TARGET): $(OBJECTS)
	$(GCC) $(GCCFLAGS) -o $(OUTPUT)/$(TARGET) $(OBJECTS)

$(OUTPUT)/%.o: %.c
	$(GCC) $(GCCFLAGS) -I./include -c $< -o $@

clean:
	rm -rf build/*

.PHONY: all clean
