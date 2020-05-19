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
    printf "${ESC}[32m%s${ESC}[m\n" "[PASS] $input => $actual"
  else
    printf "${ESC}[31m%s${ESC}[m\n" "[FAIL] $input => $expected expected, but got $actual"
  fi
}

echo "# TESTCASES"
echo "## Step4"
assert 18 "10 +8 ;"
assert 21 "5 + 20-4;"
assert 41 " 12 + 34 - 5 ;"
echo "## Step5"
assert 12 "4+4*2;"
assert 20 "((44+6)*2)/5;"
assert 47 '5+6*7;'
assert 15 '5*(9-6);'
assert 4 '(3+5)/2;'
echo "## Step6;"
assert 12 '-4+16;'
assert 4  '-((8+8)/2) + 12;'
echo "## Step7"
assert 1 '1 == 1;'
assert 0 '1 == 0;'

assert 1 '1 != 0;'
assert 0 '1 != 1;'

assert 1 '0 < 1;'
assert 0 '0 < 0;'
assert 0 '1 < 0;'

assert 1 '1 > 0;'
assert 0 '0 > 0;'
assert 0 '0 > 1;'

assert 1 '1 >= 1;'
assert 1 '1 >= 0;'
assert 0 '0 >= 1;'

assert 1 '1 <= 1;'
assert 1 '0 <= 1;'
assert 0 '1 <= 0;'
echo "## Step8"
assert 1 'hoge=1;'
assert 0 'a=1; b=1; c=a-b;'
assert 2 'a=2; b=1; a;'
assert 1 'hoge = 3; fuga = 2; hoge-fuga;'
echo "## Step11"
assert 1 'return 1;'
assert 2 ' return 1+1;'
assert 1 'hoge=3; fuga=2; return 3-2;'
echo "## Step12"
assert 2 'hoge=1; if (hoge>0)  hoge = 2;  return hoge;'
assert 1 'hoge=1; if (hoge<0)  hoge = 2;  return hoge;'

echo OK
