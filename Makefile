CPPCHECK?=cppcheck
CLANG_FORMAT?=clang-format
CFLAGS=-std=c99 -Wall -Werror
# On MacOS we need the brew install version
CLANG?=/usr/local/opt/llvm/bin/clang

default: intelhex_parser_test

%.o: %.c intelhex_parser.h
	$(CPPCHECK) --std=c99 --enable=style,warning,performance,portability --quiet $<
	$(CC) -c -o $@ $< $(CFLAGS)

intelhex_parser_test: intelhex_parser_test.o intelhex_parser.o
	$(CC) -o $@ intelhex_parser.o intelhex_parser_test.o

intelhex_parser_fuzz: intelhex_parser.c intelhex_parser_fuzz.c
	$(CLANG) -I. -g -o $@ intelhex_parser_fuzz.c -fsanitize=fuzzer,address

fuzz: intelhex_parser_fuzz
	mkdir -p fuzz-artifacts
	mkdir -p fuzz-corpus
	./intelhex_parser_fuzz -verbosity=0 -max_total_time=120 -max_len=8192 -rss_limit_mb=1024 -artifact_prefix="./fuzz-artifacts/" fuzz-corpus

test: intelhex_parser_test
	./intelhex_parser_test

format:
	$(CLANG_FORMAT) -i intelhex_parser.c intelhex_parser.h intelhex_parser_test.c intelhex_parser_fuzz.c

clean:
	rm -f *.o intelhex_parser_test intelhex_parser_fuzz
	rm -rf fuzz-artifacts

.PHONY: clean test format fuzz default