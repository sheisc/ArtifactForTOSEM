
__thread long volatile tls_num;

    

long test_tcb_tls(int repeat){	
	for(int i = 0; i < repeat; i++){
		tls_num;
		// __asm__ __volatile__ (	
	 //        "leaq	tls_num@tlsgd(%%rip), %%rdi \r\n"	
	 //        "call	__tls_get_addr@PLT \r\n"	
	 //        "movq	(%%rax), %%rbx \r\n"	
	 //        : 
	 //        : 
	 //        :"rax", "rbx", "rdi"	
  //   	);
	}
	return 0;
}



//////////////////////////////////////////////////////////////////////////

// 2 * 1024 * 1024 = 2097152
// 4 * 1024 * 1024 = 4194304

// Suppose the size of call stack is 2MB, 
// We want to save the value 
long test_stk_tls(int repeat){	
	for(int i = 0; i < repeat; i++){
		__asm__ __volatile__ (	
	        "movq  %%rsp, %%rax \r\n"	
	        "andq  $-2097152, %%rax \r\n"	
	        "movq  -4194304(%%rax), %%rbx \r\n"	
	        : 
	        : 
	        :"rax", "rbx"	
    	);
	}
	return 0;
}

///////////////////////////////////////////////////////////////////////////

// Suppose r15 is the reserved register and we want to move it to rbx  

long test_reg_tls(int repeat){	
	for(int i = 0; i < repeat; i++){
		__asm__ __volatile__ (	
	    	"movq  %%r15, %%rbx \r\n"	
	        : 
	        : 
	        :"r15", "rbx" 
	    );	
	}
	return 0;
}



