#ifndef CAS_POLY_H
#define CAS_POLY_H
struct CPoly{
    unsigned int len;
    double complex *coeff;
};
struct CPoly *poly_cpoly(unsigned int len);
double complex poly_eval(struct CPoly *poly,double complex c);
double complex *poly_roots(struct CPoly *poly);
void poly_print(struct CPoly *poly);
#endif