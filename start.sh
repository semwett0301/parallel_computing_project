#!/bin/sh
mpicc -o main main.c -std=c99 -D_SVID_SOURCE
mpiexec -n 5 main 3 ./resources/dataset_1.txt 57 ./resources/dataset_2.txt 50 ./resources/dataset_1.txt 50