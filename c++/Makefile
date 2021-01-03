SRCDIR ?= src
OBJDIR ?= obj
EXE ?= knight

CXX = g++
CEXEFLAGS =
CXXFLAGS = -Wall -Wextra -Werror -Wpedantic -std=c++17
override CFLAGS += -F$(SRCDIR)

ifdef DEBUG
CFLAGS += -g
else
CEXEFLAGS += -Wl,-stack_size,1600000 -O2
endif

# lower case because it's local to makefile.
objects = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(wildcard $(SRCDIR)/*.cpp))

# bar: foo
# 		echo $(DEBUG)
all: $(EXE)

$(EXE): $(objects)
	$(CXX) $(CXXFLAGS) $(CEXEFLAGS) -o $@ $+

clean:
	-@rm -r $(OBJDIR)
	-@rm $(EXE)

$(OBJDIR):
	@mkdir -p $(OBJDIR)

$(objects): | $(OBJDIR)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CXX) $(CXXFLAGS) -c $< -o $@
