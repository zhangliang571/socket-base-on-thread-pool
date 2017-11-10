
SHELL = /bin/sh
CC = g++
LIBS = -lpthread
AR = ar
RANLIB = ranlib
SRCDIR = ./src
ARFLAGS = crs
CFLAGS += -D _DEBUG
CFLAGS += -g 
INCLUDES += ./include
SERV_BIN := serv
CLIE_BIN := clie 
STATICLIB = libhello.a
SHAREDLIB = libhello.so

#SRCS = $(wildcard $(SRCDIR)/*.c++)
SERV_SRCS += ./src/thread_pool.c++
SERV_SRCS += ./src/serv.c++
CLIE_SRCS += ./src/thread_pool.c++
CLIE_SRCS += ./src/clie.c++
SERV_OBJS = $(patsubst %.c++,%.o,$(SERV_SRCS))
CLIE_OBJS = $(patsubst %.c++,%.o,$(CLIE_SRCS))

.PHONY: all clean help staticlib

all: $(SERV_BIN) $(CLIE_BIN)

# build static lib
staticlib: $(objs) $(STATICLIB)
$(STATICLIB):$(OBJS)
	$(AR) $(ARFLAGS) $@ $^

#build shared lib
sharedlib: $(objs) $(SHAREDLIB)
$(SHAREDLIB):$(OBJS)
	$(CC) -shared -o $@ $^

$(SERV_BIN):$(SERV_OBJS)
	$(CC)  -o $@  $^ $(LIBS)
$(CLIE_BIN):$(CLIE_OBJS)
	$(CC)  -o $@  $^ $(LIBS)

%.o:%.c++ 
	$(CC) $(CFLAGS) -I $(INCLUDES) -c $< -o $@

help:
	@echo ""
	@echo "make all"
	@echo "make clean"
	@echo "make staticlib"
	@echo "make sharedlib"
	@echo ""
clean:
	-rm -rf $(SERV_OBJS) $(CLIE_OBJS) *.a *.so $(SERV_BIN) $(CLIE_BIN) 
