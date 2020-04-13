#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
  TK_RESERVED,
  TK_NUM,
  TK_EOF,
} TokenKind;

// 初期化時にstructの宣言を不要にする
typedef struct Token Token;

// トークンの型
struct Token {
  TokenKind kind;
  Token *next;
  int val;
  char *str;
};

//現在着目しているトークン
Token *token;

// エラー
void error(char *fmt, ...) {
  // 引数リスト
  va_list ap;
  // 可変長引数リストの初期化
  // 引数リストapにfmt以降の引数を格納
  va_start(ap, fmt);
  //vfprintf(出力先ストリーム, 変換していを含んだ文字列, 引数リスト)
  // 任意のストリームに，変換指定に沿って変換された文字列を吐き出す
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

// 次のトークンが期待している記号の時には読み進めtrueを返す
// それ以外はfalseを返す
bool consume(char op) {
  if (token->kind != TK_RESERVED || token->str[0] != op)
    return false;
  token = token->next;
  return true;
}

// 次のトークンが期待している記号のときには読み進める
// それ以外のときはエラーを返す
void expect(char op) {
  if (token->kind != TK_RESERVED || token->str[0] != op)
    error("'%c'ではありません", op);
  token = token->next;
}

int expect_number() {
  if (token->kind != TK_NUM)
    error("数ではありません");
  int val = token->val;
  token = token->next;
  return val;
}

// トークンが末尾かを判定
bool at_eof() {
  return token->kind == TK_EOF;
}

Token *new_token(TokenKind kind, Token *cur, char *str) {
  // ヒープメモリからsizeof(Token)バイトのブロックを１つ割り当てる
  // 確保したメモリを示すポインタを返す
  Token *tok = calloc(1, sizeof(Token));
  // printf("これ%d", *str);
  tok->kind = kind;
  tok->str = str;
  cur->next = tok;
  return tok;
}

// 入力文字pをトークナイズしてそれを返す
Token *tokenize(char *p) {
  Token head;
  head.next = NULL;
  Token *cur = &head;

  while (*p) {
    if (isspace(*p)) {
      p++;
      continue;
    }

    if (*p == '+' || *p == '-') {
      cur = new_token(TK_RESERVED, cur, p++);
      continue;
    }

    if (isdigit(*p)) {
      cur = new_token(TK_NUM, cur, p);
      cur->val = strtol(p, &p, 10);
      continue;
    }
    error("トークナイズできません");
  }
  new_token(TK_EOF, cur, p);
  return head.next;
}

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "引数の個数が正しくありません\n");
    return 1;
  }
  token = tokenize(argv[1]);

  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");

  printf("  mov rax, %d\n", expect_number());

  while (!at_eof()) {
    if (consume('+')) {
      printf("  add rax, %d\n", expect_number());
      continue;
    }
    expect('-');
    printf("  sub rax, %d\n", expect_number());
  }
  printf("  ret\n");
  return 0;
}
