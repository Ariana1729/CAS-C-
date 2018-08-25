#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<complex.h>
#include"expr.h"
#include"simplify.h"
#include"algo.h"
struct CPoly{
    unsigned int len;
    double complex *coeff;
};
struct CPoly *poly_cpoly(unsigned int len){
    struct CPoly *ret=malloc(sizeof(struct CPoly));
    ret->len=len;
    ret->coeff=malloc(len*sizeof(double complex));
    return ret;
}
double complex poly_eval(struct CPoly *poly,double complex c){
    unsigned int i;
    double complex res=0;
    for(i=poly->len;i-->0;){
        res*=c;
        res+=poly->coeff[i];
    }
    return res;
}
double complex *poly_roots(struct CPoly *poly){
    struct CPoly *r0=poly_cpoly(poly->len-1);
    double complex *r1=malloc((poly->len-1)*sizeof(double complex));;
    double complex start=0.4+0.9*I,ctmp;//random?
    unsigned int i,j;
    ctmp=1;
    for(i=0;i<r0->len;++i){
        r0->coeff[i]=ctmp;
        ctmp*=start;
    }
    for(i=0;i<r0->len;++i){
        start=1;
        for(j=0;j<r0->len;++j){
            if(i!=j)start*=(r0->coeff[i]-r0->coeff[j]);
        }
        r1[i]=r0->coeff[i]-(poly_eval(poly,r0->coeff[i]))/(start*poly->coeff[r0->len]);
    }
    for(i=r0->len;i-->0;){
        if(cabs(r0->coeff[i]-r1[i])>0.01){
            ++i;
            break;
        }
    }
    while(++i){
        for(i=0;i<r0->len;++i){
            r0->coeff[i]=r1[i];
        }
        for(i=0;i<r0->len;++i){
            start=1;
            for(j=0;j<r0->len;++j){
                if(i!=j)start*=(r0->coeff[i]-r0->coeff[j]);
            }
            r1[i]=r0->coeff[i]-(poly_eval(poly,r0->coeff[i]))/(start*poly->coeff[poly->len-1]);
        }
        for(i=r0->len;i-->0;){
            if(cabs(r0->coeff[i]-r1[i])>0.00001){
                ++i;
                break;
            }
        }
    }
    free(r0->coeff);
    free(r0);
    return r1;
}
void poly_print(struct CPoly *poly){
    unsigned int i;
    for(i=poly->len;i-->1;){
        cimag(poly->coeff[i])<0?printf("(%f%fi)z^%u+",creal(poly->coeff[i]),cimag(poly->coeff[i]),i):printf("(%f+%fi)z^%u+",creal(poly->coeff[i]),cimag(poly->coeff[i]),i);
    }
    printf("(%f+%fi)",creal(poly->coeff[0]),cimag(poly->coeff[0]));
}
int main(){
    struct CPoly *test=poly_cpoly(10);
    double complex *sol;
    test->coeff[0]=-5;
    test->coeff[1]=3;
    test->coeff[2]=-3;
    test->coeff[3]=1;
    test->coeff[4]=7;
    test->coeff[5]=2;
    test->coeff[6]=7;
    test->coeff[7]=32;
    test->coeff[8]=-3;
    test->coeff[9]=-6;
    sol=poly_roots(test);
    poly_print(test);printf("\n");
    for (int i = 0; i < test->len-1; ++i) {
        cimag(sol[i])>0?printf("(%f+%fi)\n",creal(sol[i]),cimag(sol[i])):printf("(%f%fi)\n",creal(sol[i]),cimag(sol[i]));
    }
}