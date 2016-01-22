#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <assert.h>
#include "gest_contexte.h"
#include "hardware.h"
#include "hw_config.h"

enum ctx_state_e{
CTX_INIT,
CTX_EXQ,
CTX_END
};

struct ctx_s{
char * ctx_base; /* adr de départ */
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
};

void start_current_ctx();
int init_ctx(struct ctx_s * ctx, int size_stack, funct_t f, void * arg,char* name);
void switch_to_ctx(struct ctx_s * ctx);
void irq_handler();
static void empty_it(void);
static void timer_it();


/* sauvegarde du contexte courant*/
static struct ctx_s* current_ctx = (struct ctx_s*) 0;

/*contexte de départ de l'anneau*/
static struct ctx_s * ctx_ring;

int ctx_names(char** name){

	name[0]=ctx_ring->ctx_name;
	struct ctx_s * cur=ctx_ring->ctx_next;
	int i=1;
	while(cur!=ctx_ring){
		name[i]=cur->ctx_name;
		i++;
		cur= cur->ctx_next;
	}
	return i;
}

struct timeval* ctx_time(char* name){
	if(!strcmp(ctx_ring->ctx_name,name)){
		return ctx_ring->ctx_time_spent;
	}
	struct ctx_s * cur=ctx_ring->ctx_next;
	while(cur!=ctx_ring && strcmp(cur->ctx_name,name)){
	printf("%s",cur->ctx_name);
		cur=cur->ctx_next;
	}
	if(!strcmp(cur->ctx_name,name))
		return cur->ctx_time_spent;
	exit(EXIT_FAILURE);
}


void create_ctx(int stack_size, funct_t f, void * arg,char* name){
	/*création du contexte courant dans l'anneau*/
	struct ctx_s * ctx_new = malloc(sizeof(struct ctx_s));
	assert(ctx_new);
	_mask(15);
	/*initialisation du contexte courant*/
	init_ctx(ctx_new,stack_size,f,arg,name);

	/*si le ring n'est pas initialisé*/
	if(ctx_ring){
		ctx_new->ctx_next=ctx_ring->ctx_next;
		ctx_ring->ctx_next=ctx_new;
	}
	else
		ctx_new->ctx_next=ctx_ring=ctx_new;
	_mask(1);
}

void yield(){
	if(current_ctx)
		switch_to_ctx(current_ctx->ctx_next);
	else if(ctx_ring)
		switch_to_ctx(ctx_ring);
	else
		return;
}


int init_ctx(struct ctx_s * ctx, int size_stack, funct_t f, void * arg,char* name){
	ctx->ctx_base=(char*)malloc(size_stack);
	ctx->ctx_name=name;
	ctx->ctx_esp=ctx->ctx_base+size_stack - sizeof(int);
	ctx->ctx_ebp=ctx->ctx_base+size_stack - sizeof(int);
	ctx->ctx_f=f;
	ctx->ctx_arg= arg;
	ctx->ctx_state=CTX_INIT;
	ctx->ctx_magic=CTX_MAGIC;
	ctx -> ctx_wake_time = malloc(sizeof(struct timeval));
	ctx -> ctx_time_spent = malloc(sizeof(struct timeval));
	(ctx -> ctx_time_spent) -> tv_sec = 0 ;
	(ctx -> ctx_time_spent) -> tv_usec = 0;
	return 0;
}

void switch_to_ctx(struct ctx_s * ctx){
	/* vérification du contexte valide */
	assert(ctx->ctx_magic==CTX_MAGIC);

	_mask(15);
	if(ctx->ctx_state==CTX_END){
		if(ctx==ctx_ring)
			ctx_ring->ctx_next=ctx->ctx_next;
		if(ctx==current_ctx)
			exit(EXIT_SUCCESS);
	/*libérer délivrer la mémoire*/
		free(ctx->ctx_base);
		free(ctx->ctx_wake_time);
		free(ctx->ctx_time_spent);
		ctx->ctx_base=NULL;
		current_ctx->ctx_next=ctx->ctx_next;
		free(ctx);
		ctx=NULL;
		switch_to_ctx(current_ctx->ctx_next);
		return;
	}

	if(current_ctx){
struct timeval *tv = malloc(sizeof(struct timeval));
		/* sauvegarde du contexte courant*/
	asm(	"movl %%esp, %0 \n"
		:"=r"(current_ctx->ctx_esp)
		:
		:"%esp");
	asm(	"movl %%ebp, %0 \n"
		:"=r"(current_ctx->ctx_ebp)
		:
		:);
    gettimeofday(tv,NULL);
    (current_ctx -> ctx_time_spent) -> tv_sec +=
      (tv -> tv_sec) - ((current_ctx -> ctx_wake_time) -> tv_sec);
    (current_ctx -> ctx_time_spent) -> tv_usec +=
      (tv -> tv_usec) - ((current_ctx -> ctx_wake_time) -> tv_usec);
	}
	current_ctx=ctx;
	/*changement du contexte courrant*/
	gettimeofday(current_ctx -> ctx_wake_time, NULL);
	asm(	"movl %0, %%esp \n"
		:
		:"r"(current_ctx->ctx_esp)
		:"%esp");
	asm(	"movl %0, %%ebp \n"
		:
		:"r"(current_ctx->ctx_ebp)
		:);
	_mask(1);

	if(current_ctx->ctx_state==CTX_INIT)
		start_current_ctx();
}

void start_current_ctx(){
	/*on change l'état du ctx courrant en execution*/
	current_ctx->ctx_state=CTX_EXQ;
	/*on lance la fonction du ctx courrant*/
	current_ctx->ctx_f(current_ctx->ctx_arg);
	/*on change l'état du ctx courrant terminé*/
	current_ctx->ctx_state=CTX_END;
	yield();
}

void start_sched(){
    unsigned int i;
    
    /* init hardware */
    if (init_hardware(HARDWARE_INI) == 0) {
	fprintf(stderr, "Error in hardware initialization\n");
	exit(EXIT_FAILURE);
    }
    
    /* dummy interrupt handlers */
    for (i=0; i<16; i++)
	IRQVECTOR[i] = empty_it;

    /* program timer */
    IRQVECTOR[TIMER_IRQ] = timer_it;    
	_out(TIMER_PARAM,128+64+32+8); /* reset + alarm on + 8 tick / alarm */
	_out(TIMER_ALARM,0xFFFFFFFE);  /* alarm at next tick (at 0xFFFFFFFF) */
	yield();
}

static void
timer_it() {
	_out(TIMER_ALARM,0xFFFFFFFE);
	yield();
}

static void
empty_it(void)
{
    return;
}

