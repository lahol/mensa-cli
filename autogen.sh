#!/bin/bash

automake --add-missing --copy
autoreconf --install

#echo "Running aclocal"
#aclocal
#echo "Running autoheader"
#autoheader
#echo "Running automake"
#automake
#echo "Running libtoolize"
#libtoolize
#echo "Running autoconf"
#autoconf