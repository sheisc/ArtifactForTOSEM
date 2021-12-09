import sys
import os


def gen_c_file(n):





	print("""
	__thread long volatile tls_num;

	    

	long test_tcb_tls(int repeat){	
	""");
		

	i = 0
	while i < n:
		print("\t\ttls_num;")
		i += 1

	print("""	
		return 0;
	}
	""");


	
	print("""//////////////////////////////////////////////////////////////////////////

	// 2 * 1024 * 1024 = 2097152
	// 4 * 1024 * 1024 = 4194304

	// Suppose the size of call stack is 2MB, 
	// We want to save the value 
	long test_stk_tls(int repeat){""");

	i = 0
	while i < n:		
		print("""
			__asm__ __volatile__ (	
		        "movq  %%rsp, %%rax \\r\\n"	
		        "andq  $-2097152, %%rax \\r\\n"	
		        "movq  -4194304(%%rax), %%rbx \\r\\n"	
		        : 
		        : 
		        :"rax", "rbx"	
	    	);
		""")
		i += 1	    

	print("""
		return 0;
	}
	""");


	print("""
	///////////////////////////////////////////////////////////////////////////

	// Suppose r15 is the reserved register and we want to move it to rbx  

	long test_reg_tls(int repeat){""")

	i = 0
	while i < n:

		print("""
			__asm__ __volatile__ (	
		    	"movq  %%r15, %%rbx \\r\\n"	
		        : 
		        : 
		        :"r15", "rbx" 
		    );""")
		i += 1

	print("""
		return 0;
	}
	""")


	# print("""
	# ///////////////////////////////////////////////////////////////////////////

	# // Suppose r15 is the reserved register and we want to move it to rbx  

	# long test_indirect_reg_tls(int repeat){""")

	# print("\t\tunsigned long  p = 0x12345678;")


	# print("""
	# 		__asm__ __volatile__ (	
	# 	    	"movq  %0, %%r15 \\r\\n"	
	# 	        : 
	# 	        :"r"(&p)
	# 	        :"r15"
	# 	    );""")	

	# i = 0
	# while i < n:

	# 	print("""
	# 		__asm__ __volatile__ (	
	# 	    	"movq  (%%r15), %%rbx \\r\\n"	
	# 	        : 
	# 	        : 
	# 	        :"r15", "rbx" 
	# 	    );""")
	# 	i += 1

	# print("""
	# 	return 0;
	# }
	# """)


if __name__ == '__main__':
	n = 10000

	if len(sys.argv) > 1:
		n = int(sys.argv[1])
	gen_c_file(n)

