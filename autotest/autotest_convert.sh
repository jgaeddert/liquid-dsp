#!/bin/sh

path="src/vector/tests/*_autotest.c"

perl -i -p -e 's/Copyright.*Gaeddert/Copyright (c) 2007 - 2026 Joseph Gaeddert/g;' $path
perl -i -p -e 's/"autotest\/autotest.h"/"liquid.autotest.h"/g;' $path
perl -i -p -e 's/void autotest_(.*)\(\)/LIQUID_AUTOTEST(\1,"","",0.1)/g;' $path

perl -i -p -e 's/CONTEND_EXPRESSION/LIQUID_CHECK/g;' $path

perl -i -p -e 's/CONTEND_EQUALITY(.*)\(( *)LIQUID_OK,(.*)\)/LIQUID_CHECK\1(\2LIQUID_OK ==\3)/g;' $path
perl -i -p -e 's/CONTEND_INEQUALITY(.*)\(( *)LIQUID_OK,(.*)\)/LIQUID_CHECK\1(\2LIQUID_OK !=\3)/g;' $path

perl -i -p -e 's/CONTEND_EQUALITY\((.*),(.*)\)/LIQUID_CHECK\(\1 == \2\)/g;' $path
perl -i -p -e 's/CONTEND_INEQUALITY\((.*),(.*)\)/LIQUID_CHECK\(\1 != \2\)/g;' $path

perl -i -p -e 's/CONTEND_DELTA/LIQUID_CHECK_DELTA/g;' $path

perl -i -p -e 's/CONTEND_LESS_THAN\((.*),(.*)\)/LIQUID_CHECK\(\1<\2\)/g;' $path
perl -i -p -e 's/CONTEND_GREATER_THAN\((.*),(.*)\)/LIQUID_CHECK\(\1>\2\)/g;' $path

perl -i -p -e 's/CONTEND_ISNULL\(/LIQUID_CHECK\(NULL ==/g;' $path

perl -i -p -e 's/CONTEND_TRUE/LIQUID_CHECK/g;' $path
perl -i -p -e 's/CONTEND_FALSE( *)\((.*)\)/LIQUID_CHECK\1(!\2)/g;' $path

perl -i -p -e 's/CONTEND_SAME_DATA/LIQUID_CHECK_ARRAY/g;' $path
#perl -i -p -e 's/(liquid_autotest_validate.*)\((.*)/\1\(__q__, \2/g;' $path
#perl -i -p -e 's/.*liquid_autotest_verbose.*(".*").*/        \1);/g;' $path
perl -i -p -e 's/AUTOTEST_FAIL/LIQUID_FAIL/g;' $path
perl -i -p -e 's/AUTOTEST_PASS/LIQUID_PASS/g;' $path

