#!/bin/sh
valgrind --tool=memcheck --leak-check=full --show-reachable=yes $1
