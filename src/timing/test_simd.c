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
#define SPA_CALL_STACK_SIZE		(8 * N)

//long __attribute__ ((aligned (4096))) shadow_stack[N];

long __attribute__ ((aligned (4096))) buddy_stack[4*N];


struct Buddy_TLS_Info{
    long random_val;            // random value for runtime rerandomization
    void *stack_top;			// page aligned
    struct Buddy_TLS_Info *buddy_info;	// pointer to our buddy
    long is_randomizing;
    long rand_cnt;
    long cpu_cycles;
    //unsigned long stack_size;   // stack size, aligned to power(2,n)
    //void *data;                 //
};


#define	 SPA_GET_RSP() (cur_rsp)


unsigned long cur_rsp;

unsigned long avx2_64_x_4_add(unsigned long * ss_ptr, unsigned long * ss_end, long x);


unsigned long get_cur_time_us(void) {
    struct timeval tv;
    struct timezone tz;

    gettimeofday(&tv, &tz);

    return (tv.tv_sec * 1000000UL) + tv.tv_usec;
}


unsigned long do_au_edu_unsw_randomize(
                                 struct Buddy_TLS_Info * tls_info,
                                 struct Buddy_TLS_Info * buddy_info,
                                 unsigned long call_stack_size,
                                 long cnt,
                                 long cycles,
                                 int mprotect_enabled){
	(void) cnt;
    unsigned long stack_top = (unsigned long) tls_info->stack_top;

    if(mprotect_enabled){
    	mprotect(tls_info, PAGE_SIZE, PROT_WRITE | PROT_READ);
    	mprotect(buddy_info, PAGE_SIZE, PROT_WRITE | PROT_READ);
	}

    tls_info->is_randomizing = 1;    
    buddy_info->is_randomizing = 1;


    // Maybe we can save the random seed into the TLS to avoid calling srandom again.
    srandom (time (0));
    long curRandOffset = random();
    //
    tls_info->random_val += curRandOffset;
    buddy_info->random_val += curRandOffset;

    //unsigned long  rbp = SPA_GET_RBP();
    unsigned long  rsp = cur_rsp;

    avx2_64_x_4_add(
                    //(unsigned long * )(rbp + SPA_CPU_WORD_LENGTH - call_stack_size),
                    (unsigned long * )(rsp - call_stack_size),
                    (unsigned long * )(stack_top - call_stack_size),
                    curRandOffset
                   );
    // printf("%p %p\n", 
    // 	(unsigned long * )(rsp - call_stack_size), 
    // 	(unsigned long * )(stack_top - call_stack_size));
    tls_info->is_randomizing = 0;
    buddy_info->is_randomizing = 0;

    tls_info->rand_cnt = cnt;
    buddy_info->rand_cnt = cnt;
    
    tls_info->cpu_cycles = cycles;
    buddy_info->cpu_cycles = cycles;

    if(mprotect_enabled){
    	mprotect(tls_info, PAGE_SIZE, PROT_READ);
    	mprotect(buddy_info, PAGE_SIZE, PROT_READ);
    }

    
    return 0;
}


unsigned long test_rand(struct Buddy_TLS_Info * tls_info,
						struct Buddy_TLS_Info * buddy_info,
						long x, 
						int repeat,
						int mprotect_enabled){
	unsigned long from, to;


	from = get_cur_time_us();
	for(int k = 0; k < repeat; k++){		
		do_au_edu_unsw_randomize(tls_info, buddy_info, 
									SPA_CALL_STACK_SIZE, 0, 0, mprotect_enabled);
	}
	to = get_cur_time_us();
	return (to - from) / repeat;
}


int main(int argc, char * argv[]) {

	int repeat = 100000;
	long x = 2021;
	printf("sizeof(buddy_stack) = %ld \n", sizeof(buddy_stack));

	struct Buddy_TLS_Info * tls_info = 
		(struct Buddy_TLS_Info *) (& buddy_stack[0]);
	struct Buddy_TLS_Info * buddy_info = 
		(struct Buddy_TLS_Info *) (& buddy_stack[N]);	
	
	tls_info->buddy_info = 	buddy_info;
	tls_info->stack_top = & buddy_stack[4 * N];

	buddy_info->buddy_info = tls_info;
	buddy_info->stack_top = & buddy_stack[4 * N];


	for(int n = 2 * N; n >= 1024; n /= 2){
		cur_rsp = ((long) &buddy_stack[4*N]) - n;
		printf("n = %d KB, %lu us\n", n  / 1024, 
					test_rand(tls_info, buddy_info, x, repeat, 1));
	}

	printf("\n\n................................... \n\n");
    mprotect(tls_info, PAGE_SIZE, PROT_WRITE | PROT_READ);
    mprotect(buddy_info, PAGE_SIZE, PROT_WRITE | PROT_READ);

	for(int n = 2 * N; n >= 1024; n /= 2){
		cur_rsp = ((long) &buddy_stack[4*N]) - n;
		printf("n = %d KB, %lu us\n", n  / 1024, 
					test_rand(tls_info, buddy_info, x, repeat, 0));
	}
    return 0;
}


