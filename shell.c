#include <stdlib.h>
#include <stdio.h>
#include "shell.h"

#define BUFFER_SIZE 128

struct cmd_s* available_cmds;

void shell (){
	init_cmds();
	char * buffer;	
	buffer = malloc(BUFFER_SIZE * sizeof(char));
	while(1){
		wait_for_cmd(buffer);
	}
}

void wait_for_cmd(char * buffer){
	printf("\n> ");
	fgets(buffer, BUFFER_SIZE, STDIN_FILENO);
	parse_cmd(buffer);
}

void parse_cmd(char* buffer){
	char** words;
	words = str_split(buffer,' ');
}



init_cmds(){
	available_cmds = {
		{"ps",ps,"Displays current processes"},
		{"ls",ls,"Displays current directory entries"},
		{"cat",cat,"Display files given as arguments"},
		{"compute",compute, "Calls a function that requires CPU time"}
	};
}
