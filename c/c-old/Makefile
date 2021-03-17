SRCDIR ?= src
OBJDIR ?= obj
EXE ?= knight

CC = gcc 
CFLAGS = -Wall -Wextra -Werror -Wpedantic -O3 -DNDEBUG -flto -march=native
override CFLAGS += -F$(SRCDIR)

ifdef DEBUG
override CFLAGS += -g -fsanitize=address,undefined
else
CEXEFLAGS += -O3 -DNDEBUG -flto -march=native
endif

objects = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(wildcard $(SRCDIR)/*.c))

all: $(EXE)

optimized:
	gcc $(CFLAGS) $(CEXEFLAGS) -o $(EXE) $(wildcard $(SRCDIR)/*.c)

$(EXE): $(objects)
	$(CC) $(CFLAGS) $(CEXEFLAGS) -o $@ $+

clean:
	-@rm -r $(OBJDIR)
	-@rm $(EXE)

$(OBJDIR):
	@mkdir -p $(OBJDIR)

$(objects): | $(OBJDIR)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@
