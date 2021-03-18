SRCDIR ?= src
OBJDIR ?= obj
EXE ?= knight

CC = gcc 
CFLAGS = -Wall -Wextra -Wpedantic -flto -march=native -Wno-gnu
CFLAGS += $(CER)
override CFLAGS += -F$(SRCDIR)

ifdef DEBUG
override CFLAGS += -g -fsanitize=address,undefined
else
CEXEFLAGS += -O3 -DNDEBUG
endif

# ifdef COMPUTED_GOTOS
# override CFLAGS += -DCOMPUTED_GOTOS -Wno-gnu-label-as-value -Wgnu-designator
# endif

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
