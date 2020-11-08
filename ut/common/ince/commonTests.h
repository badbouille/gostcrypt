//
// Created by badbouille on 08/11/2020.
//

#ifndef _COMMONTESTS_H
#define _COMMONTESTS_H

/* BufferPtr tests */

void test_bufferptr_compare();
void test_bufferptr_mutability();
void test_bufferptr_copyfrom();
void test_bufferptr_erase();
void test_bufferptr_swap();
void test_bufferptr_illegalinit();
void test_bufferptr_range();

/* SecureBufferPtr tests */
void test_securebufferptr_compare();
/*void test_securebufferptr_erase(); */ // erasure function is overriden for a more trusted one, but since both work great there is no test to do here

#endif //_COMMON_TESTS_H
