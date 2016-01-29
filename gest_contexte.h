#define CTX_MAGIC 0xCAFEBABE
typedef void (funct_t) (void*);

enum ctx_state_e{
CTX_INIT,
CTX_EXQ,
CTX_BLOCKED,
CTX_END
};


struct ctx_s{
	char * ctx_base; /* adr de départ */
	int ctx_pid;
	char* ctx_name;
	void * ctx_esp;
	void * ctx_ebp; /* enregistrement des registres de la frame du contexte */
	funct_t *  ctx_f; /* enregistrement de la fonction */
	void * ctx_arg; /* enregistrement de l'argument de la fonction f */ 
	enum ctx_state_e ctx_state; /* état du contexte */
	unsigned int ctx_magic; /* vérifie le contexte */
	struct timeval *ctx_wake_time;
	struct timeval *ctx_time_spent;
	struct ctx_s * ctx_next;
	struct ctx_s* ctx_nextBlocked;
};

struct sem_s {
	struct ctx_s * ctx_blocked_list;
	int ctx_cpt;
};


int ctx_names(char** name,int* pid);
struct timeval* ctx_time(int pid);

void load_first_context();
void create_ctx(int stack_size,funct_t f,void * arg,char* name);
void start_sched();

void sem_init(struct sem_s* sem, unsigned int val);
void sem_down (struct sem_s * sem);
void sem_up (struct sem_s * sem);

