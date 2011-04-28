#!/bin/bash

echo "Running aclocal"
aclocal
echo "Running autoheader"
autoheader
echo "Running automake"
automake
echo "Running libtoolize"
libtoolize
echo "Running autoconf"
autoconf