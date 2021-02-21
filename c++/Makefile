SRCDIR ?= src
OBJDIR ?= obj
EXE ?= knight

CXX = g++
CEXEFLAGS =
CXXFLAGS = -Wall -Wextra -Werror -Wpedantic -std=c++17
override CXXFLAGS += -F$(SRCDIR)

ifdef DEBUG
override CXXFLAGS += -g -fsanitize=address
else
CEXEFLAGS += -Wl,-stack_size,1600000 -O2 -DNDEBUG
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
