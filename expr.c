#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<math.h>
#include"simplify.h"
#include"algo.h"
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
struct Expr *expr_num(double num){
    struct Expr *ret=malloc(sizeof(struct Expr));
    ret->type=EXPR_NUM;
    ret->len=1;
    ret->data.num=num;
    return ret;
}
struct Expr *expr_var(char var){
	struct Expr *ret=malloc(sizeof(struct Expr));
	ret->type=EXPR_VAR;
	ret->len=1;
	ret->data.var=var;
	return ret;
}
struct Expr *expr_add(struct Expr *aug,struct Expr *add){
	struct Expr *ret=malloc(sizeof(struct Expr));
	ret->data.nodes=malloc(2*sizeof(struct Expr*));
	ret->data.nodes[0]=aug;
	ret->data.nodes[1]=add;
	ret->type=EXPR_ADD;
	ret->len=2;
	return ret;
}
struct Expr *expr_sub(struct Expr *min,struct Expr *sub){
	struct Expr *ret=malloc(sizeof(struct Expr));
	ret->data.nodes=malloc(2*sizeof(struct Expr*));
	ret->data.nodes[0]=min;
	ret->data.nodes[1]=sub;
	ret->type=EXPR_SUB;
	ret->len=2;
	return ret;
}
struct Expr *expr_mul(struct Expr *multiplier,struct Expr *multiplicand){
	struct Expr *ret=malloc(sizeof(struct Expr));
	ret->data.nodes=malloc(2*sizeof(struct Expr*));
	ret->data.nodes[0]=multiplier;
	ret->data.nodes[1]=multiplicand;
	ret->type=EXPR_MUL;
	ret->len=2;
	return ret;
}
struct Expr *expr_div(struct Expr *dividend,struct Expr *divisor){
	struct Expr *ret=malloc(sizeof(struct Expr));
	ret->data.nodes=malloc(2*sizeof(struct Expr*));
	ret->data.nodes[0]=dividend;
	ret->data.nodes[1]=divisor;
	ret->type=EXPR_DIV;
	ret->len=2;
	return ret;
}
struct Expr *expr_exp(struct Expr *base,struct Expr *exponent){
	struct Expr *ret=malloc(sizeof(struct Expr));
	ret->data.nodes=malloc(2*sizeof(struct Expr*));
	ret->data.nodes[0]=base;
	ret->data.nodes[1]=exponent;
	ret->type=EXPR_EXP;
	ret->len=2;
	return ret;
}
struct Expr *expr_root(struct Expr *deg,struct Expr *rad){
	struct Expr *ret=malloc(sizeof(struct Expr));
	ret->data.nodes=malloc(2*sizeof(struct Expr*));
	ret->data.nodes[0]=deg;
	ret->data.nodes[1]=rad;
	ret->type=EXPR_ROOT;
	ret->len=2;
	return ret;
}
struct Expr *expr_log(struct Expr *base,struct Expr *alog){
    struct Expr *ret=malloc(sizeof(struct Expr));
    ret->data.nodes=malloc(2*sizeof(struct Expr*));
    ret->data.nodes[0]=base;
    ret->data.nodes[1]=alog;
    ret->type=EXPR_LOG;
    ret->len=2;
    return ret;
}
struct Expr *expr_ln(struct Expr *alog){
    struct Expr *ret=malloc(sizeof(struct Expr));
    ret->data.nodes=malloc(sizeof(struct Expr*));
    ret->data.nodes[0]=alog;
    ret->type=EXPR_LN;
    ret->len=1;
    return ret;
}
double expr_cmp(struct Expr *a,struct Expr *b){
	if(a->type!=b->type){
		return a->type-b->type;
	}
	if(a->len!=b->len){
		return a->len-b->len;
	}
	if(a->type==EXPR_NUM){
		return a->data.num-b->data.num;
	}
	if(a->type==EXPR_VAR){
		return a->data.var-b->data.var;
	}
	unsigned int i;
	double j;
	for(i=0;i<a->len;++i){
		j=expr_cmp(a->data.nodes[i],b->data.nodes[i]);
		if(j){
			return j;
		}
	}
	return 0;
}
void expr_swop(struct Expr **a,struct Expr **b){
	(*a)=(struct Expr *)((uintptr_t)(*a)^(uintptr_t)(*b));
	(*b)=(struct Expr *)((uintptr_t)(*a)^(uintptr_t)(*b));
	(*a)=(struct Expr *)((uintptr_t)(*a)^(uintptr_t)(*b));
}
void expr_print(struct Expr *node){
	if(node->len==0)return;
	unsigned int i;
	switch(node->type){
		case EXPR_VAR:printf("%c",node->data.var);return;
		case EXPR_NUM:
		    if(node->data.num==(int)node->data.num)printf("%d",(int)node->data.num);
		    else printf("%f",node->data.num);
		    return;
		case EXPR_SUB:printf("(");expr_print(node->data.nodes[0]);printf(")-(");expr_print(node->data.nodes[1]);printf(")");break;
		case EXPR_DIV:printf("(");expr_print(node->data.nodes[0]);printf(")/(");expr_print(node->data.nodes[1]);printf(")");break;
        case EXPR_EXP:printf("(");expr_print(node->data.nodes[0]);printf(")^(");expr_print(node->data.nodes[1]);printf(")");break;
        case EXPR_ROOT:printf("(");expr_print(node->data.nodes[0]);printf(")root(");expr_print(node->data.nodes[1]);printf(")");break;
		case EXPR_LOG:printf("log_(");expr_print(node->data.nodes[0]);printf(")(");expr_print(node->data.nodes[1]);printf(")");break;
		case EXPR_LN:printf("ln(");expr_print(node->data.nodes[0]);printf(")");break;
		default:
			printf("(");expr_print(node->data.nodes[0]);printf(")");
			for(i=1;i<node->len;++i){
				switch(node->type){
					case EXPR_ADD:printf("+");break;
					case EXPR_MUL:printf("*");break;
					default:printf("help");
				}
				printf("(");expr_print(node->data.nodes[i]);printf(")");
			}	
	}
}
void expr_cmpfree(struct Expr *a,struct Expr *b){
	if(a==b)return;
    if(a->type<EXPR_LBRACE)return;
    if(b->type<EXPR_LBRACE)return;
	unsigned int i;
	for(i=0;i<a->len&&i<b->len;++i){
		expr_cmpfree(a->data.nodes[i],b->data.nodes[i]);
	}
	free(b);
}
unsigned int expr_locate(struct Expr *node,struct Expr *ele){
    unsigned int i;
    for(i=0;i<node->len;++i){
        if(!expr_cmp(node->data.nodes[i],ele))break;
    }
    return i;
}
struct Expr *expr_dup(struct Expr *node){
	struct Expr *ret=malloc(sizeof(struct Expr));
	ret->type=node->type;
	ret->len=node->len;
	if(ret->type==EXPR_VAR){
		ret->data.var=node->data.var;
		return ret;
	}
	if(ret->type==EXPR_NUM){
		ret->data.num=node->data.num;
		return ret;
	}
	ret->data.nodes=malloc(ret->len*sizeof(struct Expr*));
	unsigned int i;
	for(i=0;i<ret->len;++i){
		ret->data.nodes[i]=expr_dup(node->data.nodes[i]);
	}
	return ret;
}
struct Expr *expr_rm(struct Expr *node,unsigned int n){
	unsigned int i;
	struct Expr *ret=expr_dup(node->data.nodes[n]);
	for(i=n+1;i<node->len;++i){
		node->data.nodes[i-1]=node->data.nodes[i];
	}
	--node->len;
	if(node->len==0){
		free(node->data.nodes[0]);
		return ret;
	}
	struct Expr **tmp=realloc(node->data.nodes,(node->len)*sizeof(struct Expr*));
	if(tmp==NULL){
		printf("Memory error, please try again\n");
		exit(EXIT_FAILURE);
	}
	node->data.nodes=tmp;
	return ret;
}
void expr_del(struct Expr *node,unsigned int n){
	unsigned int i;
	for(i=n+1;i<node->len;++i){
		node->data.nodes[i-1]=node->data.nodes[i];
	}
	--node->len;
	if(node->len==0){
		return free(node->data.nodes[0]);
	}
	struct Expr **tmp=realloc(node->data.nodes,(node->len)*sizeof(struct Expr*));
	if(tmp==NULL){
		printf("Memory error, please try again\n");
		exit(EXIT_FAILURE);
	}
	node->data.nodes=tmp;
}
void expr_free(struct Expr *node){
	if(node->type<0)return free(node);
	int i;
	for(i=0;i<node->len;++i)expr_free(node->data.nodes[i]);
}