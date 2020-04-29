#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

//=========================
//                      Tokenizer
//=========================
typedef enum {
  TK_RESERVED,
  TK_IDENT,
  TK_NUM,
  TK_EOF,
  TK_RETURN,
} TokenKind;

typedef struct Token {
  TokenKind kind;
  struct Token *next;
  int val;
  char *str;
  int len;
} Token;

// ローカル変数の型
typedef struct LVar {
  struct LVar *next; // 次の変数かNULL
  char *name; // 変数の名前
  int len;    // 名前の長さ
  int offset; // RBPからのオフセット
} LVar;

void error_at(char *loc, char *fmt, ...);
Token *tokenize(char *p);

//=========================
//                         Parser
//=========================
typedef enum {
  ND_ASSIGN,
  ND_ADD, // +
  ND_SUB, // -
  ND_MUL, // *
  ND_DIV, // /
  ND_NUM, // int
  ND_LVAR, // lacal variable
  ND_LT, // <
  ND_LE, // <=
  ND_EQ, // ==
  ND_NE // !=
} Nodekind;

typedef struct Node {
  Nodekind kind;
  struct Node *lhs;
  struct Node *rhs;
  int val;
  int offset; // 差分
} Node;

void program();
extern Node *code[100];
//=========================
//                       Codegen
//=========================
void gen(Node *node);

//=========================
//                       Main
//=========================
extern Token *token;
