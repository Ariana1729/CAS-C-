#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include"expr.h"
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
            algo_queuepush(&output,algo_newtoken(-1,expr_num(j)));
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
            algo_queuepush(&output,algo_newtoken(-2,expr_var(input[i])));
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
        tmpi=tok->type;
        free(tok);
        tok=algo_stackpop(&hold);
        tmpn=tok->node;
        switch(tmpi){
            case EXPR_ADD:
                hold->token->node=expr_add(hold->token->node,tmpn);
                break;
            case EXPR_SUB:
                hold->token->node=expr_sub(hold->token->node,tmpn);
                break;
            case EXPR_MUL:
                hold->token->node=expr_mul(hold->token->node,tmpn);
                break;
            case EXPR_DIV:
                hold->token->node=expr_div(hold->token->node,tmpn);
                break;
            case EXPR_EXP:
                hold->token->node=expr_exp(hold->token->node,tmpn);
                break;
            case EXPR_ROOT:
                hold->token->node=expr_root(hold->token->node,tmpn);
                break;
            case EXPR_LOG:
                hold->token->node=expr_log(hold->token->node,tmpn);
                break;
            default:
                printf("help\n\n");
        }
        free(tok);
    }
    return hold->token->node;
}//convert to syntax tree
int main(){
    struct Queue *queue=algo_parsetorpn("(1.1+2)*3-4log_(4)(x)+4root5");
    algo_printqueue(queue);
    printf("\n");
    struct Expr *expression=algo_parsetotree(queue);
    expr_print(expression);
    struct Vars **vars=malloc(sizeof(struct Vars*));
    vars[0]->var='x';
    vars[0]->num=5;
    printf("\n%f\n",expr_eval(expression,vars,1));
    return 0;
}