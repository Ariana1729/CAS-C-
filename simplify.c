#include<stdio.h>
#include<stdlib.h>
#include"expr.h"
void simplify_cmpfree(struct Expr *a,struct Expr *b){
    if(a==b)return;
    if(a->type<2)return;
    int i;
    for(i=0;i<a->len;++i){
        simplify_cmpfree(a->data.nodes[i],b->data.nodes[i]);
    }
    free(b);
}
struct Expr *simplify_add(struct Expr *node){
	int i;
	for(i=0;i<node->len;++i){
		if(node->data.nodes[i]->type==2){
			int j;
			node->len+=node->data.nodes[i]->len-1;
			struct Expr **tmp=realloc(node->data.nodes,(node->len)*sizeof(struct Expr*));
			if(tmp==NULL){
				printf("Memory error, please try again");
				exit(EXIT_FAILURE);
			}
			node->data.nodes=tmp;
			for(j=tmp[i]->len-1;j>0;--j){
				tmp[node->len-j]=tmp[i]->data.nodes[j];
			}
			tmp[i]=tmp[i]->data.nodes[j];
		}
	}
	return node;
}// +(+(a,b),c)...=+(a,b,c)
struct Expr *simplify_sub(struct Expr *node){
    node->type=2;
	node->data.nodes[1]=expr_mul(expr_num(-1),node->data.nodes[1]);
	return node;
}// -(a,b)=+(a,*(-1,b))
struct Expr *simplify_mul(struct Expr *node){
	int i;
	for(i=0;i<node->len;++i){
		if(node->data.nodes[i]->type==4){
			int j;
			node->len+=node->data.nodes[i]->len-1;
			struct Expr **tmp=realloc(node->data.nodes,(node->len)*sizeof(struct Expr*));
			if(tmp==NULL){
				printf("Memory error, please try again");
				exit(EXIT_FAILURE);
			}
			node->data.nodes=tmp;
			free(tmp[i]);
			for(j=tmp[i]->len-1;j>0;--j){
				tmp[node->len-j]=tmp[i]->data.nodes[j];
			}
			tmp[i]=tmp[i]->data.nodes[j];
		}
	}
	return node;
}// *(*(a,b),c)=*(a,b,c)
struct Expr *simplify_div(struct Expr *node){
	node->type=4;
	node->data.nodes[1]=expr_exp(node->data.nodes[1],expr_num(-1));
	return node;
}// /(a,b)=*(a,^(b,-1)) 
struct Expr *simplify_col(struct Expr *node){
	int i,j,t=node->type+2;
	for(i=0;i<node->len;++i){
		if(node->data.nodes[i]->type==t){
			for(j=0;j<node->len;++j){
				if(node->data.nodes[j]->type!=t)continue;
				if(i==j)continue;
                if(!expr_cmp(node->data.nodes[i]->data.nodes[0],node->data.nodes[j]->data.nodes[0])){
                    --node->len;
                    struct Expr *tmpn=node->data.nodes[node->len];
                    node->data.nodes[node->len]=node->data.nodes[j];
                    node->data.nodes[j]=tmpn;
                    simplify_cmpfree(node->data.nodes[i]->data.nodes[0],node->data.nodes[node->len]->data.nodes[0]);
                    tmpn=expr_add(node->data.nodes[i]->data.nodes[1],node->data.nodes[node->len]->data.nodes[1]);
                    struct Expr **tmp=realloc(node->data.nodes,(node->len)*sizeof(struct Expr*));
                    if (tmp==NULL){
                        printf("Memory error, please try again\n");
                        exit(EXIT_FAILURE);
                    }
                    printf("\n");
                    node->data.nodes=tmp;
                    node->data.nodes[i]->data.nodes[1]=tmpn;
                    i=node->len;
                    break;
                }
                if(!expr_cmp(node->data.nodes[i]->data.nodes[1],node->data.nodes[j]->data.nodes[1])){
                    --node->len;
                    struct Expr *tmpn=node->data.nodes[node->len];
                    node->data.nodes[node->len]=node->data.nodes[j];
                    node->data.nodes[j]=tmpn;
                    simplify_cmpfree(node->data.nodes[i]->data.nodes[1],node->data.nodes[j]->data.nodes[1]);
                    tmpn=t==4?expr_add(node->data.nodes[i]->data.nodes[0],node->data.nodes[node->len]->data.nodes[0]):expr_mul(node->data.nodes[i]->data.nodes[0],node->data.nodes[node->len]->data.nodes[0]);
                    struct Expr **tmp=realloc(node->data.nodes,(node->len)*sizeof(struct Expr*));
                    if (tmp==NULL){
                        printf("Memory error, please try again\n");
                        exit(EXIT_FAILURE);
                    }
                    printf("\n");
                    node->data.nodes=tmp;
                    node->data.nodes[i]->data.nodes[0]=tmpn;
                    i=j=node->len;
                }
			}
		}
	}
	return node;
}// +(*(a,b),*(a,c),x)=+(*(a,+(b,c)),x) , *(^(a,b),^(a,c),x)=*(^(a,+(b,c)),x)
/*int main(){
	struct Expr *tmp1=expr_add(expr_mul(expr_num(4),expr_num(3)),expr_mul(expr_num(5),expr_num(3)));
    struct Expr *tmp2=expr_mul(expr_mul(expr_exp(expr_num(4),expr_add(expr_num(2),expr_num(3))),expr_exp(expr_num(5),expr_num(3))),expr_exp(expr_num(5),expr_num(2)));
    struct Expr *tmp3=expr_div(expr_num(3),expr_num(4));
	simplify_col(tmp1);simplify_mul(tmp2);simplify_div(tmp3);
    expr_print(tmp1);
    printf("\n");
    expr_print(tmp2);
    printf("\n");
    simplify_col(tmp2);
    expr_print(tmp2);
    printf("\n");
    simplify_col(tmp2);
    expr_print(tmp2);
    printf("\n");
	return 0;
}
*/