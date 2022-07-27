SRCS:=$(wildcard *.c)
OBJECTS:=$(patsubst %.c, %.o, $(SRCS))
CC:=gcc
LIBS:= -lpthread
PATHS:= -I ../include
BIN:=../../bin/client

$(BIN):$(OBJECTS)
	$(CC) $^ -o $@ $(LIBS)

%.o:%.c
	$(CC) -c $^ -o $@ $(LIBS) $(PATHS)

print:
	echo $(OBJECTS)
clean:
	rm -rf $(OBJECTS)
