#include<stdio.h>
#include<stdlib.h>
#include"expr.h"
struct Expr *simplify_single(struct Expr *node){
    if(node->len!=1)return node;
    if(node->type<2)return node;
    free(node);
    node=node->data.nodes[0];
    return node;
}
struct Expr *simplify_add(struct Expr *node){
    if(node->type!=2)return node;
	int i;
	for(i=0;i<node->len;++i){
		if(node->data.nodes[i]->type!=2)continue;
        int j;
        node->len+=node->data.nodes[i]->len-1;
        struct Expr **tmp=realloc(node->data.nodes,(node->len)*sizeof(struct Expr*));
        if(tmp==NULL){
            printf("Memory error, please try again");
            exit(EXIT_FAILURE);
        }
        node->data.nodes=tmp;
        for(j=tmp[i]->len-1;j>0;--j)tmp[node->len-j]=tmp[i]->data.nodes[j];
        tmp[i]=tmp[i]->data.nodes[j];
	}
	return node;
}// +(+(a,b),c)...=+(a,b,c)
struct Expr *simplify_sub(struct Expr *node){
    if(node->type!=3)return node;
    node->type=2;
	node->data.nodes[1]=expr_mul(expr_num(-1),node->data.nodes[1]);
	return node;
}// -(a,b)=+(a,*(-1,b))
struct Expr *simplify_mul(struct Expr *node){
    if(node->type!=4)return node;
	int i;
	for(i=0;i<node->len;++i){
		if(node->data.nodes[i]->type!=4)continue;
        int j;
        node->len+=node->data.nodes[i]->len-1;
        struct Expr **tmp=realloc(node->data.nodes,(node->len)*sizeof(struct Expr*));
        if(tmp==NULL){
            printf("Memory error, please try again");
            exit(EXIT_FAILURE);
        }
        node->data.nodes=tmp;
        free(tmp[i]);
        for(j=tmp[i]->len-1;j>0;--j)tmp[node->len-j]=tmp[i]->data.nodes[j];
        tmp[i]=tmp[i]->data.nodes[j];
	}
	return node;
}// *(*(a,b),c)=*(a,b,c)
struct Expr *simplify_div(struct Expr *node){
    if(node->type!=5)return node;
	node->type=4;
	node->data.nodes[1]=expr_exp(node->data.nodes[1],expr_num(-1));
	return node;
}// /(a,b)=*(a,^(b,-1))
struct Expr *simplify_expanda(struct Expr *node){
    if(node->type!=4)return node;
    int i;
    for(i=0;i<node->len;++i){
        if(node->data.nodes[i]->type!=2)continue;
        int j;
        struct Expr *tmp=node;
        node=expr_rm(tmp,i);
        if(node->len==0){
            expr_free(node);
            return expr_num(0);
        }
        node->data.nodes[0]=expr_mul(node->data.nodes[0],tmp);
        for(j=1;j<node->len;++j)node->data.nodes[j]=expr_mul(node->data.nodes[j],expr_dup(tmp));
        return node;
    }
    return node;
}// *(+(a,b),c)=+(*(a,c),*(b,c))
struct Expr *simplify_expandm(struct Expr *node){
    if(node->type!=6)return node;
    int i;
    for(i=0;i<node->len;++i){
        if(node->data.nodes[i]->type!=4)continue;
        int j;
        struct Expr *tmp=node;
        node=expr_rm(tmp,i);
        if(node->len==0){
            expr_free(node);
            return expr_num(0);
        }
        node->data.nodes[0]=expr_mul(node->data.nodes[0],tmp);
        for(j=1;j<node->len;++j)node->data.nodes[j]=expr_mul(node->data.nodes[j],expr_dup(tmp));
        return node;
    }
    return node;
}// ^(*(a,b),c)=*(^(a,c),^(b,c))
struct Expr *simplify_cola(struct Expr *node){
    if(node->type!=2)return node;
    int i,j,k,l;
    for(i=0;i<node->len;++i){
        if(node->data.nodes[i]->type!=4)continue;
        for(j=0;j<node->len;++j){
            if(node->data.nodes[j]->type!=4)continue;
            if(i==j)continue;
            for(k=0;k<node->data.nodes[i]->len;++k){
                for(l=0;l<node->data.nodes[j]->len;++l){
                    if(expr_cmp(node->data.nodes[i]->data.nodes[k],node->data.nodes[j]->data.nodes[l]))continue;
                    if(node->data.nodes[i]==node->data.nodes[j])node->data.nodes[j]=expr_dup(node->data.nodes[i]);
                    expr_cmpfree(node->data.nodes[i]->data.nodes[k],expr_rm(node->data.nodes[j],l));//removing additional node
                    //+(*(a,b,c),*(d,e),f)
                    struct Expr *tmpn=expr_rm(node->data.nodes[i],k);//a,*(b,c)
                    if(node->data.nodes[i]->len==0){
                        free(node->data.nodes[i]);
                        node->data.nodes[i]=expr_num(1);
                    }
                    if(node->data.nodes[j]->len==0){
                        free(node->data.nodes[j]);
                        node->data.nodes[j]=expr_num(1);
                    }
                    node->data.nodes[i]=expr_mul(tmpn,expr_add(node->data.nodes[i],node->data.nodes[j]));//*(a,+(*(b,c),*(d,e)))
                    expr_del(node,j);
                    k=node->data.nodes[i]->len;
                    break;
                }
            }
        }
    }
}// +(*(a,b,c),*(a,d,e),f)=+(*(a,+(*(b,c),*(d,e))),f)
struct Expr *simplify_colm(struct Expr *node){
    if(node->type!=4)return node;
	int i,j;
	for(i=0;i<node->len;++i){
        for(j=i+1;j<node->len;++j){
            if(node->data.nodes[i]->type!=6||node->data.nodes[j]->type!=6){
                if(!expr_cmp(node->data.nodes[i],node->data.nodes[j])){
                    expr_cmpfree(node->data.nodes[i],expr_rm(node,j));
                    node->data.nodes[i]=expr_exp(node->data.nodes[i],expr_num(2));
                }
                else if(node->data.nodes[i]->type==6){
                    if(expr_cmp(node->data.nodes[i]->data.nodes[0],node->data.nodes[j]))continue;
                    expr_cmpfree(node->data.nodes[i]->data.nodes[0],expr_rm(node,j));
                    node->data.nodes[i]->data.nodes[1]=expr_add(node->data.nodes[i]->data.nodes[1],expr_num(1));
                }
                else if(node->data.nodes[j]->type==6){
                    if(expr_cmp(node->data.nodes[j]->data.nodes[0],node->data.nodes[i]))continue;
                    expr_cmpfree(node->data.nodes[j]->data.nodes[0],expr_rm(node,i));
                    node->data.nodes[j]->data.nodes[1]=expr_add(node->data.nodes[j]->data.nodes[1],expr_num(1));
                }
                i=j=0;
                continue;
            }
            if(!expr_cmp(node->data.nodes[i]->data.nodes[0],node->data.nodes[j]->data.nodes[0])){
                expr_cmpfree(node->data.nodes[i]->data.nodes[0],node->data.nodes[j]->data.nodes[0]);
                node->data.nodes[i]->data.nodes[1]=expr_add(node->data.nodes[i]->data.nodes[1],node->data.nodes[j]->data.nodes[1]);
                expr_del(node,j);
                i=j=0;
                continue;
            }//*(^(a,b),^(a,c),x)=*(^(a,+(b,c)),x)
            if(!expr_cmp(node->data.nodes[i]->data.nodes[1],node->data.nodes[j]->data.nodes[1])){
                expr_cmpfree(node->data.nodes[i]->data.nodes[1],node->data.nodes[j]->data.nodes[1]);
                node->data.nodes[i]->data.nodes[0]=expr_mul(node->data.nodes[i]->data.nodes[0],node->data.nodes[j]->data.nodes[0]);
                expr_del(node,j);
                i=j=0;
                continue;
            }//*(^(a,b),^(c,b),x)=*(^(*(a,c),b),x)
        }
	}
	return node;
}//*(^(a,b),^(a,c),x)=*(^(a,+(b,c)),x) *(^(a,b),a,x)=*(^(a,+(b,1)),x)
int main(){
	struct Expr *tmp1=expr_add(simplify_mul(expr_mul(expr_num(1),expr_mul(expr_num(2),expr_num(3)))),expr_mul(expr_num(1),expr_num(4)));
    expr_print(tmp1);
    printf("\n");
    simplify_cola(tmp1);
    expr_print(tmp1);
    printf("\n");
    tmp1=simplify_single(tmp1);
    tmp1=simplify_expanda(tmp1);
    expr_print(tmp1);
    printf("\n");
    printf("%f\n",expr_eval(tmp1,NULL,0));
    struct Expr *tmp2=expr_mul(expr_mul(expr_exp(expr_num(4),expr_add(expr_num(2),expr_num(3))),expr_exp(expr_num(5),expr_num(3))),expr_exp(expr_num(5),expr_num(2)));
    simplify_mul(tmp2);
	expr_print(tmp2);
    printf("\n");
    simplify_colm(tmp2);
    expr_print(tmp2);
    printf("\n");
    tmp2=simplify_single(tmp2);
    tmp2=simplify_expanda(tmp2);
    expr_print(tmp2);
    printf("\n");
    printf("%f\n",expr_eval(tmp2,NULL,0));
    struct Expr *tmp3=expr_mul(expr_exp(expr_num(3),expr_num(4)),expr_mul(expr_num(5),expr_num(3)));
    simplify_mul(tmp3);
    expr_print(tmp3);
    printf("\n");
    simplify_colm(tmp3);
    expr_print(tmp3);
    printf("\n");
    struct Expr *tmp4=expr_mul(expr_num(1),expr_num(2));
    expr_del(tmp4,1);
    tmp4=expr_add(tmp4,tmp4);
    expr_print(tmp4);
    printf("\n");
    simplify_cola(tmp4);
    expr_print(tmp4);
	return 0;
}