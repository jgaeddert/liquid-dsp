// auto-generated file, do not edit
//
// created with autotest v0.1.0

#ifndef __AUTOTEST_INCLUDE_H__
#define __AUTOTEST_INCLUDE_H__

// header files from which this file was generated:
#include "autotest_example.h"

// number of autotests
#define NUM_AUTOTESTS 2

// array of benchmarks
struct autotest_s autotests[NUM_AUTOTESTS] = {
    {0,&autotest_example_01,"example_01",0,0,0,0},
    {1,&autotest_example_02,"example_02",0,0,0,0}
};

// number of packages
#define NUM_PACKAGES 1

// array of packages
struct package_s packages[NUM_PACKAGES] = {
    {0,0,2,"autotest_example"}
};
#endif // __BENCHINCLUDE_H__
