#!/bin/sh
mpicc -o main main.c -std=c99 -D_SVID_SOURCE

echo "Amount of processes: $1"

for (( k = 10; k > 0; k-- )); do
  num=$((1000000 * $k))
  echo "Amount of millions: $num"
  mpiexec -n $1 main 5 /resources/$k/1.txt $num /resources/$k/2.txt $num /resources/$k/3.txt $num /resources/$k/4.txt $num /resources/$k/5.txt $num
  echo "-----------------------------------------------------------------------------"
done