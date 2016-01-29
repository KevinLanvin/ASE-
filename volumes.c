#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "hardware.h"
#include "drive.h"
#include "volumes.h"
#include "inode.h"

static struct mbr_s mbr;
static struct super_s super;


/* Affiche l'état du volume courant : o : free ; x : occupé */
void display_vol() {
	unsigned int i;
	int t[mbr.mbr_vols[current_volume].vol_size];
	for(i=0; i<mbr.mbr_vols[current_volume].vol_size ; i++)
		t[i]=0;
	struct free_block_s fb;
	unsigned int index=super.super_first_free;
	while(index>0){
		read_block_n(current_volume,index,(unsigned char*)&fb,sizeof(fb));
		for(i=0;i<fb.fb_size;i++)
			t[i+index]=1;
		index=fb.fb_next;
	}
	printf("Volume %d\n",current_volume);
	printf("\n");
	printf("S");
	for(i=1;i<mbr.mbr_vols[current_volume].vol_size;i++)
		if(t[i]==1)
			printf("o");
		else
			printf("x");
	printf("\n");
}


/* Recupere le volume courrant */
void load_current_volume(){
	current_volume = (unsigned int)atoi(getenv("CURRENT_VOLUME"));
    	if (mbr.mbr_n_vol < current_volume)
        	exit(EXIT_FAILURE);
	
	read_block_n(current_volume,0,(unsigned char*) &super,sizeof(super));
}


/* Sauve Le superbloc */
void save_super(){
	write_block_n(current_volume,0,(unsigned char*) &super,sizeof(super));
}


/* Donne l'index d'un bloc libre pour y écrire */
unsigned int new_block (){
	struct free_block_s fb;
	unsigned int res;
	if(super.super_n_free==0) /*volume plein*/
		return 0;
	read_block_n(current_volume,super.super_first_free,(unsigned char*)&fb,sizeof(fb));
	res = super.super_first_free;
	super.super_n_free --;
	if(fb.fb_size == 1)
		super.super_first_free = fb.fb_next;
	else {
		super.super_first_free++;
		fb.fb_size --;
		write_block_n(current_volume,super.super_first_free,(unsigned char*)&fb,sizeof(fb));
	}
	save_super();
	return res;
}



/* Libere un bloc */
void free_block(unsigned int n) {
	if(n == 0)
		return;
	struct free_block_s fb;
	fb.fb_size = mbr.mbr_vols[current_volume].vol_size -1;
	fb.fb_next = super.super_first_free;
	super.super_first_free = n;
	super.super_n_free ++;
	write_block_n(current_volume,n,(unsigned char*) &fb,sizeof(fb));
}

void free_blocks(unsigned int blocks [], unsigned int n){
	unsigned int i;
	for(i=0;i<n;++i)
		free_block(blocks[i]);
}



/* Initialisation d'une partition */
void init_volume(char* name, unsigned int serial){
	struct free_block_s fb;
	super.super_serial = serial;
	strcpy(super.super_name,name);
	super.super_magic = SUPER_MAGIC;
	super.super_first_free = (unsigned int)1;
	super.super_n_free = mbr.mbr_vols[current_volume].vol_size -1;
	save_super();
	fb.fb_size = super.super_n_free;
	fb.fb_next = 0;
	write_block_n(current_volume,1,(unsigned char*)&fb,sizeof(fb));
}






/* Supression du n ieme volume du disque */
void delete_volume(unsigned int n){
    assert(n!=0);
    unsigned int i;
    for(i=n;i<mbr.mbr_n_vol-1;i=i+1){
            mbr.mbr_vols[i]=mbr.mbr_vols[i+1];           
    }
    --mbr.mbr_n_vol;
}




/* Formate une partition secteur par secteur */
void format_volume(unsigned int n){
    assert(n!=0);
    unsigned int i;
    unsigned int j;
    for(i=0;i<mbr.mbr_n_vol;i=i+1)
        if(i==n)
            for(j=0;j<mbr.mbr_vols[i].vol_size;j=j+1)
                format_sector(i,j);
}


void afficher_emplacement_libre(){
	//cylinder
	unsigned int i;
	//sector
	unsigned int j;
	//volume
	unsigned int v;
	//taille du volume
	unsigned int s;
	printf("Etat du disque : \n\n");
	for(i=0;i<MAX_CYLINDER;i++){
		printf("%d.\t",i);
        	for(j=0;j<MAX_SECTOR;j++){

        		//Pour chaque bloc on verifie si c'est le debut d'une partition
        		for(v=0;v < mbr.mbr_n_vol;v++){
				if(i==mbr.mbr_vols[v].vol_cylinder && j==mbr.mbr_vols[v].vol_sector)
                    			for(s=0;s<mbr.mbr_vols[v].vol_size;s++){
                        			printf("X");
						fflush(NULL);
                        			j=j+1;
                        			if(j==MAX_SECTOR)
                            			printf("\n");
                    			}  
            		}
	 		//master boot record
            		if(j==0&&i==0){
                		printf("M");
                		fflush(NULL);
	            	}else{
			        printf("O");
	    			fflush(NULL);
			}
        	}
        	printf("\n");
    	}
	printf("\n");
}




int create_volume(unsigned int cylinder, unsigned int sector, unsigned int size){
	/* Verifications */	
	assert(cylinder	<= MAX_CYLINDER);
	assert(sector 	<= MAX_SECTOR);
	assert(size	<= (MAX_SECTOR -1)  * MAX_CYLINDER * SECTORSIZE);
	assert(mbr.mbr_n_vol < MAX_VOL);
	assert( cylinder != 0||sector != 0);

	/* Creation du volume */
	sort_mbr();
	struct vol_s vol;
	vol.vol_cylinder = cylinder;
	vol.vol_sector = sector;
	vol.vol_size = size;
	vol.vol_type = VOL_STD;
	if(is_free(vol)==1){
		printf("OQP\n");
		return -1;
	}
	/* Ajout du volume au mbr */
	mbr.mbr_vols[mbr.mbr_n_vol] = vol;
	mbr.mbr_n_vol ++;
	write_mbr();
	unsigned char* buffer = malloc(256*sizeof(int));
	read_sector(0,0,buffer);
	free(buffer);
	return 0;
}

/* Indique si la place demandée est libre */
int is_free(struct vol_s vol){
	if(mbr.mbr_n_vol==0)
		return 0;
	if(end_cylinder_of(vol) < mbr.mbr_vols[0].vol_cylinder)
		return 0;
	if(end_cylinder_of(vol) == mbr.mbr_vols[0].vol_cylinder && end_sector_of(vol) < mbr.mbr_vols[0].vol_sector)
		return 0;
	unsigned int i;
	for(i=0;i<mbr.mbr_n_vol-1;++i){
		/* On est entre deux si 
			- on démarre après la fin du précédent
			- on finit avant le début du suivant */

		// Le cylindre est strictement entre le précédent et le suivant
		if(vol.vol_cylinder > end_cylinder_of(mbr.mbr_vols[i])
		&& end_cylinder_of(vol) < mbr.mbr_vols[i+1].vol_cylinder )
			return 0;
		// Le cylindre est egal au precedent, on teste donc aussi les secteurs
		else if(vol.vol_cylinder == end_cylinder_of(mbr.mbr_vols[i])
		&& end_cylinder_of(vol) < mbr.mbr_vols[i+1].vol_cylinder
		&& vol.vol_sector > end_sector_of(mbr.mbr_vols[i]))
			return 0;
		// Le cylindre est egal au suivant, on teste les secteurs aussi
		else if(vol.vol_cylinder > end_cylinder_of(mbr.mbr_vols[i])
		&& end_cylinder_of(vol) == mbr.mbr_vols[i+1].vol_cylinder
		&& end_sector_of(vol) < mbr.mbr_vols[i+1].vol_sector)
			return 0;
		// Le cylindre est egal au precedent ET au suivant
		// On teste les secteurs des deux cas
		else if(vol.vol_cylinder == end_cylinder_of(mbr.mbr_vols[i])
		&& end_cylinder_of(vol) == mbr.mbr_vols[i+1].vol_cylinder
		&& vol.vol_sector > end_sector_of(mbr.mbr_vols[i])
		&& end_sector_of(vol) < mbr.mbr_vols[i+1].vol_sector)
			return 0;
	}
	if(vol.vol_cylinder > end_cylinder_of(mbr.mbr_vols[mbr.mbr_n_vol-1]))
		return 0;
	if( vol.vol_cylinder == end_cylinder_of(mbr.mbr_vols[mbr.mbr_n_vol-1]) && vol.vol_sector > end_sector_of(mbr.mbr_vols[mbr.mbr_n_vol-1]))
		return 0;
	return 1;
}


void display_mbr(){
	sort_mbr();
	printf("Il y a %d partitions sur le disque.\n",mbr.mbr_n_vol);
	unsigned int o;
	for(o=0;o<mbr.mbr_n_vol;++o)	
		printf("partition %d: (%d,%d) --> (%d,%d)\n", 
			o, 
			mbr.mbr_vols[o].vol_cylinder, 
			mbr.mbr_vols[o].vol_sector, 
			end_cylinder_of(mbr.mbr_vols[o]),
			end_sector_of(mbr.mbr_vols[o]));
	printf("\n\n");
}




/* Lis dans le mbr et l'initialise si le MBR_MAGIC ne correspond pas 
	retourne 0 si le MBR n'est pas initialisé
	retourne 1 si il l'est
*/
int read_mbr(){
	read_sector_n(0,0,(unsigned char*)&mbr,sizeof(mbr));
	if(mbr.mbr_magic != MBR_MAGIC) {
		mbr.mbr_magic = MBR_MAGIC;
		mbr.mbr_n_vol = 0;
		return 0;
	}
	return 1;
}

/* Ecris le MBR */
void write_mbr(){
	write_sector_n(0,0,(unsigned char*)&mbr,sizeof(mbr));
}

/* Retourne le numero du cylindre correspondant à un bloc d'un volume donné */
unsigned int cylinder_of_bloc(unsigned int vol, unsigned int bloc){
	return (unsigned int) mbr.mbr_vols[vol].vol_cylinder + ((mbr.mbr_vols[vol].vol_sector + bloc) / MAX_SECTOR);
}


/* Retourne le numero du secteur correspondant à un bloc d'un volume donné */
unsigned int sector_of_bloc(unsigned int vol, unsigned int bloc){
	return (unsigned int)(mbr.mbr_vols[vol].vol_sector + bloc) % MAX_SECTOR;
}


/* Lis un bloc d'un volume donné */
void read_block(unsigned int vol, unsigned int bloc,unsigned char* buffer){
	assert(vol<mbr.mbr_n_vol);
	assert(bloc<mbr.mbr_vols[vol].vol_size);
	unsigned int c = cylinder_of_bloc(vol,bloc);
	unsigned int s = sector_of_bloc(vol,bloc);
	read_sector(c,s,buffer);
}

void read_block_n(unsigned int vol, unsigned int bloc,unsigned char* buffer, unsigned int size){
	assert(vol<mbr.mbr_n_vol);
	assert(bloc<mbr.mbr_vols[vol].vol_size);
	unsigned int c = cylinder_of_bloc(vol,bloc);
	unsigned int s = sector_of_bloc(vol,bloc);
	read_sector_n(c,s,buffer,size);
}


/* Ecris un bloc d'un volume donné */
void write_block(unsigned int vol, unsigned int bloc,unsigned char* buffer){
	assert(vol<mbr.mbr_n_vol);
	assert(bloc<mbr.mbr_vols[vol].vol_size);
	unsigned int c = cylinder_of_bloc(vol,bloc);
	unsigned int s = sector_of_bloc(vol,bloc);
	write_sector(c,s,buffer);
}


void write_block_n(unsigned int vol, unsigned int bloc, unsigned char* buffer, unsigned int size){
	assert(vol<mbr.mbr_n_vol);
	assert(bloc<mbr.mbr_vols[vol].vol_size);
	unsigned int c = cylinder_of_bloc(vol,bloc);
	unsigned int s = sector_of_bloc(vol,bloc);
	write_sector_n(c,s,buffer,size);
}


/* Formate un bloc d'un volume donné */
void format_block(unsigned int vol){
	assert(vol<mbr.mbr_n_vol);
	unsigned int c = cylinder_of_bloc(vol,mbr.mbr_vols[vol].vol_cylinder);
	unsigned int s = sector_of_bloc(vol,mbr.mbr_vols[vol].vol_sector);
	format_sector(c,s);
}


/* Trie les volumes dans l'ordre croissant de cylindre-secteur */
void sort_mbr(){
	if(mbr.mbr_n_vol < 2) return; 
	struct vol_s tmp;
	unsigned int i;
	int has_swapped = 1;
	while(has_swapped == 1){
		has_swapped = 0;
		for (i=0; i<mbr.mbr_n_vol-1;++i){
			if((mbr.mbr_vols[i+1].vol_cylinder < mbr.mbr_vols[i].vol_cylinder)
				 || ((mbr.mbr_vols[i+1].vol_cylinder == mbr.mbr_vols[i].vol_cylinder)
				&& (mbr.mbr_vols[i+1].vol_sector < mbr.mbr_vols[i].vol_sector))){
					tmp = mbr.mbr_vols[i];
					mbr.mbr_vols[i] = mbr.mbr_vols[i+1];
					mbr.mbr_vols[i+1] = tmp;
					has_swapped = 1;
			}
		}
	}
}



unsigned int end_cylinder_of(struct vol_s vol){
	return (unsigned int) vol.vol_cylinder + ((vol.vol_sector + vol.vol_size) / MAX_SECTOR);
}


unsigned int end_sector_of(struct vol_s vol){
	return (unsigned int) (vol.vol_sector + vol.vol_size - 1) % MAX_SECTOR;
}




