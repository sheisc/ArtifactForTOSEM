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
#include <stdlib.h>
#include <string.h>


#define N (1024 * 1024)
long __attribute__ ((aligned (4096))) shadow_stack[N*256];


unsigned long avx2_64_x_4_add(unsigned long * ss_ptr, unsigned long * ss_end, long x);


unsigned long get_cur_time_us(void) {
    struct timeval tv;
    struct timezone tz;

    gettimeofday(&tv, &tz);

    return (tv.tv_sec * 1000000UL) + tv.tv_usec;
}




unsigned long test_4_mprotect(int repeat, char * addr){
	unsigned long from, to;
	from = get_cur_time_us();
	for(int i = 0; i < repeat; i++){
		mprotect(addr, PAGE_SIZE, PROT_WRITE);
		mprotect(addr, PAGE_SIZE, PROT_READ);

		mprotect(addr + 8*N, PAGE_SIZE, PROT_WRITE);
	    mprotect(addr + 8*N, PAGE_SIZE, PROT_READ);
	}
	to = get_cur_time_us();
	return (to - from) / repeat;
}



unsigned long test_2_mprotect(int repeat, char * addr){
	unsigned long from, to;
	from = get_cur_time_us();
	for(int i = 0; i < repeat; i++){
		// 16 MB
		mprotect(addr, 8 * N * 2, PROT_WRITE);
		mprotect(addr,  8 * N * 2, PROT_READ);
		
	}
	to = get_cur_time_us();
	return (to - from) / repeat;
}

int main(int argc, char * argv[]) {

	int repeat = 100;
	char * addr = (char *) malloc(N * 1024);
	addr = (char *) (((unsigned long) addr) & (-4096));
	printf("test_4_mprotect() = %lu \n", test_4_mprotect(repeat, addr));
	printf("test_2_mprotect() = %lu \n", test_2_mprotect(repeat, addr));

    return 0;
}


