all: Proxy Control

Proxy:
	cd Proxy; make all

Control:
	cd Control; make all

clean:
	cd Proxy; make clean
	cd Control; make clean

.PHONY: Proxy Control all clean
