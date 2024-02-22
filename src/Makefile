export CPLUS_INCLUDE_PATH="$CPLUS_INCLUDE_PATH:/usr/include/python3.10/"

CC=g++
SOURCEDIR = cpp
BUILDDIR = build

SOURCES = $(wildcard $(addsuffix /*.cpp, $(SOURCEDIR))) #$(wildcard $(SOURCEDIR)/*.cpp)
OBJECTS := $(patsubst %.cpp, %.o, $(SOURCES))

#OBJECTS = $(patsubst $(SOURCEDIR)/%.cpp, $(BUILDDIR)/%.o,$(SOURCES))

PYLIBPATH=$(shell python3-config --exec-prefix)/lib
PY_CFLAGS=$(shell python3-config --cflags) -fPIC
PY_LDFLAGS=$(shell python3-config --ldflags) -shared -Wl,-rpath,$(PYLIBPATH) 

default:$(BUILDDIR)/lltree.so;

$(BUILDDIR)/lltree.so:$(OBJECTS)
	$(CC) $(PY_LDFLAGS) $< -o $@ -lpython3.10 -lboost_python310 -lboost_numpy310

$(OBJECTS):$(SOURCES)
	$(CC) $(PY_CFLAGS) -c $< -o $@

dir:
	mkdir -p $(BUILDDIR)

clean:
	rm -rf $(BUILDDIR)/*.so $(BUILDDIR)/*.o

.PHONY: default clean dir
