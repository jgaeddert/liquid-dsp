# Makefile for liquid-dsp pre-compiled tables and constants

all:

CFLAGS  := -I include -g -O2 -Wall
LDFLAGS :=
LIBS    := -lm -lc -lliquid

.PHONY: gentab
gentab_programs =						\
	gentab/count_ones_gentab				\
	gentab/qtype_constants_gentab				\
	gentab/qtype_cordic_gentab				\
	gentab/qtype_hyperbolic_cordic_gentab			\
	gentab/qtype_logexp_frac_gentab				\
	gentab/qtype_logexp_shiftadd_gentab			\
	gentab/qtype_trig_gentab				\
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
	# utility
	./gentab/count_ones_gentab > src/utility/src/count_ones.c
	# fixed-point math
	./gentab/qtype_constants_gentab         -n 16 > src/fpm/src/q16_constants.c
	./gentab/qtype_constants_gentab         -n 32 > src/fpm/src/q32_constants.c
	./gentab/qtype_cordic_gentab            -n 16 > src/fpm/src/q16_cordic.c
	./gentab/qtype_cordic_gentab            -n 32 > src/fpm/src/q32_cordic.c
	./gentab/qtype_hyperbolic_cordic_gentab -n 16 > src/fpm/src/q16_hyperbolic_cordic.c
	./gentab/qtype_hyperbolic_cordic_gentab -n 32 > src/fpm/src/q32_hyperbolic_cordic.c
	./gentab/qtype_logexp_frac_gentab       -n 16 > src/fpm/src/q16_logexp_frac.c
	./gentab/qtype_logexp_frac_gentab       -n 32 > src/fpm/src/q32_logexp_frac.c
	./gentab/qtype_logexp_shiftadd_gentab   -n 16 > src/fpm/src/q16_logexp_shiftadd.c
	./gentab/qtype_logexp_shiftadd_gentab   -n 32 > src/fpm/src/q32_logexp_shiftadd.c
	./gentab/qtype_trig_gentab              -n 16 > src/fpm/src/q16_trigtab.c
	./gentab/qtype_trig_gentab              -n 32 > src/fpm/src/q32_trigtab.c

all: run

