BINDIR=bin
BINARY=$(BINDIR)/FTM-6000_Readout
CODEDIRS=./src
INCDIRS=I./inc
LIBS=
OBJDIR=obj
MKDIR=mkdir
CXX=g++
OPT=-O2
# generate files that encode make rules for .h dependencies
DEPFLAGS=-MP -MD
# automatically add the -I onto each include directory
CPPFLAGS=-Wall -Wextra -DNDEBUG -g $(foreach D,$(INCDIRS),-$(D)) $(OPT) $(DEPFLAGS)

# for-style iteration (foreach) and regular expression completions (wildcard)
CPPFILES=$(foreach D,$(CODEDIRS),$(wildcard $(D)/*.cpp))
#
# regular expression replacement
OBJECTS=$(patsubst ./src/%.cpp,./$(OBJDIR)/%.o,$(CPPFILES))
DEPFILES=$(patsubst ./%.cpp,./$(OBJDIR)%.d,$(CPPFILES))

DIRS := $(BINDIR) $(OBJDIR)

all: $(DIRS) $(BINARY)

debug: CPPFLAGS= -O0 -Wmain -g -ansi -std=c++14 -pedantic -Wall $(foreach D,$(INCDIRS),-$(D)) -fsanitize=address -fno-omit-frame-pointer -lasan
debug: LIBS=-lasan
debug: $(BINARY)

$(BINARY): $(OBJECTS)
	$(CXX) -o $@ $^ $(LIBS)
	
# only want the .c files dependency here, thus $< instead of $^.
./$(OBJDIR)/%.o:./$(CODEDIRS)/%.cpp
	$(CXX) $(CPPFLAGS) -c -o $@ $<
	
$(DIRS):
	$(MKDIR) -p $@
clean:
	rm -rf $(BINARY) $(OBJDIR)/*

