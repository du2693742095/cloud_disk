SRCS:=$(wildcard *.c)
OBJS:=$(patsubst %.c, %.o, $(SRCS))
CC:=gcc
LIBS:= -lpthread
PATH:= include/
BIN:=server

$(BIN):$(OBJS)
	$(CC) $^ -o $@ $(LIBS)

%.o:%.c
	$(CC) -c $^ -o $@ $(LIBS) -I $(PATH)

cleanALL:
	rm -rf $(BOJS) $(BIN)
clean:
	rm -rf $(BIN)

