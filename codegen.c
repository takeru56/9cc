#include "9cc.h"

void gen_lval(Node *node) {
  if (node->kind != ND_LVAR)
    error_at("代入の左辺値が変数ではありません", "");

    printf("  mov rax, rbp\n");
    printf("  sub rax, %d\n", node->offset);
    printf("  push rax\n");
}

void gen(Node *node) {
  switch (node->kind) {
    case ND_NUM:
      printf("  push %d\n", node->val);
      return;
    case ND_LVAR:
      gen_lval(node);
      printf("  pop rax\n");
      printf("  mov rax, [rax]\n");
      printf("  push rax\n");
      return;
    case ND_ASSIGN:
      gen_lval(node->lhs);
      gen(node->rhs);

      printf("  pop rdi\n");
      printf("  pop rax\n");
      printf("  mov [rax], rdi\n");
      printf("  push rdi\n");
      return;
    case ND_RETURN:
      gen(node->rhs);

      printf("  pop rax\n");
      printf("  mov  rsp, rbp\n");
      printf("  pop rbp\n");
      printf("  ret\n");
      return;
  }

  // 以下二項演算子
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
    case ND_EQ:
      // compareの結果はフラグレジスタに格納
      // 左側のオペラントから右側のオペラントを引いて
      // ・結果 0 　=> ZFを１
      // ・結果 負　=> SFを１
      // ・結果 正   => SFを０
      // フラグレジスタ: CPUが命令を実行したあとに演算状態を示す値を付与.
      // sete命令: 引数のレジスタにcmpで調べた結果が等しければ1をセット

      printf("  cmp rax, rdi\n");
      printf("  sete al\n");
      printf("  movzb rax, al\n");
      break;
    case ND_NE:
      printf("  cmp rax, rdi\n");
      printf("  setne al\n");
      printf("  movzb rax, al\n");
      break;
    case ND_LT:
      printf("  cmp rax, rdi\n");
      printf("  setl al\n");
      printf("  movzb rax, al\n");
      break;
    case ND_LE:
      printf("  cmp rax, rdi\n");
      printf("  setle al\n");
      printf("  movzb rax, al\n");
      break;
  }

  printf("  push rax\n");
}
