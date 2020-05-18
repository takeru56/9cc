#include <ctype.h>
#include <stdarg.h>
#include "9cc.h"

char *user_input;

// エラー
void error_at(char *loc, char *fmt, ...) {
  // 引数リスト
  va_list ap;
  // 可変長引数リストの初期化
  // 引数リストapにfmt以降の引数を格納
  va_start(ap, fmt);

  int pos = loc - user_input;
  fprintf(stderr, "%s\n", user_input);
  // pos個の空白文字を出力する
  fprintf(stderr, "%*s", pos, "");
  fprintf(stderr, "^ ");

  //vfprintf(出力先ストリーム, 変換していを含んだ文字列, 引数リスト)
  // 任意のストリームに，変換指定に沿って変換された文字列を吐き出す
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

bool is_alnum(char c) {
  return  ('a' <= c && c <= 'z') ||
    ('A' <= c && c <= 'Z') ||
    ('0' <= c && c <= '9') ||
    (c == '_');
}


Token *new_token(TokenKind kind, Token *cur, char *str, int len) {
  // ヒープメモリからsizeof(Token)バイトのブロックを１つ割り当てる
  // 確保したメモリを示すポインタを返す
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->str = str;
  tok->len = len;
  cur->next = tok;
  return tok;
}

Token *tokenize(char *p) {
  Token head;
  head.next = NULL;
  Token *cur = &head;
  user_input = p;

  while (*p) {
    if (isspace(*p)) {
      p++;
      continue;
    }

    if (memcmp(p, "==", 2) == 0 || memcmp(p, "!=", 2) == 0 ) {
      cur = new_token(TK_RESERVED, cur, p, 2);
      p+=2;
      continue;
    }

     if (memcmp(p, "<=", 2) == 0 || memcmp(p, ">=", 2) == 0 ) {
      cur = new_token(TK_RESERVED, cur, p, 2);
      p+=2;
      continue;
    }

    if (strchr("+-*/()<>=;", *p)) {
      cur = new_token(TK_RESERVED, cur, p++, 1);
      continue;
    }

    if (strncmp(p, "return", 6) == 0 && !is_alnum(p[6])) {
      cur = new_token(TK_RETURN, cur, p, 6);
      p+= 6;
      continue;
    }

    // 一文字以上のアルファベットを変数とみなす
    if (islower(*p)) {
      int l = 0;
      char *top = p;
      while (islower(*p)) {
        p++;
        l++;
      }
      cur = new_token(TK_IDENT, cur, top, l);
      continue;
    }

    if (isdigit(*p)) {
      cur = new_token(TK_NUM, cur, p, 1);
      char *q = p;
      cur->val = strtol(p, &p, 10);
      cur->len = p - q;
      continue;
    }
    error_at(p, "トークナイズできません\n");
  }
  new_token(TK_EOF, cur, p, 0);
  return head.next;
}
