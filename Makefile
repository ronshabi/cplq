CXX=g++
CXXFLAGS=-std=c++20 -g -O1 -Wall -Wextra -Wformat

TARGET=cplq
OBJECTS=Cpq.o Ast.o Types.o Utils.o

BISON=bison
BISON_FLAGS=-d
BISON_INPUT=Grammar.y
BISON_CPP_OUTPUT=$(BISON_INPUT:.y=.cpp)
BISON_HPP_OUTPUT=$(BISON_INPUT:.y=.hpp)
BISON_OBJECTS=$(BISON_INPUT:.y=.o)

FLEX=flex
FLEX_FLAGS=
FLEX_INPUT=Lexer.l
FLEX_CPP_OUTPUT=$(FLEX_INPUT:.l=.cpp)
FLEX_OBJECTS=$(FLEX_INPUT:.l=.o)

CLANG_FORMAT=clang-format
CLANG_FORMAT_FLAGS=-i --verbose

.PHONY: all
all: $(TARGET)

.PHONY: fmt
fmt:
	find . -name '*.[ch]pp' -not -name 'Grammar.*' -exec $(CLANG_FORMAT) $(CLANG_FORMAT_FLAGS) {} +

$(TARGET): $(BISON_OBJECTS) $(FLEX_OBJECTS) $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(BISON_CPP_OUTPUT) $(BISON_HPP_OUTPUT): $(BISON_INPUT)
	$(BISON) $(BISON_FLAGS) -o $(BISON_CPP_OUTPUT) $<

$(FLEX_CPP_OUTPUT): $(FLEX_INPUT)
	$(FLEX) $(FLEX_FLAGS) -o $@ $<

$(BISON_OBJECTS): $(BISON_CPP_OUTPUT) $(BISON_HPP_OUTPUT)
	$(CXX) -g -c $< -o $@

$(FLEX_OBJECTS): $(FLEX_CPP_OUTPUT)
	$(CXX) -g -c $< -o $@

%.o: %.cpp %.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f $(FLEX_CPP_OUTPUT) $(FLEX_OBJECTS)
	rm -f $(BISON_CPP_OUTPUT) $(BISON_HPP_OUTPUT) $(BISON_OBJECTS)
	rm -f *.o
	rm -f $(TARGET)