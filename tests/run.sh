#!/bin/bash
# Usage: tests/run.sh
shell=./build/shell
pass=0
fail=0

for infile in tests/cases/*.in; do
  name=$(basename "$infile" .in)
  expected="tests/cases/$name.out"
  actual=$(timeout 5 "$shell" < "$infile" 2> /dev/null)
  if [ "$actual" == "$(cat "$expected")" ]; then
    echo "PASS  $name"
    pass=$((pass + 1))
  else
    echo "FAIL  $name"
    diff <(echo "$actual") "$expected" | head -10
    fail=$((fail + 1))
  fi
done

echo "$pass passed, $fail failed"
[ "$fail" -eq 0 ]
