#export CPLUS_INCLUDE_PATH="$CPLUS_INCLUDE_PATH:/usr/include/python3.10/"

CC=g++
SOURCEDIR = cpp
BUILDDIR = build

SOURCES = $(wildcard $(SOURCEDIR)/*.cpp)
OBJECTS = $(patsubst $(SOURCEDIR)/%.cpp,$(BUILDDIR)/%.o,$(SOURCES))
TARGETS = $(BUILDDIR)/lltrees.so 

BOOST = -DBOOST_BIND_GLOBAL_PLACEHOLDERS -DBOOST_ALLOW_DEPRECATED_HEADERS
PYLIBPATH=$(shell python3-config --exec-prefix)/lib
PY_CFLAGS=$(shell python3-config --cflags) -fPIC -c
PY_LDFLAGS=$(shell python3-config --ldflags) -shared -Wl,-rpath,$(PYLIBPATH) 

all: build

build: $(TARGETS)

$(TARGETS) : $(OBJECTS)
	$(CC) -o $@ $^ $(PY_LDFLAGS) -lpython3.10 -lboost_python310 -lboost_numpy310

$(BUILDDIR)/%.o: $(SOURCEDIR)/%.cpp
	$(CC) -o $@ $^ $(PY_CFLAGS) -std=c++20 $(BOOST)

clean:
	rm -rf $(BUILDDIR)
	mkdir -p $(BUILDDIR)

print-%  : ; @echo $* = $($*)

.PHONY: default clean
