#include<stdio.h>
#include<stdlib.h>
struct Expr{
	int type;
	union{
		struct Expr **nodes;
		int num;
		char var;
	}data;
	int len;
};
struct Expr *expr_var(char var){
	struct Expr *ret=malloc(sizeof(struct Expr));
	ret->type=0;
	ret->len=1;
	ret->data.var=var;
	return ret;
}
struct Expr *expr_num(int num){
	struct Expr *ret=malloc(sizeof(struct Expr));
	ret->type=1;
	ret->len=1;
	ret->data.num=num;
	return ret;
}
struct Expr *expr_add(struct Expr *aug,struct Expr *add){
	struct Expr *ret=malloc(sizeof(struct Expr));
	ret->data.nodes=malloc(2*sizeof(struct Expr*));
	ret->data.nodes[0]=aug;
	ret->data.nodes[1]=add;
	ret->type=2;
	ret->len=2;
	return ret;
}
struct Expr *expr_sub(struct Expr *min,struct Expr *sub){
	struct Expr *ret=malloc(sizeof(struct Expr));
	ret->data.nodes=malloc(2*sizeof(struct Expr*));
	ret->data.nodes[0]=min;
	ret->data.nodes[1]=sub;
	ret->type=3;
	ret->len=2;
	return ret;
}
struct Expr *expr_mul(struct Expr *multiplier,struct Expr *multiplicand){
	struct Expr *ret=malloc(sizeof(struct Expr));
	ret->data.nodes=malloc(2*sizeof(struct Expr*));
	ret->data.nodes[0]=multiplier;
	ret->data.nodes[1]=multiplicand;
	ret->type=4;
	ret->len=2;
	return ret;
}
struct Expr *expr_div(struct Expr *dividend,struct Expr *divisor){
	struct Expr *ret=malloc(sizeof(struct Expr));
	ret->data.nodes=malloc(2*sizeof(struct Expr*));
	ret->data.nodes[0]=dividend;
	ret->data.nodes[1]=divisor;
	ret->type=5;
	ret->len=2;
	return ret;
}
struct Expr *expr_exp(struct Expr *base,struct Expr *exponent){
	struct Expr *ret=malloc(sizeof(struct Expr));
	ret->data.nodes=malloc(2*sizeof(struct Expr*));
	ret->data.nodes[0]=base;
	ret->data.nodes[1]=exponent;
	ret->type=6;
	ret->len=2;
	return ret;
}
struct Expr *expr_root(struct Expr *deg,struct Expr *rad){
	struct Expr *ret=malloc(sizeof(struct Expr));
	ret->data.nodes=malloc(2*sizeof(struct Expr*));
	ret->data.nodes[0]=deg;
	ret->data.nodes[1]=rad;
	ret->type=7;
	ret->len=2;
	return ret;
}
struct Expr *expr_log(struct Expr *base,struct Expr *alog){
	struct Expr *ret=malloc(sizeof(struct Expr));
	ret->data.nodes=malloc(2*sizeof(struct Expr*));
	ret->data.nodes[0]=base;
	ret->data.nodes[1]=alog;
	ret->type=8;
	ret->len=2;
	return ret;
}
int expr_cmp(struct Expr *a,struct Expr *b){
	if(a->type!=b->type)return a->type-b->type;
	if(a->len!=b->len)return a->len-b->len;
	if(a->type<2)return a->data.num-b->data.num;
	int i,j;
	for(i=0;i<a->len;++i){
		j=expr_cmp(a->data.nodes[i],b->data.nodes[i]);
		if(j)return j;
	}
	return 0;
}
void expr_print(struct Expr *node){
	if(node->len==0)return;
	int i;
	switch(node->type){
		case(0):printf("%c",node->data.var);return;
		case(1):printf("%d",node->data.num);return;
		case(3):printf("(");expr_print(node->data.nodes[0]);printf(")-(");expr_print(node->data.nodes[1]);printf(")");break;
		case(5):printf("(");expr_print(node->data.nodes[0]);printf(")/(");expr_print(node->data.nodes[1]);printf(")");break;
		case(7):printf("log_(");expr_print(node->data.nodes[0]);printf(") (");expr_print(node->data.nodes[1]);printf(")");break;
		default:
			printf("(");expr_print(node->data.nodes[0]);printf(")");
			for(i=1;i<node->len;++i){
				switch(node->type){
					case(2):printf("+");break;
					case(4):printf("*");break;
					case(6):printf("^");break;
				}
				printf("(");expr_print(node->data.nodes[i]);printf(")");
			}	
	}
}
