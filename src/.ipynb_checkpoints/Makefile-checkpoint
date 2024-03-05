export CPLUS_INCLUDE_PATH="$CPLUS_INCLUDE_PATH:/usr/include/python3.10/"

CC=g++
SOURCEDIR = cpp
BUILDDIR = build

TARGETS = $(BUILDDIR)/lltrees.so 
OBJECTS = $(BUILDDIR)/lltrees.o $(BUILDDIR)/wrapper.o $(BUILDDIR)/gbt.o $(BUILDDIR)/tree.o $(BUILDDIR)/criterion.o
#$(BUILDDIR)/*.o  #$(BUILDDIR)/lltrees.o $(BUILDDIR)/wrapper.o $(BUILDDIR)/gbt.o 
#SOURCES = $(SOURCEDIR)/*.cpp #$(SOURCEDIR)/lltrees.cpp
#DEPENDS = $(SOURCEDIR)/*.hpp
BOOST = -DBOOST_BIND_GLOBAL_PLACEHOLDERS -DBOOST_ALLOW_DEPRECATED_HEADERS
PYLIBPATH=$(shell python3-config --exec-prefix)/lib
PY_CFLAGS=$(shell python3-config --cflags) -fPIC -c
PY_LDFLAGS=$(shell python3-config --ldflags) -shared -Wl,-rpath,$(PYLIBPATH) 

all: build

build: $(TARGETS)

$(TARGETS) : $(OBJECTS)
	$(CC) -o $@ $^ $(PY_LDFLAGS) -lpython3.10 -lboost_python310 -lboost_numpy310

$(BUILDDIR)/gbt.o : $(SOURCEDIR)/gbt.cpp
	$(CC) -o $@ $^ $(PY_CFLAGS) -std=c++20 $(BOOST)

$(BUILDDIR)/wrapper.o : $(SOURCEDIR)/wrapper.cpp
	$(CC) -o $@ $^ $(PY_CFLAGS) -std=c++20 $(BOOST)

$(BUILDDIR)/lltrees.o : $(SOURCEDIR)/lltrees.cpp
	$(CC) -o $@ $^ $(PY_CFLAGS) -std=c++20 $(BOOST)

$(BUILDDIR)/tree.o : $(SOURCEDIR)/tree.cpp
	$(CC) -o $@ $^ $(PY_CFLAGS) -std=c++20 $(BOOST)

$(BUILDDIR)/criterion.o : $(SOURCEDIR)/criterion.cpp
	$(CC) -o $@ $^ $(PY_CFLAGS) -std=c++20 $(BOOST)

$(BUILDDIR)/factories.o : $(SOURCEDIR)/factories.cpp
	$(CC) -o $@ $^ $(PY_CFLAGS) -std=c++20 $(BOOST)

clean:
	rm -rf $(BUILDDIR)
	mkdir -p $(BUILDDIR)

print-%  : ; @echo $* = $($*)

.PHONY: default clean
	

#$(CC) -o $@ $^ $(PY_LDFLAGS) -lpython3.10 -lboost_python310 -lboost_numpy310
#$(OBJECTS): $(SOURCES)
#	$(CC) -o $@ $< $(PY_CFLAGS)

#$(BUILDDIR)/criterion.o : $(SOURCEDIR)/criterion.cpp
#	$(CC) -o $@ $< $(PY_CFLAGS)


#$(OBJECTS) : $(SOURCES)
#	$(CC) -o $@ $< $(PY_CFLAGS) -std=c++20
