#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "gest_contexte.h"

enum ctx_state_e{
CTX_INIT,
CTX_EXQ,
CTX_END,
};

struct ctx_s{
char * ctx_base; /* adr de départ */
void * ctx_esp;
void * ctx_ebp; /* enregistrement des registres de la frame du contexte */
funct_t *  ctx_f; /* enregistrement de la fonction */
void * ctx_arg; /* enregistrement de l'argument de la fonction f */ 
enum ctx_state_e ctx_state; /* état du contexte */
unsigned int ctx_magic; /* vérifie le contexte */
struct ctx_s * ctx_next;
};

void start_current_ctx();
int init_ctx(struct ctx_s * ctx, int size_stack, funct_t f, void * arg);
void switch_to_ctx(struct ctx_s * ctx);


// sauvegarde du contexte courant
static struct ctx_s* current_ctx = (struct ctx_s*) 0;

//contexte de départ de l'anneau
static struct ctx_s * ctx_ring;



void create_ctx(int stack_size, funct_t f, void * arg){
	//création du contexte courant dans l'anneau
	struct ctx_s * ctx_new = malloc(sizeof(struct ctx_s));
	assert(ctx_new);
	//initialisation du contexte courant
	init_ctx(ctx_new,stack_size,f,arg);

	//si le ring n'est pas initialisé
	if(ctx_ring){
		ctx_new->ctx_next=ctx_ring->ctx_next;
		ctx_ring->ctx_next=ctx_new;
	}
	else
		ctx_new->ctx_next=ctx_ring=ctx_new;

}

void yield(){
	if(current_ctx)
		switch_to_ctx(current_ctx->ctx_next);
	else if(ctx_ring)
		switch_to_ctx(ctx_ring);
	else
		return;
}





int init_ctx(struct ctx_s * ctx, int size_stack, funct_t f, void * arg){
	ctx->ctx_base=(char*)malloc(size_stack);
	ctx->ctx_esp=ctx->ctx_base+size_stack - sizeof(int);
	ctx->ctx_ebp=ctx->ctx_base+size_stack - sizeof(int);
	ctx->ctx_f=f;
	ctx->ctx_arg= arg;
	ctx->ctx_state=CTX_INIT;
	ctx->ctx_magic=CTX_MAGIC;
	return 0;
}

void switch_to_ctx(struct ctx_s * ctx){
	// vérification du contexte valide 
	assert(ctx->ctx_magic==CTX_MAGIC);

	if(ctx->ctx_state==CTX_END){
		if(ctx==ctx_ring)
			ctx_ring->ctx_next=ctx->ctx_next;
		if(ctx==current_ctx)
			exit(EXIT_SUCCESS);
	//libérer délivrer la mémoire
		free(ctx->ctx_base);
		ctx->ctx_base=NULL;
		current_ctx->ctx_next=ctx->ctx_next;
		free(ctx);
		ctx=NULL;
		switch_to_ctx(current_ctx->ctx_next);
		return;
	}

	if(current_ctx){

		// sauvegarde du contexte courant
	asm(	"movl %%esp, %0 \n"
		:"=r"(current_ctx->ctx_esp)
		:
		:"%esp");
	asm(	"movl %%ebp, %0 \n"
		:"=r"(current_ctx->ctx_ebp)
		:
		:);
	}
	current_ctx=ctx;
	//changement du contexte courrant
	asm(	"movl %0, %%esp \n"
		:
		:"r"(current_ctx->ctx_esp)
		:"%esp");
	asm(	"movl %0, %%ebp \n"
		:
		:"r"(current_ctx->ctx_ebp)
		:);

	if(current_ctx->ctx_state==CTX_INIT)
		start_current_ctx();
}

void start_current_ctx(){
	//on change l'état du ctx courrant en execution
	current_ctx->ctx_state=CTX_EXQ;
	//on lance la fonction du ctx courrant
	current_ctx->ctx_f(current_ctx->ctx_arg);
	//on change l'état du ctx courrant terminé
	current_ctx->ctx_state=CTX_END;
	yield();
}

