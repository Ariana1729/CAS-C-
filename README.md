# CAS(C)
Experimental CAS in C

Goal: Impliment solving of equations with steps that a human can understand

## Expression types:
0. variable
1. number
2. a+b
3. a-b
4. a*b
5. a/b
6. a^b
7. aroot(b)
8. log_a(b)

## Simplification algorithms:

`(a+b)+c=a+b+c`

`a-b=a+-1*b`

`(a*b)*c=a*b*C`

`a/b=a*b^-1`

`(a*b)+(a*c)=a*(b+c),(a*b)+(c*b)=(a+c)*b`

`(a^b)*(a^c)=a^(b+c),(a^b)*(c^b)=(a*c)^b`

## Ordering

Different types are just compared by a->type - b->type

Same types but different lengths are just compared by a->len - b->len

Numbers and variables are just compared by a - b

Else compare function resursively scans tree, using pre-order search



## ToDo
* Expanding
* Simplification of radicals and logarithms
* Parser
* Differentiation
* Integration(definite, indefinite, contour)
* Differential equations
