#!/bin/sh

try() {
  expected="$1"
  input="$2"

  ./micc "$input" > tmp.s
  gcc -o tmp tmp.s
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$input => $expected expected, but got $actual" 
  fi
}

try 21 '5+20-4'

echo OK
