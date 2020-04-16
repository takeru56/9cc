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

typedef enum {
  ND_ADD, // +
  ND_SUB, // -
  ND_MUL, // *
  ND_DIV, // /
  ND_NUM, // int
} Nodekind;

typedef struct Node Node;

struct Node {
  Nodekind kind;
  Node *lhs;
  Node *rhs;
  int val;
};

// 演算子ノードの作成
Node *new_node(Nodekind kind, Node *lhs, Node *rhs) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

Node *new_node_num(int val) {
  Node *node = calloc(1,sizeof(Node));
  node->kind = ND_NUM;
  node->val = val;
  return node;
}

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
    error_at(token->str, "'%c'ではありません", op);
  token = token->next;
}

int expect_number() {
  if (token->kind != TK_NUM)
    error_at(token->str, "数ではありません");
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
  user_input = p;

  while (*p) {
    if (isspace(*p)) {
      p++;
      continue;
    }

    if (strchr("+-*/()", *p)) {
      cur = new_token(TK_RESERVED, cur, p++);
      continue;
    }

    if (isdigit(*p)) {
      cur = new_token(TK_NUM, cur, p);
      cur->val = strtol(p, &p, 10);
      continue;
    }
    error_at(p, "トークナイズできません");
  }
  new_token(TK_EOF, cur, p);
  return head.next;
}

Node *expr();
Node *mul();
Node *unary();
Node *primary();
// 再帰下降構文解析法 LL(1)
// 下記３つの非終端記号を関数にマップする
// 1. expr    = mul("+" mul | "-" mul)*
// 2. mul     = primary("*" primary | "/" primary)*
// 3. unary   = ("+" | "-")?primary
// 4. primary = num | "(" expr ")"

Node *expr() {
  Node *node = mul();

  for (;;) {
    if (consume('+'))
      node = new_node(ND_ADD, node, mul());
    else if (consume('-'))
      node = new_node(ND_SUB, node, mul());
    else
      return node;
  }
}

Node *mul() {
  Node *node =unary();

  for (;;) {
    if (consume('*'))
      node = new_node(ND_MUL, node,unary());
    else if (consume('/'))
      node = new_node(ND_DIV, node,unary()) ;
    else
      return node;
  }
}

Node *unary() {
  if (consume('+'))
    return primary();
  if (consume('-'))
    // -x を　0-xの式に置き換える
    return new_node(ND_SUB, new_node_num(0), primary());
  return primary();
}

Node *primary() {
  if (consume('(')) {
    Node *node = expr();
    expect(')');
    return node;
  }
  return new_node_num(expect_number());
}

void gen(Node *node) {
  if (node->kind == ND_NUM) {
    printf("  push %d\n", node->val);
    return;
  }

  gen(node->lhs);
  gen(node->rhs);

  printf("  pop rdi\n");
  printf("  pop rax\n");

  switch (node->kind) {
    case ND_ADD:
      printf("  add rax, rdi\n");
      break;
    case ND_SUB:
      printf("  sub rax, rdi\n");
      break;
    case ND_MUL:
      printf("  imul rax, rdi\n");
      break;
    case ND_DIV:
      printf("  cqo\n");
      // 符号あり除算
      // raxの値をrdiで割り，商をrax，余りをedxに入れる
      // rax=5, rdi=3, idiv rbx => rax=1, rbx=3, rdx=2
      printf("  idiv rdi\n");
      break;
  }

  printf("  push rax\n");
}

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
