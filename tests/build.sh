#!/bin/sh

cd $(dirname $0)

gcc test.c   ../CxxLogLib.c -o ctest.out
g++ test.cpp ../CxxLogLib.c -o cpptest.out
