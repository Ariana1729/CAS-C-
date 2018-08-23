#ifndef EXPR_H
#define EXPR_H
#define EXPR_NUM (-2)
#define EXPR_VAR (-1)
#define EXPR_LBRACE 0
#define EXPR_RBRACE 1
#define EXPR_ADD 2
#define EXPR_SUB 3
#define EXPR_MUL 4
#define EXPR_DIV 5
#define EXPR_EXP 6
#define EXPR_ROOT 7
#define EXPR_LOG 8
#define EXPR_LN 9
struct Expr{
	int type;
	union{
		struct Expr **nodes;
		double num;
		char var;
	}data;
	unsigned int len;
};
struct Vars{
    char var;
    double num;
};
struct Expr *expr_var(char var);
struct Expr *expr_num(double num);
struct Expr *expr_add(struct Expr *aug,struct Expr *add);
struct Expr *expr_sub(struct Expr *min,struct Expr *sub);
struct Expr *expr_mul(struct Expr *multiplier,struct Expr *multiplicand);
struct Expr *expr_div(struct Expr *dividend,struct Expr *divisor);
struct Expr *expr_exp(struct Expr *base,struct Expr *exponent);
struct Expr *expr_root(struct Expr *deg,struct Expr *rad);
struct Expr *expr_log(struct Expr *base,struct Expr *alog);
struct Expr *expr_ln(struct Expr *alog);
double expr_cmp(struct Expr *a,struct Expr *b);
void expr_print(struct Expr *node);
void expr_cmpfree(struct Expr *a,struct Expr *b);
unsigned int expr_locate(struct Expr *node,struct Expr *ele);
struct Expr *expr_dup(struct Expr *node);
struct Expr *expr_rm(struct Expr *node,unsigned int n);
void expr_del(struct Expr *node,unsigned int n);
void expr_free(struct Expr *node);
double expr_eval(struct Expr *node,struct Vars **variables,unsigned int n);
int expr_peval_double(struct Expr *node);
int expr_peval_int(struct Expr *node);
struct Expr *expr_peval(struct Expr *node);
#endif
