// #include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "9cc.h" 

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

// 次のトークンが期待している記号の時には読み進めtrueを返す
// それ以外はfalseを返す
bool consume(char *op) {
  if (token->kind != TK_RESERVED || 
      strlen(op) != token->len ||
      memcmp(token->str, op, token->len))
    return false;
  token = token->next;
  return true;
}

// 次のトークンが期待している記号のときには読み進める
// それ以外のときはエラーを返す
void expect(char *op) {
 if (token->kind != TK_RESERVED || 
      strlen(op) != token->len ||
      memcmp(token->str, op, token->len))
    error_at(token->str, "'%s'ではありません", op);
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

Node *expr();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();
// 再帰下降構文解析法 LL(1)
//  expr          = equality
//  equality     = relational ("==" relational | "!=" relational)
//  relational   = add ("<" add | "<=" add | ">" add | ">=" add)*
//  add           = mul("+" mul | "-" mul)*
//  mul           = primary("*" primary | "/" primary)*
//  unary        = ("+" | "-")?primary
//  primary     = num | "(" expr ")"

Node *expr() {
  Node *node = equality();
  return node;
} 

Node *equality() {
  Node *node = relational();

  for (;;) {
    if (consume("==")){
      node = new_node(ND_EQ, node, relational());  
    } else if (consume("!=")) {
      node = new_node(ND_NE, node, relational());
    } else {
      return node;
    }
  }
}

Node *relational() {
  Node *node = add();

   for (;;) {
    if (consume("<")){
      node = new_node(ND_LT, node, add());  
    } else if (consume(">")) {
      node = new_node(ND_LT, add(), node);
    } else if (consume("<=")) {
      node = new_node(ND_LE, node, add());
    } else if (consume(">=")) {
      node = new_node(ND_LE, add(), node);
    } else {
      return node;
    }
  }
}

Node *add() {
  Node *node = mul();

  for (;;) {
    if (consume("+"))
      node = new_node(ND_ADD, node, mul());
    else if (consume("-"))
      node = new_node(ND_SUB, node, mul());
    else
      return node;
  }
}

Node *mul() {
  Node *node =unary();

  for (;;) {
    if (consume("*"))
      node = new_node(ND_MUL, node,unary());
    else if (consume("/"))
      node = new_node(ND_DIV, node,unary());
    else
      return node;
  }
}

Node *unary() {
  if (consume("+"))
    return primary();
  if (consume("-"))
    // -x を　0-xの式に置き換える
    return new_node(ND_SUB, new_node_num(0), primary());
  return primary();
}

Node *primary() {
  if (consume("(")) {
    Node *node = expr();
    expect(")");
    return node;
  }
  return new_node_num(expect_number());
}
