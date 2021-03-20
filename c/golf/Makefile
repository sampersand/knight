CC=gcc
ifdef DEBUG
override CFLAGS+=-g -fsanitize=address,undefined
else
CFLAGS=-O3 -DNDEBUG -march=native
endif

.PHONY: clean

knight: knight.o
	$(CC) $(CFLAGS) knight.o -o knight

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	-@rm *.o
	-@rm knight