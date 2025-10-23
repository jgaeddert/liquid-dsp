# Makefile for liquid-dsp pre-compiled tables and constants

all:

CFLAGS  := -I include -g -O2 -Wall
LDFLAGS :=
LIBS    := -lm -lc -lliquid

.PHONY: gentab
gentab_programs =						\
	gentab/count_ones_gentab				\
	gentab/firfilt_gentab					\

gentab_objects	= $(patsubst %,%.o,$(gentab_programs))
gentab: $(gentab_programs)

# NOTE: linked libraries must come _after_ the target program
$(gentab_objects): %.o : %.c

$(gentab_programs): % : %.o ${ARCHIVE_LIB}
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS) $(LIBS)

# clean gentab binaries
clean:
	$(RM) gentab/*.o
	$(RM) $(gentab_programs)

run: $(gentab_programs)
	./gentab/count_ones_gentab > src/utility/src/count_ones.c

all: run


