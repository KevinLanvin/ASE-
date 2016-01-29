#ifndef VOLUMESH
#define VOLUMESH	0x8173
#define MAX_VOL		8
#define MBR_MAGIC	0xBA0BAB
#define SUPER_MAGIC	0xB00B5
#define SUPER		0
#define SUPERNAME_MAX	30



enum vol_type_e { VOL_STD , VOL_ANX , VOL_OTHER };

/* Structures */

struct vol_s {
	unsigned int vol_cylinder;
	unsigned int vol_sector;
	unsigned int vol_size;
	enum vol_type_e vol_type;
};

struct mbr_s {
	struct vol_s mbr_vols [MAX_VOL];
	unsigned int mbr_n_vol;
	unsigned int mbr_magic;
};

struct super_s {
	unsigned super_first_free;
	unsigned super_n_free;
	unsigned super_magic;
	unsigned super_root_inumber;
	unsigned super_serial;
	char super_name [SUPERNAME_MAX];
};

struct free_block_s {
	unsigned fb_next;
	unsigned fb_size;
};



unsigned int current_volume;

/* Liste chainee de blocks */
unsigned int new_block ();
void free_block(unsigned int n);
void free_blocks(unsigned int blocks [], unsigned int n); 


/* Opérations sur les volumes */
int create_volume(unsigned int cylinder, unsigned int sector, unsigned int size);
void delete_volume(unsigned int n);
void format_volume(unsigned int n);
void init_volume(char* name, unsigned int serial);
void load_current_volume();
void display_vol();


/* MBR */
int read_mbr();
void write_mbr();
void display_mbr();
void afficher_emplacement_libre();
void sort_mbr();


/* Tools */
unsigned int cylinder_of_bloc(unsigned int vol, unsigned int bloc);
unsigned int sector_of_bloc(unsigned int vol, unsigned int bloc);
unsigned int end_cylinder_of(struct vol_s vol);
unsigned int end_sector_of(struct vol_s vol);
int is_free(struct vol_s vol);




/*Blocks */
#define read_bloc(vol,bloc,buffer)  read_block(vol,bloc,buffer)
#define write_bloc(vol,bloc,buffer)  write_block(vol,bloc,buffer)
void read_block(unsigned int vol, unsigned int bloc,unsigned char* buffer);
void read_block_n(unsigned int vol, unsigned int bloc,unsigned char* buffer, unsigned int size);
void write_block(unsigned int vol, unsigned int bloc,unsigned char* buffer);
void write_block_n(unsigned int vol, unsigned int bloc,unsigned char* buffer, unsigned int size);
void format_block(unsigned int vol);
void save_super();


/**
int agrandir_partition(unsigned int part, unsigned int taille);
void verifier_cohérence();
int déplacer_partition(unsigned int part,unsigned int vol, unsigned int bloc);
*/

#endif
