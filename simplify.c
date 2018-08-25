#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<math.h>
#include"expr.h"
struct Expr *simplify_single(struct Expr *node){
    if(node->len!=1||node->type<0||node->type==EXPR_LN)return node;
    free(node);
    node=node->data.nodes[0];
    return node;
}
struct Expr *simplify_add(struct Expr *node){
    if(node->type!=EXPR_ADD)return node;
	unsigned int i;
	struct Expr *num0=expr_num(0);
	for(i=0;i<node->len;++i){
        if(node->data.nodes[i]->len==0){
            expr_rm(node,i);
            --i;
            continue;
        };
		if(node->data.nodes[i]->type!=EXPR_ADD)continue;
        unsigned int j;
        node->len+=node->data.nodes[i]->len-1;
        struct Expr **tmp=realloc(node->data.nodes,(node->len)*sizeof(struct Expr*));
        if(tmp==NULL){
            printf("Memory error, please try again");
            exit(EXIT_FAILURE);
        }
        node->data.nodes=tmp;
        for(j=tmp[i]->len-1;j>0;--j)tmp[node->len-j]=tmp[i]->data.nodes[j];
        tmp[i]=tmp[i]->data.nodes[j];
        for(i=0;i<node->len;++i){
            if(expr_cmp(node->data.nodes[i],num0))continue;
            expr_rm(node,i);
            --i;
        }
	}
	free(num0);
	return node;
}// +(+(a,b),c)...=+(a,b,c)
struct Expr *simplify_sub(struct Expr *node){
    if(node->type!=EXPR_SUB)return node;
    node->type=EXPR_ADD;
	node->data.nodes[1]=expr_mul(expr_num(-1),node->data.nodes[1]);
	return node;
}// -(a,b)=+(a,*(-1,b))
struct Expr *simplify_mul(struct Expr *node){
    if(node->type!=EXPR_MUL)return node;
	unsigned int i;
    struct Expr *num=expr_num(0);
    for(i=0;i<node->len;++i){
        num->data.num=0;
        if(!expr_cmp(node->data.nodes[i],num)){
            expr_free(node);
            return num;
        }
        num->data.num=1;
        if(!expr_cmp(node->data.nodes[i],num)){
            expr_rm(node,i);
            --i;
        }
    }
    free(num);
	for(i=0;i<node->len;++i){
        if(node->data.nodes[i]->len==0){
            expr_rm(node,i);
            --i;
            continue;
        };
		if(node->data.nodes[i]->type!=4)continue;
        unsigned int j;
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
    if(node->type!=EXPR_DIV)return node;
	node->type=EXPR_MUL;
	node->data.nodes[1]=expr_exp(node->data.nodes[1],expr_num(-1));
	return node;
}// /(a,b)=*(a,^(b,-1))
struct Expr *simplify_exp(struct Expr *node){
    if(node->type!=EXPR_EXP)return node;
    if(node->data.nodes[0]->type==EXPR_NUM){
        if(node->data.nodes[0]->data.num==0){
            expr_free(node);
            return expr_num(0);
        }
        if(node->data.nodes[0]->data.num==1){
            expr_free(node);
            return expr_num(1);
        }
    }
    if(node->data.nodes[1]->type==EXPR_NUM){
        if(node->data.nodes[1]->data.num==0){
            expr_free(node);
            return expr_num(1);
        }
        if(node->data.nodes[1]->data.num==1){
            free(node->data.nodes[1]);
            free(node);
            return node->data.nodes[0];
        }
    }
    return node;
}// 0^a=0,1^a=1,a^0=1,a^1=a
struct Expr *simplify_root(struct Expr *node){
    if(node->type!=EXPR_ROOT)return node;
    node->data.nodes[0]=(struct Expr *)((uintptr_t)node->data.nodes[0]^(uintptr_t)node->data.nodes[1]);
    node->data.nodes[1]=(struct Expr *)((uintptr_t)node->data.nodes[0]^(uintptr_t)node->data.nodes[1]);
    node->data.nodes[0]=(struct Expr *)((uintptr_t)node->data.nodes[0]^(uintptr_t)node->data.nodes[1]);
    node->data.nodes[1]=expr_div(expr_num(1),node->data.nodes[1]);
    node->type=EXPR_EXP;
    return node;
}// a root b=b^1/a
struct Expr *simplify_log(struct Expr *node){
    if(node->type!=EXPR_LOG)return node;
    expr_swop(&node->data.nodes[0],&node->data.nodes[1]);
    node->data.nodes[0]=expr_ln(node->data.nodes[0]);
    node->data.nodes[1]=expr_ln(node->data.nodes[1]);
    node->type=EXPR_DIV;
    return node;
}// log_a(b)=ln(b)/ln(a)
struct Expr *simplify_ln(struct Expr *node){
    if(node->type!=EXPR_LN)return node;
    if(node->data.nodes[0]->type==EXPR_MUL){
        struct Expr *tmp=node;
        node=node->data.nodes[0];
        free(tmp);
        node->type=EXPR_ADD;
        unsigned int i;
        for(i=0;i<node->len;++i){
            node->data.nodes[i]=expr_ln(node->data.nodes[i]);
        }
    }else if(node->data.nodes[0]->type==EXPR_EXP){
        node->data.nodes[0]->type=EXPR_MUL;//ln(a*b)
        struct Expr *tmp=node->data.nodes[0];//a*b
        node->data.nodes[0]=node->data.nodes[0]->data.nodes[0];//ln(a)
        tmp->data.nodes[0]=node;//ln(a)*b
        node=tmp;
    }
    return node;
}// ln(a*b)=ln(a)+ln(b),ln(a^b)=b*ln(a)
struct Expr *simplify_expanda(struct Expr *node){//simplify expand add
    if(node->type!=EXPR_MUL)return node;
    unsigned int i;
    for(i=0;i<node->len;++i){
        if(node->data.nodes[i]->len==0){
            expr_rm(node,i);
            --i;
            continue;
        };
        if(node->data.nodes[i]->type!=EXPR_ADD)continue;
        unsigned int j;
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
struct Expr *simplify_expandm(struct Expr *node){//simplify expand multiply
    if(node->type!=EXPR_EXP)return node;
    if(node->data.nodes[0]->type!=EXPR_MUL)return node;
    struct Expr *pow=expr_rm(node,1);
    free(node);
    node=node->data.nodes[0];
    unsigned int i;
    if(node->len==0){
        expr_free(node);
        return expr_num(1);
    }
    node->data.nodes[0]=expr_exp(node->data.nodes[0],pow);
    for(i=1;i<node->len;++i){
        node->data.nodes[i]=expr_exp(node->data.nodes[i],expr_dup(pow));
    }
    return node;
}// ^(*(a,b),c)=*(^(a,c),^(b,c))
struct Expr *simplify_cola(struct Expr *node){//simplify collect add
    if(node->type!=EXPR_ADD)return node;
    unsigned int i,j,k,l;
    for(i=0;i<node->len;++i){
        if(node->data.nodes[i]->len==0){
            expr_rm(node,i);
            --i;
            continue;
        };
        if(node->data.nodes[i]->type!=EXPR_MUL)continue;
        for(j=0;j<node->len;++j){
            if(node->data.nodes[j]->len==0){
                expr_rm(node,j);
                --j;
                continue;
            };
            if(node->data.nodes[j]->type!=EXPR_MUL)continue;
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
                    if(i!=0)--i;
                    j=node->len;
                    k=node->data.nodes[i]->len;
                    break;
                }
            }
        }
    }
    return node;
}// +(*(a,b,c),*(a,d,e),f)=+(*(a,+(*(b,c),*(d,e))),f)
struct Expr *simplify_colan(struct Expr *node,struct Expr *factor){//simplify collect add node
    if(node->type!=EXPR_ADD)return node;
    unsigned int i,j,k,l;
    for(i=0;i<node->len;++i){
        if(node->data.nodes[i]->len==0){
            expr_rm(node,i);
            --i;
            continue;
        };
        k=expr_locate(node->data.nodes[i],factor);
        if(k==node->len)continue;
        for(j=i+1;j<node->len;++j){
            if(node->data.nodes[j]->len==0){
                expr_rm(node,j);
                --j;
                continue;
            };
            l=expr_locate(node->data.nodes[j],factor);
            if(l==node->len)continue;
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
}// +(*(a,b,c),*(a,b,d)),b -> +(*(b,+(*(a,c),*(a,d))))
struct Expr *simplify_colm(struct Expr *node){//simplify collect multiply
    if(node->type!=EXPR_MUL)return node;
	unsigned int i,j;
	for(i=0;i<node->len;++i){
        if(node->data.nodes[i]->len==0){
            expr_rm(node,i);
            --i;
            continue;
        };
        for(j=i+1;j<node->len;++j){
            if(node->data.nodes[j]->len==0){
                expr_rm(node,j);
                --j;
                continue;
            };
            if(node->data.nodes[i]->type!=EXPR_EXP||node->data.nodes[j]->type!=EXPR_EXP){
                if(!expr_cmp(node->data.nodes[i],node->data.nodes[j])){
                    expr_cmpfree(node->data.nodes[i],expr_rm(node,j));
                    node->data.nodes[i]=expr_exp(node->data.nodes[i],expr_num(2));
                    i=j=0;
                }
                else if(node->data.nodes[i]->type==EXPR_EXP){
                    if(expr_cmp(node->data.nodes[i]->data.nodes[0],node->data.nodes[j]))continue;
                    expr_cmpfree(node->data.nodes[i]->data.nodes[0],expr_rm(node,j));
                    node->data.nodes[i]->data.nodes[1]=expr_add(node->data.nodes[i]->data.nodes[1],expr_num(1));
                    i=j=0;
                }
                else if(node->data.nodes[j]->type==EXPR_EXP){
                    if(expr_cmp(node->data.nodes[j]->data.nodes[0],node->data.nodes[i]))continue;
                    expr_cmpfree(node->data.nodes[j]->data.nodes[0],expr_rm(node,i));
                    node->data.nodes[j]->data.nodes[1]=expr_add(node->data.nodes[j]->data.nodes[1],expr_num(1));
                    i=j=0;
                }
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
struct Expr *simplify_repfunc(struct Expr *node,struct Expr *(*func)(struct Expr *)){
    struct Expr *tmp=expr_dup(node);
    node=(*func)(node);
    while(expr_cmp(tmp,node)){
        expr_free(tmp);
        tmp=expr_dup(node);
        node=(*func)(node);
    }
    expr_free(tmp);
    return node;
}
int simplify_sort_qsortcmp(const void *a, const void *b){
    return expr_cmp(*(struct Expr **)a,*(struct Expr **)b)>0;
}
struct Expr *simplify_sort(struct Expr *node){
    if(node->type!=EXPR_ADD&&node->type!=EXPR_MUL)return node;
    unsigned int i;
    for(i=0;i<node->len;++i){
        node->data.nodes[i]= simplify_sort(node->data.nodes[i]);
    }
    qsort(node->data.nodes, node->len, sizeof(struct Expr *), simplify_sort_qsortcmp);
    return node;
}//sorts the node
double simplify_eval(struct Expr *node, struct Vars **variables, unsigned int n){
    unsigned int i;
    double res;
    switch(node->type){
        case EXPR_VAR:
            for(i=0;i<n;++i){
                if(node->data.var==variables[i]->var)return variables[i]->num;
            }
            return 0;
        case EXPR_NUM:return node->data.num;
        case EXPR_ADD:
            res=0;
            for(i=0;i<node->len;++i){
                res+= simplify_eval(node->data.nodes[i], variables, n);
            }
            return res;
        case EXPR_SUB:
            return simplify_eval(node->data.nodes[0], variables, n)- simplify_eval(node->data.nodes[1], variables, n);
        case EXPR_MUL:
            res=1;
            for(i=0;i<node->len;++i){
                res*=simplify_eval(node->data.nodes[i], variables, n);
            }
            return res;
        case EXPR_DIV:
            return simplify_eval(node->data.nodes[0], variables, n)/ simplify_eval(node->data.nodes[1], variables, n);
        case EXPR_EXP:
            return pow(simplify_eval(node->data.nodes[0], variables, n), simplify_eval(node->data.nodes[1], variables, n));
        case EXPR_ROOT:
            return pow(simplify_eval(node->data.nodes[0], variables, n),1/ simplify_eval(node->data.nodes[1], variables, n));
        case EXPR_LOG:
            return log(simplify_eval(node->data.nodes[1], variables, n))/log(simplify_eval(node->data.nodes[0], variables, n));
        case EXPR_LN:
            return log(simplify_eval(node->data.nodes[0], variables, n));
        default:
            printf("help in simplify_eval\n");
            return 0;
    }
}
int simplify_peval_double(struct Expr *node){
    if(node->type==EXPR_NUM)return 1;
    if(node->type==EXPR_VAR)return 0;
    if(node->type==EXPR_ROOT)return 0;
    if(node->type==EXPR_LOG)return 0;
    if(node->type==EXPR_LN)return 0;
    unsigned int i;
    for(i=0;i<node->len;++i){
        if(!simplify_peval_double(node->data.nodes[i]))return 0;
    }
    return 1;
}
int simplify_peval_int(struct Expr *node){
    if(node->type==EXPR_NUM)return node->data.num==(int)node->data.num;
    if(node->type==EXPR_VAR)return 0;
    if(node->type==EXPR_ROOT)return 0;
    if(node->type==EXPR_LOG)return 0;
    if(node->type==EXPR_LN)return 0;
    unsigned int i;
    for(i=0;i<node->len;++i){
        if(!simplify_peval_double(node->data.nodes[i]))return 0;
    }
    return 1;
}
struct Expr *simplify_peval(struct Expr *node){
    if(node->type<EXPR_LBRACE)return node;
    unsigned int i;
    for(i=0;i<node->len;++i)node->data.nodes[i]= simplify_peval(node->data.nodes[i]);
    if(!simplify_peval_double(node)){
        if(node->type==EXPR_ADD){
            node=simplify_add(node);
            if(node->type!=EXPR_ADD)return simplify_peval(node);
            for(i=0;i<node->len;++i){
                if(!simplify_peval_double(node->data.nodes[i]))break;
            }
            if(i==node->len||i==0)return node;
            struct Expr *newnode=malloc(sizeof(struct Expr));
            newnode->len=node->len-i;
            newnode->data.nodes=node->data.nodes+i;
            newnode->type=node->type;
            node->len=i;
            double tmp;
            tmp=simplify_eval(node,NULL,0);
            expr_free(node);
            node=expr_add(expr_num(tmp),newnode);
            node=simplify_add(node);
        }else if(node->type==EXPR_MUL){
            node=simplify_mul(node);
            if(node->type!=EXPR_MUL)return simplify_peval(node);
            for(i=0;i<node->len;++i){
                if(!simplify_peval_double(node->data.nodes[i]))break;
            }
            if(i==node->len||i==0)return node;
            struct Expr *newnode=malloc(sizeof(struct Expr));
            newnode->len=node->len-i;
            newnode->data.nodes=node->data.nodes+i;
            newnode->type=node->type;
            node->len=i;
            double tmp;
            tmp=simplify_eval(node,NULL,0);
            expr_free(node);
            node=expr_mul(expr_num(tmp),newnode);
            node=simplify_mul(node);
        }
        return node;
    }
    double tmp;
    node=simplify_single(node);
    switch(node->type){
        case EXPR_ADD:
        case EXPR_SUB:
        case EXPR_MUL:
            if(!simplify_peval_double(node->data.nodes[1]))return node;
            break;
        case EXPR_EXP:
            if(!simplify_peval_int(node->data.nodes[1]))return node;
            break;
        case EXPR_DIV:
        case EXPR_ROOT:
        case EXPR_LOG:
        case EXPR_LN:
            return node;
        default:
            return node;//check if div,exp,float,log, does not produce infinite decimal
    }
    node=expr_num(simplify_eval(node,NULL,0));
    return node;
}
struct Expr *simplify_expand_func(struct Expr *node){
    if(node->type<EXPR_LBRACE)return node;
    node=simplify_repfunc(node,simplify_single);
    if(node->type<EXPR_LBRACE)return node;
    switch(node->type){
        case(EXPR_ADD):
            node=simplify_repfunc(node,simplify_add);
            node=simplify_cola(node);
            node=simplify_repfunc(node,simplify_single);
            break;
        case(EXPR_SUB):
            node=simplify_sub(node);
            break;
        case(EXPR_MUL):
            node=simplify_repfunc(node,simplify_mul);
            node=simplify_colm(node);
            node=simplify_repfunc(node,simplify_single);
            break;
        case(EXPR_DIV):
            node=simplify_div(node);
            break;
        case(EXPR_EXP):
            node=simplify_exp(node);
            break;
        case(EXPR_ROOT):
            node=simplify_root(node);
            break;
        case(EXPR_LOG):
            node=simplify_log(node);
            break;
        case(EXPR_LN):
            node=simplify_ln(node);
            break;
        default:
            break;
    }
    node=simplify_peval(simplify_sort(node));
    if(node->type==EXPR_EXP){
        node=simplify_expandm(node);
    }
    if(node->type==EXPR_MUL){
        node=simplify_expanda(node);
    }
    node=simplify_peval(simplify_sort(node));
    if(node->type<EXPR_LBRACE)return node;
    unsigned int i;
    for(i=0;i<node->len;++i){
        node->data.nodes[i]=simplify_expand_func(node->data.nodes[i]);
    }
    return node;
}
struct Expr *simplify_expand(struct Expr *node){
    return simplify_repfunc(node,simplify_expand_func);
}
struct Expr *simplify_pfactor_func(struct Expr *node){
    if(node->type<EXPR_LBRACE)return node;
    unsigned int i;
    for(i=0;i<node->len;++i){
        node->data.nodes[i]=simplify_pfactor_func(node->data.nodes[i]);
    }
    switch(node->type){
        case(EXPR_SUB):
            node=simplify_sub(node);
        case(EXPR_ADD):
            node=simplify_repfunc(node,simplify_add);
            node=simplify_repfunc(node,simplify_cola);
            break;
        case(EXPR_DIV):
            node=simplify_div(node);
        case(EXPR_MUL):
            node=simplify_repfunc(node,simplify_mul);
            node=simplify_repfunc(node,simplify_colm);
            break;
        case(EXPR_EXP):
        case(EXPR_ROOT):
        case(EXPR_LOG):
        case(EXPR_LN):
            break;
        default:
            break;
    }
    return node;
}
struct Expr *simplify_pfactor(struct Expr *node){
    return simplify_repfunc(node,simplify_expand_func);
}
/*int main(){
	struct Expr *test=expr_mul(expr_num(1),expr_div(expr_var('s'),expr_num(5)));
	expr_rm(test,0);
    expr_print(test);printf("\n");
    test=simplify_expand(test);
    expr_print(test);printf("\n");
    test=expr_peval(test);
    expr_print(test);printf("\n");
	return 0;
}*/