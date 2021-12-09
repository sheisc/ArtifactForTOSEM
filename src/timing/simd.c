#include <mmintrin.h>  // MMX
#include <xmmintrin.h> // SSE (include mmintrin.h)
#include <emmintrin.h> // SSE2 (include xmmintrin.h)
#include <pmmintrin.h> // SSE3 (include emmintrin.h)
#include <tmmintrin.h> // SSSE3 (include pmmintrin.h)
#include <smmintrin.h> // SSE4.1 (include tmmintrin.h)
#include <nmmintrin.h> // SSE4.2 (include smmintrin.h)
#include <wmmintrin.h> // AES (include nmmintrin.h)
#include <immintrin.h> // AVX (include wmmintrin.h)

#include <sys/user.h>
// mprotect
#include <sys/mman.h>

// gettimeofday
#include <sys/time.h>
#include <time.h>
#include <stdio.h>



/*
    Add x to every long integers on the shadow stack [ss_ptr, ss_end).
    Can it be further improved by loop unrolling ?  Or Inline this function ?

    @ss_ptr         the start position, which will be 32-byte aligned in the function
    @ss_end         ss_end should have already been page-aligned, the high bound of the shadow stack
    @x              the random value

    @return         the microseconds we need
 */
unsigned long avx2_64_x_4_add(unsigned long * ss_ptr, unsigned long * ss_end, long x){
    ss_ptr = (unsigned long *)(((unsigned long) ss_ptr) & (-32));
    //unsigned long from, to;
    __m256i A, B, C;
    long __attribute__ ((aligned (32))) xs[] = { x, x, x, x};
    B = _mm256_load_si256((__m256i *)xs);
    while(ss_ptr < ss_end){
        A = _mm256_load_si256((__m256i *) ss_ptr);
        C = _mm256_add_epi64(A, B);
        _mm256_store_si256((__m256i *) ss_ptr, C);
        // 4 long integers
        ss_ptr += 4;
    }
    return 0;
}

