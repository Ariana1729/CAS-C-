#ifndef EXPR_H
#define EXPR_H
struct Expr{
	int type;
	union{
		struct Expr **nodes;
		int num;
		char var;
	}data;
	int len;
};
/*
0:var
1:num
2:a+b
3:a-b
4:a*b
5:a/b
6:a^b
7:aroot(b)
8:log_a(b)
*/
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
int expr_cmp(struct Expr *a,struct Expr *b);
void expr_print(struct Expr *node);
void expr_cmpfree(struct Expr *a,struct Expr *b);
struct Expr *expr_dup(struct Expr *node);
struct Expr *expr_rm(struct Expr *node,int n);
void expr_del(struct Expr *node,int n);
void expr_free(struct Expr *node);
double expr_eval(struct Expr *node,struct Vars **variables,int n);
#endif
