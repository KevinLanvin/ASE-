#define CTX_MAGIC 0xCAFEBABE
typedef void (funct_t) (void*);

int ctx_names(char** name);
struct timeval* ctx_time(char* name);

void create_ctx(int stack_size,funct_t f,void * arg,char* name);
void start_sched();
