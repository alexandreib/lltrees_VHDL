export CPLUS_INCLUDE_PATH="$CPLUS_INCLUDE_PATH:/usr/include/python3.10/"

CC=g++
SOURCEDIR = cpp
BUILDDIR = build

TARGETS = $(BUILDDIR)/lltrees.so 
OBJECTS = $(BUILDDIR)/lltrees.o 
SOURCES = $(SOURCEDIR)/lltrees.cpp
DEPENDS = $(SOURCEDIR)/*.hpp #$(patsubst %.cpp,%.d,$(SOURCES))

PYLIBPATH=$(shell python3-config --exec-prefix)/lib
PY_CFLAGS=$(shell python3-config --cflags) -fPIC -c
PY_LDFLAGS=$(shell python3-config --ldflags) -shared -Wl,-rpath,$(PYLIBPATH) 

all: build

build: $(TARGETS)

$(TARGETS):$(OBJECTS)
	$(CC) -o $@ $(OBJECTS) $(PY_LDFLAGS) -lpython3.10 -lboost_python310 -lboost_numpy310

$(OBJECTS): $(SOURCES) $(DEPENDS)
	$(CC) -o $@ -c $< $(PY_CFLAGS)

clean:
	rm -rf $(BUILDDIR)
	mkdir -p $(BUILDDIR)

print-%  : ; @echo $* = $($*)

.PHONY: default clean
