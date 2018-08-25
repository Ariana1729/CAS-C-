#ifndef ALGO_H
#define ALGO_H
struct Token{
    int type;
    struct Expr *node;
};
struct Stack{
    struct Token *token;
    struct Stack *prev;
};
struct QueueNode{
    struct Token *token;
    struct QueueNode *next;
};
struct Queue{
    struct QueueNode *start;
    struct QueueNode *last;
};
struct Token *algo_newtoken(int type,struct Expr *node);
struct Stack *algo_stackcreate(struct Token *token);
void *algo_stackpush(struct Stack **stack,struct Token *token);
struct Token *algo_stackpop(struct Stack **stack);
struct Queue *algo_queuecreate();
void *algo_queuepush(struct Queue **stack,struct Token *token);
struct Token *algo_queuepop(struct Queue **stack);
void algo_printqueue(struct Queue *queue);
struct Queue *algo_parsetorpn(char *input);//convert to reverse polish notation
struct Expr *algo_parsetotree(struct Queue *rpn);//convert to syntax tree
int algo_isconst(struct Expr *node,struct Expr *var);
double algo_polynomial_exp(struct Expr *node,struct Expr *var);//handle non-double?
struct Expr *algo_polynomial(struct Expr *node,struct Expr *var);//converts to a polynomial in var
struct Expr *algo_differentiate(struct Expr *node,struct Expr *var);//differentate wrt var,maybe implement chain rule easier?
#endif
