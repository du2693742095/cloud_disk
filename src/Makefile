SRCS:=$(wildcard *.c)
OBJS:=$(patsubst %.c, %.o, $(SRCS))
cc:=gcc
LIBS:= -lpthread -lcrypt
PATH:= ../include
BIN:=server

$(BIN):$(OBJECTS)
	$(CC) $^ -o $@ $(LIBS)

%.o:%.c
	$(CC) -c $^ -o $@ $(LIBS) -I $(PATH)

.PHONY: BIN clean cleanALL

cleanALL:
	rm -rf $(BOJS) $(BIN)
clean:
	rm -rf $(BIN)

