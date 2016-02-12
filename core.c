#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "hardware.h"
#include "core.h"


void printbide(){
	printf("printit\n");
}
void empty_it(){
}

void init_core_et_un(){
	init_hardware("core.ini");
	int i;
	for(i=0;i<16;i++)
		IRQVECTOR[i]= empty_it;

	IRQVECTOR[0]= printbide;
	_out(CORE_STATUS,2);
}

int main(){
	init_core_et_un();
	while(1);
	return 0;
}
