CC = g++
PYLIBPATH = $(shell python3-config --exec-prefix)/lib
LIB = $(shell python3-config --ldflags) -shared -Wl,-rpath,$(PYLIBPATH)
OPTS = $(shell python3-config --cflags) -fPIC

default: zoo.so
	@python3 ./zoo.py

zoo.so: zoo.o
	$(CC) $(LIB) $< -o $@ -lpython3.10 -lboost_python -lboost_numpy

zoo.o: zoo.cpp Makefile
	$(CC) $(OPTS) -c $< -o $@

clean:
	rm -rf *.so *.o

.PHONY: default clean