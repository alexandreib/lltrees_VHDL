export CPLUS_INCLUDE_PATH="$CPLUS_INCLUDE_PATH:/usr/include/python3.10/"

CC=g++
SOURCEDIR = cpp
BUILDDIR = build

TARGETS = $(BUILDDIR)/boosted_lltrees.so 
OBJECTS = $(BUILDDIR)/lltree.o $(BUILDDIR)/boosted_lltrees.o 

PYLIBPATH=$(shell python3-config --exec-prefix)/lib
PY_CFLAGS=$(shell python3-config --cflags) -fPIC
PY_LDFLAGS=$(shell python3-config --ldflags) -shared -Wl,-rpath,$(PYLIBPATH) 

all: build

build: $(TARGETS)

$(TARGETS):$(OBJECTS)
	$(CC) -o $@ $(OBJECTS) $(PY_LDFLAGS) -lpython3.10 -lboost_python310 -lboost_numpy310

$(BUILDDIR)/lltree.o:$(SOURCEDIR)/lltree.cpp
	$(CC) -o $@ -c $< $(PY_CFLAGS)

$(BUILDDIR)/boosted_lltrees.o:$(SOURCEDIR)/boosted_lltrees.cpp 
	$(CC) -o $@ -c $< $(PY_CFLAGS)

clean:
	rm -rf $(BUILDDIR)
	mkdir -p $(BUILDDIR)

print-%  : ; @echo $* = $($*)

.PHONY: default clean

#$(OBJECTS): $(SOURCES)
#	$(CC) -o $@ -c $< $(PY_CFLAGS)
