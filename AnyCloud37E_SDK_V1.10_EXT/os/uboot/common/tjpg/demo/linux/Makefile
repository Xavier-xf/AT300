CROSS_COMPILE := 

CC      := $(CROSS_COMPILE)gcc
CXX     := $(CROSS_COMPILE)g++
LD      := $(CROSS_COMPILE)ld
AR      := $(CROSS_COMPILE)ar
RANLIB  := $(CROSS_COMPILE)ranlib
STRIP   := $(CROSS_COMPILE)strip

BIN     = tJpgDec
SOURCES = $(wildcard *.c)
SOURCES += $(wildcard ../../*.c)
HEADERS = $(wildcard *.h)
HEADERS += $(wildcard ../../*.h)
OBJECTS = $(patsubst %.c, %.o, $(SOURCES))

IFLAGS  = -I./ -I../../
CFLAGS  = $(IFLAGS)
LFLAGS  = -lpthread

$(BIN) : $(OBJECTS)
	$(CC) $^ $(LFLAGS) -o $@

%.o:%.c
	$(CC) $(CFLAGS) -c $^ -o $@ 

clean :
	rm -f src/*.o  $(BIN)
