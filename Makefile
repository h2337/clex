# Makefile for clex - A simple lexer generator for C
CC = gcc
CFLAGS = -Wall -Wextra -O2
DEBUG_FLAGS = -g -O0 -DDEBUG
TEST_FLAGS =

# Source files
SOURCES = clex.c fa.c
HEADERS = clex.h fa.h
OBJECTS = $(SOURCES:.c=.o)

# Test configurations
TEST_CLEX = -DTEST_CLEX
TEST_REGEX = -DTEST_REGEX
TEST_NFA_DRAW = -DTEST_NFA_DRAW

# Default target
.PHONY: all
all: help

# Help target
.PHONY: help
help:
	@echo "Available targets:"
	@echo "  make test-all    - Run all tests"
	@echo "  make test-clex   - Run clex tests"
	@echo "  make test-regex  - Run regex tests"
	@echo "  make test-nfa    - Run NFA drawing test"
	@echo "  make example     - Build the example from README"
	@echo "  make lib         - Build object files for library use"
	@echo "  make clean       - Remove all build artifacts"
	@echo "  make check       - Run all tests and verify no output"

# Build object files for library use
.PHONY: lib
lib: $(OBJECTS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Test targets
.PHONY: test-all
test-all: test-clex test-regex test-nfa
	@echo "All tests completed!"

.PHONY: test-clex
test-clex: $(SOURCES) $(HEADERS) tests.c
	@echo "Running clex tests..."
	@$(CC) $(TEST_FLAGS) $(TEST_CLEX) tests.c $(SOURCES) -o test_clex
	@./test_clex && echo "✓ Clex tests passed" || (echo "✗ Clex tests failed" && exit 1)
	@rm -f test_clex

.PHONY: test-regex
test-regex: $(SOURCES) $(HEADERS) tests.c
	@echo "Running regex tests..."
	@$(CC) $(TEST_FLAGS) $(TEST_REGEX) tests.c $(SOURCES) -o test_regex
	@./test_regex && echo "✓ Regex tests passed" || (echo "✗ Regex tests failed" && exit 1)
	@rm -f test_regex

.PHONY: test-nfa
test-nfa: $(SOURCES) $(HEADERS) tests.c
	@echo "Running NFA drawing test..."
	@$(CC) $(TEST_FLAGS) $(TEST_NFA_DRAW) tests.c $(SOURCES) -o test_nfa
	@echo "Generating NFA graphs..."
	@./test_nfa > nfa_output.dot
	@echo "✓ NFA drawing test completed (output in nfa_output.dot)"
	@rm -f test_nfa

# Quick check - run all tests and ensure they pass silently
.PHONY: check
check:
	@$(CC) $(TEST_FLAGS) $(TEST_CLEX) tests.c $(SOURCES) -o test_clex 2>/dev/null
	@$(CC) $(TEST_FLAGS) $(TEST_REGEX) tests.c $(SOURCES) -o test_regex 2>/dev/null
	@./test_clex && ./test_regex && echo "All tests passed!" || (echo "Tests failed!" && exit 1)
	@rm -f test_clex test_regex

# Build example from README
.PHONY: example
example: example.c $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) example.c $(SOURCES) -o example
	@echo "Example built successfully! Run ./example to test"

# Create example.c from README if it doesn't exist
example.c:
	@echo "Creating example.c from README..."
	@echo '#include "clex.h"' > example.c
	@echo '#include <assert.h>' >> example.c
	@echo '#include <string.h>' >> example.c
	@echo '' >> example.c
	@sed -n '/^typedef enum TokenKind {/,/^}/p' README.md >> example.c
	@echo '' >> example.c
	@sed -n '/^int main() {/,/^}/p' README.md >> example.c

# Debug builds
.PHONY: debug
debug: CFLAGS = $(DEBUG_FLAGS)
debug: lib

# Clean target
.PHONY: clean
clean:
	rm -f $(OBJECTS)
	rm -f test_clex test_regex test_nfa
	rm -f example example.c
	rm -f nfa_output.dot
	rm -f *.o
	rm -f a.out

# Install target (for system-wide installation)
PREFIX ?= /usr/local
INSTALL_DIR = $(PREFIX)/include/clex
.PHONY: install
install: $(HEADERS)
	@echo "Installing headers to $(INSTALL_DIR)..."
	@mkdir -p $(INSTALL_DIR)
	@cp $(HEADERS) $(INSTALL_DIR)/
	@echo "Installation complete!"

.PHONY: uninstall
uninstall:
	@echo "Removing headers from $(INSTALL_DIR)..."
	@rm -rf $(INSTALL_DIR)
	@echo "Uninstallation complete!"
