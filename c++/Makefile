SRCDIR ?= src
OBJDIR ?= obj
EXE ?= knight

CXX = g++
CEXEFLAGS =
CXXFLAGS = -std=c++17 -Wall -Wextra -Werror -Wpedantic -Wno-infinite-recursion
override CXXFLAGS += -F$(SRCDIR)

ifdef DEBUG
override CXXFLAGS += -g -fsanitize=address,undefined
else
CEXEFLAGS += -Wl,-stack_size,160000000 -O3 -DNDEBUG
endif

# lower case because it's local to makefile.
objects = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(wildcard $(SRCDIR)/*.cpp))

all: $(EXE)

$(EXE): $(objects)
	$(CXX) $(CXXFLAGS) $(CEXEFLAGS) -o $@ $+

clean:
	-@rm -r $(OBJDIR)
	-@rm $(EXE)

$(OBJDIR):
	@mkdir -p $(OBJDIR)

$(objects): | $(OBJDIR)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
