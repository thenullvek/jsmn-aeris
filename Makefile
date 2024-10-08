# You can put your build options here
-include config.mk

ifdef CI_BUILD_MACOS
LLVM_PATH="/opt/homebrew/opt/llvm/bin"
CC=$(LLVM_PATH)/clang
LLVM_PROFDATA=$(LLVM_PATH)/llvm-profdata
LLVM_COV=$(LLVM_PATH)/llvm-cov
else
CC=clang
LLVM_PROFDATA=llvm-profdata
LLVM_COV=llvm-cov
endif

CFLAGS+=-Wall

TEST_LINKS_TARGET=tests/coverage-test-links
TEST_DEF_TARGET=tests/coverage-test-default

test: test_default test_links 
test_default: tests/tests.c jsmn2.c
	$(CC) -g -DJSMN_TESTMODE $(CFLAGS) $(LDFLAGS) $^ -o tests/$@
	./tests/$@
test_links: tests/tests.c jsmn2.c
	$(CC) -g -DJSMN_TESTMODE -DJSMN_PARENT_LINKS=1 $(CFLAGS) $(LDFLAGS) $^ -o tests/$@
	./tests/$@

simple_example: example/simple.c jsmn2.c
	$(CC) $(LDFLAGS) $^ -o $@

jsondump: example/jsondump.c jsmn2.c
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

fmt:
	clang-format -i jsmn2.h tests/*.[ch] example/*.[ch]

lint:
	clang-tidy jsmn2.h --checks='*'

coverage: links.info default.info
$(TEST_LINKS_TARGET): tests/tests.c jsmn2.c
	$(CC) -DJSMN_TESTMODE -DJSMN_PARENT_LINKS=1 -O0 -g $(CFLAGS) \
		-fprofile-instr-generate \
		-fcoverage-mapping  $(LDFLAGS) $^ -o $@

$(TEST_DEF_TARGET): tests/tests.c jsmn2.c
	$(CC) -DJSMN_TESTMODE -O0 -g $(CFLAGS) \
		-fprofile-instr-generate \
		-fcoverage-mapping  $(LDFLAGS) $^ -o $@

jsmn-links.profraw: tests/coverage-test-links
	LLVM_PROFILE_FILE="$@" $<

jsmn-default.profraw: tests/coverage-test-default
	LLVM_PROFILE_FILE="$@" $<

links.info: jsmn-links.profraw
	$(LLVM_PROFDATA) merge $^ -o jsmn-links.profdata
	$(LLVM_COV) export -ignore-filename-regex "tests/.*" \
		--instr-profile jsmn-links.profdata --format lcov \
		-object tests/coverage-test-links > $@
	rm -f jsmn-links.profdata

default.info: jsmn-default.profraw
	$(LLVM_PROFDATA) merge $^ -o jsmn-default.profdata
	$(LLVM_COV) export -ignore-filename-regex "tests/.*" \
		--instr-profile jsmn-default.profdata --format lcov \
		-object tests/coverage-test-default > $@
	rm -f jsmn-default.profdata

clean:
	rm -f *.o example/*.o
	rm -f tests/test_default tests/test_links
	rm -f simple_example
	rm -f jsondump
	rm -rf *.dSYM
	rm -rf tests/*.dSYM
	rm -rf tests/coverage-*
	rm -f *.prof* *.info
	rm -f $(TEST_DEF_TARGET) $(TEST_LINKS_TARGET)

.PHONY: clean test coverage

