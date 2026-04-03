# simple makefile to test linking to liquid after library has been installed

all: check

check: liquid_linker_test
	./liquid_linker_test

liquid_linker_test: % : scripts/%.c
	$(CC) -Wall -O2 -o $@ $< -lm -lc -lliquid

# same as above but using pkg-config
check-pkg-config: liquid_linker_test_pkgconfig
	./liquid_linker_test_pkgconfig

liquid_linker_test_pkgconfig : scripts/liquid_linker_test.c
	$(CC) -Wall -O2 -o $@ $< `pkg-config --cflags --libs liquid-dsp`

clean:
	rm -f liquid_linker_test liquid_linker_test_pkgconfig

