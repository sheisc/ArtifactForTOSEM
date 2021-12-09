#ifndef SPA_H
#define SPA_H

// (1) Instrumentation Option 1

// thread local storage for the random value + buddy stack
#define    USE_SPA_BUDDY_STACK_TLS

// thread local storage for the random value + global stack size +  buddy stack
//#define    USE_SPA_BUDDY_STACK_TLS_WITH_STK_SIZE
// When building firefox, it should be commented.
//#define    NON_PIE_GLOBAL_VAR_FOR_STK_SIZE


// (2) Instrumentation Option 2
// traditional shadow stack
//#define  USE_SPA_SHADOW_STACK
//#define  USE_SPA_SHADOW_STACK_VIA_REG

// (3) Instrumentation Option 3
// global variable for the random value + shadow stack
//#define    USE_SPA_SHADOW_STACK_PLUS_GLOBAL_RANDVAR





//#define  ENABLE_SPA_RDSTC



#define    SAVE_TOTAL_RAND_CNT

#ifdef __cplusplus
extern "C" {
#endif




#include <time.h>
//#include "buddy_tls.h"

#define SPA_VERSION         "0.01"

#define SPA_CFI_STARTPROC                       "\t.cfi_startproc\n"
#define SPA_CLANG_RETQ                          "\tretq\n"
#define SPA_GCC_RET                             "\tret\n"
#define SPA_CFI_ENDPROC                         "\t.cfi_endproc\n"

// vdivps	%zmm10, %zmm0, %zmm0 {%k1} {z}
#define SPA_XMM_YMM_ZMM_WRITE_MASK_CLANG        "} {z}\n"
#define SPA_XMM_YMM_ZMM_WRITE_MAST_GCC          "}{z}\n"

#define SPA_DEBUG_OUTPUT(x)    do{} while(0)
//#define SPA_DEBUG_OUTPUT(x)    do{printf("%s %d: ", __FILE__, __LINE__);  x ; } while(0)


//#define SPA_STACK_SIZE      BUDDY_CALL_STACK_SIZE


void __libc_csu_init(void);

#define	SPA_MAIN_STACK_END          ((unsigned long)__libc_csu_init)
#define	SPA_OTHER_STACK_END         0

// Once this environment varible, runtime rerandomization is enabled
#define SPA_ENABLE_RANDOMIZATION_ENV      "__SPA_ENABLE_RANDOMIZATION"

#define SPA_CALL_STACK_SIZE_ENV           "__SPA_CALL_STACK_SIZE"
//#define SPA_MAIN_EXE_INITED_ENV           "__SPA_MAIN_EXE_INITED"

// \x43\x53\x45\x40\x55\x4e\x53\x57,  "CSE@UNSW"
#define SPA_GLOBAL_STACK_SIZE_MAGIC_NUM     (0x57534E5540455343L)
#define SPA_MAIN_FUNC_CALLED_FLAG           2021



// Intel(R) Core(TM) i5-6500 CPU @ 3.20GHz
// randomization period in microseconds
#define SPA_RAND_PERIOD_IN_US           1000L
#define SPA_CPU_FREQUENCY               3200000000L
#define CPU_CYCLES_PER_US               (SPA_CPU_FREQUENCY / 1000000)
#define CPU_CYCLES_PER_RANDOMIZATION        (CPU_CYCLES_PER_US * SPA_RAND_PERIOD_IN_US)


#define SPA_RESERVED_REG        "xmm15"


#define SPA_RANDOM_VAL_NO_PIE          ".unsw.randomval"
#define SPA_RANDOM_VAL_PIE             ".unsw.randomval@GOTPCREL(%rip)"
#define SPA_RANDOM_VAL                 SPA_RANDOM_VAL_PIE


#define SPA_ERROR(format, ...)                                      \
    do {                                                            \
        fprintf(stderr, "error: " format "\n", ##__VA_ARGS__);      \
        abort();                                                    \
    } while (0)



#define	 SPA_GET_RBP() ({ \
    unsigned long  p; \
    __asm__ __volatile__ (	\
        "movq\t%%rbp, %0 \r\n"	\
        :"=r"(p) \
        :	\
        :	\
    );	\
    p; \
})


#define	 SPA_GET_RSP() ({ \
    unsigned long  p; \
    __asm__ __volatile__ (	\
        "movq\t%%rsp, %0 \r\n"	\
        :"=r"(p) \
        :	\
        :	\
    );	\
    p; \
})




// FIXME: 4 on 32-bit, 8 on 64-bit
// It should be signed integer, as "-8" might be used in assembly code.
#define SPA_CPU_WORD_LENGTH           ((long)(sizeof(unsigned long)))


#define DYN_SYM_TABLE_FILE  "/dynamic_symbol_table.txt"
//#define BUDDY_STACK_SIZE_LIB   "/rt_lib.so"

// if it is definded in Makefile, then bla bla ...
#ifdef SPA_CUR_WORK_DIR
    #define DEFAULT_SPA_DYNAMIC_SYMBOL_TABLE_PATH  SPA_CUR_WORK_DIR  DYN_SYM_TABLE_FILE
    #define DEFAULT_BUDDY_STACK_SIZE_LIB_PATH  SPA_CUR_WORK_DIR
#else
    #define DEFAULT_SPA_DYNAMIC_SYMBOL_TABLE_PATH    "/home/iron/src/SPA" DYN_SYM_TABLE_FILE
    #define DEFAULT_BUDDY_STACK_SIZE_LIB_PATH  "/home/iron/src/SPA"
#endif

//////////////////////////////////////// ACCESS  RESERVED REGISTER ////////////////////////////////////////////



// multiple-thread, shared-object
// @tpoff
// @tlsgd
// @gottpoff
// @GOTPCREL
// @gotntpoff
// %%xmm15
#define	 GET_MT_BARRA_RANVAL() ({ \
    unsigned long  p; \
    __asm__ __volatile__ (	\
        "movq	%%" SPA_RESERVED_REG ", %0 \r\n"	\
        :"=r"(p) \
        :	\
        :	\
    );	\
    p; \
})

#define	 SET_MT_BARRA_RANVAL(val) ({ \
    unsigned long  p = (val); \
    __asm__ __volatile__ (	\
        "movq	%0, %%" SPA_RESERVED_REG " \r\n"	\
        : \
        :"r"(p)\
        :	\
    );	\
    p; \
})


#define	 GET_SPA_RANVAL() ({ \
    unsigned long  p; \
    __asm__ __volatile__ (	\
        "movq	%%" SPA_RESERVED_REG ", %0 \r\n"	\
        :"=r"(p) \
        :	\
        :	\
    );	\
    p; \
})



//////////////////////////////////////////// BUDDY STACK API //////////////////////////////////////////////////////
// See weak_stack_size.s,  the first 32 bytes are reserved
#define SPA_OFFSET_OF_MAIN_FUNC_CALLED_FLAG          32

//fprintf(outf, "\tmovq\t" ".BUDDY.CALL_STACK_SIZE_MASK@GOTPCREL(%%rip), %s\n", reg_x);
//fprintf(outf, "\tmovq\t" "$%ld, %ld(%s)\n", SPA_GLOBAL_STACK_SIZE_MAGIC_NUM,

#define GET_FLAG_OF_MAIN_FUNC_CALLED() ({ \
    long p; \
    __asm__ __volatile__ (	\
        "movq  .BUDDY.CALL_STACK_SIZE_MASK@GOTPCREL(%%rip), %0 \r\n"	\
        "movq  32(%0), %0 \r\n"	\
        :"=r"(p) \
        :	\
        :	\
    );	\
    p; \
})


#define	 GET_BUDDY_CALL_STACK_SIZE_MASK_ADDR() ({ \
    void *p; \
    __asm__ __volatile__ (	\
        "movq  .BUDDY.CALL_STACK_SIZE_MASK@GOTPCREL(%%rip), %0 \r\n"	\
        :"=r"(p) \
        :	\
        :	\
    );	\
    p; \
})


#define	 GET_BUDDY_CALL_STACK_SIZE_MASK() ({ \
    long  p; \
    __asm__ __volatile__ (	\
        "movq  .BUDDY.CALL_STACK_SIZE_MASK@GOTPCREL(%%rip), %0 \r\n"	\
        "movq  (%0), %0 \r\n"	\
        :"=r"(p) \
        :	\
        :	\
    );	\
    p; \
})


#define	 SET_BUDDY_CALL_STACK_SIZE_MASK(val) ({ \
    long  p = (val); \
    __asm__ __volatile__ (	\
        "movq  .BUDDY.CALL_STACK_SIZE_MASK@GOTPCREL(%%rip), %%rax \r\n"	\
        "movq  %%rbx, (%%rax) \r\n"	\
        : \
        :"b"(p)\
        :"rax"	\
    );	\
    p; \
})

// Macros starting with "DEF_*" are used as constants in afl-as.c
#define DEF_BUDDY_CALL_STACK_SIZE                   (8L << 20)
#define DEF_BUDDY_CALL_STACK_SIZE_MASK              (-DEF_BUDDY_CALL_STACK_SIZE)

#define DEF_BUDDY_SHADOW_STACK_SIZE                     DEF_BUDDY_CALL_STACK_SIZE
#define DEF_BUDDY_FUNCTION_LOCAL_STORAGE_SIZE           (DEF_BUDDY_CALL_STACK_SIZE + DEF_BUDDY_SHADOW_STACK_SIZE)
#define DEF_BUDDY_THREAD_LOCAL_STORAGE_SIZE             DEF_BUDDY_FUNCTION_LOCAL_STORAGE_SIZE
#define DEF_BUDDY_STACK_SIZE                        (4 * DEF_BUDDY_CALL_STACK_SIZE)

#define DEF_SPA_SS_OFFSET       DEF_BUDDY_CALL_STACK_SIZE

//#if defined(USE_SPA_BUDDY_STACK_TLS_WITH_STK_SIZE)
//#define BUDDY_CALL_STACK_SIZE                       (-GET_BUDDY_CALL_STACK_SIZE_MASK())
//#else
//#define BUDDY_CALL_STACK_SIZE                       DEF_BUDDY_CALL_STACK_SIZE
//#endif //

//  Macros without "DEF_*" are used as variables
#define BUDDY_CALL_STACK_SIZE                       (-GET_BUDDY_CALL_STACK_SIZE_MASK())
#define BUDDY_CALL_STACK_SIZE_MASK                  (~(unsigned long)(BUDDY_CALL_STACK_SIZE - 1))

#define BUDDY_SHADOW_STACK_SIZE                     BUDDY_CALL_STACK_SIZE
#define BUDDY_FUNCTION_LOCAL_STORAGE_SIZE           (BUDDY_CALL_STACK_SIZE + BUDDY_SHADOW_STACK_SIZE)
#define BUDDY_THREAD_LOCAL_STORAGE_SIZE             BUDDY_FUNCTION_LOCAL_STORAGE_SIZE
#define BUDDY_STACK_SIZE                            (BUDDY_FUNCTION_LOCAL_STORAGE_SIZE + BUDDY_THREAD_LOCAL_STORAGE_SIZE)



#define BUDDY_ALIGN_CALL_STACK_POINTER(x)           (BUDDY_CALL_STACK_SIZE_MASK & (unsigned long)(x))



// ATI
#define BUDDY_VIR_REG_0         0
#define BUDDY_VIR_REG_1         8
#define BUDDY_VIR_REG_2         16
#define BUDDY_VIR_REG_3         24
// ...


#define BUDDY_VIR_REG_RANDOM_VAL        BUDDY_VIR_REG_0
#define BUDDY_VIR_REG_STACK_TOP         BUDDY_VIR_REG_1
#define BUDDY_VIR_REG_BUDDY_INFO        BUDDY_VIR_REG_2
//#define BUDDY_VIR_REG_STACK_SIZE        BUDDY_VIR_REG_3




struct Buddy_TLS_Info{
    long random_val;            // random value for runtime rerandomization
    void *stack_top;			// page aligned
    struct Buddy_TLS_Info *buddy_info;	// pointer to our buddy
    long is_randomizing;
    //long rand_cnt;
    long cpu_cycles;
    //unsigned long stack_size;   // stack size, aligned to power(2,n)
    //void *data;                 //
};



//
struct Buddy_TLS_Info *  buddy_get_tls_info(void);
//
void buddy_print_tls_info(struct Buddy_TLS_Info *p_info);
//
void buddy_init_main_thread_tls(void);
//
void buddy_init_other_thread_tls(void);
//
void buddy_init_rt_lib_hooker(void);

//////////////////////////////////////////////////////////////////////////////////////

//
unsigned long spa_get_cur_time_us(void);

//
//unsigned long avx2_64_x_4_add(unsigned long * ss_ptr, unsigned long * ss_end, long x);
//
//unsigned long do_au_edu_unsw_randomize(
//                                 struct Buddy_TLS_Info * tls_info,
//                                 struct Buddy_TLS_Info * buddy_info,
//                                 unsigned long call_stack_size,
//                                 long cnt,
//                                 long cycles);

//void au_edu_unsw_randomize(const char *func_name, unsigned long rsp, long cycles);

//void avx2_64_x_4_add(unsigned long * ss_ptr, unsigned long * ss_end, long x);

void au_edu_unsw_randomize(struct Buddy_TLS_Info * tls_info, long cycles);

#ifdef __cplusplus
}
#endif

#endif // SPA_H
