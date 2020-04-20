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
