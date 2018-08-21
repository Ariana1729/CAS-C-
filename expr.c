#include<stdio.h>
#include<stdlib.h>
#include<math.h>
struct Expr{
	int type;
	union{
		struct Expr **nodes;
		double num;
		char var;
	}data;
	int len;
};
struct Vars{
    char var;
    double num;
};
struct Expr *expr_var(char var){
	struct Expr *ret=malloc(sizeof(struct Expr));
	ret->type=0;
	ret->len=1;
	ret->data.var=var;
	return ret;
}
struct Expr *expr_num(double num){
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
		case(1):printf("%f",node->data.num);return;
		case(3):printf("(");expr_print(node->data.nodes[0]);printf(")-(");expr_print(node->data.nodes[1]);printf(")");break;
		case(5):printf("(");expr_print(node->data.nodes[0]);printf(")/(");expr_print(node->data.nodes[1]);printf(")");break;
        case(7):printf("(");expr_print(node->data.nodes[0]);printf(")root(");expr_print(node->data.nodes[1]);printf(")");break;
		case(8):printf("log_(");expr_print(node->data.nodes[0]);printf(") (");expr_print(node->data.nodes[1]);printf(")");break;
		default:
			printf("(");expr_print(node->data.nodes[0]);printf(")");
			for(i=1;i<node->len;++i){
				switch(node->type){
					case(2):printf("+");break;
					case(4):printf("*");break;
					case(6):printf("^");break;
					default:printf("help");
				}
				printf("(");expr_print(node->data.nodes[i]);printf(")");
			}	
	}
}
void expr_cmpfree(struct Expr *a,struct Expr *b){
	if(a==b)return;
	if(a->type<2)return;
	int i;
	for(i=0;i<a->len;++i){
		expr_cmpfree(a->data.nodes[i],b->data.nodes[i]);
	}
	free(b);
}
struct Expr *expr_dup(struct Expr *node){
	struct Expr *ret=malloc(sizeof(struct Expr));
	ret->type=node->type;
	ret->len=node->len;
	if(ret->type==0){
		ret->data.var=node->data.var;
		return ret;
	}
	if(ret->type==1){
		ret->data.num=node->data.num;
		return ret;
	}
	ret->data.nodes=malloc(ret->len*sizeof(struct Expr*));
	int i;
	for(i=0;i<ret->len;++i){
		ret->data.nodes[i]=expr_dup(node->data.nodes[i]);
	}
	return ret;
}
struct Expr *expr_rm(struct Expr *node,int n){
	int i;
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
void expr_del(struct Expr *node,int n){
	int i;
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
	if(node->type<2)return free(node);
	int i;
	for(i=0;i<node->len;++i)expr_free(node->data.nodes[i]);
}
double expr_eval(struct Expr *node,struct Vars **variables,int n){
    int i;double res;
    switch(node->type){
        case(0):
            for(i=0;i<n;++i){
                if(node->data.var==variables[i]->var)return variables[i]->num;
            }
            return 0;
        case(1):return node->data.num;
        case(2):
            res=0;
            for(i=0;i<node->len;++i){
                res+=expr_eval(node->data.nodes[i],variables,n);
            }
            return res;
        case(3):
            return expr_eval(node->data.nodes[0],variables,n)-expr_eval(node->data.nodes[1],variables,n);
        case(4):
            res=1;
            for(i=0;i<node->len;++i){
                res*=expr_eval(node->data.nodes[i],variables,n);
            }
            return res;
        case(5):
            return expr_eval(node->data.nodes[0],variables,n)/expr_eval(node->data.nodes[1],variables,n);
        case(6):
            return pow(expr_eval(node->data.nodes[0],variables,n),expr_eval(node->data.nodes[1],variables,n));
        case(7):
            return pow(expr_eval(node->data.nodes[0],variables,n),1/expr_eval(node->data.nodes[1],variables,n));
        case(8):
            return log(expr_eval(node->data.nodes[0],variables,n))/log(1/expr_eval(node->data.nodes[1],variables,n));
		default:
			printf("help\n");
			return 0;
    }
}