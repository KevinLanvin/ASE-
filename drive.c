#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "gest_contexte.h"
#include "hardware.h"
#include "drive.h"
typedef void (funct_irq) ();

struct sem_s mutex_disk ,mutex_buff_cpy;
funct_irq* irq_funct;


//void write(const unsigned char* buffer, int size);
//void read(unsigned char* buffer,int size);
void format();
//void seek(unsigned int c, unsigned int s, const unsigned char* buffer, int size, unsigned char car);
void seek_format(unsigned int c, unsigned int s);
void consume_next();

void dump(unsigned char *buffer, unsigned int buffer_size,int ascii_dump,int octal_dump)
{
    unsigned int i,j;
    
    for (i=0; i<buffer_size; i+=16) {
	/* offset */
	printf("%.8o",i);

	/* octal dump */
	if (octal_dump) {
	    for(j=0; j<8; j++)
		printf(" %.2x", buffer[i+j]);
	    printf(" - ");
	    
	    for( ; j<16; j++)
		printf(" %.2x", buffer[i+j]);
	    
	    printf("\n");
	}
	/* ascii dump */
	if (ascii_dump) {
	    printf("%8c", ' ');
	    
	    for(j=0; j<8; j++)
		printf(" %1c ", isprint(buffer[i+j])?buffer[i+j]:' ');
	    printf(" - ");
	    
	    for( ; j<16; j++)
		printf(" %1c ", isprint(buffer[i+j])?buffer[i+j]:' ');
	    
	    printf("\n");
	}
    }
}

/* place un secteur dans MASTERBUFFER */
void read_sector_n(unsigned int c, unsigned int s,unsigned char* buffer,int size){
	assert(size<=SECTORSIZE);
	sem_down(&mutex_disk);
	/* deplacer la tête de lecture */
	seek(c,s,buffer,size,'r');

	/* lire sur le secteur */
	sem_down(&mutex_buff_cpy);
	sem_up(&mutex_disk);  
}

void read_sector(unsigned int c, unsigned int s,unsigned char* buffer){
	read_sector_n(c,s,buffer,SECTORSIZE);
}


/* écrit un secteur dans MASTERBUFFER */
void write_sector_n(unsigned int c, unsigned int s, const unsigned char* buffer, int size){
	assert(size <= SECTORSIZE);
	sem_down(&mutex_disk);
	/* deplacer la tête de lecture */
	seek(c,s,buffer,size,'w');

	/* écrit sur le secteur */
	sem_down(&mutex_buff_cpy);
	sem_up(&mutex_disk);  
}

void write_sector(unsigned int c, unsigned int s, const unsigned char* buffer){
	write_sector_n(c,s,buffer,strlen((char*)buffer));
}


void format_sector(unsigned int c, unsigned int s){
	sem_down(&mutex_disk);
printf("debut frmt\n");
	/* deplacer la tête de lecture */
	seek_format(c,s);
printf("fin frmt\n");
	/* attente de la tete de lecture */
	sem_down(&mutex_buff_cpy);
	/* écrit sur le secteur */
	printf("salut\n");
	sem_up(&mutex_disk);  
}


void write(const unsigned char* buffer, int size){
	_out(HDA_DATAREG,0);
	_out(HDA_DATAREG+1,1);
	memcpy(MASTERBUFFER,buffer,size);
	_out(HDA_CMDREG,CMD_WRITE);
	sem_up(&mutex_buff_cpy);	
}

void read(unsigned char* buffer,int size){
	_out(HDA_DATAREG,0);
	_out(HDA_DATAREG+1,1);
	_out(HDA_CMDREG,CMD_READ);
	memcpy(buffer,MASTERBUFFER,size);
	sem_up(&mutex_buff_cpy);
}

void format(){
int v=0;
printf("bonjours\n");
	/* nbSec(int16) */
	_out(HDA_DATAREG,((1>>8)&0xff));
	_out(HDA_DATAREG+1,(1&0xff));
	/* val (int32) */
	_out(HDA_DATAREG+2,((v>>24)&0xff));
	_out(HDA_DATAREG+3,((v>>16)&0xff));
	_out(HDA_DATAREG+4,((v>>8)&0xff));
	_out(HDA_DATAREG+5,(v&0xff));
	_out(HDA_CMDREG,CMD_FORMAT);
	sem_up(&mutex_buff_cpy);
}

void seek_format(unsigned int c, unsigned int s){
	_out(HDA_DATAREG,((c>>8)&0xff));
	_out(HDA_DATAREG+1,(c&0xff));
	_out(HDA_DATAREG+2,((s>>8)&0xff));
	_out(HDA_DATAREG+3,(s&0xff));
	_out(HDA_CMDREG,CMD_SEEK);
	irq_funct = format;
}

/*
void seek(unsigned int c, unsigned int s, const unsigned char* buffer, int size, unsigned char car){
	_out(HDA_DATAREG,((c>>8)&0xff));
	_out(HDA_DATAREG+1,(c&0xff));
	_out(HDA_DATAREG+2,((s>>8)&0xff));
	_out(HDA_DATAREG+3,(s&0xff));
	_out(HDA_CMDREG,CMD_SEEK);
	struct irq_funct_s new_irq_funct;
	new_irq_funct.buffer = buffer;
	new_irq_funct.size = size;
	switch (car){
	case 'w':
		new_irq_funct.f = write;
		break;
	case 'r': 
		new_irq_funct.f = read;
		break;
	case 'f':
		new_irq_funct.f = format;
		break;
	default:
		exit(EXIT_FAILURE);
	}
	irq_funct = new_irq_funct;
}*/

void consume_next() {
printf("ouais%s\n",irq_funct);
 (irq_funct)();
}

void init_disk(){
	IRQVECTOR[14]= consume_next;
	sem_init(&mutex_disk, 1);
	sem_init(&mutex_buff_cpy, 0);
}



