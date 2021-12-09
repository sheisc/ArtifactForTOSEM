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


#define N (1024 * 1024)
long __attribute__ ((aligned (4096))) shadow_stack[N];


unsigned long avx2_64_x_4_add(unsigned long * ss_ptr, unsigned long * ss_end, long x);


unsigned long get_cur_time_us(void) {
    struct timeval tv;
    struct timezone tz;

    gettimeofday(&tv, &tz);

    return (tv.tv_sec * 1000000UL) + tv.tv_usec;
}




unsigned long test_rand(unsigned long * ss_ptr, 
					unsigned long * ss_end, 
					long x, int repeat){
	unsigned long from, to;
	from = get_cur_time_us();
	for(int k = 0; k < repeat; k++){
		avx2_64_x_4_add(ss_ptr, ss_end, x);
	}
	to = get_cur_time_us();
	return (to - from) / repeat;
}


int main(int argc, char * argv[]) {

	int repeat = 100;
	long x = 2021;
	printf("sizeof(shadow_stack) = %ld \n", sizeof(shadow_stack));

	for(int n = 1024; n <= N; n *= 2){
		printf("n = %d KB, %lu us\n", n * 8 / 1024, 
					test_rand(shadow_stack, &shadow_stack[0] + n, x, repeat));
	}

    return 0;
}


