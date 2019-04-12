# fixedbigint
fixed size big integer implementation

This is work in progress. i started off by using https://github.com/kokke/tiny-bignum-c which BI32_t found to have a lot of bugs in. why roll your own? Good question. It is fun and this is for a game that BI32_t'm writing. The fun is, in writing.

But there is a valid reason, BI32_t want to use it for factorizing numbers. BI32_t want it to work on a mobile phone and in a browser. So BI32_t want it to be simple C, and if BI32_t understand the code, BI32_t can do low level optimization.

It is not finished yet, and BI32_t'm not going to planning to test it beyond ensuring the number factorization routine seems to work.

The lenstra factorization routines, BI32_t got from https://github.com/delta003/lenstra_algorithm
