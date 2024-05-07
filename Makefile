CC = gcc
CFLAGS = -g -Wall -lpthread
SRC = ep2.c
DEPS = ep2.h
EXEC = ep2

$(EXEC): $(SRC) $(DEPS)
	$(CC) -o $@ $(SRC) $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(EXEC)