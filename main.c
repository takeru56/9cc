#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "9cc.h"  

Token *token;
int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "引数の個数が正しくありません\n");
    return 1;
  }
  // トークナイズ
  token = tokenize(argv[1]);
  // パース
  Node *node = expr();

  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");

  // コード生成
  gen(node);

  printf("  pop rax\n");
  printf("  ret\n");
  return 0;
}
