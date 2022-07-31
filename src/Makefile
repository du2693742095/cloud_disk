SRCS:=$(wildcard *.c)
OBJECTS:=$(patsubst %.c, %.o, $(SRCS))
CC:=gcc
LIBS:= -lpthread -lcrypt -lmysqlclient
PATHS:= -I ../include
BIN:= ../bin/server

$(BIN):$(OBJECTS)
	$(CC) $^ -o $@ $(LIBS)

%.o:%.c
	$(CC) -c $^ -o $@ $(LIBS) $(PATHS)

print:
	echo $(OBJECTS)
clean:
	rm -rf $(OBJECTS)
