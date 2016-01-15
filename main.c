#include <stdlib.h>
#include <stdio.h>
#include "gest_contexte.h"
#include "hardware.h"
#include "hw_config.h"


/*déclaration des fonctions*/
void f_ping(void * arg);
void f_pong(void * arg);
void f_pang(void * arg);
void f_pung(void * arg);

int main(){
	create_ctx(16384,f_ping,NULL);
	create_ctx(16384,f_pong,NULL);
	create_ctx(16384,f_pang,NULL);
	start_sched();

	exit(EXIT_SUCCESS);
}


void f_ping(void * args){
	while(1){
		printf("ping\n");
	}
}
void f_pong(void * args){
	while(1){
		printf("pong\n");
	}
}
void f_pang(void * args){
	while(1){
		printf("pang\n");
	}
create_ctx(16384,f_pung,NULL);
}
void f_pung(void * args){
	while(1){
		printf("coucou\n");
	}
}


