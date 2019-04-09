#include <stdio.h>

#include "bigint.h"

int main() {

    //for (int i=0; i<10; i++) {
    I a, b, c, x, y, z, x2, y2, z2, x3, y3, z3;
    FROMSTRING(&x, "8866128975287528");
    FROMSTRING(&y, "-8778405442862239");
    FROMSTRING(&z, "-2736111468807040");

    MUL(&x, &x, &x2);
    MUL(&x2, &x, &x3);

    MUL(&y, &y, &y2);
    MUL(&y2, &y, &y3);

    MUL(&z, &z, &z2);
    MUL(&z2, &z, &z3);

    //PRINTD(&x3);
    //PRINTD(&y3);
    //PRINTD(&z3);

    ADD(&x3, &y3, &y3);
    ADD(&y3, &z3, &z3);

    DIV(FROMINT(&a, -123), FROMINT(&b, 1230), &c);
    PRINTD(&c);

    //}


    //FROMINT(&b, 0xffffffffL);

    //MUL(&a, &b, &c);
    //PRINT(&c);
    //PRINTD(&c);
    //PRINT(&b);
    //PRINTD(&a);
    //DIVMOD0(&c, &c, &q, &r);

    //FROMINT(&b, -11);
    //printf("%d\n", COMPARE(&a,&b));
    //printf("%d\n", COMPARE(&a, &b));
    //PRINT(&q);
    //PRINT(&r);

    return 0;
}