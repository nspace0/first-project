CC = gcc
CFLAGS = -Wall -Werror -Wextra -std=c11
TARGET = s21_grep
SRC = s21_grep.c

all: $(TARGET) test

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

test:
	bash ./grep.sh
	@echo ""
	clang-format -n *.c
	@echo ""
	#bash ./valgrind_test.sh

clean:
	rm -f $(TARGET)

.PHONY: all test clean
