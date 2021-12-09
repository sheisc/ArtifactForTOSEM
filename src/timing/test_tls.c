#include <stdio.h>
#include <stdlib.h>

#include <sys/time.h>
#include <time.h>








// The formal parameter n was used to specify the #loops.
// But to avoid the side-effects of cmp and jmp instructions in a loop,
// we write gen_tls_pic.py to loop-unroll all the TLS-accessing instructions,
// such that the result we get can be more accurate.
// So the n in gen_tls_pic.py (gen_c_file(n)) controls the number of iterations.
long test_tcb_tls(int n);
long test_reg_tls(int n);
long test_stk_tls(int n);
//long test_indirect_reg_tls(int n);


static unsigned long get_cur_time_us(void) {
    struct timeval tv;
    struct timezone tz;

    gettimeofday(&tv, &tz);

    return (tv.tv_sec * 1000000UL) + tv.tv_usec;
}




int main(int argc, char *argv[]){
	int n = 20000;
	if(argc >= 2){
		n = atoi(argv[1]);
	}

	int repeat = 1;
	long from, to;
	long sum_empty = 0, sum_tcb = 0, sum_reg = 0, sum_stk = 0;//, sum_ind_reg = 0;


	for(int i = 0; i < repeat; i++){


		from = get_cur_time_us();
		test_reg_tls(n);	
		to = get_cur_time_us();
		sum_reg += to - from;


		from = get_cur_time_us();
		test_tcb_tls(n);	
		to = get_cur_time_us();
		sum_tcb += to - from;
		

		from = get_cur_time_us();
		test_stk_tls(n);	
		to = get_cur_time_us();
		sum_stk += to - from;

		// from = get_cur_time_us();
		// test_indirect_reg_tls(n);	
		// to = get_cur_time_us();
		// sum_ind_reg += to - from;
	}
	



	printf("test_reg_tls: time = %ld us \n", sum_reg);
	//printf("test_indirect_reg_tls: time = %ld us \n", sum_ind_reg);
	printf("test_tcb_tls: time = %ld us \n", sum_tcb);	
	printf("test_stk_tls: time = %ld us \n", sum_stk);

	return 0;
}


