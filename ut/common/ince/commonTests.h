/**
 * @file commonTests.h
 * @author badbouille
 * @date 08/11/2020
 *
 * This project is released under the GNU General Public License v3.0.
 */

#ifndef _COMMONTESTS_H
#define _COMMONTESTS_H

/* BufferPtr tests */

void test_bufferptr_compare();
void test_bufferptr_mutability();
void test_bufferptr_copyfrom();
void test_bufferptr_erase();
void test_bufferptr_swap();
void test_bufferptr_xor();
void test_bufferptr_illegalinit();
void test_bufferptr_range();

/* SecureBufferPtr tests */
void test_securebufferptr_compare();
/*void test_securebufferptr_erase(); */ // erasure function is overriden for a more trusted one, but since both work great there is no test to do here

/* Shared memory tests */
void test_shared_rw();

/* Progress tests */
void test_progress_callback();
void test_progress_master();
void test_progress_shm();

#endif //_COMMON_TESTS_H
