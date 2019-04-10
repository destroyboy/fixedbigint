#include <stdio.h>
#include <sys/time.h>

#include "bigint.h"
#include "lenstra.h"

extern long _seed;

void test1() {

    I c;
    for (int i=0; i<100; i++) {
        //printf("X:%d\n", i);
        k_random_digits(20, &c);
        //FROMSTRING(&c, "1009012267184813");
        //_seed = 3410633414;
        printf("%d ", i); PRINTD(&c);
        LENSTRA_TEST(&c, 500);
    }
}

int main() {

    struct timeval stop, start;
    gettimeofday(&start, NULL);
    test1();
    gettimeofday(&stop, NULL);
    printf("took %lu\n", stop.tv_sec - start.tv_sec);

    //

    return 0;
}