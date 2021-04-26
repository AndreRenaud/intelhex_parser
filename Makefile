CPPCHECK?=cppcheck
CLANG_FORMAT?=clang-format
CFLAGS=-std=c99 -Wall -Werror

default: intelhex_parser_test

%.o: %.c intelhex_parser.h
	$(CPPCHECK) --std=c99 --enable=style,warning,performance,portability --quiet $<
	$(CC) -c -o $@ $< $(CFLAGS)

intelhex_parser_test: intelhex_parser_test.o intelhex_parser.o
	$(CC) -o $@ intelhex_parser.o intelhex_parser_test.o

test: intelhex_parser_test
	./intelhex_parser_test

format:
	$(CLANG_FORMAT) -i intelhex_parser.c intelhex_parser.h intelhex_parser_test.c

clean:
	rm *.o intelhex_parser_test

.PHONY: clean test format