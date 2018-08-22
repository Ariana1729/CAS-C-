#ifndef SIMPLIFY_H
#define SIMPLIFY_H
struct Expr *simplify_single(struct Expr *node);
struct Expr *simplify_add(struct Expr *node);// +(+(a,b),c)...=+(a,b,c)
struct Expr *simplify_sub(struct Expr *node);// -(a,b)=+(a,*(-1,b))
struct Expr *simplify_mul(struct Expr *node);// *(*(a,b),c)=*(a,b,c)
struct Expr *simplify_div(struct Expr *node);// /(a,b)=*(a,^(b,-1))
struct Expr *simplify_expanda(struct Expr *node);// *(+(a,b),c)=+(*(a,c),*(b,c))
struct Expr *simplify_expandm(struct Expr *node);// ^(*(a,b),c)=*(^(a,c),^(b,c))
struct Expr *simplify_cola(struct Expr *node);// +(*(a,b,c),*(a,d,e),f)=+(*(a,+(*(b,c),*(d,e))),f)
struct Expr *simplify_colan(struct Expr *node,struct Expr *factor);//+(*(a,b,c),*(a,b,d)),b -> +(*(b,+(*(a,c),*(a,d))))
struct Expr *simplify_colm(struct Expr *node);//*(^(a,b),^(a,c),x)=*(^(a,+(b,c)),x) *(^(a,b),a,x)=*(^(a,+(b,1)),x)
#endif
