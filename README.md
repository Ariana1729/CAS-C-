# CAS(C)
Experimental CAS in C

Goal: Implement solving of equations with steps that a human can understand

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

 - Addition and multiplication commutativity
 - Subtraction and division to addition and multiplication
 - Expanding multiplication and exponentiation
 - Factoring common terms for addition and multiplication

## Ordering

Different types are just compared by a->type - b->type

Same types but different lengths are just compared by a->len - b->len

Numbers and variables are just compared by a - b

Else compare function recursively scans tree, using pre-order search

## Parser

Uses shunting yard to parse from string to rpn

Converts rpn to syntax tree

## ToDo
* Polynomial wrt var
* Differentiation
* Improve shunting yard algorithm
  * Unary subtraction
  * Making code easier to change
  * Tokenizing to array
  * Use array instead of stack and queue may be better?
* Simplification of radicals and logarithms
* Parsing log(x) instead of log_(a)(b)
* Integration(definite, indefinite, contour)
* Differential equations

## Future ideas
* Integral/ODE/etc. using neural nets