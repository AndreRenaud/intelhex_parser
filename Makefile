CFLAGS=-std=c99 -Wall -Werror
default: intelhex_parser_test

%.o: %.c intelhex_parser.h
	$(CC) -c -o $@ $< $(CFLAGS)

intelhex_parser_test: intelhex_parser_test.o intelhex_parser.o
	$(CC) -o $@ intelhex_parser.o intelhex_parser_test.o

test: intelhex_parser_test
	./intelhex_parser_test

clean:
	rm *.o intelhex_parser_test

.PHONY: clean test