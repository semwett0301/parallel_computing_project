#!/bin/sh
mpicc -o main main.c -std=c99 -D_SVID_SOURCE

echo "Amount of processes: $1"


for (( k = 10; k > 9; k-- )); do
  S=$(date +%s%N)
  num=$((100000 * $k))
  echo "Amount of values: $num"
  mpiexec -n $1 main 2 /resources/$k/1.txt $num /resources/$k/2.txt $num /resources/$k/3.txt $num /resources/$k/4.txt $num /resources/$k/5.txt $num
  E=$(date +%s%N)
  DIFF=$(($E - $S))
  echo
  echo "Elapsed time (ms): $(($DIFF / 1000000))"
  echo "-----------------------------------------------------------------------------"
done

S=$(date +%s%N)
mpiexec -n $1 main 10 /resources/10/1.txt $num /resources/5/2.txt $num /resources/3/2.txt $num /resources/5/2.txt $num /resources/5/4.txt $num /resources/5/3.txt $num /resources/8/2.txt $num /resources/7/2.txt $num /resources/3/2.txt $num /resources/7/1.txt $num
E=$(date +%s%N)
DIFF=$(($E - $S))
echo
echo "Elapsed time (ms): $(($DIFF / 1000000))"
echo "-----------------------------------------------------------------------------"