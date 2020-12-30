SRCDIR ?= src
OBJDIR ?= obj
EXE ?= knight

CC = gcc
CEXEFLAGS = ""
CFLAGS = -Wall -Wextra -Werror -Wpedantic -Wno-gnu-zero-variadic-macro-arguments
override CFLAGS += -F$(SRCDIR)

ifdef (DEBUG)
CFLAGS += -g
else
CEXEFLAGS + =-Wl,-stack_size,1600000000 -O2
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

$(OBJDIR)/%.o: $(srcdir)/%.c
	$(CC) $(CFLAGS) -c $< -o $@
