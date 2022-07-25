SRCS:=$(wildcard *.c)
OBJS:=$(patsubst %.c, %.o, $(SRCS))
ELF:=server
CC:=gcc
CFLAGS:=-g -Wall
$(ELF):$(OBJS)
	gcc $^ -o $@ -pthread -lcrypt -I ../include

%.o:%.c
	gcc $^ -o $@ -pthread -lcrypt -I ../include

clean:
		rm -rf  $(ELF)

