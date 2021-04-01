SRCDIR?=src
OBJDIR?=obj
EXE?=knight
CXX=g++

CXXFLAGS+=-Wall -Wextra -Wpedantic -std=c++17
override CXXFLAGS+=-F$(SRCDIR)

ifdef DEBUG
override CXXFLAGS+=-g -fsanitize=address,undefined
else
override	CXXFLAGS+=-O2
endif

ifdef OPTIMIZED
override CXXFLAGS+=-O3 -DNDEBUG -flto -march=native -fno-stack-protector
endif

objects=$(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(wildcard $(SRCDIR)/*.cpp))

.PHONY: all optimized clean

all: $(EXE)

optimized:
	$(CXX) $(CXXFLAGS) -o $(EXE) $(wildcard $(SRCDIR)/*.cpp)

$(EXE): $(objects)
	$(CXX) $(CXXFLAGS) -o $@ $+

clean:
	-@rm -r $(OBJDIR)
	-@rm $(EXE)

$(OBJDIR):
	@mkdir -p $(OBJDIR)

$(objects): | $(OBJDIR)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
