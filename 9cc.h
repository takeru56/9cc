//=========================
//                      Tokenizer
//=========================
typedef enum {
  TK_RESERVED,
  TK_IDENT,
  TK_NUM,
  TK_EOF,
} TokenKind;

typedef struct Token {
  TokenKind kind;
  struct Token *next;
  int val;
  char *str;
  int len;
} Token;

void error_at(char *loc, char *fmt, ...);
Token *tokenize(char *p);

//=========================
//                         Parser
//=========================
typedef enum {
  ND_ADD, // +
  ND_SUB, // -
  ND_MUL, // *
  ND_DIV, // /
  ND_NUM, // int
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
} Node;

//=========================
//                       Codegen
//=========================
Node *expr();
void gen(Node *node);

//=========================
//                       Main
//=========================
extern Token *token;