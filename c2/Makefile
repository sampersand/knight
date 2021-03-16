SRCDIR?=src
OBJDIR?=obj
EXE?=knight

CC=gcc
CFLAGS+=-Wall -Wextra -Wpedantic
override CFLAGS+=-F$(SRCDIR)

ifdef DEBUG
override CFLAGS+=-g -fsanitize=address,undefined
else
CEXEFLAGS+=-O3 -DNDEBUG -flto -march=native
endif

ifdef COMPUTED_GOTOS
override CFLAGS+=-DKN_COMPUTED_GOTOS -Wno-gnu-label-as-value -Wno-gnu-designator
endif

objects = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(wildcard $(SRCDIR)/*.c))

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
