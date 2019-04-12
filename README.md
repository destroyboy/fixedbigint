# fixedbigint
fixed size big integer implementation

This is work in progress. i started off by using https://github.com/kokke/tiny-bignum-c which I found to have issues. why roll your own? Good question. It is fun and this is for a game that I'm writing. The fun is, in writing.

But there is a valid reason, I want to use it for factorizing numbers. I want it to work on a mobile phone and in a browser. So I want it to be simple C, and if I understand the code, I can do low level optimization.

It is not finished yet, and I'm not going to planning to test it beyond ensuring the number factorization routine seems to work.

The lenstra factorization routines, I got from https://github.com/delta003/lenstra_algorithm

Both BI32_t class and BI63_t class seem to work. The fact that they can perform all the factorisation calculations seem to imply there are no bugs. But I'm not 100% sure of course.
