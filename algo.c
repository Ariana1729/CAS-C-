#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<math.h>
#include"expr.h"
#include"simplify.h"
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
struct Token *algo_newtoken(int type,struct Expr *node){
    struct Token *ret=malloc(sizeof(struct Token));
    ret->type=type;
    ret->node=node;
    return ret;
}
struct Stack *algo_stackcreate(struct Token *token){
    struct Stack *ret=malloc(sizeof(struct Stack));
    ret->token=token;
    ret->prev=NULL;
    return ret;
}
void *algo_stackpush(struct Stack **stack,struct Token *token){
    struct Stack *new=algo_stackcreate(token);
    new->prev=*stack;
    *stack=new;
}
struct Token *algo_stackpop(struct Stack **stack){
    struct Stack *tmp=*stack;
    struct Token *ret=tmp->token;
    *stack=(*stack)->prev;
    free(tmp);
    return ret;
}
struct Queue *algo_queuecreate(){
    struct Queue *ret=malloc(sizeof(struct Queue));
    ret->last=ret->start=NULL;
}
void *algo_queuepush(struct Queue **stack,struct Token *token){
    struct QueueNode *new=malloc(sizeof(struct QueueNode));
    new->token=token;
    new->next=NULL;
    if((*stack)->start==NULL){
        return (*stack)->start=(*stack)->last=new;
    }
    (*stack)->last->next=new;
    (*stack)->last=new;
}
struct Token *algo_queuepop(struct Queue **stack){
    struct Token *ret=(*stack)->start->token;
    free((*stack)->start);
    (*stack)->start=(*stack)->start->next;
    return ret;
}
void algo_printqueue(struct Queue *queue){
    struct QueueNode *node=queue->start;
    while(node){
        switch(node->token->type){
            case EXPR_VAR:
                printf("%c",node->token->node->data.var);
                break;
            case EXPR_NUM:
                printf("%f",node->token->node->data.num);
                break;
            case EXPR_ADD:
                printf("+");
                break;
            case EXPR_SUB:
                printf("-");
                break;
            case EXPR_MUL:
                printf("*");
                break;
            case EXPR_DIV:
                printf("/");
                break;
            case EXPR_EXP:
                printf("^");
                break;
            case EXPR_ROOT:
                printf("root");
                break;
            case EXPR_LOG:
                printf("log");
                break;
            case EXPR_LN:
                printf("ln");
                break;
            default:
                printf("help");
                break;
        }
        node=node->next;
        if(node)printf(",");
    }
}
struct Queue *algo_parsetorpn(char *input){
    struct Stack *op=NULL;
    struct Queue *output=algo_queuecreate();
    int i,k,prevop=1;
    double j;
    for(i=0;input[i];++i){
        if(input[i]==' ')continue;
        if('0'<=input[i]&&input[i]<='9'){
            j=0;
            while('0'<=input[i]&&input[i]<='9'){
                j*=10;
                j+=input[i]-'0';
                ++i;
            }
            if(input[i]=='.'){
                k=i;
                ++i;
                while('0'<=input[i]&&input[i]<='9'){
                    j*=10;
                    j+=input[i]-'0';
                    ++i;
                }
                j/=pow(10,i-k-1);
            }
            algo_queuepush(&output,algo_newtoken(EXPR_NUM,expr_num(j)));
            --i;
            prevop=0;
            continue;
        }
        k=EXPR_VAR;
        if(input[i]=='(')k=EXPR_LBRACE;
        else if(input[i]==')')k=EXPR_RBRACE;
        else if(input[i]=='+')k=EXPR_ADD;
        else if(input[i]=='-')k=EXPR_SUB;
        else if(input[i]=='*')k=EXPR_MUL;
        else if(input[i]=='/')k=EXPR_DIV;
        else if(input[i]=='^')k=EXPR_EXP;
        else if(input[i]=='r'&&input[i+1]=='o'&&input[i+2]=='o'&&input[i+3]=='t')k=EXPR_ROOT;
        else if(input[i]=='l'&&input[i+1]=='o'&&input[i+2]=='g'&&input[i+3]=='_')k=EXPR_LOG;
        else if(input[i]=='l'&&input[i+1]=='n')k=EXPR_LN;
        if(i!=0){
            if((input[i]=='('&&input[i-1]==')'&&op->token->type!=EXPR_LOG)||(!prevop&&(k==EXPR_VAR||k==EXPR_LOG))){
                while(op){
                    if(op->token->type==EXPR_LBRACE)break;
                    if(EXPR_MUL<=op->token->type&&op->token->type<=EXPR_LOG){
                        algo_queuepush(&output,algo_stackpop(&op));
                    }else break;
                }
                algo_stackpush(&op,algo_newtoken(EXPR_MUL,NULL));
            }
        }
        if(k==EXPR_RBRACE){
            while(op->token->type!=0){
                algo_queuepush(&output,algo_stackpop(&op));
                if(op->token==NULL){
                    printf("Extra close bracket in %s at %d\n",input,i);
                    exit(EXIT_FAILURE);
                }
            }
            free(algo_stackpop(&op));
            if(op){
                if(op->prev){
                    if(op->prev->token->type>EXPR_EXP){
                        algo_queuepush(&output,algo_stackpop(&op));
                        break;
                    }
                }
            }
            continue;
        }
        if(k==EXPR_VAR){
            algo_queuepush(&output,algo_newtoken(EXPR_VAR,expr_var(input[i])));
            prevop=0;
            continue;
        }
        switch(k){
            case EXPR_ADD:
            case EXPR_SUB:
                while(op){
                    if(op->token->type==EXPR_LBRACE)break;
                    if(EXPR_ADD<=op->token->type&&op->token->type<=EXPR_LOG){
                        algo_queuepush(&output,algo_stackpop(&op));
                    }else break;
                }
                break;
            case EXPR_MUL:
            case EXPR_DIV:
                while(op){
                    if(op->token->type==EXPR_LBRACE)break;
                    if(EXPR_MUL<=op->token->type&&op->token->type<=EXPR_LOG){
                        algo_queuepush(&output,algo_stackpop(&op));
                    }else break;
                }
                break;
            case EXPR_ROOT:
            case EXPR_LOG:
                i+=3;
                break;
            case EXPR_LN:
                i+=1;
            default:
                break;
        }
        algo_stackpush(&op,algo_newtoken(k,NULL));
        prevop=1;
    }
    while(op){
        if(op->token->type==EXPR_LBRACE){
            printf("Unmatched brackets");
            while(op){free(algo_stackpop(&op));}
            while(output){free(algo_queuepop(&output));}
            return NULL;
        }
        algo_queuepush(&output,algo_stackpop(&op));
    }
    return output;
}//convert to reverse polish notation
struct Expr *algo_parsetotree(struct Queue *rpn){
    struct Stack *hold=NULL;
    struct Token *tok;
    struct Expr *tmpn;
    int tmpi;
    while(rpn->start){
        tok=algo_queuepop(&rpn);
        if(tok->node){
            tok->type=-1;
            algo_stackpush(&hold,tok);
            continue;
        }//tok->type=-1 would be a node
        if(tok->type==EXPR_LN){
            hold->token->node=expr_ln(hold->token->node);
            free(tok);
            continue;
        }
        tmpi=tok->type;
        free(tok);
        tok=algo_stackpop(&hold);
        tmpn=tok->node;
        switch(tmpi){
            case EXPR_ADD:
                if(hold==NULL){
                    printf("Adding an invalid expression");
                    exit(EXIT_FAILURE);
                }
                hold->token->node=expr_add(hold->token->node,tmpn);
                break;
            case EXPR_SUB:
                if(hold==NULL){
                    printf("Subtraction an invalid expression");
                    exit(EXIT_FAILURE);
                }
                hold->token->node=expr_sub(hold->token->node,tmpn);
                break;
            case EXPR_MUL:
                if(hold==NULL){
                    printf("Multiplying an invalid expression");
                    exit(EXIT_FAILURE);
                }
                hold->token->node=expr_mul(hold->token->node,tmpn);
                break;
            case EXPR_DIV:
                if(hold==NULL){
                    printf("Dividing an invalid expression");
                    exit(EXIT_FAILURE);
                }
                hold->token->node=expr_div(hold->token->node,tmpn);
                break;
            case EXPR_EXP:
                if(hold==NULL){
                    printf("Exponentiation of an invalid expression");
                    exit(EXIT_FAILURE);
                }
                hold->token->node=expr_exp(hold->token->node,tmpn);
                break;
            case EXPR_ROOT:
                if(hold==NULL){
                    printf("Root of an invalid expression");
                    exit(EXIT_FAILURE);
                }
                hold->token->node=expr_root(hold->token->node,tmpn);
                break;
            case EXPR_LOG:
                if(hold==NULL){
                    printf("Log of an invalid expression");
                    exit(EXIT_FAILURE);
                }
                hold->token->node=expr_log(hold->token->node,tmpn);
                break;
            default:
                printf("help\n");
        }
        free(tok);
    }
    return hold->token->node;
}//convert to syntax tree
int algo_isconst(struct Expr *node,struct Expr *var){
    if(node->type==EXPR_NUM)return 1;
    if(node->type==EXPR_VAR)return node->data.var==var->data.var;
    unsigned int i;
    for(i=0;i<node->len;++i){
        if(!algo_isconst(node->data.nodes[i],var))return 0;
    }
    return 1;//check if node is const wrt var
}
double algo_polynomial_exp(struct Expr *node,struct Expr *var){
    unsigned int i;
    if(node->type==EXPR_NUM)return 0;
    if(node->type==EXPR_VAR){
        if(node->data.var==var->data.var)return 1;
        return 0;
    }
    for(i=0;i<node->len;++i){
        if(!expr_cmp(node->data.nodes[i],var))return 1;
    }
    for(i=0;i<node->len;++i){
        if(node->data.nodes[i]->type!=EXPR_EXP)continue;
        if(!expr_cmp(node->data.nodes[i]->data.nodes[0],var))return node->data.nodes[i]->data.nodes[1]->data.num;
    }
    return 0;
}//handle non-double?
struct Expr *algo_polynomial(struct Expr *node,struct Expr *var){
    if(node->type!=EXPR_ADD&&node->type!=EXPR_SUB)return node;
    unsigned int i,j;
    node=simplify_expand(node);//fully expand
    for(i=0;i<node->len;++i){
        node->data.nodes[i]=simplify_colm(node->data.nodes[i]);
    }//converts everything into exponential form
    node=simplify_peval(node);
    node=simplify_cola(node);
    node=simplify_peval(node);
    if(node->type!=EXPR_ADD&&node->type!=EXPR_SUB)return node;
    for(i=0;i<node->len;++i){
        if(node->data.nodes[i]->type<EXPR_LBRACE)continue;
        for(j=0;j<node->data.nodes[i]->len;++j){
            if(node->data.nodes[i]->data.nodes[j]->type!=EXPR_EXP)continue;
            if(!expr_cmp(node->data.nodes[i]->data.nodes[j]->data.nodes[0],var))break;
        }
        if(j>=node->data.nodes[i]->len-1)continue;
        expr_swop(&node->data.nodes[i]->data.nodes[j],&node->data.nodes[i]->data.nodes[node->data.nodes[i]->len-1]);
    }
    int algo_polynomial_tmpfunc(const void *a,const void *b){
        return algo_polynomial_exp((*(struct Expr **)a),var)>algo_polynomial_exp((*(struct Expr **)b),var);
    };
    qsort(node->data.nodes,node->len,sizeof(struct Expr*),algo_polynomial_tmpfunc);
    return node;
}//converts to a polynomial in var
struct Expr *algo_differentiate(struct Expr *node,struct Expr *var){
    if(var->type!=EXPR_VAR)return node;//not implemented DF/DG
    unsigned int i;
    struct Expr *tmp;
    switch(node->type){
        case EXPR_NUM:return expr_num(0);
        case EXPR_VAR:return expr_num(node->data.var==var->data.var);
        case EXPR_ADD:
        case EXPR_SUB:
            for(i=0;i<node->len;++i){
                tmp=algo_differentiate(node->data.nodes[i],var);
                node->data.nodes[i]=tmp;
            }
            return node;
        case EXPR_MUL:
            tmp=expr_dup(node);
            expr_free(node);
            node->len=tmp->len;
            node->type=EXPR_ADD;
            for(i=1;i<node->len;++i){
                node->data.nodes[i]=expr_dup(tmp);
                node->data.nodes[i]->data.nodes[i]=algo_differentiate(node->data.nodes[i]->data.nodes[i],var);
            }
            node->data.nodes[0]=tmp;
            node->data.nodes[0]->data.nodes[0]=algo_differentiate(node->data.nodes[0]->data.nodes[0],var);
            return node;
        case EXPR_DIV:
            tmp=node->data.nodes[0];
            node->data.nodes[0]=expr_sub(expr_mul(algo_differentiate(expr_dup(node->data.nodes[0]),var),expr_dup(node->data.nodes[1])),
                                         expr_mul(expr_dup(node->data.nodes[0]),algo_differentiate(expr_dup(node->data.nodes[1]),var)));
            node->data.nodes[1]=expr_exp(node->data.nodes[1],expr_num(2));
            expr_free(tmp);
            return node;
        case EXPR_EXP:
            tmp=expr_mul(node,
                    expr_add(
                            expr_mul(
                                    algo_differentiate(expr_dup(node->data.nodes[0]),var),
                                    expr_div(expr_dup(node->data.nodes[1]),expr_dup(node->data.nodes[0]))),
                            expr_mul(algo_differentiate(expr_dup(node->data.nodes[1]),var),expr_ln(expr_dup(node->data.nodes[0])))
                    ));
            return tmp;
        case EXPR_ROOT:
            tmp=expr_exp(node->data.nodes[0],expr_div(expr_num(1),node->data.nodes[1]));
            free(node);
            return tmp;
        case EXPR_LOG:
            tmp=algo_differentiate(expr_div(expr_ln(node->data.nodes[0]),expr_ln(node->data.nodes[1])),var);
            free(node);
            return tmp;
        case EXPR_LN:
            tmp=expr_div(algo_differentiate(node->data.nodes[0],var),node->data.nodes[0]);
            free(node);
            return tmp;
        default:
            printf("help in expr_differentiate");
            break;
    }
    return node;
}//differentate wrt var,maybe implement chain rule easier?
int main(){
    struct Queue *queue=algo_parsetorpn("3+4+x*x^2^2+5*x-x^5+2.5x^2+3*5*x*5*x^4+log_5(x)");
    printf("Reverse Polish notation:\n");
    algo_printqueue(queue);
    printf("\n");
    struct Expr *expression=algo_parsetotree(queue);
    expression=algo_polynomial(expression,expr_var('x'));
    printf("Simplified expression\n");
    expr_print(expression);printf("\n");
    expr_swop(&expression->data.nodes[0],&expression->data.nodes[1]);
    expr_print(expression);printf("\n");
    printf("\n%d\n",algo_isconst(expression,expr_var('x')));
    struct Expr *test=expr_mul(expr_num(5),expr_ln(expr_exp(expr_var('x'),expr_num(3))));
    expr_print(test);printf("\n");
    test=simplify_expand(test);
    expr_print(test);printf("\n");
    /*expression=algo_differentiate(expression,expr_var('x'));
    expression=algo_polynomial(expression,expr_var('x'));
    printf("Differentiated:\n");
    expr_print(expression);
    printf("\n");
    struct Vars **vars=malloc(sizeof(struct Vars*));
    vars[0]=malloc(sizeof(struct Vars));
    vars[0]->var='x';
    vars[0]->num=5;
    printf("%f\n",simplify_eval(expression,vars,1));*/
    return 0;
}