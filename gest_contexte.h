#define CTX_MAGIC 0xCAFEBABE
typedef void (funct_t) (void*);

int ctx_names(char** name,int* pid);
struct timeval* ctx_time(int pid);

void create_ctx(int stack_size,funct_t f,void * arg,char* name);
void start_sched();
