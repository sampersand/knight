CC=gcc
override CFLAGS+=-Wall -Wpedantic -Wextra -Werror
ifdef DEBUG
override CFLAGS+=-g -fsanitize=address,undefined
else
CFLAGS=-O1
endif

.PHONY: clean

knight: knight.o
	$(CC) $(CFLAGS) knight.o -o knight

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	-@rm *.o
	-@rm knight