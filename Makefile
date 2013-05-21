CC = gcc

BGMERGE = -DBGMERGE
DEBUG =	-g -ggdb -DINFO

#detect OS,support Linux and Mac OS
UNAME := $(shell uname)
ifeq ($(UNAME), Linux)
	CFLAGS =-c -std=c99 -W -Wall -Werror -fPIC $(DEBUG) $(BGMERGE)
	LDFLAGS=-fPIC -shared
	LIB_EXTENSION=so
endif
ifeq ($(UNAME), Darwin)
	CFLAGS =-c -std=c99 -W -Wall -Werror $(DEBUG) $(BGMERGE)
	LDFLAGS=-std=c99 -W -Wall -Werror -dynamiclib -flat_namespace
	LIB_EXTENSION=dylib
endif




LIB_OBJS = \
	./src/xmalloc.o\
	./src/debug.o\
	./src/sst.o\
	./src/common.o\
	./src/buffer.o\
	./src/data.o\
	./src/PMurHash.o\
	./src/sstmanager.o

TEST = \
	./test/db-test.o


EXE = \
	./test\

LIBRARY = libsstable.$(LIB_EXTENSION)

all: $(LIBRARY)

clean:
	-rm -f $(LIBRARY)  
	-rm -f $(LIB_OBJS)
	-rm -f $(EXE)
	-rm -f $(TEST)
	cd test;make clean

cleandb:
	-rm -rf *.data
	-rm -rf *.event

$(LIBRARY): $(LIB_OBJS)
	$(CC) -pthread $(LDFLAGS) $(LIB_OBJS) -o libsstable.$(LIB_EXTENSION) -lm

db-test:  test/test.o $(LIB_OBJS)
	$(CC) -pthread $(LIB_OBJS) $(DEBUG) test/test.o -o $@ -lm
test: all $(LIB_OBJS)
	cd test;make
	export LD_LIBRARY_PATH=. && ./test/test
