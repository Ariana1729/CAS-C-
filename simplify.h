#ifndef SIMPLIFY_H
#define SIMPLIFY_H
void simplify_cmpfree(struct Expr *a,struct Expr *b);
struct Expr *simplify_add(struct Expr *node);// +(+(a,b),c)...=+(a,b,c)
struct Expr *simplify_sub(struct Expr *node);// -(a,b)=+(a,*(-1,b))
struct Expr *simplify_mul(struct Expr *node);// *(*(a,b),c)=*(a,b,c)
struct Expr *simplify_div(struct Expr *node);// /(a,b)=*(a,^(b,-1))
struct Expr *simplify_col(struct Expr *node);// stuff like a^b*c^b, ab+cb, a^b+a^c
#endif
