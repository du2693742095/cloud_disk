SRCS:=$(wildcard *.c)
OBJECTS:=$(patsubst %.c, %.o, $(SRCS))
CC:=gcc
LIBS:= -lpthread
PATH:= -I ../include
BIN:=server

$(BIN):$(OBJECTS)
	$(CC) $^ -o $@ $(LIBS)

%.o:%.c
	$(CC) -c $^ -o $@ $(LIBS) $(PATH)

print:
	echo $(OBJECTS)
clean:
	rm -rf $(OBJECTS)
