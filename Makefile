CXX=g++
CXXFLAGS=-std=c++20 -g -O1 -Wall -Wextra -Wformat

TARGET=cplq
OBJECTS=cpq.o ast.o types.o

BISON=bison
BISON_FLAGS=-d
BISON_INPUT=grammar.y
BISON_CXX_OUTPUT=$(BISON_INPUT:.y=.cxx)
BISON_HXX_OUTPUT=$(BISON_INPUT:.y=.hxx)
BISON_OBJECTS=$(BISON_INPUT:.y=.o)

FLEX=flex
FLEX_FLAGS=
FLEX_INPUT=lexer.l
FLEX_CXX_OUTPUT=$(FLEX_INPUT:.l=.cxx)
FLEX_OBJECTS=$(FLEX_INPUT:.l=.o)

CLANG_FORMAT=clang-format
CLANG_FORMAT_FLAGS=-i --verbose

.PHONY: all
all: $(TARGET)


$(TARGET): $(BISON_OBJECTS) $(FLEX_OBJECTS) $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(BISON_CXX_OUTPUT) $(BISON_HXX_OUTPUT): $(BISON_INPUT)
	$(BISON) $(BISON_FLAGS) -o $(BISON_CXX_OUTPUT) $<

$(FLEX_CXX_OUTPUT): $(FLEX_INPUT)
	$(FLEX) $(FLEX_FLAGS) -o $@ $<

$(BISON_OBJECTS): $(BISON_CXX_OUTPUT) $(BISON_HXX_OUTPUT)
	$(CXX) -c $< -o $@

$(FLEX_OBJECTS): $(FLEX_CXX_OUTPUT)
	$(CXX) -c $< -o $@

%.o: %.cxx %.hxx
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: %.cxx
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f $(FLEX_CXX_OUTPUT) $(FLEX_OBJECTS)
	rm -f $(BISON_CXX_OUTPUT) $(BISON_HXX_OUTPUT) $(BISON_OBJECTS)
	rm -f $(OBJECTS)
