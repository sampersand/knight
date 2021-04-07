SRCDIR?=src
OBJDIR?=obj
EXE?=knight

CC?=gcc
CFLAGS+=-Wall -Wextra -Wpedantic -std=c11
override CFLAGS+=-F$(SRCDIR)

ifdef DEBUG
override CFLAGS+=-g -fsanitize=address,undefined
else
override CFLAGS+=-O3 -flto -march=native -DNDEBUG
endif

ifdef COMPUTED_GOTOS
override CFLAGS+=-DKN_COMPUTED_GOTOS -Wno-gnu-label-as-value -Wno-gnu-designator
endif

objects = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(wildcard $(SRCDIR)/*.c))

.PHONY: all optimized clean

all: $(EXE)

optimized:
	$(CC) $(CFLAGS) $(CEXEFLAGS) -o $(EXE) $(wildcard $(SRCDIR)/*.c)

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
