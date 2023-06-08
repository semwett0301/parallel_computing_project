#!/bin/sh
for (( i = 5; i > 0; i-- )); do
  for (( k = 0; k < 10; k++ )); do
    cat $i.txt >> new_$i.txt
    if (( $k != 9 ))
    then
      echo "," >> new_$i.txt
    fi
  done
  rm $i.txt
  mv new_$i.txt $i.txt
#cat $i.txt | sed 's/.$//'
done

