#!/bin/bash
DIR=$(basedir $@)
TESTS=./build/test/test*.bin
for i in $TESTS; do
    echo "Executing $i tests..."
    "./$i"
done
