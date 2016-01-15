#define CTX_MAGIC 0xCAFEBABE
typedef void (funct_t) (void*);

void create_ctx(int stack_size,funct_t f,void * arg);
void yield();
