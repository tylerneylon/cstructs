# Variables for targets.
tests = $(addprefix out/,arraytest listtest cmaptest cmapunsettest)
obj = $(addprefix out/,CArray.o CList.o CMap.o memprofile.o ctest.o)

# Variables for build settings.
includes = -Isrc
cflags = $(includes)
cc = clang $(cflags)

# Primary rules; meant to be used directly.
all: $(obj) $(tests)

# Indirect rules; meant to only be used to complete a primary rule.
test-build: $(tests)

## (temp) Here's a suggestion for how to add a test rule:
## test:
##	for test in $(TESTS); do bash test-runner.sh $$test || exit 1; done
##
## Also, on max os x, I can run tests like this to help ensure a crash
## on any bad memory access:
## DYLD_INSERT_LIBRARIES=/usr/lib/libgmalloc.dylib ./out/arraytest
##
## Actually, I like this better as it suppresses some potentially-annoying
## boilerplate text from guard malloc:
## DYLD_INSERT_LIBRARIES=/usr/lib/libgmalloc.dylib MALLOC_LOG_FILE=/dev/null ./out/arraytest
##

out:
	mkdir -p out

out/ctest.o: test/ctest.c test/ctest.h | out
	$(cc) -o out/ctest.o -c test/ctest.c

out/%.o : src/%.c src/%.h | out
	$(cc) -o $@ -c $<

out/% : test/%.c $(obj)
	$(cc) -o $@ $^

clean:
	rm -rf out

# Listing this special-name rule prevents the deletion of intermediate files.
.SECONDARY:

