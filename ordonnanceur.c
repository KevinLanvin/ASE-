#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <assert.h>
#include "volumes.h"
#include "ordonnanceur.h"
#include "hardware.h"
#include "hw_config.h"


void start_current_ctx();
int init_ctx(struct ctx_s * ctx, int size_stack, funct_t f, void * arg,char* name);
void switch_to_ctx(struct ctx_s * ctx,int id);
void irq_handler();
void init_rings_and_curr_ctx();
static void timer_it();

int next_pid=0;

/* sauvegarde du contexte courant*/
static struct ctx_s* current_ctx[CORE_NCORE];

/*contexte de départ de l'anneau*/
static struct ctx_s * ctx_ring[CORE_NCORE];


void create_ctx(int stack_size, funct_t f, void * arg,char* name,int core){
	/*création du contexte courant dans l'anneau*/
	struct ctx_s * ctx_new = malloc(sizeof(struct ctx_s));
	assert(ctx_new);
	_mask(15);
	/*initialisation du contexte courant*/
	init_ctx(ctx_new,stack_size,f,arg,name);
	next_pid++;
	/*si le ring n'est pas initialisé*/
	if(ctx_ring[core]){
		ctx_new->ctx_next=ctx_ring[core]->ctx_next;
		ctx_ring[core]->ctx_next=ctx_new;
	}
	else {
		ctx_new->ctx_next=ctx_ring[core]=ctx_new;
		current_ctx[core]=ctx_ring[core];
	}
	_mask(1);
}

void yield(int id){
	if(current_ctx[id])
		switch_to_ctx(current_ctx[id]->ctx_next,id);
	else if(ctx_ring[id])
		switch_to_ctx(ctx_ring[id],id);
	else
		return;
}


int init_ctx(struct ctx_s * ctx, int size_stack, funct_t f, void * arg,char* name){
	ctx->ctx_base=(char*)malloc(size_stack);
	ctx->ctx_pid=next_pid;
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
	ctx -> ctx_nextBlocked = NULL;
	return 0;
}

void switch_to_ctx(struct ctx_s * ctx,int id){
	/* vérification du contexte valide */
	assert(ctx->ctx_magic==CTX_MAGIC);

	_mask(15);
	if(ctx->ctx_state==CTX_END || ctx->ctx_state==CTX_BLOCKED){
		if(ctx==ctx_ring[id])
			ctx_ring[id]->ctx_next=ctx->ctx_next;
		if(ctx==current_ctx[id])
			exit(EXIT_FAILURE);
		
	/*libérer délivrer la mémoire*/
		free(ctx->ctx_base);
		free(ctx->ctx_wake_time);
		free(ctx->ctx_time_spent);
		ctx->ctx_base=NULL;
		current_ctx[id]->ctx_next=ctx->ctx_next;
		free(ctx);
		ctx=NULL;
		switch_to_ctx(current_ctx[id]->ctx_next,id);
		return;
	}

	if(current_ctx[id]){
struct timeval *tv = malloc(sizeof(struct timeval));
		/* sauvegarde du contexte courant*/
	asm(	"movl %%esp, %0 \n"
		:"=r"(current_ctx[id]->ctx_esp)
		:
		:"%esp");
	asm(	"movl %%ebp, %0 \n"
		:"=r"(current_ctx[id]->ctx_ebp)
		:
		:);
    gettimeofday(tv,NULL);
    (current_ctx[id] -> ctx_time_spent) -> tv_sec +=
      (tv -> tv_sec) - ((current_ctx[id] -> ctx_wake_time) -> tv_sec);
    (current_ctx[id] -> ctx_time_spent) -> tv_usec +=
      (tv -> tv_usec) - ((current_ctx[id] -> ctx_wake_time) -> tv_usec);
	}
	current_ctx[id]=ctx;
	/*changement du contexte courrant*/
	gettimeofday(current_ctx[id] -> ctx_wake_time, NULL);
	asm(	"movl %0, %%esp \n"
		:
		:"r"(current_ctx[id]->ctx_esp)
		:"%esp");
	asm(	"movl %0, %%ebp \n"
		:
		:"r"(current_ctx[id]->ctx_ebp)
		:);
	_mask(1);

	if(current_ctx[id]->ctx_state==CTX_INIT)
		start_current_ctx(id);
}

void start_current_ctx(int id){
	/*on change l'état du ctx courrant en execution*/
	current_ctx[id]->ctx_state=CTX_EXQ;
	/*on lance la fonction du ctx courrant*/
	current_ctx[id]->ctx_f(current_ctx[id]->ctx_arg);
	/*on change l'état du ctx courrant terminé*/
	current_ctx[id]->ctx_state=CTX_END;
	yield(id);
}


void start_sched(){
    	/* program timer */
	IRQVECTOR[TIMER_IRQ] = timer_it;    
	_out(TIMER_PARAM,128+64+32+8); /* reset + alarm on + 8 tick / alarm */
	_out(TIMER_ALARM,0xFFFFFFFE);  /* alarm at next tick (at 0xFFFFFFFF) */
	yield(_in(CORE_ID));
}

static void
timer_it() {
	_out(TIMER_ALARM,0xFFFFFFFE);
	yield(_in(CORE_ID));
}
