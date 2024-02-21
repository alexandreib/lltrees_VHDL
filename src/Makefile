CC=g++
SOURCES=lltree.cpp 
OBJECTS=$(SOURCES:.cpp=.o)
PYLIBPATH=$(shell python3-config --exec-prefix)/lib
PY_CFLAGS=$(shell python3-config --cflags) -fPIC
PY_LDFLAGS=$(shell python3-config --ldflags) -shared -Wl,-rpath,$(PYLIBPATH)

#action:
#	echo argument is $(PY_CFLAGS)

default:lltree.so;

lltree.so:$(OBJECTS)
	$(CC) $(PY_LDFLAGS) $< -o $@ -lpython3.10 -lboost_python310 -lboost_numpy310

$(OBJECTS):$(SOURCES)
	$(CC) $(PY_CFLAGS) -c $< -o $@

clean:
	rm -rf *.so *.o

.PHONY: default clean
