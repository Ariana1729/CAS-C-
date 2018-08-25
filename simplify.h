#ifndef SIMPLIFY_H
#define SIMPLIFY_H
struct Expr *simplify_single(struct Expr *node);
struct Expr *simplify_add(struct Expr *node);// +(+(a,b),c)...=+(a,b,c)
struct Expr *simplify_sub(struct Expr *node);// -(a,b)=+(a,*(-1,b))
struct Expr *simplify_mul(struct Expr *node);// *(*(a,b),c)=*(a,b,c)
struct Expr *simplify_div(struct Expr *node);// /(a,b)=*(a,^(b,-1))
struct Expr *simplify_exp(struct Expr *node);// a^0=1,b^1=b
struct Expr *simplify_root(struct Expr *node);// a root b=b^1/a
struct Expr *simplify_log(struct Expr *node);// log_a(b)=ln(b)/ln(a)
struct Expr *simplify_ln(struct Expr *node);// ln(a*b)=ln(a)+ln(b),ln(a^b)=b*ln(a)
struct Expr *simplify_expanda(struct Expr *node);// *(+(a,b),c)=+(*(a,c),*(b,c))
struct Expr *simplify_expandm(struct Expr *node);// ^(*(a,b),c)=*(^(a,c),^(b,c))
struct Expr *simplify_cola(struct Expr *node);// +(*(a,b,c),*(a,d,e),f)=+(*(a,+(*(b,c),*(d,e))),f)
struct Expr *simplify_colan(struct Expr *node,struct Expr *factor);// +(*(a,b,c),*(a,b,d)),b -> +(*(b,+(*(a,c),*(a,d))))
struct Expr *simplify_colm(struct Expr *node);//*(^(a,b),^(a,c),x)=*(^(a,+(b,c)),x) *(^(a,b),a,x)=*(^(a,+(b,1)),x)
struct Expr *simplify_repfunc(struct Expr *node,struct Expr *(*func)(struct Expr *));
int simplify_sort_qsortcmp(const void *a, const void *b);
struct Expr *simplify_sort(struct Expr *node);//sorts the node
double simplify_eval(struct Expr *node, struct Vars **variables, unsigned int n);
int simplify_peval_double(struct Expr *node);
int simplify_peval_int(struct Expr *node);
struct Expr *simplify_peval(struct Expr *node);
struct Expr *simplify_expand_func(struct Expr *node);
struct Expr *simplify_expand(struct Expr *node);
struct Expr *simplify_pfactor_func(struct Expr *node);
struct Expr *simplify_pfactor(struct Expr *node);
#endif