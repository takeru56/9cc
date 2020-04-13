#!/bin/sh

assert() {
  expected="$1"
  input="$2"

  ./9cc "$input" > tmp.s
  gcc -o tmp tmp.s
  ./tmp
  actual="$?"

  ESC=$(printf '\033')
  if [ "$actual" = "$expected" ]; then
    printf "${ESC}[32m%s${ESC}[m\n" "- $input => $actual"
  else
    printf "${ESC}[31m%s${ESC}[m\n" "☓ $input => $expected expected, but got $actual" 
  fi
}

assert 18 "10 +8 "
assert 21 "5 + 20-4"
assert 41 " 12 + 34 - 5 "

echo OK
