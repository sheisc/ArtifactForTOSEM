#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <cctype>
#include <map>
// gettid()
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>
// gettimeofday
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#include <signal.h>

#include "pin.H"



using std::cerr;
using std::string;
using std::endl;


// Virtual register we use to point to each thread's TINFO structure.
//
static REG reg_t_info;


#define  DEF_PIN_NAME       "PIN"

#define  PRINT_TO_STD_ERR
// \x43\x53\x45\x40\x55\x4e\x53\x57,  "CSE@UNSW"
#define SPA_GLOBAL_STACK_SIZE_MAGIC_NUM     (0x57534E5540455343L)

// Information about each thread.
//
struct TINFO{
    TINFO(ADDRINT base, THREADID tid) : _stackBase(base), _max(0), _tid(tid), _cur_depth(0), _max_depth(0) {
        _magic_num = SPA_GLOBAL_STACK_SIZE_MAGIC_NUM;
    }
    ADDRINT _stackBase;         // Base (highest address) of stack.
    size_t _max;                // Maximum stack usage so far.
    std::ostringstream _os;     // Used to format messages.
    THREADID _tid;              // thread id.
    unsigned long _cur_depth;   // current call stack depth
    unsigned long _max_depth;   // maximum call stack depth so far.
    unsigned long _magic_num;
};

typedef std::map<THREADID, TINFO *> TINFO_MAP;
static TINFO_MAP thread_infos;
static PIN_LOCK pinLock;
static std::string app_name;
static BOOL is_nginx;

static VOID on_thread_start(THREADID, CONTEXT *, INT32, VOID *);
static VOID on_thread_end(THREADID, const CONTEXT *, INT32, VOID *);
static VOID do_instruction(INS, VOID *);
static VOID handle_a_ret(ADDRINT, ADDRINT);
static VOID handle_a_call(ADDRINT);
static unsigned long get_stack_base(TINFO *tinfo, unsigned long rsp);
static std::string get_program_name_by_pid(pid_t pid);
static VOID on_stack_change(ADDRINT sp, ADDRINT addrInfo);
///////////////////////////////////////////////////////////////////////////////////////////


unsigned long spa_get_cur_time_us(void) {
    struct timeval tv;
    struct timezone tz;

    gettimeofday(&tv, &tz);

    return (tv.tv_sec * 1000000UL) + tv.tv_usec;
}



INT32 usage(){
    cerr << "This tool records the size of call stack of each thread." << endl;
    return -1;
}


// pin  -t ./obj-intel64/MyPinTool.so -- ~/test/spa/TLS2/main
// return "~/test/spa/TLS2/main"
std::string get_program_name(int argc, char *argv[]){
    string name = DEF_PIN_NAME;
    for(int i = 1; i < argc; i++){
        string cur_arg = argv[i];
        if(cur_arg == "--" && (i + 1) < argc){
            name = argv[i+1];
            break;
        }
    }
    return name;
}

static std::string get_program_name_by_pid(pid_t pid){
    // read the /proc/pid/cmdline
    //pid_t pid = getpid();
    std::stringstream ss;
    ss << "/proc/" << pid << "/cmdline";
    std::string name = ss.str();
    std::ifstream proc_cmdline(name.c_str());
    std::string line = DEF_PIN_NAME;
    std::getline(proc_cmdline,line);
    return line;
}




static VOID on_thread_start(THREADID tid, CONTEXT *ctxt, INT32, VOID *){       
    TINFO *tinfo = new TINFO(PIN_GetContextReg(ctxt, REG_STACK_PTR), tid);
    PIN_GetLock(&pinLock, tid);
    thread_infos.insert(std::make_pair(tid, tinfo));
    PIN_ReleaseLock(&pinLock);
    PIN_SetContextReg(ctxt, reg_t_info, reinterpret_cast<ADDRINT>(tinfo));
}


static VOID save_one_call_stack_info(TINFO * tinfo){
    pid_t pid = getpid();
    pid_t tid = syscall(SYS_gettid);
    tinfo->_os.str("");
    tinfo->_os << "SPA.call.stack."
               << pid << "."
               << tid << "."
               << tinfo->_tid
               << ".txt";
    std::string name = tinfo->_os.str();
    std::ofstream tid_out(name.c_str());

    if(app_name == DEF_PIN_NAME){
        app_name = get_program_name_by_pid(pid);
    }

    tid_out << pid << " "
            << tid << " "
            << tinfo->_tid << " "
            << tinfo->_max << " "
            << tinfo->_max_depth << " "
            << app_name << std::endl;
    tid_out << std::flush;
    tid_out.close();
}

//VOID save_thread_id(){
//    pid_t pid = getpid();
//    pid_t tid = syscall(SYS_gettid);
//    usleep(1);
//    std::stringstream ss;
//    ss << "SPA.thread.id."
//               << pid << "."
//               << tid << "."
//               << spa_get_cur_time_us() << "."
//               << ".txt";
//    std::ofstream tid_out(ss.str().c_str());

//    if(app_name == DEF_PIN_NAME){
//        app_name = get_program_name_by_pid(pid);
//    }

//    tid_out << pid << " "
//            << tid << " "
//            << app_name << std::endl;
//    tid_out << std::flush;
//    tid_out.close();
//}

static VOID on_thread_end(THREADID tid, const CONTEXT *ctxt, INT32, VOID *){
    PIN_GetLock(&pinLock, tid);
    TINFO_MAP::iterator it = thread_infos.find(tid);
    if (it != thread_infos.end()) {
        TINFO * tinfo = it->second;
        save_one_call_stack_info(tinfo);
//        delete it->second;
//        thread_infos.erase(it);
    // As for Apache Httpd, the output is in httpd-orig.llvm7.0/install.bin/logs/error_log.
    // FIXME: MUTEX needed
#if defined(PRINT_TO_STD_ERR)
        std::cerr << getpid() << " "
                << syscall(SYS_gettid) << " "
                << tinfo->_tid << " "
                << tinfo->_max << " "
                << tinfo->_max_depth << " "
                << app_name << std::endl;
#endif
    }
    PIN_ReleaseLock(&pinLock);
}

static VOID on_stack_change(ADDRINT sp, ADDRINT addrInfo){
    TINFO *tinfo = reinterpret_cast<TINFO *>(addrInfo);
//    if(tinfo->_magic_num != SPA_GLOBAL_STACK_SIZE_MAGIC_NUM){
//        return;
//    }
    // It happens when PIN is attached to a running process,
    // we have not recorded its stack base yet.
    if(tinfo->_stackBase == 0){
        tinfo->_stackBase = get_stack_base(tinfo, sp);
    }

    // The stack pointer may go above the base slightly.  (For example, the application's dynamic
    // loader does this briefly during start-up.)
    //
    if (sp > tinfo->_stackBase)
        return;

    // Keep track of the maximum stack usage.
    //
    size_t size = tinfo->_stackBase - sp;
    if (size > tinfo->_max){
        tinfo->_max = size;
        save_one_call_stack_info(tinfo);
    }
}

static VOID do_instruction(INS ins, VOID *){
    if(INS_IsRet(ins)){
        INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)handle_a_ret,
                       IARG_REG_VALUE, REG_STACK_PTR,
                       IARG_REG_VALUE, reg_t_info, IARG_END);
    }
    else if(INS_IsCall(ins)){
        INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)handle_a_call,
                       IARG_REG_VALUE, reg_t_info, IARG_END);
    }
    else if (INS_RegWContain(ins, REG_STACK_PTR)){
        if (INS_IsSysenter(ins)) return; // no need to instrument system calls

        IPOINT where = IPOINT_AFTER;
        if (!INS_IsValidForIpointAfter(ins)){
            if (INS_IsValidForIpointTakenBranch(ins)){
                where = IPOINT_TAKEN_BRANCH;
            }
            else{
                return;
            }
        }
        INS_InsertCall(ins, where, (AFUNPTR)on_stack_change,
                       IARG_REG_VALUE, REG_STACK_PTR,
                       IARG_REG_VALUE, reg_t_info, IARG_END);
    }
}



static unsigned long get_stack_base(TINFO *tinfo, unsigned long rsp){
    // read the /proc/pid/maps
    pid_t pid = getpid();
    tinfo->_os.str("");
    tinfo->_os << "/proc/" << pid << "/maps";
    std::string name = tinfo->_os.str();
    std::ifstream proc_maps(name.c_str());
    std::string line;

    while(std::getline(proc_maps,line)){
        unsigned long low_addr = 0, high_addr = 0;
        // 561496557000-561498187000 r--p 00000000 08:15 9839457  /opt/google/chrome/chrome
        std::string low_high = line.substr(0, line.find(" "));
        std::size_t pos = low_high.find("-");
        std::string low_str = "0x" + low_high.substr(0, pos);
        std::string high_str = "0x" + low_high.substr(pos + 1);
        std::stringstream ss(low_str + " " + high_str);
        ss >> std::hex >> low_addr >> high_addr;
        if(low_addr < rsp && rsp < high_addr){
            return high_addr;
        }

    }
    return 0;
}


static VOID handle_a_call(ADDRINT addrInfo){
    TINFO *tinfo = reinterpret_cast<TINFO *>(addrInfo);
    tinfo->_cur_depth++;
    if(tinfo->_cur_depth > tinfo->_max_depth){
        tinfo->_max_depth = tinfo->_cur_depth;
        if(is_nginx){ // Workaroud for nginx
            save_one_call_stack_info(tinfo);
        }
    }
}

/*
 * Update the size of call stack at a return instruction.
 *
 *  sp[in]          Value of the stack pointer (after it is updated).
 *  addrInfo[in]    Address of the thread's TINFO structure.
 *
 *
 */
static VOID handle_a_ret(ADDRINT sp, ADDRINT addrInfo){
    TINFO *tinfo = reinterpret_cast<TINFO *>(addrInfo);
    //
    if(tinfo->_cur_depth > 0){
        tinfo->_cur_depth--;
    }
}


//static void OnExit(INT32 code, VOID *v){
//    save_thread_id();
//}

int main(int argc, char *argv[]){
    // Initialize symbol table code.
    // Pin does not read symbols unless this is called.
    PIN_InitSymbols();
    // Initialize Pin system. Must be called before PIN_StartProgram
    if (PIN_Init(argc, argv)){
        return usage();
    }

    PIN_InitLock(&pinLock);

    app_name = get_program_name(argc, argv);

    if(app_name.find("nginx") != string::npos){
        is_nginx = TRUE;
    }

    // Allocate a virtual register that each thread uses to point to its
    // TINFO data.  Threads can use this virtual register to quickly access
    // their own thread-private data.
    //
    reg_t_info = PIN_ClaimToolRegister();
    if (!REG_valid(reg_t_info)){
        std::cerr << "Cannot allocate a scratch register.\n";
        std::cerr << std::flush;
        return 1;
    }

    // Register a notification function that is called when a thread starts executing in the application.
    // The call-back happens even for the application's root (initial) thread.
    PIN_AddThreadStartFunction(on_thread_start, 0);
    // Register a notification function that is called when an application thread terminates.
    PIN_AddThreadFiniFunction(on_thread_end, 0);
    // Add a function used to instrument at instruction granularity
    INS_AddInstrumentFunction(do_instruction, 0);

    //PIN_AddFiniFunction(OnExit, 0);
    // Starts executing the application, when Pin is in JIT mode, which is the default.
    // Note that PIN_Init() must be called before PIN_StartProgram().
    PIN_StartProgram();
    return 0;
}
