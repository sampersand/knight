SRCDIR ?= src
OBJDIR ?= obj
EXE ?= knight

CC = gcc 
CFLAGS = -Wall -Wextra -Werror -Wpedantic -Wno-gnu-label-as-value
override CFLAGS += -F$(SRCDIR)

ifdef DEBUG
override CFLAGS += -g -fsanitize=address,undefined -O1
else
CEXEFLAGS += -Wl,-stack_size,1600000 -O3 -DNDEBUG
endif

# lower case because it's local to makefile.
objects = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(wildcard $(SRCDIR)/*.c))

all: $(EXE)

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
