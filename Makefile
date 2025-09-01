UNAME := $(shell uname)
CXX := g++
CXXFLAGS := -std=c++20 -Wall -Werror -Wextra -I./src
LDFLAGS := -lgtest -lgtest_main -lpthread
GCOV_FLAGS := --coverage

PREF_OBJ = ./objects/
SRC := tests.cpp
OBJ := $(PREF_OBJ)tests.o

TARGET := containers

all: test

test: $(TARGET)
	./$(TARGET)

$(TARGET) : $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(TARGET) $(LDFLAGS)

$(OBJ) : $(SRC) | $(PREF_OBJ)
	$(CXX) $(CXXFLAGS) -c $(SRC) -o $(OBJ)

$(PREF_OBJ):
	mkdir -p $(PREF_OBJ)



gcov-build: clean
	$(MAKE) CXXFLAGS="$(CXXFLAGS) $(GCOV_FLAGS)" LDFLAGS="$(LDFLAGS) $(GCOV_FLAGS)" $(TARGET)

gcov: gcov-build
	./$(TARGET)
	lcov --directory . --capture --output-file $(PREF_OBJ)coverage.info \
	     --exclude '*/tests.cpp' --exclude '/opt/homebrew/*' --exclude '/usr/*'
	genhtml $(PREF_OBJ)coverage.info --ignore-errors category --output-directory $(PREF_OBJ)coverage_html
	open $(PREF_OBJ)coverage_html/index.html



leaks: $(TARGET)
	leaks -atExit -- ./$(TARGET)



clean:
	rm -rf $(PREF_OBJ)
	rm -rf $(TARGET)