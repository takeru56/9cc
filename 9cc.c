#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "引数の個数が正しくありません\n");
    return 1;
  }

  char *p = argv[1];

  printf(".intel_syntax noprefix\n.global main\n");
  printf("main:\n");
  printf(" mov rax, %ld\n", strtol(p, &p, 10));

  while(*p) {
    if (*p == '+'){
      p++;
      printf(" add rax, %ld\n", strtol(p, &p, 10));
      continue;
    }

    if (*p == '-'){
      p++;
      printf(" sub rax, %ld\n", strtol(p, &p, 10));
      continue;
    }

    fprintf(stderr, "予期しない文字です: '%c'\n", *p);
    return 1;
  }

  printf(" ret\n");
  return 0;

}
