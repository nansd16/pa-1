#!/bin/bash

echo -e "\nRunning make clean"
make -f makefile clean
echo -e "\nRunning make prog1"
make
rc=$?
total=0
if [[ $rc == 0 ]]; then
  tests=( "rrotate 7F400001 2"
          "rrotate 7F400001 4"
          "rrotate 7F400010 5"
          "rrotate 7F400010 10"
          "rrotate 7F401000 16"
          "lrotate 7F400001 2"
          "lrotate 7F400001 4"
          "lrotate 7F400010 5"
          "lrotate 7F400010 10"
          "lrotate 7F401000 16"
          "saturate 7fffffff 00000001"
          "saturate a000ff0f e0ff00ff"
          "saturate 000000ff 0000ff00"
          "saturate 7f000000 8fff0000"
          "saturate 84000000 a500ffff"
          "saturate ffffffff 00000001"
          "usaturate 7fffffff 00000010"
          "usaturate ffffffff 00000001"
          "usaturate 00000100 8000ffff"
          "usaturate 8fff0000 8fff0000"
          "twice 3f400000"
          "twice 007fffff"
          "twice 7f000000"
          "twice ff800000"
          "twice ff800777"
          "half 3fc00000"
          "half 00E00000"
          "half ff800000"
          "half 7f800777"
          "rrotate 200gffff 2"
        )

  for test in "${tests[@]}"; do
    total=$((total + 1))
    echo -e "\nTest $total with args: $test"
    ./prog1 $test | tee -a tests.out
  done
paste -d '|' tests.out tests.reference | grep -Po '^(.*)\|+\K\1$' > out
count=0
input="./out"
while IFS= read -r line
do
  count=$((count + 1))
done < "$input"

if [[ $count == 30 ]]; then
   echo -e "\nAll tests passed"
   echo -e "Score: 75/75\n"
   exit 0
else
   score=$((count * 2 + count / 2))
   echo -e "\n$count out of 32 passed"
   echo -e "Score: $score/75\n"
   exit 1
fi

fi
