#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include "gest_contexte.h"
#include "shell.h"
/*#include "drive.h"*/

#define BUFFER_SIZE 	128
#define NB_CMD 		5

struct cmd_s available_cmds[NB_CMD]= {
		{.cmd_name="ps",.function=ps,.help="Displays current processes"},
		{.cmd_name="format",.function=format_all,.help="Format all disk"},
		{.cmd_name="ls",.function=ls,.help="Displays current directory entries"},
		{.cmd_name="cat",.function=cat,.help="Display files given as arguments"},
		{.cmd_name="compute",.function=compute,.help="Calls a function that requires CPU time"}
};

int fibo(int a,int b);
void wait_for_cmd(char * buffer);
void parse_cmd(char* buffer);


void shell(void* arg){
	char * buffer;	
	buffer = malloc(BUFFER_SIZE * sizeof(char));
	while(1){
		wait_for_cmd(buffer);
	}
}

void wait_for_cmd(char * buffer){
	printf("\n> ");
	fgets(buffer, BUFFER_SIZE,stdin);
	parse_cmd(buffer);
}

void parse_cmd(char* buffer){
	int i;
	void* arg =NULL;
	for(i=0;i<NB_CMD;i++)
		if(!strncmp(buffer,available_cmds[i].cmd_name,strlen(available_cmds[i].cmd_name))){
			if(buffer[strlen(buffer)-2]=='&')
				create_ctx(16384,available_cmds[i].function,NULL,available_cmds[i].cmd_name);
			else
				available_cmds[i].function(arg);
		}
}



/* definitions des commandes */
void ps(void* arg){
	char** name= malloc(sizeof(char*)*128);
	int* pids = malloc(sizeof(int)*128);
	int nb_ctx = ctx_names(name,pids);
	int i;
	struct timeval* t;
	printf("Processes (%d) :\n",nb_ctx);
	for(i=0;i<nb_ctx;i++){
		t=ctx_time(pids[i]);
		printf("\t- %10s (%d) %8dus\n",name[i],pids[i],((int)t->tv_sec*1000000) + (int)t->tv_usec);
	}
	free(name);
	free(pids);
}
void ls(){
}
void cat(){
}

void format_all(){
int i;
int j;
	for(i=0;i<16;i++)
		for(j=0;j<16;j++)
			printf("format sector %d, cylindre %d\n",j,i);/*format_sector(i,j);*/
}
void compute(){
	int i;
	for(i=0;i<10000000;++i)
		fibo(0,1);
	ps(NULL);
}

int fibo(int a,int b){
	return (a>100000000) ?  b : fibo(b,a+b);
}
